
#include "stdafx.h" 
#include "pjsip.h" 
#include "SIPCallMgr.h"
#include <mmsystem.h>
#include "SipMsgChange.h"
#include "CallMgr.h"
#include "XUtil.h"

#define THIS_FILE	"pjsip.cpp"
#define NO_LIMIT	(int)0x7FFFFFFF
#define ReceivedIPMaxLen 128

static pj_oshandle_t				sip_log_file = NULL;		//Output log file handle
static pj_caching_pool				g_CachingPool;
static pjsip_endpoint *				g_pSipEndPoint=NULL;
static pj_pool_t *					g_pSipPool=NULL;
static pj_thread_t *				sipThread=NULL;
pj_bool_t							sip_thread_quit_flag = 0;  //Thread quit flag.
static pj_thread_desc				g_sip_thread_desc ={0};
static  pj_thread_t *				g_sip_thread_t = NULL;

enum {
	SC_Deflect,
	SC_CFU,
	SC_CFNR,
	SC_DND,
	SC_3Pty,
	SC_CFB
	};

enum ETransportMode 
	{
	TM_UDP,
	TM_TCP,
	TM_TLS
	};

extern void SetDisConnectReason();
extern IMXEC_CALL_END_REASON GetCallDisconnectReason(char * szReason);
extern char * GetCallDisconnectReason(IMXEC_CALL_END_REASON nReason);

static pj_bool_t video_on_rx_request(pjsip_rx_data *rdata );
static pj_bool_t video_on_rx_response(pjsip_rx_data *rdata );
static pj_status_t video_on_tx_request(pjsip_tx_data *tdata );
static pj_status_t video_on_tx_response(pjsip_tx_data *tdata );

static pjsip_module mod_video =
	{
	NULL, NULL,						/* prev, next.	*/
		{ "mod-video", 9 },			/* Name.		*/
		-1,							/* Id			*/
		15,							/* Priority		*/ /*要小于 PJSIP_MOD_PRIORITY_TSX_LAYER	= 16,*/
		NULL,						/* load()		*/
		NULL,						/* start()		*/
		NULL,						/* stop()		*/
		NULL,						/* unload()		*/
		&video_on_rx_request,		/* on_rx_request()	*/
		&video_on_rx_response,		/* on_rx_response()	*/
		&video_on_tx_request,	    /* on_tx_request.	*/
		&video_on_tx_response,	    /* on_tx_response()	*/
		NULL,						/* on_tsx_state()	*/
	};

std::string ANSI2UTF8(const std::string&str)
{
#ifdef WIN32
	USES_CONVERSION;
	wchar_t*wszTemp=A2W(str.c_str());

	int nTempLen=WideCharToMultiByte(CP_UTF8,0,wszTemp,-1,NULL,0,NULL,NULL) ;
	char*szTemp=new char[nTempLen+1];
	WideCharToMultiByte(CP_UTF8,0,wszTemp,-1,szTemp,nTempLen,NULL,NULL) ;

	std::string strTemp=szTemp;

	delete[] szTemp;
	szTemp=NULL;

	return strTemp;
#else
	return str;
#endif
}

std::string UTF82ANSI(const std::string&str)
{
#ifdef WIN32
	int nTempLen=MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,NULL,0);
	wchar_t* wszTemp = new wchar_t[nTempLen+1];
	MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,wszTemp,nTempLen);

	USES_CONVERSION;
	std::string strTemp=W2A(wszTemp);

	delete[]wszTemp;
	wszTemp=NULL;

	return strTemp;
#else
	return str;
#endif
}

void sip_register_thread(void)
{
	if (!pj_thread_is_registered())
		pj_thread_register(NULL, g_sip_thread_desc, &g_sip_thread_t);
}

void CallStateDestroyed(void*  pCall,char * callId,pjsip_status_code cause,int reason)
{
	SIPMediaCall*  pSipCall =(SIPMediaCall * )pCall;
	if(pSipCall ==NULL) return;
	if(pSipCall->CallDestroyStatus()==false) pSipCall->CallDestroyStatus() =true;
	else return;

	if(pSipCall->m_CallDirection == SIPCall::CALL_DIR_OUT) pSipCall->m_pInvSessionUAC =NULL;
	if(pSipCall->m_CallDirection == SIPCall::CALL_DIR_IN) pSipCall->m_pInvSessionUAS =NULL;

	std::string strDisconnectReason="DisconnectedNormal";
	IMXEC_CALL_END_REASON discReason =ModeDisconnectedUnknown;
	if(reason < 0)
	{
		switch(cause)
		{
		case PJSIP_SC_BUSY_HERE:
			discReason =ModeDisconnectedBusy;
			break;
		case PJSIP_SC_NOT_ACCEPTABLE_HERE:
		case PJSIP_SC_NOT_ACCEPTABLE_ANYWHERE:
		case PJSIP_SC_NOT_ACCEPTABLE:
			discReason =ModeDisconnectedReject;
			break;
		case PJSIP_SC_NOT_FOUND:
			discReason =ModeDisconnectedNotFound;
			break;
		case PJSIP_SC_SERVICE_UNAVAILABLE:
			discReason =ModeDisconnectedServiceUnavailable;
			break;
		case PJSIP_SC_REQUEST_TIMEOUT:
			discReason =ModeDisconnectedRequestTimeout;
		case PJSIP_SC_OK:
			discReason =ModeDisconnectedLocal;
			break;
		default:
			discReason =ModeDisconnectedUnknown;
			break;
		}
	}
	else
		discReason =(IMXEC_CALL_END_REASON) reason;

	if( pSipCall->m_nLocalHungUP ==1) discReason =ModeDisconnectedLocal;  //本端主动挂断
	if( pSipCall->m_nLocalHungUP ==2) discReason =ModeDisconnectedNet;
	if( pSipCall->m_nLocalHungUP ==3) discReason =ModeDisconnectedRemote; //远端正常挂断(接收到bye)

	int st_code =cause;
	if( (st_code >402 && st_code <407) || st_code == 408 || st_code/48==10 || st_code/100==6 || st_code/100==5)
	{
		const pj_str_t * statusText=pjsip_get_status_text(cause);
		char strText[256]={0};
		strncpy(strText,statusText->ptr,statusText->slen);
		strText[255]=0;//end
		discReason = ModeDisconnectedRemote;
		strDisconnectReason=strText;
	}
	else strDisconnectReason=GetCallDisconnectReason(discReason);

	//OutputDebugString(strDisconnectReason.c_str());
	if(SIPCallMgr::Instance().DisconnectCheckRoundTripDalay(pSipCall) )
	{
		discReason =ModeDisconnectedNet;
		strDisconnectReason= GetCallDisconnectReason(discReason);
	}

	//仍为Normal则置Unknown OR 业务层排除掉
	if(discReason == ModeDisconnectedNormal)
	{
		discReason = ModeDisconnectedUnknown;
		strDisconnectReason= GetCallDisconnectReason(discReason);
	}

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	if(pSipCall) SIPCallMgr::Instance().OnDAP_CallDestroyed(callId,pSipCall->GetUserID(),pSipCall->GetUserName(),discReason,strDisconnectReason);
#else

	SIPCallMgr::Instance().OnCallDestroyed(callId,discReason);
#endif

}


