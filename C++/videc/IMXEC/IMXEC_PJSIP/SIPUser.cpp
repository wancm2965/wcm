#include "stdafx.h"
#include "SIPUser.h"
#include "SIPCallMgr.h"
#include "XSocketUDP.h"
#include "XSocketTCP.h"
#include "XUtil.h"
#include "../IMXEC_Call/CallMgr.h"

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0
#include "../IMXEC_Call/CallBase.h"
#endif

#include <algorithm>

#include "MMSystem.h"
#include "DNSLookup.h"

extern unsigned short GetTCPPort(void);
extern unsigned short GetRTPPort(unsigned short usRTPBasePort);
extern bool IsCallIPString(const char * str);
extern int  IsCallIPPortString(const char * str);
extern int GetLocalFirstIP(std::string & strLocalIP);
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);

#define MAX_PATH_LEN    1024

#define sip_param_add(list,pname,pvalue)  \
	do { \
	pjsip_param *param; \
	param=PJ_POOL_ALLOC_T(pool, pjsip_param); \
	param->name = pj_str(pname); \
	param->value = pj_str(pvalue); \
	pj_list_insert_before(&list, param); \
	} while (0)


SIPUser::SIPUser(SIPCallMgr&rSIPCallMgr)
:m_rSIPCallMgr(rSIPCallMgr)
{
	m_nAccountId=0;		//ACC ID未注册时为0
	m_nLineID =-1;		//m_nLineID起始为0
	m_strUserID ="";
	m_strUserName ="";
	m_strAuthUserID ="";
	m_strPassword ="";
	m_strDomainRealm ="";
	m_strSIPProxy ="";
	m_strStunServer ="";
	m_bUseStun =false;
	//其他的成员初始化
	Init();
}

//注意 m_strAuthUserID 不需要时要设为空
SIPUser::SIPUser(SIPCallMgr&rSIPCallMgr,int nLineID,const std::string&strUserID,const std::string&strUserName,const std::string&strPassword,const std::string&strSIPProxy,const std::string&strAuthUserID,const std::string&strDomain)
:m_rSIPCallMgr(rSIPCallMgr)
,m_nLineID(nLineID)
,m_strUserID(strUserID)
,m_strUserName(strUserName)
,m_strPassword(strPassword)
,m_strDomainRealm(strDomain)
,m_strAuthUserID(strAuthUserID)
,m_strSIPProxy(strSIPProxy)
,m_strStunServer("")
,m_bUseStun(false)
{
	m_nAccountId=0;    //ACC ID起始为1
	std::transform(m_strDomainRealm.begin(), m_strDomainRealm.end(), m_strDomainRealm.begin(), ::tolower);
	std::transform(m_strSIPProxy.begin(), m_strSIPProxy.end(), m_strSIPProxy.begin(), ::tolower);
	//其他的成员初始化
	Init();
}

SIPUser::~SIPUser(void)
{
	if(m_pPOOL)
	{
		pj_pool_release(m_pPOOL);
		m_pPOOL=NULL;
	}
}

void SIPUser::Init(void)
{
	m_strSIPProxyHost="";
	m_usProxyPort=5060;
	m_nFlagAddedContact=0;
	m_bActive=false;
	m_bRegistrationSuccess=false;
	m_bRegistrationError=false;
	m_strRegistrationErrorCode="";
	m_bAvaya=false;
	m_bCanBeDeleted=false;
	m_SipCredCount =0;
	m_bHasVIA=false;
	m_nViaRPort=0;
	m_nViaSendByPort=0;
	m_strViaSendByAddress="";
	m_bValid=false;
	m_pSipRegc=NULL;
	m_bReplyReg=false;
	m_strContactParams="";
	m_strContactURIParams="";
	m_nUseRFC5626=PJ_FALSE;
	m_nRFC5626_status=0;
	m_strRFC5626_instprm="";
	m_strRFC5626_regprm="";
	m_nKaInterval=2;
	m_strKaData=pj_str("\r\n");
	m_KaTimer.id=PJ_FALSE;
	m_pPOOL=NULL;
	m_pPOOL = pjsip_endpt_create_pool(pjsip_ua_get_endpt(pjsip_ua_instance()), "SIPUser%p", 2048,256 );
	m_nSIPREGFail=0;
	m_strLocalIP="";
	m_nRegSetting =0;
	m_nSIPCheckLastTimestamp=XGetTimestamp();
	m_nSIPRegFailLastTimestamp=XGetTimestamp();
	m_bSendReg=false;
	m_bTimeOutReg=false;
	m_nLocalBindPort=0;
}

