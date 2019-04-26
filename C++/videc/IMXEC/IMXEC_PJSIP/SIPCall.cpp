#include "stdafx.h"
#include "SIPCall.h"
#include "SIPUser.h"
#include "XSocket.h"
#include "RTPHeader.h"
#include "SIPCallMgr.h"
#include "../IMXEC_Util/XUtil.h"
#include "CallMgr.h"

extern unsigned short GetTCPPort(void);
extern unsigned short GetRTPPort(unsigned short usRTPBasePort);
//notice test
extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);
extern VIDEO_FORMAT_TYPE GetVideoFormatType(int nVideoWidth,int nVideoHeight);
extern void GetVideoFormatType(VIDEO_FORMAT_TYPE nFormatType,int & w,int & h);
extern void DebugOutput(int nType,char *szFormat, ...);

// BFCP Request Status
enum bfcp_rstat {
	BFCP_PENDING   = 1,
	BFCP_ACCEPTED  = 2,
	BFCP_GRANTED   = 3,
	BFCP_DENIED    = 4,
	BFCP_CANCELLED = 5,
	BFCP_RELEASED  = 6,
	BFCP_REVOKED   = 7
};

static const pj_str_t STR_AUDIO = { "audio", 5};
static const pj_str_t STR_VIDEO = { "video", 5};
static const pj_str_t STR_APP = { "application", 11};

SIPCall::SIPCall(SIPUser&rSIPUser,const std::string&strCallID,const std::string&strLocalIP,unsigned short nLocalAudioPort,unsigned short nLocalVideoPort,ConfInfo*pConfInfo)
:m_rSIPUser(rSIPUser)
,m_strLocalIP(strLocalIP)
,m_nLocalAudioPort(nLocalAudioPort)
,m_nLocalVideoPort(nLocalVideoPort)
,m_strCallID(strCallID)
,m_CallDirection(CALL_DIR_OUT)
,m_CallStatus(CALL_STATUS_RING)
,m_pVideoRTPSession(NULL)
,m_pAudioRTPSession(NULL)
,m_nLastSentAudioRTPTimestamp(0)
{
	m_nCapDirectionDefault =3;
	m_bRemoteIsMaster=true;

	m_pDualVideoRTPSession=NULL;
	m_bHasRemoteH239Video=false;
	m_bH239ReceiverOpen=false;
	memset(m_strRemoteIP,0,64);
	m_nDTMFTimestamp=0x3200;
	m_nDTMFSeqNo=0;
	m_nDTMFSSRC=(unsigned long)this;
	m_nRemoteAudioReliableRTP=0;
	m_nRemoteVideoReliableRTP=0;
	m_nAudioReliableRTP=1;
	m_nVideoReliableRTP=1;
	m_nMainVideoPacketTimestamp=0;
	m_nSecondVideoPacketTimestamp=0;
	m_nUserIDIndex=0;
	m_nRemoteDualVideoReliableRTP=0;
	m_nDualVideoReliableRTP=1;
	m_nRemoteDataReliableRTP=0;
	m_nDataReliableRTP=1;
	m_BfcpFloorControlType=e_Floor_Control_None;
	m_BfcpSetupType=e_BFCP_Setup_None;
	m_nBFCPTransportType=e_BFCP_Transport_None;
	m_nBFCPConfId=0;
	m_nBFCPUserId=0;
	m_nBFCPFloorId=0;
	m_nBFCPMediaLabel=0;
	m_nDualVideoLabel=0;
	m_nBFCPTransportPort=0;
	m_nLocalDualVideoPort=0;
	m_nLocalBFCPTransPort=0;
	m_nLocalDualVideoRtcpPort=0;
	m_pBFCPUDP=NULL;
	m_pBFCPTCPClient=NULL;
	m_pBFCPTCPServer=NULL;
	m_pBFCPTCP=NULL;
	m_pBFCPPJSIPUDP=NULL;
	m_nDualVideoDevID=0;
	m_nDualVideoFrameRate=5;
	m_strRemoteLastIP="";
	m_nBFCPTransportPort=0;
	m_nBFCPLastTransportPort=0;

	m_nRemoteVideoPort=0;
	m_nRemoteVideoRTCPPort=0;
	m_nRemoteAudioPort=0;
	m_nRemoteAudioRTCPPort=0;
	m_nRemoteDualVideoPort=0;
	m_nRemoteDualVideoRTCPPort=0;

	m_bAccept=false;
	m_bHunguped=false;
	m_pInvSessionUAC=NULL;
	m_pInvSessionUAS=NULL;
	m_pSipDialog=NULL;
	m_pCallLocalSDP = NULL;
	m_pCallRemoteSDP = NULL;
	m_pAudioTransport = NULL;
	m_pVideoTransport = NULL;
	m_pDualVideoTransport = NULL;
	m_nAudioOpenState = 0;
	m_nVideoOpenState = 0;
	m_nDualVideoOpenState = 0;
	m_nMediaCount=0;
	m_pPOOL=NULL;
	m_pPOOL = pjsip_endpt_create_pool(pjsip_ua_get_endpt(pjsip_ua_instance()), "SIPCall%p", 2048,256 );

	m_bNegCapAudio=false;
	m_bNegCapVideo=false;
	m_bNegCapDualVideo=false;
	m_bNegCapData=false;
	m_bSendUpdateSDP=false;
	m_pSend180Data=NULL;
	m_pSend200Data=NULL;
	m_nInviteFindSDP=0;
	m_nRecvDV=0;
	m_nRSeq=1;
	m_nRecvWithdraw=0;
	m_strCalledID="";
	m_nSupportH239=0;

	m_nMainVideoBitrate=384;
	m_nSecondVideoMaxBitrate=384;
	m_nLocalMainVideoH264Level=0;
	m_nLocalSecondVideoH264Level=0;
	m_nLocalMainVideoH264HPLevel=0;
	m_nLocalSecondVideoH264HPLevel=0;
	m_nMainVideoMaxFrame=30;
	m_nSecondVideoMaxFrame=16;
	m_bAudioCapItemSetFirst=false;
	m_bVideoCapItemSetFirst=false;
	m_bDualVideoCapItemSetFirst=false;

	m_nMainVideoFormatType=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFormatType=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH263=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH264=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH264HP=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH263=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH264=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH264HP=VIDEO_FORMAT_QCIF;

	if (pConfInfo)
	{
		memcpy(&m_ConfInfo,pConfInfo,sizeof(ConfInfo));
		ConfCapabilitisDefault();
	}
	m_nSupportH239=0;
	m_strMediaMulticastIP ="";
	m_bMediaMulticastEnable=false;
	m_nMediaMulticastTTL=0;

	m_nMainVideoPayloadType=-1;
	m_strMainVideoPayloadName="";
	m_nMainVideoNegBitrate=m_nMainVideoBitrate;
	m_nMainVideoWidth=352;
	m_nMainVideoHeight=288;
	m_nSendDTMFRTP =0;

	m_nTransactionId =0;
	m_nUserId =0;
	m_nFloorId =0;
	m_bCallDestroy=false;
	m_bCallState =false;
	m_nLocalHungUP=0;
	m_nCallDir=IMXEC_CALL_DIR_UNKOWN;
	m_nCallNum =0;
	m_bHasRemoteDualSDP=false;
	m_bSendSDPAll=true;
	m_nMainChannelState=0;
	m_nDualChannelState=0;

	m_strAudioPayloadName="";
	m_nAudioPayloadType=-1;
	m_nAudioSampleRate=8000;
	m_AudioCodecType = X_AUDIO_CODEC_G711U;
	m_DataCodecID =DATA_CODEC_H224;
	m_nDataPayloadType =-1;
	m_nMainVideoCodecType =VIDEC_CODEC_COUNT;
	m_nDualVideoCodecType =VIDEC_CODEC_COUNT;
	m_nMainVideoFormat =VIDEO_FORMAT_QCIF;
	m_nDualVideoFormat =VIDEO_FORMAT_QCIF;
	m_nAudioClockRate =0;
	//notice ,win7 original init value is 0 or certainly value ,winxp is not certainly value
	//DebugOutput(2,"SIPCall init m_nSecondVideoDir:%d\n",m_nSecondVideoDir);
	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;

}

SIPCall::~SIPCall(void)
{
	if(m_pPOOL)
	{
		sip_register_thread();
		pj_pool_release(m_pPOOL);
		m_pPOOL=NULL;
	}
}

void SIPCall::Close(void)
{
	m_bCallState =false;
	if(m_pAudioRTPSession) m_pAudioRTPSession->PacketRecvPause();
	if(m_pVideoRTPSession) m_pVideoRTPSession->PacketRecvPause();
	if(m_pDualVideoRTPSession) m_pDualVideoRTPSession->PacketRecvPause();

	AudioRTPSession*pAudioRTPSession=m_pAudioRTPSession;
	{
		//XAutoLock l(m_csAudioRTPSession);
		m_pAudioRTPSession=NULL;
	}
	if (pAudioRTPSession!=NULL)
	{
		pAudioRTPSession->Close();
		delete pAudioRTPSession;
		pAudioRTPSession=NULL;
		//port save
		SIPCallMgr::Instance().SaveMediaReleasePort(m_nLocalAudioPort);
	}

	VideoRTPSession*pVideoRTPSession=m_pVideoRTPSession;
	{
		//XAutoLock l(m_csVideoRTPSession);
		m_pVideoRTPSession=NULL;
	}
	if (pVideoRTPSession!=NULL)
	{
		pVideoRTPSession->Close();
		delete pVideoRTPSession;
		pVideoRTPSession=NULL;
		//port save
		SIPCallMgr::Instance().SaveMediaReleasePort(m_nLocalVideoPort);
	}
	OnDualVideoRTPClose();
	//port save
	if(GetMgrSupportH239() >0)
	{
		SIPCallMgr::Instance().SaveMediaReleasePort(m_nLocalDualVideoPort);
		CloseBFCPTransports();
		SIPCallMgr::Instance().SaveMediaReleasePort(m_nLocalBFCPTransPort);
	}

}

int SIPCall::AcceptCall(ConfInfo *pConfInfo)
{
	int nRet=0;
	pjmedia_sdp_session *sdp = NULL;
	pj_pool_t * pool=m_pPOOL;

	if(pConfInfo)
	{
		if(pConfInfo->m_DAPCallCapability.m_nSecondVideoCapCount >0) SetSupportH239(1);
		//m_nMainVideoBitrate=pConfInfo->m_DAPCallCapability.m_nMainVideoMaxBitrate;
		//m_nSecondVideoMaxBitrate=pConfInfo->m_DAPCallCapability.m_nSecondVideoMaxBitrate;
		memcpy(&m_ConfInfo,pConfInfo,sizeof(ConfInfo));
		ConfCapabilitisDefault();
		CopyVideoCapOtherParam(pConfInfo,1,1);
	}
	int nMgrSupportDual=GetMgrSupportH239();
	AssignLocalMediaRTPPort(nMgrSupportDual);

	sip_register_thread();
	if(m_nInviteFindSDP <= 0)
	{
		if(GenerateSDP(&sdp) >= 0 ) SIPSendUpdateRequest(sdp);
		return 0;
	}
	else
	{
		if(OnOffer(&sdp) <0 ) 
		{
			//SDP Negotiation has failed  Disconnect call  response 488 sdp
			NotAcceptCall(m_nCallId);
			sdp = NULL;
			PJ_LOG(3,("SIPCall","AcceptCall func call NotAcceptCall after."));
			char szCallIDBack[128]={0}; strcpy(szCallIDBack,m_strCallID.c_str());
			SIPCallMgr::Instance().OutPutStackLog(1,"SIPStack NotAcceptCall,SDP Negotiation Failure.",szCallIDBack);
			return 0;
		}
	}

	m_rSIPUser.ICEEncodeInSDP(*this,pool,false,sdp,m_pCallRemoteSDP);
	CopySIPSDP(true,sdp);
	pjsip_inv_set_sdp_answer(m_pInvSessionUAS, sdp);

	try
	{
		pj_bool_t ret=PJ_FALSE;
		//Answer call
		pjsip_inv_session *inv=m_pInvSessionUAS;
		pjsip_tx_data *tdata=NULL;
		pj_status_t status;
		if(inv==NULL) return -1;

		if( nMgrSupportDual >0 && !m_bHasRemoteDualSDP && m_bSendSDPAll && SIPCallMgr::Instance().m_nDualVideoSDPMode ==2)
		{
			m_bSendSDPAll=false;
			if(CreateLocalSDP(m_pPOOL,&sdp,nMgrSupportDual) == false) return -1;
		}

		status = pjsip_inv_answer(inv, 200, NULL, NULL/*sdp*/, &tdata);
		if( (status != PJ_SUCCESS) || (tdata==NULL) ) return -1;

		//Not use stack neg sdp,build new offer SDP Attach
		if(sdp)
		{
			pjmedia_sdp_session *sdp_copy = sdp;
			pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
		}

		status = m_rSIPUser.SIPInvSendMessage(inv, tdata);
		if(status != PJ_SUCCESS) return -1;
		//media update
		MediaOpenSetState();
		nRet=0;
	}
	catch(...)
	{
		nRet=-1;
	};
	char szCallIDBack[128]={0}; strcpy(szCallIDBack,m_strCallID.c_str());
	SIPCallMgr::Instance().OutPutStackLog(0,"SIPStack AcceptCall OK.",szCallIDBack);
	return nRet;
}


int SIPCall::HangupCall(void)
{
	CloseBFCPTransports();
	return 0;
}

int SIPCall::GetLocalAudioCodecType(bool first,X_AUDIO_CODEC_ID type)
{
	int ret=0;

	SIPCallCapability &sdpCap=GetMgrCapability();

	if(first) 
	{
		switch(type)
		{
		case X_AUDIO_CODEC_GSM:
			ret=sdpCap.IsFirstGSM();				
			break;
		case X_AUDIO_CODEC_G7231_63:
			ret=sdpCap.IsFirstG7231();				
			break;
		case X_AUDIO_CODEC_G711U:
			ret=sdpCap.IsFirstG711u_64K();				
			break;
		case X_AUDIO_CODEC_G711A:
			ret=sdpCap.IsFirstG711a_64K();				
			break;
		case X_AUDIO_CODEC_G729:
			ret=sdpCap.IsFirstG729A();				
			break;
		case X_AUDIO_CODEC_G722_64:
			ret=sdpCap.IsFirstG722_64K();				
			break;
		case X_AUDIO_CODEC_G7221_7_24:
			ret=sdpCap.IsFirstG7221_24K();				
			break;
		case X_AUDIO_CODEC_G7221_7_32:
			ret=sdpCap.IsFirstG7221_32K();				
			break;
		case X_AUDIO_CODEC_G7221_14_24:
			ret=sdpCap.IsFirstG7221C_24K();				
			break;
		case X_AUDIO_CODEC_G7221_14_32:
			ret=sdpCap.IsFirstG7221C_32K();				
			break;
		case X_AUDIO_CODEC_G7221_14_48:
			ret=sdpCap.IsFirstG7221C_48K();				
			break;
		default:
			break;
		}

		return ret;
	}
	else 
	{
		switch(type)
		{
		case X_AUDIO_CODEC_GSM:
			ret=sdpCap.IsExistGSM();				
			break;
		case X_AUDIO_CODEC_G7231_63:
			ret=sdpCap.IsExistG7231();				
			break;
		case X_AUDIO_CODEC_G711U:
			ret=sdpCap.IsExistG711u_64K();				
			break;
		case X_AUDIO_CODEC_G711A:
			ret=sdpCap.IsExistG711a_64K();				
			break;
		case X_AUDIO_CODEC_G729:
			ret=sdpCap.IsExistG729A();				
			break;
		case X_AUDIO_CODEC_G722_64:
			ret=sdpCap.IsExistG722_64K();				
			break;
		case X_AUDIO_CODEC_G7221_7_24:
			ret=sdpCap.IsExistG7221_24K();				
			break;
		case X_AUDIO_CODEC_G7221_7_32:
			ret=sdpCap.IsExistG7221_32K();				
			break;
		case X_AUDIO_CODEC_G7221_14_24:
			ret=sdpCap.IsExistG7221C_24K();				
			break;
		case X_AUDIO_CODEC_G7221_14_32:
			ret=sdpCap.IsExistG7221C_32K();				
			break;
		case X_AUDIO_CODEC_G7221_14_48:
			ret=sdpCap.IsExistG7221C_48K();				
			break;
		default:
			break;
		}

		return ret;
	}

	return ret;
}

int SIPCall::GetLocalVideoCodecType(bool first,VIDEC_CODEC_TYPE type)
{
	int ret=0;

	SIPCallCapability &sdpCap=GetMgrCapability();

	if(first) 
	{
		switch(type)
		{
			//video
		case VIDEC_CODEC_H263:
			ret=sdpCap.IsFirstH263();				
			break;
		case VIDEC_CODEC_H263P:
			ret=sdpCap.IsFirstH263v2();				
			break;
		case VIDEC_CODEC_H263PP:
			ret=sdpCap.IsFirstH263v3();				
			break;
		case VIDEC_CODEC_H261:
			ret=sdpCap.IsFirstH261();				
			break;
		case VIDEC_CODEC_H264:
			ret=sdpCap.IsFirstMainVideoH264BaseLineProfile();				
			break;
		case VIDEC_CODEC_H264_SVC:
			ret=sdpCap.IsFirstMainVideoH264HighProfile();				
			break;
		default:
			break;
		}
		return ret;
	}
	else 
	{
		switch(type)
		{
			//video
		case VIDEC_CODEC_H263:
			ret=sdpCap.IsExistH263();				
			break;
		case VIDEC_CODEC_H263P:
			ret=sdpCap.IsExistH263v2();				
			break;
		case VIDEC_CODEC_H263PP:
			ret=sdpCap.IsExistH263v3();				
			break;
		case VIDEC_CODEC_H261:
			ret=sdpCap.IsExistH261();				
			break;
		case VIDEC_CODEC_H264:
			ret=sdpCap.IsExistMainVideoH264BaseLineProfile();				
			break;
		case VIDEC_CODEC_H264_SVC:
			ret=sdpCap.IsExistMainVideoH264HighProfile();				
			break;
		default:
			break;
		}
		return ret;
	}

	return ret;
}

int SIPCall::GetLocalDualVideoCodecType(bool first,VIDEC_CODEC_TYPE type)
{
	int ret=0;

	SIPCallCapability &sdpCap=GetMgrCapability();//SIPCallMgr::Instance().m_sdpCap;

	if(first) 
	{
		switch(type)
		{
			//video
		case VIDEC_CODEC_H263:
			ret=sdpCap.IsFirstH239VideoH263Extend();				
			break;
		case VIDEC_CODEC_H263P:
			ret=sdpCap.IsFirstH239VideoH263v2Extend();				
			break;
		case VIDEC_CODEC_H263PP:
			ret=sdpCap.IsFirstH239VideoH263v3Extend();				
			break;
		case VIDEC_CODEC_H261:
			ret=sdpCap.IsFirstH239VideoH261Extend();				
			break;
		case VIDEC_CODEC_H264:
			ret=sdpCap.IsFirstH239VideoH264BaseLineProfile();				
			break;
		case VIDEC_CODEC_H264_SVC:
			ret=sdpCap.IsFirstH239VideoH264HighProfile();				
			break;
		default:
			break;
		}
		return ret;
	}
	else 
	{
		switch(type)
		{
			//video
		case VIDEC_CODEC_H263:
			ret=sdpCap.IsExistH239VideoH263Extend();				
			break;
		case VIDEC_CODEC_H263P:
			ret=sdpCap.IsExistH239VideoH263v2Extend();				
			break;
		case VIDEC_CODEC_H263PP:
			ret=sdpCap.IsExistH239VideoH263v3Extend();				
			break;
		case VIDEC_CODEC_H261:
			ret=sdpCap.IsExistH239VideoH261Extend();				
			break;
		case VIDEC_CODEC_H264:
			ret=sdpCap.IsExistH239VideoH264BaseLineProfile();				
			break;
		case VIDEC_CODEC_H264_SVC:
			ret=sdpCap.IsExistH239VideoH264HighProfile();				
			break;
		default:
			break;
		}

		return ret;
	}

	return ret;
}

int SIPCall::GetLocalH224CodecType(bool first,DATA_CODEC_ID type)
{
	int ret=0;

	SIPCallCapability &sdpCap=GetMgrCapability();//SIPCallMgr::Instance().m_sdpCap;

	if(first) 
	{
		switch(type)
		{
		case DATA_CODEC_H224:
			ret=sdpCap.IsFirstFECCData();				
			break;
		default:
			break;
		}
		return ret;
	}
	else 
	{
		switch(type)
		{
		case DATA_CODEC_H224:
			ret=sdpCap.IsFirstFECCData();				
			break;
		default:
			break;
		}
		return ret;
	}

	return ret;
}

bool SIPCall::GetAudioRemoteCodecTypeFromPalyloadType(X_AUDIO_CODEC_ID &type,int PT)
{
	bool ret=false;
	//音频
	int nums=m_RemoteAudioCap.m_nCapNum;
	for(int cnt=0; cnt<nums;cnt++)
	{
		if( m_RemoteAudioCap.m_Caps[cnt].nPayLoad==PT ) 
		{
			type=m_RemoteAudioCap.m_Caps[cnt].AudioCodecID;
			ret=true;
			break;
		}
	}

	return ret;
}
bool SIPCall::GetVideoRemoteCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int PT)
{
	bool ret=false;
	int nums=m_RemoteVideoCap.m_nCapNum;
	for(int cnt=0; cnt<nums;cnt++)
	{
		if( m_RemoteVideoCap.m_Caps[cnt].nPayLoad==PT ) 
		{
			type=m_RemoteVideoCap.m_Caps[cnt].eCapType;
			ret=true;
			break;
		}
	}

	return ret;
}

void SIPCall::AddAnswerAudioCodecType(SIPCallCapability & answerCap)
{

}

void SIPCall::AddAnswerVideoCodecType(SIPCallCapability & answerCap)
{

}

void SIPCall::AddAnswerDualVideoCodecType(SIPCallCapability & answerCap)
{

}

void SIPCall::AddAnswerDataCodecType(SIPCallCapability & answerCap)
{

}

int SIPCall::OnOffer0(pjmedia_sdp_session **p_sdp)
{
	return 0;
}

VIDEO_FORMAT_TYPE SIPCall::GetFormatTypeFromLevel(int nLevel,int & nVideoWidth,int & nVideoHeight)
{
	VIDEO_FORMAT_TYPE videoFormatType=VIDEO_FORMAT_QCIF;
	switch (nLevel)
	{
	case 0x0B:
		nVideoWidth=176;
		nVideoHeight=144;
		videoFormatType=VIDEO_FORMAT_QCIF;
		break;
	case 0x0D:			
		nVideoWidth=352;
		nVideoHeight=288;
		videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 0x14:
		nVideoWidth=352;
		nVideoHeight=288;
		videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 0x15:
		nVideoWidth=352;
		nVideoHeight=480;
		videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 0x16:
		nVideoWidth=352;
		nVideoHeight=480;
		videoFormatType=VIDEO_FORMAT_CIF;
		break;
	case 0x1E:		
		nVideoWidth=704;
		nVideoHeight=576;
		videoFormatType=VIDEO_FORMAT_4CIF;
		break;
	case 0x1F:
		nVideoWidth=1280;
		nVideoHeight=720;
		videoFormatType=VIDEO_FORMAT_720P;
		break;
	case 0x28:
		nVideoWidth=1920;
		nVideoHeight=1080;
		videoFormatType=VIDEO_FORMAT_1080P;
		break;
	case 0x20:
		nVideoWidth=1280;
		nVideoHeight=1024;
		videoFormatType=VIDEO_FORMAT_SXGA;
		break;
	default:
		nVideoWidth=176;
		nVideoHeight=144;
		videoFormatType=VIDEO_FORMAT_QCIF;
		break;
	}
	return videoFormatType;
}

void SIPCall::GetVideoFormatTypeWH(VIDEO_FORMAT_TYPE videoFormatType,int & w,int & h)
{
	switch(videoFormatType)
	{
	case VIDEO_FORMAT_QCIF:
		w=176,h=144;
		break;
	case VIDEO_FORMAT_CIF:
		w=352,h=288;
		break;
	case VIDEO_FORMAT_4CIF:
		w=704,h=576;
		break;
	case VIDEO_FORMAT_16CIF:
		w=1408,h=1152;
		break;
	case VIDEO_FORMAT_720I:
		w=1280,h=360;
		break;
	case VIDEO_FORMAT_720P:
		w=1280,h=720;
		break;
	case VIDEO_FORMAT_1080I:
		w=1920,h=540;
		break;
	case VIDEO_FORMAT_1080P:
		w=1920,h=1080;
		break;
	case VIDEO_FORMAT_QVGA:
		w=320,h=240;
		break;
	case VIDEO_FORMAT_VGA:
		w=640,h=480;
		break;
	case VIDEO_FORMAT_SVGA:
		w=800,h=600;
		break;
	case VIDEO_FORMAT_XGA:
		w=1024,h=768;
		break;
	case VIDEO_FORMAT_SXGA:
		w=1280,h=1024;
		break;
	default:
		w=352,h=288;
		break;
	}
}

void SIPCall::OnMediaUpdate()
{
	//重定义处理 20141105
	SIPCallMgr::Instance().OnMediaUpdate(m_strCallID,m_strRemoteIP,m_nRemoteAudioPort,
		m_nAudioPayloadType,m_strAudioPayloadName,m_nAudioSampleRate,m_AudioCodecType,m_nRemoteVideoPort,
		m_nMainVideoPayloadType,m_strMainVideoPayloadName,m_nMainVideoNegBitrate,m_nMainVideoWidth,m_nMainVideoHeight);

	//char msg[512]={0};
	//sprintf(msg,"m_strRemoteIP =%s,m_nRemoteAudioPort =%d,m_nAudioPayloadType=%d,m_strAudioPayloadName =%s,m_nAudioSampleRate=%d,m_AudioCodecType=%d,\n m_nRemoteVideoPort=%d,m_nMainVideoPayloadType=%d,m_strMainVideoPayloadName =%s,m_nMainVideoNegBitrate=%d,m_nMainVideoWidth=%d,m_nMainVideoHeight=%d \n",
	//	m_strRemoteIP,m_nRemoteAudioPort,
	//	m_nAudioPayloadType,m_strAudioPayloadName.c_str(),m_nAudioSampleRate,m_AudioCodecType,m_nRemoteVideoPort,
	//	m_nMainVideoPayloadType,m_strMainVideoPayloadName.c_str(),m_nMainVideoNegBitrate,m_nMainVideoWidth,m_nMainVideoHeight);
	//OutputDebugString(msg);

	PJ_LOG(3,("SIPCall","OnMediaUpdate func call end."));
	return;
}