/* Callback to be called to handle new incoming requests. */
static pj_bool_t video_on_rx_request( pjsip_rx_data *rdata )
	{
	pj_str_t type;	        /**< Media type. */
    pj_str_t subtype;	    /**< Media subtype. */
	int clen=0;
	pj_str_t ctype;	        /**< Media type. */
    pj_str_t csubtype;	    /**< Media subtype. */
	SIPMediaCall*  pSipCall=NULL;
	SIPUser * pSIPUser=NULL;

	char displayStr[128]={0};
	const pjsip_method pjsip_info_method = { PJSIP_OTHER_METHOD, {"INFO", 4}};
	const pjsip_method pjsip_prack_method = { PJSIP_OTHER_METHOD, {"PRACK", 5}};
	const pjsip_method pjsip_update_method ={ PJSIP_OTHER_METHOD, { "UPDATE",6}};
	const pjsip_method pjsip_notify_method ={ PJSIP_OTHER_METHOD, { "NOTIFY",6}};
	const pjsip_method pjsip_subscribe_method ={ PJSIP_OTHER_METHOD, { "SUBSCRIBE",9}};

	if(rdata==NULL)
		return PJ_FALSE;
	if (rdata->msg_info.msg==NULL)
		return PJ_FALSE;		
	if (rdata->msg_info.cid==NULL)
			return PJ_FALSE;

	sip_register_thread();

	int status=-1;
	pj_str_t callIdStr;
	callIdStr.ptr=NULL;
	callIdStr=rdata->msg_info.cid->id;
	char idStr[128]={0};
	if(callIdStr.ptr!=NULL) 
	{
		strncpy(idStr,callIdStr.ptr,callIdStr.slen);
	}

	pSipCall=NULL;
	if(callIdStr.ptr != NULL) pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);
	//收到BYE 挂断
	if (rdata->msg_info.msg->line.req.method.id == PJSIP_BYE_METHOD && callIdStr.ptr != NULL) 
	{
		IMXEC_CALL_END_REASON discReason = ModeDisconnectedNormal;
		pjsip_status_code cause =(pjsip_status_code)0; 
		if(pSipCall)
		{
			pSipCall->m_nLocalHungUP =3;
			CallStateDestroyed(pSipCall,idStr,cause,discReason);
		}
		SendResponse200OKNOSDP(rdata);
		return PJ_TRUE;
	}

	//check sdp
	if(rdata->msg_info.clen==NULL) return PJ_FALSE;
	clen=rdata->msg_info.clen->len;
	int nFindSDP=0;
	if(clen>0)
	{
		type=rdata->msg_info.ctype->media.type;
		subtype=rdata->msg_info.ctype->media.subtype;
		ctype=pj_str("application");
		csubtype=pj_str("sdp");	
		if( (pj_stricmp(&type, &ctype)==0) && (pj_stricmp(&subtype, &csubtype)==0) )
		{
			if (rdata->msg_info.msg->body)
			{
				nFindSDP=1;
			}
		}
	}

	if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_notify_method) == 0)
		return SendResponse200OKNOSDP(rdata);
	if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_subscribe_method) == 0)
		return SendResponse200OKNOSDP(rdata);

	//pSipCall=NULL;
	if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_info_method) == 0)
	{
		if(callIdStr.ptr!=NULL && rdata->msg_info.ctype) 
		{
			char mimeType[128]={0};
			char xmlBuf[1024]={0};
			int maxLen=1024-1;
			pj_str_t submessage;
			pj_str_t submessage2;
			pj_str_t submediacontrol;
			//收到INFO取xml
			type=rdata->msg_info.ctype->media.type;
			subtype=rdata->msg_info.ctype->media.subtype;
			ctype=pj_str("application");
			csubtype=pj_str("xml");                       //application/xml
			submessage=pj_str("message+text");            //application/message+text
			submessage2=pj_str("message+xml");            //application/message+xml
			submediacontrol =pj_str("media_control+xml"); //application/media_control+xml
			snprintf(mimeType, 127, "%.*s/%.*s", type.slen,type.ptr,subtype.slen,subtype.ptr);
			if (rdata->msg_info.msg->body && clen>0 )
			{
				pjsip_print_text_body(rdata->msg_info.msg->body,xmlBuf,maxLen);

				if( pj_stricmp(&type, &ctype)==0 && ( pj_stricmp(&subtype, &submessage)==0 || pj_stricmp(&subtype, &submessage2)==0 ) )
				{
					SIPCallMgr::Instance().OnDAP_RecvCallCustomMsg(idStr,xmlBuf,mimeType);
				}
				else if( (pj_stricmp(&type, &ctype)==0) && ( (pj_stricmp(&subtype, &csubtype)==0) || (pj_stricmp(&subtype, &submediacontrol)==0)) )
				{
					//get picture_fast_update
					char * pVFU = strstr(xmlBuf,"picture_fast_update");
					if(pVFU)
					{
						char * pDualMedia = strstr(xmlBuf,"dual_video");
						if(pSipCall && pDualMedia)
						{
							pSipCall->OnChannelRequestKeyFrame(2);
						}
						else if(pSipCall)
						{
							pSipCall->OnChannelRequestKeyFrame(1);
						}
					}
					else
					{
						//use to zhongxing dualvideo
						char * pDualToken=strstr(xmlBuf,"xmlns=\'xconf:video:duovideotoken\'");
						if(pDualToken ==NULL ) pDualToken=strstr(xmlBuf,"xmlns=\"xconf:video:duovideotoken\"");
						if(pDualToken)
						{
							//pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);
							char * pTokenType=strstr(xmlBuf,"<query type=\'apply\'");
							if(pTokenType ==NULL ) pTokenType=strstr(xmlBuf,"<query type=\"apply\"");
							if(pSipCall && pTokenType)
							{
								pSipCall->OnRecvSIPDualVideoTokenApply();
							}
							pTokenType=strstr(xmlBuf,"<query type=\'release\'");
							if(pTokenType ==NULL ) pTokenType=strstr(xmlBuf,"<query type=\"release\"");
							if(pSipCall && pTokenType)
							{
								pSipCall->OnRecvSIPDualVideoTokenRelease();
							}
							pTokenType=strstr(xmlBuf,"<query type=\'withdraw\'");
							if(pTokenType ==NULL ) pTokenType=strstr(xmlBuf,"<query type=\"withdraw\"");
							if(pSipCall && pTokenType)
							{
								pSipCall->OnRecvSIPDualVideoTokenWithdraw();
							}
							pTokenType=strstr(xmlBuf,"<query type=\'permit\'");
							if(pTokenType ==NULL ) pTokenType=strstr(xmlBuf,"<query type=\"permit\"");
							if(pSipCall && pTokenType)
							{
								pSipCall->OnRecvSIPDualVideoTokenPermit();
							}
						}
					}
				}
			}

		}
		//if(pSipCall) return pSipCall->SIPSend200Status(rdata);
		return SendResponse200OKNOSDP(rdata);
	}

	//get utf8 display name 
	if(rdata->msg_info.len>0)
	{
		char * bufstr=0;
		char * pn1=0;
		char * pn2=0;
		char cmdstr[128]={0};		
		bufstr = rdata->msg_info.msg_buf;
		pn1=strstr(bufstr,"From: \"");
		if(pn1==NULL) pn1=strstr(bufstr,"from: \"");
		if(pn1) pn2=strstr(pn1,"<");
		if(pn2!=NULL && pn1!=NULL)
			strncpy(cmdstr,pn1+7,pn2-pn1-8);
		int cnt=strlen(cmdstr);
		while(cnt)
		{
			if(cmdstr[cnt-1]==' ') cmdstr[cnt-1]=0;
			else if (cmdstr[cnt-1]=='\"') cmdstr[cnt-1]=0;
			else break;
			cnt--;
		}

		if(strlen(cmdstr)>0)
		{
			std::string tempstr=UTF82ANSI(cmdstr);
			strcpy(displayStr,tempstr.c_str());
		}
	}

	pSIPUser=NULL;
	pjsip_uri * toURI = rdata->msg_info.to->uri;
	pSIPUser=SIPCallMgr::Instance().GetSIPUserFromSipUri(toURI,rdata->tp_info.transport);
	//create call
	pSipCall=NULL;
	if(callIdStr.ptr!=NULL)
	{
		if (rdata->msg_info.msg->line.req.method.id == PJSIP_INVITE_METHOD) 
		{
			if (CallMgr::Instance().IsBlackListIP(rdata->pkt_info.src_name))
			{
				return PJ_FALSE;
			}
			//int accountID =0;
			pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);
			//call self control
			//if( (pSipCall) && (pSipCall->GetCallDirection()==SIPCall::CALL_DIR_OUT) ) pSipCall=NULL;
			if(pSipCall==NULL)
			{
				char userId[64]={0};
				char sipUriHost[64]={0};
				pjsip_uri *uri=NULL;
				pjsip_sip_uri *sip_uri=NULL;
				uri = rdata->msg_info.from->uri;
				//pjsip_uri * toURI = rdata->msg_info.to->uri;
				sip_uri = (pjsip_sip_uri*)pjsip_uri_get_uri(uri);
				if(sip_uri!=NULL)
				{
					strncpy(userId,sip_uri->user.ptr,((sip_uri->user.slen < 64) ? sip_uri->user.slen : 63 ));
					if(sip_uri->host.slen>0) strncpy(sipUriHost,sip_uri->host.ptr,((sip_uri->host.slen <64) ? sip_uri->host.slen : 63) );
					if( strlen(sipUriHost) <=0 ) strcpy(sipUriHost,rdata->pkt_info.src_name);
				}
				//SIPUser * pSIPUser=SIPCallMgr::Instance().GetSIPUserFromSipUri(toURI,rdata->tp_info.transport);

//限制同时只能有一个call在呼叫
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else

				if(CallMgr::Instance().IsCanCall() ==false)
				{
					return SendResponse200OKNOSDP(rdata ,PJSIP_SC_BUSY_HERE);
				}

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) 
#else  
				CallMgr::Instance().SetCanCallNot(); 
#endif 

#endif

				pSipCall=SIPCallMgr::Instance().CreateSIPMediaCall(pSIPUser,idStr,sipUriHost);
				if(pSipCall && sip_uri->user.slen == 0) 
				{
					pSipCall->SetUserID(sipUriHost);
					if(strlen(displayStr)==0) strcpy(displayStr,sipUriHost);
					pSipCall->SetUserName(displayStr);
				}
				if(pSipCall && sip_uri->user.slen>0)
				{
					pSipCall->SetUserID(userId);
					if(strlen(displayStr)==0) strcpy(displayStr,userId);
					pSipCall->SetUserName(displayStr);
				}
				//if(pSipCall) pSipCall->SetCallNum(1);
			}
		}
		else 
			pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);

	}

	//解析SDP能力集	
	if(nFindSDP>0)
	{
		//Parse SDP from incoming request
		pjmedia_sdp_session *sdp;
		pj_pool_t * pool=NULL;
		pjmedia_sdp_session *remoteSdp = NULL;
		pool = pj_pool_create(&g_CachingPool.factory, "SIPEndPoint", 2*1024, 1024,NULL);
		status = pjmedia_sdp_parse(rdata->tp_info.pool,(char*)rdata->msg_info.msg->body->data,
			rdata->msg_info.msg->body->len, &sdp);	
		if (status != PJ_SUCCESS)  
		{
			return PJ_FALSE;
		}
		if( pSipCall!=NULL ) 
		{
			remoteSdp = pjmedia_sdp_session_clone(pool,sdp);
			if(remoteSdp!=NULL)
			{
				pSipCall->SetCallDirection(SIPCall::CALL_DIR_IN);
				pSipCall->ParseRemoteSDP(remoteSdp);
			}
		}	
		pj_pool_release(pool);
	}

	if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_prack_method) == 0)
	{
		status=SendResponse200OKNOSDP(rdata);
		//use to zhongxing sip mcu
		//callback
		if(nFindSDP > 0)
		{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
			SIPCallMgr::Instance().OnDAP_CallRingIn(idStr,idStr,idStr,"","",pSIPUser->m_strUserID);
#else
			SIPCallMgr::Instance().OnCallRingIn(idStr);
#endif

		}
		return status;
	}

	//recv sip msg process
	if(pSipCall && rdata->msg_info.len>0)
	{
		if (rdata->msg_info.msg->line.req.method.id == PJSIP_INVITE_METHOD) 
		{
			pSipCall->OnRecvInviteRequest(rdata,nFindSDP);
			SIPCallMgr::Instance().GetTerminalInfo(pSipCall,rdata);
		}
		if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,&pjsip_update_method) == 0)
		{
			pSipCall->OnRecvUpdateRequest(rdata,nFindSDP);
			return PJ_TRUE;
		}
	}

	return PJ_FALSE;
}