int SIPUser::Open(void)
{
	if (m_strSIPProxy.length()==0)	return -1;
	if(m_nSIPREGFail >=2 ) return 0;
	int nTryCount=0;
	std::string  strDomainRealm =m_strDomainRealm;
	std::string  strAuthUserID =m_strAuthUserID;
	sip_register_thread();
	while (m_rSIPCallMgr.m_nActualSIPListenPort==0)
	{
		nTryCount++;
		try{
			unsigned short usListenPort=m_rSIPCallMgr.GetSIPListenPort();
			if (usListenPort==0)
			{
				usListenPort=5070;
			}
			usListenPort=GetRTPPort(m_rSIPCallMgr.GetSIPListenPort());
			m_sipProtoName = "UDP";
			m_rSIPCallMgr.m_nActualSIPListenPort=usListenPort;
			break;
		}
		catch (...)
		{
			if (nTryCount<10)
			{
				XSleep(100);
			}
			else
			{
				return -1;
			}
		}
	}

	int nPos=m_strSIPProxy.find(":");
	if (nPos>0)
	{
		m_strSIPProxyHost=m_strSIPProxy.substr(0,nPos);
		std::string strPort=m_strSIPProxy.substr(nPos+1,m_strSIPProxy.size()-nPos-1);
		m_usProxyPort=atoi(strPort.c_str());
	}
	else
	{
		m_strSIPProxyHost=m_strSIPProxy;
		m_usProxyPort=5060;
	}

	if (strDomainRealm=="ims.gd.chinamobile.com" || strDomainRealm=="gd.ctcims.cn")
	{
		if (strAuthUserID.find("ims.gd.chinamobile.com")>0 || strAuthUserID.find("gd.ctcims.cn")>0)
		{
		}
		else
		{
			strAuthUserID+="@";
			strAuthUserID+=strDomainRealm;
		}
	}

	char RegServer[128]={0};
	char ProxyServer[128]={0};
	char AuthorizationUser[128]={0};
	char RegUser[64]={0};
	char RegPWD[64]={0};
	char CallURI[128]={0};

	strcpy(RegUser,m_strUserID.c_str());
	strcpy(RegPWD,m_strPassword.c_str());
	strcpy(ProxyServer,m_strSIPProxy.c_str());
	strcpy(RegServer,m_strSIPProxy.c_str());

	if(m_strSIPProxyHost==strDomainRealm || strDomainRealm.size()==0)
		strDomainRealm="*";

	if(strAuthUserID.length()>0)
	{
		strcpy(AuthorizationUser,strAuthUserID.c_str());
		if (strDomainRealm.size() && strDomainRealm!="*")
			strcpy(RegServer,strDomainRealm.c_str());
	}

	char accUri[256]={0};
	char regUri[256]={0};
	int accId=-1;
	char proxy[128]={0};	
	char userName[128]={0};
	sprintf(accUri,"sip:%s@%s",RegUser,RegServer);
	//m_strLocalIP=GetLocalIPByRemoteIP(RegServer,m_usProxyPort);
	//sprintf(accUri,"sip:%s@%s",RegUser,m_strLocalIP.c_str()); //add set local contact	

	sprintf(regUri,"sip:%s",RegServer);
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		sprintf(regUri,"sip:%s;transport=tcp",RegServer);
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		sprintf(regUri,"sip:%s;transport=tls",RegServer);

	if(strlen(ProxyServer)>0)
		sprintf(proxy,"sip:%s",ProxyServer);
	else 
		strcpy(proxy,"");

	if(strDomainRealm=="*")
		strcpy(proxy,"");

	if(strlen(AuthorizationUser)>0)
		strcpy(userName,AuthorizationUser);
	else 
		strcpy(userName,RegUser);

	if(m_strUserName.length()>0)
	{
		std::string display=ANSI2UTF8(m_strUserName);
		char accUriTemp[256]={0};
		int len=display.length();
		int len2=m_strUserName.length();
		if(display.length()<128)
		{
			//sprintf(accUriTemp," \"显示名\"<%s>",accUri);
			sprintf(accUriTemp," %s<%s>",display.c_str(),accUri);
			strcpy(accUri,accUriTemp);
		}
	}

	//支持多网卡，取注册IP相关的本机IP
	std::string strDestIP="";
	if(IsCallIPPortString(m_strSIPProxyHost.c_str()) < 0 )
	{
		char dnsIP[128]={0};
		int nLen =0;
		if(GetDNSNameIP((char *)m_strSIPProxyHost.c_str(),dnsIP,nLen) ==true)
		{
			strDestIP=dnsIP;
		}
	}
	else strDestIP = m_strSIPProxyHost.c_str();

	CallMgr::Instance().GetSameToDestIP(m_strLocalIP,strDestIP);

	int nRet=SipRegisterAccount(this,accUri,regUri,(char*)(strDomainRealm.c_str()),userName,RegPWD,proxy,true);
	if(nRet >= 0) m_nAccountId=m_nLineID+1;
	m_bActive=true;
	m_bValid=true;
	return 0;

}

void SIPUser::Close(void)
{
	if(m_nAccountId > 0)
	{

		SipUnRegisterAccount(this);
		m_bValid=false;
		m_nAccountId=0;
	}
	if (m_bRegistrationSuccess)
	{
		m_bRegistrationSuccess=false;
	}
	m_bActive=false;
	m_nLocalBindPort=0;
}