void SIPCall::CreateBFCPTransports(EBFCPTransportType type)
{
	std::string	strRemoteIP=m_strRemoteIP;
	if(m_strRemoteLastIP==strRemoteIP && m_nBFCPLastTransportPort==m_nBFCPTransportPort) return;

	CloseBFCPTransports();

	//udp 
	if(m_pBFCPUDP==NULL)
	{
		if(SIPCallMgr::Instance().m_nRequestTransportType == e_BFCP_Transport_UDP)
			m_pBFCPUDP=SIP_BFCP::CreateBFCPUDP(*this);
		if(m_pBFCPUDP)
		{
			if(true != m_pBFCPUDP->Open(m_strLocalIP.c_str(),m_nLocalBFCPTransPort,m_strRemoteIP,m_nBFCPTransportPort) )
			{
				m_pBFCPUDP->Close();
				delete m_pBFCPUDP;
				m_pBFCPUDP=NULL;
			}
		}
		if(m_pBFCPUDP)
		{
			SIPCallMgr::Instance().SetSIPBFCPLocalInfo(m_pBFCPUDP,m_nCallDir);
			m_pBFCPUDP ->SendBFCPHello();
		}
	}

	//notice present only use udp because server only has udp bfcp transport
	//return;

	//tcp client
	if(m_pBFCPTCPClient==NULL)
	{
		//if(m_nCallDir==IMXEC_CALL_DIR_OUT)
		if( (m_BfcpFloorControlType==e_Floor_Control_Server) || (m_BfcpFloorControlType==e_Floor_Control_All) )
		{
			if(SIPCallMgr::Instance().m_nRequestTransportType == e_BFCP_Transport_TCP)
				m_pBFCPTCPClient=SIP_BFCP::CreateBFCPTCPClient(*this);
			if(m_pBFCPTCPClient)
			{
				if(true != m_pBFCPTCPClient->Open(m_strLocalIP.c_str(),m_nLocalBFCPTransPort,m_strRemoteIP,m_nBFCPTransportPort) )
				{
					m_pBFCPTCPClient->Close();
					delete m_pBFCPTCPClient;
					m_pBFCPTCPClient=NULL;
				}
			}
			if(m_pBFCPTCPClient)
			{
				SIPCallMgr::Instance().SetSIPBFCPLocalInfo(m_pBFCPTCPClient,m_nCallDir);
			}
		}
	}

	//tcp server
	if(m_pBFCPTCPServer==NULL)
	{
		if( (m_BfcpFloorControlType==e_Floor_Control_Client) || (m_BfcpFloorControlType==e_Floor_Control_All) )
		{
			if(SIPCallMgr::Instance().m_nRequestTransportType == e_BFCP_Transport_TCP)
				m_pBFCPTCPServer=SIP_BFCP::CreateBFCPTCPServer(*this);
			if(m_pBFCPTCPServer)
			{
				if(true != m_pBFCPTCPServer->Open(m_strLocalIP.c_str(),m_nLocalBFCPTransPort,m_strRemoteIP,m_nBFCPTransportPort) )
				{
					m_pBFCPTCPServer->Close();
					delete m_pBFCPTCPServer;
					m_pBFCPTCPServer=NULL;
				}
			}
			if(m_pBFCPTCPServer)
			{
				SIPCallMgr::Instance().SetSIPBFCPLocalInfo(m_pBFCPTCPServer,m_nCallDir);
			}
		}
	}
}

void SIPCall::CloseBFCPTransports()
{
	//udp
	if(m_pBFCPUDP!=NULL)
	{
		m_pBFCPUDP->Close();
		delete m_pBFCPUDP;
		m_pBFCPUDP=NULL;
	}
	//tcp client
	if(m_pBFCPTCPClient!=NULL)
	{
		m_pBFCPTCPClient->Close();
		delete m_pBFCPTCPClient;
		m_pBFCPTCPClient=NULL;
	}
	//tcp server
	if(m_pBFCPTCPServer!=NULL)
	{
		m_pBFCPTCPServer->Close();
		delete m_pBFCPTCPServer;
		m_pBFCPTCPServer=NULL;
	}

	m_pBFCPTCP=NULL;

	//pjsip udp 
	if(m_pBFCPPJSIPUDP!=NULL)
	{
		m_pBFCPPJSIPUDP->Close();
		delete m_pBFCPPJSIPUDP;
		m_pBFCPPJSIPUDP=NULL;
	}

}

void SIPCall::SetAcceptCallState(bool bState)
{
	m_bAccept=bState;
}

void SIPCall::SetLocalDualVideoPort(int RtpPort,int RtcpPort)
{
	m_nLocalDualVideoPort=RtpPort;
	m_nLocalDualVideoRtcpPort=RtcpPort;
}

void SIPCall::SetLocalBFCPTransPort(int nBfcpPort)
{
	m_nLocalBFCPTransPort=nBfcpPort;
}

bool SIPCall::CreateLocalSDP(pj_pool_t *pool,pjmedia_sdp_session **p_sdp,int nSupportDual)
{
	SIPCallCapability & offerSdpCap=GetMgrCapability();
	SetLocalCapabilitis(offerSdpCap);
	SIPCallMgr::Instance().SetCapabilityParam(this,offerSdpCap,nSupportDual,m_strLocalIP,m_nLocalAudioPort,m_nLocalVideoPort,m_nLocalDualVideoPort,m_nLocalBFCPTransPort,m_nCallDir);
	if(offerSdpCap.SDP_Capabilitis_Add(pool,p_sdp)<0)
	{
		LogFile(NULL,NULL,"","","CreateLocalSDP SDP_Capabilitis_Add Error.");
		return false;
	}
	return true;
}

//SDP 协商
int SIPCall::SDP_Negotiate()
{
	m_bNegCapAudio=false;
	m_bNegCapVideo=false;
	m_bNegCapDualVideo=false;
	m_bNegCapData=false;
	SIPVideoCap &remoteVideoCap=m_RemoteVideoCap;
	SIPAudioCap &remoteAudioCap=m_RemoteAudioCap;
	SIPDualVideoCap &remoteDualVideoCap=m_RemoteDualVideoCap;
	SIPDataCap &remoteDataCap=m_RemoteDataCap;

	XAutoLock l(m_csCapNeg);

	RecvCapCopyToDapCaps();

	RecvCapDapCapsSort(VIDEC_CODEC_H263);
	RecvCapDapCapsSort(VIDEC_CODEC_H263P);
	RecvCapDapCapsSort(VIDEC_CODEC_H263PP);

	LogRecvCapDapCaps();

	//能力集协商 ?? 
#if defined(HAS_SIP_AVCON_MP) && ( (HAS_SIP_AVCON_MP == 2) || (HAS_SIP_AVCON_MP == 1) ) 
	CapabilitisNegotiate(4,false);
	if(m_bNegCapVideo ==false) CapabilitisNegotiate(3,false);
	if(m_bNegCapDualVideo ==false) CapabilitisNegotiate(3,false);

#else 

	CapabilitisNegotiate(1,false);
	if(m_bNegCapVideo ==false) CapabilitisNegotiate(2,false);
	if(m_bNegCapDualVideo ==false) CapabilitisNegotiate(2,false);

#endif


	//if (m_nNegAudioCapLocation <0)
	//{
	//	return -1;
	//}
	//{
	//	XAutoLock l(m_csCapNeg);

	//	if(m_nNegAudioCapLocation >=0)
	//	{
	//		memcpy(&m_CapNegAudio,&remoteAudioCap.m_Caps[m_nNegAudioCapLocation],sizeof(SIPAUDIOCAPBILITY));
	//		m_bNegCapAudio=true;
	//	}
	//	if(m_nNegVideoCapLocation >=0)
	//	{
	//		memcpy(&m_CapNegVideo,&remoteVideoCap.m_Caps[m_nNegVideoCapLocation],sizeof(SIPVIDEOCAPBILITY));
	//		m_bNegCapVideo=true;
	//	}
	//	if(m_nNegDualVideoCapLocation >=0)
	//	{
	//		memcpy(&m_CapNegDualVideo,&remoteDualVideoCap.m_Caps[m_nNegDualVideoCapLocation],sizeof(SIPVIDEOCAPBILITY));
	//		m_bNegCapDualVideo=true;
	//	}
	//	if(m_nNegDataCapLocation >=0)
	//	{
	//		memcpy(&m_CapNegData,&remoteDataCap.m_Caps[m_nNegDataCapLocation],sizeof(SIPDATACAPBILITY));
	//		m_bNegCapData=true;
	//	}
	//}

	return 0;
}

void SIPCall::OnMediaUpdate(const std::string&strRemoteIP,unsigned short nRemoteAudioPort,int nAudioPayloadType,const std::string&strAudioPayloadName,int nAudioSampleRate,X_AUDIO_CODEC_ID AudioCodecID,unsigned short nRemoteVideoPort,int nVideoPayloadType,const std::string&strVideoPayloadName,int nVideoBitrate,int nVideoWidth,int nVideoHeight)
{
	LogFile(NULL,NULL,"","","SIPCall::OnMediaUpdate         111");

	int nClockRate=90000;
	int nSamplePerFrame=0 ;
	pjmedia_endpt *	pMediaEndpoint=SIPCallMgr::Instance().GetMediaEndpoint();
	int nCmpCount=SIPCallMgr::Instance().m_nICETransportCount;
	pj_pool_t * sdpPool = m_pPOOL;
	m_nMediaCount=0;
	m_bCallState =true;

	sip_register_thread();
	//PJ_LOG(3,("SIPCall","OnMediaUpdate open media func call begin."));

	if (nAudioPayloadType<255)
	{
		nClockRate = m_nAudioClockRate; //m_RemoteAudioCap.m_Caps[m_nNegAudioCapLocation].nClockRate;
		if(nClockRate<=0) nClockRate=8000;

		GetAudioCodecInfo(AudioCodecID,nSamplePerFrame);
		{
			AudioRTPSession* pRTPSession =m_pAudioRTPSession;
			//XAutoLock l(m_csAudioRTPSession);
			if (m_pAudioRTPSession!=NULL)
			{
				m_pAudioRTPSession=NULL;
				pRTPSession->AddParam(nAudioPayloadType,nClockRate,nSamplePerFrame);
				pRTPSession->AddRemote(strRemoteIP.c_str(),nRemoteAudioPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,0,nCmpCount);
				m_pAudioRTPSession =pRTPSession;
			}
			else
			{
				int nFlagReliableRTP=0;
				if(m_nAudioReliableRTP && m_nRemoteAudioReliableRTP) 
					nFlagReliableRTP=1;

				pRTPSession=new AudioRTPSession(*this);
//notice
if(pRTPSession ) pRTPSession->InitRtpParam(320,64*1024*1.5,30,1,AudioCodecID,0,nClockRate ,nSamplePerFrame ,nFlagReliableRTP);

				if(pRTPSession ) pRTPSession->Open(m_strLocalIP.c_str(),m_nLocalAudioPort,nFlagReliableRTP,"",nAudioPayloadType 
					,pMediaEndpoint,nClockRate,nSamplePerFrame);
				if(pRTPSession ) pRTPSession->AddRemote(strRemoteIP.c_str(),nRemoteAudioPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,0,nCmpCount);
				m_pAudioRTPSession =pRTPSession;
			}
		}
		m_nMediaCount++;
		if(m_nAudioOpenState==2)
		{
			m_rSIPUser.CloseRingTone();
			OpenLocalAudioCapDev(AudioCodecID,nAudioPayloadType);
			OpenRemoteAudioPlayChan(AudioCodecID,nAudioPayloadType);

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) 
			unsigned long ulLocalIP=htonl(XSocket::atoh(m_strLocalIP.c_str()));
			unsigned short usLocalPort=m_nLocalAudioPort;
			unsigned long ulRemoteIP=htonl(XSocket::atoh(strRemoteIP.c_str()));
			unsigned short usRemotePort=nRemoteAudioPort;
			SIPCallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(m_strCallID.c_str(),AudioCodecID,nAudioPayloadType,ulLocalIP,usLocalPort);
			SIPCallMgr::Instance().OnDAP_CallAudioSendChannelConnected((HDAPAPPCALL)this,m_strCallID.c_str(),AudioCodecID,nAudioPayloadType,ulRemoteIP,usRemotePort);
#else 
#endif 
			m_pAudioRTPSession->PacketRecvRestart();
		}
		//PJ_LOG(3,("SIPCall","OnMediaUpdate open audio media func call end."));
	}

	LogFile(NULL,NULL,"","","SIPCall::OnMediaUpdate         111222");

	if(nRemoteVideoPort<=0) return ;

	if (nVideoPayloadType<255)
	{
		std::string strCodeName="";

		bool bGotCodecID=false;
		VIDEC_CODEC_TYPE codecType=VIDEC_CODEC_H263;
		int nDevID=SIPCallMgr::Instance().GetMainVideoDevID();
		int nFrameRate=SIPCallMgr::Instance().GetMainVideoFrameRate();
        int nNetworkType=SIPCallMgr::Instance().GetMainVideoNetworkType();
		if(nFrameRate<=0) nFrameRate=30;

		nSamplePerFrame=90000/nFrameRate;
		if (nVideoPayloadType==34)
		{
			bGotCodecID=true;
			codecType=VIDEC_CODEC_H263;

			strCodeName="H263";
		}
		else if (nVideoPayloadType>=96)
		{
			if (strVideoPayloadName=="H264")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H264;
				strCodeName="H264";
			}
			else if (strVideoPayloadName=="H264SVC")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H264_SVC;
				strCodeName="H264";
			}
			else if (strVideoPayloadName=="H263-1998")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H263P;

				strCodeName="H263-1998";
			}
			else if (strVideoPayloadName=="H263-2000")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H263PP;

				strCodeName="H263-2000";
			}
		}

		if (bGotCodecID)
		{
			{
				int nFlagReliableRTP=0;
				if(m_nVideoReliableRTP && m_nRemoteVideoReliableRTP)
					nFlagReliableRTP=1;

				VideoRTPSession* pVideoRTPSession =m_pVideoRTPSession;
				//XAutoLock l(m_csVideoRTPSession);
				if (m_pVideoRTPSession !=NULL)
				{
					m_pVideoRTPSession=NULL;
					pVideoRTPSession->AddParam(nVideoPayloadType,nClockRate,nSamplePerFrame);
					pVideoRTPSession->AddRemote(strRemoteIP.c_str(),nRemoteVideoPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,1,nCmpCount,nFlagReliableRTP);
					m_pVideoRTPSession=pVideoRTPSession;
				}
				else
				{
					pVideoRTPSession=new VideoRTPSession(*this);
					//notice use medianetrtp
					//if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(nVideoWidth * nVideoHeight ,nVideoBitrate *1024 * 1.5,nFrameRate,2,codecType);
					//if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(500<<10, nVideoBitrate *1024 * 1.5, nFrameRate, 2, codecType);
					if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(500<<10,6<<20, nFrameRate, 2, codecType ,0,0,0,nFlagReliableRTP);

					if(pVideoRTPSession ) pVideoRTPSession->Open(m_strLocalIP.c_str(),m_nLocalVideoPort,nFlagReliableRTP,strCodeName,nVideoPayloadType 
						,pMediaEndpoint,nClockRate,nSamplePerFrame);
					if(pVideoRTPSession ) pVideoRTPSession->AddRemote(strRemoteIP.c_str(),nRemoteVideoPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,1,nCmpCount,nFlagReliableRTP);
					m_pVideoRTPSession=pVideoRTPSession;
				}
			}
			m_nMediaCount++;
			if(m_nVideoOpenState==2)
			{
				m_nMainChannelState=1;
				OpenRemoteMainVideoPlayChan(codecType,nVideoBitrate,nVideoPayloadType);
				//sprintf(szDebug,"nVideoWidth=%d nVideoHeight=%d nVideoBitrate=%d\n",nVideoWidth,nVideoHeight,nVideoBitrate);
				OpenLocalMainVideoCapDev(nDevID,nVideoWidth,nVideoHeight,nFrameRate,codecType,nVideoBitrate,nVideoPayloadType,nNetworkType);

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) 
				unsigned long ulLocalIP=htonl(XSocket::atoh(m_strLocalIP.c_str()));
				unsigned short usLocalPort=m_nLocalVideoPort;
				unsigned long ulRemoteIP=htonl(XSocket::atoh(strRemoteIP.c_str()));
				unsigned short usRemotePort=nRemoteVideoPort;
				int nLevel=GetMgrCapability().GetVideoH264Level(m_nMainVideoFormat); //m_CapNegVideo.nLevel;
				SIPCallMgr::Instance().OnDAP_CallMediaChannelConnected(2,2,NULL,m_strCallID.c_str(),
					codecType,nVideoBitrate,nVideoPayloadType,ulLocalIP,usLocalPort,nVideoWidth,nVideoHeight,nLevel);
				SIPCallMgr::Instance().OnDAP_CallMediaChannelConnected(2,1,(HDAPAPPCALL)this,m_strCallID.c_str(),
					codecType,nVideoBitrate,nVideoPayloadType,ulRemoteIP,usRemotePort,nVideoWidth,nVideoHeight,nLevel);

#else 
#endif 
				m_pVideoRTPSession->PacketRecvRestart();
			}
			PJ_LOG(3,("SIPCall","OnMediaUpdate open video media func call end."));
		}
	}

	m_nDualVideoDevID=SIPCallMgr::Instance().GetSecondVideoDevID();
	m_nDualVideoFrameRate=SIPCallMgr::Instance().GetSecondVideoDevFrameRate();

	std::string strDualRemoteIP=m_strRemoteIP;
	nRemoteVideoPort=m_nRemoteDualVideoPort;
	nVideoPayloadType=m_nDualVideoPayloadType;
	std::string& strDualVideoPayloadName=m_strDualVideoPayloadName;
	nVideoBitrate=m_nDualVideoBitrate;

	LogFile(NULL,NULL,"","","SIPCall::OnMediaUpdate         111333");

	if(nRemoteVideoPort<=0) return ;

	if (nVideoPayloadType<255)
	{
		std::string strCodeName="";
		
		bool bGotCodecID=false;
		VIDEC_CODEC_TYPE codecType=VIDEC_CODEC_H263;
		if(m_nDualVideoFrameRate<=0) m_nDualVideoFrameRate=30;
		nSamplePerFrame=90000/m_nDualVideoFrameRate;
		if (nVideoPayloadType==34)
		{
			bGotCodecID=true;
			codecType=VIDEC_CODEC_H263;
			strCodeName="H263";
		}
		else if (nVideoPayloadType>=96)
		{
			if (strDualVideoPayloadName=="H264")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H264;
				strCodeName="H264";
			}
			else if (strDualVideoPayloadName=="H264SVC")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H264_SVC;
				strCodeName="H264";
			}
			else if (strDualVideoPayloadName=="H263-1998")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H263P;
				strCodeName="H263-1998";
			}
			else if (strDualVideoPayloadName=="H263-2000")
			{
				bGotCodecID=true;
				codecType=VIDEC_CODEC_H263PP;
				strCodeName="H263-2000";
			}
		}

		if (bGotCodecID)
		{
			int nFlagReliableRTP=0;
			if(m_nDualVideoReliableRTP && m_nRemoteDualVideoReliableRTP)
				nFlagReliableRTP=1;

			VideoRTPSession* pVideoRTPSession =m_pDualVideoRTPSession;
			//XAutoLock l(m_csDualVideoRTPSession);
			if (m_pDualVideoRTPSession!=NULL)
			{
				m_pDualVideoRTPSession=NULL;
				pVideoRTPSession->AddParam(nVideoPayloadType,nClockRate,nSamplePerFrame);
				pVideoRTPSession->AddRemote(strDualRemoteIP.c_str(),nRemoteVideoPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,2,nCmpCount,nFlagReliableRTP);
				m_pDualVideoRTPSession=pVideoRTPSession;
			}
			else
			{
				m_DualVideoRTPCallBack.SetSipCall(this);
				pVideoRTPSession=new VideoRTPSession(m_DualVideoRTPCallBack);

				//notice
				//if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(m_nDualVideoWidth * m_nDualVideoHeight ,nVideoBitrate *1024 * 1.5,m_nDualVideoFrameRate,3,codecType);
				//if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(500<<10, nVideoBitrate *1024 * 1.5, m_nDualVideoFrameRate, 3,codecType);
				if(pVideoRTPSession ) pVideoRTPSession->InitRtpParam(500<<10, 6<<20, m_nDualVideoFrameRate, 3,codecType ,0,0,0,nFlagReliableRTP);

				if( pVideoRTPSession ) pVideoRTPSession->Open(m_strLocalIP.c_str(),m_nLocalDualVideoPort,nFlagReliableRTP,strCodeName,nVideoPayloadType 
					,pMediaEndpoint,nClockRate,nSamplePerFrame);
                if( pVideoRTPSession ) pVideoRTPSession->AddRemote(strDualRemoteIP.c_str(),nRemoteVideoPort,sdpPool,m_pCallLocalSDP,m_pCallRemoteSDP,2,nCmpCount, nFlagReliableRTP);
				m_pDualVideoRTPSession=pVideoRTPSession;
			}
			m_nMediaCount++;
			if(m_nDualVideoOpenState==2)
			{
				m_nDualChannelState=1;
				OnSecondVideoRecvChannelConnected(codecType,nVideoBitrate,nVideoPayloadType);

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
				unsigned long ulLocalIP=htonl(XSocket::atoh(m_strLocalIP.c_str()));
				unsigned short usLocalPort=m_nLocalDualVideoPort;
				unsigned long ulRemoteIP=htonl(XSocket::atoh(strDualRemoteIP.c_str()));
				unsigned short usRemotePort=nRemoteVideoPort;
				int nLevel= GetMgrCapability().GetVideoH264Level(m_nDualVideoFormat); //m_CapNegDualVideo.nLevel;
				SIPCallMgr::Instance().OnDAP_CallMediaChannelConnected(3,2,NULL,m_strCallID.c_str(),
					codecType,nVideoBitrate,nVideoPayloadType,ulLocalIP,usLocalPort,nVideoWidth,nVideoHeight,nLevel);
				SIPCallMgr::Instance().OnDAP_CallMediaChannelConnected(3,1,(HDAPAPPCALL)this,m_strCallID.c_str(),
					codecType,nVideoBitrate,nVideoPayloadType,ulRemoteIP,usRemotePort,nVideoWidth,nVideoHeight,nLevel);

#else


#endif
				m_pDualVideoRTPSession->PacketRecvRestart();
			}
		}
	}

	LogFile(NULL,NULL,"","","SIPCall::OnMediaUpdate         222");

	//PJ_LOG(3,("SIPCall","OnMediaUpdate open media func call end."));
	//h224
}

void SIPCall::SendGenericRequest() 
{
	DebugOutput(2,"SendGenericRequest callID:%s\n",this->m_strCallID.c_str());

	int nUseDualVideoTokenType=SIPCallMgr::Instance().m_nUseDualVideoTokenType;
	switch(nUseDualVideoTokenType)
	{
	case 1:
		SIPSendApplyDualVideoToken();
		break;
	default:
		{
			if(m_pBFCPTCP==NULL)
			{
				if(m_pBFCPTCPClient!=NULL) 
					m_pBFCPTCP=m_pBFCPTCPClient;
				else if(m_pBFCPTCPServer!=NULL)
					m_pBFCPTCP=m_pBFCPTCPServer;
			}
			if( (m_nBFCPTransportType==e_BFCP_Transport_UDP) && (m_pBFCPUDP!=NULL) )
			{
				m_pBFCPUDP->SendBFCPFloorRequest();
			}
			if( (m_nBFCPTransportType==e_BFCP_Transport_TCP) && (m_pBFCPTCP!=NULL) )
			{
				m_pBFCPTCP->SendBFCPFloorRequest();
			}

			if(m_pBFCPTCP ==NULL) LogFile(NULL,NULL,"","","BFCPTCP NULL");
			if(m_pBFCPUDP ==NULL) LogFile(NULL,NULL,"","","BFCPUDP NULL");

		}
		break;
	}
}

void SIPCall::SendGenericResponse(unsigned short transactionId,unsigned short userId,unsigned short floorId,unsigned short status)
{
	DebugOutput(2,"SendGenericResponse callID:%s\n",this->m_strCallID.c_str());

	if(m_pBFCPTCP==NULL)
	{
		if(m_pBFCPTCPClient!=NULL) 
			m_pBFCPTCP=m_pBFCPTCPClient;
		else 
			if(m_pBFCPTCPServer!=NULL) m_pBFCPTCP=m_pBFCPTCPServer;
	}

	if( (m_nBFCPTransportType==e_BFCP_Transport_UDP) && (m_pBFCPUDP!=NULL) )
	{
		m_pBFCPUDP->SendBFCPFloorRequestStatus( transactionId,userId,floorId, status);
	}
	if( (m_nBFCPTransportType==e_BFCP_Transport_TCP) && (m_pBFCPTCP!=NULL) )
		m_pBFCPTCP->SendBFCPFloorRequestStatus( transactionId,userId,floorId, status);
}

void SIPCall::SendGenericCommand()
{
	DebugOutput(2,"SendGenericCommand callID:%s\n",this->m_strCallID.c_str());
	int nUseDualVideoTokenType=SIPCallMgr::Instance().m_nUseDualVideoTokenType;
	switch(nUseDualVideoTokenType)
	{
	case 1:
		SIPSendReleaseDualVideoToken();
		break;
	default:
		{
			if(m_pBFCPTCP==NULL)
			{
				if(m_pBFCPTCPClient!=NULL) 
					m_pBFCPTCP=m_pBFCPTCPClient;
				else 
					if(m_pBFCPTCPServer!=NULL) m_pBFCPTCP=m_pBFCPTCPServer;
			}

			if( (m_nBFCPTransportType==e_BFCP_Transport_UDP) && (m_pBFCPUDP!=NULL) )
			{
				m_pBFCPUDP->SendBFCPFloorRelease();
			}

			if( (m_nBFCPTransportType==e_BFCP_Transport_TCP) && (m_pBFCPTCP!=NULL) )
				m_pBFCPTCP->SendBFCPFloorRelease();
		}
		break;
	}
}

void SIPCall::SendBFCPHello() //Hello
{
	if(m_pBFCPTCP==NULL)
	{
		if(m_pBFCPTCPClient!=NULL) 
			m_pBFCPTCP=m_pBFCPTCPClient;
		else 
			if(m_pBFCPTCPServer!=NULL) m_pBFCPTCP=m_pBFCPTCPServer;
	}

	if( (m_nBFCPTransportType==e_BFCP_Transport_UDP) && (m_pBFCPUDP!=NULL) )
		m_pBFCPUDP->SendBFCPHello();
	if( (m_nBFCPTransportType==e_BFCP_Transport_TCP) && (m_pBFCPTCP!=NULL) )
		m_pBFCPTCP->SendBFCPHello();
}

void SIPCall::SendBFCPHelloACK() //Hello ack
{
	if(m_pBFCPTCP==NULL)
	{
		if(m_pBFCPTCPClient!=NULL) 
			m_pBFCPTCP=m_pBFCPTCPClient;
		else 
			if(m_pBFCPTCPServer!=NULL) m_pBFCPTCP=m_pBFCPTCPServer;
	}

	if( (m_nBFCPTransportType==e_BFCP_Transport_UDP) && (m_pBFCPUDP!=NULL) )
		m_pBFCPUDP->SendBFCPHelloACK();
	if( (m_nBFCPTransportType==e_BFCP_Transport_TCP) && (m_pBFCPTCP!=NULL) )
		m_pBFCPTCP->SendBFCPHelloACK();
}

//发送DTMF
typedef struct tagX_DTMF_2833 
{
	unsigned int event:8;
	unsigned int volume:6;
	unsigned int r:1;
	unsigned int e:1;
	unsigned int duration:16;
}X_DTMF_2833,*PX_DTMF_2833;

int SIPCall::SendDTMF(const char dtmf,int nType)
{

//notice test
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#else

	if(m_nSendDTMFRTP >0 ) nType=0;
#endif


	if (nType)
	{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
		return 0;
#else

		AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);

#endif

	}
	else
	{
	//XAutoLock l(m_csAudioRTPSession);
	if (m_pAudioRTPSession!=NULL)
	{
		int nPayloadType=101;
		unsigned short nDuration=960;


		int nDurationCount=nDuration/160;
		if (nDurationCount<2)
			nDurationCount=2;
		nDuration=0;

		unsigned char event=0;
		switch (dtmf)
		{
		case '0':
			event=0;
			break;
		case '1':
			event=1;
			break;
		case '2':
			event=2;
			break;
		case '3':
			event=3;
			break;
		case '4':
			event=4;
			break;
		case '5':
			event=5;
			break;
		case '6':
			event=6;
			break;
		case '7':
			event=7;
			break;
		case '8':
			event=8;
			break;
		case '9':
			event=9;
			break;
		case '*':
			event=10;
			break;
		case '#':
			event=11;
			break;
		default:
			return -1;
		}

		X_DTMF_2833 tDTMF2833;
		PX_DTMF_2833 pXDTMF2833=(PX_DTMF_2833)&tDTMF2833;
		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		nDuration+=160;
		pXDTMF2833->duration=htons(nDuration);

		m_nDTMFTimestamp+=160;

		unsigned char pRTPBuffer[24];
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPBuffer;
		memset(pRTPHeader,0,RTP_HEADER_LEN);
		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->marker=1;
		pRTPHeader->sequence=htons(m_nDTMFSeqNo++);
		pRTPHeader->timestamp=htonl(m_nDTMFTimestamp);
		pRTPHeader->ssrc=htonl(m_nDTMFSSRC);

		memcpy(pRTPBuffer+RTP_HEADER_LEN,pXDTMF2833,sizeof(X_DTMF_2833));
		int nRTPPacketLen=RTP_HEADER_LEN+sizeof(X_DTMF_2833);

		m_pAudioRTPSession->SendRTPPacket(pXDTMF2833,sizeof(X_DTMF_2833),nPayloadType,1,0,pRTPBuffer,nRTPPacketLen);

		pRTPHeader->marker=0;

		for (int i=1;i<(nDurationCount-1);i++)
		{
			nDuration+=160;
			pXDTMF2833->duration=htons(nDuration);

			pRTPHeader->sequence=htons(m_nDTMFSeqNo++);
			m_pAudioRTPSession->SendRTPPacket(pXDTMF2833,sizeof(X_DTMF_2833),nPayloadType,0,0,pRTPBuffer,nRTPPacketLen);
		}


		pXDTMF2833->e=1;
		nDuration+=160;
		pXDTMF2833->duration=htons(nDuration);

		pRTPHeader->sequence=htons(m_nDTMFSeqNo++);
		m_pAudioRTPSession->SendRTPPacket(pXDTMF2833,sizeof(X_DTMF_2833),nPayloadType,0,160,pRTPBuffer,nRTPPacketLen,3);

		return 0;
	}
	}
	return -1;
}