static pj_bool_t video_on_rx_response( pjsip_rx_data *rdata )
	{
	pj_str_t type;	        /**< Media type. */
    pj_str_t subtype;	    /**< Media subtype. */
	int clen=0;
	pj_str_t ctype;	        /**< Media type. */
    pj_str_t csubtype;	    /**< Media subtype. */
	pjsip_cseq_hdr		*cseq=NULL;
	pjsip_contact_hdr *contact=NULL;
	char * addrStrPtr=NULL;
	int addrStrLen=0;
	char strReceivedIP[ReceivedIPMaxLen]={0};
	int st_code=0;
	char * substr1=NULL;
	char * substr2=NULL;
	char userIdStr[64]={0};
	int acc_id=-1;
	const pjsip_method pjsip_update_method ={ PJSIP_OTHER_METHOD, { "UPDATE",6}};
	const pjsip_method pjsip_info_method = { PJSIP_OTHER_METHOD, {"INFO", 4}};

	if(rdata==NULL)
		return PJ_FALSE;
	if (rdata->msg_info.msg==NULL)
		return PJ_FALSE;
	cseq=rdata->msg_info.cseq;
	st_code=rdata->msg_info.msg->line.status.code;
	if(rdata->msg_info.cid==NULL) return PJ_FALSE;

	sip_register_thread();

	if((st_code==200 || st_code/40==10 )&&(cseq->method.id==PJSIP_REGISTER_METHOD) )
	{
		SIPCallMgr::Instance().GetServerInfo(rdata);
	}

	int status=-1;
	pj_str_t callIdStr;
	callIdStr.ptr=NULL;
	callIdStr=rdata->msg_info.cid->id;
	char idStr[128]={0};
	if(callIdStr.ptr!=NULL) 
	{
		strncpy(idStr,callIdStr.ptr,callIdStr.slen);
	}
	SIPMediaCall*  pSipCall=NULL;
	if(callIdStr.ptr!=NULL)
	{
		pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);
	}

	if((st_code==200 )&&(cseq->method.id==PJSIP_CANCEL_METHOD) && callIdStr.ptr!=NULL)
	{
		IMXEC_CALL_END_REASON discReason =ModeDisconnectedLocal;
		pjsip_status_code cause =(pjsip_status_code)0; 
		if(pSipCall) CallStateDestroyed(pSipCall,idStr,cause,discReason);
		//return PJ_TRUE;
	}

	if( ( (st_code >402 && st_code <407) || st_code == 408 || st_code/48==10 || st_code/100==6 || st_code/100==5) && callIdStr.ptr!=NULL)
	{
		pjsip_status_code cause =PJSIP_SC_BUSY_HERE;
		if(pSipCall)
		{
			pSipCall->SIPSendACKRequest(rdata,2,st_code);//1 request 2 response
			CallStateDestroyed(pSipCall,idStr,cause,-1);
			return PJ_TRUE;
		}
		else return PJ_FALSE;
	}

	//check sdp
	if(rdata->msg_info.clen==NULL) return PJ_FALSE;
	clen=rdata->msg_info.clen->len;
	int nFindSDP=0;
	if(clen>0)
	{
		type=rdata->msg_info.ctype->media.type;
		subtype=rdata->msg_info.ctype->media.subtype;
		ctype=pj_str("application");
		csubtype=pj_str("sdp");	
		if( (pj_stricmp(&type, &ctype)==0) && (pj_stricmp(&subtype, &csubtype)==0) )
		{
			if (rdata->msg_info.msg->body)
			{
				nFindSDP=1;
			}
		}
	}

	const pjsip_method pjsip_prack_method = { PJSIP_OTHER_METHOD, {"PRACK", 5}};
	if((st_code==200 ) && (pjsip_method_cmp(&cseq->method,&pjsip_prack_method) == 0) )
	{

	}

	if(nFindSDP>0)
	{
		pjmedia_sdp_session *sdp;
		//Parse SDP from incoming response
		if (rdata->msg_info.msg->body)
		{
			pj_pool_t * pool=NULL;
			pjmedia_sdp_session *remoteSdp = NULL;
			pool = pj_pool_create(&g_CachingPool.factory, "SIPEndPoint", 2*1024, 1024,NULL);
			status = pjmedia_sdp_parse(rdata->tp_info.pool,(char*)rdata->msg_info.msg->body->data,
				rdata->msg_info.msg->body->len, &sdp);
			if (status != PJ_SUCCESS)  
			{
				return PJ_FALSE;
			}
			if(pSipCall!=NULL) 
			{
				if( (st_code==200 ) || (st_code==183 )  || (st_code==180 ) )  //再次收到200 OK SDP会重新打开音视频库
				{
					//解析SDP能力集
					remoteSdp = pjmedia_sdp_session_clone(pool,sdp);
					if(remoteSdp!=NULL) pSipCall->ParseRemoteSDP(remoteSdp);
					//cap neg
					pSipCall->SDP_Negotiate();
					//media update
					pSipCall->MediaOpenSetState();
					//test
					if(pjsip_method_cmp(&cseq->method,&pjsip_update_method) == 0)
						pSipCall->Send200ACKNoSDP();

					//if(st_code==183 ) pSipCall->SIPSendPRACKRequest(NULL);
					if(st_code==200) SIPCallMgr::Instance().GetTerminalInfo(pSipCall,rdata);
				}
			}

			pj_pool_release(pool);
		}
	}

	if( st_code==200 && nFindSDP<=0 && (cseq->method.id==PJSIP_INVITE_METHOD) )
	{
		if(pSipCall!=NULL)
		{
			pSipCall->SetSDPNegStateDone();
		}
	}

	if( ( st_code==200 ) && (pjsip_method_cmp(&cseq->method,&pjsip_info_method) == 0) )
	{
	}

	return PJ_FALSE;
	}

	static pj_status_t video_on_tx_request(pjsip_tx_data *tdata)
	{
		sip_register_thread();
		pjsip_method_e id=tdata->msg->line.req.method.id;

		int nLocalPort=ntohs(tdata->tp_info.transport->local_addr.ipv4.sin_port);

		//test
		//char msg[128]={0};
		//sprintf(msg,"on_tx_request local port :%d \n",nLocalPort);
		//OutputDebugString(msg);

		//test
		const pjsip_method pjsip_info_method = { PJSIP_OTHER_METHOD, {"INFO", 4}};
		if (pjsip_method_cmp(&tdata->msg->line.req.method,&pjsip_info_method) == 0)
		{
			pj_str_t callIdStr;
			callIdStr.ptr=NULL;
			pjsip_cid_hdr *cidhdr;
			cidhdr = PJSIP_MSG_CID_HDR(tdata->msg);//pjsip_msg_find_hdr_by_name
			if(cidhdr)
			{
				callIdStr=cidhdr->id;
			}
			char idStr[128]={0};
			if(callIdStr.ptr!=NULL) 
			{
				strncpy(idStr,callIdStr.ptr,callIdStr.slen);
			}
			SIPMediaCall*  pSipCall=NULL;
			if(callIdStr.ptr!=NULL)
			{
				//pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(idStr);
			}
		}

		if(id == PJSIP_REGISTER_METHOD)
		{
			pjsip_via_hdr * via=(pjsip_via_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_VIA,NULL);
			pjsip_from_hdr * from=(pjsip_from_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_FROM,NULL);
			if(via && from)
			{
				SIPUser * pSipUser=NULL;
				pSipUser=SIPCallMgr::Instance().GetSIPUserFromSipUri(from->uri,tdata->tp_info.transport);
				if(pSipUser)
				{
					pSipUser ->SetLocalBindPort(nLocalPort);
					pSipUser->m_nSIPCheckLastTimestamp =XGetTimestamp();
					pSipUser->m_bSendReg=true;
					pSipUser->SipHdrContactParamLocalAddFirst(tdata->msg,pSipUser->m_pPOOL,nLocalPort);
					//notice tcp timeout ,not use 
					//if(SIPCallMgr::Instance().m_nSIPServerType <= 0)
					//pSipUser->SipHdrViaParamLocalAddFirst(tdata->msg,pSipUser->m_pPOOL);
				}
			}
		}

		if(id != PJSIP_REGISTER_METHOD)
		{
			pjsip_via_hdr * via=(pjsip_via_hdr*)
				pjsip_msg_find_hdr(tdata->msg, PJSIP_H_VIA,NULL);
			pjsip_from_hdr * from=(pjsip_from_hdr*)
				pjsip_msg_find_hdr(tdata->msg, PJSIP_H_FROM,NULL);
			//set via add do:
			if(SIPCallMgr::Instance().m_nProxySendRtp >0 && via && from)
			{
				SIPUser * pSipUser=NULL;
				pSipUser=SIPCallMgr::Instance().GetSIPUserFromSipUri(from->uri,tdata->tp_info.transport);
				if(pSipUser) //set via mapaddr
				{
					if(0)
					{
					std::string strIP=pSipUser->GetHostAddress();
					pj_str_t localIP=pj_str((char *)strIP.c_str());
					int rt=pj_strcmp(&(via->sent_by.host),&localIP);
					if( (pSipUser->m_bHasVIA) && (rt==0) )
					{
						via->sent_by.host=pj_str((char *)pSipUser->m_strViaSendByAddress.c_str());
						via->sent_by.port=pSipUser->m_nViaRPort;
						//pSipUser->GenerateVIABranchParam(via,pSipUser->m_pPOOL);//if the add,not call success
					}
					else
						pSipUser->SipHdrViaParamLocalAddFirst(tdata->msg,pSipUser->m_pPOOL,nLocalPort);
					}
					else
					{
					//notice set one local via , use call phone
					pSipUser->SipHdrViaParamLocalAddFirst(tdata->msg,pSipUser->m_pPOOL,nLocalPort);
					pSipUser->SipHdrContactParamLocalAddFirst(tdata->msg,pSipUser->m_pPOOL,nLocalPort);
					}
				}
			}
		}

		//pjsip_msg_print( tdata->msg, tdata->buf.start,tdata->buf.end - tdata->buf.start);
		return PJ_FALSE;
	}

static pj_status_t video_on_tx_response(pjsip_tx_data *tdata)
{
	sip_register_thread();

	int st_code=0;
	pj_str_t st_text;	
	st_code=tdata->msg->line.status.code;
	if(st_code>=400) {
		char st_info[128]={0};
		st_text = *pjsip_get_status_text(st_code);
		if(st_code==488) 
			sprintf(st_info,"system:%d %.*s%s",st_code,st_text.slen,st_text.ptr,"(No Matching Codec or Encryption Algo)");
		else
			sprintf(st_info,"system:%d %.*s",st_code,st_text.slen,st_text.ptr);
		SIPCallMgr::Instance().StackDisplayMsg(st_info,1);
		return PJ_FALSE;
	}

	return PJ_FALSE;
}

enum {
	AVAILABLE, BUSY, OTP, IDLE, AWAY, BRB, OFFLINE, OPT_MAX
	};

struct presence_status {
	int id;
	char *name;
	} opts[] = {
		{ AVAILABLE, "Available" },
		{ BUSY, "Busy"},
		{ OTP, "On the phone"},
		{ IDLE, "Idle"},
		{ AWAY, "Away"},
		{ BRB, "Be right back"},
		{ OFFLINE, "Offline"}
	};

	struct call_data
		{
		pj_timer_entry	    timer;
		pj_bool_t		    ringback_on;
		pj_bool_t		    ring_on;
		};


	/* Pjsua application data */
	static struct app_config
		{
		pjsua_config	    cfg;
		pjsua_logging_config    log_cfg;
		pjsua_media_config	    media_cfg;
		pj_bool_t		    no_refersub;
		pj_bool_t		    no_tcp;
		pj_bool_t		    no_udp;
		pj_bool_t		    use_tls;
		pjsua_transport_config  udp_cfg;
		pjsua_transport_config  rtp_cfg;

		unsigned		    acc_cnt;
		pjsua_acc_config	    acc_cfg[PJSUA_MAX_ACC];

		unsigned		    buddy_cnt;
		pjsua_buddy_config	    buddy_cfg[PJSUA_MAX_BUDDIES];

		struct call_data	    call_data[PJSUA_MAX_CALLS];

		pj_pool_t		   *pool;

		unsigned		    codec_cnt;
		pj_str_t		    codec_arg[32];
		unsigned		    codec_dis_cnt;
		pj_str_t                codec_dis[32];
		pj_bool_t		    null_audio;
		unsigned		    wav_count;
		pj_str_t		    wav_files[32];
		unsigned		    tone_count;
		pjmedia_tone_desc	    tones[32];
		pjsua_conf_port_id	    tone_slots[32];
		pjsua_player_id	    wav_id;
		pjsua_conf_port_id	    wav_port;
		pj_bool_t		    auto_play;
		pj_bool_t		    auto_play_hangup;
		pj_timer_entry	    auto_hangup_timer;
		pj_bool_t		    auto_loop;
		pj_bool_t		    auto_conf;
		pj_str_t		    rec_file;
		pj_bool_t		    auto_rec;
		pjsua_recorder_id	    rec_id;
		pjsua_conf_port_id	    rec_port;
		unsigned		    auto_answer;
		unsigned		    duration;

#ifdef STEREO_DEMO
		pjmedia_snd_port	   *snd;
#endif

		float		    mic_level,
			speaker_level;

		int			    capture_dev, playback_dev;
		unsigned		    capture_lat, playback_lat;

		pj_bool_t		    no_tones;
		int			    ringback_slot;
		int			    ringback_cnt;
		pjmedia_port	   *ringback_port;
		int			    ring_slot;
		int			    ring_cnt;
		pjmedia_port	   *ring_port;

		} app_config;

	static SipConfigStruct sipek_config;
	static bool sipekConfigEnabled = false;

	static pjsua_call_id	current_call = PJSUA_INVALID_ID;
	static pj_bool_t	cmd_echo;
	static int		stdout_refresh = -1;
	static const char      *stdout_refresh_text = "STDOUT_REFRESH";
	static pj_bool_t	stdout_refresh_quit = PJ_FALSE;
	static pj_str_t		uri_arg;

	static char some_buf[1024 * 3];

#ifdef STEREO_DEMO
	static void stereo_demo();