SIPMediaCall* SIPUser::MakeCall(const std::string&strCalleeID,ConfInfo *pConfInfo)
{
	int nCallType=0;//use to call zhongxing 0 terminal 1 mcu 
	std::string strUserID=strCalleeID;
	std::string strSaveCalledID=strCalleeID;

	//支持多网卡，取呼叫IP相关的本机IP
	std::string strSameIP="";
	bool bGetSameIP=false;
	if(IsCallIPString(strCalleeID.c_str()))
	{
		bGetSameIP=CallMgr::Instance().GetSameToDestIP(strSameIP,(std::string &)strCalleeID);
	}

	if (m_strDomainRealm=="ims.gd.chinamobile.com" || m_strDomainRealm=="gd.ctcims.cn")
	{
		const char*cszCalleeID=strCalleeID.c_str();
		if (cszCalleeID[0]=='1')
		{
			strUserID="86"+strCalleeID;
		}
		else if (cszCalleeID[0]=='0')
		{
			cszCalleeID++;
			strUserID="86";
			strUserID+=cszCalleeID;
		}
		else
		{
			strUserID="8620"+strCalleeID;
		}
	}

	std::string strCalleeIDTemp=strCalleeID;
	//add use sip mcu
	std::string strURIUserID="";
	int nFindATPos=strCalleeIDTemp.find("@");
	if(nFindATPos>0)
	{
		nCallType=1;
		strSaveCalledID=strCalleeIDTemp.substr(0,nFindATPos);
		strURIUserID="sip:"+strUserID;
	}
	else
		strURIUserID="sip:"+strUserID+"@"+m_strSIPProxy;

	char contactTmp[256]={0};
	std::string display="";
	if(m_strUserName.length()>0)
	{
		display=ANSI2UTF8(m_strUserName);
		if(display.length()>128)
		{
			display="";
		}
	}
	std::string strLocalURI=" "+display+"<sip:"+m_strUserID+"@"+m_strSIPProxy+">";
	//add use sip mcu
	if( (nFindATPos>0) && (m_rSIPCallMgr.m_nUseInvReqLine) )
	{
		strLocalURI=" "+display+"<sip:"+m_strUserID+"@"+m_rSIPCallMgr.m_strInvReqLineAddr+">";
	}

	std::string strLocalIP= "";
	if(bGetSameIP)
	{
		strLocalIP =strSameIP;
	}
	else
	{
		CallMgr::Instance().GetOneSaveLocalIP(strLocalIP);
	}

	if(strLocalIP.size() <=0) strLocalIP=GetHostAddress();
	unsigned short nLocalAudioPort=SIPCallMgr::Instance().GetCallRTPPort();
	unsigned short nLocalVideoPort=SIPCallMgr::Instance().GetCallRTPPort();
	unsigned short nLocalDualVideoPort=0;
	unsigned short nLocalBFCPTransPort=0;

	//use to ip call
	if( (m_nAccountId == 0) && ( IsCallIPPortString(strCalleeID.c_str()) >=0 ) ) 
	{
		if(strLocalIP.size() <=0) strLocalIP=GetLocalIPByRemoteIPPort(strCalleeID);
		std::string strTemp=m_strUserID;
		if(strTemp.length()>0) strTemp=m_strUserID+"@";

		strLocalURI="sip:"+strTemp+strLocalIP;
		strURIUserID="sip:"+strTemp+strCalleeID;
		//add
		if(IsCallIPPortString(strCalleeID.c_str()) ==0 && m_rSIPCallMgr.m_nSIPListenPort !=5060 )
		{
			char strPort[32]={0};
			sprintf(strPort,":%u",m_rSIPCallMgr.m_nSIPListenPort);
			strURIUserID =strURIUserID+strPort;
		}
	}
	else strLocalIP =m_strLocalIP; //支持多网卡，使用注册IP相关的本机IP

	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strURIUserID = strURIUserID + ";transport=tcp" ;
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strURIUserID = strURIUserID + ";transport=tls" ;

	//Create SDP
	int newcallId = -1; 
	int accountId = -1;
	pj_pool_t * pool = m_pPOOL;

	pjsip_dialog * pSipdlg;
	pjsip_inv_session  *pInvSessionUac;
	//20121115 chang cheng zuan tan use
	std::string strTem="<"+strURIUserID;
	strTem=strTem+">";

	//num@dns
	int nPos=strCalleeIDTemp.find("@");
	if (nPos>0)
	{
		int nPos2=strCalleeIDTemp.find("sip:");
		if(nPos2 != std::string::npos) 
		{
			std::string strCalleeIDTemp2=strCalleeIDTemp.substr(nPos2+4,strCalleeIDTemp.size()-nPos2-4);
			strCalleeIDTemp=strCalleeIDTemp2;
		}
		strTem="<sip:"+strCalleeIDTemp;

		if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
			strTem = strTem + ";transport=tcp" ;
		if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
			strTem = strTem + ";transport=tls" ;
		strTem=strTem+">";
	}

	pj_str_t uri = pj_str((char *)strTem.c_str());
	pjsip_tx_data *tdata;
	pj_status_t status;
	std::string strTarget=strURIUserID;

	//SipViaParamAdd(strTarget,pool);
	pj_str_t target = pj_str((char *)strTarget.c_str());
	pj_str_t localUri = pj_str((char *)strLocalURI.c_str());

	unsigned	inv_option = 0;
	inv_option |= PJSIP_INV_SUPPORT_100REL;
	//inv_option |= PJSIP_INV_SUPPORT_TIMER;

	std::string  strContact="";
	GetCallContactUri(strLocalIP,strContact);
	pj_str_t contact = pj_str((char *)strContact.c_str());

	//char luri[128]={0};
	//sprintf(luri,"<sip:%s@%s>",m_strUserID.c_str(),m_strLocalIP.c_str());//add set local contact
	//localUri = pj_str(luri);

	//Create dlg
	status = pjsip_dlg_create_uac(pjsip_ua_instance(),&localUri,NULL/*&contact*/, &uri, &target, &pSipdlg);
	if(status!=PJ_SUCCESS)
	{
		m_rSIPCallMgr.OutPutStackLog(1,"SIPStack Create SIP dlg Failure.");
		return NULL;
	}

	//SipHdrViaParamAdd2(&pSipdlg->inv_hdr,pool);
	pjmedia_sdp_session *offer = NULL;
	char strId[128]={0};
	memcpy(strId,pSipdlg->call_id->id.ptr,pSipdlg->call_id->id.slen);
	std::string strCallID=strId;

	SIPMediaCall * pSIPMediaCall=new SIPMediaCall(*this,strCallID,strLocalIP,nLocalAudioPort,nLocalVideoPort,pConfInfo);
	if(! pSIPMediaCall) goto OnError;

	if(pConfInfo)
	{
		if(pConfInfo->m_DAPCallCapability.m_nSecondVideoCapCount >0) pSIPMediaCall->SetSupportH239(1);
		pSIPMediaCall->CopyVideoCapOtherParam(pConfInfo,1,1);
		//pSIPMediaCall->m_nMainVideoBitrate=pConfInfo->m_DAPCallCapability.m_nMainVideoMaxBitrate;
		//pSIPMediaCall->m_nSecondVideoMaxBitrate=pConfInfo->m_DAPCallCapability.m_nSecondVideoMaxBitrate;
	}
	int nSupportDual=pSIPMediaCall->GetMgrSupportH239();
	if(nSupportDual >0)
	{
		nLocalDualVideoPort=SIPCallMgr::Instance().GetCallRTPPort();
		nLocalBFCPTransPort=SIPCallMgr::Instance().GetCallRTPPort();
	}
	//generate sdp
	SIPCallCapability & offerSdpCap=pSIPMediaCall->GetMgrCapability();
	if(m_rSIPCallMgr.m_nDualVideoSDPMode ==1)
	{
		pSIPMediaCall->SetLocalCapabilitis(offerSdpCap);//notice
	}
	else if(m_rSIPCallMgr.m_nDualVideoSDPMode ==2)
	{
		pSIPMediaCall->SetLocalCapabilitis(offerSdpCap,true,0);//notice
	}

	m_rSIPCallMgr.SetCapabilityParam(pSIPMediaCall,offerSdpCap,nSupportDual,pSIPMediaCall->GetLocalIP(),nLocalAudioPort,nLocalVideoPort,nLocalDualVideoPort,nLocalBFCPTransPort);
	//sdp add
	if(offerSdpCap.SDP_Capabilitis_Add(pool,&offer)<0)
	{
		LogFile(NULL,NULL,"","","MakeCall SDP_Capabilitis_Add Error.");
		return NULL;
	}
	ICEEncodeInSDP(*pSIPMediaCall,pool,true,offer);
	pSIPMediaCall->CopySIPSDP(true,offer);
	//offer->media_count=3;

	//Create UAC
	status = pjsip_inv_create_uac(pSipdlg, offer, inv_option, &pInvSessionUac);
	if(status!=PJ_SUCCESS) goto OnError;
	//Set dialog Route-Set
	if(m_rSIPCallMgr.m_nOutboundProxyCount>0) 
	{
		if (!pj_list_empty(&m_rSIPCallMgr.m_RouteHdr))
			pjsip_dlg_set_route_set(pSipdlg, &m_rSIPCallMgr.m_RouteHdr);
	}

	if (m_SipCredCount) 
	{
		pjsip_auth_clt_set_credentials( &pSipdlg->auth_sess,m_SipCredCount,m_SipCredInfo);
	}
	//Make call
	status = pjsip_inv_invite(pInvSessionUac, &tdata);
	if( (status != PJ_SUCCESS) || (tdata==NULL) ) goto OnError;

	//set request line uri is addr
	if(m_rSIPCallMgr.m_nUseInvReqLine)
	{
		char * addr=(char *)m_rSIPCallMgr.m_strInvReqLineAddr.c_str();
		SetRequestLineAddr(tdata,addr,(char *)strCalleeID.c_str());
	}

	if( ( m_rSIPCallMgr.m_nSendMultipartSDP > 0 ) && (nCallType ==0 ) )  m_rSIPCallMgr.m_nSendMultipartSDP =0; //the here is use to call zhongxing terminal

	//Set MultipartSDP request message
	if( m_rSIPCallMgr.m_nSendMultipartSDP > 0 ) //use to call zhongxing mcu
	{
		pj_bool_t bMultiCreate=SIPSetMultipartSDPRequest(offer,tdata,"1");
		if(bMultiCreate == PJ_FALSE ) goto OnError;
	}

	pSIPMediaCall->OnCallRingOut();
	m_rSIPCallMgr.AddCall(pSIPMediaCall);
	status = SIPInvSendMessage(pInvSessionUac, tdata,0);
	if(status!=PJ_SUCCESS) goto OnError;


	//set SIPMediaCall info
	pSIPMediaCall->SetUserID(strUserID);
	pSIPMediaCall->SetUserName(strUserID);
	pSIPMediaCall->SetPJSIPCallID(newcallId);
	pSIPMediaCall->SetCalledID(strSaveCalledID);
	//set bfcp local info
	m_rSIPCallMgr.SetSIPCallBFCPLocalInfo(pSIPMediaCall);
	if(nSupportDual >0)
	{
		pSIPMediaCall->SetLocalDualVideoPort(nLocalDualVideoPort,nLocalDualVideoPort+1);
		pSIPMediaCall->SetLocalBFCPTransPort(nLocalBFCPTransPort);
	}
	pSIPMediaCall->SetInvSessionUAC(pInvSessionUac);
	pSIPMediaCall->SetSIPDialog(pSipdlg);
	pSIPMediaCall->SetCallDirection(SIPCall::CALL_DIR_OUT);

	PlayRingTone();
	m_rSIPCallMgr.OutPutStackLog(0,"SIPStack MakeCall OK.",(char *)pSIPMediaCall->GetCallID().c_str());
	return pSIPMediaCall;

OnError:
	if(pSIPMediaCall) 
	{
		pSIPMediaCall->Close();
		pSIPMediaCall=NULL;
	}
	m_rSIPCallMgr.OutPutStackLog(1,"SIPStack MakeCall FAIL.");
	return NULL;
}