const std::string&SIPCall::GetCallID(void)
{
	return m_strCallID;
}

void SIPCall::SetPJSIPCallID(int call_id)
{
	m_nCallId=call_id;
}


void SIPCall::SetLineID(int LineID)
{
	m_nLineId=LineID;
}

int SIPCall::GetLineID(void)
{
	return m_rSIPUser.GetLineID();
}

SIPCall::CALL_DIRECTION SIPCall::GetCallDirection(void)
{
	return m_CallDirection;
}

SIPCall::CALL_STATUS SIPCall::GetCallStatus(void)
{
	return m_CallStatus;
}

const std::string&SIPCall::GetLocalIP(void)
{
	return m_strLocalIP;
}

SIPVideoCap &SIPCall::GetSIPCallVideoCap(void)
{
	return m_RemoteVideoCap;
}

SIPAudioCap &SIPCall::GetSIPCallAudioCap(void)
{
	return m_RemoteAudioCap;
}

void SIPCall::SetLocalIP(const std::string&strLocalIP)
{
	m_strLocalIP=strLocalIP;
}

bool SIPCall::IsCallEstablished()
{
	return m_CallStatus==CALL_STATUS_ESTABLISHED ? true:false;
}

void SIPCall::SetRemoteIP(char * ip)
{
	m_strRemoteLastIP=m_strRemoteIP;
	if(ip!=NULL) strcpy(m_strRemoteIP,ip);
}

void SIPCall::SetRemoteAudioPort(int RtpPort,int RtcpPort)
{
	m_nRemoteAudioPort=RtpPort;
	m_nRemoteAudioRTCPPort=RtcpPort;
}
void SIPCall::SetRemoteVideoPort(int RtpPort,int RtcpPort)
{
	m_nRemoteVideoPort=RtpPort;
	m_nRemoteVideoRTCPPort=RtcpPort;
}

void SIPCall::SetRemoteDualVideoPort(int RtpPort,int RtcpPort)
{
	m_nRemoteDualVideoPort=RtpPort;
	m_nRemoteDualVideoRTCPPort=RtcpPort;
}

unsigned short SIPCall::GetLocalAudioPort(void)
{
	return m_nLocalAudioPort;
}

void SIPCall::SetLocalAudioPort(unsigned short nLocalAudioPort)
{
	m_nLocalAudioPort=nLocalAudioPort;
}

unsigned short SIPCall::GetLocalVideoPort(void)
{
	return m_nLocalVideoPort;
}

void SIPCall::SetLocalVideoPort(unsigned short nLocalVideoPort)
{
	m_nLocalVideoPort=nLocalVideoPort;
}

const unsigned char SIPCall::GetUserIDIndex(void)
{
	return m_nUserIDIndex;
}

void SIPCall::SetUserIDIndex(const unsigned char nUserIDIndex)
{
	m_nUserIDIndex=nUserIDIndex;
}


const std::string&SIPCall::GetUserID(void)
{
	return m_strUserID;
}

void SIPCall::SetUserID(const std::string&strUserID)
{
	m_strUserID=strUserID;
}

const std::string&SIPCall::GetUserName(void)
{
	return m_strUserName;
}

void SIPCall::SetUserName(const std::string&strUserName)
{
	m_strUserName=strUserName;
}

void SIPCall::OnCallEstablished(void)
{
	m_CallStatus=CALL_STATUS_ESTABLISHED;
	//test sip zhongxing mcu use
	if(SIPCallMgr::Instance().m_nSendSubscribre >0 ) SIPSendSubscribeRequest();

	if(m_bHasRemoteDualSDP && m_nBFCPTransportPort >0 && m_pCallLocalSDP)
	{
		//bfcp transport create notice
		SIPCallMgr::Instance().OnCallBFCPTransportChange(this);

		if(SIPCallMgr::Instance().m_nDualVideoSDPMode ==2)
		{
			pjmedia_sdp_session *sdp = NULL;
			int nSupportDual=GetMgrSupportH239();

			SIPCallCapability & offerSdpCap=GetMgrCapability();
			SetLocalCapabilitis(offerSdpCap);
			SIPCallMgr::Instance().SetCapabilityParam(this,offerSdpCap,nSupportDual,m_strLocalIP,m_nLocalAudioPort,m_nLocalVideoPort,m_nLocalDualVideoPort,m_nLocalBFCPTransPort,m_nCallDir);
			offerSdpCap.SetBFCPFloorControlType(e_Floor_Control_Client);
			offerSdpCap.SetBFCPSetupType(e_BFCP_Setup_Active);
			offerSdpCap.SetBFCPFloorId(1);
			if(offerSdpCap.SDP_Capabilitis_Add(m_pPOOL,&sdp)<0)
			{
				LogFile(NULL,NULL,"","","OnCallEstablished SDP_Capabilitis_Add Error.");
				return ;
			}

			sdp->media[2]->desc.port=m_nBFCPTransportPort;
			SIPSendInviteRequest(sdp);
			m_bHasRemoteDualSDP=false;
		}
	}

}

void SIPCall::SendAudio(char*pRTPPacketData,int nRTPPacketLen)
{
	//XAutoLock l(m_csAudioRTPSession);
	if (m_pAudioRTPSession!=NULL)
	{
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPPacketData;
		unsigned char*pPayloadData=(unsigned char*)pRTPPacketData+RTP_HEADER_LEN;
		int nPayloadLen=nRTPPacketLen-RTP_HEADER_LEN;
		unsigned long nDelta=20;
		if (m_nLastSentAudioRTPTimestamp!=0)
		{
			nDelta=ntohl(pRTPHeader->timestamp)-m_nLastSentAudioRTPTimestamp;
			if (nDelta>48000)
			{
				nDelta=20;
			}
		}

		m_nLastSentAudioRTPTimestamp=ntohl(pRTPHeader->timestamp);
		unsigned char nPayloadType=pRTPHeader->type;
		bool bMarker=(pRTPHeader->marker!=0);

		m_nDTMFSSRC=ntohl(pRTPHeader->ssrc);

		m_pAudioRTPSession->SendRTPPacket(pPayloadData,nPayloadLen,nPayloadType,bMarker,nDelta,(unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
}

void SIPCall::SendMainVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	//XAutoLock l(m_csVideoRTPSession);
	if (m_pVideoRTPSession!=NULL)
	{
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPPacketData;
		unsigned char*pPayloadData=(unsigned char*)pRTPPacketData+RTP_HEADER_LEN;
		int nPayloadLen=nRTPPacketLen-RTP_HEADER_LEN;
		unsigned char nPayloadType=pRTPHeader->type;
		bool bMarker=(pRTPHeader->marker!=0);

		m_pVideoRTPSession->SendRTPPacket(pPayloadData,nPayloadLen,nPayloadType,bMarker,bMarker?3003:0,(unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
}

void SIPCall::OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	SIPCallMgr::Instance().OnDAP_RecvRemoteMainVideoRTPPacket(m_strCallID.c_str(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	//sip_register_thread();

	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
	{
		m_nMainVideoPacketTimestamp=nTimestamp;
		if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) SIPMainVideoSendPictureFastUpdate();
	}
}

void SIPCall::OnVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen)
{

}

void SIPCall::OnVideoRTPSessionCallbackRequestKeyFrame(void)
{

}

void SIPCall::OnAudioRTPSessionCallbackRTPPacket(AudioRTPSession*pAudioRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	OnRecvdRemoteAudioRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	SIPCallMgr::Instance().OnDAP_RecvRemoteAudioRTPPacket(m_strCallID.c_str(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

}

int SIPCall::GetHasVideo(void)
{
	XAutoLock l(m_csVideoRTPSession);
	if (m_pVideoRTPSession!=NULL)
	{
		return 1;
	}

	return 0;
}

void SIPCall::GetAllPayloadTypeFromAttr(pjmedia_sdp_media *m,int payloadTypeArray[ ], unsigned & ptCount)
{
	unsigned k=0;
	unsigned j=0;
	int status=-1;
	bool findPt=false;
	//取出所有rtpmap PT
	for(j=0;j<m->attr_count;j++)
	{
		pjmedia_sdp_rtpmap rtpmap;

		if(pj_stricmp(&(m->attr[j]->name),&(pj_str("rtpmap")))==0)
		{						
			status=pjmedia_sdp_attr_get_rtpmap(m->attr[j],&rtpmap);
			if(status!=PJ_SUCCESS) continue;

			payloadTypeArray[ptCount]=pj_strtoul(&rtpmap.pt);
			ptCount++;

		}					
	}
	//取出所有fmtp PT				
	for(j=0;j<m->attr_count;j++)
	{
		pjmedia_sdp_fmtp fmtp;					
		if(pj_stricmp(&(m->attr[j]->name),&(pj_str("fmtp")))==0)
		{						
			status=pjmedia_sdp_attr_get_fmtp(m->attr[j],&fmtp);
			if(status!=PJ_SUCCESS) continue;
			findPt=false;
			for(k=0;k<ptCount;k++)
			{
				if(payloadTypeArray[k]==pj_strtoul(&fmtp.fmt) )
				{
					findPt=true;
					break;
				}
			}
			if(!findPt) 
			{
				payloadTypeArray[ptCount]=pj_strtoul(&fmtp.fmt);
				ptCount++;
			}
		}					
	}

	char fmt[4]={0};	
	//取出所有fmt PT				
	for(j=0;j<m->desc.fmt_count;j++)
	{
		strncpy(fmt,m->desc.fmt[j].ptr,m->desc.fmt[j].slen);
		fmt[m->desc.fmt[j].slen]=0;

		findPt=false;
		for(k=0;k<ptCount;k++)
		{
			if(payloadTypeArray[k]==strtoul(fmt,NULL,10) )
			{
				findPt=true;
				break;
			}
		}
		if(!findPt) 
		{
			payloadTypeArray[ptCount]=strtoul(fmt,NULL,10);
			ptCount++;
		}
	}

}

pj_bool_t SIPCall::ParseRemoteSDP(const pjmedia_sdp_session *sdp)
{
	//获取对方SDP能力集
	unsigned i,j;
	pjmedia_sdp_media *m;			
	int status=-1;

	char strRemoteIP[64]={0};
	unsigned int nRemoteAudioPort=0;
	unsigned int nRemoteAudioRTCPPort=0;
	unsigned int nRemoteVideoRTCPPort=0;
	unsigned int nRemoteVideoPort=0; 		
	unsigned int nRemoteDataPort=0;
	unsigned int nRemoteDataRTCPPort=0;

	int nBandWidth=384;

	memset(&m_RemoteVideoCap,0,sizeof(SIPVideoCap));
	memset(&m_RemoteAudioCap,0,sizeof(SIPAudioCap));
	memset(&m_RemoteDualVideoCap,0,sizeof(SIPDualVideoCap));
	memset(&m_RemoteDataCap,0,sizeof(SIPDataCap));

	SIPVideoCap &remoteVideoCap=m_RemoteVideoCap;
	SIPAudioCap &remoteAudioCap=m_RemoteAudioCap;
	SIPDualVideoCap &remoteDualVideoCap=m_RemoteDualVideoCap;
	SIPDataCap &remoteDataCap=m_RemoteDataCap;

	PJ_LOG(3,("SIPCall","ParseRemoteSDP func call begin."));
	CopySIPSDP(false,(pjmedia_sdp_session *)sdp);
	PJ_LOG(3,("SIPCall","ParseRemoteSDP func call CopySIPSDP after."));
	//Parse SDP
	//status=pjmedia_sdp_validate(sdp);
	//if(status!=PJ_SUCCESS) return PJ_FALSE;

	m_BfcpFloorControlType=e_Floor_Control_None;
	m_BfcpSetupType=e_BFCP_Setup_None;
	m_nBFCPTransportType=e_BFCP_Transport_UDP;//1 udp 2 tcp
	m_nBFCPConfId=0;
	m_nBFCPUserId=0;
	m_nBFCPFloorId=0;
	m_nBFCPMediaLabel=0;
	m_nDualVideoLabel=0;
	m_nBFCPTransportPort=0;

	int bfcpMedia=0;
	int mediaLabel=0;
	int applicationRtpMedia=0;

	strncpy(strRemoteIP,sdp->conn->addr.ptr,sdp->conn->addr.slen);
	SetRemoteIP(strRemoteIP);

	for(i=0;i<sdp->media_count;i++)
	{
		int nPayloadType=-1;
		m=sdp->media[i];
		if(m->desc.port<=0) continue;

		int mediaContent=0; //1 main media 2 slides media

		if( (m->bandw_count>0) && (m->bandw!=NULL) )
		{
			//Bandwidth Information (b): TIAS:384000
			if(pj_stricmp(&(m->bandw[0]->modifier),&(pj_str("TIAS")))==0)
				nBandWidth=m->bandw[0]->value/1000;

			if(pj_stricmp(&(m->bandw[0]->modifier),&(pj_str("AS")))==0)
				nBandWidth=m->bandw[0]->value;

			if(nBandWidth <=0) nBandWidth =384;
		}
		//get reliablertp
		int reliablertp=0;
		unsigned int k=0;
		for(k=0;k<m->attr_count;k++)
		{
			if(pj_stricmp2(&(m->attr[k]->name),"reliablertp")==0)
			{
				//reliablertp BNF: a=reliablertp:<fec-enable>
				if(pj_stricmp2(&(m->attr[k]->value),"fec-enable")==0)
				{
					reliablertp=1;
					break;
				}
			}
		}

		//get content get label
		for(k=0;k<m->attr_count;k++)
		{
			if(pj_stricmp2(&(m->attr[k]->name),"content")==0)
			{
				//content BNF: a=content:<token>
				if(pj_stricmp2(&(m->attr[k]->value),"main")==0)
				{
					mediaContent=1;					
				}
				else if(pj_stricmp2(&(m->attr[k]->value),"slides")==0)
				{
					mediaContent=2;					
				}
			}
			else if(pj_stricmp2(&(m->attr[k]->name),"label")==0)
			{
				mediaLabel=pj_strtoul(&(m->attr[k]->value) );
			}
		}

		//get bfcp attr
		if (pj_stricmp2(&(m->desc.media),"application")==0)
		{
			char * subStr=NULL;
			pj_str_t STR_BFCP = { "BFCP", 4};
			subStr=pj_stristr(&(m->desc.transport),&STR_BFCP);
			if(subStr!=NULL) bfcpMedia=1;

			subStr=NULL;
			pj_str_t STR_RTPAVP = { "RTP/AVP", 7};
			subStr=pj_stristr(&(m->desc.transport),&STR_RTPAVP);
			if(subStr!=NULL) applicationRtpMedia=1;

			if(bfcpMedia==1)
			{
			bfcpMedia=0;
			//get BFCP Transport Type
			pj_str_t STR_UDP = { "UDP", 3};
			pj_str_t STR_TCP = { "TCP", 3};
			subStr=NULL;
			subStr=pj_stristr(&(m->desc.transport),&STR_UDP);
			if(subStr!=NULL) m_nBFCPTransportType=e_BFCP_Transport_UDP;
			subStr=NULL;
			subStr=pj_stristr(&(m->desc.transport),&STR_TCP);
			if(subStr!=NULL) m_nBFCPTransportType=e_BFCP_Transport_TCP;

			//get bfcp port
			m_nBFCPLastTransportPort=m_nBFCPTransportPort;
			m_nBFCPTransportPort=m->desc.port;
			//get attr
			int contentSlides=0;
			for(k=0;k<m->attr_count;k++)
			{
				if(pj_stricmp2(&(m->attr[k]->name),"floorctrl")==0)
				{
					if(pj_stricmp2(&(m->attr[k]->value),"s-only")==0)
					{
						m_BfcpFloorControlType=e_Floor_Control_Server;						
					}
					else if(pj_stricmp2(&(m->attr[k]->value),"c-only")==0)
					{
						m_BfcpFloorControlType=e_Floor_Control_Client;						
					}
					else if(pj_stricmp2(&(m->attr[k]->value),"c-s")==0)
					{
						m_BfcpFloorControlType=e_Floor_Control_All;						
					}
				}
				else if(pj_stricmp2(&(m->attr[k]->name),"confid")==0)
				{
					m_nBFCPConfId=pj_strtoul(&(m->attr[k]->value));
				}
				else if(pj_stricmp2(&(m->attr[k]->name),"userid")==0)
				{
					m_nBFCPUserId=pj_strtoul(&(m->attr[k]->value));
				}
				else if(pj_stricmp2(&(m->attr[k]->name),"setup")==0)
				{
					if(pj_stricmp2(&(m->attr[k]->value),"passive")==0)
					{
						m_BfcpSetupType=e_BFCP_Setup_Passive;						
					}
					else if(pj_stricmp2(&(m->attr[k]->value),"actpass")==0)
					{
						m_BfcpSetupType=e_BFCP_Setup_Actpass;						
					}
					else if(pj_stricmp2(&(m->attr[k]->value),"active")==0)
					{
						m_BfcpSetupType=e_BFCP_Setup_Active;						
					}
				}
				else if(pj_stricmp2(&(m->attr[k]->name),"floorid")==0)
				{
					//floorid:2 mstrm:12
					char * subStr=NULL;
					pj_str_t str_mstrm = { "mstrm", 5};
					subStr=pj_stristr(&(m->attr[k]->value),&str_mstrm);
					if(subStr!=NULL) 
					{
						char floorId[33]={0};
						strncpy(floorId,m->attr[k]->value.ptr,subStr - m->attr[k]->value.ptr -1);
						m_nBFCPFloorId=atoi(floorId);
						char bfcpLabel[33]={0};
						int slen=0;
						int flooridStrLen=subStr - m->attr[k]->value.ptr;
						slen=m->attr[k]->value.slen -(flooridStrLen+ str_mstrm.slen+1);
						strncpy(bfcpLabel,subStr+ str_mstrm.slen+1,slen);
						m_nBFCPMediaLabel=atoi(bfcpLabel);
					}
				}
			}

			}

			if(applicationRtpMedia==1)
			{
			applicationRtpMedia=0;
			int capIndex=-1;
			int index=-1;	
			remoteDataCap.m_nCapNum=0;
			nRemoteDataPort=m->desc.port;

			const pjmedia_sdp_attr *rtcpa=NULL;
			int payloadTypeArray[MAX_DATACAP_NUM]={0};
			unsigned nPayloadTypeNum=0;
			memset(payloadTypeArray,0,sizeof(payloadTypeArray));
			GetAllPayloadTypeFromAttr(m,payloadTypeArray,nPayloadTypeNum);
			if(reliablertp) m_nRemoteDataReliableRTP=1;
			for(j=0;j<nPayloadTypeNum;j++)
			{
				pjmedia_sdp_rtpmap rtpmap;
				pjmedia_sdp_fmtp fmtp;
				char params[256]={0};
				char *pn=0;						
				const pjmedia_sdp_attr *ra=NULL;
				const pjmedia_sdp_attr *fa=NULL;
				memset(params,0,256);
				nPayloadType=payloadTypeArray[j];
				char strFmt[4]={0};
				pj_utoa(nPayloadType,strFmt);
				pj_str_t fmt=pj_str(strFmt);
				ra = pjmedia_sdp_media_find_attr2(m, "rtpmap", &fmt);
				if (ra) status=pjmedia_sdp_attr_get_rtpmap(ra, &rtpmap);

				if (status!=PJ_SUCCESS) ra=NULL;
				fa = pjmedia_sdp_media_find_attr2(m, "fmtp",&fmt);
				if (fa) 
					status=pjmedia_sdp_attr_get_fmtp(fa, &fmtp);
				if (status!=PJ_SUCCESS) fa=NULL;
				if(ra) 
				{
					if(nPayloadType>=96)
					{
						if( (pj_stricmp(&(rtpmap.enc_name),&(pj_str("H224")))==0) )
						{
							if(rtpmap.clock_rate==4800) 
							{								
								capIndex++; if(capIndex >= MAX_DATACAP_NUM) capIndex =0;
								SIPDATACAPBILITY &cap=remoteDataCap.m_Caps[capIndex];
								cap.nPayLoad=nPayloadType;
								cap.nClockRate=rtpmap.clock_rate;
								cap.DataCodecID=DATA_CODEC_H224;
								remoteDataCap.m_nCapNum=capIndex+1;
							}							
						}
					}
				}
				else if(fa)	{ }
			}
			rtcpa = pjmedia_sdp_attr_find2(m->attr_count, m->attr,"rtcp", NULL);
			if (rtcpa) 
			{
				pjmedia_sdp_rtcp_attr rtcp;
				status = pjmedia_sdp_attr_get_rtcp(rtcpa, &rtcp);
				if(status==PJ_SUCCESS) nRemoteDataRTCPPort=rtcp.port;
				else nRemoteDataRTCPPort=nRemoteDataPort+1;
			}
			else nRemoteDataRTCPPort=nRemoteDataPort+1;

			m_nRemoteDataPort=nRemoteDataPort;
			m_nRemoteDataRTCPPort=nRemoteDataRTCPPort;
			}
		}

		//1 get PT rtpmap fmtp
		//2 rtpmap ? yes            --> set rtpmap params  fmtp ? yes set fmtp params
		//3 rtpmap ? no  fmtp ? yes --> set everyone knows pt type set fmtp params
		//4 rtpmap ? no  fmtp ? no  --> set everyone knows pt type
		if (pj_stricmp(&(m->desc.media),&STR_AUDIO)==0)
		{
			int capIndex=-1;
			int index=-1;	
			remoteAudioCap.m_nCapNum=0;
			nRemoteAudioPort=m->desc.port;
			const pjmedia_sdp_attr *rtcpa=NULL;
			int payloadTypeArray[MAX_VIDEOCAP_NUM+MAX_AUDIOCAP_NUM]={0};
			unsigned nPayloadTypeNum=0;
	
			memset(payloadTypeArray,0,sizeof(payloadTypeArray));
			GetAllPayloadTypeFromAttr(m,payloadTypeArray,nPayloadTypeNum);
			if(reliablertp) m_nRemoteAudioReliableRTP=1;
			for(j=0;j<nPayloadTypeNum;j++)
			{
				pjmedia_sdp_rtpmap rtpmap;
				pjmedia_sdp_fmtp fmtp;
				char params[256]={0};
				char *pn=0;						
				const pjmedia_sdp_attr *ra=NULL;
				const pjmedia_sdp_attr *fa=NULL;

				memset(params,0,256);

				nPayloadType=payloadTypeArray[j];
				char strFmt[4]={0};
				pj_utoa(nPayloadType,strFmt);
				pj_str_t fmt=pj_str(strFmt);

				ra = pjmedia_sdp_media_find_attr2(m, "rtpmap", &fmt);
				if (ra) 
					status=pjmedia_sdp_attr_get_rtpmap(ra, &rtpmap);

				if (status!=PJ_SUCCESS) ra=NULL;

				fa = pjmedia_sdp_media_find_attr2(m, "fmtp",&fmt);
				if (fa) 
					status=pjmedia_sdp_attr_get_fmtp(fa, &fmtp);

				if (status!=PJ_SUCCESS) fa=NULL;

				if(ra) 
				{					
					if(nPayloadType==0)
					{
						capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
						SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
						cap.AudioCodecID=X_AUDIO_CODEC_G711U;
						cap.nPayLoad=nPayloadType;
						cap.nClockRate=rtpmap.clock_rate;
						remoteAudioCap.m_nCapNum=capIndex+1;

					}
					if(nPayloadType==3)
					{
						capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
						SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
						cap.AudioCodecID=X_AUDIO_CODEC_GSM;
						cap.nPayLoad=nPayloadType;
						cap.nClockRate=rtpmap.clock_rate;
						remoteAudioCap.m_nCapNum=capIndex+1;
					}
					else if(nPayloadType==8) 
					{
						capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
						SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
						cap.AudioCodecID=X_AUDIO_CODEC_G711A;
						cap.nPayLoad=nPayloadType;
						cap.nClockRate=rtpmap.clock_rate;
						remoteAudioCap.m_nCapNum=capIndex+1;
					}
					else if(nPayloadType==9) 
					{
						capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
						SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
						cap.AudioCodecID=X_AUDIO_CODEC_G722_64;
						cap.nPayLoad=nPayloadType;
						cap.nClockRate=rtpmap.clock_rate;
						remoteAudioCap.m_nCapNum=capIndex+1;
					}
					else if(nPayloadType==18)
					{
						//20121115 chang cheng zuan tan use:has not fmtp param
						////if (fa) 
						{ 
							////strncpy(params,fmtp.fmt_param.ptr,fmtp.fmt_param.slen);
							//Media Attribute (a): fmtp:18 annexb=no
							////if((pn=strstr(params,"annexb=no"))!=NULL) 
							{
								capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
								SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
								cap.AudioCodecID=X_AUDIO_CODEC_G729;
								cap.nPayLoad=nPayloadType;
								cap.nClockRate=rtpmap.clock_rate;
								remoteAudioCap.m_nCapNum=capIndex+1;
							}
						}
					}
					else if(nPayloadType>=96)  
					{
						if( (pj_strcmp(&(rtpmap.enc_name),&(pj_str("G7221")))==0) || (pj_strcmp(&(rtpmap.enc_name),&(pj_str("g7221")))==0) )
						{
							int bitRate=0;

							if (fa) 
							{ 
								strncpy(params,fmtp.fmt_param.ptr,fmtp.fmt_param.slen);
								//Media Attribute (a): fmtp:102 bitrate=32000
								if((pn=strstr(params,"bitrate="))!=NULL) 
								{
									char buf[64]={0};
									strncpy(buf,pn+8,5);									
									pj_str_t  strBitRate=pj_str(buf);
									bitRate=pj_strtoul(&strBitRate);
								}
							}
							if(rtpmap.clock_rate==16000) 
							{
								if(bitRate==24000)
								{
									capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
									SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
									cap.nPayLoad=nPayloadType;
									cap.nClockRate=rtpmap.clock_rate;
									cap.AudioCodecID=X_AUDIO_CODEC_G7221_7_24;
									remoteAudioCap.m_nCapNum=capIndex+1;
								}
								else if(bitRate==32000)
								{
									capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
									SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
									cap.nPayLoad=nPayloadType;
									cap.nClockRate=rtpmap.clock_rate;
									cap.AudioCodecID=X_AUDIO_CODEC_G7221_7_32;
									remoteAudioCap.m_nCapNum=capIndex+1;
								}
							}
							else if(rtpmap.clock_rate==32000) 
							{
								if(bitRate==24000)
								{
									capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
									SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
									cap.nPayLoad=nPayloadType;
									cap.nClockRate=rtpmap.clock_rate;
									cap.AudioCodecID=X_AUDIO_CODEC_G7221_14_24;
									remoteAudioCap.m_nCapNum=capIndex+1;
								}
								else if(bitRate==32000)
								{
									capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
									SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
									cap.nPayLoad=nPayloadType;
									cap.nClockRate=rtpmap.clock_rate;
									cap.AudioCodecID=X_AUDIO_CODEC_G7221_14_32;
									remoteAudioCap.m_nCapNum=capIndex+1;
								}
								else if(bitRate==48000)
								{
									capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
									SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
									cap.nPayLoad=nPayloadType;
									cap.nClockRate=rtpmap.clock_rate;
									cap.AudioCodecID=X_AUDIO_CODEC_G7221_14_48;
									remoteAudioCap.m_nCapNum=capIndex+1;
								}
							}

						}
					}
				}
				else if(fa)
				{
					switch (nPayloadType)
					{
					case 0:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G711U;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 3:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_GSM;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 8:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G711A;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 18:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G729;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					}
				}

				else
				{
					switch (nPayloadType)
					{
					case 0:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G711U;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 3:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_GSM;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 8:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G711A;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					case 18:
						{
							capIndex++; if(capIndex >= MAX_AUDIOCAP_NUM) capIndex =0;
							SIPAUDIOCAPBILITY &cap=remoteAudioCap.m_Caps[capIndex];
							cap.AudioCodecID=X_AUDIO_CODEC_G729;
							cap.nPayLoad=nPayloadType;
							cap.nClockRate=8000;
							remoteAudioCap.m_nCapNum=capIndex+1;
						}
						break;
					}
				}
			}

			rtcpa = pjmedia_sdp_attr_find2(m->attr_count, m->attr,"rtcp", NULL);
			if (rtcpa) {
				pjmedia_sdp_rtcp_attr rtcp;
				status = pjmedia_sdp_attr_get_rtcp(rtcpa, &rtcp);
				if(status==PJ_SUCCESS) nRemoteAudioRTCPPort=rtcp.port;
				else 
					nRemoteAudioRTCPPort=nRemoteAudioPort+1;
			}
			else 
				nRemoteAudioRTCPPort=nRemoteAudioPort+1;

			SetRemoteAudioPort(nRemoteAudioPort,nRemoteAudioRTCPPort);
		}

		if (pj_stricmp(&(m->desc.media),&STR_VIDEO)==0)
		{
			SIPVideoCap remoteMediaCap;
			memset(&remoteMediaCap,0,sizeof(SIPVideoCap));

			//{{gsh 处理视频及辅流能力集代码相同 20120206
			int capIndex=-1;
			int index=-1;
			remoteMediaCap.m_nCapNum=0;
			nRemoteVideoPort=m->desc.port;
			const pjmedia_sdp_attr *rtcpa=NULL;
			int payloadTypeArray[MAX_VIDEOCAP_NUM+MAX_AUDIOCAP_NUM]={0};
			unsigned nPayloadTypeNum=0;

			memset(payloadTypeArray,0,sizeof(payloadTypeArray));
			GetAllPayloadTypeFromAttr(m,payloadTypeArray,nPayloadTypeNum);

			for(j=0;j<nPayloadTypeNum;j++)
			{
				pjmedia_sdp_rtpmap rtpmap;
				pjmedia_sdp_fmtp fmtp;
				char *pn=0;						
				const pjmedia_sdp_attr *ra=NULL;
				const pjmedia_sdp_attr *fa=NULL;
				
				nPayloadType=payloadTypeArray[j];
				char strFmt[4]={0};
				pj_utoa(nPayloadType,strFmt);
				pj_str_t fmt=pj_str(strFmt);

				ra = pjmedia_sdp_media_find_attr2(m, "rtpmap", &fmt);
				if (ra) status=pjmedia_sdp_attr_get_rtpmap(ra, &rtpmap);
				if (status!=PJ_SUCCESS) ra=NULL;
				fa = pjmedia_sdp_media_find_attr2(m, "fmtp",&fmt);
				if (fa) status=pjmedia_sdp_attr_get_fmtp(fa, &fmtp);
				if (status!=PJ_SUCCESS) fa=NULL;

				if(ra)
				{
					if(nPayloadType==31) 
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H261;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if(nPayloadType==34) 
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H263;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if( (pj_strcmp(&(rtpmap.enc_name),&(pj_str("H263-1998")))==0) || (pj_strcmp(&(rtpmap.enc_name),&(pj_str("h263-1998")))==0) )
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H263P;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if( (pj_strcmp(&(rtpmap.enc_name),&(pj_str("H263-2000")))==0) || (pj_strcmp(&(rtpmap.enc_name),&(pj_str("h263-2000")))==0) )
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H263PP;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if( (pj_strcmp(&(rtpmap.enc_name),&(pj_str("H264")))==0) || (pj_strcmp(&(rtpmap.enc_name),&(pj_str("h264")))==0) )
					{
						if(fa) 
						{
							//H.264 : fmtp:106 profile-level-id=42800d; max-mbps=20000; max-fs=1792
							if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("profile-level-id=")) ))!=NULL) 
							{
								capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
								SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
								cap.eCapType=VIDEC_CODEC_H264;
								cap.nPayLoad=nPayloadType;
								cap.nBitrate=nBandWidth;//rtpmap.clock_rate;						
								//remoteMediaCap.m_nCapNum=capIndex+1;
								{
								char fbuf[16]={0};
								strncpy(fbuf,pn+17+4,2);
								pj_str_t strtbuf=pj_str(fbuf);
								cap.nLevel=pj_strtoul2(&strtbuf,NULL,16);
								}
								//notice highprofile
								{
								char fbuf[16]={0};
								strncpy(fbuf,pn+17,2);
								pj_str_t strtbuf=pj_str(fbuf);
								cap.nProfile=pj_strtoul2(&strtbuf,NULL,16);
								}

								if(cap.nProfile==0x42) cap.eCapType=VIDEC_CODEC_H264;
								if(cap.nProfile==0x64) cap.eCapType=VIDEC_CODEC_H264_SVC;//notice wait find value

								remoteMediaCap.m_nCapNum=capIndex+1;
							}
							if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("max-mbps=")) ))!=NULL) 
							{
								SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
								int theLen=strlen("max-mbps=");
								int valuelen=0;
								int pi=0;
								for(pi=0;pi<16;pi++)
								{
									if(pn[theLen+pi] <'0' || pn[theLen+pi] >'9') break;
								}
								{
								char fbuf[32]={0};
								strncpy(fbuf,pn+theLen,pi);
								pj_str_t strtbuf=pj_str(fbuf);
								cap.nQCIF=pj_strtoul2(&strtbuf,NULL,10);
								}
							}
							if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("max-fs=")) ))!=NULL) 
							{
								SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
								int theLen=strlen("max-fs=");
								int valuelen=0;
								int pi=0;
								for(pi=0;pi<16;pi++)
								{
									if(pn[theLen+pi] <'0' || pn[theLen+pi] >'9') break;
								}
								{
								char fbuf[32]={0};
								strncpy(fbuf,pn+theLen,pi);
								pj_str_t strtbuf=pj_str(fbuf);
								cap.nCIF=pj_strtoul2(&strtbuf,NULL,10);
								}
							}
							SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
							cap.nVideoFormatType = GetCallFormatTypeFromH264Param(cap.nProfile,cap.nLevel,cap.nCIF,cap.nQCIF,cap.nBitrate,0);
							float fFrame=GetFrameRateFromH264Param(cap.nCIF,cap.nQCIF);
							cap.nFrame =fFrame;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
					}

					//逐步解析出各个参数 set fmtp params
					SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
					if(cap.eCapType !=VIDEC_CODEC_H264 && cap.eCapType !=VIDEC_CODEC_H264_SVC && fa)
					{
						//空格去除的问题 “=”符号二侧不允许有空格
						//H.263 H.261 : fmtp:96 CIF4=2;CIF=1;QCIF=1;SQCIF=1;F;J;T
						char paramBuf[512]={0};
						strncpy(paramBuf,fmtp.fmt_param.ptr,fmtp.fmt_param.slen);
						//to upper
						int strSlen=fmtp.fmt_param.slen;
						char * strPtr=fmtp.fmt_param.ptr;					
						for(int strPn=0;strPn<strSlen;strPn++)
						{							
							if( (paramBuf[strPn]>'a') && (paramBuf[strPn]<'z') )
								paramBuf[strPn]=paramBuf[strPn]-32;							
						}

						//{{ get maxCustomPictureWidth						
						int nCustomFormatWidth=0;
						int nCustomFormatHeight=0;
						int nCustomFormatOption=0;
						int nWidthValue=0;
						int nHeightValue=0;
						int nOptionValue=0;						
						strPtr=paramBuf;
						while(1)
						{
							nWidthValue=0;
							nHeightValue=0;
							nOptionValue=0;
							pn=NULL;
							if( (pn=strstr(strPtr,"CUSTOM=") ) != NULL)
							{
								char * pnDot1=NULL;
								pnDot1=strstr(strPtr,",");
								if(pnDot1!=NULL)
								{
									char fbuf[16]={0};
									strncpy(fbuf,pn+7,pnDot1-pn-7);									
									nWidthValue=atoi(fbuf);
									strPtr=pnDot1+1;
								}
								else 
								{
									strPtr=pn+7; //move pointer								
								}

								pnDot1=strstr(strPtr,",");
								if(pnDot1!=NULL)
								{
									char fbuf[16]={0};
									strncpy(fbuf,strPtr,pnDot1-strPtr);									
									nHeightValue=atoi(fbuf);
									strPtr=pnDot1+1;

									if(nWidthValue>nCustomFormatWidth)
									{
										nCustomFormatWidth=nWidthValue;
										nCustomFormatHeight=nHeightValue;							
									}
									strncpy(fbuf,strPtr,2);	
									//not only ; or space 
									/*if( (* (fbuf+1)==';') || (* (fbuf+1)==' ') )*/ strPtr=strPtr+1;
									if( (* fbuf>='0') && (* fbuf<='9') )
									{
										fbuf[1]=0;	
										strPtr=strPtr+1;
										nOptionValue=atoi(fbuf);										
										nCustomFormatOption=nOptionValue;
									}
								}
							}
							else break;
						}

						if(nCustomFormatWidth>0 && nCustomFormatHeight>0)
						{
							cap.nLevel=nCustomFormatWidth;
							cap.nProfile=nCustomFormatHeight;
							cap.nVideoFormatType = GetVideoFormatType(cap.nLevel,cap.nProfile);
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						//}} get maxCustomPictureWidth

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("cif=")) ) )!=NULL)
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ) )!=1)
								if(*(pn+4)>'0') cap.nCIF=1;

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ) )!=NULL) 
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("sqcif=")) ))!=1)
								if(*(pn+5)>'0') cap.nQCIF=1;	

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("cif4=")) ))!=NULL) 
							if(*(pn+5)>'0') cap.n4CIF=1;
						if(nCustomFormatWidth <=0 || nCustomFormatHeight <=0)
						{
							if(cap.n4CIF >0)
							{
								cap.nVideoFormatType = VIDEO_FORMAT_4CIF;
								cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
							}
							else if(cap.nCIF >0)
							{
								cap.nVideoFormatType = VIDEO_FORMAT_CIF;
								cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
							}
							else if(cap.nQCIF >0)
							{
								cap.nVideoFormatType = VIDEO_FORMAT_QCIF;
								cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
							}
						}
					}

				}
				else if(fa) //注意 此只针对有fmtp无rtpmap的处理
				{					
					if(nPayloadType==34)
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H263;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("cif=")) ))!=NULL)
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ))!=1)
								if(*(pn+4)>'0') cap.nCIF=1;

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ))!=NULL) 
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("sqcif=")) ))!=1)
								if(*(pn+5)>'0') cap.nQCIF=1;	

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("cif4=")) ))!=NULL) 
							if(*(pn+5)>'0') cap.n4CIF=1;

						if(cap.n4CIF >0)
						{
							cap.nVideoFormatType = VIDEO_FORMAT_4CIF;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						else if(cap.nCIF >0)
						{
							cap.nVideoFormatType = VIDEO_FORMAT_CIF;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						else if(cap.nQCIF >0)
						{
							cap.nVideoFormatType = VIDEO_FORMAT_QCIF;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if(nPayloadType==31)
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H261;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("cif=")) ))!=NULL)
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ))!=1)
								if(*(pn+4)>'0') cap.nCIF=1;

						if((pn=pj_stristr(&fmtp.fmt_param,&(pj_str("qcif=")) ))!=NULL) 
							if((pn-pj_stristr(&fmtp.fmt_param,&(pj_str("sqcif=")) ))!=1)
								if(*(pn+5)>'0') cap.nQCIF=1;

						if(cap.nCIF >0)
						{
							cap.nVideoFormatType = VIDEO_FORMAT_CIF;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						else if(cap.nQCIF >0)
						{
							cap.nVideoFormatType = VIDEO_FORMAT_QCIF;
							cap.nFormatValue = GetVideoFormatValue((VIDEO_FORMAT_TYPE)cap.nVideoFormatType);
						}
						remoteMediaCap.m_nCapNum=capIndex+1;
					}

				}
				else 
				{
					if(nPayloadType==31) 
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H261;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						cap.nQCIF=1;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
					if(nPayloadType==34) 
					{
						capIndex++; if(capIndex >= MAX_VIDEOCAP_NUM) capIndex =0;
						SIPVIDEOCAPBILITY &cap=remoteMediaCap.m_Caps[capIndex];
						cap.eCapType=VIDEC_CODEC_H263;
						cap.nPayLoad=nPayloadType;
						cap.nBitrate=nBandWidth;//rtpmap.clock_rate;
						cap.nQCIF=1;
						remoteMediaCap.m_nCapNum=capIndex+1;
					}
				}
			}

			rtcpa = pjmedia_sdp_attr_find2(m->attr_count, m->attr,"rtcp", NULL);
			if (rtcpa) {
				pjmedia_sdp_rtcp_attr rtcp;
				status = pjmedia_sdp_attr_get_rtcp(rtcpa, &rtcp);
				if(status==PJ_SUCCESS) nRemoteVideoRTCPPort=rtcp.port;
				else 
					nRemoteVideoRTCPPort=nRemoteVideoPort+1;
			}
			else 
				nRemoteVideoRTCPPort=nRemoteVideoPort+1;

			//}}gsh 处理视频及辅流能力集代码相同 20120206
			if( (mediaContent==0) || (mediaContent==1) )
			//if( (bfcpMedia==0) || ( (bfcpMedia==1) && (mediaContent==1) ) )
			{
				memcpy(&m_RemoteVideoCap,&remoteMediaCap,sizeof(SIPVideoCap));
				m_nVideoLabel=mediaLabel;
				if(reliablertp) m_nRemoteVideoReliableRTP=1;
				SetRemoteVideoPort(nRemoteVideoPort,nRemoteVideoRTCPPort);
			}
			else if(mediaContent==2)
			//if( (bfcpMedia==1) /*&& (mediaContent==2)*/  && (m_nBFCPMediaLabel==mediaLabel) )
			{
				memcpy(&m_RemoteDualVideoCap,&remoteMediaCap,sizeof(SIPVideoCap));
				m_nDualVideoLabel=mediaLabel;
				if(reliablertp) m_nRemoteDualVideoReliableRTP=1;
				SetRemoteDualVideoPort(nRemoteVideoPort,nRemoteVideoRTCPPort);
				m_bHasRemoteDualSDP=true;
			}
		}
	}
	//PJ_LOG(3,("SIPCall","ParseRemoteSDP func call end."));

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2) 