#endif
	pj_status_t app_destroy(void);

	void setSipConfig(SipConfigStruct* config)
	{
		sipekConfigEnabled = true;
		sipek_config = *config; 
	}
	//////////////////////////////////////////////////////////////////////////

	/* Set default config. */
	void sip_default_config(struct app_config *cfg)
	{
		pjsua_transport_config_default(&cfg->udp_cfg);
		pjsua_transport_config_default(&cfg->rtp_cfg);
		cfg->rtp_cfg.port = 4000;
		cfg->duration = NO_LIMIT;
		cfg->wav_id = PJSUA_INVALID_ID;
		cfg->rec_id = PJSUA_INVALID_ID;
		cfg->wav_port = PJSUA_INVALID_ID;
		cfg->rec_port = PJSUA_INVALID_ID;
		cfg->mic_level = cfg->speaker_level = 1.0;
		cfg->capture_dev = PJSUA_INVALID_ID;
		cfg->playback_dev = PJSUA_INVALID_ID;
		cfg->capture_lat = PJMEDIA_SND_DEFAULT_REC_LATENCY;
		cfg->playback_lat = PJMEDIA_SND_DEFAULT_PLAY_LATENCY;
		cfg->ringback_slot = PJSUA_INVALID_ID;
		cfg->ring_slot = PJSUA_INVALID_ID;
	}

	static void default_config(struct app_config *cfg)
	{
		sip_default_config(cfg);
	}

	static void init_random_seed(void)
	{
		pj_sockaddr addr;
		const pj_str_t *hostname;
		pj_uint32_t pid;
		pj_time_val t;
		unsigned seed=0;

		/* Add hostname */
		hostname = pj_gethostname();
		seed = pj_hash_calc(seed, hostname->ptr, (int)hostname->slen);

		/* Add primary IP address */
		if (pj_gethostip(pj_AF_INET(), &addr)==PJ_SUCCESS)
			seed = pj_hash_calc(seed, &addr.ipv4.sin_addr, 4);

		/* Get timeofday */
		pj_gettimeofday(&t);
		seed = pj_hash_calc(seed, &t, sizeof(t));

		/* Add PID */
		pid = pj_getpid();
		seed = pj_hash_calc(seed, &pid, sizeof(pid));

		/* Init random seed */
		pj_srand(seed);
	}

	static pj_bool_t on_rx_msg(pjsip_rx_data *rdata)
	{
		if (SIPCallMgr::Instance().m_nLogModuleEnabled) {
			PJ_LOG(4,(THIS_FILE, "RX %d bytes %s from %s:%s:%d:\n"
				"%.*s\n"
				"--end msg--",
				rdata->msg_info.len,
				pjsip_rx_data_get_info(rdata),
				rdata->tp_info.transport->type_name,
				rdata->pkt_info.src_name,
				rdata->pkt_info.src_port,
				rdata->msg_info.len,
				rdata->msg_info.msg_buf));
		}

		return PJ_FALSE;
	}

	static pj_status_t on_tx_msg(pjsip_tx_data *tdata)
	{
		if (SIPCallMgr::Instance().m_nLogModuleEnabled) {
			PJ_LOG(4,(THIS_FILE, "TX %d bytes %s to %s:%s:%d:\n"
				"%.*s\n"
				"--end msg--",
				(tdata->buf.cur - tdata->buf.start),
				pjsip_tx_data_get_info(tdata),
				tdata->tp_info.transport->type_name,
				tdata->tp_info.dst_name,
				tdata->tp_info.dst_port,
				(tdata->buf.cur - tdata->buf.start),
				tdata->buf.start));
		}
		return PJ_SUCCESS;
	}

	/* Message logger module. */
	static pjsip_module mod_msg_logger = 
	{
		NULL, NULL,				/* prev and next	*/
		{ "mod-msg-logger", 14},		/* Name.		*/
		-1,					/* Id			*/
		PJSIP_MOD_PRIORITY_TRANSPORT_LAYER-1,/* Priority		*/
		NULL,				/* load()		*/
		NULL,				/* start()		*/
		NULL,				/* stop()		*/
		NULL,				/* unload()		*/
		&on_rx_msg,				/* on_rx_request()	*/
		&on_rx_msg,				/* on_rx_response()	*/
		&on_tx_msg,				/* on_tx_request()	*/
		&on_tx_msg,				/* on_tx_response()	*/
		NULL,				/* on_tsx_state()	*/
	};

	int init_msg_logger(void)
	{
		pj_status_t status;
		if (mod_msg_logger.id != -1)
			return 0;
		status = pjsip_endpt_register_module(g_pSipEndPoint, &mod_msg_logger);
		if (status != PJ_SUCCESS) {
			return -10;
		}
		return 0;
	}

	int msg_logger_set_enabled(pj_bool_t enabled)
	{
		int val = SIPCallMgr::Instance().m_nLogModuleEnabled;
		SIPCallMgr::Instance().m_nLogModuleEnabled = enabled;
		return val;
	}


// Log callback
static void sip_log_writer(int level, const char *buffer, int len)
{
    /* Write to file, stdout or application callback. */

    if (sip_log_file) {
	pj_ssize_t size = len;
	pj_file_write(sip_log_file, buffer, &size);
	/* This will slow things down considerably! Don't do it!
	 pj_file_flush(sip_log_file);
	*/
    }

    if (level <= (int)app_config.log_cfg.console_level) 
	{
	    pj_log_write(level, buffer, len);
    }
}

pj_status_t sip_configure_logging(const pjsua_logging_config *cfg)
{
    pj_status_t status;

    /* Redirect log function to ours */
    pj_log_set_log_func( &sip_log_writer );

    /* Set decor */
    pj_log_set_decor(cfg->decor);

    /* Set log level */
    pj_log_set_level(cfg->level);

    /* Close existing file, if any */
    if (sip_log_file) {
	pj_file_close(sip_log_file);
	sip_log_file = NULL;
    }

    /* If output log file is desired, create the file: */
    if (cfg->log_filename.slen) {
	unsigned flags = PJ_O_WRONLY;
	flags |= cfg->log_file_flags;
	status = pj_file_open(g_pSipPool, 
			      cfg->log_filename.ptr,
			      flags, 
			      &sip_log_file);

	if (status != PJ_SUCCESS)
	{
	    return status;
	}
    }

    return PJ_SUCCESS;
}

void init_data()
{
	g_pSipEndPoint=NULL;
	g_pSipPool=NULL;
	SIPCallMgr::Instance().m_nLogModuleEnabled=0;
	sip_log_file=NULL;
	sip_thread_quit_flag = 0;  //Thread quit flag
	sipThread=NULL;
}

int sip_pjsip_handle_events(unsigned msec_timeout)
{
	unsigned count = 0;
	pj_time_val tv;
	pj_status_t status;

	tv.sec = 0;
	tv.msec = msec_timeout;
	pj_time_val_normalize(&tv);

	status = pjsip_endpt_handle_events2(g_pSipEndPoint, &tv, &count);

	if (status != PJ_SUCCESS)
		return -status;

	return count;
}

/* Worker thread function. */
static int sip_worker_thread(void *arg)
{
	enum { TIMEOUT = 10 };
	sip_register_thread();
	while (!sip_thread_quit_flag)
	{
		int count;
		count = sip_pjsip_handle_events(TIMEOUT);
		if (count < 0)
			pj_thread_sleep(TIMEOUT);
	}
	return 0;
}

/* Sleep with polling */
void sip_busy_sleep(unsigned msec)
{
	pj_time_val timeout, now;
	pj_gettimeofday(&timeout);
	timeout.msec += msec;
	pj_time_val_normalize(&timeout);

	do {
		int i;
		i = msec / 10;
		while (sip_pjsip_handle_events(10) > 0 && i > 0)
			--i;
		pj_gettimeofday(&now);
	} while (PJ_TIME_VAL_LT(now, timeout));
}

/**************** INVITE SESSION CALLBACKS ******************/
static void on_sip_rx_offer(pjsip_inv_session *inv,const pjmedia_sdp_session *offer)
{

}


static void on_sip_create_offer(pjsip_inv_session *inv,
							pjmedia_sdp_session **p_offer)
{

}

static void on_sip_media_update(pjsip_inv_session *inv_ses, 
							pj_status_t status)
{

}

static void on_sip_state_changed(pjsip_inv_session *inv, pjsip_event *e)
{
	char callId[128]={0};
	pjsip_inv_state state=inv->state;
	strncpy(callId,inv->dlg->call_id->id.ptr,inv->dlg->call_id->id.slen);
	SIPMediaCall*  pSipCall=NULL;
	pSipCall=SIPCallMgr::Instance().GetSIPMediaCallFromCallID(callId);
	//if(pSipCall!=NULL) pSipCall->SetPJSIPCallID(call_id);
	pjsip_status_code cause =inv->cause;

	switch(state)
	{
	case PJSIP_INV_STATE_CALLING://After INVITE is sent
		break;
	case PJSIP_INV_STATE_CONNECTING://After 2xx is sent/received
		//status = pjmedia_sdp_neg_get_active_remote(call->inv->neg, &remote_sdp);
		//if (status != PJ_SUCCESS) break;
		break;
	case PJSIP_INV_STATE_INCOMING://After INVITE is received
		//status = pjmedia_sdp_neg_get_active_remote(call->inv->neg, &remote_sdp);
		//if (status != PJ_SUCCESS) break;
		//if(video_get_remote_sdp(remote_sdp)) { }
		break;
	case PJSIP_INV_STATE_CONFIRMED://After ACK is sent/received
		if(pSipCall==NULL) break;
		if(!pSipCall->IsCallEstablished()) 
		{
			//g_SIPCallMgr.OnCallSDPNegotiate(pSipCall); //限制接通后再次收到200 OK SDP不再重新打开音视频库
			//SIPCallMgr::Instance().OnCallEstablished(callId);
			pSipCall->OnCallEstablished();
			//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
			SIPCallMgr::Instance().OnDAP_CallEstablished(callId,pSipCall->GetUserID(),pSipCall->GetUserName(),"");
#else
			SIPCallMgr::Instance().OnCallEstablished(callId);
#endif

			SIPCallMgr::Instance().OpenSIPMediaChannel(pSipCall);//呼通后再打开通道

		}
		break;
	case PJSIP_INV_STATE_DISCONNECTED://Session is terminated					
		//此不能确定是对端挂断的
		//用于超时等情况下的状态还原
		if(pSipCall) CallStateDestroyed(pSipCall,callId,cause,-1);
		break;
	default:
		break;
	}
}

static void on_sip_new_session(pjsip_inv_session *inv, pjsip_event *e)
{
}

static pjsip_dialog* on_dlg_forked(pjsip_dialog *first_set, pjsip_rx_data *res)
{
	return NULL;
}

pj_pool_t * GetSipPool()
{
	return g_pSipPool;
}
pj_caching_pool * GetSipCachingPool()
{
	return &g_CachingPool;
}
pjsip_endpoint * GetSipEndpoint()
{
	return g_pSipEndPoint;
}

//Notification on incoming request
static pj_bool_t options_on_rx_request(pjsip_rx_data *rdata)
{
	pjsip_tx_data *tdata;
	pjsip_response_addr res_addr;
	const pjsip_hdr *cap_hdr;
	pj_status_t status;

	//Only want to handle OPTIONS requests
	if (pjsip_method_cmp(&rdata->msg_info.msg->line.req.method,
		pjsip_get_options_method()) != 0)
	{
		return PJ_FALSE;
	}

	//Don't want to handle if shutdown is in progress
	if (sip_thread_quit_flag) {
		pjsip_endpt_respond_stateless(g_pSipEndPoint, rdata,PJSIP_SC_TEMPORARILY_UNAVAILABLE, NULL,NULL, NULL);
		return PJ_TRUE;
	}

	//Create basic response.
	status = pjsip_endpt_create_response(g_pSipEndPoint, rdata, 200, NULL,&tdata);
	if (status != PJ_SUCCESS) {
		return PJ_TRUE;
	}

	//Add Allow header
	cap_hdr = pjsip_endpt_get_capability(g_pSipEndPoint, PJSIP_H_ALLOW, NULL);
	if (cap_hdr) {
		pjsip_msg_add_hdr(tdata->msg,(pjsip_hdr*) pjsip_hdr_clone(tdata->pool, cap_hdr));
	}

	//Add Accept header
	cap_hdr = pjsip_endpt_get_capability(g_pSipEndPoint, PJSIP_H_ACCEPT, NULL);
	if (cap_hdr) {
		pjsip_msg_add_hdr(tdata->msg,(pjsip_hdr*) pjsip_hdr_clone(tdata->pool, cap_hdr));
	}

	//Add Supported header
	cap_hdr = pjsip_endpt_get_capability(g_pSipEndPoint, PJSIP_H_SUPPORTED, NULL);
	if (cap_hdr) {
		pjsip_msg_add_hdr(tdata->msg,(pjsip_hdr*) pjsip_hdr_clone(tdata->pool, cap_hdr));
	}

	//Add Allow-Events header from the evsub module
	cap_hdr = pjsip_evsub_get_allow_events_hdr(NULL);
	if (cap_hdr) {
		pjsip_msg_add_hdr(tdata->msg,(pjsip_hdr*) pjsip_hdr_clone(tdata->pool, cap_hdr));
	}

	//Add User-Agent header
	if(strlen(SIPCallMgr::Instance().m_strUserAgentName) > 0)
	{
		const pj_str_t USER_AGENT = { "User-Agent", 10};
		pjsip_hdr *h;
		pj_str_t agent=pj_str(SIPCallMgr::Instance().m_strUserAgentName);
		h = (pjsip_hdr*) pjsip_generic_string_hdr_create(tdata->pool,&USER_AGENT,&agent);
		pjsip_msg_add_hdr(tdata->msg, h);
	}

	//Send response
	pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
	status = pjsip_endpt_send_response(g_pSipEndPoint, &res_addr, tdata, NULL, NULL);
	if (status != PJ_SUCCESS) pjsip_tx_data_dec_ref(tdata);

	return PJ_TRUE;
}