int SIPUser::GenerateSDP(SIPCall &rSIPMediaCall,pj_pool_t *pool,std::string strLocalIP,pjmedia_sdp_session **p_sdp)
{
	int nMgrSupportDual=rSIPMediaCall.GetMgrSupportH239();
	SIPCallCapability & offerSdpCap=rSIPMediaCall.GetMgrCapability();
	rSIPMediaCall.AssignLocalMediaRTPPort(nMgrSupportDual);
	m_rSIPCallMgr.SetCapabilityParam(&rSIPMediaCall,offerSdpCap,nMgrSupportDual,strLocalIP,rSIPMediaCall.m_nLocalAudioPort,rSIPMediaCall.m_nLocalVideoPort,rSIPMediaCall.m_nLocalDualVideoPort,rSIPMediaCall.m_nLocalBFCPTransPort,rSIPMediaCall.m_nCallDir);
	rSIPMediaCall.SetLocalCapabilitis(offerSdpCap);
	if(offerSdpCap.SDP_Capabilitis_Add(pool,p_sdp)<0)
	{
		LogFile(NULL,NULL,"","","GenerateSDP SDP_Capabilitis_Add Error.");
		return -1;
	}
	return 0;
}

void SIPUser::HangupCall(SIPCall&rSIPCall)
{
}

int SIPUser::GetLineID(void)
{
	return m_nLineID;
}

bool SIPUser::IsRegistrationSuccess(void)
{
	return m_bRegistrationSuccess;
}

bool SIPUser::IsRegistrationError(void)
{
	return m_bRegistrationError;
}

const std::string&SIPUser::GetRegistrationErrorCode(void)
{
	return m_strRegistrationErrorCode;
}

SIPMediaCall* SIPUser::onNewSession(const std::string & callID,std::string strHost)
{
	std::string strCallID=callID.c_str();
	std::string strLocalIP=GetHostAddress();
	unsigned short nLocalAudioPort=SIPCallMgr::Instance().GetCallRTPPort();
	unsigned short nLocalVideoPort=SIPCallMgr::Instance().GetCallRTPPort();
	//use to ip call
	if( m_nAccountId == 0 ) 
	{
		strLocalIP=GetLocalIPByRemoteIPPort(strHost);
	}
	SIPMediaCall*pSIPMediaCall=NULL;
	pSIPMediaCall=new SIPMediaCall(*this,strCallID,strLocalIP,nLocalAudioPort,nLocalVideoPort);
	//set SIPMediaCall info
	if(pSIPMediaCall)
	{
		//set bfcp local info
		m_rSIPCallMgr.SetSIPCallBFCPLocalInfo(pSIPMediaCall,IMXEC_CALL_DIR_IN);
		m_rSIPCallMgr.OnSIPMediaCallCreated(pSIPMediaCall);
	}
	return pSIPMediaCall;
}

void SIPUser::onRegistrationError(const std::string&strErrorCode)
{
	m_nFlagAddedContact=0;
	m_bRegistrationSuccess=false;
	m_bRegistrationError=true;
	m_strRegistrationErrorCode=strErrorCode;
	m_nSIPREGFail=1;
}

void SIPUser::onRegistrationSuccess()
{
	m_bRegistrationSuccess=true;
	m_bRegistrationError=false;
	m_strRegistrationErrorCode="";
	m_nSIPREGFail=2;
	if (m_nFlagAddedContact==0)
	{
		m_nFlagAddedContact=1;
		try{
			std::string received="";
			int rport=0;
			char szRPort[128]="";
			sprintf(szRPort,"%d",rport);
			std::string strContactUri="sip:";
			strContactUri+=m_strUserID;
			strContactUri+="@";
			strContactUri+=received.c_str();
			strContactUri+=":";
			strContactUri+=szRPort;
		}
		catch (...)
		{
		}

		try{
			std::string strServer="";
			//if (m_strDomainRealm=="ims.gd.chinamobile.com" || m_strDomainRealm=="gd.ctcims.cn" || strServer.find("AVAYA")>=0)
			if (strServer.find("AVAYA")>=0)
			{
				m_bAvaya=true;
			}
			else
			{
				m_bAvaya=false;
			}
		}
		catch(...)
		{
			//if (m_strDomainRealm=="ims.gd.chinamobile.com" || m_strDomainRealm=="gd.ctcims.cn")
			//{
			//	m_bAvaya=true;
			//}
			//else
			{
				m_bAvaya=false;
			}
		}
	}
}

std::string SIPUser::GetHostAddress(void)
{
	if (m_strLocalIP != "" && m_strLocalIP != "127.0.0.1" && m_strLocalIP != "0.0.0.0")
	{
		return m_strLocalIP;
	}

	std::string strHostAddress="";
	strHostAddress=NETEC_Node::GetLocalIP();

	if(IsCallIPPortString(m_strSIPProxyHost.c_str()) >=0 )
	{
		if (strHostAddress.size()==0)
		{
			XSocketUDP tXSocketUDP;
			tXSocketUDP.Connect(m_strSIPProxyHost.c_str(),m_usProxyPort);
			strHostAddress=XSocket::htoa(tXSocketUDP.GetLocalIP());
			tXSocketUDP.Close();
		}
	}

	if (strHostAddress.size()==0) GetLocalFirstIP(strHostAddress);

	if (strHostAddress != "" && strHostAddress != "127.0.0.1" && strHostAddress != "0.0.0.0")
	{
		m_strLocalIP =strHostAddress;
	}

	return strHostAddress;
}