#else 
	if(m_CallDirection == SIPCall::CALL_DIR_IN)
	{
		if(m_RemoteVideoCap.m_nCapNum >=1)
			SIPCallMgr::Instance().m_nMainVideoBitrate=m_RemoteVideoCap.m_Caps[0].nBitrate;
		if(m_RemoteDualVideoCap.m_nCapNum >=1)
			SIPCallMgr::Instance().m_nSecondVideoMaxBitrate=m_RemoteDualVideoCap.m_Caps[0].nBitrate;
	}
#endif

	return PJ_FALSE;
}


void SIPCall::OnDualVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(SIPCallMgr::Instance().m_nUseDualVideoToken <= 0)
	{
		if(m_nRecvDV == 0)
		{
			SIPCallMgr::Instance().OnCallSecondVideoStart(m_strCallID);
			m_nRecvDV++;
		}
	}
	OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	SIPCallMgr::Instance().OnDAP_RecvRemoteSecondVideoRTPPacket(m_strCallID.c_str(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	//sip_register_thread();
	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
	{
		m_nSecondVideoPacketTimestamp=nTimestamp;
		if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) SIPDualVideoSendPictureFastUpdate();
	}
}

void SIPCall::OnDualVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen)
{

}

void SIPCall::OnDualVideoRTPSessionCallbackRequestKeyFrame(void)
{

}

void SIPCall::SetBFCPConfId(int confId)
{
	m_nRequestConfId=confId;
}

void SIPCall::SetBFCPFloorId(int floorId)
{
	m_nRequestFloorId=floorId;
}

void SIPCall::SetBFCPUserId(int userId)
{
	m_nRequestUserId=userId;
}

void SIPCall::SetBFCPRequestPriority(unsigned short priority)
{
	m_nRequestPriority=priority;
}

void SIPCall::SetBFCPFloorControlType(EBFCPFloorControlType type)
{
	m_nRequestFloorControlType=type;
}

void SIPCall::SetBFCPSetupType(EBFCPSetupType type)
{
	m_nRequestSetupType=type;
}

void SIPCall::SetBFCPTransportType(EBFCPTransportType type)
{
	m_nRequestTransportType=type;
}

//call back
void SIPCall::OnSIPBFCPTCPConnected(EBFCPTCPCONType nType)
{
	if(m_pBFCPTCP==NULL)
	{
		//m_pBFCPTCP=m_pBFCPTCPClient;
		if(nType==e_BFCP_TCP_Client)
		{
			m_pBFCPTCP=m_pBFCPTCPClient;
		}
		if(nType==e_BFCP_TCP_Server)
		{
			m_pBFCPTCP=m_pBFCPTCPServer;
		}
		if(m_pBFCPTCP) m_pBFCPTCP->SendBFCPHello();
	}
}

void SIPCall::OnSIPBFCPCallbackFloorRequest(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorId)
{
	OnRecvGenericRequest(nTransactionId,nUserId,nFloorId);
}

void SIPCall::OnSIPBFCPCallbackFloorRequestStatus(unsigned short nFloorReqId, unsigned short nReqStatus)
{
	OnRecvGenericResponse(nFloorReqId,nReqStatus);
}

void SIPCall::OnSIPBFCPCallbackFloorRelease(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorReqId)
{
	OnRecvGenericCommand(nTransactionId,nUserId,nFloorReqId);
}

void SIPCall::OnSIPBFCPCallbackHello()
{
	SendBFCPHelloACK();
}

//call back
void DualVideoRTPSessionCallback::OnVideoRTPSessionCallbackRTPPacket(VideoRTPSession*pVideoRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(m_pSipCall) m_pSipCall->OnDualVideoRTPSessionCallbackRTPPacket(pVideoRTPSession, pRTPPacketData, nRTPPacketLen, nPayloadType, nRTPHeaderLen,usSequence, ulTimestamp);
}

void DualVideoRTPSessionCallback::OnVideoRTPSessionCallbackRcvdFrame(const void*pFrameData,int nFrameLen)
{
	if(m_pSipCall) m_pSipCall->OnDualVideoRTPSessionCallbackRcvdFrame(pFrameData,nFrameLen);
}

void DualVideoRTPSessionCallback::OnVideoRTPSessionCallbackRequestKeyFrame(void)
{
	if(m_pSipCall) m_pSipCall->OnDualVideoRTPSessionCallbackRequestKeyFrame();
}

//recv bfcp msg
void SIPCall::OnRecvGenericRequest(unsigned short transactionId,unsigned short userId,unsigned short floorId)
{
	DebugOutput(2,"OnRecvGenericRequest callID:%s\n",this->m_strCallID.c_str());
	if(m_bHasRemoteH239Video ==false) return;

	m_nTransactionId =transactionId;
	m_nUserId =userId;
	m_nFloorId =floorId;

//此MCU接口执行后不会再执行到实体CALL中的处理,如也需要执行则移到下面或注释掉下面两行
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	SIPCallMgr::Instance().OnDAP_RecvCallDualVideoControl(m_strCallID,1,transactionId,userId,floorId);
	//A7 add
	//modify 20140712 not use
	//if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	//{
	//	//是否停止由上层业务处理
	//	//StopSecondVideo();
	//}
	////m_bH239ReceiverOpen=true;
	//m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
	//// 通知业务层对端请求发双流，由业务层决定是否允许发起
	//SIPCallMgr::Instance().OnCallSecondVideoStart(m_strCallID);

#else

	int nUseType=SIPCallMgr::Instance().m_nUseDualVideoTokenType;
	if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	{
		StopSecondVideo();
	}

	if(nUseType==0)
	{
		SendGenericResponse(transactionId,userId,floorId,BFCP_PENDING);
		SendGenericResponse(0,userId,floorId,BFCP_ACCEPTED);
		SendGenericResponse(0,userId,floorId,BFCP_GRANTED);
	}
	if(nUseType==1)
	{
		SIPSendPermitDualVideoToken();
	}

	if (m_bH239ReceiverOpen==false)
	{
		m_bH239ReceiverOpen=true;
		m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
		SIPCallMgr::Instance().OnCallSecondVideoStart(m_strCallID);
	}

#endif

}

void SIPCall::OnRecvGenericResponse( unsigned short floorReqId, unsigned short reqStatus)
{
	DebugOutput(2,"OnRecvGenericResponse callID:%s\n",this->m_strCallID.c_str());
	if(m_bHasRemoteH239Video ==false) return;

	bool bAccept = false;
	int status=reqStatus;

//modify 20140712 add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

		SIPCallMgr::Instance().OnDAP_RecvCallDualVideoControl(m_strCallID,2,0,0,floorReqId,status);

#else

	if(reqStatus==BFCP_GRANTED)
	{
		bAccept = true;

		if(bAccept) status=1;
		SIPCallMgr::Instance().OnDAP_RecvCallDualVideoControl(m_strCallID,2,0,0,floorReqId,status);

		if (bAccept)
		{
			m_nSecondVideoDir=IMXEC_CALL_DIR_OUT;
			OnLocalSecondVideoStarted();
		}
		else
		{
			//notice the terminal refuse is not call modify 20140715
			//OnLocalSecondVideoStop();
		}
	}

#endif
}

void SIPCall::OnRecvGenericCommand(unsigned short transactionId,unsigned short userId,unsigned short floorReqId)
{
	DebugOutput(2,"OnRecvGenericCommand callID:%s\n",this->m_strCallID.c_str());
	if(m_bHasRemoteH239Video ==false) return;

	int nUseType=SIPCallMgr::Instance().m_nUseDualVideoTokenType;

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	SIPCallMgr::Instance().OnDAP_RecvCallDualVideoControl(m_strCallID,4,transactionId,userId,floorReqId);
	//A7 add
	//modify 20140712 not use
	//if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	//{
	//	//是否停止由上层业务处理
	//	//StopSecondVideo();
	//}
	//else
	//{
	//	m_bH239ReceiverOpen=false;
	//	SIPCallMgr::Instance().OnCallSecondVideoStop(m_strCallID);
	//	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	//	//调整视频双流比例
	//	//SetVideoBitRate();
	//}
	//if(nUseType==0)
	//{
	//	SendGenericResponse(transactionId,userId,m_nRequestFloorId,BFCP_RELEASED);//允许
	//}
	//if(nUseType==1)
	//{
	//}

#else

	if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	{
		StopSecondVideo();
	}
	else
	{
		m_bH239ReceiverOpen=false;
		SIPCallMgr::Instance().OnCallSecondVideoStop(m_strCallID);
		m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	}
	if(nUseType==0)
	{
	//notice if BFCP_RELEASED not control , mcu OnRecvGenericResponse callback while  
	SendGenericResponse(transactionId,userId,m_nRequestFloorId,BFCP_RELEASED);//允许释放
	}
	if(nUseType==1)
	{
	}

#endif

}


/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCall::StartSecondVideo (void)
{
	if (m_bHasRemoteH239Video)
	{
		DebugOutput(2,"StartSecondVideo callID:%s\n",this->m_strCallID.c_str());

		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			DebugOutput(2,"StartSecondVideo Dir==IMXEC_CALL_DIR_OUT callID:%s\n",this->m_strCallID.c_str());
			return 0;
		}
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_IN)
		{
			StopSecondVideo();
		}
		SendGenericRequest();
		return 0;
	}

	return -1;
}

/******************************************************************************
* StopSecondVideo
*描述：停止辅流
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCall::StopSecondVideo (void)
{
	if (m_bHasRemoteH239Video)
	{
		DebugOutput(2,"StopSecondVideo callID:%s\n",this->m_strCallID.c_str());

		m_bH239ReceiverOpen=false;
		//SendGenericCommand();
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			OnLocalSecondVideoStop();
		}
		return 0;
	}
	return -1;
}

/******************************************************************************
* GetSecondVideoDirection
*描述：获取辅流的方向
*输入：	cszCallID		-呼叫标识
*输出： nDir			-辅流的方向
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int SIPCall::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	nDir=m_nSecondVideoDir;
	return 0;
}

void SIPCall::OnLocalSecondVideoStarted()
{
	if (m_bH239ReceiverOpen==false)
	{
		m_bH239ReceiverOpen=true;
		m_nSecondVideoDir=IMXEC_CALL_DIR_OUT;
		//media open and notify
		SIPCallMgr::Instance().OnCallSecondVideoStart(m_strCallID,1);
	}
}

void SIPCall::OnLocalSecondVideoStop()
{
	m_nRecvDV = 0;
	m_bH239ReceiverOpen=false;
	//media close and notify
	SIPCallMgr::Instance().OnCallSecondVideoStop(m_strCallID,1);
	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
}

int SIPCall::GetSupportSecondVideo(void)
{
	return (m_bHasRemoteH239Video?1:0);
}
void SIPCall::SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	if(m_bHasRemoteH239Video)
	{
		//XAutoLock l(m_csDualVideoRTPSession);
		if (m_pDualVideoRTPSession!=NULL)
		{
			PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPPacketData;
			unsigned char*pPayloadData=(unsigned char*)pRTPPacketData+RTP_HEADER_LEN;
			int nPayloadLen=nRTPPacketLen-RTP_HEADER_LEN;
			unsigned char nPayloadType=pRTPHeader->type;
			bool bMarker=(pRTPHeader->marker!=0);
			m_pDualVideoRTPSession->SendRTPPacket(pPayloadData,nPayloadLen,nPayloadType,bMarker,bMarker?3003:0,(unsigned char*)pRTPPacketData,nRTPPacketLen);
		}
	}
	else
	{
		SendMainVideo(pRTPPacketData,nRTPPacketLen);
	}

}

void SIPCall::OnDualVideoRTPClose()
{
	VideoRTPSession*pVideoRTPSession=m_pDualVideoRTPSession;
	{
		//XAutoLock l(m_csDualVideoRTPSession);
		m_pDualVideoRTPSession=NULL;
	}
	if (pVideoRTPSession!=NULL)
	{
		pVideoRTPSession->Close();
		delete pVideoRTPSession;
		pVideoRTPSession=NULL;
	}
}

int SIPCall::CheckMediaHangupCall(void) //check media stop hungup sip call
{
	m_nLocalHungUP=2;
	m_rSIPUser.CloseRingTone();
	pjsip_inv_session *inv=m_pInvSessionUAC;
	pjsip_tx_data *tdata=NULL;
	pj_status_t status=-1;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	int code=PJSIP_SC_SERVICE_UNAVAILABLE;
	CallStateDestroyed((SIPMediaCall *)this,(char *)m_strCallID.c_str(),(pjsip_status_code)code,-1);
	sip_register_thread();
	if(inv!=NULL)
	{
		pjsip_inv_terminate(inv,code,PJ_TRUE);
	}
	CloseBFCPTransports();
	return 0;
}

int SIPCall::HangupSIPCall(void) //hungup sip protocol call
{
	LogFile(NULL,NULL,"","","SIPCall::HangupSIPCall         111");

	m_rSIPUser.CloseRingTone();
	pjsip_inv_session *inv=m_pInvSessionUAC;
	pjsip_tx_data *tdata=NULL;
	pj_status_t status=-1;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;

	if(m_CallDirection == SIPCall::CALL_DIR_OUT) m_pInvSessionUAC =NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) m_pInvSessionUAS =NULL;

	sip_register_thread();
	if(inv!=NULL)
	{
		int code=PJSIP_SC_OK;
		if (inv->state == PJSIP_INV_STATE_CONFIRMED)
			code = PJSIP_SC_OK;
		else if (inv->role == PJSIP_ROLE_UAS)
			code = PJSIP_SC_DECLINE;
		else
			code = PJSIP_SC_REQUEST_TERMINATED;

		status = pjsip_inv_end_session(inv, code, 0, &tdata);

		if( status != PJ_SUCCESS || tdata==NULL)
		{
			pjsip_inv_terminate(inv,code,PJ_TRUE);
			LogFile(NULL,NULL,"","","SIPCall::HangupSIPCall         111 01");

			return -1;
		}
		//set request line uri is addr
		if(SIPCallMgr::Instance().m_nUseInvReqLine)
		{
			char * pConfCentralNum=SIPCallMgr::Instance().m_strConfCentralNum;
			int nConfCentralNumLen=strlen(pConfCentralNum);
			if(nConfCentralNumLen <=0)
			{
				char * addr=(char *)SIPCallMgr::Instance().m_strInvReqLineAddr.c_str();
				m_rSIPUser.SetRequestLineAddr(tdata,addr,(char *)m_strCalledID.c_str());
			}
			else
			{
				char addrBuf[256]={0};
				std::string strAddr=SIPCallMgr::Instance().m_strOutboundProxy[0] +4;
				if(strAddr.length() <=0) strAddr=m_rSIPUser.m_strSIPProxy;
				int nPos=strAddr.find(":");
				if (nPos <= 0)
				{
					sprintf(addrBuf,"%s@%s:5060",pConfCentralNum,strAddr.c_str());
				}
				else
				{
					sprintf(addrBuf,"%s@%s",pConfCentralNum,strAddr.c_str());
				}
				m_rSIPUser.SetRequestLineAddr(tdata,addrBuf);
			}
		}

		status = m_rSIPUser.SIPInvSendMessage(inv, tdata,0);
		//status = pjsip_inv_send_msg(inv, tdata);
		LogFile(NULL,NULL,"","","SIPCall::HangupSIPCall         111 02");

		if(status != PJ_SUCCESS) return -1;
		pjsip_inv_terminate(inv,code,PJ_TRUE);
	}
	LogFile(NULL,NULL,"","","SIPCall::HangupSIPCall         222");

	return 0;
}

//Disconnect call  response 488 and sdp 
int SIPCall::NotAcceptCall(int callId)
{
	int code=PJSIP_SC_NOT_ACCEPTABLE_HERE;
	pj_pool_t * pool=m_pPOOL;
	pjsip_inv_session *inv=m_pInvSessionUAC;
	pjsip_tx_data *tdata;
	pj_status_t status;
	pjmedia_sdp_session *sdp = NULL;
	int nSupportDual=GetMgrSupportH239();
	if(CreateLocalSDP(pool,&sdp,nSupportDual) == false) return -1;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv!=NULL)
	{
		status = pjsip_inv_end_session(inv, code, 0, &tdata);
		if( (status != PJ_SUCCESS) || (tdata==NULL) ) return -1;
		//Add SDP in 488 status
		if (tdata->msg->type==PJSIP_RESPONSE_MSG && code==PJSIP_SC_NOT_ACCEPTABLE_HERE) 
		{
			pjsip_create_sdp_body(tdata->pool, sdp, &tdata->msg->body);
		}
		//Send the message
		status = m_rSIPUser.SIPInvSendMessage(inv, tdata);
		if(status != PJ_SUCCESS) return -1;
	}
	return 0;
}

//Put the specified call on hold.
pj_status_t SIPCall::SetSipCallHold(int callId)
{
    pjmedia_sdp_session *sdp = NULL;
    pjsip_tx_data *tdata;
    pj_status_t status;
	pj_pool_t * pool=m_pPOOL;
	pjsip_inv_session *inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return -1;
	sip_register_thread();
	sdp=m_pCallLocalSDP;
    //Create re-INVITE with new offer
	std::string  strContact="";
	m_rSIPUser.GetCallContactUri(m_strLocalIP,strContact);
	pj_str_t contact = pj_str((char *)strContact.c_str());
    status = pjsip_inv_reinvite( inv,NULL/*&contact*/, sdp, &tdata);
	if( (status != PJ_SUCCESS) || (tdata==NULL) ) return status;

    //Send the request
    status = m_rSIPUser.SIPInvSendMessage( inv, tdata,0);
    if (status != PJ_SUCCESS) {
	return status;
    }

    return PJ_SUCCESS;
}