/* The module instance. */
static pjsip_module pjsip_options_handler = 
{
	NULL, NULL,				/* prev, next.		*/
	{ "mod-pjsua-options", 17 },	/* Name.		*/
	-1,					/* Id			*/
	PJSIP_MOD_PRIORITY_APPLICATION,	/* Priority	        */
	NULL,				/* load()		*/
	NULL,				/* start()		*/
	NULL,				/* stop()		*/
	NULL,				/* unload()		*/
	&options_on_rx_request,		/* on_rx_request()	*/
	NULL,				/* on_rx_response()	*/
	NULL,				/* on_tx_request.	*/
	NULL,				/* on_tx_response()	*/
	NULL,				/* on_tsx_state()	*/

};

//reference pjsua_logging_config_default
void pjsip_logging_config_default(pjsua_logging_config *cfg)
{
	pj_bzero(cfg, sizeof(*cfg));

	cfg->msg_logging = PJ_TRUE;
	cfg->level = 5;
	cfg->console_level = 4;
	cfg->decor = PJ_LOG_HAS_SENDER | PJ_LOG_HAS_TIME | 
		PJ_LOG_HAS_MICRO_SEC | PJ_LOG_HAS_NEWLINE |
		PJ_LOG_HAS_SPACE | PJ_LOG_HAS_THREAD_SWC |
		PJ_LOG_HAS_INDENT;
#if defined(PJ_WIN32) && PJ_WIN32 != 0
	cfg->decor |= PJ_LOG_HAS_COLOR;
#endif
}

//set intercept sip msg process handle callback
pj_ssize_t __stdcall SIP_MSG_Change(char * msg,pj_ssize_t msgLen,pjsip_msg *sipStruct)
{
	return ChangeSIPMSG( msg, msgLen,sipStruct);
}

int SipInit(void)
{
	pj_status_t status;
	pjsua_transport_config pjsua_tcp_cfg;

	//Init pjsua data
	init_data();

	srand(timeGetTime());
	unsigned randNum=(unsigned) rand();

	//Initialize default config
	sip_default_config(&app_config);
	//Set default logging settings
	pjsip_logging_config_default(&app_config.log_cfg);
	if(sipek_config.logLevel>0)
	{
		app_config.log_cfg.log_filename = pj_str(sipek_config.logFileName);
		app_config.log_cfg.level=sipek_config.logLevel;
	}

	//Initialize logging first so that info/errors can be captured 
	status = sip_configure_logging( &app_config.log_cfg);
	if (status != PJ_SUCCESS) SipDestroy();

	//Init PJLIB: 
	status = pj_init();
	if (status != PJ_SUCCESS) {return status;}

	pj_log_push_indent();

	//Init random seed
	//notice update
	init_random_seed();

	//Init PJLIB-UTIL: 
	status = pjlib_util_init();
	if (status != PJ_SUCCESS) { SipDestroy(); return status;}

	//Init PJNATH 
	status = pjnath_init();
	if (status != PJ_SUCCESS) { SipDestroy(); return status;}
	//Init caching pool. 
	pj_caching_pool_init( &g_CachingPool, NULL, 0);
	if (status != PJ_SUCCESS) {
		pj_caching_pool_destroy(&g_CachingPool);
		return status;
	}

	//Create memory pool for application. 
	//Pool factory is thread safe, no need to lock 
	g_pSipPool = pj_pool_create(&g_CachingPool.factory, "SIPEndPoint", 100*1024, 1024,NULL);
	//PJ_ASSERT_RETURN(g_pSipPool, PJ_ENOMEM);
	if (status != PJ_SUCCESS) { SipDestroy(); return PJ_ENOMEM;}

	status = pjsip_endpt_create(&g_CachingPool.factory, "SIPEndPoint", &g_pSipEndPoint);
	//PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);
	if (status != PJ_SUCCESS) { SipDestroy(); return status;}

	//Init logger module. 
	init_msg_logger();
	msg_logger_set_enabled(1);

	//Start transaction layer module. 
	status = pjsip_tsx_layer_init_module(g_pSipEndPoint);
	if (status != PJ_SUCCESS) { SipDestroy(); return status;}

	//Init UA layer 
	if (pjsip_ua_instance()->id == -1) {
		pjsip_ua_init_param ua_param;
		pj_bzero(&ua_param, sizeof(ua_param));
		ua_param.on_dlg_forked = &on_dlg_forked;
		pjsip_ua_init_module(g_pSipEndPoint, &ua_param);
	}

	// Initialize Replaces support. See the Replaces specification in RFC 3891
	status = pjsip_replaces_init_module (g_pSipEndPoint);
	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1);


	//Initialize 100rel support 
	status = pjsip_100rel_init_module(g_pSipEndPoint);
	if (status != PJ_SUCCESS)	{ SipDestroy(); return status;}

	//Initialize session timer support 
	status = pjsip_timer_init_module(g_pSipEndPoint);
	if (status != PJ_SUCCESS)	{ SipDestroy(); return status;}

	//Init inv-usage 
	if (pjsip_inv_usage_instance()->id == -1) {
		pjsip_inv_callback inv_cb;
		pj_bzero(&inv_cb, sizeof(inv_cb));
		inv_cb.on_media_update = &on_sip_media_update;
		inv_cb.on_rx_offer = &on_sip_rx_offer;
		inv_cb.on_create_offer = &on_sip_create_offer;
		inv_cb.on_state_changed = &on_sip_state_changed;
		inv_cb.on_new_session = &on_sip_new_session;
		pjsip_inv_usage_init(g_pSipEndPoint, &inv_cb);
	}

	//{{gaoshizhong 20120710 Add use tls
	if (sipek_config.useTLS == PJ_TRUE)
	{
		if(sipek_config.ca_list_file[0] != '\0' ) 
			app_config.udp_cfg.tls_setting.ca_list_file = pj_str(sipek_config.ca_list_file);
		if(sipek_config.cert_file[0] != '\0' )
			app_config.udp_cfg.tls_setting.cert_file = pj_str(sipek_config.cert_file);
		if(sipek_config.privkey_file[0] != '\0' )
			app_config.udp_cfg.tls_setting.privkey_file = pj_str(sipek_config.privkey_file);
		if(sipek_config.password[0] != '\0' )
			app_config.udp_cfg.tls_setting.password = pj_str(sipek_config.password);
		if(sipek_config.ciphers_num != 0 )
			app_config.udp_cfg.tls_setting.ciphers_num = (sipek_config.ciphers_num);
		if(sipek_config.ciphers != 0 )
			app_config.udp_cfg.tls_setting.ciphers = &(sipek_config.ciphers);
		//if(sipek_config.server_name[0] != '\0' )
		//	app_config.udp_cfg.tls_setting.server_name = pj_str(sipek_config.server_name);
		if(sipek_config.method != 0 )
			app_config.udp_cfg.tls_setting.method = sipek_config.method;
		if(sipek_config.verify_server != 0 )
			app_config.udp_cfg.tls_setting.verify_server = sipek_config.verify_server;
		if(sipek_config.verify_client != 0 )
			app_config.udp_cfg.tls_setting.verify_client = sipek_config.verify_client;
		if(sipek_config.require_client_cert != 0 )
			app_config.udp_cfg.tls_setting.require_client_cert = sipek_config.require_client_cert;

		if( (sipek_config.timeout_sec > 0 ) || (sipek_config.timeout_msec > 0 ) )
		{
			app_config.udp_cfg.tls_setting.timeout.sec = sipek_config.timeout_sec;
			app_config.udp_cfg.tls_setting.timeout.msec = sipek_config.timeout_msec;
		}

		if(sipek_config.qos_type != 0 )
			app_config.udp_cfg.tls_setting.qos_type = (pj_qos_type)sipek_config.qos_type;
		if(sipek_config.qos_ignore_error != 1 )
			app_config.udp_cfg.tls_setting.qos_ignore_error = sipek_config.qos_ignore_error;
	}
	//}}gaoshizhong 20120710 Add

	//set config parameters passed by SipConfigStruct
	app_config.udp_cfg.port = sipek_config.listenPort;
	app_config.no_udp =  (sipek_config.noUDP == true ? PJ_TRUE : PJ_FALSE); 
	app_config.no_tcp =  (sipek_config.noTCP == true ? PJ_TRUE : PJ_FALSE);
	app_config.use_tls =  (sipek_config.useTLS == true ? PJ_TRUE : PJ_FALSE);
	pj_memcpy(&pjsua_tcp_cfg, &app_config.udp_cfg, sizeof(pjsua_tcp_cfg));

	//modi by sgb for UDP/TCP Both listen 20140707
	//if (app_config.no_tcp ==PJ_FALSE ) 
	//modi by sgb
	{
		//Create TCP transport.
		pjsip_tpfactory *tcp;
		pjsip_tcp_transport_cfg tcp_cfg;
		pjsip_tcp_transport_cfg_default(&tcp_cfg, pj_AF_INET());

		if (pjsua_tcp_cfg.port > 0)
			pj_sockaddr_set_port(&tcp_cfg.bind_addr, (pj_uint16_t)pjsua_tcp_cfg.port);

		if (pjsua_tcp_cfg.bound_addr.slen > 0)
		{
			status = pj_sockaddr_set_str_addr(tcp_cfg.af, &tcp_cfg.bind_addr,&pjsua_tcp_cfg.bound_addr);
			if (status != PJ_SUCCESS) SipDestroy();
		}

		if (pjsua_tcp_cfg.public_addr.slen > 0)
			tcp_cfg.addr_name.host = pjsua_tcp_cfg.public_addr;

		tcp_cfg.qos_type = pjsua_tcp_cfg.qos_type;
		pj_memcpy(&tcp_cfg.qos_params, &pjsua_tcp_cfg.qos_params, sizeof(pjsua_tcp_cfg.qos_params));

		status = pjsip_tcp_transport_start3(g_pSipEndPoint, &tcp_cfg, &tcp);

		if (status != PJ_SUCCESS) { SipDestroy(); return status;}
	}

	//modi by sgb for UDP/TCP Both listen 20140707
	//if (app_config.no_udp ==PJ_FALSE ) 
	//modi by sgb
	{
		//Create UDP transport.
		pj_sockaddr_in addr;
		pjsip_transport *tp=NULL;
		pjsip_host_port sip_host_port;
		pj_sockaddr_in_init(&addr, NULL, app_config.udp_cfg.port);

		if ( app_config.udp_cfg.port > 0)
			pj_sockaddr_in_set_port(&addr, (pj_uint16_t) app_config.udp_cfg.port);

		if (app_config.udp_cfg.bound_addr.slen >0)
		{
			status = pj_sockaddr_in_set_str_addr(&addr,&app_config.udp_cfg.bound_addr);
			if (status != PJ_SUCCESS) SipDestroy();
		}

		if (app_config.udp_cfg.public_addr.slen >0)
			sip_host_port.host = app_config.udp_cfg.public_addr;
		if ( app_config.udp_cfg.port > 0)
			sip_host_port.port = app_config.udp_cfg.port;
		//notice update 
		status = pjsip_udp_transport_start(g_pSipEndPoint, &addr, NULL, 1, &tp);

		if (status != PJ_SUCCESS) { SipDestroy(); return status;}
	}

	//{{gaoshizhong 20120710 Add use tls
	//Add TLS transport when application wants one
	if (app_config.use_tls == PJ_TRUE)
	{
		//Create TLS transport.
		pjsip_host_port a_name;
		pjsip_tpfactory *tls;
		pj_sockaddr_in local_addr;
		//Init local address
		pj_sockaddr_in_init(&local_addr, 0, 0);
		if ( pjsua_tcp_cfg.port > 0)
		{
			pjsua_tcp_cfg.port++;//Set TLS port as TCP port+1
			local_addr.sin_port = pj_htons((pj_uint16_t)pjsua_tcp_cfg.port);
		}
		if (pjsua_tcp_cfg.bound_addr.slen)
		{
			status = pj_sockaddr_in_set_str_addr(&local_addr,&pjsua_tcp_cfg.bound_addr);
			if (status != PJ_SUCCESS) SipDestroy();
		}
		//Init published name
		pj_bzero(&a_name, sizeof(pjsip_host_port));
		if (pjsua_tcp_cfg.public_addr.slen)
			a_name.host = pjsua_tcp_cfg.public_addr;

		status = pjsip_tls_transport_start(g_pSipEndPoint,&pjsua_tcp_cfg.tls_setting,&local_addr, &a_name, 1, &tls);
		if (status != PJ_SUCCESS) { SipDestroy(); return status;}

		pjsua_tcp_cfg.port--;
	}
	//}}gaoshizhong 20120710 Add

	status = pj_thread_create(g_pSipPool, "SIPEndPoint", &sip_worker_thread,
		NULL, 0, 0, &sipThread);

	if (status != PJ_SUCCESS)	{ SipDestroy(); return status;}


	status = pjsip_endpt_register_module(g_pSipEndPoint, &mod_video);
	if (status != PJ_SUCCESS)	{ SipDestroy(); return status;}


	// Init the event subscription module.
	// It extends PJSIP by supporting SUBSCRIBE and NOTIFY methods
	status = pjsip_evsub_init_module (g_pSipEndPoint);
	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1);

	// Init xfer/REFER module
	status = pjsip_xfer_init_module (g_pSipEndPoint);
	PJ_ASSERT_RETURN (status == PJ_SUCCESS, 1);

	// Register OPTIONS handler
	pjsip_endpt_register_module(g_pSipEndPoint, &pjsip_options_handler);

	// Add endpoint capabilities (INFO, OPTIONS, etc) for this UA
	pj_str_t allowed[] = { { (char*) "INFO", 4},{ (char*) "MESSAGE",7}, { (char*) "OPTIONS", 7}, { (char*) "PRACK", 5} };       //{"INVITE", 6}, {"ACK",3}, {"BYE",3}, {"CANCEL",6}
	//pj_str_t allowed[] = { { (char*) "INFO", 4},{ (char*) "MESSAGE",7}, { (char*) "OPTIONS", 7} };       //{"INVITE", 6}, {"ACK",3}, {"BYE",3}, {"CANCEL",6}

	//pj_str_t accepted[] = { { (char*) "application/sdp",15},{ (char*) "application/xml",15}, { (char*) "application/media_control+xml", 29} };
	pj_str_t accepted[] = { { (char*) "application/sdp",15} };

	pj_str_t supported[] = { { (char*) "100rel", 6},{ (char*) "timer",5}, { (char*) "replaces", 8}, { (char*) "resource-priority", 17} }; 

	// Register allow methods
	pjsip_endpt_add_capability (g_pSipEndPoint, &mod_video, PJSIP_H_ALLOW, NULL, PJ_ARRAY_SIZE (allowed), allowed);

	// Register in ACCEPT header
	pjsip_endpt_add_capability (g_pSipEndPoint, &mod_video, PJSIP_H_ACCEPT, NULL, PJ_ARRAY_SIZE (accepted),accepted);

	////Register supported methods
	//pjsip_endpt_add_capability (g_pSipEndPoint, &mod_video, PJSIP_H_SUPPORTED, NULL, PJ_ARRAY_SIZE (supported), supported);

	SetSIPMSGChangeHandle(SIP_MSG_Change);

	return status;
}