std::string SIPUser::GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort)
{
	if (m_strLocalIP != "" && m_strLocalIP != "127.0.0.1" && m_strLocalIP != "0.0.0.0")
	{
		return m_strLocalIP;
	}

	std::string strHostAddress="";
	strHostAddress=NETEC_Node::GetLocalIP();
	if(IsCallIPPortString(m_strSIPProxyHost.c_str()) >=0 )
	{
		if (strHostAddress.size()==0)
		{
			XSocketUDP tXSocketUDP;
			tXSocketUDP.Connect(strHost.c_str(),nPort);
			strHostAddress=XSocket::htoa(tXSocketUDP.GetLocalIP());
			tXSocketUDP.Close();
		}
	}
	if (strHostAddress.size()==0) GetLocalFirstIP(strHostAddress);

	if (strHostAddress != "" && strHostAddress != "127.0.0.1" && strHostAddress != "0.0.0.0")
	{
		m_strLocalIP =strHostAddress;
	}
	return strHostAddress;
}

std::string SIPUser::GetLocalIPByRemoteIPPort(const std::string&strHostIPPort)
{
	if (m_strLocalIP != "" && m_strLocalIP != "127.0.0.1" && m_strLocalIP != "0.0.0.0")
	{
		return m_strLocalIP;
	}

	std::string strHostAddress="";
	std::string strRemoteIP="";
	unsigned short usPort=0;
	int nPos=strHostIPPort.find(":");
	if (nPos>0)
	{
		strRemoteIP=strHostIPPort.substr(0,nPos);
		std::string strPort=strHostIPPort.substr(nPos+1,strHostIPPort.size()-nPos-1);
		usPort=atoi(strPort.c_str());
	}
	else
	{
		strRemoteIP=strHostIPPort;
		usPort=m_rSIPCallMgr.m_nSIPListenPort;
	}
	strHostAddress=GetLocalIPByRemoteIP(strRemoteIP,usPort);
	if (strHostAddress != "" && strHostAddress != "127.0.0.1" && strHostAddress != "0.0.0.0")
	{
		m_strLocalIP =strHostAddress;
	}
	return strHostAddress;
}

bool SIPUser::CompareLocalIPEqual(const std::string&strHostIPPort)
{
	bool bCompare=false;
	std::string strHostAddress="";
	std::string strRemoteIP="";
	unsigned short usPort=0;
	int nPos=strHostIPPort.find(":");
	if (nPos>0)
	{
		strRemoteIP=strHostIPPort.substr(0,nPos);
		std::string strPort=strHostIPPort.substr(nPos+1,strHostIPPort.size()-nPos-1);
		usPort=atoi(strPort.c_str());
	}
	else
	{
		strRemoteIP=strHostIPPort;
		usPort=m_rSIPCallMgr.m_nSIPListenPort;
	}
	strHostAddress=GetLocalIPByRemoteIP(strRemoteIP,usPort);
	if(strHostAddress.compare(strRemoteIP) ==0 ) bCompare=true;
	return bCompare;
}

void SIPUser::GetRegContactUri(std::string & strContact)
{
	std::string strFormat=" %s<sip:%s@%s:%d>";
	std::string strLocalIP=GetHostAddress();
	unsigned short nSipPort=m_rSIPCallMgr.GetSIPListenPort();
	if(m_nLocalBindPort >0) nSipPort=m_nLocalBindPort;
	char contactTmp[256]={0};
	std::string display="";
	if(m_strUserName.length()>0)
	{
		display=ANSI2UTF8(m_strUserName);
		if(display.length()>128)
		{
			display="";
		}
	}
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strFormat=" %s<sip:%s@%s:%d;transport=tcp>";
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strFormat=" %s<sip:%s@%s:%d;transport=tls>";
	sprintf (contactTmp, strFormat.c_str(),display.c_str(),m_strUserID.c_str(), strLocalIP.c_str(), nSipPort);
	strContact=contactTmp;
}

void SIPUser::GetCallContactUri(std::string strLocalIP,std::string & strContact)
{
	std::string strFormat=" %s<sip:%s%s:%d;transport=UDP>";
	unsigned short nSipPort=m_rSIPCallMgr.GetSIPListenPort();
	if(m_nLocalBindPort >0) nSipPort=m_nLocalBindPort;
	if(m_bHasVIA) //set mapaddr rport  //notice , if need not set via 
	{
		if(m_strViaSendByAddress.length()>0)
			strLocalIP=m_strViaSendByAddress;
		nSipPort=m_nViaRPort >0 ? m_nViaRPort : nSipPort;
	}
	char contactTmp[256]={0};
	std::string display="";
	if(m_strUserName.length()>0)
	{
		display=ANSI2UTF8(m_strUserName);
		if(display.length()>128)
		{
			display="";
		}
	}

	std::string strTemp=m_strUserID;
	if(strTemp.length()>0) 
		strTemp=m_strUserID+"@";

	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strFormat=" %s<sip:%s%s:%d;transport=tcp>";
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strFormat=" %s<sip:%s%s:%d;transport=tls>";
	sprintf (contactTmp, strFormat.c_str(),display.c_str(),strTemp.c_str(), strLocalIP.c_str(), nSipPort);
	strContact=contactTmp;
}

//Send a request or response message.
pj_status_t SIPUser::SIPInvSendMessage( pjsip_inv_session *inv,pjsip_tx_data *tdata,int type) //type 0 request ,1 response
{
	pj_status_t status;
	if(m_nAccountId > 0 && type==0 )
	{
		//SipHdrViaParamLocalAddFirst(tdata->msg,m_pPOOL);
		//SipHdrViaParamLocalAdd(tdata->msg,m_pPOOL/*,m_rSIPCallMgr.m_nSIPListenPort*/); //set
	}
	SIPMessageAddUserAgentCapability(tdata);
	//SIPMessageAddDateCapability(tdata);
	//send message
	status = pjsip_inv_send_msg(inv, tdata);
	return status;
}

//Add User-Agent header
void SIPUser::SIPMessageAddUserAgentCapability(pjsip_tx_data *tdata)
{
	pjsip_generic_string_hdr * ua_hdr =NULL;
	const pj_str_t STR_UserAgent = { "User-Agent", 10};
	pjsip_msg * msg = tdata->msg;
	ua_hdr = (pjsip_generic_string_hdr*)pjsip_msg_find_hdr_by_name(msg, &STR_UserAgent, NULL); //PJSIP_H_USER_AGENT_UNIMP

	//Add User-Agent header
	if(strlen(m_rSIPCallMgr.m_strUserAgentName) > 0 && ua_hdr==NULL)
	{
		SIPHeaderAddCapability(tdata,"User-Agent",m_rSIPCallMgr.m_strUserAgentName);

	}
}

//Add Date header
void SIPUser::SIPMessageAddDateCapability(pjsip_tx_data *tdata)
{
	if(m_rSIPCallMgr.m_strDateTime.length()>0)
	{
		SIPHeaderAddCapability(tdata,"Date",(char *)m_rSIPCallMgr.m_strDateTime.c_str());
	}
}