pj_bool_t SIPCall::OnRecvInviteRequest( pjsip_rx_data *rdata ,int nFindSDP)
{
	pjsip_dialog * pSipdlg;
	pjsip_inv_session  *pInvSessionUas;
	pj_str_t contact;
	pjmedia_sdp_session *sdp = NULL;
	pj_pool_t * pool=m_pPOOL;
	pjsip_tx_data *tdata;
	pj_status_t status;
	std::string  strContact="";
	PJ_LOG(3,("SIPCall","OnRecvInviteRequest func call begin."));

	//add by sgb 根据Invite的TransportType来决定contact地址
	//m_rSIPUser.GetCallContactUri(m_strLocalIP,strContact);
	//m_rSIPUser.GetCallContactUriUas( m_strLocalIP, strContact, rdata);
	//add end
	m_rSIPUser.GetCallContactUri( m_strLocalIP, strContact);

	m_nInviteFindSDP=nFindSDP;

	if(nFindSDP<=0)
	{
		if(GenerateLocal183SDP(&sdp,0) <0 ) return PJ_FALSE;
		//notice 
		m_rSIPUser.ICEEncodeInSDP(*this,pool,false,sdp,m_pCallRemoteSDP);
		CopySIPSDP(true,sdp);
	}
	if(m_pSipDialog==NULL )
	{
		m_rSIPUser.PlayRingTone();
		unsigned	inv_option = 0;
		inv_option |= PJSIP_INV_SUPPORT_100REL;
		//inv_option |= PJSIP_INV_SUPPORT_TIMER;
		contact = pj_str((char *)strContact.c_str());
		//Create UAS
		status = pjsip_dlg_create_uas(pjsip_ua_instance(), rdata,&contact, &pSipdlg);
		if (status != PJ_SUCCESS)  
		{
			return PJ_FALSE;
		}
		status = pjsip_inv_create_uas(pSipdlg, rdata,NULL, inv_option, &pInvSessionUas);
		if (status != PJ_SUCCESS)  
		{
			return PJ_FALSE;
		}
		status = pjsip_inv_initial_answer(pInvSessionUas, rdata,100, NULL, NULL, &tdata);
		if( (status != PJ_SUCCESS) || (tdata==NULL) )
		{
			return PJ_FALSE;
		}
		status = m_rSIPUser.SIPInvSendMessage(pInvSessionUas, tdata);
		if (status != PJ_SUCCESS)  
		{
			return PJ_FALSE;
		}

		if(nFindSDP > 0)
		{
			status = pjsip_inv_initial_answer(pInvSessionUas, rdata, 180, NULL,NULL, &tdata);
		}
		else
		{
			pjsip_inv_set_local_sdp(pInvSessionUas, sdp);//或此默认不支持辅流  要么回调由外面设置是否支持辅流(需快速返回)
			status = pjsip_inv_initial_answer(pInvSessionUas, rdata, 183, NULL,NULL, &tdata);
			m_rSIPUser.SIPHeaderAddCapability(tdata,"Supported","100rel,precondition");
			char szRSeq[8]={0};
			itoa(m_nRSeq,szRSeq,10);
			m_rSIPUser.SIPHeaderAddCapability(tdata,"RSeq",szRSeq);
			m_nRSeq++;
			m_rSIPUser.SIPMessageAddRequireCapability(tdata);
		}

		if( (status != PJ_SUCCESS) || (tdata==NULL) ) return PJ_FALSE;
		status = m_rSIPUser.SIPInvSendMessage(pInvSessionUas, tdata);
		if (status != PJ_SUCCESS)
		{
			return PJ_FALSE;
		}

		SetInvSessionUAS(pInvSessionUas);
		SetSIPDialog(pSipdlg);
		SetCallDirection(SIPCall::CALL_DIR_IN);
		PJ_LOG(3,("SIPCall","OnRecvInviteRequest func call SIPInvSendMessage after."));

		if(nFindSDP <= 0)
		{
			//then reply 180 200 OK
			status = pjsip_inv_initial_answer(pInvSessionUas, rdata, 180, NULL,NULL, &tdata);
			if( (status != PJ_SUCCESS) || (tdata==NULL) ) return PJ_FALSE;
			char szRSeq[8]={0};
			itoa(m_nRSeq,szRSeq,10);
			m_rSIPUser.SIPHeaderAddCapability(tdata,"RSeq",szRSeq);
			m_nRSeq++;
			m_rSIPUser.SIPMessageAddRequireCapability(tdata);
			m_rSIPUser.SIPHeaderAddCapability(tdata,"Supported","100rel");
			m_pSend180Data=tdata;
			//status = pjsip_endpt_create_response(g_pSipEndPoint, rdata, 200, NULL, &tdata);
			status = pjsip_dlg_create_response(m_pSipDialog, rdata, 200, NULL, &tdata);
			if( (status != PJ_SUCCESS) || (tdata==NULL) ) return PJ_FALSE;
			m_pSend200Data=tdata;
		}

		//callback
		if(nFindSDP > 0)
		{
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
			//SIPCallMgr::Instance().OnDAP_CallRingIn(m_strCallID,m_strRemoteIP,m_strRemoteIP,"","",m_rSIPUser.m_strUserID);
			SIPCallMgr::Instance().OnDAP_CallRingIn(m_strCallID,m_strUserID,m_strUserName,"","",m_rSIPUser.m_strUserID);

#else
			////added by jiangdingfeng 2013-11-18 解决同时两个终端呼叫导致imexc报错
			//if (CallMgr::Instance().HasActiveCall() == true)
			//{
			//	HangupSIPCall();
			//	return PJ_FALSE;
			//}
			SIPCallMgr::Instance().OnCallRingIn(m_strCallID);
#endif

		}

	}
	else
	{
		sdp=m_pCallLocalSDP;
		return Send200SDPResponse(rdata,nFindSDP,sdp);
	}

	return PJ_FALSE;
}

pj_bool_t SIPCall::Send200SDPResponse( pjsip_rx_data *rdata ,int nFindSDP,pjmedia_sdp_session *sdp)
{
	pjsip_inv_session  *pInvSessionUas=NULL;
	pj_pool_t * pool=m_pPOOL;
	pjsip_tx_data *tdata;
	pj_status_t status;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) pInvSessionUas=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) pInvSessionUas=m_pInvSessionUAS;
	if(pInvSessionUas==NULL) return PJ_FALSE;
	//Create 2xx answer
	status = pjsip_dlg_create_response(m_pSipDialog, rdata, 200, NULL, &tdata);
	if (status != PJ_SUCCESS) return PJ_FALSE;
	//Attach SDP body
	if(sdp)
	{
		pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
		pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
	}
	//Create UAS transaction for this request
	pjsip_transaction *tsx = NULL;
	tsx=pjsip_rdata_get_tsx(rdata);
	if(tsx == NULL && rdata->msg_info.msg->line.req.method.id != PJSIP_ACK_METHOD) 
	{
		pjsip_dialog *dlg=m_pSipDialog;
		status = pjsip_tsx_create_uas(dlg->ua, rdata, &tsx);
		if (status != PJ_SUCCESS) return PJ_FALSE;
		pjsip_tsx_recv_msg(tsx,rdata);
		tsx->mod_data[dlg->ua->id] = dlg;
		++dlg->tsx_count;
	}
	//Save the invite transaction.
	if(tsx!=NULL) pInvSessionUas->invite_tsx = tsx;

	//Invoke Session Timers
	//pjsip_timer_update_resp(pInvSessionUas, tdata);
	status = m_rSIPUser.SIPInvSendMessage(pInvSessionUas, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	//media update add
	if(sdp && nFindSDP > 0) MediaOpenSetState();
	return PJ_TRUE;
}

void SIPCall::SetInvSessionUAC(pjsip_inv_session  *pInvSessionUac)
{
	m_pInvSessionUAC=pInvSessionUac;
}
void SIPCall::SetInvSessionUAS(pjsip_inv_session  *pInvSessionUas)
{
	m_pInvSessionUAS=pInvSessionUas;
}
void SIPCall::SetSIPDialog(pjsip_dialog * pSipDlg)
{
	m_pSipDialog=pSipDlg;
}
void SIPCall::SetCallDirection(CALL_DIRECTION dir)
{
	m_CallDirection=dir;
}

void SIPCall::GetAudioCodecInfo(X_AUDIO_CODEC_ID nAudioCodecID,int & nSamplesPerFrame)
{
	std::string strCodecName="";
	int nSampleRate=8000;
	int nBitrate=64000;
	int nMSPerFrame=0;

	switch (nAudioCodecID)
	{
	case X_AUDIO_CODEC_G711U:
		{
			strCodecName="PCMU";
			nSampleRate=8000;
			nSamplesPerFrame=80;
			nBitrate=64000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G711A:
		{
			strCodecName="PCMA";
			nSampleRate=8000;
			nSamplesPerFrame=80;
			nBitrate=64000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_L16_8KHZ:
		{
			strCodecName="L16";
			nSampleRate=8000;
			nSamplesPerFrame=160;
			nBitrate=128000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_L16_16KHZ:
		{
			strCodecName="L16";
			nSampleRate=16000;
			nSamplesPerFrame=320;
			nBitrate=256000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_L16_32KHZ:
		{
			strCodecName="L16";
			nSampleRate=32000;
			nSamplesPerFrame=640;
			nBitrate=512000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_L16_48KHZ:
		{
			strCodecName="L16";
			nSampleRate=48000;
			nSamplesPerFrame=480;
			nBitrate=768000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G7231_63:
		{
			strCodecName="G7231";
			nSampleRate=8000;
			nSamplesPerFrame=240;
			nBitrate=6300;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G729:
		{
			strCodecName="G729";
			nSampleRate=8000;
			nSamplesPerFrame=80;
			nBitrate=8000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_GSM:
		{
			strCodecName="GSM";
			nSampleRate=8000;
			nSamplesPerFrame=160;
			nBitrate=13200;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_EVRC:
		{
			strCodecName="EVRC";
			nSampleRate=8000;
			nBitrate=8550;
			nSamplesPerFrame=160;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;

	case X_AUDIO_CODEC_ILBC_15:
		{
			strCodecName="iLBC";
			nSampleRate=8000;
			nSamplesPerFrame=160;
			nBitrate=15200;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_7_24:
	case X_AUDIO_CODEC_G7221_7_24:
		{
			strCodecName="G7221";
			nSampleRate=16000;
			nSamplesPerFrame=320;
			nBitrate=24000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_7_32:
	case X_AUDIO_CODEC_G7221_7_32:
		{
			strCodecName="G7221";
			nSampleRate=16000;
			nSamplesPerFrame=320;
			nBitrate=32000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_14_24:
	case X_AUDIO_CODEC_G7221_14_24:
	case X_AUDIO_CODEC_AUDEC_24:
		{
			strCodecName="G7221";
			nSampleRate=32000;
			nSamplesPerFrame=640;
			nBitrate=24000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_14_32:
	case X_AUDIO_CODEC_G7221_14_32:
		{
			strCodecName="G7221";
			nSampleRate=32000;
			nSamplesPerFrame=640;
			nBitrate=32000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_14_48:
	case X_AUDIO_CODEC_G7221_14_48:
		{
			strCodecName="G7221";
			nSampleRate=32000;
			nSamplesPerFrame=640;
			nBitrate=48000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;

	case X_AUDIO_CODEC_OPUS_FWB_24:
		{
			strCodecName="OPUS";
			nSampleRate=48000;
			nSamplesPerFrame=960;
			nBitrate=24000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_OPUS_FWB_48:
		{
			strCodecName="OPUS";
			nSampleRate=48000;
			nSamplesPerFrame=960;
			nBitrate=48000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G719_32:
		{
			strCodecName="G719";
			nSampleRate=48000;
			nSamplesPerFrame=960;
			nBitrate=32000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G719_48:
		{
			strCodecName="G719";
			nSampleRate=48000;
			nSamplesPerFrame=960;
			nBitrate=48000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G719_64:
		{
			strCodecName="G719";
			nSampleRate=48000;
			nSamplesPerFrame=960;
			nBitrate=64000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	case X_AUDIO_CODEC_G722_64:
		{
			strCodecName="G722";
			nSampleRate=16000;
			nSamplesPerFrame=160;
			nBitrate=64000;
			nMSPerFrame=(nSamplesPerFrame*1000/nSampleRate);
		}
		break;
	default:
		break;

	}
}

AudioRTPSession* SIPCall::GetOpenAudioRTPSession()
{
	pjmedia_endpt *	pMediaEndpoint=SIPCallMgr::Instance().GetMediaEndpoint();
	if(m_pAudioRTPSession==NULL)
	{
		pjmedia_transport *	 pICETransport=SIPCallMgr::Instance().GetICEMediaTrans();
		AudioRTPSession* pAudioRTPSession=new AudioRTPSession(*this);
		if(pAudioRTPSession && pICETransport)
		{
			pAudioRTPSession->PJSIPICEOpen(pMediaEndpoint,(int)SIPCallMgr::Instance().m_bICEEnable,pICETransport);
			m_pAudioRTPSession=pAudioRTPSession;
			m_pAudioTransport=pICETransport;
		}
	}
	return m_pAudioRTPSession;
}

VideoRTPSession* SIPCall::GetOpenVideoRTPSession()
{
	pjmedia_endpt *	pMediaEndpoint=SIPCallMgr::Instance().GetMediaEndpoint();
	if(m_pVideoRTPSession==NULL)
	{
		pjmedia_transport *	 pICETransport=SIPCallMgr::Instance().GetICEMediaTrans();
		VideoRTPSession* pVideoRTPSession=new VideoRTPSession(*this);
		if(pVideoRTPSession  && pICETransport)
		{
			pVideoRTPSession->PJSIPICEOpen(pMediaEndpoint,(int)SIPCallMgr::Instance().m_bICEEnable,pICETransport);
			m_pVideoRTPSession=pVideoRTPSession;
			m_pVideoTransport=pICETransport;
		}
	}
	return m_pVideoRTPSession;
}

VideoRTPSession* SIPCall::GetOpenDualVideoRTPSession()
{
	pjmedia_endpt *	pMediaEndpoint=SIPCallMgr::Instance().GetMediaEndpoint();
	if(m_pDualVideoRTPSession==NULL)
	{
		pjmedia_transport *	 pICETransport=SIPCallMgr::Instance().GetICEMediaTrans();
		VideoRTPSession* pVideoRTPSession=new VideoRTPSession(*this);
		if(pVideoRTPSession  && pICETransport)
		{
			pVideoRTPSession->PJSIPICEOpen(pMediaEndpoint,(int)SIPCallMgr::Instance().m_bICEEnable,pICETransport);
			m_pDualVideoRTPSession=pVideoRTPSession;
			m_pDualVideoTransport=pICETransport;
		}
	}
	return m_pDualVideoRTPSession;
}

void SIPCall::CopySIPSDP(bool bLocal,pjmedia_sdp_session *sdp)
{
	if(sdp==NULL) return;
	pj_pool_t * sdpPool = m_pPOOL;
	int type=0;
	if(bLocal)
	{
		m_pCallLocalSDP=pjmedia_sdp_session_clone(sdpPool,sdp);
		type=1;
	}
	else
	{
		m_pCallRemoteSDP=pjmedia_sdp_session_clone(sdpPool,sdp);
		type=0;
	}
	int nCapPrint=SIPCallMgr::Instance().GetDAPCapPrintEnable();
	if(nCapPrint>0)
	{
		char sdpBuf[4096]={0};
		int nSdpBufLen=4095;
		pjmedia_sdp_print(sdp,sdpBuf,nSdpBufLen);
		SIPCallMgr::Instance().OutPutCapContent(m_strCallID.c_str(),type,sdpBuf);
	}
}

bool SIPCall::ICECheckCand(pj_pool_t *sdp_pool,pjmedia_sdp_session *sdp_local,pjmedia_sdp_session *sdp_rem,int nMediaIndex,pjmedia_transport * pMediaTransport)
{
	pjmedia_sdp_media * rem_m = sdp_rem->media[nMediaIndex];
	bool bFindCand=false;
	int status=-1;
	if(pMediaTransport==NULL) return bFindCand;
	//Find the default addresses in a=candidate attributes
	const pj_str_t STR_CANDIDATE	= { "candidate", 9};
	for (unsigned int i=0; i<rem_m->attr_count; ++i)
	{
		pj_ice_sess_cand cand;
		if (pj_strcmp(&rem_m->attr[i]->name, &STR_CANDIDATE)!=0) continue;
		status = pj_parse_cand(pMediaTransport, sdp_pool,&rem_m->attr[i]->value, &cand);
		if (status == PJ_SUCCESS)
		{
			bFindCand=true;
			break;
		}
	}
	return bFindCand;
}

void SIPCall::MediaOpenSetState()
{
	if(!m_bNegCapAudio) return ;
	bool bUseICE=SIPCallMgr::Instance().m_bICEEnable;
	if(bUseICE==0)
	{
		m_nAudioOpenState = 2;
		m_nVideoOpenState = 2;
		m_nDualVideoOpenState = 2;
		//h224
	}
	else
	{
		bool bFindCand=false;
		bFindCand=ICECheckCand(m_pPOOL,m_pCallLocalSDP,m_pCallRemoteSDP,0,m_pAudioTransport);
		if(bFindCand) m_nAudioOpenState=1;
		else m_nAudioOpenState=2;
		bFindCand=ICECheckCand(m_pPOOL,m_pCallLocalSDP,m_pCallRemoteSDP,1,m_pVideoTransport);
		if(bFindCand) m_nVideoOpenState=1;
		else m_nVideoOpenState=2;
		bFindCand=ICECheckCand(m_pPOOL,m_pCallLocalSDP,m_pCallRemoteSDP,2,m_pDualVideoTransport);
		if(bFindCand) m_nDualVideoOpenState=1;
		else m_nDualVideoOpenState=2;
		//h224
	}
	//media update
	SIPCallMgr::Instance().OnCallSDPNegotiate(this);
}

void SIPCall::OnICETransCheckComplete(pjmedia_transport * m_pMediaTransport,int nMediaType)
{
	static int nMediaCount=0;
	switch(nMediaType)
	{
	case 1:
		if(m_nAudioOpenState==1) m_nAudioOpenState=2;
		nMediaCount++;
		break;
	case 2:
		if(m_nVideoOpenState==1) m_nVideoOpenState=2;
		nMediaCount++;
		break;
	case 3:
		if(m_nDualVideoOpenState==1) m_nDualVideoOpenState=2;
		nMediaCount++;
		break;
		//h224
	default:
		break;
	}
	//media update
	if(nMediaCount>=m_nMediaCount)
	{
		SIPCallMgr::Instance().OnCallSDPNegotiate(this);
		nMediaCount=0;
	}
}

int SIPCall::CompareMediaTransport(pjmedia_transport * pMediaTransport)
{
	int nMediaType=0;
	if(pMediaTransport == m_pAudioTransport)
		nMediaType=1;
	else if(pMediaTransport == m_pVideoTransport)
		nMediaType=2;
	else if(pMediaTransport == m_pDualVideoTransport)
		nMediaType=3;
	//h224
	return nMediaType;
}

void SIPCall::CloseRingTone()
{
	m_rSIPUser.CloseRingTone();
}


pj_bool_t SIPCall::OnRecvUpdateRequest( pjsip_rx_data *rdata ,int nFindSDP)
{
	pjsip_inv_session  *pInvSessionUas=NULL;
	pjmedia_sdp_session *sdp = NULL;
	pj_pool_t * pool=m_pPOOL;
	pjsip_tx_data *tdata;
	pj_status_t status;
	std::string  strContact="";
	PJ_LOG(3,("SIPCall","OnRecvUpdateRequest func call begin."));

	m_rSIPUser.GetCallContactUri(m_strLocalIP,strContact);
	if(OnOffer(&sdp) <0 ) 
	{
		//SDP Negotiation has failed  Disconnect call  response 488 sdp
		NotAcceptCall(m_nCallId);
		sdp = NULL;
		PJ_LOG(3,("SIPCall","OnRecvUpdateRequest func call NotAcceptCall after."));
		return PJ_FALSE;
	}
	m_rSIPUser.ICEEncodeInSDP(*this,pool,false,sdp,m_pCallRemoteSDP);
	CopySIPSDP(true,sdp);
	PJ_LOG(3,("SIPCall","OnRecvUpdateRequest func call ICEEncodeInSDP after."));

	if(m_pSipDialog!=NULL )
	{
		pjsip_endpoint * pEndpoint= GetSipEndpoint();
		pjsip_response_addr res_addr;
		if(m_CallDirection == SIPCall::CALL_DIR_OUT) pInvSessionUas=m_pInvSessionUAC;
		if(m_CallDirection == SIPCall::CALL_DIR_IN) pInvSessionUas=m_pInvSessionUAS;
		if(pInvSessionUas==NULL) return PJ_FALSE;
		//Create basic response.
		//status = pjsip_endpt_create_response(pEndpoint, rdata, 200, NULL, &tdata);
		status = pjsip_dlg_create_response(pInvSessionUas->dlg, rdata, PJSIP_SC_OK, NULL, &tdata);
		if (status == PJ_SUCCESS && sdp)
		{
			//Attach SDP body
			pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
			pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
		}
		//Send response
		pjsip_get_response_addr(tdata->pool, rdata, &res_addr);
		status = pjsip_endpt_send_response(pEndpoint, &res_addr, tdata, NULL, NULL);
		if (status != PJ_SUCCESS) pjsip_tx_data_dec_ref(tdata);
		return PJ_FALSE;
	}

	return PJ_FALSE;
}

//Send update request message.
pj_bool_t SIPCall::SIPSendUpdateRequest(pjmedia_sdp_session *sdp)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;
	 const pjsip_method pjsip_update_method ={ PJSIP_OTHER_METHOD, { "UPDATE",6}};

	pjsip_dlg_inc_lock(inv->dlg);
	//Create request
	status = pjsip_dlg_create_request(inv->dlg, &pjsip_update_method,-1, &tdata);
	if (status != PJ_SUCCESS) 
	{
		pjsip_dlg_dec_lock(inv->dlg);
		return PJ_FALSE;
	}

	//Attach SDP body
	if(sdp)
	{
		pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
		pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
	}

	pjsip_dlg_dec_lock(inv->dlg);

	status = pjsip_timer_update_req(inv, tdata);
	//status = pjsip_inv_update(pInvSessionUas, NULL, sdp, &tdata);

	if (status != PJ_SUCCESS) return PJ_FALSE;
	m_rSIPUser.SIPMessageAddUserAgentCapability(tdata);
	//m_rSIPUser.SIPMessageAddDateCapability(tdata);
	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	m_bSendUpdateSDP=true;
	return PJ_TRUE;
}

//Send SUBSCRIBE request message.
pj_bool_t SIPCall::SIPSendSubscribeRequest(pjmedia_sdp_session *sdp)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;
	const pjsip_method pjsip_subscribe_method ={ PJSIP_OTHER_METHOD, { "SUBSCRIBE",9}};

	pjsip_dlg_inc_lock(inv->dlg);
	//Create request
	status = pjsip_dlg_create_request(inv->dlg, &pjsip_subscribe_method,-1, &tdata);
	if (status != PJ_SUCCESS) 
	{
		pjsip_dlg_dec_lock(inv->dlg);
		return PJ_FALSE;
	}

	//Attach SDP body
	if(sdp)
	{
		pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
		pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
	}

	pjsip_dlg_dec_lock(inv->dlg);

	m_rSIPUser.SIPHeaderAddCapability(tdata,"Supported","eventlist,ZTE-SUB");
	m_rSIPUser.SIPHeaderAddCapability(tdata,"Expires","3600");
	m_rSIPUser.SIPHeaderAddCapability(tdata,"Event","conference");
	m_rSIPUser.SIPHeaderAddCapability(tdata,"Accept","application/conference-info+xml");
	m_rSIPUser.SIPHeaderAddCapability(tdata,"User-Agent","Conf-client/CMIMS");

	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	return PJ_TRUE;
}

//Send INFO request message.
pj_bool_t SIPCall::SIPSendInfoRequest(char * content,char * mimeType)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pj_str_t mime_type;
	pj_str_t sipcontent;
	pjsip_media_type media_type;
	pj_str_t body;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;
	sip_register_thread();
	const pjsip_method pjsip_info_method = { PJSIP_OTHER_METHOD, {"INFO", 4}};
	const pj_str_t mime_text_plain = pj_str("application/media_control+xml");

	if(mimeType != NULL) mime_type=pj_str(mimeType);
	if(content != NULL) sipcontent=pj_str(content);
	if( (mimeType == NULL) || (mime_type.slen) <= 0)
	{
		mime_type = mime_text_plain;
	}

	pjsip_dlg_inc_lock(inv->dlg);

	//Create request
	status = pjsip_dlg_create_request(inv->dlg, &pjsip_info_method,-1, &tdata);
	if (status != PJ_SUCCESS) 
	{
		pjsip_dlg_dec_lock(inv->dlg);
		return PJ_FALSE;
	}
	if(content)
	{
		//Parse MIME type 
		pjsip_parse_media_type(tdata->pool, &mime_type, &media_type);
		//Create data and attach to the request.
		pj_strdup_with_null(tdata->pool, &body, &sipcontent);
		//Add message body
		tdata->msg->body = pjsip_msg_body_create(tdata->pool, &media_type.type,&media_type.subtype,&body);
		if (tdata->msg->body == NULL)
		{
			pjsip_dlg_dec_lock(inv->dlg);
			return PJ_FALSE;
		}
	}
	pjsip_dlg_dec_lock(inv->dlg);

	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	return PJ_TRUE;
}

pj_bool_t SIPCall::SIPMainVideoSendPictureFastUpdate()
{
	//因发info时如对方不响应则会自动产生因超时原因挂断,需要时再放开测试
	pj_bool_t status=PJ_FALSE;
	char * msg="<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n"
		"<media_control>\r\n"
		 "<vc_primitive>\r\n"
		  "<to_encoder>\r\n"
		   "<picture_fast_update>\r\n"
		   "</picture_fast_update>\r\n"
		  "</to_encoder>\r\n"
		 "</vc_primitive>\r\n"
		"</media_control>\r\n";
	char * mimeType="application/media_control+xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

pj_bool_t SIPCall::SIPDualVideoSendPictureFastUpdate()
{
	//因发info时如对方不响应则会自动产生因超时原因挂断,需要时再放开测试
	pj_bool_t status=PJ_FALSE;
	char * msg="<?xml version=\"1.0\" encoding=\"utf-8\" ?>\r\n"
		"<media_control>\r\n"
		 "<vc_primitive>\r\n"
		  "<to_encoder>\r\n"
		   "<dual_video>\r\n"
		   "</dual_video>\r\n"
		   "<picture_fast_update>\r\n"
		   "</picture_fast_update>\r\n"
		  "</to_encoder>\r\n"
		 "</vc_primitive>\r\n"
		"</media_control>\r\n";
	char * mimeType="application/media_control+xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

int SIPCall::GenerateSDP(pjmedia_sdp_session **p_sdp)
{
	return m_rSIPUser.GenerateSDP( * this,m_pPOOL,m_strLocalIP,p_sdp);
}

//Send update request recv response 180 200
void SIPCall::Send200ACKNoSDP()
{
	pj_status_t status;
	pjsip_inv_session  *inv=NULL;
	if(m_bSendUpdateSDP)
	{
		if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
		if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
		if(inv==NULL) return ;
		if(m_pSend180Data)
			status = m_rSIPUser.SIPInvSendMessage(inv, m_pSend180Data);
		if(m_pSend200Data)
			status = m_rSIPUser.SIPInvSendMessage(inv, m_pSend200Data);
		m_bSendUpdateSDP=false;
	}
}

void SIPCall::SetSDPNegStateDone()
{
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return ;
	if(inv->neg)
	{
		pjmedia_sdp_neg_state neg_state;
		neg_state = pjmedia_sdp_neg_get_state(inv->neg);
		if(neg_state != PJMEDIA_SDP_NEG_STATE_DONE) pjmedia_sdp_neg_cancel_offer(inv->neg);
	}
}

void SIPCall::CheckZeroSetBitrate(int nWidth ,int nHeight,int & nBitrate)
{
	if(nBitrate > 0)  return;
	switch(nWidth)
	{
	case 176:
		if(nHeight==144) nBitrate=128;
		break;
	case 352:
		if(nHeight==288) nBitrate=384;
		break;
	case 640:
		if(nHeight==480) nBitrate=768;
		break;
	case 704:
		if(nHeight==576) nBitrate=768;
		break;
	case 800:
		if(nHeight==600) nBitrate=768;
		break;
	case 1024:
		if(nHeight==768) nBitrate=1024;
		break;
	case 1408:
		if(nHeight==1152) nBitrate=1536;
		break;
	case 1280:
		if(nHeight==360) nBitrate=1024;
		if(nHeight==720) nBitrate=1024;
		if(nHeight==1024) nBitrate=1536;
		break;
	case 1920:
		if(nHeight==540) nBitrate=1536;
		if(nHeight==1080) nBitrate=1536;
		break;
	default:
		nBitrate=768;
		break;
	}
}

//Send PRACK request message.
pj_bool_t SIPCall::SIPSendPRACKRequest(pjmedia_sdp_session *sdp)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;
	const pjsip_method pjsip_prack_method ={ PJSIP_OTHER_METHOD, { "PRACK",5}};

	pjsip_dlg_inc_lock(inv->dlg);
	//Create request
	status = pjsip_dlg_create_request(inv->dlg, &pjsip_prack_method,-1, &tdata);
	if (status != PJ_SUCCESS) 
	{
		pjsip_dlg_dec_lock(inv->dlg);
		return PJ_FALSE;
	}

	//Attach SDP body
	if(sdp)
	{
		pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
		pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
	}

	pjsip_dlg_dec_lock(inv->dlg);

	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	return PJ_TRUE;
}


pj_bool_t SIPCall::SIPSendApplyDualVideoToken()
{
	pj_bool_t status=PJ_FALSE;

	char * msg=
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<query type=\'apply\' xmlns=\'xconf:video:duovideotoken\'>"
		"</query>";
	char * mimeType="application/xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

pj_bool_t SIPCall::SIPSendReleaseDualVideoToken()
{
	pj_bool_t status=PJ_FALSE;
	char * msg=
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<query type=\'release\' xmlns=\'xconf:video:duovideotoken\'>"
		"</query>";
	char * mimeType="application/xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

pj_bool_t SIPCall::SIPSendPermitDualVideoToken()
{
	pj_bool_t status=PJ_FALSE;
	char * msg=
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		"<query type=\"permit\" xmlns=\"xconf:video:duovideotoken\">"
		"</query>";
	char * mimeType="application/xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

pj_bool_t SIPCall::SIPSendWithdrawDualVideoToken()
{
	pj_bool_t status=PJ_FALSE;
	char * msg=
		"<query type=\"withdraw\" xmlns=\"xconf:video:duovideotoken\">"
		"</query>";
	char * mimeType="application/xml";
	status = SIPSendInfoRequest(msg,mimeType);

	return status;
}

void SIPCall::OnRecvSIPDualVideoTokenApply(int value)
{
	OnRecvGenericRequest(0,0,0);
}

void SIPCall::OnRecvSIPDualVideoTokenRelease(int value)
{
	OnRecvGenericCommand(0,0,0);
}

void SIPCall::OnRecvSIPDualVideoTokenPermit(int value)
{
	if(m_nRecvWithdraw <=0) OnRecvGenericResponse(0,BFCP_GRANTED);
	else
	{
		m_nRecvWithdraw=0;
		SIPSendReleaseDualVideoToken();
	}
}

void SIPCall::OnRecvSIPDualVideoTokenWithdraw(int value)
{
	if(m_nRecvWithdraw <=0) m_nRecvWithdraw=1;
	OnRecvGenericCommand(0,0,0);
	SIPSendReleaseDualVideoToken();
}

void SIPCall::SetCalledID(std::string strCalledID)
{
	m_strCalledID=strCalledID;
}



SIPCallCapability & SIPCall::GetMgrCapability(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return m_sdpCap;
#else
	return SIPCallMgr::Instance().m_sdpCap;
#endif

}


int SIPCall::GetMgrMainVideoMaxBitrate(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return m_nMainVideoBitrate;
#else
	return SIPCallMgr::Instance().GetMainVideoBitrate();
#endif

}

int SIPCall::GetMgrSecondVideoMaxBitrate(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return m_nSecondVideoMaxBitrate;
#else
	return SIPCallMgr::Instance().GetSecondVideoMaxBitrate();
#endif

}

void SIPCall::SetSupportH239(int nSupportH239)
{
	m_nSupportH239=nSupportH239;
}

int SIPCall::GetMgrSupportH239(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	return m_nSupportH239;
#else
	return SIPCallMgr::Instance().GetSupportH239();
#endif
}

VIDEO_FORMAT_TYPE SIPCall::GetMgrMainVideoFormatType(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	if(m_nMainVideoFTH264 > 0) m_nMainVideoFormatType=m_nMainVideoFTH264;
	else if(m_nMainVideoFTH264HP > 0) m_nMainVideoFormatType=m_nMainVideoFTH264HP;
	else if(m_nMainVideoFTH263 > 0) m_nMainVideoFormatType=m_nMainVideoFTH263;

	return m_nMainVideoFormatType;
#else
	return SIPCallMgr::Instance().GetMainVideoFormatType();
#endif

}

VIDEO_FORMAT_TYPE SIPCall::GetMgrSecondVideoFormatType(void)
{
	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	if(m_nSecondVideoFTH264 > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH264;
	else if(m_nSecondVideoFTH264HP > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH264HP;
	else if(m_nSecondVideoFTH263 > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH263;

	return m_nSecondVideoFormatType;
#else
	return SIPCallMgr::Instance().GetSecondVideoFormatType();
#endif

}

int SIPCall::SetLocalCapabilitis( SIPCallCapability & sdpCap,bool bAddG7221C,int nSupportDual)
{
	//移除所有 按所需添加
	sdpCap.RemoveAllCapabilitis();
	//CallMgr::Instance().SetAudioOnly(1);

	//mcu adapter add
#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	sdpCap.SetMainVideoMaxBitrate(m_nMainVideoBitrate);
	sdpCap.SetSecondVideoMaxBitrate(m_nSecondVideoMaxBitrate);
	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	DapCapCopyToOriginalCaps(GetMgrCapability(),dapCallCap);   //notice 
	return 0;

#else

	SetTerminalConfInfoCaps();
	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	DapCapCopyToOriginalCaps(GetMgrCapability(),dapCallCap);

	char msg[128]={0};
	sprintf(msg,"Set Local SDP First codecTye=%d,formatType=%d",SIPCallMgr::Instance().GetMainVideoCodecType(),SIPCallMgr::Instance().GetMainVideoFormatType());
	LogFile(NULL,NULL,"","",msg);

	return 0;

#endif

}

void SIPCall::AssignLocalMediaRTPPort(int nSupportDual)
{
	if(m_nLocalAudioPort <=0 )
	{
		m_nLocalAudioPort=SIPCallMgr::Instance().GetCallRTPPort();
	}
	if(m_nLocalVideoPort <=0 )
	{
		m_nLocalVideoPort=SIPCallMgr::Instance().GetCallRTPPort();
	}
	if(nSupportDual >0)
	{
		if(m_nLocalDualVideoPort <=0 )
		{
			m_nLocalDualVideoPort=SIPCallMgr::Instance().GetCallRTPPort();
		}

		if(m_nLocalBFCPTransPort <=0 )
		{
			m_nLocalBFCPTransPort=SIPCallMgr::Instance().GetCallRTPPort();
		}
	}
}

int SIPCall::GenerateLocal183SDP(pjmedia_sdp_session **p_sdp,int nSupportDual)
{
	pj_pool_t *pool=m_pPOOL;
	SIPCallCapability & offerSdpCap=GetMgrCapability();

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	offerSdpCap.AddG711u_64K();
	offerSdpCap.AddG711a_64K();
	offerSdpCap.AddH264CIF();
	offerSdpCap.SetH264FirstCIF();
	if(nSupportDual >0)
	{
		offerSdpCap.AddH239VideoH264CIFExtend();
		offerSdpCap.SetFirstH239VideoH264CIFExtend();
	}

#else
	nSupportDual=GetMgrSupportH239();
	SetLocalCapabilitis(offerSdpCap);

#endif

	AssignLocalMediaRTPPort(nSupportDual);
	SIPCallMgr::Instance().SetCapabilityParam(this,offerSdpCap,nSupportDual,m_strLocalIP,m_nLocalAudioPort,m_nLocalVideoPort,m_nLocalDualVideoPort,m_nLocalBFCPTransPort,m_nCallDir);

	//sdp add
	if(offerSdpCap.SDP_Capabilitis_Add(pool,p_sdp)<0)
	{
		LogFile(NULL,NULL,"","","GenerateLocal183SDP SDP_Capabilitis_Add Error.");
		return -1;
	}

	return 0;
}

int SIPCall::SetLocalMainVideoH264Level(int nLevel)
{
	if(nLevel > 0) m_nLocalMainVideoH264Level=nLevel;
	return 0;
}

int SIPCall::SetLocalSecondVideoH264Level(int nLevel)
{
	if(nLevel > 0) m_nLocalSecondVideoH264Level=nLevel;
	return 0;
}

int SIPCall::SetLocalMainVideoH264HPLevel(int nLevel)
{
	if(nLevel > 0) m_nLocalMainVideoH264HPLevel=nLevel;
	return 0;
}

int SIPCall::SetLocalSecondVideoH264HPLevel(int nLevel)
{
	if(nLevel > 0) m_nLocalSecondVideoH264HPLevel=nLevel;
	return 0;
}

int SIPCall::SetRTPMediaMulticast(const char*cszMulticastIP,int ttl)
{
	if(cszMulticastIP && ( strlen(cszMulticastIP) > 0 ) )
	{
		m_strMediaMulticastIP =cszMulticastIP;
		m_bMediaMulticastEnable=true;
		m_nMediaMulticastTTL=ttl;
	}
	return 0;
}

int SIPCall::SendDualVideoControlMsg(int nType,int transactionId,int userId,int floorId,int status)
{
	//1 请求 2 响应 3 指示 4 取消
	switch(nType)
	{
	case 1:
		SendGenericRequest();
		break;
	case 2:
		if(status==1)
		{
			SendGenericResponse(transactionId,userId,floorId,BFCP_PENDING);
			SendGenericResponse(0,userId,floorId,BFCP_ACCEPTED);
			SendGenericResponse(0,userId,floorId,BFCP_GRANTED);
		}
		if(status==0)
		{
			SendGenericResponse(transactionId,userId,floorId,BFCP_DENIED);
		}
		break;
	case 3:
		//SendGenericIndication();
		break;
	case 4:
		SendGenericCommand();
		break;
	default:
		break;
	}
	return 0;
}


#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

//audio cap
int SIPCall::AddCapAudioAll(void)
{
	return 0;
}

int SIPCall::RemoveCapAudioAll(void)
{
	SIPCallCapability & m_CallCapability=m_sdpCap;

	m_CallCapability.RemoveAudioAllCapabilitis();
	m_bAudioCapItemSetFirst=false;
	return 0;
}

int SIPCall::AddCapAudioItem(X_AUDIO_CODEC_ID	codecID)
{
	return 0;
}

int SIPCall::AddCapAudioG711A_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G711A);
}

int SIPCall::AddCapAudioG711U_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G711U);
}

int SIPCall::AddCapAudioG722_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G722_64);
}

int SIPCall::AddCapAudioG7221_24K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_7_24);
}

int SIPCall::AddCapAudioG7221_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_7_32);
}

int SIPCall::AddCapAudioG7221C_24K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_24);
}

int SIPCall::AddCapAudioG7221C_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_32);
}

int SIPCall::AddCapAudioG7221C_48K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_48);
}

int SIPCall::AddCapAudioG729A(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G729);
}

int SIPCall::AddCapAudioG719_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_32);
}

int SIPCall::AddCapAudioG719_48K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_48);
}

int SIPCall::AddCapAudioG719_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_64);
}

int SIPCall::AddCapVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel)
{
	return 0;
}

int SIPCall::AddCapDualVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel)
{
	return 0;
}