int SipDestroy(void)
{
	//notice reference  pjsua_destroy2(0);

	//Terminate all calls. 

	//Signal threads to quit: 
	sip_thread_quit_flag = 1;
	//Wait worker threads to quit
	if (sipThread)
	{
		pj_thread_join(sipThread);
		pj_thread_destroy(sipThread);
		sipThread = NULL;
	}

	//Terminate any pending STUN resolution 

	//Wait until all unregistrations are done (ticket #364) 
	//First stage, get the maximum wait time 
	//No waiting if RX is disabled 
	//Second stage, wait for unregistrations to complete 
	SIPCallMgr::Instance().WaitSIPUserUnRegComplete();

	//Note variable 'i' is used below 
	//Wait for some time to allow unregistration and ICE/TURN
	//transports shutdown to complete: 
	//Dumping memory pool usage
	PJ_LOG(3,(THIS_FILE, "Peak memory size=%u MB",
		g_CachingPool.peak_used_size / 1000000));

	if(g_pSipEndPoint)
		pjsip_endpt_destroy(g_pSipEndPoint);

	//Destroy pool and pool factory.
	if (g_pSipPool) 
	{
		pj_pool_release(g_pSipPool);
		g_pSipPool=NULL;
		pj_caching_pool_destroy(&g_CachingPool);
		//End logging
		if (sip_log_file) 
		{
			pj_file_close(sip_log_file);
			sip_log_file = NULL;
		}
		pj_log_pop_indent();
		//Shutdown PJLIB
		pj_shutdown();
	}

	//Clear
	init_data();

	//Done.
	return PJ_SUCCESS;
}

/* Check if IP is private IP address */
static pj_bool_t sip_is_private_ip(const pj_str_t *addr)
{
	const pj_str_t private_net[] = 
	{
		{ "10.", 3 },
		{ "127.", 4 },
		{ "172.16.", 7 },
		{ "192.168.", 8 }
	};
	for (int i=0; i<PJ_ARRAY_SIZE(private_net); ++i)
	{
		if (pj_strncmp(addr, &private_net[i], private_net[i].slen)==0)
			return PJ_TRUE;
	}
	return PJ_FALSE;
}

enum
{
	OUTBOUND_UNKNOWN,	// status unknown
	OUTBOUND_WANTED,	// initiated in registration
	OUTBOUND_ACTIVE,	// got positive response from server
	OUTBOUND_NA		// not wanted or got negative response from server
};

/* Create reg_contact, mainly for SIP outbound */
static void sip_update_regc_contact(void * token,pj_pool_t * pool,pj_str_t contact,pj_str_t * resultcontact)
{
    pj_bool_t need_outbound = PJ_FALSE;
    const pj_str_t tcp_param = pj_str(";transport=tcp");
    const pj_str_t tls_param = pj_str(";transport=tls");
	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser * )token;
    unsigned &       rfc5626_status=pSipUser->m_nRFC5626_status;/**< SIP outbound status:0: not used 1: requested 2: acknowledged by servers   */
    pj_str_t	     rfc5626_instprm=pj_str((char * )pSipUser->m_strRFC5626_instprm.c_str());/**< SIP outbound instance param.  */
    pj_str_t         rfc5626_regprm=pj_str((char * )pSipUser->m_strRFC5626_regprm.c_str());/**< SIP outbound reg param.        */

    if (!pSipUser->m_nUseRFC5626)
	goto done;

    /* Check if outbound has been requested and rejected */
    if (rfc5626_status == OUTBOUND_NA)
	goto done;

    if (pj_stristr(&contact, &tcp_param)==NULL &&
	pj_stristr(&contact, &tls_param)==NULL)
    {
	/* Currently we can only do SIP outbound for TCP
	 * and TLS.
	 */
	goto done;
    }

    /* looks like we can use outbound */
    need_outbound = PJ_TRUE;

done:
	if (!need_outbound)
	{
		/* Outbound is not needed/wanted for the account. acc->reg_contact
		* is set to the same as acc->contact.
		*/
		* resultcontact = contact;
		rfc5626_status = OUTBOUND_NA;
	}
	else 
	{
		/* Need to use outbound, append the contact with +sip.instance and
		* reg-id parameters.
		*/
		unsigned len;
		pj_str_t reg_contact;
		rfc5626_status = OUTBOUND_WANTED;
		len = contact.slen + rfc5626_instprm.slen +
			rfc5626_regprm.slen;
		reg_contact.ptr = (char*) pj_pool_alloc(pool, len);

		pj_strcpy(&reg_contact, &contact);
		pj_strcat(&reg_contact, &rfc5626_regprm);
		pj_strcat(&reg_contact, &rfc5626_instprm);
		* resultcontact = reg_contact;
	}
}

//参看pjsua_acc.c  keep_alive_timer_cb
/* Keep alive timer callback */
static void keep_alive_timer_cb(pj_timer_heap_t *th, pj_timer_entry *te)
{
    SIPUser *acc;
    pjsip_tpselector tp_sel;
    pj_time_val delay;
    //char addrtxt[PJ_INET6_ADDRSTRLEN];
    pj_status_t status;
    PJ_UNUSED_ARG(th);
    te->id = PJ_FALSE;
    acc = (SIPUser*) te->user_data;
    /* Select the transport to send the packet */
    pj_bzero(&tp_sel, sizeof(tp_sel));
    tp_sel.type = PJSIP_TPSELECTOR_TRANSPORT;
    tp_sel.u.transport = acc->m_pKaTransport;

    /* Send raw packet */
    status = pjsip_tpmgr_send_raw(pjsip_endpt_get_tpmgr(g_pSipEndPoint),
				  PJSIP_TRANSPORT_UDP, &tp_sel,NULL, acc->m_strKaData.ptr, 
				  acc->m_strKaData.slen, &acc->m_KaTarget, acc->m_nKaTarget_len,NULL, NULL);
    if (status != PJ_SUCCESS && status != PJ_EPENDING)
	{
    }

    /* Check just in case keep-alive has been disabled. This shouldn't happen
     * though as when ka_interval is changed this timer should have been
     * cancelled.
     */
    if (acc->m_nKaInterval == 0)
	goto on_return;

    /* Reschedule next timer */
    delay.sec = acc->m_nKaInterval;
    delay.msec = 0;
    status = pjsip_endpt_schedule_timer(g_pSipEndPoint, te, &delay);
    if (status == PJ_SUCCESS) {
	te->id = PJ_TRUE;
    }
	else
	{
    }

on_return:
	return;
}

//参看pjsua_acc.c update_keep_alive
/* Update keep-alive for the account */
static void update_keep_alive(SIPUser * acc, pj_bool_t start,
			      struct pjsip_regc_cbparam *param)
{
    /* In all cases, stop keep-alive timer if it's running. */
    if (acc->m_KaTimer.id) {
	pjsip_endpt_cancel_timer(g_pSipEndPoint, &acc->m_KaTimer);
	acc->m_KaTimer.id = PJ_FALSE;
	PJ_LOG(5,("SipReg","update_keep_alive pjsip_endpt_cancel_timer"));
	pjsip_transport_dec_ref(acc->m_pKaTransport);
	acc->m_pKaTransport = NULL;
    }

    if (start)
	{
	pj_time_val delay;
	pj_status_t status;

	if (/*pjsua_var.stun_srv.ipv4.sin_family == 0 ||*/
	    acc->m_nKaInterval == 0 ||
	    param->rdata->tp_info.transport->key.type != PJSIP_TRANSPORT_UDP)
	{
	    /* Keep alive is not necessary */
	    return;
	}

	/* Save transport and destination address. */
	acc->m_pKaTransport = param->rdata->tp_info.transport;
	pjsip_transport_add_ref(acc->m_pKaTransport);
	pj_memcpy(&acc->m_KaTarget, &param->rdata->pkt_info.src_addr,
		  param->rdata->pkt_info.src_addr_len);
	acc->m_nKaTarget_len = param->rdata->pkt_info.src_addr_len;

	/* Setup and start the timer */
	acc->m_KaTimer.cb = &keep_alive_timer_cb;
	acc->m_KaTimer.user_data = (void*)acc;

	delay.sec = acc->m_nKaInterval;
	delay.msec = 0;
	status = pjsip_endpt_schedule_timer(g_pSipEndPoint, &acc->m_KaTimer,&delay);
	if (status == PJ_SUCCESS)
	{
		PJ_LOG(5,("SipReg","update_keep_alive pjsip_endpt_schedule_timer"));
	    acc->m_KaTimer.id = PJ_TRUE;
	    PJ_LOG(4,(THIS_FILE, "Keep-alive timer started for acc %d, "
				 "destination:%s:%d, interval:%ds",0/*acc->index*/,param->rdata->pkt_info.src_name,
				 param->rdata->pkt_info.src_port,acc->m_nKaInterval));
	}
	else
	{
	    acc->m_KaTimer.id = PJ_FALSE;
	    pjsip_transport_dec_ref(acc->m_pKaTransport);
	    acc->m_pKaTransport = NULL;
	}
    }
}