//Add Require header
void SIPUser::SIPMessageAddRequireCapability(pjsip_tx_data *tdata)
{
	//Add Require header Require: 100rel
	SIPHeaderAddCapability(tdata,"Require","100rel");
}

//Add name value Capability header
void SIPUser::SIPHeaderAddCapability(pjsip_tx_data *tdata,char * pName,char * pValue)
{
	pj_str_t pjName=pj_str(pName);
	pj_str_t pjValue=pj_str(pValue);
	pjsip_hdr *h;
	h = (pjsip_hdr*) pjsip_generic_string_hdr_create(tdata->pool,&pjName,&pjValue);
	pjsip_msg_add_hdr(tdata->msg, h);
}

//if more one via add do:add via header
void SIPUser::SipHdrViaParamAdd(std::string & strTarget,pj_pool_t * pool)
{
	unsigned int & viaNum=m_SIPVia.m_nCapNum;
	if(viaNum>0)
	{
		char buf[2048]={0};
		size_t buflen=2048;
		int len1=0;
		int len2=0;
		len1=strlen("Via: ");
		len2=strlen("sip:localhost");
		pjsip_sip_uri *url = pjsip_sip_uri_create(pool, 0);
		pj_strdup2(pool, &url->host, "localhost");
		for(int cnt=0;cnt<viaNum;cnt++)
		{
			pjsip_via_hdr * via=m_SIPVia.m_Caps[cnt];
			char buf1[256]={0};
			size_t buflen1=256;
			pj_str_t *via_addr=NULL;
			int rport=0;
			GetSIPViaRportViaAddr(via, rport,&via_addr);
			if((rport>0) && (via_addr!=NULL) )
			{
				via->sent_by.host=*via_addr;
				via->sent_by.port=rport;
				via->rport_param=0;
			}
			GenerateVIABranchParam(via,pool);
			pjsip_hdr_print_on(via,buf1,buflen1);
			sip_param_add(url->header_param, "Via",buf1+len1);
		}
		pjsip_uri_print(PJSIP_URI_IN_OTHER,url,buf,buflen);
		strTarget=strTarget+(buf+len2);
	}
}

//if more one via add do:add via header 2 notice :pool not release
void SIPUser::SipHdrViaParamAdd2(pjsip_msg *msg,pj_pool_t * pool)
{
	unsigned int & viaNum2=m_SIPVia.m_nCapNum;
	for(int cnt=0;cnt<viaNum2;cnt++)
	{
		pjsip_via_hdr * via=m_SIPVia.m_Caps[cnt];
		pj_str_t *via_addr=NULL;
		int rport=0;
		GetSIPViaRportViaAddr(via, rport,&via_addr);
		if((rport>0) && (via_addr!=NULL) )
		{
			via->sent_by.host=*via_addr;
			via->sent_by.port=rport;
			via->rport_param=0;
		}
		GenerateVIABranchParam(via,pool);
		pjsip_via_hdr *new_via;
		new_via = (pjsip_via_hdr*)pjsip_hdr_clone(pool, via);
		if(new_via)
		{
			pjsip_msg_add_hdr(msg, (pjsip_hdr*)new_via);
			//pj_list_push_front(&hdr, new_via);
			//pj_list_push_back(&hdr, new_via);
		}
	}
}

void SIPUser::GetSIPViaRportViaAddr(pjsip_via_hdr * via, int & rport,pj_str_t * * via_addr,pjsip_transport *tp)
{
	if(via==NULL) return;
	if (via->rport_param < 1) 
	{
		//Remote doesn't support rport
		rport = via->sent_by.port;
		if( (rport==0) && (tp!=NULL) ) 
		{
			pjsip_transport_type_e tp_type;
			tp_type = (pjsip_transport_type_e) tp->key.type;
			rport = pjsip_transport_get_default_port_for_type(tp_type);
		}
	} 
	else
		rport = via->rport_param;

	if (via->recvd_param.slen > 0)
		* via_addr = &via->recvd_param;
	else
		* via_addr = &via->sent_by.host;
}

void SIPUser::SipHdrSubjectParamAdd(std::string & strTarget,pj_pool_t * pool,char * pTitleValue)
{
	char buf[2048]={0};
	size_t buflen=2048;
	int len1=0;
	int len2=0;
	len2=strlen("sip:localhost");
	pjsip_sip_uri *url = pjsip_sip_uri_create(pool, 0);
	pj_strdup2(pool, &url->host, "localhost");
	sip_param_add(url->header_param, "Subject",pTitleValue);
	pjsip_uri_print(PJSIP_URI_IN_OTHER,url,buf,buflen);
	strTarget=strTarget+(buf+len2);
}

void SIPUser::ICEEncodeInSDP(SIPCall & pSipCall,pj_pool_t * pool,bool bOffer,pjmedia_sdp_session * localSdp,pjmedia_sdp_session * remSdp)
{
	//add ice use ,add ice attr to sdp
	bool bUseICE=m_rSIPCallMgr.m_bICEEnable;
	int nCmpCount=m_rSIPCallMgr.m_nICETransportCount;
	if(bUseICE)
	{
		int nMediaIndex=0;
		AudioRTPSession* pAudioRTPSession=pSipCall.GetOpenAudioRTPSession();
		if(bOffer)
			pAudioRTPSession->PJSIPEncodeInOfferSDP(pool,nMediaIndex,localSdp,nCmpCount);
		else
			pAudioRTPSession->PJSIPEncodeInAnswerSDP(pool,nMediaIndex,localSdp,remSdp,nCmpCount);
		if (CallMgr::Instance().GetAudioOnly()==0)
		{
			nMediaIndex++;
			VideoRTPSession* pVideoRTPSession=pSipCall.GetOpenVideoRTPSession();
			if(bOffer)
				pVideoRTPSession->PJSIPEncodeInOfferSDP(pool,nMediaIndex,localSdp,nCmpCount);
			else
				pVideoRTPSession->PJSIPEncodeInAnswerSDP(pool,nMediaIndex,localSdp,remSdp,nCmpCount);
		}
		if(pSipCall.GetMgrSupportH239())
		{
			nMediaIndex++;
			VideoRTPSession* pVideoRTPSession=pSipCall.GetOpenDualVideoRTPSession();
			if(bOffer)
				pVideoRTPSession->PJSIPEncodeInOfferSDP(pool,nMediaIndex,localSdp,nCmpCount);
			else
				pVideoRTPSession->PJSIPEncodeInAnswerSDP(pool,nMediaIndex,localSdp,remSdp,nCmpCount);
		}
	}
}