int SIPCall::AddCapVideoAll(void)
{
	return 0;
}

int SIPCall::RemoveCapVideoAll(void)
{
	SIPCallCapability & m_CallCapability=m_sdpCap;

	m_CallCapability.RemoveVideoAllCapabilitis();
	m_bVideoCapItemSetFirst=false;
	return 0;
}

int SIPCall::AddCapVideoH263(int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	return 0;
}

int SIPCall::AddCapVideoH263Plus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	return 0;
}


int SIPCall::AddCapVideoH263PlusPlus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	return 0;
}

int SIPCall::AddCapVideoH264QCIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_QCIF,nLevel);
}

int SIPCall::AddCapVideoH264CIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_CIF,nLevel);
}

int SIPCall::AddCapVideoH2644CIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_4CIF,nLevel);
}

int SIPCall::AddCapVideoH264720P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_720P,nLevel);
}

int SIPCall::AddCapVideoH2641080P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_1080P,nLevel);
}

int SIPCall::AddCapVideoH264HP720P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_720P,nLevel);
}

int SIPCall::AddCapVideoH264HP1080P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_1080P,nLevel);
}

int SIPCall::AddCapDualVideoAll()
{
	return 0;
}

int SIPCall::RemoveCapDualVideoAll()
{
	return 0;
}

int SIPCall::AddCapDualVideoH263(int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	return 0;
}

int SIPCall::AddCapDualVideoH263Plus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	return 0;
}

int SIPCall::AddCapDualVideoH263PlusPlus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	return 0;
}

int SIPCall::AddCapDualVideoH264QCIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_QCIF,nLevel);
}

int SIPCall::AddCapDualVideoH264CIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_CIF,nLevel);
}

int SIPCall::AddCapDualVideoH2644CIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_4CIF,nLevel);
}

int SIPCall::AddCapDualVideoH264720P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_720P,nLevel);
}

int SIPCall::AddCapDualVideoH2641080P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_1080P,nLevel);
}

int SIPCall::AddCapDualVideoH264HP720P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_720P,nLevel);
}

int SIPCall::AddCapDualVideoH264HP1080P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_1080P,nLevel);
}

int SIPCall::AddCapVideoH264VGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_VGA,nLevel);
}

int SIPCall::AddCapVideoH264SVGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SVGA,nLevel);
}

int SIPCall::AddCapVideoH264XGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_XGA,nLevel);
}

int SIPCall::AddCapVideoH264SXGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SXGA,nLevel);
}

int SIPCall::AddCapDualVideoH264VGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_VGA,nLevel);
}

int SIPCall::AddCapDualVideoH264SVGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SVGA,nLevel);
}

int SIPCall::AddCapDualVideoH264XGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_XGA,nLevel);
}

int SIPCall::AddCapDualVideoH264SXGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SXGA,nLevel);
}


int SIPCall::AddCapFECCData()
{
	SIPCallCapability & m_CallCapability=m_sdpCap;

	m_CallCapability.AddFECCData();
	return 0;
}

int SIPCall::RemoveCapAll(void)
{

	return 0;
}

int SIPCall::RemoveCapFECCDataAll(void)
{
	return 0;
}

int SIPCall::DAPSetLocalCapabilitis(void)
{
	return 0;
}

#endif


void SIPCall::CopyVideoCapOtherParam0(ConfInfo *pConfInfo,int setBitrate,int setFormat)
{
}

void SIPCall::AddCodecCapFromConfig(SIPCallCapability & sdpCap)
{

}

void SIPCall::AllowToSendSecondVideo(bool bAllow)
{
	int nUseType=SIPCallMgr::Instance().m_nUseDualVideoTokenType;
	if (m_bH239ReceiverOpen==false)
	{
		m_bH239ReceiverOpen=true;
		m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
	}
	if (bAllow)
	{
		//SendGenericResponse(true);//允许
		if(nUseType==0)
		{
			SendGenericResponse(m_nTransactionId,m_nUserId,m_nFloorId,BFCP_PENDING);
			SendGenericResponse(0,m_nUserId,m_nFloorId,BFCP_ACCEPTED);
			SendGenericResponse(0,m_nUserId,m_nFloorId,BFCP_GRANTED);
		}
		if(nUseType==1)
		{
			SIPSendPermitDualVideoToken();
		}
	}
	else
	{
		//SendGenericResponse(false);//不允许
		SendGenericResponse(m_nTransactionId,m_nUserId,m_nFloorId,BFCP_DENIED);
		m_bH239ReceiverOpen=false;
		m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	}

	//调整视频双流比例
	//SetVideoBitRate();
}

//Send ACK request message. 1 request 2 response
pj_bool_t SIPCall::SIPSendACKRequest(pjsip_rx_data * rdata,int type,int st_code)
{
	//pjsip_inv_create_ack(inv, rdata->msg_info.cseq->cseq,&tdata);
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;

	pjsip_dlg_inc_lock(inv->dlg);
	//Create request
	status = pjsip_dlg_create_request(inv->dlg, pjsip_get_ack_method(),rdata->msg_info.cseq->cseq, &tdata);
	pjsip_dlg_dec_lock(inv->dlg);
	if (status != PJ_SUCCESS) 
	{
		return PJ_FALSE;
	}

	//reference pjsip_endpt_create_ack
	pjsip_to_hdr *to;
    to = (pjsip_to_hdr*) pjsip_msg_find_hdr(tdata->msg, PJSIP_H_TO, NULL);
    pj_strdup(tdata->pool, &to->tag, &rdata->msg_info.to->tag);
    pjsip_hdr *via;
	const pjsip_hdr *hdr;
    while ((via=(pjsip_hdr*)pjsip_msg_find_hdr(tdata->msg, PJSIP_H_VIA, NULL)) != NULL)
	pj_list_erase(via);
    //Must contain single Via, just as the original INVITE
    hdr = (pjsip_hdr*) pjsip_msg_find_hdr( rdata->msg_info.msg, PJSIP_H_VIA, NULL);
    pjsip_msg_insert_first_hdr( tdata->msg,(pjsip_hdr*) pjsip_hdr_clone(tdata->pool,hdr) );

	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;

	if(type==2 && ( (st_code >402 && st_code <407) || st_code == 408 || st_code/48==10 || st_code/100==6 || st_code/100==5) )
	{
		Sleep(10);
		pjsip_inv_terminate(inv,st_code,PJ_TRUE);
	}

	return PJ_TRUE;

}

bool & SIPCall::CallDestroyStatus()
{
	return m_bCallDestroy;
}

void SIPCall::SaveCallMediaUpdatePacket(std::string & strMediaText)
{
	m_strMediaUpdate = strMediaText;
}

std::string & SIPCall::GetCallMediaUpdatePacket()
{
	return m_strMediaUpdate;
}

void SIPCall::OnChannelRequestKeyFrame(int type)  //1 MainVideo , 2 DualVideo
{

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

	if (type == 1)
	{
		SIPCallMgr::Instance().OnDAP_RecvMainVideoRequestKeyFrame(m_strCallID.c_str());
	}
	else if (type == 2)
	{
		SIPCallMgr::Instance().OnDAP_RecvSecondVideoRequestKeyFrame(m_strCallID.c_str());
	}

#else

	if (type == 1)
	{
		OnRecvdMainVideoRequestKeyFrame();
	}
	else if (type == 2)
	{
		OnRecvdSecondVideoRequestKeyFrame();
	}
#endif //#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

}

void SIPCall::OnCallRingIn(void)
{
	m_nCallDir=IMXEC_CALL_DIR_IN;
}

void SIPCall::OnCallRingOut(void)
{
	m_nCallDir=IMXEC_CALL_DIR_OUT;
}


VIDEO_FORMAT_TYPE SIPCall::GetVideoFormatTypeFromMaxBitrate(int nVideoBitrate,VIDEC_CODEC_TYPE videoCodecType)
{
	VIDEO_FORMAT_TYPE VideoFormatType =VIDEO_FORMAT_QCIF;
	int div=1;
	if(videoCodecType == VIDEC_CODEC_H264_SVC) div=2;

	struct tagH264Option
	{
		VIDEO_FORMAT_TYPE FormatType;
		int nMaxBitrate;
	};
	tagH264Option H264OptionArray[] =
	{
		{VIDEO_FORMAT_CIF,384},{VIDEO_FORMAT_4CIF,768},
		{VIDEO_FORMAT_720P,1024},{VIDEO_FORMAT_720P,1536},{VIDEO_FORMAT_720P,1920},
		{VIDEO_FORMAT_1080P,2048},{VIDEO_FORMAT_1080P,4096},{VIDEO_FORMAT_1080P,8192},{VIDEO_FORMAT_1080P,20480}
	};
	int nCount=sizeof(H264OptionArray) / sizeof(tagH264Option);
	if(nVideoBitrate >0)
	{
		for(int i=0;i<nCount;i++)
		{
			if(nVideoBitrate <= H264OptionArray[i].nMaxBitrate /div )
			{
				VideoFormatType = H264OptionArray[i].FormatType;
				break;
			}
		}
	}
	return VideoFormatType;
}

void SIPCall::SetVideoFormatTypeFromMaxFSBitrate(int nVideoBitrate,VIDEC_CODEC_TYPE videoCodecType,int nH264Level,VIDEO_FORMAT_TYPE & videoFmt,int nType) //0 terminal use 1 mcu mp use
{
}

VIDEO_FORMAT_TYPE SIPCall::GetCompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2,int nType)
{
	int nValue[2]={0};
	VIDEO_FORMAT_TYPE formatType=nType1;
	for(int i=0;i<2;i++)
	{
		if(i==1) formatType=nType2;
		switch(formatType)
		{
		case VIDEO_FORMAT_QCIF:
			nValue[i]= 0;
			break;
		case VIDEO_FORMAT_CIF:
			nValue[i]= 1;
			break;
		case VIDEO_FORMAT_4CIF:
			nValue[i]= 3;
			break;
		case VIDEO_FORMAT_16CIF:
			nValue[i]= 10;
			break;
		case VIDEO_FORMAT_720I:
			nValue[i]= 4;
			break;
		case VIDEO_FORMAT_720P:
			nValue[i]= 7;
			break;
		case VIDEO_FORMAT_1080I:
			nValue[i]= 8;
			break;
		case VIDEO_FORMAT_1080P:
			nValue[i]= 11;
			break;
		case VIDEO_FORMAT_QVGA:
			nValue[i]= 0;
			break;
		case VIDEO_FORMAT_VGA:
			nValue[i]= 2;
			break;
		case VIDEO_FORMAT_SVGA:
			nValue[i]= 5;
			break;
		case VIDEO_FORMAT_XGA:
			nValue[i]= 6;
			break;
		case VIDEO_FORMAT_SXGA:
			nValue[i]= 9;
			break;
		default:
			nValue[i]= 0 ;
			break;
		}
	}

	if(nType ==0) formatType = (nValue[0] >= nValue[1] ) ? nType1 : nType2 ;
	else if(nType ==1)
	{
		formatType = (nValue[0] <= nValue[1] ) ? nType1 : nType2 ;
		if(formatType == 0) formatType = (nValue[0] >= nValue[1] ) ? nType1 : nType2 ;
	}
	return formatType;
}

void SIPCall::SetCallNum(int num)
{
	m_nCallNum =num;
}

int SIPCall::GetCallNum( )
{
	return m_nCallNum ;
}

pj_bool_t SIPCall::SIPSend200Status(pjsip_rx_data *rdata ,int code) //Send 200 OK Status
{
	return Send200SDPResponse(rdata,0,NULL);
}

int SIPCall::GetLocalMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	if(nWidth >0 && nHeight >0)
	{
		if(m_nMainChannelState !=1) { nWidth=0; nHeight=0; chInfo.m_nFrame=0;}
		else
		{
			nWidth =m_nMainVideoWidth;
			nHeight=m_nMainVideoHeight;
			chInfo.m_nFrame =m_nRemoteMainVideoFrame;		//对方最大帧速率
			//GetVideoFormatTypeWH(SIPCallMgr::Instance().m_nMainVideoFormatType,nWidth,nHeight);
		}
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_SIP;
	chInfo.m_bChannelOpen =m_bNegCapVideo;
	return nRet;
}