/* Update NAT address from the REGISTER response */
static pj_bool_t sip_reg_check_nat_addr(struct pjsip_regc_cbparam *param)
{
    pjsip_transport *tp;
    const pj_str_t *via_addr;
    pj_pool_t *pool;
    int rport;
    pjsip_sip_uri *uri;
    pjsip_via_hdr *via;
    pj_sockaddr contact_addr;
    pj_sockaddr recv_addr;
    pj_status_t status;
    pj_bool_t matched;
    pj_str_t srv_ip;
    pjsip_contact_hdr *contact_hdr;
    const pj_str_t STR_CONTACT = { "Contact", 7 };

	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser * )param->token;
	if(pSipUser->m_pSipRegc == NULL) return PJ_FALSE;

	std::string strContact="";
	pSipUser->GetRegContactUri(strContact);
	pj_str_t         reg_contact=pj_str((char *)strContact.c_str());
	pj_str_t         contact;

	//notice reference acc_check_nat_addr
	tp = param->rdata->tp_info.transport;
	pool = g_pSipPool;

    /* Get the received and rport info */
    via = param->rdata->msg_info.via;
	if (via->rport_param < 1) 
	{
		/* Remote doesn't support rport */
		rport = via->sent_by.port;
		if (rport==0) 
		{
			pjsip_transport_type_e tp_type;
			tp_type = (pjsip_transport_type_e) tp->key.type;
			rport = pjsip_transport_get_default_port_for_type(tp_type);
		}
	} 
	else rport = via->rport_param;

    if (via->recvd_param.slen != 0)
        via_addr = &via->recvd_param;
    else
        via_addr = &via->sent_by.host;

	pjsip_host_port  host_port_addr;      /**< Address for Via header         */
	//pool all use //notice pool not release
	pj_strdup(pool, &host_port_addr.host, via_addr);
	host_port_addr.port = rport;
	pjsip_regc_set_via_sent_by(pSipUser->m_pSipRegc, &host_port_addr,tp);

	//pjsip_publishc_set_via_sent_by(publish_sess,&host_port_addr, tp);
    //Compare received and rport with the URI in our registration
    contact_hdr = (pjsip_contact_hdr*)
		  pjsip_parse_hdr(pool, &STR_CONTACT,(char *)strContact.c_str(),strContact.length(), NULL);
    pj_assert(contact_hdr != NULL);
    uri = (pjsip_sip_uri*) contact_hdr->uri;
    pj_assert(uri != NULL);
    uri = (pjsip_sip_uri*) pjsip_uri_get_uri(uri);

	if (uri->port == 0) 
	{
		pjsip_transport_type_e tp_type;
		tp_type = (pjsip_transport_type_e) tp->key.type;
		uri->port = pjsip_transport_get_default_port_for_type(tp_type);
	}

    // Convert IP address strings into sockaddr for comparison.
    status = pj_sockaddr_parse(pj_AF_UNSPEC(), 0, &uri->host, 
			       &contact_addr);
    if (status == PJ_SUCCESS)
	status = pj_sockaddr_parse(pj_AF_UNSPEC(), 0, via_addr, 
				   &recv_addr);
	if (status == PJ_SUCCESS) 
	{
		//Compare the addresses as sockaddr according to the ticket above
		matched = (uri->port == rport &&
			pj_sockaddr_cmp(&contact_addr, &recv_addr)==0);
	}
	else 
	{
		/* Compare the addresses as string, as before */
		matched = (uri->port == rport &&
			pj_stricmp(&uri->host, via_addr)==0);
	}

	if (matched) 
	{
		/* Address doesn't change */
		return PJ_FALSE;
	}

    /* Get server IP */
    srv_ip = pj_str(param->rdata->pkt_info.src_name);

    if (/*acc->cfg.allow_contact_rewrite != 2 &&*/ !sip_is_private_ip(&uri->host) &&
	!sip_is_private_ip(&srv_ip) && sip_is_private_ip(via_addr))
    {
	/* Don't switch */
	return PJ_FALSE;
    }

    /* Also don't switch if only the port number part is different, and
     * the Via received address is private.
     */
    if (/*acc->cfg.allow_contact_rewrite != 2 &&*/
	pj_sockaddr_cmp(&contact_addr, &recv_addr)==0 &&
	sip_is_private_ip(via_addr))
    {
	/* Don't switch */
	return PJ_FALSE;
    }

    /*
     * Build new Contact header
     */
    {
	const char *ob = ";ob";
	char *tmp;
	const char *beginquote, *endquote;
	int len;

	/* Enclose IPv6 address in square brackets */
	if (tp->key.type & PJSIP_TRANSPORT_IPV6)
	{
	    beginquote = "[";
	    endquote = "]";
	}
	else
	{
	    beginquote = endquote = "";
	}

	pj_str_t	    user_part=pj_str((char *)pSipUser->m_strUserID.c_str());
	pj_str_t	    contact_params=pj_str((char *)pSipUser->m_strContactParams.c_str());
	pj_str_t	    contact_uri_params=pj_str((char *)pSipUser->m_strContactURIParams.c_str());
	unsigned        use_rfc5626=pSipUser->m_nUseRFC5626;

	tmp = (char*) pj_pool_alloc(pool, PJSIP_MAX_URL_SIZE);
	len = pj_ansi_snprintf(tmp, PJSIP_MAX_URL_SIZE,
			       "<sip:%.*s%s%s%.*s%s:%d;transport=%s%.*s%s>%.*s",
			       (int)user_part.slen,
			       user_part.ptr,
			       (user_part.slen? "@" : ""),
			       beginquote,
			       (int)via_addr->slen,
			       via_addr->ptr,
			       endquote,
			       rport,
			       tp->type_name,
			       (int)contact_uri_params.slen,
			       contact_uri_params.ptr,
			       (use_rfc5626? ob: ""),
			       (int)contact_params.slen,
			       contact_params.ptr);
	if (len < 1)
	{
	    return PJ_FALSE;
	}

	if(SIPCallMgr::Instance().m_nSIPServerType >0 )
	{
		pj_strdup2_with_null(pool, &contact, tmp);
		sip_update_regc_contact(pSipUser,pool,contact,&reg_contact);
	}

    }
    if (/*acc->cfg.contact_rewrite_method == 2 &&*/ pSipUser->m_pSipRegc != NULL)
	{
		if(SIPCallMgr::Instance().m_nSIPServerType >0 )
			pjsip_regc_update_contact(pSipUser->m_pSipRegc, 1, &/*acc->*/reg_contact);
    }

    //Perform new registration
	if(pSipUser && pSipUser->m_bHasVIA==true && pSipUser->m_bReplyReg==false)
	{
		if(SIPCallMgr::Instance().m_nProxySendRtp >0 && SIPCallMgr::Instance().m_nSIPServerType >0 )
		{
			SipRegisterAccountReply(pSipUser);
		}
	}

    return PJ_TRUE;
}

//参看pjsua_acc.c  regc_cb
//regc callback
static void sip_regc_cb(struct pjsip_regc_cbparam *param)
{
	int nAccountId=-1;
	if(param==NULL) return;
	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser * )param->token;
	if(pSipUser) nAccountId=pSipUser->m_nAccountId;

	sip_register_thread();

	if (param->code>0)
	{
		const pj_str_t*strStatus=pjsip_get_status_text(param->code);
		SIPCallMgr::Instance().OnUserIdRegistration(nAccountId,param->code,strStatus->ptr);
		if(pSipUser) pSipUser->m_bTimeOutReg=true;
	}

	int st_code=param->code;
	pjsip_rx_data *rdata=NULL;
	pjsip_contact_hdr *contact=NULL;
	char * addrStrPtr=NULL;
	int addrStrLen=0;
	char strReceivedIP[ReceivedIPMaxLen]={0};

	SIPUser *acc =pSipUser;
	if (param->status!=PJ_SUCCESS)
	{
		PJ_LOG(5,("SipReg","sip_regc_cb param->status=%d",param->status));
		/* Stop keep-alive timer if any. */
		update_keep_alive(acc, PJ_FALSE, NULL);
	}
	else if (param->code < 0 || param->code >= 300)
	{
		PJ_LOG(5,("SipReg","sip_regc_cb param->code < 0 || param->code >= 300 param->code=%d",param->code));
		/* Stop keep-alive timer if any. */
		update_keep_alive(acc, PJ_FALSE, NULL);
	}
	else if (PJSIP_IS_STATUS_IN_CLASS(param->code, 200))
	{
		if (param->expiration < 1)
		{
			PJ_LOG(5,("SipReg","sip_regc_cb param->expiration < 1 param->expiration=%d",param->expiration));
			/* Stop keep-alive timer if any. */
			update_keep_alive(acc, PJ_FALSE, NULL);
		}
		else
		{
			PJ_LOG(5,("SipReg","sip_regc_cb update_keep_alive=PJ_TRUE,%d",1));
			/* Check NAT bound address */
			//if (acc_check_nat_addr(acc, param))
			if((st_code==200 )) 
			{
				rdata=param->rdata;

				//收到注册响应取contact 注 暂时性 服务器在公网时需要
				contact = (pjsip_contact_hdr*)
					pjsip_msg_find_hdr(rdata->msg_info.msg, PJSIP_H_CONTACT,NULL);
				pjsip_via_hdr * via=(pjsip_via_hdr*)
					pjsip_msg_find_hdr(rdata->msg_info.msg, PJSIP_H_VIA,NULL);

				if(pSipUser)
				{
					pSipUser->m_SIPVia.m_nCapNum=0;
				}
				if(via && pSipUser)
				{
					//Copy all the via headers, in order.
					via = rdata->msg_info.via;
					while ( via && (via->type==PJSIP_H_VIA) ) 
					{
						pjsip_transport *tp;
						pj_str_t *via_addr;
						int rport=0;
						tp = rdata->tp_info.transport;
						pSipUser->GetSIPViaRportViaAddr(via, rport,&via_addr,tp);
						if(rport>0 )
						{
							pSipUser->m_nViaRPort=rport;
							pSipUser->m_nViaSendByPort=via->sent_by.port;
							char addr[128]={0};
							strncpy(addr,via_addr->ptr,via_addr->slen);
							pSipUser->m_strViaSendByAddress=addr;
							pSipUser->m_bHasVIA=true;

							pjsip_via_hdr *new_via;
							new_via = (pjsip_via_hdr*)pjsip_hdr_clone(g_pSipPool, via);
							if(new_via)
							{
								unsigned int & viaNum=pSipUser->m_SIPVia.m_nCapNum;
								pSipUser->m_SIPVia.m_Caps[viaNum]=new_via;
								viaNum++;
							}
						}
						via = via->next;
					}
				}
				if(contact) 
				{
					pjsip_param * param;
					pj_str_t received=pj_str("received");
					param = contact->other_param.next;
					while (param != NULL)
					{
						if(!pj_stricmp(&(param->name),&received))
						{
							addrStrPtr=param->value.ptr;
							addrStrLen=param->value.slen;
							break;
						}
						if(param->name.ptr==NULL) break;
						else param = param->next;
					}
					if(addrStrPtr!=NULL)
					{
						addrStrPtr=addrStrPtr+5;
						addrStrLen=addrStrLen-6;
						strncpy(strReceivedIP,addrStrPtr,addrStrLen<ReceivedIPMaxLen ? addrStrLen:ReceivedIPMaxLen-1);
					}
				}
				/* Check NAT bound address */
				if (sip_reg_check_nat_addr(param))
				{
					pj_log_pop_indent();
				}
			}
			/* Start keep-alive timer if necessary. */
			update_keep_alive(acc, PJ_TRUE, param);
		}
	}
	else
	{
		PJ_LOG(5,("SipReg","sip_regc_cb last SIP registration updated status param->code=%d",param->code));
		PJ_LOG(4, (THIS_FILE, "SIP registration updated status=%d", param->code));
	}

}

//Verify that valid SIP url is given.  reference pjsua_verify_sip_url
pj_status_t pjsip_verify_sip_url(const char *c_url)
{
    pjsip_uri *p;
    pj_pool_t *pool;
    char *url;
    int len = (c_url ? pj_ansi_strlen(c_url) : 0);
    if (!len) return PJSIP_EINVALIDURI;
    pool = pj_pool_create(&g_CachingPool.factory, "check%p", 1024, 0, NULL);
    if (!pool) return PJ_ENOMEM;
    url = (char*) pj_pool_alloc(pool, len+1);
    pj_ansi_strcpy(url, c_url);

    p = pjsip_parse_uri(pool, url, len, 0);
    if (!p || (pj_stricmp2(pjsip_uri_get_scheme(p), "sip") != 0 &&
	       pj_stricmp2(pjsip_uri_get_scheme(p), "sips") != 0))
    {
	p = NULL;
    }
    pj_pool_release(pool);
    return p ? 0 : PJSIP_EINVALIDURI;
}