void SIPUser::SipHdrViaParamLocalAdd(pjsip_msg *msg,pj_pool_t * pool)
{
	pjsip_via_hdr * via= pjsip_via_hdr_create(pool);
	pj_str_t type=pj_str("UDP");
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		type=pj_str("TCP");
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		type=pj_str("TLS");
	if(via)
	{
		std::string strLocalIP=GetHostAddress();
		pj_strdup(pool, &via->transport,&type);
		pj_strdup2(pool, &via->sent_by.host,(char *)strLocalIP.c_str());
		via->sent_by.port=m_rSIPCallMgr.m_nSIPListenPort;
		if(m_nLocalBindPort >0) via->sent_by.port=m_nLocalBindPort;
		via->rport_param=0;
		GenerateVIABranchParam(via,pool);
		pjsip_msg_add_hdr(msg, (pjsip_hdr*)via);
		//pjsip_msg_insert_first_hdr(msg, (pjsip_hdr*)via);
	}
}

//add the add local via ,notice via branch copy
void SIPUser::SipHdrViaParamLocalAddFirst(pjsip_msg *msg,pj_pool_t * pool,int nLocalPort)
{
	char branch[128]={0};
	int  nBranchLen=0;
	pjsip_via_hdr * via0=(pjsip_via_hdr *)pjsip_msg_find_hdr(msg,PJSIP_H_VIA,NULL);
	if(via0)
	{
		nBranchLen=via0->branch_param.slen;
		memcpy(branch,via0->branch_param.ptr,nBranchLen);
		via0=(pjsip_via_hdr *)pjsip_msg_find_remove_hdr(msg,PJSIP_H_VIA,NULL);
	}

	pjsip_via_hdr * via= pjsip_via_hdr_create(pool);
	pj_str_t type=pj_str("UDP");
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		type=pj_str("TCP");
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		type=pj_str("TLS");
	if(via)
	{
		std::string strLocalIP=GetHostAddress();
		pj_strdup(pool, &via->transport,&type);
		pj_strdup2(pool, &via->sent_by.host,(char *)strLocalIP.c_str());
		via->sent_by.port=m_rSIPCallMgr.m_nSIPListenPort;
		if(nLocalPort>0) via->sent_by.port=nLocalPort;
		//via->sent_by.port=pj_strtoul(&pjStrPort);
		via->rport_param=0;
		GenerateVIABranchParam(via,pool);
		//notice via branch copy ,if not copy can not find tsx , tsx = (pjsip_transaction*)pj_hash_get regc_refresh_timer_cb
		if(nBranchLen >0) memcpy(via->branch_param.ptr,branch,via->branch_param.slen);
		pjsip_msg_insert_first_hdr(msg, (pjsip_hdr*)via);
	}
}

void SIPUser::GenerateVIABranchParam(pjsip_via_hdr * via,pj_pool_t * pool)
{
	//generate branch_param
	pj_str_t tmp;
	via->branch_param.ptr = (char*)pj_pool_alloc(pool, PJSIP_MAX_BRANCH_LEN);
	via->branch_param.slen = PJSIP_MAX_BRANCH_LEN;
	pj_memcpy(via->branch_param.ptr, PJSIP_RFC3261_BRANCH_ID,PJSIP_RFC3261_BRANCH_LEN);
	tmp.ptr = via->branch_param.ptr + PJSIP_RFC3261_BRANCH_LEN + 2;
	*(tmp.ptr-2) = 80;
	*(tmp.ptr-1) = 106;
	pj_generate_unique_string( &tmp );
}

char * GetRingTonePath()
{
	static char strGetPath[MAX_PATH_LEN]={0};
	static BOOL bGetFlag = FALSE;
	if(!bGetFlag)
	{
		unsigned long dwLen = ::GetModuleFileName(NULL, strGetPath, MAX_PATH_LEN);
		if(dwLen)
		{
			char * p = strGetPath + dwLen;
			while(p>strGetPath && *p!=('\\'))
				p--;
			if(*p == ('\\')) strcpy(p, ("\\SOUNDS\\siptone.wav"));
		}
		else strcpy(strGetPath, (".\\SOUNDS\\siptone.wav"));

		bGetFlag = TRUE;
	}
	return strGetPath;
}

void SIPUser::PlayRingTone()
{
	char * path=GetRingTonePath();
	sndPlaySound(path, SND_ASYNC | SND_LOOP);
}

void SIPUser::CloseRingTone()
{
	sndPlaySound(NULL,NULL);
}

void SIPUser::SetRequestLineAddr(pjsip_tx_data *tdata,char * addr,char * calleeid)
{
	int addrLen=strlen(addr);
	if( (addrLen<=0) || (addrLen>127) ) return;
	char uriReq[128]={0};
	std::string strFormat="";
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strFormat=";transport=tcp";
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strFormat=";transport=tls";

	if(calleeid!=NULL)
		sprintf(uriReq,"sip:%s@%s%s",calleeid,addr,strFormat.c_str());
	else
		sprintf(uriReq,"sip:%s%s",addr,strFormat.c_str());

	//add use sip mcu
	std::string strCalleeid="";
	if(calleeid!=NULL)
	{
		strCalleeid=calleeid;
		int nPos=strCalleeid.find("@");
		if (nPos>0)
		{
			sprintf(uriReq,"sip:%s%s",calleeid,strFormat.c_str());
		}
	}

	pj_str_t tmp;
	pj_str_t param_target=pj_str(uriReq);
	pjsip_uri *target;
	pj_strdup_with_null(tdata->pool, &tmp, &param_target);
	target = pjsip_parse_uri( tdata->pool, tmp.ptr, tmp.slen, 0);
	tdata->msg->line.req.uri =target;
}

//Set MultipartSDP request message.
pj_bool_t SIPUser::SIPSetMultipartSDPRequest(pjmedia_sdp_session * sdp,pjsip_tx_data *tdata,char * pwd,char * mimeType)
{
	pj_str_t mime_type;
	pjsip_media_type media_type;
	pj_str_t body;
	const pj_str_t mime_text_plain = pj_str("multipart/mixed;boundary=\"boundary1\"");

	if(mimeType != NULL) mime_type=pj_str(mimeType);
	if( (mimeType == NULL) || (mime_type.slen) <= 0)
	{
		mime_type = mime_text_plain;
	}

	pjsip_parse_media_type(tdata->pool, &mime_type, &media_type);
	char sdpBuf[4096]={0};
	int nSdpBufLen=4096;
	char multiBuf[4096]={0};
	int nMultiBufLen=4096;
	char msg[512]={0};

	sprintf(msg,
		"\r\n--boundary1\r\n"
		"Content-Type:text/plain\r\n\r\n"
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
		"<auth>\r\n"
		"<password>%s</password>\r\n"
		"</auth>\r\n"
		"\r\n--boundary1--\r\n"
		,pwd);

	if(sdp!=NULL)
	{
		pjmedia_sdp_print(sdp,sdpBuf,nSdpBufLen);
		sprintf(multiBuf,"--boundary1\r\nContent-Type:application/sdp\r\n\r\n%s%s",sdpBuf,msg);
	}
	pj_str_t boundaryContent=pj_str(multiBuf);
	//pjsip_multipart_create
	pj_strdup_with_null(tdata->pool, &body, &boundaryContent);
	//Add message body
	tdata->msg->body = pjsip_msg_body_create(tdata->pool, &media_type.type,&media_type.subtype,&body);
	if (tdata->msg->body == NULL) return PJ_FALSE;
	SIPHeaderAddCapability(tdata,"Server","conf-serv/3GPP");
	SIPHeaderAddCapability(tdata,"P-Preferred-Service","urn:3gpp:mmtel.conf.version1");
	return PJ_TRUE;
}