int SIPCall::GetSecondVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	//notice wait ?
	if(nWidth >0 && nHeight >0 && m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	{
		if(m_nDualChannelState !=1) { nWidth=0; nHeight=0; chInfo.m_nFrame=0;}
		else
		{
			nWidth =m_nDualVideoWidth;
			nHeight=m_nDualVideoHeight;
			chInfo.m_nFrame = m_nRemoteSecondVideoFrame;		//对方最大帧速率
			//GetVideoFormatTypeWH(SIPCallMgr::Instance().m_nSecondVideoFormatType,nWidth,nHeight);
		}
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_SIP;
	chInfo.m_bChannelOpen =m_bNegCapDualVideo;
	return nRet;
}

int SIPCall::GetRemoteMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	if(nWidth >0 && nHeight >0 )
	{
		if(m_nMainChannelState !=1)  
		{ nWidth=0; nHeight=0; chInfo.m_nFrame=0;}
		else chInfo.m_nFrame =m_nRemoteMainVideoFrame;		//对方最大帧速率
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_SIP;
	chInfo.m_bChannelOpen =m_bNegCapVideo;
	return nRet;
}

//Send update request message.
pj_bool_t SIPCall::SIPSendInviteRequest(pjmedia_sdp_session *sdp)
{
	pj_status_t status;
	pjsip_tx_data *tdata;
	pjsip_inv_session  *inv=NULL;
	if(m_CallDirection == SIPCall::CALL_DIR_OUT) inv=m_pInvSessionUAC;
	if(m_CallDirection == SIPCall::CALL_DIR_IN) inv=m_pInvSessionUAS;
	if(inv==NULL) return PJ_FALSE;
	 const pjsip_method pjsip_update_method ={ PJSIP_INVITE_METHOD, { "INVITE",6}};

	pjsip_dlg_inc_lock(inv->dlg);
	//Create request
	status = pjsip_dlg_create_request(inv->dlg, &pjsip_update_method,-1, &tdata);
	if (status != PJ_SUCCESS) 
	{
		pjsip_dlg_dec_lock(inv->dlg);
		return PJ_FALSE;
	}

	//Attach SDP body
	if(sdp)
	{
		pjmedia_sdp_session *sdp_copy = pjmedia_sdp_session_clone(tdata->pool, sdp);
		pjsip_create_sdp_body(tdata->pool, sdp_copy, &tdata->msg->body);
	}

	pjsip_dlg_dec_lock(inv->dlg);

	//status = pjsip_timer_update_req(inv, tdata);
	//status = pjsip_inv_update(pInvSessionUas, NULL, sdp, &tdata);

	if (status != PJ_SUCCESS) return PJ_FALSE;
	m_rSIPUser.SIPMessageAddUserAgentCapability(tdata);
	//m_rSIPUser.SIPMessageAddDateCapability(tdata);
	status = pjsip_inv_send_msg(inv, tdata);
	if(status != PJ_SUCCESS)  return PJ_FALSE;
	m_bSendUpdateSDP=true;
	return PJ_TRUE;
}


int SIPCall::GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType)
{
	return GetMgrCapability().GetVideoFormatValue(formatType);
}

float SIPCall::GetFrameRateFromH264Param(int nMaxFS,int nMBPS)
{
	float fFrameRate= VideoFrameBase;
	if(nMBPS >0 && nMaxFS >0) fFrameRate = nMBPS /nMaxFS;
	return fFrameRate;
}

VIDEO_FORMAT_TYPE SIPCall::GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate)
{
	if(nProfile == 100) nProfile =8; //H264 high profile  64--64 100--8  ?? more add
	return GetMgrCapability().GetH264FormatTypeFromBitrate(nFrame,nProfile,nBitrate);
}

VIDEO_FORMAT_TYPE SIPCall::GetCallFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType) //nType 0 get min ,1 get max
{
	float fFrame=GetFrameRateFromH264Param(nMaxFS,nMBPS);
	if(nProfile == 100) nProfile =8; //H264 high profile  64--64 100--8  ?? more add
	int nH323Level = GetMgrCapability().GetH264H323Level(nLevel);
	nLevel = nH323Level;
	return GetMgrCapability().GetSIPFormatTypeFromH264Param(nProfile,nLevel,nMaxFS,nBitrate,fFrame,nType);
}

void SIPCall::AddCodecDapCapFromConfig()
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	m_nSendDTMFRTP = 0;
	char szPath[512] ={0};
	int nValue=0;
	int i=0;

	GetModuleFileName(0,szPath,512);
	for(i=strlen(szPath);i>0;i--)
	{
		if(szPath[i]=='\\')
		{
			szPath[i+1]=0;
			break;
		}
	}
	strcat(szPath,"CapMedia.ini");

	int nForceVideoWid=0;
	int nForceVideoHei=0;
	nForceVideoWid=GetPrivateProfileInt("CapMedia","ForceVideoWid",0,szPath);
	nForceVideoHei=GetPrivateProfileInt("CapMedia","ForceVideoHgh",0,szPath);
	if(nForceVideoWid >0 && nForceVideoHei >0)
	{
		//m_nForceVideoWid=nForceVideoWid;
		//m_nForceVideoHei=nForceVideoHei;
	}

	nForceVideoWid=0;
	nForceVideoHei=0;
	nForceVideoWid=GetPrivateProfileInt("CapMedia","ForceDualVideoWid",0,szPath);
	nForceVideoHei=GetPrivateProfileInt("CapMedia","ForceDualVideoHgh",0,szPath);
	if(nForceVideoWid >0 && nForceVideoHei >0)
	{
		//m_nForceDualVideoWid=nForceVideoWid;
		//m_nForceDualVideoHei=nForceVideoHei;
	}

	int nFirstType=0;
	nValue=GetPrivateProfileInt("CapMedia","UseIniAdd",0,szPath);
	if(nValue>0)
	{
		GetMgrCapability().RemoveAllCapabilitis();
	}
	else return;

	int nSendDtmf=GetPrivateProfileInt("CapMedia","DTMFSend",0,szPath); //0 audec send , 1 rtp send
	m_nSendDTMFRTP = nSendDtmf;



	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	dapCallCap.m_nAudioCapCount =0;
	dapCallCap.m_nMainVideoCapCount =0;
	dapCallCap.m_nSecondVideoCapCount =0;
	dapCallCap.m_nDataCapCount =0;

	//audio
	nValue=GetPrivateProfileInt("MediaAudio","G711Ulaw",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G711U;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G711Alaw",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G711A;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G72264K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G722_64;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G729A",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G729;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 8;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G722124K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_7_24;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 24;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G722132K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_7_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G7221C24K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_24;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 24;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G7221C32K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G7221C48K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_48;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 48;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G71932K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G71948K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_48;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 48;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","G71964K",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_64;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	nValue=GetPrivateProfileInt("MediaAudio","GSM",0,szPath);
	//if(nValue>0)
	//{
	//	dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_GSM;
	//	dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = m_nCapDirectionDefault;
	//	dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 8;
	//	dapCallCap.m_nAudioCapCount ++;
	//}


	char mediaName[32]={0};
	for(int i=0;i<2;i++)
	{
		int nVideoRate=768;
		int nDual=0;
		int nVideoFrameRate = 30;
		if(i==0)
		{
			nDual=0;
			strcpy(mediaName,"MediaVideo");
			nVideoRate=768;
			nVideoFrameRate = SIPCallMgr::Instance().GetMainVideoFrameRate();
			nValue=GetPrivateProfileInt(mediaName,"MaxBitrate",0,szPath);
			if(nValue>0)
			{
				nVideoRate =nValue;
				GetMgrCapability().SetMainVideoMaxBitrate(nValue);
			}
		}
		else if(i==1)
		{
			nDual=1;
			strcpy(mediaName,"MediaDualVideo");
			nVideoRate=768;
			nVideoFrameRate = SIPCallMgr::Instance().GetSecondVideoDevFrameRate();
			nValue=GetPrivateProfileInt(mediaName,"MaxBitrate",0,szPath);
			if(nValue>0)
			{
				nVideoRate =nValue;
				GetMgrCapability().SetSecondVideoMaxBitrate(nValue);
			}
		}

		nValue=GetPrivateProfileInt(mediaName,"H264QCIF",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_QCIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H264CIF",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H2644CIF",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_4CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H264720P",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_720P,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H2641080P",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_1080P,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}

		nValue=GetPrivateProfileInt(mediaName,"H264SVGA",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_SVGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}

		nValue=GetPrivateProfileInt(mediaName,"H264XGA",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_XGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H264SXGA",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_SXGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H26416CIF",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264,VIDEO_FORMAT_16CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}


		nValue=GetPrivateProfileInt(mediaName,"H264HP720P",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_720P,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"H264HP1080P",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_1080P,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}


		nValue=GetPrivateProfileInt(mediaName,"CIF4H263Version2",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263P,VIDEO_FORMAT_4CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"VGAH263Version2",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263P,VIDEO_FORMAT_VGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"XGAH263Version2",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263P,VIDEO_FORMAT_XGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"SXGAH263Version2",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263P,VIDEO_FORMAT_SXGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}


		nValue=GetPrivateProfileInt(mediaName,"CIF4H263Version3",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263PP,VIDEO_FORMAT_4CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}

		nValue=GetPrivateProfileInt(mediaName,"VGAH263Version3",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263PP,VIDEO_FORMAT_VGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"XGAH263Version3",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263PP,VIDEO_FORMAT_XGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"SXGAH263Version3",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263PP,VIDEO_FORMAT_SXGA,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}


		nValue=GetPrivateProfileInt(mediaName,"CIF4H263",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263,VIDEO_FORMAT_4CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"CIFH263",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263,VIDEO_FORMAT_CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}
		nValue=GetPrivateProfileInt(mediaName,"QCIFH263",0,szPath);
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_H263,VIDEO_FORMAT_QCIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}

		nValue=GetPrivateProfileInt(mediaName,"MPEG4",0,szPath); //use to call kedacom
		if(nValue>0)
		{
			AddVideoConfInfoCaps(VIDEC_CODEC_MPEG4,VIDEO_FORMAT_4CIF,nVideoFrameRate,m_nCapDirectionDefault,nVideoRate,nDual);
		}

	}

	//FECC
	nValue=GetPrivateProfileInt("MediaFECC","H224",0,szPath);
	if(nValue>0)
	{
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_DataCodecID = DATA_TYPE_H224;;
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDataBitrate = 48;
		dapCallCap.m_nDataCapCount ++;
		//GetMgrCapability().AddFECCData();
	}



	//X_AUDIO_CODEC_ID nAudioID=SIPCallMgr::Instance().GetAudioCodecID();
	//nFirstType=GetPrivateProfileInt("CapMedia","AudioFirst",-1,szPath);
	//if(nFirstType >= 0) nAudioID=(X_AUDIO_CODEC_ID) nFirstType;


	//video

	//VIDEC_CODEC_TYPE nVideoID=SIPCallMgr::Instance().GetMainVideoCodecType();
	//nFirstType=GetPrivateProfileInt("CapMedia","VideoFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoID=(VIDEC_CODEC_TYPE) nFirstType;

	//VIDEO_FORMAT_TYPE nVideoFormat= GetMgrMainVideoFormatType();
	//nFirstType=GetPrivateProfileInt("CapMedia","VideoFormatFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoFormat=(VIDEO_FORMAT_TYPE) nFirstType;

	//nVideoID=SIPCallMgr::Instance().GetSecondVideoCodecType();
	//nFirstType=GetPrivateProfileInt("CapMedia","DualVideoFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoID=(VIDEC_CODEC_TYPE) nFirstType;

	//nVideoFormat= GetMgrSecondVideoFormatType();
	//nFirstType=GetPrivateProfileInt("CapMedia","DualVideoFormatFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoFormat=(VIDEO_FORMAT_TYPE) nFirstType;


#endif

}

void SIPCall::AddVideoConfInfoCaps(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nFrame,int nDirection,int nBitrate,int nDual)
{
	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	if(nDual ==0)
	{
		for(int i=0;i<dapCallCap.m_nMainVideoCapCount;i++)
		{
			//the different to H323
			if(dapCallCap.m_aMainVideoCap[0].m_VideoCodecType == nCodec /* && dapCallCap.m_aMainVideoCap[0].m_VideoFormatType == nFormat*/ ) return;
		}

		if(dapCallCap.m_nMainVideoCapCount <MAX_VIDEO_CAP)
		{
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_VideoCodecType =nCodec;
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_nDirection = nDirection;
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_nVideoBitrate = nBitrate;
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_nVideoFrameRate = nFrame;
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_VideoFormatType = nFormat;
			dapCallCap.m_aMainVideoCap[dapCallCap.m_nMainVideoCapCount].m_nFormatValue = GetVideoFormatValue(nFormat);
			dapCallCap.m_nMainVideoCapCount ++;
		}
	}
	if(nDual ==1)
	{
		for(int i=0;i<dapCallCap.m_nSecondVideoCapCount;i++)
		{
			//the different to H323
			if(dapCallCap.m_aSecondVideoCap[0].m_VideoCodecType == nCodec /*&& dapCallCap.m_aMainVideoCap[0].m_VideoFormatType == nFormat*/ ) return;
		}

		if(dapCallCap.m_nSecondVideoCapCount <MAX_VIDEO_CAP)
		{
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_VideoCodecType =nCodec;
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_nDirection = nDirection;
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_nVideoBitrate = nBitrate;
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_nVideoFrameRate = nFrame;
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_VideoFormatType = nFormat;
			dapCallCap.m_aSecondVideoCap[dapCallCap.m_nSecondVideoCapCount].m_nFormatValue = GetVideoFormatValue(nFormat);
			dapCallCap.m_nSecondVideoCapCount ++;
		}
	}

}

void SIPCall::AddVideoConfInfoBaseCaps2(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nVideoFrameRate,int nDirection,int nVideoBitrate,int nDual)
{
	struct tagResStandardItem
	{
		ResStandard nStandard;
		VIDEO_FORMAT_TYPE nFormat;
	};

	int &nResStand =CallMgr::Instance().m_nResStand;

	//添加首选能力并检查
	bool bSet1=true;
	bool bSet2=false;
	bool bSet3=false;
	struct tagResStandardItem resStands[]={ {res4CIF,VIDEO_FORMAT_QCIF}, 
	{res4CIF,VIDEO_FORMAT_CIF}, {res4CIF,VIDEO_FORMAT_4CIF}, {res4CIF,VIDEO_FORMAT_VGA},{res4CIF,VIDEO_FORMAT_SVGA},
	{res720P,VIDEO_FORMAT_720P}, {res720P,VIDEO_FORMAT_720I}, {res720P,VIDEO_FORMAT_XGA},{res720P,VIDEO_FORMAT_SXGA},
	{res1080P,VIDEO_FORMAT_1080P}, {res1080P,VIDEO_FORMAT_1080I}, {res1080P,VIDEO_FORMAT_16CIF} };
	int nCount=sizeof(resStands)/sizeof(tagResStandardItem);

	//this different to H323
	bSet2=true;


	//产品高标清控制
	for(int i=0;i<nCount;i++)
	{
		if(resStands[i].nFormat == nFormat && resStands[i].nStandard <=nResStand)
		{
			bSet3=true;
			break;
		}
	}
	if(bSet1 && bSet2 && bSet3)
	{
		AddVideoConfInfoCaps(nCodec,nFormat,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
	}

	VIDEC_CODEC_TYPE nCodecType;
	VIDEO_FORMAT_TYPE nFT;
	//add H264 必带能力
	//this different to H323
	{
		if(nResStand >=res1080P)
		{
			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_1080P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else if(nResStand >=res720P)
		{
			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_720P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_SXGA;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else
		{
			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_4CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
	}

	//add h264 highProfile 优先则带此能力
	if(nCodecType == VIDEC_CODEC_H264_SVC)
	{
		if(nResStand >=res1080P)
		{
			nCodecType=VIDEC_CODEC_H264_SVC;
			nFT=VIDEO_FORMAT_1080P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else if(nResStand >=res720P)
		{
			nCodecType=VIDEC_CODEC_H264_SVC;
			nFT=VIDEO_FORMAT_720P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else
		{
			nCodecType=VIDEC_CODEC_H264_SVC;
			nFT=VIDEO_FORMAT_4CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
	}

	//Add H263v2 优先则带此能力
	if(nCodecType == VIDEC_CODEC_H263P)
	{
		if(nResStand >=res1080P)
		{
			nCodecType=VIDEC_CODEC_H263P;
			nFT=VIDEO_FORMAT_1080P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else if(nResStand >=res720P)
		{
			nCodecType=VIDEC_CODEC_H263P;
			nFT=VIDEO_FORMAT_720P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else
		{
			nCodecType=VIDEC_CODEC_H263P;
			nFT=VIDEO_FORMAT_4CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
	}

	//Add H263 必带能力
	nCodecType=VIDEC_CODEC_H263;
	nFT=VIDEO_FORMAT_4CIF;
	AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

	//Add H263v3 选择了则会添加 优先则带此能力
	if(nCodecType == VIDEC_CODEC_H263PP)
	{
		if(nResStand >=res1080P)
		{
			nCodecType=VIDEC_CODEC_H263PP;
			nFT=VIDEO_FORMAT_1080P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else if(nResStand >=res720P)
		{
			nCodecType=VIDEC_CODEC_H263PP;
			nFT=VIDEO_FORMAT_720P;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
		else
		{
			nCodecType=VIDEC_CODEC_H263PP;
			nFT=VIDEO_FORMAT_4CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}
	}

	//this different to H323

}

void SIPCall::ConfCapabilitisDefault(void)
{
	for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount;ii++)
	{
		m_ConfInfo.m_DAPCallCapability.m_aAudioCap[ii].m_nDirection =m_nCapDirectionDefault;
	}

	for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;ii++)
	{
		m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii].m_nDirection =m_nCapDirectionDefault;
		m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii].m_nFormatValue =GetVideoFormatValue(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii].m_VideoFormatType);
	}

	for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
	{
		m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii].m_nDirection =m_nCapDirectionDefault;
		m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii].m_nFormatValue =GetVideoFormatValue(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii].m_VideoFormatType);
	}

	for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nDataCapCount;ii++)
	{
		m_ConfInfo.m_DAPCallCapability.m_DataCap[ii].m_nDirection =m_nCapDirectionDefault;
	}
}



void SIPCall::SetTerminalConfInfoCaps(void)
{
	int nFound=-1;
	SIPCallMgr & rCallMgr = SIPCallMgr::Instance();

	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	int nDirection =m_nCapDirectionDefault;

	memset(&dapCallCap,0,sizeof(DapCallCapability));
	//dapCallCap.m_nAudioCapCount =0;
	//dapCallCap.m_nMainVideoCapCount =0;
	//dapCallCap.m_nSecondVideoCapCount =0;
	//dapCallCap.m_nDataCapCount =0;

	//add audio cap
	bool bG722124=true;
	bool bG722132=true;
	bool bG7221c24=true;
	bool bG7221c32=true;
	bool bG7221c48=true;
	bool bG71932=true;
	bool bG71948=true;
	bool bG71964=true;
	bool bG711a=true;
	bool bG711u=true;
	bool bG72264=true;
	bool bG729=true;

	//control add audio cap
	bG7221c24=false;
	bG7221c32=false;
	bG7221c48=false;
	bG71932=false;
	bG71948=false;
	bG71964=false;
	bG729=true; //chang cheng zuantan use


	if(bG711u)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G711U;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG711a)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G711A;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG722124)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_7_24;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 24;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG722132)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_7_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}

	if(bG7221c24)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_24;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 24;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG7221c32)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG7221c48)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G7221_14_48;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 48;
		dapCallCap.m_nAudioCapCount ++;
	}

	if(bG71932)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_32;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 32;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG71948)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_48;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 48;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG71964)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G719_64;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}


	if(bG72264)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G722_64;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 64;
		dapCallCap.m_nAudioCapCount ++;
	}
	if(bG729)
	{
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_AudioCodecType =X_AUDIO_CODEC_G729;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nDirection = nDirection;
		dapCallCap.m_aAudioCap[dapCallCap.m_nAudioCapCount].m_nAudioBitrate = 8;
		dapCallCap.m_nAudioCapCount ++;
	}

	//set first 
	X_AUDIO_CODEC_ID AudioCodecPri=rCallMgr.GetAudioCodecID();
	AudioCapability tempAudioCap;
	for(int i=0;i <dapCallCap.m_nAudioCapCount;i++)
	{
		if( dapCallCap.m_aAudioCap[i].m_AudioCodecType ==AudioCodecPri)
		{
			if(i >0)
			{
				memcpy(&tempAudioCap,&(dapCallCap.m_aAudioCap[i]),sizeof(tempAudioCap) );
				memcpy(&(dapCallCap.m_aAudioCap[i]),&(dapCallCap.m_aAudioCap[0]),sizeof(tempAudioCap) );
				memcpy(&(dapCallCap.m_aAudioCap[0]),&tempAudioCap,sizeof(tempAudioCap) );
			}
			break;
		}
	}

	VIDEC_CODEC_TYPE videoCodecPri;
	VIDEO_FORMAT_TYPE videoFormatPri;
	int nVideoFrameRate = VideoFrameBase;
	int nVideoBitrate ;
	int nDual=0;

	//add main video caps
	if (CallMgr::Instance().GetAudioOnly()==0)
	{
		videoCodecPri =rCallMgr.GetMainVideoCodecType();
		videoFormatPri =rCallMgr.GetMainVideoFormatType();
		nVideoBitrate =rCallMgr.GetMainVideoBitrate();
		nVideoFrameRate = rCallMgr.GetMainVideoFrameRate();
		nDual=0;
		AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

		//add dual video caps
		if(rCallMgr.GetSupportH239() )
		{
			videoCodecPri =rCallMgr.GetSecondVideoCodecType();
			videoFormatPri =rCallMgr.GetSecondVideoFormatType();
			nVideoBitrate =rCallMgr.GetSecondVideoMaxBitrate();
			nVideoFrameRate = rCallMgr.GetSecondVideoDevFrameRate();
			nDual=1;
			AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
		}

		//H224
		bool bSupportFECC=true;
		if(bSupportFECC)
		{
			dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_DataCodecID = DATA_TYPE_H224;;
			dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDirection = m_nCapDirectionDefault;
			dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDataBitrate = 48;
			dapCallCap.m_nDataCapCount ++;
		}
	}

	//notice test , if config,only use the config cap add
	AddCodecDapCapFromConfig();

}

void SIPCall::DapCapCopyToOriginalCaps(SIPCallCapability & sdpCapability,DapCallCapability & DapCap)
{
	//copy to original save , encrytion capability need
	AudioCap & audioCap =sdpCapability.m_AudioCap;				      // 音频能力集
	VideoCap & videoCap =sdpCapability.m_VideoCap;				      // 视频能力集
	H239VideoCap & h239VideoCap =sdpCapability.m_H239VideoCap;		  // H239双流能力集
	DataCap & dataCap =sdpCapability.m_DataCap;	                  // 数据能力集

	int i=0;
	audioCap.m_nCapNum =DapCap.m_nAudioCapCount;
	for(i=0; i<DapCap.m_nAudioCapCount; i++)
	{
		audioCap.m_Caps[i].AudioCodecID =DapCap.m_aAudioCap[i].m_AudioCodecType;
		audioCap.m_Caps[i].nDirection   =DapCap.m_aAudioCap[i].m_nDirection;
		audioCap.m_Caps[i].nMaxBitrate  =DapCap.m_aAudioCap[i].m_nAudioBitrate;
	}
	videoCap.m_nCapNum =DapCap.m_nMainVideoCapCount;
	for(i=0; i<DapCap.m_nMainVideoCapCount; i++)
	{
		videoCap.m_Caps[i].VideoCodecType  =DapCap.m_aMainVideoCap[i].m_VideoCodecType;
		videoCap.m_Caps[i].VideoFormatType =DapCap.m_aMainVideoCap[i].m_VideoFormatType;
		videoCap.m_Caps[i].nVideoFrameRate =DapCap.m_aMainVideoCap[i].m_nVideoFrameRate;
		videoCap.m_Caps[i].nDirection      =DapCap.m_aMainVideoCap[i].m_nDirection;
		videoCap.m_Caps[i].nFormatValue    =DapCap.m_aMainVideoCap[i].m_nFormatValue;
		videoCap.m_Caps[i].nMaxBitrate     =DapCap.m_aMainVideoCap[i].m_nVideoBitrate;
	}

	h239VideoCap.m_nCapNum =DapCap.m_nSecondVideoCapCount;
	for(i=0; i<DapCap.m_nSecondVideoCapCount; i++)
	{
		h239VideoCap.m_Caps[i].VideoCodecType  =DapCap.m_aSecondVideoCap[i].m_VideoCodecType;
		h239VideoCap.m_Caps[i].VideoFormatType =DapCap.m_aSecondVideoCap[i].m_VideoFormatType;
		h239VideoCap.m_Caps[i].nVideoFrameRate =DapCap.m_aSecondVideoCap[i].m_nVideoFrameRate;
		h239VideoCap.m_Caps[i].nDirection      =DapCap.m_aSecondVideoCap[i].m_nDirection;
		h239VideoCap.m_Caps[i].nFormatValue    =DapCap.m_aSecondVideoCap[i].m_nFormatValue;
		h239VideoCap.m_Caps[i].nMaxBitrate     =DapCap.m_aSecondVideoCap[i].m_nVideoBitrate;
	}

	dataCap.m_nCapNum =DapCap.m_nDataCapCount;
	for(i=0; i<DapCap.m_nDataCapCount; i++)
	{
		dataCap.m_Caps[i].DataCodecID =(DATA_CODEC_ID) DapCap.m_DataCap[i].m_DataCodecID;
		dataCap.m_Caps[i].nDirection  =DapCap.m_DataCap[i].m_nDirection;
		dataCap.m_Caps[i].nMaxBitrate =DapCap.m_DataCap[i].m_nDataBitrate;
	}
}


void SIPCall::RecvCapCopyToDapCaps(void)
{
	int i=0;
	m_RecvCallCaps.m_nAudioCapCount =m_RemoteAudioCap.m_nCapNum;
	for(i=0; i<m_RemoteAudioCap.m_nCapNum; i++)
	{
		m_RecvCallCaps.m_aAudioCap[i].m_AudioCodecType = m_RemoteAudioCap.m_Caps[i].AudioCodecID;
		m_RecvCallCaps.m_aAudioCap[i].m_nDirection     = m_nCapDirectionDefault ;   //m_RemoteAudioCap.m_Caps[i].nDirection; //??
		m_RecvCallCaps.m_aAudioCap[i].m_nPayload       = m_RemoteAudioCap.m_Caps[i].nPayLoad;
		m_RecvCallCaps.m_aAudioCap[i].m_nClockRate     = m_RemoteAudioCap.m_Caps[i].nClockRate;
	}

	m_RecvCallCaps.m_nMainVideoCapCount =m_RemoteVideoCap.m_nCapNum;
	for(i=0; i<m_RemoteVideoCap.m_nCapNum; i++)
	{
		m_RecvCallCaps.m_aMainVideoCap[i].m_VideoCodecType = m_RemoteVideoCap.m_Caps[i].eCapType;
		m_RecvCallCaps.m_aMainVideoCap[i].m_nDirection = m_nCapDirectionDefault ; //m_RemoteVideoCap.m_Caps[i].nDirection; //??
		m_RecvCallCaps.m_aMainVideoCap[i].m_VideoFormatType = (VIDEO_FORMAT_TYPE)m_RemoteVideoCap.m_Caps[i].nVideoFormatType;
		m_RecvCallCaps.m_aMainVideoCap[i].m_nFormatValue = m_RemoteVideoCap.m_Caps[i].nFormatValue;
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoBitrate = m_RemoteVideoCap.m_Caps[i].nBitrate;
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoFrameRate = m_RemoteVideoCap.m_Caps[i].nFrame;
		m_RecvCallCaps.m_aMainVideoCap[i].m_nPayload = m_RemoteVideoCap.m_Caps[i].nPayLoad;
	}

	m_RecvCallCaps.m_nSecondVideoCapCount =m_RemoteDualVideoCap.m_nCapNum;
	for(i=0; i< m_RemoteDualVideoCap.m_nCapNum; i++)
	{
		m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoCodecType = m_RemoteDualVideoCap.m_Caps[i].eCapType;
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nDirection = m_nCapDirectionDefault ; //m_RemoteDualVideoCap.m_Caps[i].nDirection; //??
		m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoFormatType = (VIDEO_FORMAT_TYPE)m_RemoteDualVideoCap.m_Caps[i].nVideoFormatType;
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nFormatValue = m_RemoteDualVideoCap.m_Caps[i].nFormatValue;
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoBitrate = m_RemoteDualVideoCap.m_Caps[i].nBitrate;
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoFrameRate = m_RemoteDualVideoCap.m_Caps[i].nFrame;
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nPayload = m_RemoteDualVideoCap.m_Caps[i].nPayLoad;
	}

	m_RecvCallCaps.m_nDataCapCount =m_RemoteDataCap.m_nCapNum;
	for(i=0; i< m_RemoteDataCap.m_nCapNum; i++)
	{
		m_RecvCallCaps.m_DataCap[i].m_DataCodecID = (DATA_TYPE_ID)m_RemoteDataCap.m_Caps[i].DataCodecID;
		m_RecvCallCaps.m_DataCap[i].m_nDirection = m_nCapDirectionDefault ; //m_RemoteDataCap.m_Caps[i].nDirection; //??
		m_RecvCallCaps.m_DataCap[i].m_nDataBitrate = m_RemoteDataCap.m_Caps[i].nMaxBitrate;
		m_RecvCallCaps.m_DataCap[i].m_nPayload = m_RemoteDataCap.m_Caps[i].nPayLoad;
	}

}