int SipRegisterAccount(void * token,char* uri, char* reguri, char* domain, char* username, char* password, char* proxy, bool isdefault)
{
	pjsua_acc_config accConfig; 
	pjsua_acc_config_default(&accConfig);

	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser *) token;
	if(pSipUser==NULL) return -1;

	//set parameters 
	//disable contact rewrite
	accConfig.allow_contact_rewrite = 2;
	accConfig.publish_enabled = sipek_config.publishEnabled == true ? PJ_TRUE : PJ_FALSE; // enable publish
	accConfig.reg_timeout = sipek_config.expires;
	accConfig.id = pj_str(uri);
	accConfig.reg_uri = pj_str(reguri);
	pj_str_t tmpproxy = pj_str(proxy);
	if(tmpproxy.slen > 0)
	{
		if (pjsip_verify_sip_url(proxy) != 0) {
			PJ_LOG(1,(THIS_FILE, "Error: invalid SIP URL '%s' in proxy argument", proxy));
			return PJ_EINVAL;
		}
		accConfig.proxy_cnt = 1;
		pj_strcat2(&tmpproxy, ";lr");
		accConfig.proxy[0] = tmpproxy;
	}
	//AUTHENTICATION
	accConfig.cred_count = 1;
	accConfig.cred_info[0].username = pj_str(username);
	accConfig.cred_info[0].realm = pj_str(domain);
	accConfig.cred_info[0].scheme = pj_str("Digest");
	accConfig.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	accConfig.cred_info[0].data = pj_str(password);

	//use to make call
	pSipUser->m_SipCredInfo[0].username = pj_strdup3(g_pSipPool,username);
	pSipUser->m_SipCredInfo[0].realm = pj_strdup3(g_pSipPool,domain);
	pSipUser->m_SipCredInfo[0].scheme = pj_strdup3(g_pSipPool,"Digest");
	pSipUser->m_SipCredInfo[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	pSipUser->m_SipCredInfo[0].data = pj_strdup3(g_pSipPool,password);
	pSipUser->m_SipCredCount = 1;

	int nRet=0;
	unsigned i=0;
	pjsip_tx_data *tdata;
	pj_status_t status=0;
	unsigned contact_cnt =1;
	pj_str_t contacts[12];

	pjsip_endpoint *endpt=g_pSipEndPoint;
	status = pjsip_regc_create(endpt, token, &sip_regc_cb, &pSipUser->m_pSipRegc);
	if (status != PJ_SUCCESS) return -100;
	std::string  strContact ="";
	pSipUser->GetRegContactUri(strContact);
	contacts[0] = pj_str((char *)strContact.c_str());

	status = pjsip_regc_init( pSipUser->m_pSipRegc,&accConfig.reg_uri, &accConfig.id, &accConfig.id,contact_cnt,contacts, accConfig.reg_timeout ? accConfig.reg_timeout : 600);

	if (status != PJ_SUCCESS) 
	{
		pjsip_regc_destroy(pSipUser->m_pSipRegc);
		return -110;
	}

	//add outbound 20130425
	pj_list_init(&SIPCallMgr::Instance().m_OutboundProxyRoute);
	for (i=0; i<SIPCallMgr::Instance().m_nOutboundProxyCount; ++i)
	{
		pj_str_t tmp;
		pj_str_t hname = { "Route", 5};
		pjsip_route_hdr *r;
		pj_strdup2/*pj_strdup2_with_null*/(g_pSipPool, &tmp, SIPCallMgr::Instance().m_strOutboundProxy[i]);
		r = (pjsip_route_hdr*)pjsip_parse_hdr(g_pSipPool, &hname, tmp.ptr,(unsigned)tmp.slen, NULL);
		if (r != NULL) 
		{
			//设置了outbound后此控制request line不再使用outbound  if (has_lr_param)
			if (SIPCallMgr::Instance().m_nForcelrParam)
			{
				pjsip_sip_uri *sip_url;
				if (!PJSIP_URI_SCHEME_IS_SIP(r->name_addr.uri) && !PJSIP_URI_SCHEME_IS_SIP(r->name_addr.uri))
				{
					status = PJSIP_EINVALIDSCHEME;
				}
				sip_url = (pjsip_sip_uri*)r->name_addr.uri;
				sip_url->lr_param = 1;
			}
			pj_list_push_back(&SIPCallMgr::Instance().m_OutboundProxyRoute, r);
		}
	}

	if(SIPCallMgr::Instance().m_nOutboundProxyCount>0) 
	{
		const pjsip_route_hdr *r;
		pj_list_init(&SIPCallMgr::Instance().m_RouteHdr);
		r = SIPCallMgr::Instance().m_OutboundProxyRoute.next;
		while (r != &SIPCallMgr::Instance().m_OutboundProxyRoute)
		{
			pj_list_push_back(&SIPCallMgr::Instance().m_RouteHdr, pjsip_hdr_shallow_clone(g_pSipPool, r));
			r = r->next;
		}
		if (!pj_list_empty(&SIPCallMgr::Instance().m_RouteHdr))
			pjsip_regc_set_route_set(pSipUser->m_pSipRegc, &SIPCallMgr::Instance().m_RouteHdr );
	}

	pjsip_cred_info cred;
	pj_bzero(&cred, sizeof(cred));
	pj_memcpy(&cred,&(accConfig.cred_info[0]),sizeof(cred));
	status = pjsip_regc_set_credentials(pSipUser->m_pSipRegc, 1, &cred);
	if (status != PJ_SUCCESS) 
	{
		pjsip_regc_destroy(pSipUser->m_pSipRegc);
		return -115;
	}
	//Add User-Agent Header
	if(strlen(SIPCallMgr::Instance().m_strUserAgentName) > 0)
	{
		pjsip_hdr hdr_list;
		const pj_str_t USER_AGENT = { "User-Agent", 10};
		pj_pool_t * pool = NULL;
		pjsip_generic_string_hdr *h=NULL;
		pj_list_init (&hdr_list);
		pool = pjsip_endpt_create_pool(pjsip_ua_get_endpt(pjsip_ua_instance()), "reg%p", 512,512);
		pj_str_t useragent=pj_str(SIPCallMgr::Instance().m_strUserAgentName);
		h = pjsip_generic_string_hdr_create (pool, &USER_AGENT, &useragent);
		pj_list_push_back (&hdr_list, (pjsip_hdr*) h);
		pjsip_regc_add_headers (pSipUser->m_pSipRegc, &hdr_list);
		pj_pool_release(pool);
	}
	//Register
	status = pjsip_regc_register(pSipUser->m_pSipRegc, PJ_TRUE, &tdata);
	if (status != PJ_SUCCESS) {
		pjsip_regc_destroy(pSipUser->m_pSipRegc);
		return -120;
	}
	//set request line uri is addr
	if(SIPCallMgr::Instance().m_nUseRegReqLine)
	{
		char * addr=(char *)SIPCallMgr::Instance().m_strRegReqLineAddr.c_str();
		pSipUser->SetRequestLineAddr(tdata,addr);
	}

	//print uri
	char uribuf[128]={0};
	pjsip_uri * urid = (pjsip_uri*) pjsip_uri_get_uri(tdata->msg->line.req.uri);
	pjsip_uri_print( PJSIP_URI_IN_REQ_URI, urid, uribuf, 128);

	status = pjsip_regc_send(pSipUser->m_pSipRegc, tdata);
	return nRet;
}


int SipRegisterAccountReply(void * token)
{
	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser *) token;
	if(pSipUser==NULL) return -1;

	int nRet=0;
	pjsip_tx_data *tdata;
	pj_status_t status=0;
	//Register
	status = pjsip_regc_register(pSipUser->m_pSipRegc, PJ_TRUE, &tdata);
	if (status != PJ_SUCCESS) {
		pjsip_regc_destroy(pSipUser->m_pSipRegc);
		return -120;
	}

	//set request line uri is addr
	if(SIPCallMgr::Instance().m_nUseRegReqLine)
	{
		char * addr=(char *)SIPCallMgr::Instance().m_strRegReqLineAddr.c_str();
		pSipUser->SetRequestLineAddr(tdata,addr);
	}

	pSipUser->SipHdrViaParamLocalAdd(tdata->msg,pSipUser->m_pPOOL);//set
	status = pjsip_regc_send(pSipUser->m_pSipRegc, tdata);
	pSipUser->m_bReplyReg=true;
	return nRet;
}

// Delete account.
pj_status_t SipUnRegisterAccount(void * token)
{
	pj_status_t status = 0;
	pjsip_tx_data *tdata = 0;
	SIPUser * pSipUser=NULL;
	pSipUser=(SIPUser *) token;
	if(pSipUser==NULL) return -1;
	pjsip_endpt_cancel_timer(g_pSipEndPoint, &pSipUser->m_KaTimer);
	pSipUser->m_KaTimer.id = PJ_FALSE;
	//Delete registration
	if (pSipUser->m_pSipRegc != NULL) 
	{
		if (pSipUser->m_pSipRegc == NULL) 
		{
			PJ_LOG(3,(THIS_FILE, "Currently not registered"));
			status = PJ_EINVALIDOP;
			return status;
		}
		//notice
		//pjsua_pres_unpublish(&pjsua_var.acc[acc_id]);
		status = pjsip_regc_unregister(pSipUser->m_pSipRegc, &tdata);
		if (status == PJ_SUCCESS) 
		{
			status = pjsip_regc_send( pSipUser->m_pSipRegc, tdata );
		}
		if (pSipUser->m_pSipRegc)
		{
			pjsip_regc_destroy(pSipUser->m_pSipRegc);
		}
		pSipUser->m_pSipRegc = NULL;
	}
	return status;
}

static pj_bool_t Send200OKNOSDP(pjsip_rx_data *rdata)
{
	pjsip_tx_data *tdata;
	pjsip_response_addr res_addr;
	pj_status_t status;
	//Don't want to handle if shutdown is in progress
	if (sip_thread_quit_flag) {
		pjsip_endpt_respond_stateless(g_pSipEndPoint, rdata,PJSIP_SC_TEMPORARILY_UNAVAILABLE, NULL,NULL, NULL);
		return PJ_TRUE;
	}
	//Create basic response.
	status = pjsip_endpt_create_response(g_pSipEndPoint, rdata, 200, NULL,&tdata);
	if (status != PJ_SUCCESS) {
		return PJ_TRUE;
	}
	//Send response
	pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
	status = pjsip_endpt_send_response(g_pSipEndPoint, &res_addr, tdata, NULL, NULL);
	if (status != PJ_SUCCESS) pjsip_tx_data_dec_ref(tdata);
	return PJ_TRUE;
}

pj_bool_t SendResponse200OKNOSDP(pjsip_rx_data *rdata ,int code)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_response_addr res_addr;
	//Create basic response.
	status = pjsip_endpt_create_response(g_pSipEndPoint, rdata, code, NULL, &tdata);
	//Send response
	pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
	status = pjsip_endpt_send_response(g_pSipEndPoint, &res_addr, tdata, NULL, NULL);
	if (status != PJ_SUCCESS) pjsip_tx_data_dec_ref(tdata);
	return PJ_TRUE;
}

//reference pjsua_parse_media_type
void pjsip_parse_media_type( pj_pool_t *pool,const pj_str_t *mime,pjsip_media_type *media_type)
{
	pj_str_t tmp;
	char *pos;

	pj_bzero(media_type, sizeof(*media_type));

	pj_strdup_with_null(pool, &tmp, mime);

	pos = pj_strchr(&tmp, '/');
	if (pos) {
		media_type->type.ptr = tmp.ptr; 
		media_type->type.slen = (pos-tmp.ptr);
		media_type->subtype.ptr = pos+1; 
		media_type->subtype.slen = tmp.ptr+tmp.slen-pos-1;
	} else {
		media_type->type = tmp;
	}
}