void SIPUser::SIPRegCheckStatus(unsigned long nTimestamp)
{
	if(m_bSendReg)
	{
		if (m_nSIPCheckLastTimestamp > nTimestamp)
		{
			m_nSIPCheckLastTimestamp=nTimestamp;
		}
		else if (nTimestamp - m_nSIPCheckLastTimestamp >= 30 * 1000)
		{
			m_nSIPCheckLastTimestamp=nTimestamp;
			std::string strErrorCode="SIP Registration Error.";
			m_rSIPCallMgr.OnRegistrationError(m_nLineID,strErrorCode,m_strUserID.c_str());
			if(m_nRegSetting == 2) //timer reg
			{
				if (m_nSIPRegFailLastTimestamp > nTimestamp)
				{
					m_nSIPRegFailLastTimestamp=nTimestamp;
				}
				else if (nTimestamp - m_nSIPRegFailLastTimestamp >= m_rSIPCallMgr.m_nSIPRegFailTimerTimestamp)
				{
					Open();
					m_nSIPRegFailLastTimestamp=nTimestamp;
				}
			}
			m_nSIPRegFailLastTimestamp=nTimestamp;
		}
		m_bSendReg =false;
	}

	if(m_bTimeOutReg && m_nRegSetting == 2)
	{
		if (m_nSIPRegFailLastTimestamp > nTimestamp)
		{
			m_nSIPRegFailLastTimestamp=nTimestamp;
		}
		else if (nTimestamp - m_nSIPRegFailLastTimestamp >= m_rSIPCallMgr.m_nSIPRegFailTimerTimestamp /12 ) //10s
		{
			//Close();
			//int nMaxWait = 50;
			//if(m_bValid) sip_busy_sleep(nMaxWait);
			Open();
			m_nSIPRegFailLastTimestamp=nTimestamp;
			m_bTimeOutReg=false;
		}
	}
}

//add by sgb 20140709
void SIPUser::GetCallContactUriUas( std::string strLocalIP,std::string & strContact, pjsip_rx_data *rdata )
{
	std::string strFormat;
	if( !rdata )
	{
		strFormat=" %s<sip:%s%s:%d;transport=udp>";
	}
	else
	{
		if( !memicmp( rdata->tp_info.transport->type_name, "tcp", strlen("tcp") ) )
		{
			strFormat=" %s<sip:%s%s:%d;transport=tcp>";
		}
		else if( !memicmp( rdata->tp_info.transport->type_name, "UDP", strlen("UDP") ) )
		{
			strFormat=" %s<sip:%s%s:%d;transport=udp>";
		}
		else
		{
			strFormat=" %s<sip:%s%s:%d;transport=tls>";
		}
	}

	unsigned short nSipPort=m_rSIPCallMgr.GetSIPListenPort();
	if(m_bHasVIA)
	{
		if(m_strViaSendByAddress.length()>0)
			strLocalIP=m_strViaSendByAddress;
		nSipPort=m_nViaRPort >0 ? m_nViaRPort : nSipPort;
	}
	char contactTmp[256]={0};
	std::string display="";
	if(m_strUserName.length()>0)
	{
		display=ANSI2UTF8(m_strUserName);
		if(display.length()>128)
		{
			display="";
		}
	}

	std::string strTemp=m_strUserID;
	if(strTemp.length()>0) 
		strTemp=m_strUserID+"@";

	sprintf (contactTmp, strFormat.c_str(),display.c_str(),strTemp.c_str(), strLocalIP.c_str(), nSipPort);
	strContact=contactTmp;
}
//add end


void SIPUser::SetLocalBindPort(int nLocalPort)
{
	m_nLocalBindPort=nLocalPort;
}

//add the add local Contact use send request add
void SIPUser::SipHdrContactParamLocalAddFirst(pjsip_msg *msg,pj_pool_t * pool,int nLocalPort)
{
	std::string strFormat=" %s<sip:%s@%s:%d>";
	std::string strLocalIP=GetHostAddress();
	unsigned short nSipPort=m_rSIPCallMgr.GetSIPListenPort();
	if(nLocalPort >0) nSipPort=nLocalPort;
	char contactTmp[256]={0};
	std::string display="";
	if(m_strUserName.length()>0)
	{
		display=ANSI2UTF8(m_strUserName);
		if(display.length()>128)
		{
			display="";
		}
	}
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TCP)
		strFormat=" %s<sip:%s@%s:%d;transport=tcp>";
	if(m_rSIPCallMgr.GetSIPTransportType() ==IMXEC_SIP::TRANS_TLS)
		strFormat=" %s<sip:%s@%s:%d;transport=tls>";
	sprintf(contactTmp, strFormat.c_str(),display.c_str(),m_strUserID.c_str(), strLocalIP.c_str(), nSipPort);
	bool bRinstanceAdd=false;  //test


	//remove old contact
	pjsip_contact_hdr * contact0=(pjsip_contact_hdr*) pjsip_msg_find_hdr(msg, PJSIP_H_CONTACT,NULL);
	if(contact0)
	{
		pjsip_msg_find_remove_hdr(msg,PJSIP_H_CONTACT,NULL);
	}
	//add new contact local contact
	const pj_str_t CONTACT = { "Contact", 7 };
	int cnt=1;
	for (int i=0; i<cnt; ++i)
	{
		pjsip_contact_hdr *hdr;
		pj_str_t tmp;
		pj_strdup2_with_null(pool, &tmp, contactTmp);
		hdr = (pjsip_contact_hdr*)pjsip_parse_hdr(pool, &CONTACT, tmp.ptr, tmp.slen, NULL);
		if(hdr == NULL && contact0)
		{
			hdr=contact0;
		}

		//if(bRinstanceAdd)
		//{
		//	int nParamLen=strlen(contactTmp);
		//	pjsip_param *p = PJ_POOL_ALLOC_T(pool, pjsip_param);
		//	pj_strdup2(pool,&p->name,"rinstance");
		//	pj_strdup2(pool,&p->value,"4cc372a25422caf9");
		//	pjsip_sip_uri *sip_uri=NULL;
		//	sip_uri = (pjsip_sip_uri*)pjsip_uri_get_uri(hdr->uri);
		//	pj_list_insert_after(&sip_uri->other_param, p);
		//}

		pjsip_msg_add_hdr(msg, (pjsip_hdr*)hdr);
		//pjsip_msg_insert_first_hdr(msg, (pjsip_hdr*)hdr);
		//SIPHeaderAddCapability(tdata,"Contact",contactTmp);
		//pjsip_msg_insert_first_hdr(msg, (pjsip_hdr*)hdr);
	}

}