bool SIPCall::H235CapNegotiate(int aEncry,int bEncry,unsigned int remoteAlgo,EncDecAlgorithmID & algoID)
{
	bool bRet=false;
	return bRet;
}

bool SIPCall::CapabilitisAudioItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem)
{
	bool bCodec=false;
	bool bDirection=false;
	bool bFromat=false;
	bool bBitrate=false;
	bool bEncrytion=false;
	bool bResult=false;
	EncDecAlgorithmID algo=ALGORITHM_NONE;

	AudioCapability * aCap = NULL;
	RecvAudioCap * bCap =NULL;
	if(nLocal ==1 || nLocal ==3)
	{
		aCap =(AudioCapability *)capItem1;
		bCap =(RecvAudioCap *)capItem2;
	}
	else
	{
		aCap =(AudioCapability *)capItem2;
		bCap =(RecvAudioCap *)capItem1;
	}

	bCodec = aCap->m_AudioCodecType == bCap->m_AudioCodecType ? true:false ;
	if(bCap->m_nDirection ==1 || bCap->m_nDirection ==3 ) bDirection = true;

	if(bEncOption) //强制加密情况
	{
		bEncrytion = H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
	}
	else
	{
		bEncrytion=true;
	}

	if(bCodec && bDirection && bEncrytion)
	{
		//check encrytion ??
		NegItem.m_bNegEncryption = H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
		NegItem.m_nAlgorithm     = algo;
		//save
		NegItem.m_CodecType      = aCap->m_AudioCodecType;
		NegItem.m_nPayload       = bCap->m_nPayload;
		NegItem.m_nClockRate     = bCap->m_nClockRate;
		NegItem.m_bNegMedia      = true;
		bResult =true;
	}

	return bResult;
}

bool SIPCall::CapabilitisDataItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem)
{
	bool bCodec=false;
	bool bDirection=false;
	bool bFromat=false;
	bool bBitrate=false;
	bool bEncrytion=false;
	bool bResult=false;
	EncDecAlgorithmID algo=ALGORITHM_NONE;

	DataCapability * aCap = NULL;
	RecvDataCap * bCap =NULL;
	if(nLocal ==1 || nLocal ==3)
	{
		aCap =(DataCapability *)capItem1;
		bCap =(RecvDataCap *)capItem2;
	}
	else
	{
		aCap =(DataCapability *)capItem2;
		bCap =(RecvDataCap *)capItem1;
	}

	bCodec = aCap->m_DataCodecID == bCap->m_DataCodecID ? true:false ;
	if(bCap->m_nDirection ==1 || bCap->m_nDirection ==3 ) bDirection = true;

	if(bEncOption) //强制加密情况
	{
		bEncrytion = H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
	}
	else
	{
		bEncrytion=true;
	}

	if(bCodec && bDirection && bEncrytion)
	{
		//check encrytion ??
		NegItem.m_bNegEncryption  = H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
		NegItem.m_nAlgorithm      = algo;
		//save
		NegItem.m_CodecType       = aCap->m_DataCodecID;
		NegItem.m_nPayload        = bCap->m_nPayload;
		NegItem.m_bNegMedia       = true;
		bResult =true;
	}

	return bResult;
}

bool SIPCall::CapabilitisVideoItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem) //nDual 0 is main video ,1 is dual video
{
	bool bCodec=false;
	bool bDirection=false;
	bool bFromat=false;
	bool bBitrate=false;
	bool bEncrytion=false;
	bool bResult=false;
	int  bitrateBack=0;
	int  frameBack = 0;
	EncDecAlgorithmID algo=ALGORITHM_NONE;
	VIDEO_FORMAT_TYPE formatBack =VIDEO_FORMAT_QCIF;

	VideoCapability * aCap = NULL;
	RecvVideoCap * bCap =NULL;
	if(nLocal ==1 || nLocal ==3)
	{
		aCap =(VideoCapability *)capItem1;
		bCap =(RecvVideoCap *)capItem2;
	}
	else
	{
		aCap =(VideoCapability *)capItem2;
		bCap =(RecvVideoCap *)capItem1;
	}

	//协商编码
	bCodec   = aCap->m_VideoCodecType == bCap->m_VideoCodecType ? true:false ;

	//协商分辨率
	if(nLocal ==1 || nLocal ==3 || nLocal ==4)
	{
		bFromat  = aCap->m_nFormatValue <= bCap->m_nFormatValue ? true:false ;
	}
	else if(nLocal ==2)
	{
		bFromat  = aCap->m_nFormatValue >= bCap->m_nFormatValue ? true:false ;
	}

	//协商方向
	if(bCap->m_nDirection ==1 || bCap->m_nDirection ==3 )
	{
		bDirection = true;
	}

	//协商加密
	if(bEncOption) //强制加密情况
	{
		bEncrytion = H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
	}
	else
	{
		bEncrytion=true;
	}

	if(bCodec && bDirection && bEncrytion && bFromat)
	{
		//取分辨率
		if(nLocal ==1 || nLocal ==3 || nLocal ==4)
		{
			formatBack  = aCap->m_VideoFormatType;
		}
		else if(nLocal ==2)
		{
			formatBack  = bCap->m_VideoFormatType;
		}
		//取码率
		if(nLocal ==3 || nLocal ==4)
		{
			bitrateBack  = aCap->m_nVideoBitrate;
		}
		else if(nLocal ==1 || nLocal ==2)
		{
			bitrateBack  = (aCap->m_nVideoBitrate >= bCap->m_nVideoBitrate) ? bCap->m_nVideoBitrate : aCap->m_nVideoBitrate ;
		}
		if(bitrateBack <=0) bitrateBack =768;

		//取帧率
		if(nLocal ==3 || nLocal ==4)
		{
			frameBack  = aCap->m_nVideoFrameRate;
		}
		else if(nLocal ==1 || nLocal ==2)
		{
			frameBack  = (aCap->m_nVideoFrameRate >= bCap->m_nVideoFrameRate) ? bCap->m_nVideoFrameRate : aCap->m_nVideoFrameRate ;
		}
		if(frameBack <=0) frameBack =VideoFrameBase;

		//check encrytion ??
		NegItem.m_bNegEncryption=H235CapNegotiate(m_ConfInfo.m_nH235MediaUse,bCap->m_nEncryption,bCap->m_nAlgorithm,algo);
		NegItem.m_nAlgorithm=algo;
		//save
		NegItem.m_CodecType       = aCap->m_VideoCodecType;
		NegItem.m_VideoFormatType = formatBack;
		NegItem.m_nBitrate        = bitrateBack;
		NegItem.m_nVideoFrameRate = frameBack;
		NegItem.m_nPayload        = bCap->m_nPayload;
		NegItem.m_bNegMedia       = true;
		bResult =true;
	}

	return bResult;
}

int SIPCall::CapabilitisNegotiate(int nLocalPriority,bool bEncryOption)
{
	bool bResult =false;
	int ii=0;
	int jj=0;

	char nameBuf[64]={0};
	char msg[128]={0};
	NegMediaCap NegItem;
	NegItem.m_bNegMedia=false;

	if(nLocalPriority ==1 || nLocalPriority ==3)
	{
		if (m_bNegCapAudio ==false && m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount>0)
		{
			bResult =false;
			for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount;ii++)
			{
				AudioCapability * aCap =&(m_ConfInfo.m_DAPCallCapability.m_aAudioCap[ii]);
				for (jj=0;jj <m_RecvCallCaps.m_nAudioCapCount;jj++)
				{
					RecvAudioCap * bCap =&(m_RecvCallCaps.m_aAudioCap[jj]);
					bResult=CapabilitisAudioItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					//this different to H323
					m_bNegCapAudio =true;
					m_nAudioPayloadType    = NegItem.m_nPayload;
					m_AudioCodecType       = (X_AUDIO_CODEC_ID)NegItem.m_CodecType;
					m_nAudioClockRate      = NegItem.m_nClockRate;
					break;
				}
			}
		}
		if (m_bNegCapVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount>0)
		{
			bResult =false;
			for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;ii++)
			{
				VideoCapability * aCap =&(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii]);
				for (jj=0;jj <m_RecvCallCaps.m_nMainVideoCapCount;jj++)
				{
					RecvVideoCap * bCap =&(m_RecvCallCaps.m_aMainVideoCap[jj]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					//this different to H323
					m_bNegCapVideo =true;
					m_nMainVideoCodecType =(VIDEC_CODEC_TYPE)NegItem.m_CodecType;
					GetVideoPayloadName(m_nMainVideoCodecType,m_strMainVideoPayloadName);
					m_nMainVideoNegBitrate = NegItem.m_nBitrate;
					m_nMainVideoPayloadType = NegItem.m_nPayload;
					m_nRemoteMainVideoFrame = NegItem.m_nVideoFrameRate;
					m_nMainVideoFormat  = NegItem.m_VideoFormatType;
					GetVideoFormatType(NegItem.m_VideoFormatType,m_nMainVideoWidth,m_nMainVideoHeight);
					CheckZeroSetBitrate(m_nMainVideoWidth,m_nMainVideoHeight,m_nMainVideoNegBitrate);
					sprintf(msg,"CapVideoNeg codec:%s,format w:%d h:%d,bitrate:%d\n",GetVideoCodecName(m_nMainVideoCodecType,nameBuf),m_nMainVideoWidth,m_nMainVideoHeight,m_nMainVideoNegBitrate);
					OutputDebugString(msg);
					break;
				}
			}
		}
					
		if (m_bNegCapDualVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount>0)
		{
			bResult =false;
			for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
			{
				VideoCapability * aCap =&(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii]);
				for (jj=0;jj <m_RecvCallCaps.m_nSecondVideoCapCount;jj++)
				{
					RecvVideoCap * bCap =&(m_RecvCallCaps.m_aSecondVideoCap[jj]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bHasRemoteH239Video=true;
					m_bNegCapDualVideo =true;
					m_nDualVideoCodecType =(VIDEC_CODEC_TYPE)NegItem.m_CodecType;
					GetVideoPayloadName(m_nDualVideoCodecType,m_strDualVideoPayloadName);
					m_nDualVideoBitrate = NegItem.m_nBitrate;
					m_nDualVideoPayloadType = NegItem.m_nPayload;
					m_nRemoteSecondVideoFrame    = NegItem.m_nVideoFrameRate;
					m_nDualVideoFormat  = NegItem.m_VideoFormatType;
					GetVideoFormatType(NegItem.m_VideoFormatType,m_nDualVideoWidth,m_nDualVideoHeight);
					CheckZeroSetBitrate(m_nDualVideoWidth,m_nDualVideoHeight,m_nDualVideoBitrate);
					sprintf(msg,"CapDualVideoNeg codec:%s,format w:%d h:%d,bitrate:%d\n",GetVideoCodecName(m_nDualVideoCodecType,nameBuf),m_nDualVideoWidth,m_nDualVideoHeight,m_nDualVideoBitrate);
					OutputDebugString(msg);
					break;
				}
			}
		}

		if (m_bNegCapData ==false && m_ConfInfo.m_DAPCallCapability.m_nDataCapCount >0)
		{
			bResult =false;
			for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nDataCapCount;ii++)
			{
				DataCapability * aCap =&(m_ConfInfo.m_DAPCallCapability.m_DataCap[ii]);
				for (jj=0;jj <m_RecvCallCaps.m_nDataCapCount;jj++)
				{
					RecvDataCap * bCap =&(m_RecvCallCaps.m_DataCap[jj]);
					bResult=CapabilitisDataItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bNegCapData =true;
					m_DataCodecID = (DATA_CODEC_ID)NegItem.m_CodecType;
					m_nDataPayloadType = NegItem.m_nPayload;
					break;
				}
			}
		}
	}
	else 
	{
		if (m_bNegCapAudio ==false && m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount>0)
		{
			bResult =false;
			for (jj=0;jj <m_RecvCallCaps.m_nAudioCapCount;jj++)
			{
				RecvAudioCap * aCap =&(m_RecvCallCaps.m_aAudioCap[jj]);
				for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount;ii++)
				{
					AudioCapability * bCap =&(m_ConfInfo.m_DAPCallCapability.m_aAudioCap[ii]);
					bResult=CapabilitisAudioItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bNegCapAudio =true;
					m_nAudioPayloadType    = NegItem.m_nPayload;
					m_AudioCodecType       = (X_AUDIO_CODEC_ID)NegItem.m_CodecType;
					m_nAudioClockRate      = NegItem.m_nClockRate;
					break;
				}
			}
		}

		if (m_bNegCapVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount>0)
		{
			bResult =false;
			for (jj=0;jj <m_RecvCallCaps.m_nMainVideoCapCount;jj++)
			{
				RecvVideoCap * aCap =&(m_RecvCallCaps.m_aMainVideoCap[jj]);
				for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;ii++)
				{
					VideoCapability * bCap =&(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bNegCapVideo =true;
					m_nMainVideoCodecType =(VIDEC_CODEC_TYPE)NegItem.m_CodecType;
					GetVideoPayloadName(m_nMainVideoCodecType,m_strMainVideoPayloadName);
					m_nMainVideoNegBitrate= NegItem.m_nBitrate;
					m_nMainVideoPayloadType = NegItem.m_nPayload;
					m_nRemoteMainVideoFrame = NegItem.m_nVideoFrameRate;
					m_nMainVideoFormat  = NegItem.m_VideoFormatType;
					GetVideoFormatType(NegItem.m_VideoFormatType,m_nMainVideoWidth,m_nMainVideoHeight);
					CheckZeroSetBitrate(m_nMainVideoWidth,m_nMainVideoHeight,m_nMainVideoNegBitrate);
					sprintf(msg,"CapVideoNeg codec:%s,format w:%d h:%d,bitrate:%d\n",GetVideoCodecName(m_nMainVideoCodecType,nameBuf),m_nMainVideoWidth,m_nMainVideoHeight,m_nMainVideoNegBitrate);
					OutputDebugString(msg);
					break;
				}
			}
		}

		if (m_bNegCapDualVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount>0)
		{
			bResult =false;
			for (jj=0;jj <m_RecvCallCaps.m_nSecondVideoCapCount;jj++)
			{
				RecvVideoCap * aCap =&(m_RecvCallCaps.m_aSecondVideoCap[jj]);
				for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
				{
					VideoCapability * bCap =&(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bHasRemoteH239Video=true;
					m_bNegCapDualVideo =true;
					m_nDualVideoCodecType =(VIDEC_CODEC_TYPE)NegItem.m_CodecType;
					GetVideoPayloadName(m_nDualVideoCodecType,m_strDualVideoPayloadName);
					m_nDualVideoBitrate = NegItem.m_nBitrate;
					m_nDualVideoPayloadType = NegItem.m_nPayload;
					m_nRemoteSecondVideoFrame    = NegItem.m_nVideoFrameRate;
					m_nDualVideoFormat  = NegItem.m_VideoFormatType;
					GetVideoFormatType(NegItem.m_VideoFormatType,m_nDualVideoWidth,m_nDualVideoHeight);
					CheckZeroSetBitrate(m_nDualVideoWidth,m_nDualVideoHeight,m_nDualVideoBitrate);
					sprintf(msg,"CapDualVideoNeg codec:%s,format w:%d h:%d,bitrate:%d\n",GetVideoCodecName(m_nDualVideoCodecType,nameBuf),m_nDualVideoWidth,m_nDualVideoHeight,m_nDualVideoBitrate);
					OutputDebugString(msg);
					break;
				}
			}
		}

		if (m_bNegCapData ==false && m_ConfInfo.m_DAPCallCapability.m_nDataCapCount >0)
		{
			bResult =false;
			for (jj=0;jj <m_RecvCallCaps.m_nDataCapCount;jj++)
			{
				RecvDataCap * aCap =&(m_RecvCallCaps.m_DataCap[jj]);
				for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nDataCapCount;ii++)
				{
					DataCapability * bCap =&(m_ConfInfo.m_DAPCallCapability.m_DataCap[ii]);
					bResult=CapabilitisDataItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					m_bNegCapData =true;
					m_DataCodecID = (DATA_CODEC_ID)NegItem.m_CodecType;
					m_nDataPayloadType = NegItem.m_nPayload;
					break;
				}
			}
		}
	}

	return 0;
}

int SIPCall::OnOffer(pjmedia_sdp_session **p_sdp)
{
	pj_pool_t *pool=m_pPOOL;
	int nSupportDual=0;
	SIPCallCapability & SdpCap=GetMgrCapability();
	SetLocalCapabilitis(SdpCap);
	//cap neg
	SDP_Negotiate();

	if(m_bNegCapAudio ==false)
	{
		LogFile(NULL,NULL,"","","OnOffer SDP_Negotiate Failed.");
		return -1;
	}

	//Create SDP
	SIPCallCapability answerSdpCap;
	answerSdpCap.RemoveAllCapabilitis();

	DapCallCapability dapCaps;
	memset(&dapCaps,0,sizeof(DapCallCapability));

	//sdp cap add
	if(m_bNegCapAudio)
	{
		//AddAnswerAudioCodecType(answerSdpCap);
		//RecvAudioCap & recvAudioCap = m_RecvCallCaps.m_aAudioCap[m_nNegAudioCapLocation];
		dapCaps.m_nAudioCapCount =1;
		dapCaps.m_aAudioCap[0].m_AudioCodecType = m_AudioCodecType;
		dapCaps.m_aAudioCap[0].m_nDirection = m_nCapDirectionDefault;
	}
	if(m_bNegCapVideo)
	{
		//AddAnswerVideoCodecType(answerSdpCap);
		//RecvVideoCap & recvVideoCap = m_RecvCallCaps.m_aMainVideoCap[m_nNegVideoCapLocation];
		dapCaps.m_nMainVideoCapCount =1;
		dapCaps.m_aMainVideoCap[0].m_VideoCodecType = m_nMainVideoCodecType;
		dapCaps.m_aMainVideoCap[0].m_VideoFormatType = m_nMainVideoFormat;
		dapCaps.m_aMainVideoCap[0].m_nVideoBitrate = m_nMainVideoNegBitrate;
		dapCaps.m_aMainVideoCap[0].m_nVideoFrameRate = m_nRemoteMainVideoFrame;
		dapCaps.m_aMainVideoCap[0].m_nFormatValue = GetVideoFormatValue(m_nMainVideoFormat);
		dapCaps.m_aMainVideoCap[0].m_nDirection = m_nCapDirectionDefault;

	}
	if(m_bNegCapDualVideo)
	{
		//AddAnswerDualVideoCodecType(answerSdpCap);
		//RecvVideoCap & recvVideoCap = m_RecvCallCaps.m_aSecondVideoCap[m_nNegDualVideoCapLocation];
		dapCaps.m_nSecondVideoCapCount =1;
		dapCaps.m_aSecondVideoCap[0].m_VideoCodecType = m_nDualVideoCodecType;
		dapCaps.m_aSecondVideoCap[0].m_VideoFormatType = m_nDualVideoFormat;
		dapCaps.m_aSecondVideoCap[0].m_nVideoBitrate = m_nDualVideoBitrate;
		dapCaps.m_aSecondVideoCap[0].m_nVideoFrameRate = m_nRemoteSecondVideoFrame;
		dapCaps.m_aSecondVideoCap[0].m_nFormatValue = GetVideoFormatValue(m_nDualVideoFormat);
		dapCaps.m_aSecondVideoCap[0].m_nDirection = m_nCapDirectionDefault;

	}
	if(m_bNegCapData)
	{
		//AddAnswerDataCodecType(answerSdpCap);
		////RecvDataCap & recvDataCap = m_RecvCallCaps.m_DataCap[m_nNegDataCapLocation];
		//dapCaps.m_nDataCapCount =1;
		//dapCaps.m_DataCap[0].m_DataCodecID =( DATA_TYPE_ID)m_DataCodecID;
		//dapCaps.m_DataCap[0].m_nDirection = m_nCapDirectionDefault;
	}

	DapCapCopyToOriginalCaps(answerSdpCap,dapCaps);   //notice 

	if(m_bNegCapDualVideo) nSupportDual=1;

	m_rSIPUser.m_rSIPCallMgr.SetCapabilityParam(this,answerSdpCap,nSupportDual,m_strLocalIP,m_nLocalAudioPort,m_nLocalVideoPort,m_nLocalDualVideoPort,m_nLocalBFCPTransPort,m_nCallDir);

	if(answerSdpCap.SDP_Capabilitis_Add(pool,p_sdp)<0)
	{
		LogFile(NULL,NULL,"","","OnOffer SDP_Capabilitis_Add Error.");
		return -1;
	}

	return 0;
}

void SIPCall::GetVideoPayloadName(VIDEC_CODEC_TYPE nCodecType,std::string & strVideoPayloadName)
{
	switch(nCodecType)
	{
	case VIDEC_CODEC_H264:
		strVideoPayloadName="H264";
		break;
	case VIDEC_CODEC_H264_SVC:
		strVideoPayloadName="H264SVC";
		break;
	case VIDEC_CODEC_H263:
		strVideoPayloadName="H263";
		break;
	case VIDEC_CODEC_H263P:
		strVideoPayloadName="H263-1998";
		break;
	case VIDEC_CODEC_H263PP:
		strVideoPayloadName="H263-2000";
		break;
	default:
		break;
	}
}

void SIPCall::CopyVideoCapOtherParam(ConfInfo *pConfInfo,int setBitrate,int setFormat)
{
	m_nMainVideoBitrate =GetVideoCapMaxBitrate(pConfInfo,0);
	m_nSecondVideoMaxBitrate =GetVideoCapMaxBitrate(pConfInfo,1);
}

int SIPCall::GetVideoCapMaxBitrate(ConfInfo *pConfInfo,int nDual)  //0 is main video ,1 is dual video
{
	int nMaxValue =0;
	if(pConfInfo)
	{
		if(nDual ==0 && m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount >0)
		{
			nMaxValue = m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[0].m_nVideoBitrate;
			for(int count=0;count<m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;count++)
			{
				if(nMaxValue < m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[count].m_nVideoBitrate )
				{
					nMaxValue =m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[count].m_nVideoBitrate;
				}
			}
		}

		if(nDual ==1 && m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount >0)
		{
			nMaxValue = m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[0].m_nVideoBitrate;
			for(int count=0;count<m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;count++)
			{
				if(nMaxValue < m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[count].m_nVideoBitrate )
				{
					nMaxValue = m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[count].m_nVideoBitrate;
				}
			}
		}
	}

	return nMaxValue;
}

void SIPCall::LogRecvCapDapCaps(void)
{
	int i=0;
	char msg[256]={0};
	char nameBuf[64]={0};
	char nameBuf2[64]={0};
	char szTime[128]={0};

	time_t now = time(NULL);
	_tcsftime(szTime, 128, _T("%m-%d %H:%M:%S"), localtime(&now));
	sprintf(msg,"\nCap Recv Time %s:\n\n",szTime); 
	OutputDebugString(msg);

	for(i=0; i<m_RecvCallCaps.m_nAudioCapCount; i++)
	{
		sprintf(msg,"SIP Recv Audio Cap Index:%d, codec:%s,  dir:%d,payload:%d,enc:%d,algo:%d\n",i, 
		GetAudioCodecName(m_RecvCallCaps.m_aAudioCap[i].m_AudioCodecType,nameBuf) ,
		m_RecvCallCaps.m_aAudioCap[i].m_nDirection,
		m_RecvCallCaps.m_aAudioCap[i].m_nPayload,
		m_RecvCallCaps.m_aAudioCap[i].m_nEncryption,
		m_RecvCallCaps.m_aAudioCap[i].m_nAlgorithm );
		OutputDebugString(msg);
		LogFile(NULL,NULL,"","",msg);
	}

	for(i=0; i<m_RecvCallCaps.m_nMainVideoCapCount; i++)
	{
		sprintf(msg,"SIP Recv Video Cap Index:%d, codec:%s, format:%s,  dir:%d,formateValue:%d,bitrate:%d,frame:%d,payload:%d,enc:%d,algo:%d\n",i, 
		GetVideoCodecName(m_RecvCallCaps.m_aMainVideoCap[i].m_VideoCodecType,nameBuf) ,
		GetVideoFormatName(m_RecvCallCaps.m_aMainVideoCap[i].m_VideoFormatType,nameBuf2) ,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nDirection,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nFormatValue ,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoBitrate,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoFrameRate,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nPayload,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nEncryption,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nAlgorithm );
		OutputDebugString(msg);
		LogFile(NULL,NULL,"","",msg);
	}

	for(i=0; i< m_RecvCallCaps.m_nSecondVideoCapCount; i++)
	{
		sprintf(msg,"SIP Recv Dual Video Cap Index:%d, codec:%s, format:%s,  dir:%d,formateValue:%d,bitrate:%d,frame:%d,payload:%d,enc:%d,algo:%d\n",i, 
		GetVideoCodecName(m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoCodecType,nameBuf) ,
		GetVideoFormatName(m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoFormatType,nameBuf2) ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nDirection ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nFormatValue ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoBitrate ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoFrameRate,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nPayload,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nEncryption,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nAlgorithm );

		OutputDebugString(msg);
		LogFile(NULL,NULL,"","",msg);
	}

	OutputDebugString("\n");
}

void SIPCall::RecvCapDapCapsSort(VIDEC_CODEC_TYPE codecType)
{
	int i=0; int j=0;
	RecvVideoCap tempCap;
	int capSize =sizeof(RecvVideoCap);

	for(i=0;i<m_RecvCallCaps.m_nMainVideoCapCount; i++)
	{
		for(j=i;j<m_RecvCallCaps.m_nMainVideoCapCount; j++)
		{
			if(m_RecvCallCaps.m_aMainVideoCap[i].m_VideoCodecType ==codecType && 
				m_RecvCallCaps.m_aMainVideoCap[j].m_VideoCodecType ==codecType && 
				m_RecvCallCaps.m_aMainVideoCap[i].m_nFormatValue < m_RecvCallCaps.m_aMainVideoCap[j].m_nFormatValue)
			{
				memcpy(&tempCap,m_RecvCallCaps.m_aMainVideoCap +i,capSize );
				memcpy(m_RecvCallCaps.m_aMainVideoCap +i,m_RecvCallCaps.m_aMainVideoCap +j, capSize);
				memcpy(m_RecvCallCaps.m_aMainVideoCap +j,&tempCap,capSize );
			}
		}
	}

	for(i=0;i<m_RecvCallCaps.m_nSecondVideoCapCount; i++)
	{
		for(j=i; j<m_RecvCallCaps.m_nSecondVideoCapCount; j++)
		{
			if( m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoCodecType ==codecType && 
				m_RecvCallCaps.m_aSecondVideoCap[j].m_VideoCodecType ==codecType && 
				m_RecvCallCaps.m_aSecondVideoCap[i].m_nFormatValue < m_RecvCallCaps.m_aSecondVideoCap[j].m_nFormatValue)
			{
				memcpy(&tempCap,m_RecvCallCaps.m_aSecondVideoCap +i,capSize );
				memcpy(m_RecvCallCaps.m_aSecondVideoCap +i,m_RecvCallCaps.m_aSecondVideoCap +j, capSize);
				memcpy(m_RecvCallCaps.m_aSecondVideoCap +j,&tempCap,capSize );
			}
		}
	}
}
