#include "stdafx.h"
#include "H323Call.h"
#include "H323Channel.h"
#include "H323CallMgr.h"
#include "XSocket.h"
#include "stkHandler.h"
#include "CallMgr.h"
#include "CapalityNewType.h"
#include "XUtil.h"
#include "RTPHeader.h"
#include "H323CallMessage.h"
#include <NETEC/NETEC_Core.h>
#include <RVINC/cmctrl.h>
#include "H323RegUser.h"

//add medianetrtp channel 0615
#include "../IMXEC_PJSIP/MediaNet_RTP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IPCLIB_API

extern void IPCLIB_LogL2(const char* pchLog,...);

extern unsigned short GetRTPPort(unsigned short usRTPBasePort);
extern const std::string&GetLocalIPByRemoteIP(const std::string&strHost,unsigned short nPort);

extern "C" int RVCALLCONV utlChr2Bmp(const char*str,RvUint8*vmpStr);
extern "C" int RVCALLCONV utlBmp2Chr(OUT  char * str,IN const RvUint8* bmpStr,IN   int bmpLen);

extern void LogFile(HCALL hsCall,void * p,const char * cszUserID,const char * callID,const char * info);
extern void LogFileMediaState(H323MediaCall * pH323Call,void * p2,int nState,const char * cszUserID,const char * callID,const char * info);
extern void LogFileCall(H323MediaCall * pH323Call,void * p2, HCHAN hsChan,const char * cszUserID,const char * callID,const char * info,int type);
extern VIDEO_FORMAT_TYPE GetVideoFormatType(int nVideoWidth,int nVideoHeight);
extern void GetVideoFormatType(VIDEO_FORMAT_TYPE nFormatType,int & w,int & h);

int g_nEnableNetEC = 1;
int	g_nEnableFlowcontrol = 0;

#define MAX_KEY_FRAME_INTERVAL	3		// 单位：秒
#define STATISTICS_TIME			10		// 单位：秒
#define MAX_LOST_PACKET_RATE	0.005	// 丢包数0.5%
#define MIN_BIRATE_RATE			0.5		// 下调最小的码率比例为通道带宽的50%
#define BITRATE_REDUCE_RATE		0.8		// 码率下调速率
#define BITRATE_INCREASE_RATE	1.1		// 码率上调速率
//////////////////////////////////////////////////////////////////////////////////////////

H323Call::H323Call(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo)
: m_rH323CallMgr(rH323CallMgr)
, m_hsCall(hsCall)
{
	char szCallID[128];
	sprintf(szCallID, "H323:%u", (unsigned long)this+(unsigned long)hsCall);
	m_strCallID = szCallID;
	m_nCapDirectionDefault =1;
	m_bHasRemoteAudio=false;
	m_nRemoteAudioCodecID=X_AUDIO_CODEC_G711U;
	m_bHasRemoteVideo=false;
	m_nRemoteMainVideoCodecType=VIDEC_CODEC_H264;
	m_nRemoteMainVideoMaxBitrate=384;
	m_nRemoteMainVideoFormatType=VIDEO_FORMAT_COUNT;
	m_nMainVideoQCIF=0;
	m_nMainVideoCIF=0;
	m_nMainVideo4CIF=0;
	m_nMainVideoH264Level=0;
	m_bHasRemoteH239Video=false;
	m_nRemoteH239VideoCodecType=VIDEC_CODEC_H264;
	m_nRemoteH239VideoMaxBitrate=128;
	m_nRemoteH239VideoFormatType=VIDEO_FORMAT_COUNT;
	m_nH239QCIF=0;
	m_nH239CIF=0;
	m_nH2394CIF=0;
	m_nH239VideoH264Level=0;
	m_bHasRemoteData=false;

	m_pAudioSendChannel=NULL;
	m_pAudioRecvChannel=NULL;
	m_pMainVideoSendChannel=NULL;
	m_pMainVideoRecvChannel=NULL;
	m_pSecondVideoSendChannel=NULL;
	m_pSecondVideoRecvChannel=NULL;
	m_pH224SendChannel=NULL;
	m_pH224RecvChannel=NULL;
	m_nPTZSpeed=0;
	m_nH225Version=6;
	m_bH239ReceiverOpen=false;
	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	m_bSecondSendCap=false;            //true 修改能力集重发 false 不发
	m_nRemoteTerminalType=1;           //1 一般 2 MCU 
	m_strProductID="";
	m_strVersionID="";
	m_nLastRecvdPTZAction=PTZ_STOP_PAN_UP;
	m_nCurrentPTZAction=PTZ_STOP_PAN_UP;
	m_nLastCheckPTZActionTimestamp=0;
	m_nRemoteG729Type=2;
	m_strLocalIP="";
	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;

	//H235 Media use
	m_strPrintPathName="";
	m_bHasRemoteAudioFirst=false;
	m_bHasRemoteVideoFirst=false;;
	m_bHasRemoteH239VideoFirst=false;
	m_AudioEncDecAlgoId=ALGORITHM_NONE;
	m_bAudioHasRemoteEncryption=false;
	m_VideoEncDecAlgoId=ALGORITHM_NONE;
	m_bVideoHasRemoteEncryption=false;
	m_H239EncDecAlgoId=ALGORITHM_NONE;
	m_bH239HasRemoteEncryption=false;
	m_DataEncDecAlgoId=ALGORITHM_NONE;
	m_bDataHasRemoteEncryption=false;
	memset(m_strSharedSecret,0,sizeof(m_strSharedSecret));
	memset(m_strSecVector,0,sizeof(m_strSecVector));
	m_hSecCall=0;
	m_bUseDiffieHellman=0;
	m_nSecVector=7;						//7 des vector length 21 3des vector length
	m_bShareSecret=false;				//媒体流加密共享密钥协商结果 false失败 true成功
	m_nH239SessionId=32 ;               //1 is reject
	m_strRecvUserNonstandardInfo ="";
	m_strSetUserNonstandardInfo ="";
	m_bMakeCall=false;

	m_nLocalNodeID="";
	m_nPeerAudioID=0;
	m_nPeerVideoID=0;
	m_nPeerSecondVideoID=0;
	m_nPeerLocalPort=0;
	m_strPeerLocalIP="";
	m_strPeerMCUID="";
	m_nEnableMulticast=0;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	m_bTokenOwnState=false;
	m_strConferenceID="";
	m_bVirtualMark=false;
	m_pVirtualCall=NULL;

#endif

//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	m_CallCapability.RemoveAllCapabilitis();
	m_EncryptionCapability.RemoveAllCapabilitis();

#endif

	m_nRecvH239TokenReq=0;
	m_nCapabilityNodeID=0;
	m_nMainVideoFormatType=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFormatType=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH263=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH264=VIDEO_FORMAT_QCIF;
	m_nMainVideoFTH264HP=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH263=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH264=VIDEO_FORMAT_QCIF;
	m_nSecondVideoFTH264HP=VIDEO_FORMAT_QCIF;
	m_nCallDir=IMXEC_CALL_DIR_UNKOWN;
	m_strRecvUserInfomationNonstandData="";

	m_nMainVideoMaxBitrate=384;
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

	if (pConfInfo)
	{
		memcpy(&m_ConfInfo,pConfInfo,sizeof(ConfInfo));
		ConfCapabilitisDefault();
	}
	m_nSupportH239=0;
	m_nInvalidSIDOpenCount=0;
	m_strMediaMulticastIP ="";
	m_bMediaMulticastEnable=false;
	m_nMediaMulticastTTL=0;

	m_nRemoteMainVideoOLCMaxMBPS=0;
	m_nRemoteMainVideoOLCMaxFS=0;
	m_nRemoteMainVideoOLCMaxDPB=0;
	m_nRemoteMainVideoOLCMaxBR=0;
	m_nRemoteDualVideoOLCMaxMBPS=0;
	m_nRemoteDualVideoOLCMaxFS=0;
	m_nRemoteDualVideoOLCMaxDPB=0;
	m_nRemoteDualVideoOLCMaxBR=0;

	m_nMainVideoFrameRate=30;
	m_nSecondVideoFrameRate=30;

	m_nLastRcvdTripTimestamp=XGetTimestamp();
	m_nterLabelReqSend = 260;
	m_nLastSendGenIndTimestamp=XGetTimestamp();
	m_nLastSendSecretUpdateTS=XGetTimestamp();
	m_nAudioUpdatePT =111;
	m_nMainVideoUpdatePT =112;
	m_nDualVideoUpdatePT =113;
	m_nFECCUpdatePT =114;

	m_nH235MediaUse=0;
	m_nH235AuthUse=0;
	m_nUseAlogrithmId=ALGORITHM_AES_CBC;
	m_nMediaEncrypSpecified=0;
	memset(m_strMediaEncrypPWD,0,sizeof(m_strMediaEncrypPWD));

	if (pConfInfo)
	{
		m_nH235MediaUse=pConfInfo->m_nH235MediaUse;
		m_nH235AuthUse=pConfInfo->m_nH235AuthUse;
		m_nUseAlogrithmId=pConfInfo->m_nUseAlogrithmId;
	}

	m_nMainVideoStartRcvdPacketTimestamp=0;
	m_nMainVideoStartRcvdPacketSequence=0;
	m_nMainVideoRcvdPacketCount=0;
	m_dMainVideoRcvLostRate=0;
	m_nSecondVideoStartRcvdPacketTimestamp=0;
	m_nSecondVideoStartRcvdPacketSequence=0;
	m_nSecondVideoRcvdPacketCount=0;
	m_dSecondVideoRcvLostRate=0;
	m_bMainVideoReqKeyFrame=true;
	m_bSecondVideoReqKeyFrame=true;
	m_nMainVideoLastReqKeyFrameTimestamp=0;
	m_nSecondVideoLastReqKeyFrameTimestamp=0;
	m_strPeerNodeID ="";
	m_strMCUID ="";
	m_bRegCall=false;
	m_nBandWidth =768;

	m_nRemoteMainVideoFrameRate=30;
	m_nRemoteH239VideoFrameRate=30;
	m_nUseFWNAT =0;
	m_nMasterCall=0;
	m_nBandWidth=0;
	m_strCalled ="";
	m_nSendDTMFRTP = 0;
	m_pH323RegUser =NULL;
	m_nParamCRV =0;
	memset(m_szConferenceID,0,sizeof(m_szConferenceID));
	memset(m_szCallEndpointID,0,sizeof(m_szCallEndpointID));

	m_strHost="";
	m_nRegUseGK=2;
	memset(&m_GKAlias,0,sizeof(cmAlias) );
	memset(&m_szGKAliasBuf,0,sizeof(m_szGKAliasBuf));
	m_GKAlias.string = m_szGKAliasBuf;

	memset(&m_EPAlias,0,sizeof(cmAlias) );
	memset(&m_szEPAliasBuf,0,sizeof(m_szEPAliasBuf));
	m_EPAlias.string = m_szEPAliasBuf;
	m_bCallState =false;

	m_strE164ID ="";
	m_strH323ID ="";

	m_nMainVideoSendState =1;      //test
	m_nSecondVideoSendState=1;
	m_nAudioSendState=1;

	m_nMainVideoBitRate=0;
	m_nMainVideoActualBitRate=0;
	m_nSecondVideoBitRate=0;
	m_nSecondVideoActualBitRate=0;
	m_nLocalHungUP=0;
	m_nRoundTripSendCount=0;
	m_bRoundTripMaxCount=4;
	m_nNETECNodeCount =0;
	m_nMasterSlaveValue = 0;
	m_nCallNum =0;
	m_nRemoteCapChaneged =0;
	m_nForceVideoWid=0;
	m_nForceVideoHei=0;
	m_nForceDualVideoWid=0;
	m_nForceDualVideoHei=0;
	m_nSendH239TokenReq =0;
	m_nRecvGenericResp =false;
	m_nSendGenericReq  =false;
	m_nRecvGenericReqCount =0;
	m_nRemoteMainVideoFrame = VideoFrameBase ; //??
	m_nRemoteSecondVideoFrame = VideoFrameBase ;
	memset(&m_RecvCallCaps,0,sizeof(m_RecvCallCaps) );
	//Added by jiangdingfeng 20140929 默认为不需要生成SSRC
	m_bFlag = false;
	m_nForceIP =0;
	m_bForceIP =true;
	m_nTCSUseTerminal =0;
	m_strMakeDestIP="";
	m_strMakeSourceIP ="";
	m_strQ931CalledNumber ="";
	m_strQ931CallerNumber ="";
	m_nEntityType = EM_ENTITY_UNKNOWN;
}

H323Call::~H323Call()
{
}

void H323Call::Close(void)
{
	m_bSecondSendCap=false;
	m_nRemoteTerminalType=1;
	if (m_hsCall!=NULL)
	{
		cmCallDrop(m_hsCall);
		m_hsCall=NULL;
	}
	MapH323Channel::iterator iter = m_MapH323Channel.begin();
	while (iter != m_MapH323Channel.end())
	{
		H323Channel* pH323Channel=iter->second;
		m_MapH323Channel.erase(iter);

		pH323Channel->OnChannelDisconnected();

		delete pH323Channel;
		pH323Channel=NULL;

		iter = m_MapH323Channel.begin();
	}
	if(m_nCapabilityNodeID > 0)
	{
		HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		pvtDelete(hVal,m_nCapabilityNodeID);
		m_nCapabilityNodeID=0;
	}
}

const std::string& H323Call::GetCallID(void)
{
	return m_strCallID;
}

void H323Call::OpenAudioOutChannel()
{
	if (m_bHasRemoteAudio && m_pAudioSendChannel == NULL) 
	{
		HCHAN hsChan=NULL;
		cmChannelNew(m_hsCall,NULL,&hsChan);

		int nChannelIndex=0;
		m_pAudioSendChannel = NewChannel(hsChan);
		if(m_pAudioSendChannel)
		{
			cmChannelSetHandle(hsChan,(HAPPCHAN)m_pAudioSendChannel);
			m_pAudioSendChannel->Direction() = cmCapTransmit;
			//use h235 media
			m_pAudioSendChannel->m_bH235Cap=m_bAudioHasRemoteEncryption;
			m_pAudioSendChannel->AlgorithmId()=m_AudioEncDecAlgoId;
			std::string strChannelName="g711Ulaw64k";
			switch (m_nRemoteAudioCodecID)
			{
			case X_AUDIO_CODEC_G711U:
				nChannelIndex=CH_G711U_64K;
				strChannelName="g711Ulaw64k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 0;
				m_pAudioSendChannel->MaxBitrate()=(64);
				break;
			case X_AUDIO_CODEC_G711A:
				nChannelIndex=CH_G711A_64K;
				strChannelName="g711Alaw64k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 8;
				m_pAudioSendChannel->MaxBitrate()=(64);
				break;
			case X_AUDIO_CODEC_G722_64:
				nChannelIndex=CH_G722_64K;
				strChannelName="g722-64k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 9;
				m_pAudioSendChannel->MaxBitrate()=(64);
				break;
			case X_AUDIO_CODEC_G7221_7_24:
				nChannelIndex=CH_G7221_24K;
				strChannelName="g7221-24k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 121;
				m_pAudioSendChannel->MaxBitrate()=(24);
				break;
			case X_AUDIO_CODEC_G7221_7_32:
				nChannelIndex=CH_G7221_32K;
				strChannelName="g7221-32k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 122;
				m_pAudioSendChannel->MaxBitrate()=(32);
				break;
			case X_AUDIO_CODEC_G7221_14_24:
				nChannelIndex=CH_G7221C_24K;
				strChannelName="g7221C-24k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 123;
				m_pAudioSendChannel->MaxBitrate()=(24);
				break;
			case X_AUDIO_CODEC_G7221_14_32:
				nChannelIndex=CH_G7221C_32K;
				strChannelName="g7221C-32k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 124;
				m_pAudioSendChannel->MaxBitrate()=(32);
				break;
			case X_AUDIO_CODEC_G7221_14_48:
				nChannelIndex=CH_G7221C_48K;
				strChannelName="g7221C-48k";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 125;
				m_pAudioSendChannel->MaxBitrate()=(48);
				break;
			case X_AUDIO_CODEC_G719_32:
				nChannelIndex=0;
				strChannelName="g719";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 126;
				m_pAudioSendChannel->MaxBitrate()=(32);
				break;
			case X_AUDIO_CODEC_G719_48:
				nChannelIndex=0;
				strChannelName="g719";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 127;
				m_pAudioSendChannel->MaxBitrate()=(48);
				break;
			case X_AUDIO_CODEC_G719_64:
				nChannelIndex=0;
				strChannelName="g719";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 120;
				m_pAudioSendChannel->MaxBitrate()=(64);
				break;
			case X_AUDIO_CODEC_G7231_63:
				nChannelIndex=CH_G7231;
				strChannelName="g7231";
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 4;
				break;
			case X_AUDIO_CODEC_G729:
				nChannelIndex=0;
				//code add use kdc mcu
				m_pAudioSendChannel->AudioCodecID() = m_nRemoteAudioCodecID;
				m_pAudioSendChannel->PayloadType() = 18;
				if(m_nRemoteG729Type==2) 
				{
					strChannelName="g729AnnexA";
				}
				if(m_nRemoteG729Type==1) 
				{
					strChannelName="g729";
				}

				break;
			};

			m_pAudioSendChannel->ChannelName()=strChannelName;
			m_pAudioSendChannel->m_nDirection = cmCapTransmit;

			//设置音频发送信道端口号
			ChannelMediaType nMediaType=Enum_Channel_Media_Audio;
			m_pAudioSendChannel->m_nMediaType=nMediaType;
			unsigned short port = this->GetMediaRtpPort(nMediaType,m_pAudioSendChannel,1);/*GetCallRTPPort();*/

			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			if(m_bForceIP) ulLocalIP =m_nForceIP;
			m_pAudioSendChannel->SetLocalRTPAddress(ulLocalIP, port);
			m_pAudioSendChannel->SetLocalRTCPAddress(ulLocalIP, port+1);

			if(m_pAudioSendChannel->PayloadType()>=96)
				cmChannelSetDynamicRTPPayloadType(hsChan,m_pAudioSendChannel->PayloadType());

			unsigned long nExternalIPAddress=GetMgrExternalIPAddress();
			//if(m_bForceIP) nExternalIPAddress =m_nForceIP;
			if (nExternalIPAddress)
			{
				cmChannelSetRTCPAddress(hsChan, nExternalIPAddress, m_pAudioSendChannel->m_nLocalRTCPPort);
			}
			else
			{
				unsigned long m_ulRTCPLocalIP =m_pAudioSendChannel->m_ulRTCPLocalIP;
				if(m_bForceIP) m_ulRTCPLocalIP =m_nForceIP;
				cmChannelSetRTCPAddress(hsChan, m_ulRTCPLocalIP, m_pAudioSendChannel->m_nLocalRTCPPort);
			}
			int nError=BuildSendAudioOLCCapabilitis();
			//cmChannelOpen(hsChan,strChannelName.c_str(),NULL,NULL,0);
			//add
			if(nError >=0) m_pAudioSendChannel->SetChannelUpdatePayloadType(m_nAudioUpdatePT);
		}
	}
}

void H323Call::OpenVideoOutChannel()
{
	if (m_bHasRemoteVideo && m_pMainVideoSendChannel == NULL) 
	{
		HCHAN hsChan=NULL;
		cmChannelNew(m_hsCall,NULL,&hsChan);
		m_pMainVideoSendChannel = NewChannel(hsChan);
		if(m_pMainVideoSendChannel)
		{
			cmChannelSetHandle(hsChan,(HAPPCHAN)m_pMainVideoSendChannel);
			m_pMainVideoSendChannel->Direction() = cmCapTransmit;

			//use h235 media
			m_pMainVideoSendChannel->m_bH235Cap=m_bVideoHasRemoteEncryption;
			m_pMainVideoSendChannel->AlgorithmId()=m_VideoEncDecAlgoId;
			std::string strChannelName="h263VideoCapability";

			//m_nRemoteMainVideoCodecType =VIDEC_CODEC_H263;//test
			//m_nRemoteMainVideoFormatType =VIDEO_FORMAT_4CIF;

			//m_nRemoteMainVideoCodecType =VIDEC_CODEC_H264;//test
			//m_nRemoteMainVideoFormatType =VIDEO_FORMAT_720P;
			//m_nMainVideoH264Level =71;

			switch (m_nRemoteMainVideoCodecType)
			{
			case VIDEC_CODEC_H261:
				strChannelName="h261VideoCapability";
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 31;
				break;
			case VIDEC_CODEC_H263:
				strChannelName="h263VideoCapability";
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 34;
				break;
			case VIDEC_CODEC_H263P:
				strChannelName="h263v2";
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 107;
				cmChannelSetDynamicRTPPayloadType(hsChan,107);
				break;
			case VIDEC_CODEC_H263PP:
				strChannelName="h263v3";
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 108;
				cmChannelSetDynamicRTPPayloadType(hsChan,108);
				break;
			case VIDEC_CODEC_H264_SVC:
			case VIDEC_CODEC_H264:
				strChannelName="h264";
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 106;
				cmChannelSetDynamicRTPPayloadType(hsChan,106);
				m_pMainVideoSendChannel->H264Level()=m_nMainVideoH264Level;
				break;
			case VIDEC_CODEC_MPEG4:
				strChannelName="MPEG4";//notice ,use to call kedacom
				m_pMainVideoSendChannel->VideoFormatType() = m_nRemoteMainVideoFormatType;
				m_pMainVideoSendChannel->VideoCodecType() = m_nRemoteMainVideoCodecType;
				m_pMainVideoSendChannel->PayloadType() = 106;
				cmChannelSetDynamicRTPPayloadType(hsChan,106);
				break;
			};

			m_pMainVideoSendChannel->ChannelName() = strChannelName;
			m_pMainVideoSendChannel->MaxBitrate() = (m_nRemoteMainVideoMaxBitrate);
			m_pMainVideoSendChannel->m_nDirection = cmCapTransmit;
			m_pMainVideoSendChannel->VideoFrameRate() = m_nRemoteMainVideoFrame;

			//设置视频发送信道端口号
			ChannelMediaType nMediaType=Enum_Channel_Media_Video;
			m_pMainVideoSendChannel->m_nMediaType=nMediaType;
			unsigned short port = this->GetMediaRtpPort(nMediaType,m_pMainVideoSendChannel,1);

			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			if(m_bForceIP) ulLocalIP =m_nForceIP;
			m_pMainVideoSendChannel->SetLocalRTPAddress(ulLocalIP, port);
			m_pMainVideoSendChannel->SetLocalRTCPAddress(ulLocalIP, port+1);

			unsigned long nExternalIPAddress=GetMgrExternalIPAddress();
			//if(m_bForceIP) nExternalIPAddress =m_nForceIP;
			if (nExternalIPAddress)
			{
				cmChannelSetRTCPAddress(hsChan, nExternalIPAddress, m_pMainVideoSendChannel->m_nLocalRTCPPort);
			}
			else
			{
				unsigned long m_ulRTCPLocalIP =m_pMainVideoSendChannel->m_ulRTCPLocalIP;
				if(m_bForceIP) m_ulRTCPLocalIP =m_nForceIP;
				cmChannelSetRTCPAddress(hsChan, m_ulRTCPLocalIP, m_pMainVideoSendChannel->m_nLocalRTCPPort);
			}

			m_pMainVideoSendChannel->SetH264CapOptionParam(0);
			int nError=BuildSendVideoOLCCapabilitis();
			//int nError=cmChannelOpen(hsChan,strChannelName.c_str(),NULL,NULL,0);
			//add
			if(nError >=0) m_pMainVideoSendChannel->SetChannelUpdatePayloadType(m_nMainVideoUpdatePT);
		}
	}
}

void H323Call::OpenH239VideoOutChannel()
{
	if (m_bHasRemoteH239Video && m_pSecondVideoSendChannel == NULL) 
	{
		HCHAN hsChan=NULL;
		cmChannelNew(m_hsCall,NULL,&hsChan);
		m_pSecondVideoSendChannel = NewChannel(hsChan);

		// 注意 sessionId分配 在此分配为了32
		int sessionId=m_nH239SessionId;
		m_nchIDReqSend=::GetSecondLogicalChannelNum(hsChan,sessionId);

		if(m_pSecondVideoSendChannel)
		{
			cmChannelSetHandle(hsChan,(HAPPCHAN)m_pSecondVideoSendChannel);
			m_pSecondVideoSendChannel->Direction() = cmCapTransmit;
			//use h235 media
			m_pSecondVideoSendChannel->m_bH235Cap=m_bH239HasRemoteEncryption;
			m_pSecondVideoSendChannel->AlgorithmId()=m_H239EncDecAlgoId;

			std::string strChannelName="h263Extend";
			switch (m_nRemoteH239VideoCodecType)
			{
			case VIDEC_CODEC_H261:
				strChannelName="h261Extend";
				m_pSecondVideoSendChannel->VideoFormatType() = m_nRemoteH239VideoFormatType;
				m_pSecondVideoSendChannel->VideoCodecType() = m_nRemoteH239VideoCodecType;
				m_pSecondVideoSendChannel->PayloadType() = 31;
				break;
			case VIDEC_CODEC_H263:
				strChannelName="h263Extend";
				m_pSecondVideoSendChannel->VideoFormatType() = m_nRemoteH239VideoFormatType;
				m_pSecondVideoSendChannel->VideoCodecType() = m_nRemoteH239VideoCodecType;
				m_pSecondVideoSendChannel->PayloadType() = 34;
				break;
			case VIDEC_CODEC_H263P:
				strChannelName="h263v2Extend";
				m_pSecondVideoSendChannel->VideoFormatType() = m_nRemoteH239VideoFormatType;
				m_pSecondVideoSendChannel->VideoCodecType() = m_nRemoteH239VideoCodecType;
				m_pSecondVideoSendChannel->PayloadType() = 107;
				cmChannelSetDynamicRTPPayloadType(hsChan,107);
				break;
			case VIDEC_CODEC_H263PP:
				strChannelName="h263v3Extend";
				m_pSecondVideoSendChannel->VideoFormatType() = m_nRemoteH239VideoFormatType;
				m_pSecondVideoSendChannel->VideoCodecType() = m_nRemoteH239VideoCodecType;
				m_pSecondVideoSendChannel->PayloadType() = 108;
				cmChannelSetDynamicRTPPayloadType(hsChan,108);
				break;			
			case VIDEC_CODEC_H264_SVC:
			case VIDEC_CODEC_H264:
				strChannelName="h264Extend";
				m_pSecondVideoSendChannel->VideoFormatType() = m_nRemoteH239VideoFormatType;
				m_pSecondVideoSendChannel->VideoCodecType() = m_nRemoteH239VideoCodecType;
				m_pSecondVideoSendChannel->PayloadType() = 106;
				cmChannelSetDynamicRTPPayloadType(hsChan,106);
				m_pSecondVideoSendChannel->H264Level()=m_nH239VideoH264Level;
				break;
			};
			m_pSecondVideoSendChannel->ChannelName() =strChannelName;
			m_pSecondVideoSendChannel->MaxBitrate() =(m_nRemoteH239VideoMaxBitrate);
			m_pSecondVideoSendChannel->m_nDirection = cmCapTransmit;
			m_pSecondVideoSendChannel->VideoFrameRate() = m_nRemoteSecondVideoFrame;

			//设置双流发送信道端口号
			ChannelMediaType nMediaType=Enum_Channel_Media_DualVideo;
			m_pSecondVideoSendChannel->m_nMediaType=nMediaType;
			unsigned short port = this->GetMediaRtpPort(nMediaType,m_pSecondVideoSendChannel,1);

			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			if(m_bForceIP) ulLocalIP =m_nForceIP;
			m_pSecondVideoSendChannel->SetLocalRTPAddress(ulLocalIP, port);
			m_pSecondVideoSendChannel->SetLocalRTCPAddress(ulLocalIP, port+1);

			unsigned long nExternalIPAddress=GetMgrExternalIPAddress();
			//if(m_bForceIP) nExternalIPAddress =m_nForceIP;
			if (nExternalIPAddress)
			{
				cmChannelSetRTCPAddress(hsChan, nExternalIPAddress, m_pSecondVideoSendChannel->m_nLocalRTCPPort);
			}
			else
			{
				unsigned long m_ulRTCPLocalIP =m_pSecondVideoSendChannel->m_ulRTCPLocalIP;
				if(m_bForceIP) m_ulRTCPLocalIP =m_nForceIP;
				cmChannelSetRTCPAddress(hsChan, m_ulRTCPLocalIP, m_pSecondVideoSendChannel->m_nLocalRTCPPort);
			}

			m_pSecondVideoSendChannel->SetH264CapOptionParam(1);
			int nError=BuildSendDualVideoOLCCapabilitis();
			//cmChannelOpen(hsChan,strChannelName.c_str(),NULL,NULL,0);
			//add
			if(nError >=0) m_pSecondVideoSendChannel->SetChannelUpdatePayloadType(m_nDualVideoUpdatePT);
		}
	}
}

void H323Call::OpenFECCOutChannel()
{
	if (m_bHasRemoteData && m_pH224SendChannel == NULL) 
	{
		HCHAN hsChan=NULL;
		cmChannelNew(m_hsCall,NULL,&hsChan);
		m_pH224SendChannel = NewChannel(hsChan);
		if(m_pH224SendChannel)
		{
			cmChannelSetHandle(hsChan,(HAPPCHAN)m_pH224SendChannel);
			m_pH224SendChannel->Direction() = cmCapTransmit;

			//use h235 media
			m_pH224SendChannel->m_bH235Cap=m_bDataHasRemoteEncryption;
			m_pH224SendChannel->AlgorithmId()=m_DataEncDecAlgoId;

			std::string stChannelName="h224";
			m_pH224SendChannel->PayloadType() = 100;     //call hdx7000 此必须为100
			cmChannelSetDynamicRTPPayloadType(hsChan,100);
			m_pH224SendChannel->ChannelName()=stChannelName;
			m_pH224SendChannel->MaxBitrate()=(48);

			//设置远遥发送信道端口号
			ChannelMediaType nMediaType=Enum_Channel_Media_H224;
			m_pH224SendChannel->m_nMediaType=nMediaType;
			unsigned short port = this->GetMediaRtpPort(nMediaType,m_pH224SendChannel,1);

			unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
			if(m_bForceIP) ulLocalIP =m_nForceIP;
			m_pH224SendChannel->SetLocalRTPAddress(ulLocalIP, port);
			m_pH224SendChannel->SetLocalRTCPAddress(ulLocalIP, port+1);

			unsigned long nExternalIPAddress=GetMgrExternalIPAddress();
			//if(m_bForceIP) nExternalIPAddress =m_nForceIP;
			if (nExternalIPAddress)
			{
				cmChannelSetRTCPAddress(hsChan, nExternalIPAddress, m_pH224SendChannel->m_nLocalRTCPPort);
			}
			else
			{
				unsigned long m_ulRTCPLocalIP =m_pH224SendChannel->m_ulRTCPLocalIP;
				if(m_bForceIP) m_ulRTCPLocalIP =m_nForceIP;
				cmChannelSetRTCPAddress(hsChan, m_ulRTCPLocalIP, m_pH224SendChannel->m_nLocalRTCPPort);
			}
			int nError=BuildSendDataOLCCapabilitis();
			//cmChannelOpen(hsChan,stChannelName.c_str(),NULL,NULL,0);
			//add
			if(nError >=0) m_pH224SendChannel->SetChannelUpdatePayloadType(m_nFECCUpdatePT);
		}
	}
}

int H323Call::GetH235MediaCapNegotiate(h235MediaCap * pRemoteH235Cap,cmCapStruct* pCapAlt,int codecType,bool h239,int nLevel)
{
	int result=0;
	unsigned int count=0;
	int j=0;
	int bit=0;
	bool localFirst=true;
	int algoExist=0;
	int codecExist=0;
	int k=0;
	int mediaType=0;
	EncDecAlgorithmID algo=ALGORITHM_NONE;

	if(pCapAlt==NULL) return result;
	int entryNum=pCapAlt->capabilityId;
	cmCapDataType type=pCapAlt->type;

	//find cap
	for(count=0;count<pRemoteH235Cap->m_nCapNum;count++)
	{
		if(pRemoteH235Cap->m_Caps[count].entryNum==entryNum)
		{ result=count+1; break; }
	}
	if(result==0) return result;
	//find Negotiate algorithm
	k=2;localFirst=true;algoExist=0;
	while(k)
	{
		result=0;
		for(j=0;j<32;j++)
		{
			bit=BitGet(pRemoteH235Cap->m_Caps[count].algorithm,j);
			if(bit==1) 
			{
				algo=(EncDecAlgorithmID)(j+1);	
				if(localFirst) 
				{
					algoExist=GetMgrEncryptionCapability().IsFirstFromAlgorithm(algo);
				}
				else 
				{
					algoExist=GetMgrEncryptionCapability().IsExistFromAlgorithm(algo);
				}
				if(algoExist)
				{ result=j+1; break;}
			}
		}
		//is no find second times
		if(result==0) localFirst=false;
		else break;

		k--;
	}
	if(result==0) return result;
	
	//find Negotiate codecType
	mediaType=0;
	switch(type)
	{
	case cmCapAudio:
		mediaType=1;
		break;
	case cmCapVideo:
		if(h239) mediaType=3;
		else mediaType=2;
		break;
	case cmCapData:
		mediaType=4;
		break;
	}

	k=2;localFirst=true;codecExist=0;
	while(k)
	{
		result=0;

		if(localFirst) 
		{
			codecExist=GetMgrEncryptionCapability().IsFirstCodecTypeFromCapsArray(mediaType ,codecType,nLevel);
		}
		else 
		{
			codecExist=GetMgrEncryptionCapability().IsExistCodecTypeFromCapsArray(mediaType ,codecType,nLevel);
		}
		if(codecExist)
		{ result=k; break;}

		//is no find second times
		if(result==0) localFirst=false;
		else break;

		k--;
	}
	if(result==0) return result;

	//1 audio 2 video 3 H239 4 Data
	switch(mediaType)
	{
	case 1:
		m_AudioEncDecAlgoId=algo;
		m_bAudioHasRemoteEncryption=true;
		break;
	case 2:
		m_VideoEncDecAlgoId=algo;
		m_bVideoHasRemoteEncryption=true;
		break;
	case 3:
		m_H239EncDecAlgoId=algo;
		m_bH239HasRemoteEncryption=true;
		break;
	case 4:
		m_DataEncDecAlgoId=algo;
		m_bDataHasRemoteEncryption=true;
		break;
	}

	return result;
}

void H323Call::GetH235MediaCapSet(h235MediaCap * pRemoteH235Cap,void* capabilities)
{
	cmCapStruct*** pRemoteCapDes = NULL; // 能力描述表指针
	cmCapStruct** pRemoteCapSim = NULL;  // 同时能力集指针
	cmCapStruct* pRemoteCapAlt = NULL;   // 可选能力指针
	cmCapStruct**** pRemoteCapStruct = (cmCapStruct****)capabilities;
	RvInt32			nValue;
	RvBool			bString;

	char tempbuf[1024];
	int error=-1;
	int count=0;
	int nodeId=-1;
	int  lenData;
	RvBool  bIsString;
	int k=1;
	unsigned int alg=0;
	unsigned char strData[256]={0};
	RvBool  bGetAlgrithm=RV_FALSE;

	HPVT pvtTree = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int iDescriptorRemote = 0, iSimultaneousRemote = 0, iAlternativeRemote = 0; // 对端接收能力三级索引
	
	while( pRemoteCapDes = pRemoteCapStruct[iDescriptorRemote] ) // 对每个描述表扫描
	{
		iSimultaneousRemote = 0; // 从第一个同时能力集开始
		while( pRemoteCapSim = pRemoteCapDes[iSimultaneousRemote] ) // 对同时接收能力集扫描
		{
			iAlternativeRemote = 0; // 从第一个可选接收能力集开始
			while( pRemoteCapAlt = pRemoteCapSim[iAlternativeRemote] ) // 对可选接收能力扫描
			{
				if (pRemoteCapAlt->direction == cmCapReceive || pRemoteCapAlt->direction == cmCapReceiveAndTransmit)
				{
					switch(pRemoteCapAlt->type)
					{
					case cmCapH235:
						if (strcmp(pRemoteCapAlt->name,"H235") != 0) continue;
						nodeId=pvtParent(pvtTree,pRemoteCapAlt->capabilityHandle);
						if(nodeId<0) break;

						error=pvtGetByPath(pvtTree,nodeId,"capability.h235SecurityCapability.mediaCapability",NULL,&nValue,&bString);
						if(error<0) break;

						bGetAlgrithm=RV_FALSE;
						k=1;
						while(1)
						{
							sprintf(tempbuf, "capability.h235SecurityCapability.encryptionAuthenticationAndIntegrity.encryptionCapability.%d.algorithm", k);	
							int nodeData = pvtGetByPath( pvtTree, nodeId, tempbuf, NULL, &lenData, &bIsString );
							if(nodeData<0) break;							
							if( nodeData && bIsString && lenData>0 )
							{								
								if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
								{
									EncDecAlgorithmID algId=ALGORITHM_NONE;
									error=H323CallMgr::Instance().GetEncDecAlgorithmTypeFromOID(algId,strData,lenData);
									if(error)
									{
										unsigned int & algNum=pRemoteH235Cap->m_Caps[pRemoteH235Cap->m_nCapNum].algorithm;
										alg=algId-1;
										BitSet(algNum,alg);
										bGetAlgrithm=RV_TRUE;
									}
								}
							}
							k++;
						}
						if(bGetAlgrithm)
						{
							pRemoteH235Cap->m_Caps[pRemoteH235Cap->m_nCapNum].entryNum=nValue;
							pRemoteH235Cap->m_nCapNum++;
						}				
						break;
					default:
						break;
					}
				}

				iAlternativeRemote++;
			}

			iSimultaneousRemote++;
		}
		iDescriptorRemote++;
	}
}


void H323Call::OnChannelMediaEncryptionState(H323Channel*pH323Channel)
{
	IMXEC_CALL_MEDIA_TYPE mediaType=IMXEC_CALL_MEDIA_UNKOWN;

	if(pH323Channel == m_pAudioRecvChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_AUDIO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelRecvMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_AUDIO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelSendMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_MAINVIDEO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelRecvMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_MAINVIDEO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelSendMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_H239VIDEO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelRecvMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_H239VIDEO;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelSendMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_H224DATA;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelRecvMediaEncryption(m_strCallID.c_str(),mediaType);
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		mediaType=IMXEC_CALL_MEDIA_H224DATA;
		if(pH323Channel->EncryptionStatus()==true) 
			H323CallMgr::Instance().OnChannelSendMediaEncryption(m_strCallID.c_str(),mediaType);
	}
}

//逻辑信道
H323Channel* H323Call::NewChannel(HCHAN hsChan)
{
	H323Channel* pNewH323Channel = new H323Channel(*this,hsChan);
	if(pNewH323Channel)
	{
		AddChannel(pNewH323Channel);
		return pNewH323Channel;
	}

	return NULL;
}

void H323Call::AddChannel(H323Channel*pH323Channel)
{
	m_MapH323Channel[pH323Channel]=pH323Channel;
}

void H323Call::RemoveChannel(H323Channel*pH323Channel)
{
	m_MapH323Channel.erase(pH323Channel);
}

int H323Call::SetLocalCapabilitis( int nodeId )
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 

	if(m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount >0) m_nSupportH239=1;
	CopyVideoCapOtherParam(&m_ConfInfo,1,1);

	int nFound=-1; //productId: H3C MG6060 // SCOPIA_VC240 //test
	nFound=m_strProductID.find("SCOPIA_VC240");
	if(nFound >=0) m_CallCapability.SetMainVideoMaxBitrate(2000);
	else m_CallCapability.SetMainVideoMaxBitrate(m_nMainVideoMaxBitrate);
	m_CallCapability.SetSecondVideoMaxBitrate(m_nSecondVideoMaxBitrate);
	SetMCUConfCapFromProduct();  //根据第三方设备型号重新配置MCU能力集

#else 


	SetTerminalConfInfoCaps();
	ConfCapabilitisDefault();
#endif 




	BuildConfInfoCaps(nodeId);
	return 0;
}


void H323Call::OnCallRingIn(void)
{

	if(m_bRegCall ==false)
	{
		LogFileCall((H323MediaCall *)this,NULL, 0,"","","OnCallRingIn",1);

#if defined(TAIERLAB) && (TAIERLAB == 1)
		cmCallSendCallProceeding(m_hsCall);//add tai er ru wang test use 
#endif

		cmCallAccept(m_hsCall);
		GetMgrLocalIPByRemoteIP(m_strLocalIP,GetUserID(),H323CallMgr::Instance().m_nH323ListenPort);
		m_nCallDir=IMXEC_CALL_DIR_IN;
		m_strRemoteUserNonstandardInfo=m_strRecvUserNonstandardInfo;
		m_strTelNum="";
		char buf[288]={0};
		cmAlias alias;
		alias.string=buf;
		int error=cmCallGetParam(m_hsCall,cmParamDestinationAddress,0,0,(char*)&alias);
		if(error >= 0)
		{
			if( alias.type == cmAliasTypeE164 && alias.length >0 ) m_strTelNum=alias.string;
		}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 


		std::string strCallerE164 =m_strE164ID;
		if(m_strQ931CallerNumber.size() >0)
		{
			strCallerE164 =m_strQ931CallerNumber;
		}

		H323CallMgr::Instance().OnDAP_CallRingIn(GetCallID(),GetUserID(),GetUserFullName(),strCallerE164,m_strRemoteUserNonstandardInfo,m_strTelNum);

#else
		H323CallMgr::Instance().OnCallRingIn(GetCallID(),GetUserID(),GetUserFullName());
#endif
	}

}

void H323Call::OnCallRingOut(void)
{
	m_nCallDir=IMXEC_CALL_DIR_OUT;

	LogFileCall((H323MediaCall *)this,NULL, 0,"","","OnCallRingOut",1);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	H323CallMgr::Instance().OnDAP_CallRingOut(GetCallID(),GetUserID(),GetUserFullName());

#else
	H323CallMgr::Instance().OnCallRingOut(GetCallID(),GetUserID(),GetUserFullName());
#endif

}

const std::string H323Call::GetUserFullName(void)
{
	//cmCallGetOrigin(m_hsCall,NULL);
	if (m_strUserName.size()==0)
	{
		char	szName[1024]={0};
		int		nLen=0;
		if(cmCallGetDisplayInfo(m_hsCall,szName,sizeof(szName)) > 0)
		{
			szName[1023]=0;
			nLen=strlen(szName);
			for(int i=0; i<nLen; i++)
			{
				if(szName[i]<=0)
				{
					szName[i]=0;
					break;
				}
			}
		}
		nLen=strlen(szName);
		m_strUserName=szName;
	}
	return m_strUserName;
}

const std::string H323Call::GetUserID(void)
{
	int nPort=0;
	if(GetCallSignalAddress(m_strUserID,nPort) ) return m_strUserID;

	if (m_strUserID.size()==0)
	{
		HPROTCONN hConn = cmGetTpktChanHandle(m_hsCall,cmQ931TpktChannel); 
		if(!hConn)
		{
			return m_strUserID;
		}
		cmTransportAddress tr;
		if(cmProtocolGetRemoteAddress(H323CallMgr::Instance().m_hApp, hConn, &tr) < 0)
		{
			return m_strUserID;
		}

		if(tr.type != cmTransportTypeIP)
		{
			return m_strUserID;
		}
		struct in_addr		addr;
		addr.S_un.S_addr	=tr.ip;
		m_strUserID=inet_ntoa(addr);
	}

	return m_strUserID;
}

void H323Call::OnRcvH245CapabilitiesSet(void* capabilities)
{

}

void H323Call::SetAudioCapsParam(int nDirection,X_AUDIO_CODEC_ID nAudioCodecID,int entryNum,h235MediaCap& H235Cap)
{
	int nEncryption =0;
	unsigned int nAlgorithm =0;
	if( m_RecvCallCaps.m_nAudioCapCount <MAX_AUDIO_CAP2TS )
	{
		GetH235MediaCapInfo(H235Cap,entryNum,nEncryption,nAlgorithm);
		m_RecvCallCaps.m_aAudioCap[m_RecvCallCaps.m_nAudioCapCount].m_nDirection =nDirection;
		m_RecvCallCaps.m_aAudioCap[m_RecvCallCaps.m_nAudioCapCount].m_AudioCodecType=nAudioCodecID;
		m_RecvCallCaps.m_aAudioCap[m_RecvCallCaps.m_nAudioCapCount].m_nEncryption=nEncryption;
		m_RecvCallCaps.m_aAudioCap[m_RecvCallCaps.m_nAudioCapCount].m_nAlgorithm=nAlgorithm;
		m_RecvCallCaps.m_nAudioCapCount ++;
	}
}

void H323Call::SetVideoCapsParam(int nDirection,RecvVideoCap& item,int entryNum,h235MediaCap& H235Cap,int nDual) //nDual 0 main video ,1 dualvieo
{
	GetH235MediaCapInfo(H235Cap,entryNum,item.m_nEncryption,item.m_nAlgorithm);
	item.m_nDirection =nDirection;
	if(nDual==0 && m_RecvCallCaps.m_nMainVideoCapCount <MAX_VIDEO_CAP2TS )
	{
		memcpy( &(m_RecvCallCaps.m_aMainVideoCap[m_RecvCallCaps.m_nMainVideoCapCount]),&item,sizeof(item));
		m_RecvCallCaps.m_nMainVideoCapCount ++;
	}
	else if(nDual==1 && m_RecvCallCaps.m_nSecondVideoCapCount <MAX_VIDEO_CAP2TS )
	{
		memcpy( &(m_RecvCallCaps.m_aSecondVideoCap[m_RecvCallCaps.m_nSecondVideoCapCount]),&item,sizeof(item));
		m_RecvCallCaps.m_nSecondVideoCapCount ++;
	}
}

void H323Call::GetH235MediaCapInfo(h235MediaCap& H235Cap,int entryNum,int& nEncryption,unsigned int& nAlgorithm)
{
	unsigned int count=0;
	int j=0;
	int bit=0;
	int k=0;
	//find cap
	for(count=0;count<H235Cap.m_nCapNum;count++)
	{
		if(H235Cap.m_Caps[count].entryNum==entryNum)
		{
			for(j=0;j<32;j++)
			{
				bit=BitGet(H235Cap.m_Caps[count].algorithm,j);
				if(bit==1)
				{
					nEncryption=1;
					nAlgorithm =H235Cap.m_Caps[count].algorithm;
					break;
				}
			}
			break;
		}
	}
}

int H323Call::GetH264CapOptionParam(HPVT pvtTree,int nodeId,char * subBuf,int type)
{
	int nMaxFS=0;
	int nMBPS=0;
	return GetH264CapOptionParam(pvtTree,nodeId,nMaxFS,nMBPS,subBuf,type);
}

void H323Call::GetVideoAllCaps(HPVT pvtTree,h235MediaCap& remoteH235Cap,int nDirection,char * capAltName,int nCapAltNode,int nEntryNum,int nDual) //0 is main video ,1 is dual video
{
	RvPstFieldId	nField;
	RvInt32			nValue;
	RvBool			bString;
	VIDEC_CODEC_TYPE nVideoCodecType=VIDEC_CODEC_H264;
	int nMaxBitrate=1280;
	int nRet=0;
	int nRemoteQCIF=0;
	int nRemoteCIF=0;
	int nRemote4CIF=0;
	int bH263HighResolution=false;
	float nFps = VideoFrameBase;
	VIDEO_FORMAT_TYPE nFormat=VIDEO_FORMAT_QCIF ;
	if (strcmp(capAltName,"h261VideoCapability") == 0)
	{
		nRet=pvtGetByPath(pvtTree,nCapAltNode,"maxBitRate",&nField,&nValue,&bString);
		if(nRet>=0) nMaxBitrate =nValue /10;
		nRet=pvtGetByPath(pvtTree,nCapAltNode,"qcifMPI",&nField,&nValue,&bString);
		if(nRet>=0)
		{
			RecvVideoCap item;
			item.m_VideoCodecType=VIDEC_CODEC_H261;
			item.m_nVideoFrameRate= nFps ;
			item.m_VideoFormatType=VIDEO_FORMAT_QCIF;
			item.m_nVideoBitrate=nMaxBitrate;
			item.m_nFormatValue= GetVideoFormatValue(item.m_VideoFormatType);
			SetVideoCapsParam(nDirection,item,nEntryNum,remoteH235Cap,nDual);
		}

		nRet=pvtGetByPath(pvtTree,nCapAltNode,"cifMPI",&nField,&nValue,&bString);
		if(nRet>=0)
		{
			RecvVideoCap item;
			item.m_VideoCodecType=VIDEC_CODEC_H261;
			item.m_nVideoFrameRate= nFps ;
			item.m_VideoFormatType=VIDEO_FORMAT_CIF;
			item.m_nVideoBitrate=nMaxBitrate;
			item.m_nFormatValue= GetVideoFormatValue(item.m_VideoFormatType);
			SetVideoCapsParam(nDirection,item,nEntryNum,remoteH235Cap,nDual);
		}

	}
	else if (strcmp(capAltName, "h263VideoCapability") == 0)
	{
		CodecItemArr codecItems;
		codecItems.m_nCount=0;
		nRet=pvtGetByPath(pvtTree,nCapAltNode,"h263Options.h263Version3Options.dataPartitionedSlices", NULL, &nValue, NULL);
		if(nRet>=0)//对方具有H.263++能力
		{
			nVideoCodecType=VIDEC_CODEC_H263PP;
		}
		else
		{
			nRet=pvtGetByPath(pvtTree,nCapAltNode,"h263Options.advancedIntraCodingMode", NULL, &nValue, NULL);
			if(nRet>=0)//对方具有H.263+能力
			{
				nVideoCodecType=VIDEC_CODEC_H263P;
			}
			else
			{
				nVideoCodecType=VIDEC_CODEC_H263;
			}
		}

		nRet=pvtGetByPath(pvtTree,nCapAltNode,"maxBitRate",&nField,&nValue,&bString);
		if(nRet>=0) nMaxBitrate =nValue /10;

		nRet=pvtGetByPath(pvtTree,nCapAltNode,"qcifMPI",&nField,&nValue,&bString);
		if(nRet>=0)
		{
			codecItems.m_Items[codecItems.m_nCount].m_VideoCodecType =nVideoCodecType;
			codecItems.m_Items[codecItems.m_nCount].m_VideoFormatType =VIDEO_FORMAT_QCIF;
			codecItems.m_nCount ++;
		}
		nRet=pvtGetByPath(pvtTree,nCapAltNode,"cifMPI",&nField,&nValue,&bString);
		if(nRet>=0)
		{
			codecItems.m_Items[codecItems.m_nCount].m_VideoCodecType =nVideoCodecType;
			codecItems.m_Items[codecItems.m_nCount].m_VideoFormatType =VIDEO_FORMAT_CIF;
			codecItems.m_nCount ++;
		}

		nRet=pvtGetByPath(pvtTree,nCapAltNode,"cif4MPI",&nField,&nValue,&bString);
		if(nRet>=0)
		{
			codecItems.m_Items[codecItems.m_nCount].m_VideoCodecType =nVideoCodecType;
			codecItems.m_Items[codecItems.m_nCount].m_VideoFormatType =VIDEO_FORMAT_4CIF;
			codecItems.m_nCount ++;
		}

		//get maxCustomPictureWidth maxCustomPictureHeight
		int nCustomFormatCount=0;
		int nCustomFormatWidth=0;
		int nCustomFormatHeight=0;
		char formatStr[256]={0};
		while(1)
		{
			nCustomFormatCount++;
			sprintf(formatStr,"h263Options.customPictureFormat.%d.maxCustomPictureWidth",nCustomFormatCount);
			nRet=pvtGetByPath(pvtTree,nCapAltNode,formatStr,&nField,&nValue,&bString);
			if(nRet<0)
			{
				break;
			}
			else
			{
				if(nValue*4>nCustomFormatWidth)
				{
					int nHeightValue=0;
					sprintf(formatStr,"h263Options.customPictureFormat.%d.maxCustomPictureHeight",nCustomFormatCount);										
					nRet=pvtGetByPath(pvtTree,nCapAltNode,formatStr,&nField,&nHeightValue,&bString);
					if(nRet>=0)
					{
						nCustomFormatWidth=nValue*4;
						nCustomFormatHeight=nHeightValue*4;
						nFormat=GetVideoFormatType(nCustomFormatWidth,nCustomFormatHeight);
						codecItems.m_Items[codecItems.m_nCount].m_VideoCodecType =nVideoCodecType;
						codecItems.m_Items[codecItems.m_nCount].m_VideoFormatType =nFormat;
						codecItems.m_nCount ++;
					}
				}
			}
		}

		//add item
		for(int i=0;i<codecItems.m_nCount;i++)
		{
			RecvVideoCap item;
			item.m_VideoCodecType=codecItems.m_Items[i].m_VideoCodecType;
			item.m_VideoFormatType=codecItems.m_Items[i].m_VideoFormatType;
			item.m_nFormatValue= GetVideoFormatValue(item.m_VideoFormatType);
			item.m_nVideoFrameRate= nFps ;
			item.m_nVideoBitrate=nMaxBitrate;
			SetVideoCapsParam(nDirection,item,nEntryNum,remoteH235Cap,nDual);
		}

		RecvCapDapCapsSort(VIDEC_CODEC_H263);
		RecvCapDapCapsSort(VIDEC_CODEC_H263P);
		RecvCapDapCapsSort(VIDEC_CODEC_H263PP);

	}
	else if (strcmp(capAltName, "genericVideoCapability") == 0 )
	{
		int  lenData;
		RvBool  bIsString;
		int nodeData = pvtGetByPath( pvtTree, nCapAltNode, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
		if( nodeData && bIsString && lenData>0 )
		{
			unsigned char strData[256];
			if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
			{
				//0.0.8.241.0.0.1
				//0x00 0x08 0x81 0x71 0x00 0x00 0x01
				if( lenData == 7 &&	
					strData[0] == 0x00 && strData[1] == 0x08 && strData[2] == 0x81 && strData[3] == 0x71 &&
					strData[4] == 0x00 && strData[5] == 0x00 && strData[6] == 0x01 )
				{
					int nValue=0;
					int nRet=-1;
					int nProfile=0;
					int nLevel=0;
					int nMaxFS=0;
					int nMBPS=0;
					VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_QCIF;
					nRet=pvtGetByPath(pvtTree,nCapAltNode,"maxBitRate",&nField,&nValue,&bString);
					if(nRet>=0) nMaxBitrate = nValue /10;
					nRet=GetH264ProfileLevel(pvtTree,nCapAltNode,"" ,nProfile,nLevel);
					if(nRet>=0)
					{
						GetH264CapOptionParam(pvtTree,nCapAltNode,nMaxFS,nMBPS,"",nDual);
						nFormatType=GetCallFormatTypeFromH264Param(nProfile,nLevel,nMaxFS,nMBPS,nMaxBitrate);
						if(nProfile==64) nVideoCodecType=VIDEC_CODEC_H264;
						if(nProfile==8)  nVideoCodecType=VIDEC_CODEC_H264_SVC;
						//the support not has MaxFS value
						if(nMaxFS <=0 && nLevel>=57 && nLevel <=71 ) nFormatType =VIDEO_FORMAT_720P;
						if(nMaxFS <=0 && nLevel>=78 ) nFormatType =VIDEO_FORMAT_1080P;

						RecvVideoCap item;
						item.m_nVideoFrameRate= GetFrameRateFromH264Param(nMaxFS,nMBPS);
						item.m_nFormatValue= GetVideoFormatValue(nFormatType);
						item.m_VideoCodecType=nVideoCodecType;
						item.m_VideoFormatType= nFormatType;
						item.m_nLevelValue= nLevel;
						item.m_nProfileValue= nProfile;
						item.m_nVideoBitrate= nMaxBitrate;
						item.m_nMaxFS= nMaxFS;
						item.m_nMaxMBPS= nMBPS;												
						SetVideoCapsParam(nDirection,item,nEntryNum,remoteH235Cap,nDual);

					}
				}
			}
		}
	}

	//notice ,use to call kedacom MPEG4
	if (strcmp(capAltName,"nonStandard") == 0 )
	{
		int  lenData;
		RvBool  bIsString;
		int nodeData = pvtGetByPath( pvtTree, nCapAltNode, "data", NULL, &lenData, &bIsString );
		if( nodeData && bIsString && lenData>0 )
		{
			unsigned char strData[256]={0};
			if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
			{
				//....kedacom_js_China............
				char * pName="kedacom_js_China";
				char * pName1=(char *) strData+4;
				if( lenData == 32 && strncmp(pName1,pName,strlen(pName)) ==0 )
				{
					nVideoCodecType=VIDEC_CODEC_MPEG4;
					VIDEO_FORMAT_TYPE nFormatType =VIDEO_FORMAT_4CIF;
					int nMaxBitrate =768;
					RecvVideoCap item;
					item.m_nFormatValue= GetVideoFormatValue(nFormatType);
					item.m_VideoCodecType=nVideoCodecType;
					item.m_VideoFormatType= nFormatType;
					item.m_nVideoBitrate= nMaxBitrate;
					SetVideoCapsParam(nDirection,item,nEntryNum,remoteH235Cap,nDual);

				}
			}
		}
	}
}

bool H323Call::H235CapNegotiate(int aEncry,int bEncry,unsigned int remoteAlgo,EncDecAlgorithmID& algoID)
{
	bool bRet=false;
	int  nIndex=0;
	bool localFirst=true;
	int  times=2;
	int  algoExist=0;
	int  bit=0;
	if(aEncry >=bEncry && bEncry >0)
	{
		//find Negotiate algorithm
		while(times)
		{
			nIndex=0;
			for(int j=0;j<32;j++)
			{
				bit=BitGet(remoteAlgo,j);
				if(bit==1) 
				{
					algoID=(EncDecAlgorithmID)(j+1);	
					if(localFirst)
					{
						algoExist=GetMgrEncryptionCapability().IsFirstFromAlgorithm(algoID);
					}
					else 
					{
						algoExist=GetMgrEncryptionCapability().IsExistFromAlgorithm(algoID);
					}
					if(algoExist)
					{ nIndex=j+1; break;}
				}
			}
			//is no find second times
			if(nIndex==0)
			{
				localFirst=false;
			}
			else
			{
				bRet=true;
				break;
			}
			times--;
		}
	}
	if(bRet ==false) algoID =ALGORITHM_NONE;
	return bRet;
}

void H323Call::ConfCapabilitisDefault(void)
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

bool H323Call::CapabilitisAudioItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem)
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
		NegItem.m_bNegMedia      = true;
		bResult =true;
	}

	return bResult;
}

bool H323Call::CapabilitisDataItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem)
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
		NegItem.m_bNegMedia       = true;
		bResult =true;
	}

	return bResult;
}

bool H323Call::CapabilitisVideoItemNeg(void * capItem1,void * capItem2,int nLocal,bool bEncOption,NegMediaCap & NegItem) //nDual 0 is main video ,1 is dual video
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
			//frameBack  = aCap->m_nVideoFrameRate;
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
		NegItem.m_bNegMedia       = true;
		bResult =true;
	}

	return bResult;
}

int H323Call::CapabilitisNegotiate(int nLocalPriority,bool bEncryOption)
{
	bool bResult =false;
	int ii=0;
	int jj=0;
	NegMediaCap NegItem;
	NegItem.m_bNegMedia=false;

	if(nLocalPriority ==1 || nLocalPriority ==3)
	{
		if (m_bHasRemoteAudio ==false && m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount>0)
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
					m_bAudioHasRemoteEncryption = NegItem.m_bNegEncryption;
					m_AudioEncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
					m_bHasRemoteAudio           = NegItem.m_bNegMedia;
					m_nRemoteAudioCodecID       = (X_AUDIO_CODEC_ID)NegItem.m_CodecType;
					break;
				}
			}
		}

		if (m_bHasRemoteVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount>0)
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
					CapabilitisNegMainVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteH239Video ==false && m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount>0)
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
					CapabilitisNegDualVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteData ==false && m_ConfInfo.m_DAPCallCapability.m_nDataCapCount >0)
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
					m_bDataHasRemoteEncryption = NegItem.m_bNegEncryption;
					m_DataEncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
					m_bHasRemoteData           = NegItem.m_bNegMedia;
					break;
				}
			}
		}
	}
	else 
	{
		if (m_bHasRemoteAudio ==false && m_ConfInfo.m_DAPCallCapability.m_nAudioCapCount>0)
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
					m_bAudioHasRemoteEncryption = NegItem.m_bNegEncryption;
					m_AudioEncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
					m_bHasRemoteAudio           = NegItem.m_bNegMedia;
					m_nRemoteAudioCodecID       = (X_AUDIO_CODEC_ID)NegItem.m_CodecType;
					break;
				}
			}
		}

		if (m_bHasRemoteVideo ==false && m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount>0)
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
					CapabilitisNegMainVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteH239Video ==false && m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount>0)
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
					CapabilitisNegDualVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteData ==false && m_ConfInfo.m_DAPCallCapability.m_nDataCapCount >0)
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
					m_bDataHasRemoteEncryption = NegItem.m_bNegEncryption;
					m_DataEncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
					m_bHasRemoteData           = NegItem.m_bNegMedia;
					break;
				}
			}
		}
	}

	return 0;
}

void H323Call::SetMCUConfCapFromProduct(void)
{
	if (m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount >0)
	{
		int nFound=-1;
		nFound=m_strProductID.find("H3C MG6060");
		if( nFound >=0)
		{
			//Add H263
			int nFrame = VideoFrameBase;
			AddVideoConfInfoCaps(VIDEC_CODEC_H263,VIDEO_FORMAT_4CIF,nFrame,m_nCapDirectionDefault,768,0);
			m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =1;
		}
	}
}

void H323Call::AddVideoConfInfoCaps(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nFrame,int nDirection,int nBitrate,int nDual)
{
	ConfInfo * pConfInfo =&m_ConfInfo;
	if(m_nTCSUseTerminal ==1) pConfInfo =&m_TCSTerminalConfInfo;

	DapCallCapability & dapCallCap=pConfInfo->m_DAPCallCapability;
	if(nDual ==0)
	{
		for(int i=0;i<dapCallCap.m_nMainVideoCapCount;i++)
		{
			if(dapCallCap.m_aMainVideoCap[0].m_VideoCodecType == nCodec && dapCallCap.m_aMainVideoCap[0].m_VideoFormatType == nFormat ) return;
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
			if(dapCallCap.m_aSecondVideoCap[0].m_VideoCodecType == nCodec && dapCallCap.m_aMainVideoCap[0].m_VideoFormatType == nFormat ) return;
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

void H323Call::BuildConfInfoCaps(int nodeId)
{
	//mcu 级联测试用
	if( H323CallMgr::Instance().m_nTerminalCapsType ==1) TCSUseTerminalVideoConfCaps();

	ConfInfo * pConfInfo =&m_ConfInfo;
	if(m_nTCSUseTerminal ==1) pConfInfo =&m_TCSTerminalConfInfo;

	H323CallMgr & rCallMgr = H323CallMgr::Instance();
	DapCallCapability & dapCallCap=pConfInfo->m_DAPCallCapability;
	//H224
	bool bSupportFECC=true;

	//add h224 caps
	if(bSupportFECC)
	{
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_DataCodecID = DATA_TYPE_H224;;
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDirection = m_nCapDirectionDefault;
		dapCallCap.m_DataCap[dapCallCap.m_nDataCapCount].m_nDataBitrate = 48;
		dapCallCap.m_nDataCapCount ++;
	}

	//组建能力集信令
	GetMgrCapability().BuildConfCapabilitis(nodeId,* pConfInfo);
	//std::string strContent="211\n";
	//OutputPVTNodeContent(strContent,nodeId); //test
	//add h235media caps
	//0 不使用 1可选使用 2强制使用H235媒体流加密
	int nH235MediaValue=0;        
	nH235MediaValue=GetMgrH235MediaUse();

	if(nH235MediaValue>0)
	{
		int encSpecified=0;
		encSpecified=GetMgrH235MediaEncrypSpecified();
		if( (encSpecified==0) && (m_bShareSecret==false) ) nH235MediaValue=0;
	}
	if( (nH235MediaValue==1) || (nH235MediaValue==2) ) 
	{
		GetMgrEncryptionCapability().RemoveAllCapabilitis();
		if(GetMgrH235MediaAlogrithm() ==ALGORITHM_AES_CBC)
		{
			GetMgrEncryptionCapability().AddAES_cbcAlgorithm();
			GetMgrEncryptionCapability().SetFirstAES_cbcAlgorithm();
		}
		else if(GetMgrH235MediaAlogrithm() ==ALGORITHM_AES256_CBC)
		{
			GetMgrEncryptionCapability().AddAES256_cbcAlgorithm();
			GetMgrEncryptionCapability().SetFirstAES256_cbcAlgorithm();
		}
		else if(GetMgrH235MediaAlogrithm() ==ALGORITHM_DES_CBC)
		{
			GetMgrEncryptionCapability().AddDES_cbcAlgorithm();
			GetMgrEncryptionCapability().SetFirstDES_cbcAlgorithm();
		}
		else
		{
			GetMgrEncryptionCapability().AddAES_cbcAlgorithm();
			GetMgrEncryptionCapability().SetFirstAES_cbcAlgorithm();
		}

		CallCapability	&callCap=GetMgrCapability();
		GetMgrEncryptionCapability().BuildAllMediaEncryptionCapabilitis(nodeId,callCap); 
	}

	int nCapPrint=rCallMgr.GetDAPCapPrintEnable();
	if(nCapPrint>0)
	{
		std::string strContent="";
		OutputPVTNodeContent(strContent ,nodeId);
		H323CallMgr::Instance().OutPutCapContent(m_strCallID.c_str(),1,strContent.c_str());
	}

}

void H323Call::AddVideoConfInfoBaseCaps2(VIDEC_CODEC_TYPE nCodec,VIDEO_FORMAT_TYPE nFormat,int nVideoFrameRate,int nDirection,int nVideoBitrate,int nDual)
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

	if ((nCodec == VIDEC_CODEC_H264 || nCodec ==VIDEC_CODEC_H264_SVC) && m_nH225Version <4) bSet1=false;
	int nValue1=GetVideoFormatValue(nFormat);
	int nValue2=GetVideoFormatValue(VIDEO_FORMAT_CIF);
	if(nValue1 >nValue2 )
	{
		// >4cif check 
		if(m_bSecondSendCap || m_nRemoteTerminalType==1) bSet2=true;
	}
	else
	{
		//cif 
		bSet2=true;
	}
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
	//add H264
	if (m_nH225Version>=4)
	{
		if(m_bSecondSendCap || (m_nRemoteTerminalType==1) )
		{
			if(nResStand >=res720P)
			{
				nCodecType=VIDEC_CODEC_H264;
				nFT=VIDEO_FORMAT_720P;
				AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
				nCodecType=VIDEC_CODEC_H264;
				nFT=VIDEO_FORMAT_SXGA;
				AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
			}
			if(nResStand >=res1080P)
			{
				nCodecType=VIDEC_CODEC_H264;
				nFT=VIDEO_FORMAT_1080P;
				AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
			}
			//add h264 highProfile
			if(nResStand >=res720P)
			{
				nCodecType=VIDEC_CODEC_H264_SVC;
				nFT=VIDEO_FORMAT_720P;
				AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
			}
			if(nResStand >=res1080P)
			{
				nCodecType=VIDEC_CODEC_H264_SVC;
				nFT=VIDEO_FORMAT_1080P;
				AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
			}

			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_4CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

			nCodecType=VIDEC_CODEC_H264;
			nFT=VIDEO_FORMAT_CIF;
			AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

		}

	}

	//Add H263v2
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

	//Add H263
	nCodecType=VIDEC_CODEC_H263;
	nFT=VIDEO_FORMAT_4CIF;
	AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

	//Add H263v3 选择了则会添加
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

	//add tai er ru wang test use
#if defined(TAIERLAB) && (TAIERLAB == 1)

	{
		nCodecType=VIDEC_CODEC_H263P;
		nFT=VIDEO_FORMAT_4CIF;
		AddVideoConfInfoCaps(nCodecType,nFT,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
	} 
#endif

	//use to call kedacom 选择了则会添加
	if(nCodecType == VIDEC_CODEC_MPEG4)
	{
		nCodecType=VIDEC_CODEC_MPEG4;
		nFT=VIDEO_FORMAT_4CIF;
		AddVideoConfInfoCaps(nCodecType,nFT,VideoFrameBase,nDirection,768,nDual);
	}

}



void H323Call::AddConfCapFromConfig()
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
		m_nForceVideoWid=nForceVideoWid;
		m_nForceVideoHei=nForceVideoHei;
	}

	nForceVideoWid=0;
	nForceVideoHei=0;
	nForceVideoWid=GetPrivateProfileInt("CapMedia","ForceDualVideoWid",0,szPath);
	nForceVideoHei=GetPrivateProfileInt("CapMedia","ForceDualVideoHgh",0,szPath);
	if(nForceVideoWid >0 && nForceVideoHei >0)
	{
		m_nForceDualVideoWid=nForceVideoWid;
		m_nForceDualVideoHei=nForceVideoHei;
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
			nVideoFrameRate = H323CallMgr::Instance().GetMainVideoDevFrameRate();
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
			nVideoFrameRate = H323CallMgr::Instance().GetSecondVideoDevFrameRate();
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



	//X_AUDIO_CODEC_ID nAudioID=H323CallMgr::Instance().GetAudioCodecID();
	//nFirstType=GetPrivateProfileInt("CapMedia","AudioFirst",-1,szPath);
	//if(nFirstType >= 0) nAudioID=(X_AUDIO_CODEC_ID) nFirstType;


	//video

	//VIDEC_CODEC_TYPE nVideoID=H323CallMgr::Instance().GetMainVideoCodecType();
	//nFirstType=GetPrivateProfileInt("CapMedia","VideoFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoID=(VIDEC_CODEC_TYPE) nFirstType;

	//VIDEO_FORMAT_TYPE nVideoFormat= GetMgrMainVideoFormatType();
	//nFirstType=GetPrivateProfileInt("CapMedia","VideoFormatFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoFormat=(VIDEO_FORMAT_TYPE) nFirstType;

	//nVideoID=H323CallMgr::Instance().GetSecondVideoCodecType();
	//nFirstType=GetPrivateProfileInt("CapMedia","DualVideoFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoID=(VIDEC_CODEC_TYPE) nFirstType;

	//nVideoFormat= GetMgrSecondVideoFormatType();
	//nFirstType=GetPrivateProfileInt("CapMedia","DualVideoFormatFirst",-1,szPath);
	//if(nFirstType >= 0) nVideoFormat=(VIDEO_FORMAT_TYPE) nFirstType;


#endif

}

void H323Call::SetTerminalConfInfoCaps(void)
{
	int nFound=-1;
	H323CallMgr & rCallMgr = H323CallMgr::Instance();
	DapCallCapability & dapCallCap=m_ConfInfo.m_DAPCallCapability;
	int nDirection =m_nCapDirectionDefault;

	memset(&dapCallCap,0,sizeof(DapCallCapability));
	//dapCallCap.m_nAudioCapCount =0;
	//dapCallCap.m_nMainVideoCapCount =0;
	//dapCallCap.m_nSecondVideoCapCount =0;
	//dapCallCap.m_nDataCapCount =0;

	m_ConfInfo.m_nH235MediaUse = rCallMgr.m_nH235MediaUse;
	m_ConfInfo.m_nUseAlogrithmId = rCallMgr.m_nUseAlogrithmId;

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
	nFound=m_strProductID.find("VSX 5000");
	if(m_nH225Version <4 || nFound >=0)
	{
		bG722124=false; //不能呼叫netmeeting
		bG722132=false;
		bG7221c24=false;
		bG7221c32=false;
		bG7221c48=false;
		bG71932=false;
		bG71948=false;
		bG71964=false;
	}
	nFound=m_strProductID.find("LifeSize");
	if (nFound >=0)
	{
		bG72264=false;
	}

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
	videoCodecPri =rCallMgr.GetMainVideoCodecType();
	videoFormatPri =rCallMgr.GetMainVideoFormatType();
	nVideoBitrate =rCallMgr.GetMainVideoMaxBitrate();
	nVideoFrameRate = rCallMgr.GetMainVideoDevFrameRate();
	nDual=0;
	AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

	//add dual video caps
	if(rCallMgr.GetSupportH239() && m_nH225Version >=4)
	{
		videoCodecPri =rCallMgr.GetSecondVideoCodecType();
		videoFormatPri =rCallMgr.GetSecondVideoFormatType();
		nVideoBitrate =rCallMgr.GetSecondVideoMaxBitrate();
		nVideoFrameRate = rCallMgr.GetSecondVideoDevFrameRate();
		nDual=1;
		AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
	}

	//notice test , if config,only use the config cap add
	AddConfCapFromConfig();

}



void H323Call::OnRcvH245CapabilitiesTermUse(void* capabilities)
{
	//接收到对方发过来的空能力集 重设能力集再次发送 (一般是作MCU会议登录切换)
	if (capabilities==NULL)
	{
		m_bSecondSendCap=true;            //true 修改能力集重发 false 不发
		BuildSendLocalCapabilitis();
		return ;
	}

	//取H235能力项参数
	h235MediaCap remoteH235Cap;
	memset(&remoteH235Cap,0,sizeof(h235MediaCap));
	GetH235MediaCapSet(&remoteH235Cap,capabilities);

	std::string strCapContent="";
	int nCapPrint=H323CallMgr::Instance().GetDAPCapPrintEnable();

	if(m_pAudioSendChannel)
	{
		memset(&m_RecvCallCaps,0,sizeof(m_RecvCallCaps) );
	}

	cmCapStruct*** pRemoteCapDes = NULL;  // 能力描述表指针
	cmCapStruct** pRemoteCapSim = NULL;   // 同时能力集指针
	cmCapStruct* pRemoteCapAlt = NULL;    // 可选能力指针
	cmCapStruct**** pRemoteCapStruct = (cmCapStruct****)capabilities;
	RvPstFieldId	nField;
	RvInt32			nValue;
	RvBool			bString;
	int             error;

	HPVT pvtTree = cmGetValTree(H323CallMgr::Instance().m_hApp);
	int iDescriptorRemote = 0, iSimultaneousRemote = 0, iAlternativeRemote = 0; // 对端接收能力三级索引
	while( pRemoteCapDes = pRemoteCapStruct[iDescriptorRemote] ) // 对每个描述表扫描
	{
		iSimultaneousRemote = 0; // 从第一个同时能力集开始
		while( pRemoteCapSim = pRemoteCapDes[iSimultaneousRemote] ) // 对同时接收能力集扫描
		{
			iAlternativeRemote = 0; // 从第一个可选接收能力集开始
			while( pRemoteCapAlt = pRemoteCapSim[iAlternativeRemote] ) // 对可选接收能力扫描
			{
				if(nCapPrint>0)
				{
					OutputPVTNodeContent(strCapContent,pRemoteCapAlt->capabilityHandle,pvtTree);
				}
				if (pRemoteCapAlt->direction == cmCapReceive || pRemoteCapAlt->direction == cmCapReceiveAndTransmit)
				{
					switch(pRemoteCapAlt->type)
					{
					case cmCapAudio:
						{
							X_AUDIO_CODEC_ID nAudioCodecID=X_AUDIO_CODEC_G711U;
							if (strcmp(pRemoteCapAlt->name,"g711Ulaw64k") == 0 )
							{
								nAudioCodecID=X_AUDIO_CODEC_G711U;
								SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
							}
							else if (strcmp(pRemoteCapAlt->name, "g711Alaw64k") == 0 )
							{
								nAudioCodecID=X_AUDIO_CODEC_G711A;
								SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
							}
							else if (strcmp(pRemoteCapAlt->name, "g722-64k") == 0 )
							{
								nAudioCodecID=X_AUDIO_CODEC_G722_64;
								SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
							}
							else if(strcmp(pRemoteCapAlt->name, "genericAudioCapability") == 0 )
							{
								if (1)
								{
									int  lenData;
									RvBool  bIsString;
									int nodeData = pvtGetByPath( pvtTree, pRemoteCapAlt->capabilityHandle, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
									if( nodeData && bIsString && lenData>0 )
									{
										unsigned char strData[256];
										if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
										{
											//0.0.7.7221.1.0
											//0x00 0x07 0xb8 0x35 0x01 0x00
											if( lenData == 6 &&
												strData[0] == 0x00 && strData[1] == 0x07 && strData[2] == 0xb8 && 
												strData[3] == 0x35 && strData[4] == 0x01 && strData[5] == 0x00 )
											{
												int nValue=0;
												int nRet=0;
												nRet=pvtGetByPath(pvtTree,pRemoteCapAlt->capabilityHandle,"maxBitRate",&nField,&nValue,&bString);
												if(nRet>=0 && nValue==24000 )
												{
													nAudioCodecID=X_AUDIO_CODEC_G7221_7_24;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
												if(nRet>=0 && nValue==32000 )
												{
													nAudioCodecID=X_AUDIO_CODEC_G7221_7_32;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
											}
										}
									}									
								}
								if (1)
								{
									int  lenData;
									RvBool  bIsString;
									int nodeData = pvtGetByPath( pvtTree, pRemoteCapAlt->capabilityHandle, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
									if( nodeData && bIsString && lenData>0 )
									{
										unsigned char strData[256];
										if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
										{
											//0.0.7.7221.1.1.0
											//0x00 0x07 0xb8 0x35 0x01 0x01 0x00
											if( lenData == 7 &&
												strData[0] == 0x00 && strData[1] == 0x07 && strData[2] == 0xb8 && 
												strData[3] == 0x35 && strData[4] == 0x01 &&	strData[5] == 0x01 && 
												strData[6] == 0x00 )
											{
												int nValue=0;
												int nRet=0;
												nRet=pvtGetByPath(pvtTree,pRemoteCapAlt->capabilityHandle,"maxBitRate",&nField,&nValue,&bString);
												if(nRet>=0 && nValue==240 )
												{
													nAudioCodecID=X_AUDIO_CODEC_G7221_14_24;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
												if(nRet>=0 && nValue==320 )
												{
													nAudioCodecID=X_AUDIO_CODEC_G7221_14_32;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
												if(nRet>=0 && nValue==480 )
												{											
													nAudioCodecID=X_AUDIO_CODEC_G7221_14_48;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}

											}

										}
									}
								}
								if (1)
								{
									int  lenData;
									RvBool  bIsString;
									int nodeData = pvtGetByPath( pvtTree, pRemoteCapAlt->capabilityHandle, "capabilityIdentifier.standard", NULL, &lenData, &bIsString );
									if( nodeData && bIsString && lenData>0 )
									{
										unsigned char strData[256];
										if( pvtGetString( pvtTree, nodeData, lenData, (char*)strData ) > 0)
										{
											//notice 20111227 update
											//G719 OID
											//00 07 85 4F 01 00						
											//0x00 0x07 0x85 0x4F 0x01 0x00
											if( lenData == 6 &&
												strData[0] == 0x00 && strData[1] == 0x07 && strData[2] == 0x85 && 
												strData[3] == 0x4F && strData[4] == 0x01 && strData[5] == 0x00 )
											{
												int nValue=0;
												int nRet=0;
												nRet=pvtGetByPath(pvtTree,pRemoteCapAlt->capabilityHandle,"maxBitRate",&nField,&nValue,&bString);
												if(nRet>=0 && nValue==320 )
												{
													nAudioCodecID=X_AUDIO_CODEC_G719_32;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
												if(nRet>=0 && nValue==480)
												{
													nAudioCodecID=X_AUDIO_CODEC_G719_48;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}
												if(nRet>=0 && nValue==640 )
												{											
													nAudioCodecID=X_AUDIO_CODEC_G719_64;
													SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
												}

											}

										}
									}
								}
							}
							else if (strcmp(pRemoteCapAlt->name, "g729AnnexA") == 0 )
							{
								m_nRemoteG729Type=2;
								nAudioCodecID=X_AUDIO_CODEC_G729;
								SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
							}

							else if (strcmp(pRemoteCapAlt->name, "g729") == 0 )
							{
								m_nRemoteG729Type=1;
								nAudioCodecID=X_AUDIO_CODEC_G729;
								SetAudioCapsParam(pRemoteCapAlt->direction,nAudioCodecID,pRemoteCapAlt->capabilityId,remoteH235Cap);
							}
						}
						break;
					case cmCapVideo:
						{
							if (strcmp(pRemoteCapAlt->name, "extendedVideoCapability") == 0) //双流
							{
								char capAltName[128]={0};
								int nSubNode1 =-1;
								int nSubNode2 =-1;
								int nSubNode3 =-1;
								//get dual vieo caps
								nSubNode1 =pvtChild(pvtTree,pRemoteCapAlt->capabilityHandle);
								if(nSubNode1 >=0) nSubNode2 =pvtChild(pvtTree,nSubNode1);
								if(nSubNode2 >=0)
								{
									nSubNode3=pvtGetNodeIdByPath(pvtTree,nSubNode2,"h261VideoCapability");
									if(nSubNode3 >=0)
									{
										strcpy(capAltName,"h261VideoCapability");
										GetVideoAllCaps(pvtTree,remoteH235Cap,pRemoteCapAlt->direction,capAltName,nSubNode3,pRemoteCapAlt->capabilityId,1);
										break;
									}
									nSubNode3=pvtGetNodeIdByPath(pvtTree,nSubNode2,"h263VideoCapability");
									if(nSubNode3 >=0)
									{
										strcpy(capAltName,"h263VideoCapability");
										GetVideoAllCaps(pvtTree,remoteH235Cap,pRemoteCapAlt->direction,capAltName,nSubNode3,pRemoteCapAlt->capabilityId,1);
										break;
									}
									nSubNode3=pvtGetNodeIdByPath(pvtTree,nSubNode2,"genericVideoCapability");
									if(nSubNode3 >=0)
									{
										strcpy(capAltName,"genericVideoCapability");
										GetVideoAllCaps(pvtTree,remoteH235Cap,pRemoteCapAlt->direction,capAltName,nSubNode3,pRemoteCapAlt->capabilityId,1);
										break;
									}
									nSubNode3=pvtGetNodeIdByPath(pvtTree,nSubNode2,"nonStandard"); //notice ,use to call kedacom MPEG4
									if(nSubNode3 >=0)
									{
										strcpy(capAltName,"nonStandard");
										GetVideoAllCaps(pvtTree,remoteH235Cap,pRemoteCapAlt->direction,capAltName,nSubNode3,pRemoteCapAlt->capabilityId,1);
										break;
									}
								}
							}
							else
							{
								//get main vieo caps
								GetVideoAllCaps(pvtTree,remoteH235Cap,pRemoteCapAlt->direction,pRemoteCapAlt->name,pRemoteCapAlt->capabilityHandle,pRemoteCapAlt->capabilityId,0);
							}
						}
						break;
					case cmCapData:
						{
							if (strcmp(pRemoteCapAlt->name,"h224") == 0 )
							{
								int nRet=-1;
								int nMaxBitrate=0;
								nRet=pvtGetByPath(pvtTree,pRemoteCapAlt->capabilityHandle,"maxBitRate",&nField,&nValue,&bString);
								if(nRet>=0) nMaxBitrate=nValue /10;
								int nEncryption =0;
								unsigned int nAlgorithm =0;
								GetH235MediaCapInfo(remoteH235Cap,pRemoteCapAlt->capabilityId,nEncryption,nAlgorithm);
								m_RecvCallCaps.m_DataCap[m_RecvCallCaps.m_nDataCapCount].m_nDirection =pRemoteCapAlt->direction;
								m_RecvCallCaps.m_DataCap[m_RecvCallCaps.m_nDataCapCount].m_nDataBitrate =nMaxBitrate;
								m_RecvCallCaps.m_DataCap[m_RecvCallCaps.m_nDataCapCount].m_DataCodecID=DATA_TYPE_H224;
								m_RecvCallCaps.m_DataCap[m_RecvCallCaps.m_nDataCapCount].m_nEncryption=nEncryption;
								m_RecvCallCaps.m_DataCap[m_RecvCallCaps.m_nDataCapCount].m_nAlgorithm=nAlgorithm;
								m_RecvCallCaps.m_nDataCapCount ++;
							}
						}
						break;
					}
				}

				iAlternativeRemote++;
			}

			iSimultaneousRemote++;
		}

		iDescriptorRemote++;
	}

	if(nCapPrint>0) H323CallMgr::Instance().OutPutCapContent(m_strCallID.c_str(),0,strCapContent.c_str());

	LogRecvCapDapCaps();

	m_bHasRemoteAudio=false;
	m_bHasRemoteVideo=false;
	m_bHasRemoteH239Video=false;
	m_bHasRemoteData=false;

	//能力集协商 ?? 

	bool bEncry =false;

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 2) || (HAS_H323_AVCON_MP == 1) ) 

	if(H323CallMgr::Instance().m_nTerminalCapsType ==1)
	{
		CapabilitisNegotiate(1,bEncry);
		if(m_bHasRemoteVideo ==false) CapabilitisNegotiate(1,bEncry);
		if(m_bHasRemoteVideo ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,3,bEncry);
		if(m_bHasRemoteH239Video ==false) CapabilitisNegotiate(1,bEncry); 
		if(m_bHasRemoteH239Video ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,3,bEncry);
	}
	else
	{
		CapabilitisNegotiate(4,bEncry);
		if(m_bHasRemoteVideo ==false) CapabilitisNegotiate(3,bEncry);
		if(m_bHasRemoteVideo ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,3,bEncry);
		if(m_bHasRemoteH239Video ==false) CapabilitisNegotiate(3,bEncry); 
		if(m_bHasRemoteH239Video ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,3,bEncry);
	}

#else 
	CapabilitisNegotiate(1,bEncry);
	if(m_bHasRemoteVideo ==false) CapabilitisNegotiate(2,bEncry);
	if(m_bHasRemoteVideo ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,1,bEncry);
	if(m_bHasRemoteH239Video ==false) CapabilitisNegotiate(2,bEncry);
	if(m_bHasRemoteH239Video ==false) CapabilitisProvisionalNeg(VIDEC_CODEC_H263P ,VIDEC_CODEC_H263PP,1,bEncry); 
#endif

	if(m_pAudioSendChannel)
	{
		m_nRemoteCapChaneged ++;
	}

	//发送能力集应答/拒绝
	if( m_bHasRemoteAudio || m_bHasRemoteVideo)      //音视频有一个匹配即可
	{
		error = cmCallCapabilitiesAck(m_hsCall);
	}
	else
	{
		error = cmCallCapabilitiesReject(m_hsCall, cmCapRejectCauseUnspecified, 0);
		if (m_hsCall!=NULL)
		{
			cmCallDrop(m_hsCall);
			m_hsCall=NULL;
		}
	}

}




void H323Call::OnRcvH245OpenChannel(HANDLE hsChannel, HANDLE* lphaChannel)
{
	//打开接收信道
	H323Channel * pLC = NewChannel( (HCHAN)hsChannel );
	if( !pLC ){
		cmChannelDrop( (HCHAN)hsChannel );
	}
	else{
		pLC->m_nDirection = cmCapReceive;
		*lphaChannel = (HANDLE)pLC;
	}
}

//远遥
void H323Call::OnRemoteCameraLeft()//远遥向左
{
	if (m_nLastRecvdPTZAction==PTZ_START_TILT_LEFT)
		return;

	m_nLastRecvdPTZAction=PTZ_START_TILT_LEFT;
	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_TILT_LEFT,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopLeft()
{
	m_nLastRecvdPTZAction=PTZ_STOP_TILT_LEFT;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_TILT_LEFT,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraRight()//远遥向右
{
	if (m_nLastRecvdPTZAction==PTZ_START_TILT_RIGHT)
		return;
	m_nLastRecvdPTZAction=PTZ_START_TILT_RIGHT;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_TILT_RIGHT,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopRight()
{
	m_nLastRecvdPTZAction=PTZ_STOP_TILT_RIGHT;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_TILT_RIGHT,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraUp()//远遥向上
{
	if (m_nLastRecvdPTZAction==PTZ_START_PAN_UP)
		return;
	m_nLastRecvdPTZAction=PTZ_START_PAN_UP;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_PAN_UP,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopUp()
{
	m_nLastRecvdPTZAction=PTZ_STOP_PAN_UP;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_PAN_UP,m_nPTZSpeed);
	//OutputDebugString("OnRemoteCameraStopUp\n");
}

void H323Call::OnRemoteCameraDown()//远遥向下
{
	if (m_nLastRecvdPTZAction==PTZ_START_PAN_DOWN)
		return;
	m_nLastRecvdPTZAction=PTZ_START_PAN_DOWN;
	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_PAN_DOWN,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopDown()
{
	m_nLastRecvdPTZAction=PTZ_STOP_PAN_DOWN;
	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_PAN_DOWN,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraZoomIn()//远遥拉近（放大）
{
	if (m_nLastRecvdPTZAction==PTZ_START_ZOOM_IN)
		return;
	m_nLastRecvdPTZAction=PTZ_START_ZOOM_IN;
	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_ZOOM_IN,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopZoomIn()
{
	m_nLastRecvdPTZAction=PTZ_STOP_ZOOM_IN;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_ZOOM_IN,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraZoomOut()//远遥拉远（缩小）
{
	if (m_nLastRecvdPTZAction==PTZ_START_ZOOM_OUT)
		return;
	m_nLastRecvdPTZAction=PTZ_START_ZOOM_OUT;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_START_ZOOM_OUT,m_nPTZSpeed);
}

void H323Call::OnRemoteCameraStopZoomOut()
{
	m_nLastRecvdPTZAction=PTZ_STOP_ZOOM_OUT;

	OnCallMgrRecvdPTZ(m_strCallID.c_str(),PTZ_STOP_ZOOM_OUT,m_nPTZSpeed);
}

void H323Call::OnRemoteSourceSelect(int vsID)//远遥选择视频源
{
}

void H323Call::OnRemoteSourceSwitched(int vsID)//远遥通知它的视频源切换了
{
}

void H323Call::OnRemoteCameraPresetStore(int position)//远遥摄像头预置设置
{
}

void H323Call::OnRemoteCameraPresetActive(int position)//远遥摄像头预置激活
{
}

void H323Call::RemoteSourceSelect(int vsID)
{
	if(m_pH224SendChannel)
	{
		m_pH224SendChannel->RemoteSourceSelect(vsID);
	}
}

void H323Call::RemoteSourceSwitched(int vsID)
{
	if(m_pH224SendChannel)
	{
		m_pH224SendChannel->RemoteSourceSwitched(vsID);
	}
}

void H323Call::RemoteCameraPresetStore(int position)
{
	if(m_pH224SendChannel)
	{
		m_pH224SendChannel->RemoteCameraPresetStore(position);
	}
}

void H323Call::RemoteCameraPresetActive(int position)
{
	if(m_pH224SendChannel)
	{
		m_pH224SendChannel->RemoteCameraPresetActive(position);
	}
}

//媒体发送
int H323Call::SendMainVideo(char* RTPPacketData, int PacketDataLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else

	if (HasAVCONTransport()) return 0;

#endif

	int nRet = -1;
	//XAutoLock l(m_csMainVideoSendChannel);
	if(m_pMainVideoSendChannel)
	{
		if(m_nMainVideoSendState >0)
			LogFileMediaState((H323MediaCall *)this,(void *)m_pMainVideoSendChannel, m_nMainVideoSendState,"","","SendMainVideo");
		m_nMainVideoSendState=0;
		//H235 Media use
		////ConvertFromNetwork(RTPPacketData, 0, 3);
		m_pMainVideoSendChannel->SendData(RTPPacketData, PacketDataLen);
		nRet = 0;
	}
	return nRet;
}

int H323Call::SendAudio(char* RTPPacketData, int PacketDataLen)
{
	if (HasAVCONTransport())
		return 0;

	int nRet = -1;
	if(m_pAudioSendChannel)
	{
		if(m_nAudioSendState >0)
			LogFileMediaState((H323MediaCall *)this,(void *)m_pAudioSendChannel, m_nAudioSendState,"","","SendAudio");
		m_nAudioSendState=0;

		PRTP_HEADER pRTPHeader=(PRTP_HEADER)RTPPacketData;
		m_nAudioSSRC=pRTPHeader->ssrc;
		m_nAudioSeqNo=pRTPHeader->sequence;
		m_nAudioTimestamp=pRTPHeader->timestamp;
		//Added by jiangdingfeng 20140929 根据需要调用生成SSRC接口
		if (m_bFlag)
		{
			m_pAudioSendChannel->GenerateSSRC();
			m_bFlag=false;
		}
		//H235 Media use
		////ConvertFromNetwork(RTPPacketData, 0, 3);
		m_pAudioSendChannel->SendData(RTPPacketData, PacketDataLen);
		nRet = 0;
	}
	return nRet;
}

typedef struct tagX_DTMF_2833 
{
	unsigned int event:8;
	unsigned int volume:6;
	unsigned int r:1;
	unsigned int e:1;
	unsigned int duration:16;
}X_DTMF_2833,*PX_DTMF_2833;


//发送DTMF
int H323Call::SendDTMF(const char dtmf,int nType)
{
	//SendCallUserInfomation("TestMessage");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

	if(m_nSendDTMFRTP >0 ) nType=0;
#endif

	if (nType)
	{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		return 0;
#else//
		AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);
#endif

	}
	else
	{
		int nDuration=960;
		int nPayloadType=101;
		unsigned long nAudioSSRC=m_nAudioSSRC;
		unsigned short nAudioSeqNo=ntohs(m_nAudioSeqNo);
		unsigned long nAudioTimestamp=htonl((ntohl(m_nAudioTimestamp)+160));

		int nDurationCount=nDuration/160;
		if (nDurationCount<2)
			nDurationCount=2;
		nDuration=0;

		unsigned char pRTPPacketData[128];
		memset(pRTPPacketData,0,RTP_HEADER_LEN+sizeof(X_DTMF_2833));
		PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPPacketData;
		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(++nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=1;

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

		char*pPacketData=(char*)pRTPPacketData;
		int nPacketDataLen=RTP_HEADER_LEN+sizeof(X_DTMF_2833);
		PX_DTMF_2833 pXDTMF2833=(PX_DTMF_2833)(pRTPPacketData+RTP_HEADER_LEN);

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		nDuration+=160;
		pXDTMF2833->duration=htons(nDuration);

		if (m_pAudioSendChannel!=NULL)
		{
			////ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}

		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=1;

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		pXDTMF2833->duration=htons(nDuration);

		if (m_pAudioSendChannel!=NULL)
		{
			//ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}

		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=1;

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		pXDTMF2833->duration=htons(nDuration);

		if (m_pAudioSendChannel!=NULL)
		{
			//ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}

		int i=0;
		for (i=1;i<(nDurationCount-1);i++)
		{
			pXDTMF2833->event=event;
			pXDTMF2833->volume=10;
			pXDTMF2833->r=0;
			pXDTMF2833->e=0;
			nDuration+=160;
			pXDTMF2833->duration=htons(nDuration);

			pRTPHeader->version=RTP_VERSION;
			pRTPHeader->type=nPayloadType;
			pRTPHeader->ssrc=m_nAudioSSRC;
			pRTPHeader->sequence=htons(++nAudioSeqNo);
			pRTPHeader->timestamp=nAudioTimestamp;
			pRTPHeader->marker=0;

			if (m_pAudioSendChannel!=NULL)
			{
				//ConvertFromNetwork(pPacketData, 0, 3);
				m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
			}
		}

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		pXDTMF2833->e=1;
		nDuration+=160;
		pXDTMF2833->duration=htons(nDuration);

		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(++nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=0;

		if (m_pAudioSendChannel!=NULL)
		{
			//ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		pXDTMF2833->e=1;
		pXDTMF2833->duration=htons(nDuration);

		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=0;

		if (m_pAudioSendChannel!=NULL)
		{
			//ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}

		pXDTMF2833->event=event;
		pXDTMF2833->volume=10;
		pXDTMF2833->r=0;
		pXDTMF2833->e=0;

		pXDTMF2833->e=1;
		pXDTMF2833->duration=htons(nDuration);

		pRTPHeader->version=RTP_VERSION;
		pRTPHeader->type=nPayloadType;
		pRTPHeader->ssrc=nAudioSSRC;
		pRTPHeader->sequence=htons(nAudioSeqNo);
		pRTPHeader->timestamp=nAudioTimestamp;
		pRTPHeader->marker=0;

		if (m_pAudioSendChannel!=NULL)
		{
			//ConvertFromNetwork(pPacketData, 0, 3);
			m_pAudioSendChannel->SendData(pPacketData, nPacketDataLen);
		}
	}

	return 0;
}

int H323Call::SendSecondVideo(char* RTPPacketData, int PacketDataLen)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#else

	if (HasAVCONTransport()) return 0;

#endif

	int nRet = -1;
	if(m_pSecondVideoSendChannel)
	{
		if(m_nSecondVideoSendState >0)
			LogFileMediaState((H323MediaCall *)this,(void *)m_pSecondVideoSendChannel, m_nSecondVideoSendState,"","","SendSecondVideo");
		m_nSecondVideoSendState=0;
		//H235 Media use
		//ConvertFromNetwork(RTPPacketData, 0, 3);
		m_pSecondVideoSendChannel->SendData(RTPPacketData, PacketDataLen);
		nRet = 0;
	}
	return nRet;
}

int H323Call::RequestMainVideoKeyFrame(void)
{
	int nRet = -1;
	if(m_pMainVideoRecvChannel)
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

		unsigned long nTimestamp=XGetTimestamp();
		if (nTimestamp - m_nMainVideoLastReqKeyFrameTimestamp >= MAX_KEY_FRAME_INTERVAL * 1000)
		{
			IPCLIB_LogL2("[H323Call::RequestMainVideoKeyFrame] Request main key frame.\n");
			nRet = m_pMainVideoRecvChannel->RequestKeyFrame();
			m_nMainVideoLastReqKeyFrameTimestamp = nTimestamp;
			m_bMainVideoReqKeyFrame=false;
		}
		else
		{
			m_bMainVideoReqKeyFrame=true;
		}

#else
		nRet = m_pMainVideoRecvChannel->RequestKeyFrame();
#endif

	}
	return nRet;
}

int H323Call::RequestSecondVideoKeyFrame(void)
{
	int nRet = -1;
	if(m_pSecondVideoRecvChannel)
	{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

		unsigned long nTimestamp=XGetTimestamp();
		if (nTimestamp - m_nSecondVideoLastReqKeyFrameTimestamp >= MAX_KEY_FRAME_INTERVAL * 1000)
		{
			IPCLIB_LogL2("[H323Call::RequestSecondVideoKeyFrame] Request second key frame.\n");
			nRet = m_pSecondVideoRecvChannel->RequestKeyFrame();
			m_nSecondVideoLastReqKeyFrameTimestamp = nTimestamp;
			m_bSecondVideoReqKeyFrame=false;
		}
		else
		{
			m_bSecondVideoReqKeyFrame=true;
		}

#else
		nRet = m_pSecondVideoRecvChannel->RequestKeyFrame();
#endif
	}
	return nRet;
}

//Add VCF
int H323Call::RequestMainVideoFreeze(void)
{
	int nRet = -1;
	if(m_pMainVideoRecvChannel) nRet=cmChannelSendMiscCommand(m_pMainVideoRecvChannel->m_hsChan,cmVideoFreezePicture);
	return nRet;
}

int H323Call::RequestSecondVideoFreeze(void)
{
	int nRet = -1;
	if(m_pSecondVideoRecvChannel) nRet=cmChannelSendMiscCommand(m_pSecondVideoRecvChannel->m_hsChan,cmVideoFreezePicture);
	return nRet;
}

void H323Call::SendGenericRequest(int nSymBreking) 
{
	m_nRecvGenericResp =false;
	m_nSendGenericReq =true;
	m_nRecvGenericReqCount =0;

	SetH239TokenSendReqStatus(1);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	PrintDebugStringInfo2(GetUserID().c_str(),GetUserID().c_str());
	if(m_bHasRemoteH239Video == false) return;
	if(m_bVirtualMark)
	{
		m_rH323CallMgr.SetH239MediaStatus(3);
		m_rH323CallMgr.RecordH239MsgType(1);
		m_rH323CallMgr.SetH239ControlAgent(2);
		m_rH323CallMgr.SendH239GenericRequest((H323MediaCall *)m_pVirtualCall);
		return ;
	}
#endif

	m_nterLabelReqSend = 257;//260;//int(MCUNO(nTerLabel) * 256 + TERNO(nTerLabel));

	srand(XGetTimestamp());
	int symBreking = rand()%127 + 1;
	if(nSymBreking ==0) symBreking = nSymBreking;

	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId;

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0)
	{
	}

	nodeId = pvtBuildByPath(hVal, rootId, "request.genericRequest", 0, NULL );
	if(nodeId < 0)
	{
	}

	unsigned char temp[64];
	temp[0] = 0x00;
	temp[1] = 0x08;
	temp[2] = 0x81;
	temp[3] = 0x6f;
	temp[4] = 0x02;
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageIdentifier.standard", 5, (char*)temp) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "subMessageIdentifier", 3, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterIdentifier.standard", 44, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterValue.unsignedMin", m_nterLabelReqSend, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterIdentifier.standard", 42, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterValue.unsignedMin", m_nchIDReqSend, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.3.parameterIdentifier.standard", 43, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.3.parameterValue.unsignedMin", symBreking, NULL ) )
	{
	}

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
	}
	pvtDelete(hVal,rootId);
}

void H323Call::SendGenericResponse(bool bAccept)
{
	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_bVirtualMark)
	{
		m_rH323CallMgr.SetH239MediaStatus(2);
		m_rH323CallMgr.RecordH239MsgType(2);
		m_rH323CallMgr.SetH239ControlAgent(2);
		m_rH323CallMgr.SendH239GenericResponse((H323MediaCall *)m_pVirtualCall,true);
		return ;
	}

#endif

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0)
	{
	}
	nodeId = pvtBuildByPath(hVal, rootId, "response.genericResponse", 0, NULL );
	if(nodeId < 0)
	{
	}

	unsigned char temp[64];
	temp[0] = 0x00;
	temp[1] = 0x08;
	temp[2] = 0x81;
	temp[3] = 0x6f;
	temp[4] = 0x02;
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageIdentifier.standard", 5, (char*)temp) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "subMessageIdentifier", 4, NULL ) )
	{
	}

	if(bAccept)
	{
		if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterIdentifier.standard", 126, NULL ) )
		{
		}
	}
	else
	{
		if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterIdentifier.standard", 127, NULL ) )
		{
		}
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterValue.logical", NULL, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterIdentifier.standard", 44, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterValue.unsignedMin", m_nterLabelReqRecv, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.3.parameterIdentifier.standard", 42, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.3.parameterValue.unsignedMin", m_nchIDReqRecv, NULL ) )
	{
	}

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
		h245ConHandle =0;
	}
	pvtDelete(hVal,rootId);

}

void H323Call::SendGenericCommand()   //Command
{
	SetH239TokenSendReqStatus(0);

	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_bVirtualMark)
	{
		m_rH323CallMgr.SetH239ControlAgent(2);
		m_rH323CallMgr.RecordH239MsgType(3);
		m_rH323CallMgr.SetH239MediaStatus(4);
		m_rH323CallMgr.SendH239GenericCommand((H323MediaCall *)m_pVirtualCall);
		return ;
	}

#endif

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0)
	{
	}

	nodeId = pvtBuildByPath(hVal, rootId, "command.genericCommand", 0, NULL );
	if(nodeId < 0)
	{
	}

	unsigned char temp[64];
	temp[0] = 0x00;
	temp[1] = 0x08;
	temp[2] = 0x81;
	temp[3] = 0x6f;
	temp[4] = 0x02;
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageIdentifier.standard", 5, (char*)temp) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "subMessageIdentifier", 5, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterIdentifier.standard", 44, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterValue.unsignedMin", m_nterLabelReqSend, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterIdentifier.standard", 42, NULL ) )
	{
	}//test m_nchIDReqSend=cmChannelGetNumber(m_pSecondVideoRecvChannel->m_hsChan);
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterValue.unsignedMin", m_nchIDReqSend, NULL ) )
	{
	}

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
	}

	pvtDelete(hVal,rootId);
}

void H323Call::SendGenericIndication() 
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2 )

#else
	//获得令牌后方向为发才可以发指示信令
	if(! ( m_nRecvGenericResp && m_nSecondVideoDir==IMXEC_CALL_DIR_OUT ) ) return ;
#endif


	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_bVirtualMark)
	{
		m_rH323CallMgr.SetH239ControlAgent(2);
		m_rH323CallMgr.RecordH239MsgType(4);
		m_rH323CallMgr.SetH239MediaStatus(4);
		m_rH323CallMgr.SendH239GenericIndication((H323MediaCall *)m_pVirtualCall);
		return ;
	}

#endif

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0)
	{
	}

	nodeId = pvtBuildByPath(hVal, rootId, "indication.genericIndication", 0, NULL );
	if(nodeId < 0)
	{
	}

	unsigned char temp[64];
	temp[0] = 0x00;
	temp[1] = 0x08;
	temp[2] = 0x81;
	temp[3] = 0x6f;
	temp[4] = 0x02;
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageIdentifier.standard", 5, (char*)temp) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "subMessageIdentifier", 6, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterIdentifier.standard", 44, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.1.parameterValue.unsignedMin", m_nterLabelReqSend, NULL ) )
	{
	}

	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterIdentifier.standard", 42, NULL ) )
	{
	}
	if( 0 > pvtBuildByPath(hVal, nodeId, "messageContent.2.parameterValue.unsignedMin", m_nchIDReqSend, NULL ) )
	{
	}

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
	}
	pvtDelete(hVal,rootId);
}

void H323Call::OnRecvGenericRequest(H323Channel *pH323Channel, int terLabel, int chID, int symBreaking,int type)
{
	OutputDebugString("OnRecvGenericRequest \n");
	if(m_bHasRemoteH239Video ==false) return;

	SetH239TokenSendReqStatus(0);

	if( !( type==1 && symBreaking ==0) ) SetH239TokenRequestStatus(1);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//本端发令牌请求后没收到响应
	if(m_nSendGenericReq && !m_nRecvGenericResp)
	{
		m_nterLabelReqRecv = terLabel;
		m_nchIDReqRecv = chID;
		//m_nRecvGenericReqCount ++;
		//if(m_nRecvGenericReqCount >=6)
		//{
		//	//如连续收到6次请求后则回应允许
		//	m_nRecvGenericReqCount =0;
		//	//允许
		//	H323CallMgr::Instance().OnRecvGenericRequest((H323MediaCall *)this,terLabel,chID,symBreaking,type);
		//}
		//else
		{
			SendGenericResponse(false);//不允许
			return;
		}
	}

	if( type==1 && symBreaking ==0)
	{
		m_bH239ReceiverOpen=false;
		m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
		OnSecondVideoRecvChannelDestroyed();
		//PrintDebugStringInfo3(GetUserID().c_str(),GetUserID().c_str(),"OnSecondVideoRecvChannelDestroyed");
		m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
		SetH239TokenRequestStatus(0);
	}
	else
	{
	if(m_bVirtualMark)
	{

	}
	else 
	{
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			//StopSecondVideo();
		}
		m_nterLabelReqRecv = terLabel;
		m_nchIDReqRecv = chID;
		if (m_bH239ReceiverOpen==false)
		{
			m_bH239ReceiverOpen=true;
			m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
			OnSecondVideoRecvChannelConnected();
			////PrintDebugStringInfo3(GetUserID().c_str(),GetUserID().c_str(),"OnSecondVideoRecvChannelConnected");
		}
	}

	H323CallMgr::Instance().OnRecvGenericRequest((H323MediaCall *)this,terLabel,chID,symBreaking,type);
	}

//此MCU接口执行后不会再执行到实体CALL中的处理,如也需要执行则移到下面或注释掉下面两行
#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	m_nterLabelReqRecv = terLabel;
	m_nchIDReqRecv = chID;

	if(type==1) RecvGenericMessage(h245grqGenericRequest,0,terLabel,chID,symBreaking);
	//A7 add 
	//modify 20140712 not use
	//if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	//{
	//	//Modified by WangLr 2013-07-18 是否停止由上层业务处理
	//	//StopSecondVideo();
	//}
	//m_nterLabelReqRecv = terLabel;
	//m_nchIDReqRecv = chID;

	//m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
	//// 通知业务层对端请求发双流，由业务层决定是否允许发起
	//m_rH323CallMgr.OnCallSecondVideoStart(m_strCallID);

#else

	//本端发令牌请求后没收到响应
	if(m_nSendGenericReq && !m_nRecvGenericResp)
	{
		m_nterLabelReqRecv = terLabel;
		m_nchIDReqRecv = chID;

		//m_nRecvGenericReqCount ++;
		//if(m_nRecvGenericReqCount >=6)
		//{
		//	//如连续收到6次请求后则回应允许
		//	m_nRecvGenericReqCount =0;
		//	//SendGenericResponse(true);//允许
		//}
		//else
		{
			SendGenericResponse(false);//不允许
			m_rH323CallMgr.OnCallSecondVideoStop(m_strCallID);
			return;
		}
	}

	if( type==1 && symBreaking ==0)
	{
		{
			StopSecondVideo();
			m_nterLabelReqRecv = terLabel;
			m_nchIDReqRecv = chID;
			SendGenericResponse(true);//允许
			m_bH239ReceiverOpen=false;
			m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
			m_rH323CallMgr.OnCallSecondVideoStop(m_strCallID);
			m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
		}
		SetH239TokenRequestStatus(0);
	}
	else
	{
		LogFile(NULL,NULL,"","","RecvGenericRequest ");
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			LogFile(NULL,NULL,"","","RecvGenericRequest StopSecondVideo ");
			StopSecondVideo();
		}
		m_nterLabelReqRecv = terLabel;
		m_nchIDReqRecv = chID;

		if(type==1) SendGenericResponse(true);//允许

		if (m_bH239ReceiverOpen==false)
		{
			m_bH239ReceiverOpen=true;

			m_nSecondVideoDir=IMXEC_CALL_DIR_IN;

			LogFile(NULL,NULL,"","","RecvGenericRequest OnCallSecondVideoStart ");
			m_rH323CallMgr.OnCallSecondVideoStart(m_strCallID);
		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

}

void H323Call::OnRecvGenericResponse(H323Channel *pH323Channel, int response, int terLabel, int chID,int respValue)
{
	if(m_bHasRemoteH239Video ==false) return;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_nSendGenericReq)
	{
		m_nSendGenericReq=false;
		m_nRecvGenericResp =true;
	}

	////PrintDebugStringInfo2(GetUserID().c_str(),GetUserID().c_str());
	if(m_bVirtualMark)
	{

	}
	else 
	{
		bool bAccept = false;
		if(response == 126)
		{
			bAccept = true;
		}
		if (bAccept)
		{
			m_nSecondVideoDir=IMXEC_CALL_DIR_OUT;
			//OnLocalSecondVideoStarted();
		}
		//else
		//{
		//	OnLocalSecondVideoStop();
		//}
	}

	H323CallMgr::Instance().OnRecvGenericResponse((H323MediaCall *)this,response,terLabel,chID);

//	//modify 20140712 previous not use {{
////#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//#else
//
//		RecvGenericMessage(h245grsGenericResponse,0,terLabel,chID,0,response,respValue);
////A7 add
////#else
//	//modify 20140712 previous }}

//modify 20140712 use
#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	RecvGenericMessage(h245grsGenericResponse,0,terLabel,chID,0,response,respValue);

#else

	RecvGenericMessage(h245grsGenericResponse,0,terLabel,chID,0,response,respValue);

	if(m_nSendGenericReq)
	{
		m_nSendGenericReq=false;
		m_nRecvGenericResp =true;
	}

	bool bAccept = false;
	if(response == 126)
	{
		bAccept = true;
	}
	if (bAccept)
	{
		m_nSecondVideoDir=IMXEC_CALL_DIR_OUT;
		if (m_pSecondVideoSendChannel)
		{
			m_pSecondVideoSendChannel->SendChannelOn();
			SendGenericIndication();
		}
		LogFile(NULL,NULL,"","","RecvGenericResponse OnLocalSecondVideoStarted ");
		OnLocalSecondVideoStarted();
	}
	else
	{
		//notice the terminal refuse is not call modify 20140715
		//OnLocalSecondVideoStop();
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

}

void H323Call::OnRecvGenericCommand(H323Channel *pH323Channel, int terLabel, int chID,int type)
{
	if(m_bHasRemoteH239Video ==false) return;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if( m_bVirtualMark) 
	{
	}
	else
	{
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			//StopSecondVideo();
		}
		else
		{
			m_bH239ReceiverOpen=false;
			OnSecondVideoRecvChannelDestroyed();
			////PrintDebugStringInfo3(GetUserID().c_str(),GetUserID().c_str(),"OnSecondVideoRecvChannelDestroyed");
			m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
		}
	}

	H323CallMgr::Instance().OnRecvGenericCommand((H323MediaCall *)this,terLabel,chID);

#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if(type==1) RecvGenericMessage(h245gcGenericCommand,0,terLabel,chID);
	////A7 add
	//modify 20140712 not use
	//if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	//{
	//	//Modified by WangLr 2013-07-18 是否停止由上层业务处理
	//	//StopSecondVideo();
	//}
	//else
	//{
	//	m_bH239ReceiverOpen=false;
	//	m_rH323CallMgr.OnCallSecondVideoStop(m_strCallID);
	//	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	//	//调整视频双流比例
	//	SetVideoBitRate();
	//}

#else

	if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
	{
		StopSecondVideo();
	}
	else
	{
		m_bH239ReceiverOpen=false;
		m_rH323CallMgr.OnCallSecondVideoStop(m_strCallID);
		m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	SetH239TokenRequestStatus(0);
}

void H323Call::OnRecvGenericIndication(H323Channel *pH323Channel, int terLabel, int chID)
{
	OutputDebugString("OnRecvGenericIndication \n");

	if(m_bHasRemoteH239Video ==false) return;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if(m_bVirtualMark) 
	{
	}
	else
	{
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			//StopSecondVideo();
		}
		if (m_bH239ReceiverOpen==false)
		{
			m_bH239ReceiverOpen=true;
			m_nterLabelReqRecv = terLabel;
			m_nchIDReqRecv = chID;
			m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
			OnSecondVideoRecvChannelConnected();
			////PrintDebugStringInfo3(GetUserID().c_str(),GetUserID().c_str(),"OnSecondVideoRecvChannelConnected");
		}
	}

	H323CallMgr::Instance().OnRecvGenericIndication((H323MediaCall *)this,terLabel,chID);

//	//modify 20140712 previous not use {{
//	//#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//#else
//	//modify 20140712 previous not use }}

//modify 20140712 use
#elif defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	RecvGenericMessage(h245giGenericIndication,0,terLabel,chID);

#else

	//notice 对通时有其需要 如有需要设为true 如宝利通个别型号，中兴个别型号发辅流前仅发genericIndication
	if(true)
	{
		RecvGenericMessage(h245giGenericIndication,0,terLabel,chID);
		//A7 add
		int nStatus=GetH239TokenRequestStatus();
		if(nStatus <=0)
		{
			SetH239TokenRequestStatus(1);
			if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
			{
				StopSecondVideo();
			}
			if (m_bH239ReceiverOpen==false)
			{
				m_bH239ReceiverOpen=true;
				m_nterLabelReqRecv = terLabel;
				m_nchIDReqRecv = chID;
				m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
				m_rH323CallMgr.OnCallSecondVideoStart(m_strCallID);
			}
		}
	}
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

}

void H323Call::OnChannelConnected(H323Channel*pH323Channel)
{
OpenAudioOutChannel();
OpenVideoOutChannel();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
OpenFECCOutChannel();
OpenH239VideoOutChannel();
#endif

#else
	OpenFECCOutChannel();
	OpenH239VideoOutChannel();
#endif

	if(pH323Channel == m_pAudioRecvChannel)
	{
		OnAudioRecvChannelConnected();
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		OnAudioSendChannelConnected();
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		OnMainVideoRecvChannelConnected();
		//RequestMainVideoKeyFrame();
		H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		OnMainVideoSendChannelConnected();
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		OnSecondVideoRecvChannelConnected();
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		OnSecondVideoSendChannelConnected();
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		OnH224RecvChannelConnected();
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		OnH224SendChannelConnected();
	}


//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)

#else

	OnAdapCallChannelConnected(pH323Channel);

#endif

//#endif

	if(pH323Channel == m_pAudioSendChannel)
	{
		pH323Channel->SendChannelOn();
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		pH323Channel->SendChannelOn();
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		pH323Channel->SendChannelOn();
	}

}

void H323Call::OnChannelDisconnected(H323Channel*pH323Channel)
{
	//pH323Channel->ChannelRTPPause();

	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	OnAdapCallChannelDisConnected(pH323Channel);
	//#else
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if(pH323Channel == m_pAudioRecvChannel)
	{
		OnAudioRecvChannelDestroyed();
		m_pAudioRecvChannel=NULL;
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		OnAudioSendChannelDestroyed();
		m_pAudioSendChannel=NULL;
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		OnMainVideoRecvChannelDestroyed();
		m_pMainVideoRecvChannel=NULL;
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		OnMainVideoSendChannelDestroyed();
		//XAutoLock l(m_csMainVideoSendChannel);
		m_pMainVideoSendChannel=NULL;
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		OnSecondVideoRecvChannelDestroyed();
		m_pSecondVideoRecvChannel=NULL;
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		OnSecondVideoSendChannelDestroyed();
		m_pSecondVideoSendChannel=NULL;
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		OnH224RecvChannelDestroyed();
		m_pH224RecvChannel=NULL;
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		OnH224SendChannelDestroyed();
		m_pH224SendChannel=NULL;
	}

	RemoveChannel(pH323Channel);
}

void H323Call::OnChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
//A7 Add
#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
	OnA7ChannelRecvdRTPPacket(pH323Channel,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
#else
	OnAdapChannelRecvdRTPPacket(pH323Channel,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif

#else

	if(pH323Channel == m_pAudioRecvChannel || pH323Channel == m_pAudioSendChannel)
	{
		OnRecvdRemoteAudioRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pMainVideoRecvChannel || pH323Channel == m_pMainVideoSendChannel)
	{
		OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel || pH323Channel == m_pSecondVideoSendChannel)
	{
		OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pH224RecvChannel || pH323Channel == m_pH224SendChannel)
	{
		m_pH224RecvChannel->OnRecvAnnexQDataProc(pRTPPacketData, nRTPPacketLen);
	}

	//Add U6 U7 use
	OnAdapChannelRecvdRTPPacket(pH323Channel,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

}

void H323Call::OnChannelRequestKeyFrame(H323Channel*pH323Channel)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	std::string strMediaName="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	H323CallMessage CallMessage(*this);
	if (pH323Channel==m_pMainVideoSendChannel)
	{
		m_rH323CallMgr.OnDAP_RecvMainVideoRequestKeyFrame(m_strCallID.c_str());
		strMediaName="VIDEO";
		CallMessage.RecvVideoFastUpdatePicture(strMediaName);
	}
	else if (pH323Channel==m_pSecondVideoSendChannel)
	{
		m_rH323CallMgr.OnDAP_RecvSecondVideoRequestKeyFrame(m_strCallID.c_str());
		strMediaName="DUALVIDEO";
		CallMessage.RecvVideoFastUpdatePicture(strMediaName);
	}

#else

	if (pH323Channel==m_pMainVideoSendChannel)
	{
		OnRecvdMainVideoRequestKeyFrame();
	}
	else if (pH323Channel==m_pSecondVideoSendChannel)
	{
		OnRecvdSecondVideoRequestKeyFrame();
	}
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

}

//add VCF
void H323Call::OnChannelRequestFreeze(H323Channel*pH323Channel)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	std::string strMediaName="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	H323CallMessage CallMessage(*this);
	if (pH323Channel==m_pMainVideoSendChannel)
	{
		strMediaName="VIDEO";
		CallMessage.RecvVideoFreezePicture(strMediaName);
	}
	else if (pH323Channel==m_pSecondVideoSendChannel)
	{
		strMediaName="DUALVIDEO";
		CallMessage.RecvVideoFreezePicture(strMediaName);
	}

#else

	//notice the VIDEC not support 
	//if (pH323Channel==m_pMainVideoSendChannel)
	//{
	//	OnRecvdMainVideoRequestFreeze();
	//}
	//else if (pH323Channel==m_pSecondVideoSendChannel)
	//{
	//	OnRecvdSecondVideoRequestFreeze();
	//}
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

}

/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	nPTZAction		-云台动作
nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323Call::SendPTZ (PTZ_ACTION nPTZAction,int nSpeed)
{
	m_nCurrentPTZAction=nPTZAction;
	m_nLastCheckPTZActionTimestamp=XGetTimestamp();

	int nRet=0;
	if(m_pH224SendChannel)
	{
		switch (nPTZAction)
		{
		case PTZ_START_TILT_LEFT:
			m_pH224SendChannel->RemoteCameraLeft();
			break;
		case PTZ_STOP_TILT_LEFT:
			m_pH224SendChannel->RemoteCameraStopLeft();
			break;
		case PTZ_START_TILT_RIGHT:
			m_pH224SendChannel->RemoteCameraRight();
			break;
		case PTZ_STOP_TILT_RIGHT:
			m_pH224SendChannel->RemoteCameraStopLeft();
			break;
		case PTZ_START_PAN_UP:
			m_pH224SendChannel->RemoteCameraUp();
			break;
		case PTZ_STOP_PAN_UP:
			m_pH224SendChannel->RemoteCameraStopDown();
			break;
		case PTZ_START_PAN_DOWN:
			m_pH224SendChannel->RemoteCameraDown();
			break;
		case PTZ_STOP_PAN_DOWN:
			m_pH224SendChannel->RemoteCameraStopDown();
			break;
		case PTZ_START_ZOOM_IN:
			m_pH224SendChannel->RemoteCameraZoomIn();
			break;
		case PTZ_STOP_ZOOM_IN:
			m_pH224SendChannel->RemoteCameraStopZoomOut();
			break;
		case PTZ_START_ZOOM_OUT:
			m_pH224SendChannel->RemoteCameraZoomOut();
			break;
		case PTZ_STOP_ZOOM_OUT:
			m_pH224SendChannel->RemoteCameraStopZoomOut();
			break;
		default:
			nRet=-1;
			break;
		};
	}
	else
	{
		nRet=-1;
	}
	return nRet;
}



/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int H323Call::StartSecondVideo (void)
{
	if (m_bHasRemoteH239Video)
	{
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
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
int H323Call::StopSecondVideo (void)
{
	if (m_bHasRemoteH239Video)
	{
		m_bH239ReceiverOpen=false;
		//SendGenericCommand();
		if (m_nSecondVideoDir==IMXEC_CALL_DIR_OUT)
		{
			if(m_pSecondVideoSendChannel) m_pSecondVideoSendChannel->SendChannelOff();//add
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
int H323Call::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	nDir=m_nSecondVideoDir;
	return 0;
}

void H323Call::OnLocalSecondVideoStarted(void)
{
	if (m_bH239ReceiverOpen==false)
	{
		m_bH239ReceiverOpen=true;
		m_nSecondVideoDir=IMXEC_CALL_DIR_OUT;
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )
//m_rH323CallMgr.OnCallSecondVideoStart(m_strCallID);
#else
m_rH323CallMgr.OnCallSecondVideoStart(m_strCallID);
#endif

	}
}

void H323Call::OnLocalSecondVideoStop(void)
{
	m_bH239ReceiverOpen=false;
	OnCallMgrCallSecondVideoStop(m_strCallID);
	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
}

int H323Call::GetSupportSecondVideo(void)
{
	return (m_bHasRemoteH239Video?1:0);
}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

void H323Call::OnMainVideoSendChannelConnected(void)
{
	m_rH323CallMgr.OnCallLocalMainVideoStart(m_strCallID);
}

void H323Call::OnMainVideoSendChannelDestroyed(void)
{
	m_rH323CallMgr.OnCallLocalMainVideoStop(m_strCallID);
}

#endif

unsigned short H323Call::GetCallRTPPort(void)
{
	unsigned short nRTPPort=CallMgr::Instance().GetMediaReleasePort(H323CallMgr::Instance().m_MediaPortRelease);
	if(nRTPPort >0) return nRTPPort;
	nRTPPort=GetRTPPort(GetMgrRTPBasePort());

	SetMgrRTPBasePort(nRTPPort+2);
	return nRTPPort;
}

void H323Call::DoCheckPTZAction(void)
{
	unsigned long nTimestamp=XGetTimestamp();

//send genericIndication
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)

#else

	if(m_bHasRemoteH239Video && m_nSecondVideoDir ==IMXEC_CALL_DIR_OUT)
	{
		if (m_nLastSendGenIndTimestamp > nTimestamp)
		{
			m_nLastSendGenIndTimestamp =nTimestamp;
		}
		else if (nTimestamp - m_nLastSendGenIndTimestamp>= 10000) //10 seconds
		{
			m_nLastSendGenIndTimestamp=nTimestamp;
			SendGenericIndication();
		}
	}

#endif

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) && defined(NOT_WIN32_H323_MSG) && (NOT_WIN32_H323_MSG == 1)

#else

	//终端原接口以及WIN32界面终端使用新接口时回调使用ThreadProcMain转调 调用
	if (nTimestamp-m_nLastCheckPTZActionTimestamp<=200)
	{
		return;
	}
	m_nLastCheckPTZActionTimestamp=nTimestamp;

	switch (m_nCurrentPTZAction)
	{
	case PTZ_START_PAN_UP:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraUp();
		}
		break;
	case PTZ_START_PAN_DOWN:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraDown();
		}
		break;
	case PTZ_START_TILT_LEFT:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraLeft();
		}
		break;
	case PTZ_START_TILT_RIGHT:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraRight();
		}		
		break;
	case PTZ_START_ZOOM_IN:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraZoomIn();
		}
		break;
	case PTZ_START_ZOOM_OUT:
		if(m_pH224SendChannel)
		{
			m_pH224SendChannel->RemoteCameraZoomOut();
		}		
		break;
	}

#endif

}

void H323Call::InitMediaRtpPortAllocate()
{
	memset(&m_MediaRtpPortAllocate,0,sizeof(MediaRtpPortAllocate) );
}

//dir 0 recv ,1 send
unsigned short H323Call::GetMediaRtpPort(ChannelMediaType type,H323Channel* pChannel,unsigned char dir)
{
	unsigned short port=0;
	unsigned char dirTemp=dir;
	//notice:if recv and send not share rtp port del the following row code
	if(dir==1) dirTemp=0;
	//same media type use same rtp port at call lifetime
	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_nMediaType==type) && 
			(m_MediaRtpPortAllocate.m_Array[i].m_nDirection==dirTemp) )
		{
			port=m_MediaRtpPortAllocate.m_Array[i].m_nRtpPort;
			if(dir==1)
				m_MediaRtpPortAllocate.m_Array[i].m_pSendChannel=pChannel;
			if(dir==0) 
				m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel=pChannel;

			break;
		}
	}

	if( (port==0) && (m_MediaRtpPortAllocate.m_nCount<MAX_ALLOCATE_NUM) )
	{
		port = GetCallRTPPort();
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nRtpPort=port;
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nMediaType=type;
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nDirection=dirTemp;

		if(dir==1)
			m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_pSendChannel=pChannel;
		if(dir==0) 
			m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_pRecvChannel=pChannel;

		m_MediaRtpPortAllocate.m_nCount++;
	}

	return port;
}

bool H323Call::RelatedMediaChannelCloseRTP(void * hRtp,H323Channel* pChannel)
{
	bool bRet=false;

	H323Channel* pAnotherChannel=NULL;
	if(hRtp==NULL) return false;

	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_hRtp==hRtp) )
		{
			m_MediaRtpPortAllocate.m_Array[i].m_nUseCount --;

			pAnotherChannel=m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel;
			if(m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel==pChannel)
			{
				pAnotherChannel=m_MediaRtpPortAllocate.m_Array[i].m_pSendChannel;
			}
			//another is empty rtp close
			if(pAnotherChannel==NULL)
			{
				//if(nType ==0) rtpClose( (HRTPSESSION )m_MediaRtpPortAllocate.m_Array[i].m_hRtp);
				//else if(nType ==1)
				//{
				//	if(pChannel) pChannel->MediaNetRtpDelete(m_MediaRtpPortAllocate.m_Array[i].m_hRtp);
				//}
				if(pChannel) pChannel->ChannelRTPClose(m_MediaRtpPortAllocate.m_Array[i].m_hRtp);

				m_MediaRtpPortAllocate.m_Array[i].m_hRtp=NULL;
				bRet=true;
			}
			//set is empty
			if(m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel==pChannel)
			{
				m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel=NULL;
			}
			if(m_MediaRtpPortAllocate.m_Array[i].m_pSendChannel==pChannel)
			{
				m_MediaRtpPortAllocate.m_Array[i].m_pSendChannel=NULL;
			}

			//nRet = m_MediaRtpPortAllocate.m_Array[i].m_nUseCount;

			break;
		}
	}
	return bRet;
}

void * H323Call::GetCreateMediaRtpHandle(unsigned short port)
{
	void * hRtp=NULL;
	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_nRtpPort==port) && 
			(m_MediaRtpPortAllocate.m_Array[i].m_hRtp!=NULL) )
		{
			hRtp=m_MediaRtpPortAllocate.m_Array[i].m_hRtp;
			m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nUseCount =2;
			break;
		}
	}
	return hRtp;
}

bool H323Call::SaveMediaRtpHandle(void * hRtp,unsigned short port)
{
	bool bSave=false;

	if( ( port==0) || (hRtp==NULL) ) return false;

	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_nRtpPort==port) && 
			(m_MediaRtpPortAllocate.m_Array[i].m_hRtp==NULL) )
		{
			m_MediaRtpPortAllocate.m_Array[i].m_hRtp=hRtp;
			bSave=true;
			break;
		}
	}

	if( (bSave==false) && (m_MediaRtpPortAllocate.m_nCount<MAX_ALLOCATE_NUM) )
	{
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nRtpPort=port;
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_hRtp=hRtp;
		m_MediaRtpPortAllocate.m_Array[m_MediaRtpPortAllocate.m_nCount].m_nUseCount=1;
		m_MediaRtpPortAllocate.m_nCount++;
		bSave=true;
	}

	return bSave;
}

H323Channel* H323Call::GetRelatedMediaChannel(void * hRtp,H323Channel* pChannel)
{
	H323Channel* pAnotherChannel=NULL;

	if(hRtp==NULL) return NULL;

	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_hRtp==hRtp) )
		{
			pAnotherChannel=m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel;
			if(m_MediaRtpPortAllocate.m_Array[i].m_pRecvChannel==pChannel)
			{
				pAnotherChannel=m_MediaRtpPortAllocate.m_Array[i].m_pSendChannel;
			}
			m_MediaRtpPortAllocate.m_Array[i].m_hRtp=NULL;
			break;
		}
	}

	return pAnotherChannel;
}

void H323Call::OnChannelRelatedDisconnected(H323Channel*pH323Channel)
{
	if(pH323Channel == m_pAudioRecvChannel)
	{
		OnAudioRecvChannelDestroyed();
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		OnAudioSendChannelDestroyed();
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		OnMainVideoRecvChannelDestroyed();
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		OnMainVideoSendChannelDestroyed();
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		OnSecondVideoRecvChannelDestroyed();
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		OnSecondVideoSendChannelDestroyed();
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		OnH224RecvChannelDestroyed();
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		OnH224SendChannelDestroyed();
	}
}

//A7 add
IPCLIB_API void enablenetec(int nEnable)
{
	g_nEnableNetEC = nEnable;
}

void H323Call::GetCallUserInfomation(HCALL hsCall,cmCallState_e state,int nodeId)
{
	int nMark=0;
	HPVT hVal;
	int  lenData;
	RvBool  bIsString;
	char strData[256]={0};
	char strBuffer[256]={0};
	int nodeData =-1;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	sprintf(strBuffer,"message.information.userUser.h323-UserInformation.h323-uu-pdu.nonStandardData.data");

	if(nodeId==-1)
	{
		lenData =-1;
		switch(state)
		{
		case cmCallStateOffering :
			cmCallGetParam(m_hsCall,cmParamSetupNonStandardData,0,&lenData,strData);
			break;
		case cmCallStateConnected :
			if(m_bMakeCall) //仅作主叫时取(因作被叫时仍有此呼叫状态回调)
				cmCallGetParam(m_hsCall,cmParamConnectNonStandardData,0,&lenData,strData);
			break;
		}

		if( lenData>=0 )
		{
			sprintf(strBuffer,"data");
			nodeId=lenData;
		}
	}
	else nMark=1;

	if(state ==101) //use to ras
	{
		sprintf(strBuffer,"data");
		nMark =0;
	}

	//从接收到的消息中取对方的nonStandardData
#if ( (defined(HAS_H323_AVCON_MP)) && (HAS_H323_AVCON_MP == 2) )
	//A7 add
	if(nodeId>=0 && g_nEnableNetEC)
	{
		nodeData = pvtGetByPath( hVal, nodeId, strBuffer, NULL, &lenData, &bIsString );
		if( nodeData>=0 && bIsString && lenData>0 )
		{
			if( pvtGetString( hVal, nodeData, lenData,strData ) > 0)
			{
				if(nMark ==1 ) m_strRecvUserInfomationNonstandData =strData;
				else m_strRecvUserNonstandardInfo =strData;
				StrPacket tStrPacket(m_strRecvUserNonstandardInfo);
				unsigned long nPeerLocalIP=0;
				tStrPacket.Get("NODEID",m_strPeerNodeID);
				tStrPacket.Get("MCUID",m_strMCUID);
				tStrPacket.Get("AID",m_nPeerAudioID);
				tStrPacket.Get("VID",m_nPeerVideoID);
				tStrPacket.Get("SVID",m_nPeerSecondVideoID);
				tStrPacket.Get("LIP",nPeerLocalIP);
				tStrPacket.Get("LPORT",m_nPeerLocalPort);
				if( nPeerLocalIP ) m_strPeerLocalIP=XSocket::htoa(ntohl(nPeerLocalIP));
				//IPCLIB_LogL2("[H323Call::GetCallUserInfomation] MCUID = %s, AID = %d, VID = %d\n", m_strMCUID.c_str(), m_nPeerAudioID, m_nPeerVideoID);
			}
		}
	}
#elif ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) )
if(nodeId>=0)
{
	nodeData = pvtGetByPath( hVal, nodeId, strBuffer, NULL, &lenData, &bIsString );
	if( nodeData>=0 && bIsString && lenData>0 )
	{
		if( pvtGetString( hVal, nodeData, lenData,strData ) > 0)
		{
			if(nMark ==1 ) m_strRecvUserInfomationNonstandData =strData;
			else m_strRecvUserNonstandardInfo =strData;
			StrPacket tStrPacket(m_strRecvUserNonstandardInfo);

			unsigned long nPeerLocalIP=0;
			tStrPacket.Get("AID",m_nPeerAudioID);
			tStrPacket.Get("VID",m_nPeerVideoID);
			tStrPacket.Get("LIP",nPeerLocalIP);
			tStrPacket.Get("LPORT",m_nPeerLocalPort);
			if (nPeerLocalIP)
			{
				m_strPeerLocalIP=XSocket::htoa(ntohl(nPeerLocalIP));
			}
		}
	}
}

#else

	IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
	GetMgrCallType(nCallType);
	if(nodeId>=0 && nCallType==IMXEC_PERSONAL_CALL)
	{
		nodeData = pvtGetByPath( hVal, nodeId, strBuffer, NULL, &lenData, &bIsString );
		if( nodeData>=0 && bIsString && lenData>0 )
		{
			if( pvtGetString( hVal, nodeData, lenData,strData ) > 0)
			{
				if(nMark ==1 ) m_strRecvUserInfomationNonstandData =strData;
				else m_strRecvUserNonstandardInfo =strData;

				StrPacket tStrPacket(m_strRecvUserNonstandardInfo);
				//if (!NETEC_Node::IsStarted())  //gk mcu use 20150409 
				{
					unsigned long nPeerLocalIP=0;
					m_nEnableMulticast=0;
					tStrPacket.Get("AID",m_nPeerAudioID);
					tStrPacket.Get("VID",m_nPeerVideoID);
					tStrPacket.Get("SVID",m_nPeerSecondVideoID);
					tStrPacket.Get("LIP",nPeerLocalIP);
					tStrPacket.Get("LPORT",m_nPeerLocalPort);
					tStrPacket.Get("MCUID",m_strPeerMCUID);
					tStrPacket.Get("AVCAST",m_nEnableMulticast);

					if(nPeerLocalIP ) m_strPeerLocalIP=XSocket::htoa(ntohl(nPeerLocalIP));

					//OutputDebugString(m_strRecvUserNonstandardInfo.c_str());
					//OutputDebugString("\n");
					////the add by gshzh
					//if(CallMgr::Instance().m_nUseNETECSend > 0 )
					//{
					//	OutputDebugString("NETEC Node Start() first.\n");
					//	NETEC_NodeStart();
					//	OutputDebugString("NETEC Node Start() after.\n");
					//}
				}
			}
		}
	}
#endif


	if(nMark ==1 )
	{
		int nFound=-1;
		nFound=m_strRecvUserInfomationNonstandData.find(m_rH323CallMgr.m_strH225LogicalKA);
		if( nFound <0)
		m_rH323CallMgr.OnDAP_RecvNonstandardData(m_strCallID,m_strRecvUserInfomationNonstandData);
	}
}

void H323Call::SendCallUserInfomation(std::string strNonstandData) //Send H225 User Infomation
{
	if(0)
	{
		HPVT hVal;
		HPST synTree;
		HPROTCONN h245ConHandle;
		int rootId ,nodeId;
		int tmpError;
		int paramLen=128;
		hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
		rootId = pvtAddRoot(hVal, synTree, 0, NULL);
		if(rootId < 0)
		{
		}
		nodeId = pvtBuildByPath(hVal, rootId, "indication.nonStandard.nonStandardData", 0, NULL );
		if(nodeId >= 0)
		{
			tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35CountryCode",H323CallMgr::Instance().m_nT35Code, 0 );
			tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35Extension", H323CallMgr::Instance().m_nT35Extension, 0 );
			tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, 0 );
			paramLen=strNonstandData.length();
			tmpError = pvtBuildByPath(hVal, nodeId, "data", paramLen, strNonstandData.c_str() );
		}
		h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
		if(h245ConHandle)
		{
			cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId);
		}
		pvtDelete(hVal,rootId);
		return;
	}


	HPVT hVal;
	HPST synTree;
	HPROTCONN hQ931ConHandle;
	int rootId ,nodeId;
	int tmpNodeId;
	int tmpError;
	int paramLen=128;
	char paramStr[128]={0};
	char OID[10];
	int length;

	//H225_PROTOCOL_IDENTIFIER Same To H323Statck H225 Version
#define H225_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 2250 0 4"

	if(strNonstandData.size() <=0 || strNonstandData.size() >= 131) return ;

	length=utlEncodeOID(sizeof(OID), OID, H225_PROTOCOL_IDENTIFIER);
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "q931");

	//user information
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nodeId=rootId;
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "protocolDiscriminator", 8, 0 );

	cmCallGetParam(m_hsCall,cmParamCRV,0,&paramLen,NULL);
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "callReferenceValue.twoBytes", paramLen, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.protocolDiscriminator", 5, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body", 0, 0 );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "information.protocolIdentifier", length, OID );

	cmCallGetParam(m_hsCall,cmParamCallID,0,&paramLen,paramStr);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "information.callIdentifier.guid", paramLen, paramStr );

	paramLen=strNonstandData.length();
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.user-data.protocol-discriminator", 5, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.user-data.user-information", paramLen, strNonstandData.c_str() );

	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.h323-uu-pdu", 0, 0 );
	nodeId = pvtBuildByPath(hVal, tmpNodeId, "nonStandardData", 0, 0 );
	//tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.object", length, OID );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35CountryCode",H323CallMgr::Instance().m_nT35Code, 0 );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35Extension", H323CallMgr::Instance().m_nT35Extension, 0 );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, 0 );

	paramLen=strNonstandData.length();
	tmpError = pvtBuildByPath(hVal, nodeId, "data", paramLen, strNonstandData.c_str() );

	hQ931ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmQ931TpktChannel);
	tmpError = cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, hQ931ConHandle, rootId);
	pvtDelete(hVal,rootId);
	return;

	//releaseComplete test
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nodeId=rootId;
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "protocolDiscriminator", 8, 0 );

	cmCallGetParam(m_hsCall,cmParamCRV,0,&paramLen,NULL);
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "callReferenceValue.twoBytes", paramLen, 0);
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.cause.octet3.codingStandard", 0, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.cause.octet3.spare", 0, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.cause.octet3.location", 0, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.cause.octet4.causeValue", 0, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.userUser.protocolDiscriminator", 5, 0 );

	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body", 0, 0 );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.protocolIdentifier", length, OID );

	cmCallGetParam(m_hsCall,cmParamCallID,0,&paramLen,paramStr);

	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.callIdentifier.guid", paramLen, paramStr );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.presentationIndicator.presentationAllowed", 0, 0 );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.screeningIndicator.userProvidedVerifiedAndFailed", 0, 0 );

	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.releaseComplete.userUser.h323-UserInformation.h323-uu-pdu.h245Tunneling", 0, 0 );

	hQ931ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmQ931TpktChannel);
	tmpError = cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, hQ931ConHandle, rootId);
	pvtDelete(hVal,rootId);
	return;

	////app->synProtQ931=pstConstruct(cmEmGetQ931Syntax(), "Q931Message");
	////app->synQ931UU=pstConstruct(cmEmGetQ931Syntax(), "H323-UU-PDU");
	//synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "q931UU");
	//rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	//tmpNodeId = pvtBuildByPath(hVal, rootId, "h323-message-body", 0, 0 );
	//tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.protocolIdentifier", length, OID );
	//return;
}


void H323Call::SetCallNonStandardData(cmCallState_e state) //Set H225 NonStandard Data
{
	HPVT hVal;
	HPST synTree;
	int rootId ,nodeId;
	int tmpNodeId;
	int tmpError;
	int paramLen=128;
	char paramStr[128]={0};
	char OID[10];
	int length;
	int nonNodeId=-1;
#define H225_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 2250 0 4"

	if(m_strSetUserNonstandardInfo.size() <=0 ) return ;

	length=utlEncodeOID(sizeof(OID), OID, H225_PROTOCOL_IDENTIFIER);
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "q931");

	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nonNodeId = pvtAddRoot(hVal, NULL, 0, NULL);
	nodeId=rootId;
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "protocolDiscriminator", 8, 0 );

	cmCallGetParam(m_hsCall,cmParamCRV,0,&paramLen,NULL);
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "callReferenceValue.twoBytes", paramLen, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.protocolDiscriminator", 5, 0 );
	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.h323-uu-pdu.h323-message-body", 0, 0 );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "information.protocolIdentifier", length, OID );

	cmCallGetParam(m_hsCall,cmParamCallID,0,&paramLen,paramStr);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "information.callIdentifier.guid", paramLen, paramStr );

	tmpNodeId = pvtBuildByPath(hVal, nodeId, "message.information.userUser.h323-UserInformation.h323-uu-pdu", 0, 0 );
	nodeId = pvtBuildByPath(hVal, tmpNodeId, "nonStandardData", 0, 0 );
	//tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.object", length, OID );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35CountryCode",H323CallMgr::Instance().m_nT35Code, 0 );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.t35Extension", H323CallMgr::Instance().m_nT35Extension, 0 );
	tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.h221NonStandard.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, 0 );

	paramLen=m_strSetUserNonstandardInfo.length();
	tmpError = pvtBuildByPath(hVal, nodeId, "data", paramLen, m_strSetUserNonstandardInfo.c_str() );
	tmpError=pvtShiftTree(hVal,nonNodeId,nodeId);

	switch(state)
	{
	case cmCallStateOffering :
		cmCallSetParam(m_hsCall,cmParamSetupNonStandardData,0,nonNodeId,NULL);
		m_bMakeCall=true;//主叫状态
		break;
	case cmCallStateConnected :
		cmCallSetParam(m_hsCall,cmParamConnectNonStandardData,0,nonNodeId,NULL);
		break;
	}
	pvtDelete(hVal,rootId);
	pvtDelete(hVal,nonNodeId);
}

const std::string& H323Call::GetRecvUserNonstandardInfo(void)
{
	return m_strRecvUserNonstandardInfo;
}

void H323Call::SetUserNonstandardInfo(const char * info)
{
	m_strSetUserNonstandardInfo = info ;
}

void H323Call::GetUserNonstandardInfo(std::string & strUserNonstand)
{
	if(m_strSetUserNonstandardInfo.size() <=0)
		H323CallMgr::Instance().GetCallUserNonstandardInfo((H323MediaCall *)this,&m_ConfInfo,NULL);

	strUserNonstand = m_strSetUserNonstandardInfo ;
}

////H264 Level Value  因适应RADVision ViaIp MCU 720P作出改变
extern int H264_LEVEL_QCIF  ;     //1.1 30fps
extern int H264_LEVEL_CIF   ;     //1.3 30fps
extern int H264_LEVEL_4CIF  ;     //3   25fps
extern int H264_LEVEL_16CIF ;    //4   39fps
extern int H264_LEVEL_720I  ;     //3.1 60fps ?
extern int H264_LEVEL_720P  ;     //3.1 30fps
extern int H264_LEVEL_1080I ;     //3.2 60fps ?
extern int H264_LEVEL_1080P ;     //4   30fps
extern int H264_LEVEL_VGA   ;     //3   34fps
extern int H264_LEVEL_SVGA  ;     //3.1 57fps
extern int H264_LEVEL_XGA   ;     //3.1 35fps
extern int H264_LEVEL_SXGA  ;     //3.2 42fps

void H323Call::SetH264LevelFromProductId(VIDEO_FORMAT_TYPE nFormat,int & nLevel)
{
	//productId: Codian MCU 4515  versionId: 3.1(2.13)
	int nFindPro=-1;
	bool bFind=false;
	nFindPro=m_strProductID.find("Codian");
	if (nFindPro>=0) bFind=true;

	if(bFind)
	{
		switch (nFormat)
		{
		case VIDEO_FORMAT_QCIF:
			nLevel = H264_LEVEL_QCIF;
			break;
		case VIDEO_FORMAT_CIF:
			nLevel = H264_LEVEL_CIF;
			break;
		case VIDEO_FORMAT_4CIF:
			nLevel = H264_LEVEL_4CIF;
			break;
		case VIDEO_FORMAT_16CIF:
			nLevel = H264_LEVEL_16CIF;
			break;
		case VIDEO_FORMAT_720I:
			nLevel = H264_LEVEL_720I;
			break;
		case VIDEO_FORMAT_720P:
			nLevel = H264_LEVEL_720P;
			break;
		case VIDEO_FORMAT_1080I:
			nLevel = H264_LEVEL_1080I;
			break;
		case VIDEO_FORMAT_1080P:
			nLevel = H264_LEVEL_1080P;
			break;
		case VIDEO_FORMAT_VGA:
			nLevel = H264_LEVEL_VGA;
			break;
		case VIDEO_FORMAT_SVGA:
			nLevel = H264_LEVEL_SVGA;
			break;
		case VIDEO_FORMAT_XGA:
			nLevel = H264_LEVEL_XGA;
			break;
		case VIDEO_FORMAT_SXGA:
			nLevel = H264_LEVEL_SXGA;
			break;
		default:
			break;
		}

	}
}

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


//检查令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
bool H323Call::CheckH239TokenOwnState()
{
	return m_bTokenOwnState;
}

//置令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
void H323Call::SetH239TokenOwnState(bool bState)
{
	m_bTokenOwnState=bState;
}

void H323Call::SetConferenceID(std::string strConferenceID)
{
	m_strConferenceID=strConferenceID;
}

std::string & H323Call::GetConferenceID(void)
{
	return m_strConferenceID;
}

void H323Call::SetCallID(std::string strCallID,H323Call * pCall)
{
	m_strCallID=strCallID;
	m_pVirtualCall=pCall;
	m_strUserID=m_strCallID;
}

void H323Call::SetVitualMark(bool mark)
{
	m_bVirtualMark=mark;
	if(H323CallMgr::Instance().GetSupportH239() != 0)
		m_bHasRemoteH239Video=true;
}

int H323Call::GetChannelInfo(CHANNEL_INFO& rChannelInfo)
{
	rChannelInfo.AudioCodecID=m_struChannelInfo.AudioCodecID;
	rChannelInfo.VideoCodecType=m_struChannelInfo.VideoCodecType;
	rChannelInfo.VideoFormatType=m_struChannelInfo.VideoFormatType;
	rChannelInfo.nDirection =m_struChannelInfo.nDirection;
	rChannelInfo.nPayloadType =m_struChannelInfo.nPayloadType;
	rChannelInfo.ulRTPLocalIP =m_struChannelInfo.ulRTPLocalIP;
	rChannelInfo.nRTPLocalPort =m_struChannelInfo.nRTPLocalPort;
	rChannelInfo.ulRTPRemoteIP =m_struChannelInfo.ulRTPRemoteIP;
	rChannelInfo.nRTPRemotePort =m_struChannelInfo.nRTPRemotePort;
	rChannelInfo.ulRTCPLocalIP =m_struChannelInfo.ulRTCPLocalIP;
	rChannelInfo.nRTCPLocalPort =m_struChannelInfo.nRTCPLocalPort;
	rChannelInfo.ulRTCPRemoteIP =m_struChannelInfo.ulRTCPRemoteIP;
	rChannelInfo.nRTCPRemotePort =m_struChannelInfo.nRTCPRemotePort;
	rChannelInfo.nMaxBitrate =m_struChannelInfo.nMaxBitrate;
	return 0;
}

int H323Call::SetSendSecondVideoParam(int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nPayloadType)
{
	m_struChannelInfo.nMaxBitrate=nBitrate;
	m_struChannelInfo.nPayloadType=nPayloadType;
	m_struChannelInfo.VideoCodecType=nVideoCodecType;
	m_struChannelInfo.VideoFormatType=nVideoFormatType;
	return 0;
}


#endif //#if defined(HAS_H323_AVCON_MP) && HAS_H323_AVCON_MP == 1


void H323Call::SetH239TokenRequestStatus(int status)
{
	m_nRecvH239TokenReq=status;
}

int H323Call::GetH239TokenRequestStatus(void)
{
	return m_nRecvH239TokenReq;
}

unsigned long H323Call::GetMgrExternalIPAddress(void)
{
	unsigned long nExternalIPAddress= CallMgr::Instance().GetExternalIPAddress();
	return nExternalIPAddress;
}

//1013 add
std::string H323Call::GetMgrExternalIPAddressString(void)
{
	return CallMgr::Instance().GetExternalIP();
}

void H323Call::OnCallMgrRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	H323CallMgr::Instance().OnDAP_CallRecvdPTZControl(cszCallID,nPTZAction,nSpeed);
#else
	CallMgr::Instance().OnCallRecvdPTZ(cszCallID,nPTZAction,nSpeed);
#endif

}

void H323Call::OnCallMgrCallSecondVideoStop(const std::string&strCallID)
{
//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else

CallMgr::Instance().OnCallSecondVideoStop(strCallID);

#endif

}

unsigned short H323Call::GetMgrRTPBasePort(void)
{
	unsigned short nRtpBasePort= CallMgr::Instance().GetRTPBasePort();
	return nRtpBasePort;
}

void H323Call::SetMgrRTPBasePort(unsigned short nRTPBasePort)
{
	CallMgr::Instance().SetRTPBasePort(nRTPBasePort);
}

int H323Call::GetMgrCallType(IMXEC_CALL_TYPE&nCallType)
{
	int nRet=0;
//mcu adapter add
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#if ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) )
	nRet=0;
#else
	nRet=CallMgr::Instance().GetCallType(nCallType);
#endif

	return nRet;
}

void H323Call::GetMgrLocalIPByRemoteIP(std::string & strLocalIP,const std::string&strHost,unsigned short nPort)
{
	strLocalIP=GetLocalIPByRemoteIP(strHost,nPort);
}

CallCapability & H323Call::GetMgrCapability()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_CallCapability;
#else
	return H323CallMgr::Instance().m_CallCapability;
#endif

}

EncryptionCapability & H323Call::GetMgrEncryptionCapability()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_EncryptionCapability;
#else
	return H323CallMgr::Instance().m_EncryptionCapability;
#endif

}

int H323Call::GetMgrMainVideoMaxBitrate(void)
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nMainVideoMaxBitrate;
#else
	return H323CallMgr::Instance().GetMainVideoMaxBitrate();
#endif

}

int H323Call::GetMgrSecondVideoMaxBitrate(void)
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nSecondVideoMaxBitrate;
#else
	return H323CallMgr::Instance().GetSecondVideoMaxBitrate();
#endif

}

int H323Call::SetLocalMainVideoH264Level(int nLevel)
{
	if(nLevel > 0) m_nLocalMainVideoH264Level=nLevel;
	return 0;
}

int H323Call::SetLocalSecondVideoH264Level(int nLevel)
{
	if(nLevel > 0) m_nLocalSecondVideoH264Level=nLevel;
	return 0;
}

int H323Call::SetLocalMainVideoH264HPLevel(int nLevel)
{
	if(nLevel > 0) m_nLocalMainVideoH264HPLevel=nLevel;
	return 0;
}

int  H323Call::SetLocalSecondVideoH264HPLevel(int nLevel)
{
	if(nLevel > 0) m_nLocalSecondVideoH264HPLevel=nLevel;
	return 0;
}

int  H323Call::SetRTPMediaMulticast(const char*cszMulticastIP,int ttl)
{
	if(cszMulticastIP && ( strlen(cszMulticastIP) > 0 ) )
	{
		m_strMediaMulticastIP =cszMulticastIP;
		m_bMediaMulticastEnable=true;
		m_nMediaMulticastTTL=ttl;
	}
	return 0;
}


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

//audio cap
int H323Call::AddCapAudioAll(void)
{
	return 0;
}

int H323Call::RemoveCapAudioAll(void)
{
	m_CallCapability.RemoveAudioAllCapabilitis();
	m_bAudioCapItemSetFirst=false;
	return 0;
}

int H323Call::AddCapAudioItem(X_AUDIO_CODEC_ID	codecID)
{
	return 0;
}

int H323Call::AddCapAudioG711A_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G711A);
}

int H323Call::AddCapAudioG711U_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G711U);
}

int H323Call::AddCapAudioG722_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G722_64);
}

int H323Call::AddCapAudioG7221_24K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_7_24);
}

int H323Call::AddCapAudioG7221_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_7_32);
}

int H323Call::AddCapAudioG7221C_24K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_24);
}

int H323Call::AddCapAudioG7221C_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_32);
}

int H323Call::AddCapAudioG7221C_48K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G7221_14_48);
}

int H323Call::AddCapAudioG729A(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G729);
}

int H323Call::AddCapAudioG719_32K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_32);
}

int H323Call::AddCapAudioG719_48K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_48);
}

int H323Call::AddCapAudioG719_64K(void)
{
	return AddCapAudioItem(X_AUDIO_CODEC_G719_64);
}

int H323Call::AddCapVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel)
{
	return 0;
}

int H323Call::AddCapDualVideoItem(VIDEC_CODEC_TYPE codecID,VIDEO_FORMAT_TYPE formatType,int nLevel)
{
	return 0;
}

int H323Call::AddCapVideoAll(void)
{
	return 0;
}

int H323Call::RemoveCapVideoAll(void)
{
	m_CallCapability.RemoveVideoAllCapabilitis();
	m_bVideoCapItemSetFirst=false;
	return 0;
}

int H323Call::AddCapVideoH263(int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	VIDEO_FORMAT_TYPE nMainVideoFTH263=VIDEO_FORMAT_QCIF;
	if(nHas4CIF>0) nMainVideoFTH263=VIDEO_FORMAT_4CIF;
	else if(nHasCIF>0) nMainVideoFTH263=VIDEO_FORMAT_CIF;
	else if(nHasQCIF>0) nMainVideoFTH263=VIDEO_FORMAT_QCIF;

	return AddCapVideoItem(VIDEC_CODEC_H263,nMainVideoFTH263,0);
}

int H323Call::AddCapVideoH263Plus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	//分辨率优先处理
	VIDEO_FORMAT_TYPE nMainVideoFTH263=VIDEO_FORMAT_QCIF;
	if(nHas720P>0) nMainVideoFTH263=VIDEO_FORMAT_720P;
	else if(nHasSXGA>0) nMainVideoFTH263=VIDEO_FORMAT_SXGA;
	else if(nHasXGA>0) nMainVideoFTH263=VIDEO_FORMAT_XGA;
	else if(nHasSVGA>0) nMainVideoFTH263=VIDEO_FORMAT_SVGA;
	else if(nHasVGA>0) nMainVideoFTH263=VIDEO_FORMAT_VGA;

	return AddCapVideoItem(VIDEC_CODEC_H263P,nMainVideoFTH263,0);
}


int H323Call::AddCapVideoH263PlusPlus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	//分辨率优先处理
	VIDEO_FORMAT_TYPE nMainVideoFTH263=VIDEO_FORMAT_QCIF;
	if(nHas720P>0) nMainVideoFTH263=VIDEO_FORMAT_720P;
	else if(nHasSXGA>0) nMainVideoFTH263=VIDEO_FORMAT_SXGA;
	else if(nHasXGA>0) nMainVideoFTH263=VIDEO_FORMAT_XGA;
	else if(nHasSVGA>0) nMainVideoFTH263=VIDEO_FORMAT_SVGA;
	else if(nHasVGA>0) nMainVideoFTH263=VIDEO_FORMAT_VGA;

	return AddCapVideoItem(VIDEC_CODEC_H263PP,nMainVideoFTH263,0);
}

int H323Call::AddCapVideoH264QCIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_QCIF,nLevel);
}

int H323Call::AddCapVideoH264CIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_CIF,nLevel);
}

int H323Call::AddCapVideoH2644CIF(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_4CIF,nLevel);
}

int H323Call::AddCapVideoH264720P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_720P,nLevel);
}

int H323Call::AddCapVideoH2641080P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_1080P,nLevel);
}

int H323Call::AddCapVideoH264HP720P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_720P,nLevel);
}

int H323Call::AddCapVideoH264HP1080P(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_1080P,nLevel);
}

int H323Call::AddCapVideoH264VGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_VGA,nLevel);
}

int H323Call::AddCapVideoH264SVGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SVGA,nLevel);
}

int H323Call::AddCapVideoH264XGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_XGA,nLevel);
}

int H323Call::AddCapVideoH264SXGA(int nLevel)
{
	return AddCapVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SXGA,nLevel);
}

int H323Call::AddCapDualVideoAll()
{
	m_CallCapability.AddH239VideoH264720PExtend();
	m_CallCapability.AddH239VideoH264CIFExtend();
	m_CallCapability.AddH239VideoH2644CIFExtend();
	m_CallCapability.AddH239VideoH26416CIFExtend();
	m_CallCapability.AddH239VideoH2641080PExtend();
	m_CallCapability.AddH239VideoH264VGAExtend();
	m_CallCapability.AddH239VideoH264SVGAExtend();
	m_CallCapability.AddH239VideoH264XGAExtend();
	m_CallCapability.AddH239VideoH264SXGAExtend();						
	m_CallCapability.AddH239VideoH264720PHighProfile();
	m_CallCapability.AddH239VideoH2641080PHighProfile();
	m_CallCapability.AddH239VideoH263Extend(1,1,1);
	return 0;
}

int H323Call::RemoveCapDualVideoAll()
{
	m_CallCapability.RemoveDualVideoAllCapabilitis();
	m_bDualVideoCapItemSetFirst=false;
	return 0;
}

int H323Call::AddCapDualVideoH263(int nHasQCIF,int nHasCIF,int nHas4CIF)
{
	VIDEO_FORMAT_TYPE nSecondVideoFTH263=VIDEO_FORMAT_QCIF;
	if(nHas4CIF>0) nSecondVideoFTH263=VIDEO_FORMAT_4CIF;
	else if(nHasCIF>0) nSecondVideoFTH263=VIDEO_FORMAT_CIF;
	else if(nHasQCIF>0) nSecondVideoFTH263=VIDEO_FORMAT_QCIF;

	return AddCapDualVideoItem(VIDEC_CODEC_H263,nSecondVideoFTH263,0);
}

int H323Call::AddCapDualVideoH263Plus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	VIDEO_FORMAT_TYPE nSecondVideoFTH263=VIDEO_FORMAT_4CIF;
	if(nHas720P>0) nSecondVideoFTH263=VIDEO_FORMAT_720P;
	else if(nHasSXGA>0) nSecondVideoFTH263=VIDEO_FORMAT_SXGA;
	else if(nHasXGA>0) nSecondVideoFTH263=VIDEO_FORMAT_XGA;
	else if(nHasSVGA>0) nSecondVideoFTH263=VIDEO_FORMAT_SVGA;
	else if(nHasVGA>0) nSecondVideoFTH263=VIDEO_FORMAT_VGA;

	return AddCapDualVideoItem(VIDEC_CODEC_H263P,nSecondVideoFTH263,0);
}

int H323Call::AddCapDualVideoH263PlusPlus(int nHasVGA,int nHasSVGA,int nHasXGA,int nHasSXGA,int nHas720P)
{
	VIDEO_FORMAT_TYPE nSecondVideoFTH263=VIDEO_FORMAT_4CIF;
	if(nHas720P>0) nSecondVideoFTH263=VIDEO_FORMAT_720P;
	else if(nHasSXGA>0) nSecondVideoFTH263=VIDEO_FORMAT_SXGA;
	else if(nHasXGA>0) nSecondVideoFTH263=VIDEO_FORMAT_XGA;
	else if(nHasSVGA>0) nSecondVideoFTH263=VIDEO_FORMAT_SVGA;
	else if(nHasVGA>0) nSecondVideoFTH263=VIDEO_FORMAT_VGA;

	return AddCapDualVideoItem(VIDEC_CODEC_H263PP,nSecondVideoFTH263,0);
}

int H323Call::AddCapDualVideoH264QCIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_QCIF,nLevel);
}

int H323Call::AddCapDualVideoH264CIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_CIF,nLevel);
}

int H323Call::AddCapDualVideoH2644CIF(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_4CIF,nLevel);
}

int H323Call::AddCapDualVideoH264720P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_720P,nLevel);
}

int H323Call::AddCapDualVideoH2641080P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_1080P,nLevel);
}

int H323Call::AddCapDualVideoH264HP720P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_720P,nLevel);
}

int H323Call::AddCapDualVideoH264HP1080P(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264_SVC,VIDEO_FORMAT_1080P,nLevel);
}

int H323Call::AddCapDualVideoH264VGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_VGA,nLevel);
}

int H323Call::AddCapDualVideoH264SVGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SVGA,nLevel);
}

int H323Call::AddCapDualVideoH264XGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_XGA,nLevel);
}

int H323Call::AddCapDualVideoH264SXGA(int nLevel)
{
	return AddCapDualVideoItem(VIDEC_CODEC_H264,VIDEO_FORMAT_SXGA,nLevel);
}

int H323Call::AddCapFECCData()
{
	m_CallCapability.AddFECCData();
	return 0;
}

int H323Call::RemoveCapAll(void)
{
	return 0;
}

int H323Call::RemoveCapFECCDataAll(void)
{
	m_CallCapability.RemoveFECCDataAllCapabilitis();
	return 0;
}

int H323Call::DAPSetLocalCapabilitis(void)
{
	return 0;
}

#endif

int H323Call::SetSupportH239(int nSupportH239)
{
	m_nSupportH239=nSupportH239;
	return 0;
}

int H323Call::BuildLocalCapabilitisHeader()
{
//version update H.245 Version used  
#define H245_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 245 0 13"

	int			nodeId;
	HPST		synTree;
	int			rootId;
	int			error;

	if(m_nCapabilityNodeID<=0)
	{
		char OID[10];
		int length=utlEncodeOID(sizeof(OID), OID, H245_PROTOCOL_IDENTIFIER);
		HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
		rootId = pvtAddRoot(hVal, synTree, 0, NULL);
		if(rootId < 0)	return -1;
		nodeId = pvtBuildByPath(hVal, rootId, "request.terminalCapabilitySet", 0, NULL );
		if(nodeId < 0)
		{
			pvtDelete(hVal,rootId);
			return -1;
		}

		//nodeId = pvtAddRootByPath(hVal, synTree, "request.terminalCapabilitySet", 0, NULL );
 
		//设置能力集头
		pvtBuildByPath(hVal, nodeId, "sequenceNumber", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "protocolIdentifier",length, OID);

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.maximumAudioDelayJitter", 60, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.multicastCapability", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.multiUniCastConference", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.centralizedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.distributedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.centralizedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.distributedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.centralizedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveMultipointCapability.mediaDistributionCapability.1.distributedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.multicastCapability", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.multiUniCastConference", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.centralizedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.distributedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.centralizedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.distributedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.centralizedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.transmitMultipointCapability.mediaDistributionCapability.1.distributedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.multicastCapability", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.multiUniCastConference", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.centralizedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.distributedControl", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.centralizedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.distributedAudio", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.centralizedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.receiveAndTransmitMultipointCapability.mediaDistributionCapability.1.distributedVideo", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.mcCapability.centralizedConferenceMC", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.mcCapability.decentralizedConferenceMC", 0, NULL );

		pvtBuildByPath(hVal, nodeId, "multiplexCapability.h2250Capability.rtcpVideoControlCapability", 0, NULL );

		//设置能力集描述语头 默认只有一个同时能力集
		int nSimulNum = 1;
		char tempbuf[512];
		sprintf(tempbuf,"capabilityDescriptors.*");
		error = pvtBuildByPath( hVal, nodeId,tempbuf,0, NULL); 
		sprintf(tempbuf,"capabilityDescriptors.%d.capabilityDescriptorNumber",nSimulNum);
		error = pvtBuildByPath( hVal, nodeId,tempbuf,1, NULL);

		m_nCapabilityNodeID=rootId;
		//m_nCapabilityNodeID=nodeId;
	}

	return m_nCapabilityNodeID;
}

int H323Call::BuildSendLocalCapabilitis()
{
	int	nodeId;
	int error=-1;
	nodeId=BuildLocalCapabilitisHeader();
	if(nodeId>=0)
	{
		HPVT hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		nodeId=pvtChild(hVal,nodeId);
		nodeId=pvtChild(hVal,nodeId);
		SetLocalCapabilitis(nodeId);
		error=cmCallSendCapability(m_hsCall, nodeId);
	}
	return nodeId;
}

int H323Call::BuildSendVideoOLCCapabilitis()
{
	int			nodeId;
	HPST		synTree;
	int			rootId;
	int			error;
	int			dataNodeId;
	int			encryNodeId;
	int			dataTypeNodeId;
	HPVT		hVal;
	char        buf[512]={0};
	H323Channel*	pSendChannel=NULL;

	pSendChannel=m_pMainVideoSendChannel;
	VIDEC_CODEC_TYPE nCodecType = pSendChannel->VideoCodecType();
	HCHAN hsChan=pSendChannel->m_hsChan;
	int nPayloadType=pSendChannel->PayloadType();

	hVal=cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0) return -1;
	nodeId = pvtBuildByPath(hVal, rootId, "request.openLogicalChannel", 0, NULL );
	if(nodeId < 0) return -1;

	//not encry process
	dataNodeId = pvtBuildByPath(hVal, nodeId, "forwardLogicalChannelParameters.dataType", 0, NULL );
	dataTypeNodeId =dataNodeId;
	//encry process
	if(m_bVideoHasRemoteEncryption)
	{
		nodeId = pvtBuildByPath(hVal, dataNodeId, "h235Media.mediaType", 0, NULL );
		sprintf(buf,"h235Media.encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm");
		unsigned char pValue[64]={0};int pValueLen=64;
		if(H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType(m_VideoEncDecAlgoId,pValue,pValueLen)>0)
			encryNodeId = pvtBuildByPath(hVal, dataNodeId, buf, pValueLen, (char*)pValue);
		dataNodeId=nodeId;
	}
	if(dataNodeId < 0) return -1;

	//OLC capability Add
	CallCapability OLCCapability;
	OLCCapability.SetCapabilitisUseType(1);

	if(0)
	{
	OLCCapability.SetMainVideoMaxBitrate(m_nRemoteMainVideoMaxBitrate);
	OLCCapability.SetH264OptionParamMaxMBPS(m_nRemoteMainVideoOLCMaxMBPS);
	OLCCapability.SetH264OptionParamMaxFS(m_nRemoteMainVideoOLCMaxFS);
	OLCCapability.SetH264OptionParamMaxDPB(m_nRemoteMainVideoOLCMaxDPB);
	OLCCapability.SetH264OptionParamMaxBR(m_nRemoteMainVideoOLCMaxBR);

	int nFrameRate=0;
	int bitrate=0;
	m_CallCapability.GetVideoSetOtherParam(2,nCodecType,m_nRemoteMainVideoFormatType,nFrameRate,bitrate);
	if(nFrameRate >0) m_nMainVideoFrameRate=nFrameRate;

	OLCCapability.SetMainVideoFrameRate(m_nMainVideoFrameRate);

	OLCCapability.RemoveAllCapabilitis();
	switch(nCodecType)
	{
	case VIDEC_CODEC_H261:
		OLCCapability.AddH261(m_nMainVideoQCIF,m_nMainVideoCIF);
		break;
	case VIDEC_CODEC_H263:
		OLCCapability.AddH263(m_nMainVideoQCIF,m_nMainVideoCIF,m_nMainVideo4CIF);
		break;
	case VIDEC_CODEC_H263P:
		OLCCapability.AddH263v2(m_nMainVideoQCIF,m_nMainVideoCIF,m_nMainVideo4CIF);
		OLCCapability.SetMainVideoH263OLCOptionParam(nCodecType,m_nRemoteMainVideoFormatType);
		break;
	case VIDEC_CODEC_H263PP:
		OLCCapability.AddH263v3(m_nMainVideoQCIF,m_nMainVideoCIF,m_nMainVideo4CIF);
		OLCCapability.SetMainVideoH263OLCOptionParam(nCodecType,m_nRemoteMainVideoFormatType);
		break;
	case VIDEC_CODEC_H264:
		OLCCapability.AddH264MainVideoCap(nCodecType,m_nMainVideoH264Level);
		break;
	case VIDEC_CODEC_H264_SVC:
		OLCCapability.AddH264MainVideoCap(nCodecType,m_nMainVideoH264Level);
		break;
	case VIDEC_CODEC_MPEG4: //use to call kedacom
		OLCCapability.AddMPEG4(VIDEO_FORMAT_4CIF,768,30);
		break;
	default:
		break;
	}
	error=OLCCapability.BuildCapabilitis(dataNodeId);
	}
	else
	{
		DapCallCapability dapCaps;
		memset(&dapCaps,0,sizeof(DapCallCapability));
		dapCaps.m_nMainVideoCapCount =1;
		dapCaps.m_aMainVideoCap[0].m_VideoCodecType = nCodecType;
		dapCaps.m_aMainVideoCap[0].m_VideoFormatType = m_nRemoteMainVideoFormatType;
		//call xinjiang rmx2000
		if(nCodecType ==VIDEC_CODEC_H264 || nCodecType ==VIDEC_CODEC_H264_SVC)
			CheckSetH264FormatFromProductId(dapCaps.m_aMainVideoCap[0].m_VideoFormatType);

		dapCaps.m_aMainVideoCap[0].m_nVideoBitrate = m_nRemoteMainVideoMaxBitrate;
		dapCaps.m_aMainVideoCap[0].m_nVideoFrameRate = m_nRemoteMainVideoFrame;
		dapCaps.m_aMainVideoCap[0].m_nFormatValue = GetVideoFormatValue(dapCaps.m_aMainVideoCap[0].m_VideoFormatType);
		error =OLCCapability.BuildMediaCapabilitis(dataNodeId,dapCaps);
		//char msg[256]={0};
		//char nameBuf[256]={0};
		//char nameBuf2[256]={0};
		//sprintf(msg,"Video ChannelOpen codec:%s,format:%s,bitrate:%d\n",GetVideoCodecName(nCodecType,nameBuf),GetVideoFormatName(m_nRemoteMainVideoFormatType,nameBuf2),m_nRemoteMainVideoMaxBitrate);
		//OutputDebugString(msg);
	}
	//OLC Open
	error = cmChannelOpenDynamic(hsChan, pvtChild(hVal, dataTypeNodeId), NULL, NULL, FALSE);

	int nCapPrint=H323CallMgr::Instance().GetDAPCapPrintEnable();
	if(nCapPrint>0)
	{
	std::string strContent="";
	OutputPVTNodeContent(strContent ,rootId);
	H323CallMgr::Instance().OutPutCapContent(m_strCallID.c_str(),3,strContent.c_str());
	}

	pvtDelete(hVal,rootId);
	return error;
}

int H323Call::BuildSendDualVideoOLCCapabilitis()
{
	int			nodeId;
	HPST		synTree;
	int			rootId;
	int			error;
	int			dataNodeId;
	int			encryNodeId;
	int			dataTypeNodeId;
	HPVT		hVal;
	char        buf[512]={0};
	H323Channel*	pSendChannel=NULL;

	pSendChannel=m_pSecondVideoSendChannel;
	VIDEC_CODEC_TYPE nCodecType = pSendChannel->VideoCodecType();
	HCHAN hsChan=pSendChannel->m_hsChan;
	int nPayloadType=pSendChannel->PayloadType();

	hVal=cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0) return -1;
	nodeId = pvtBuildByPath(hVal, rootId, "request.openLogicalChannel", 0, NULL );
	if(nodeId < 0) return -1;

	//not encry process
	dataNodeId = pvtBuildByPath(hVal, nodeId, "forwardLogicalChannelParameters.dataType", 0, NULL );
	dataTypeNodeId =dataNodeId;
	//encry process
	if(m_bH239HasRemoteEncryption)
	{
		nodeId = pvtBuildByPath(hVal, dataNodeId, "h235Media.mediaType", 0, NULL );
		sprintf(buf,"h235Media.encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm");
		unsigned char pValue[64]={0};int pValueLen=64;
		if(H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType(m_H239EncDecAlgoId,pValue,pValueLen)>0)
			encryNodeId = pvtBuildByPath(hVal, dataNodeId, buf, pValueLen, (char*)pValue);
		dataNodeId=nodeId;
	}
	if(dataNodeId < 0) return -1;

	//OLC capability Add
	CallCapability OLCCapability;
	OLCCapability.SetCapabilitisUseType(1);

	if(0)
	{
	OLCCapability.SetSecondVideoMaxBitrate(m_nRemoteH239VideoMaxBitrate);
	OLCCapability.SetH264OptionParamMaxMBPS(m_nRemoteDualVideoOLCMaxMBPS);
	OLCCapability.SetH264OptionParamMaxFS(m_nRemoteDualVideoOLCMaxFS);
	OLCCapability.SetH264OptionParamMaxDPB(m_nRemoteDualVideoOLCMaxDPB);
	OLCCapability.SetH264OptionParamMaxBR(m_nRemoteDualVideoOLCMaxBR);

	int nFrameRate=0;
	int bitrate=0;
	m_CallCapability.GetVideoSetOtherParam(3,nCodecType,m_nRemoteH239VideoFormatType,nFrameRate,bitrate);
	if(nFrameRate >0) m_nSecondVideoFrameRate=nFrameRate;

	OLCCapability.SetSecondVideoFrameRate(m_nSecondVideoFrameRate);

	OLCCapability.RemoveAllCapabilitis();
	switch(nCodecType)
	{
	case VIDEC_CODEC_H261:
		OLCCapability.AddH239VideoH261Extend(m_nH239QCIF,m_nH239CIF);
		break;
	case VIDEC_CODEC_H263:
		OLCCapability.AddH239VideoH263Extend(m_nH239QCIF,m_nH239CIF,m_nH2394CIF);
		break;
	case VIDEC_CODEC_H263P:
		OLCCapability.AddH239VideoH263v2Extend(m_nH239QCIF,m_nH239CIF,m_nH2394CIF);
		OLCCapability.SetDualVideoH263OLCOptionParam(nCodecType,m_nRemoteMainVideoFormatType);
		break;
	case VIDEC_CODEC_H263PP:
		OLCCapability.AddH239VideoH263v3Extend(m_nH239QCIF,m_nH239CIF,m_nH2394CIF);
		OLCCapability.SetDualVideoH263OLCOptionParam(nCodecType,m_nRemoteMainVideoFormatType);
		break;
	case VIDEC_CODEC_H264:
		OLCCapability.AddH264SecondVideoCap(nCodecType,m_nH239VideoH264Level);
		break;
	case VIDEC_CODEC_H264_SVC:
		OLCCapability.AddH264SecondVideoCap(nCodecType,m_nH239VideoH264Level);
		break;
	default:
		break;
	}
	error=OLCCapability.BuildCapabilitis(dataNodeId);
	}
	else
	{
		DapCallCapability dapCaps;
		memset(&dapCaps,0,sizeof(DapCallCapability));
		dapCaps.m_nSecondVideoCapCount =1;
		dapCaps.m_aSecondVideoCap[0].m_VideoCodecType = nCodecType;
		dapCaps.m_aSecondVideoCap[0].m_VideoFormatType = m_nRemoteH239VideoFormatType;
		dapCaps.m_aSecondVideoCap[0].m_nVideoBitrate = m_nRemoteH239VideoMaxBitrate;
		dapCaps.m_aSecondVideoCap[0].m_nVideoFrameRate = m_nRemoteSecondVideoFrame;
		dapCaps.m_aSecondVideoCap[0].m_nFormatValue = GetVideoFormatValue(dapCaps.m_aSecondVideoCap[0].m_VideoFormatType);
		error =OLCCapability.BuildMediaCapabilitis(dataNodeId,dapCaps);
		//char msg[256]={0};
		//char nameBuf[256]={0};
		//char nameBuf2[256]={0};
		//sprintf(msg,"DualVideo ChannelOpen codec:%s,format:%s,bitrate:%d\n",GetVideoCodecName(nCodecType,nameBuf),GetVideoFormatName(m_nRemoteH239VideoFormatType,nameBuf2),m_nRemoteH239VideoMaxBitrate);
		//OutputDebugString(msg);
	}

	//OLC Open
	error = cmChannelOpenDynamic(hsChan, pvtChild(hVal, dataTypeNodeId), NULL, NULL, FALSE);


	pvtDelete(hVal,rootId);
	return error;
}

int H323Call::BuildSendAudioOLCCapabilitis()
{
	int			nodeId;
	HPST		synTree;
	int			rootId;
	int			error;
	int			dataNodeId;
	int			encryNodeId;
	int			dataTypeNodeId;
	HPVT		hVal;
	char        buf[512]={0};
	H323Channel*	pSendChannel=NULL;

	pSendChannel=m_pAudioSendChannel;
	X_AUDIO_CODEC_ID nCodecType = pSendChannel->AudioCodecID();
	HCHAN hsChan=pSendChannel->m_hsChan;
	int nPayloadType=pSendChannel->PayloadType();

	hVal=cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0) return -1;
	nodeId = pvtBuildByPath(hVal, rootId, "request.openLogicalChannel", 0, NULL );
	if(nodeId < 0) return -1;

	//not encry process
	dataNodeId = pvtBuildByPath(hVal, nodeId, "forwardLogicalChannelParameters.dataType", 0, NULL );
	dataTypeNodeId =dataNodeId;
	//encry process
	if(m_bAudioHasRemoteEncryption)
	{
		nodeId = pvtBuildByPath(hVal, dataNodeId, "h235Media.mediaType", 0, NULL );
		sprintf(buf,"h235Media.encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm");
		unsigned char pValue[64]={0};int pValueLen=64;
		if(H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType(m_AudioEncDecAlgoId,pValue,pValueLen)>0)
			encryNodeId = pvtBuildByPath(hVal, dataNodeId, buf, pValueLen, (char*)pValue);
		dataNodeId=nodeId;
	}
	if(dataNodeId < 0) return -1;

	//OLC capability Add
	CallCapability OLCCapability;
	OLCCapability.SetCapabilitisUseType(1);

	if(0)
	{
	OLCCapability.RemoveAllCapabilitis();
	switch(nCodecType)
	{
	case X_AUDIO_CODEC_G711A:
		OLCCapability.AddG711a_64K();
		break;
	case X_AUDIO_CODEC_G711U:
		OLCCapability.AddG711u_64K();
		break;
	case X_AUDIO_CODEC_G722_64:
		OLCCapability.AddG722_64K();
		break;
	case X_AUDIO_CODEC_G7221_7_24:
		OLCCapability.AddG7221_24K();
		break;
	case X_AUDIO_CODEC_G7221_7_32:
		OLCCapability.AddG7221_32K();
		break;
	case X_AUDIO_CODEC_G7221_14_24:
		OLCCapability.AddG7221C_24K();
		break;
	case X_AUDIO_CODEC_G7221_14_32:
		OLCCapability.AddG7221C_32K();
		break;
	case X_AUDIO_CODEC_G7221_14_48:
		OLCCapability.AddG7221C_48K();
		break;
	case X_AUDIO_CODEC_G719_32:
		OLCCapability.AddG719_32K();
		break;
	case X_AUDIO_CODEC_G719_48:
		OLCCapability.AddG719_48K();
		break;
	case X_AUDIO_CODEC_G719_64:
		OLCCapability.AddG719_64K();
		break;
	case X_AUDIO_CODEC_G729:
		OLCCapability.SetG729Type(m_nRemoteG729Type);
		OLCCapability.AddG729A();
		break;
	default:
		break;
	}
	error=OLCCapability.BuildCapabilitis(dataNodeId);
	}
	else
	{
	DapCallCapability dapCaps;
	memset(&dapCaps,0,sizeof(DapCallCapability));
	dapCaps.m_nAudioCapCount =1;
	dapCaps.m_aAudioCap[0].m_AudioCodecType = nCodecType;
	if(m_pAudioSendChannel->AudioCodecID() ==X_AUDIO_CODEC_G729 ) OLCCapability.SetG729Type(m_nRemoteG729Type);
	error = OLCCapability.BuildMediaCapabilitis(dataNodeId,dapCaps);
	}

	//OLC Open
	error = cmChannelOpenDynamic(hsChan, pvtChild(hVal, dataTypeNodeId), NULL, NULL, FALSE);
	pvtDelete(hVal,rootId);
	return error;
}

int H323Call::BuildSendDataOLCCapabilitis()
{
	int			nodeId;
	HPST		synTree;
	int			rootId;
	int			error;
	int			dataNodeId;
	int			encryNodeId;
	int			dataTypeNodeId;
	HPVT		hVal;
	char        buf[512]={0};
	H323Channel*	pSendChannel=NULL;

	pSendChannel=m_pH224SendChannel;
	HCHAN hsChan=pSendChannel->m_hsChan;
	int nPayloadType=pSendChannel->PayloadType();

	hVal=cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	if(rootId < 0) return -1;
	nodeId = pvtBuildByPath(hVal, rootId, "request.openLogicalChannel", 0, NULL );
	if(nodeId < 0) return -1;

	//not encry process
	dataNodeId = pvtBuildByPath(hVal, nodeId, "forwardLogicalChannelParameters.dataType", 0, NULL );
	dataTypeNodeId =dataNodeId;
	//encry process
	if(m_bDataHasRemoteEncryption)
	{
		nodeId = pvtBuildByPath(hVal, dataNodeId, "h235Media.mediaType", 0, NULL );
		sprintf(buf,"h235Media.encryptionAuthenticationAndIntegrity.encryptionCapability.1.algorithm");
		unsigned char pValue[64]={0};int pValueLen=64;
		if(H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType(m_DataEncDecAlgoId,pValue,pValueLen)>0)
			encryNodeId = pvtBuildByPath(hVal, dataNodeId, buf, pValueLen, (char*)pValue);
		dataNodeId=nodeId;
	}
	if(dataNodeId < 0) return -1;

	//OLC capability Add
	CallCapability OLCCapability;
	OLCCapability.SetCapabilitisUseType(1);
	if(0)
	{
	OLCCapability.RemoveAllCapabilitis();
	OLCCapability.AddFECCData();
	error=OLCCapability.BuildCapabilitis(dataNodeId);
	}
	else
	{
		DapCallCapability dapCaps;
		memset(&dapCaps,0,sizeof(DapCallCapability));
		dapCaps.m_nDataCapCount =1;
		dapCaps.m_DataCap[0].m_DataCodecID = (DATA_TYPE_ID)DATA_CODEC_H224;
		error =OLCCapability.BuildMediaCapabilitis(dataNodeId,dapCaps);
	}

	//OLC Open
	error = cmChannelOpenDynamic(hsChan, pvtChild(hVal, dataTypeNodeId), NULL, NULL, FALSE);
	pvtDelete(hVal,rootId);
	return error;
}

VIDEO_FORMAT_TYPE H323Call::GetMgrMainVideoFormatType(void)
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	if(m_nMainVideoFTH264 > 0) m_nMainVideoFormatType=m_nMainVideoFTH264;
	else if(m_nMainVideoFTH264HP > 0) m_nMainVideoFormatType=m_nMainVideoFTH264HP;
	else if(m_nMainVideoFTH263 > 0) m_nMainVideoFormatType=m_nMainVideoFTH263;

	return m_nMainVideoFormatType;
#else
	return H323CallMgr::Instance().GetMainVideoFormatType();
#endif

}

VIDEO_FORMAT_TYPE H323Call::GetMgrSecondVideoFormatType(void)
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	if(m_nSecondVideoFTH264 > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH264;
	else if(m_nSecondVideoFTH264HP > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH264HP;
	else if(m_nSecondVideoFTH263 > 0) m_nSecondVideoFormatType=m_nSecondVideoFTH263;

	return m_nSecondVideoFormatType;
#else
	return H323CallMgr::Instance().GetSecondVideoFormatType();
#endif

}

int H323Call::GetH264ProfileLevel(HPVT pvtTree,int nodeId,char * subBuf,int & nProfile,int & nLevel)
{
	int nValue1=0;
	int nValue2=0;
	int error=-1;
	int nCount=0;
	char buf[1024]={0};
	int len=strlen(subBuf);
	nProfile=0;
	nLevel=0;
	while(1)
	{
		nValue1=0;
		nValue2=0;
		nCount++;
		sprintf(buf,"%scollapsing.%u.parameterIdentifier.standard",subBuf,nCount);
		error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue1, NULL);
		if(error<0) break;
		else
		{
			if(nValue1==41)
			{
				sprintf(buf,"%scollapsing.%u.parameterValue.booleanArray",subBuf,nCount);
				error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue2, NULL);
				if(error>=0)
				{
					nProfile=nValue2;
				}
			}
		}
		if(error>=0 && nValue1==41 ) break;
	}

	nCount=0;
	while(1)
	{
		nValue1=0;
		nValue2=0;
		nCount++;
		sprintf(buf,"%scollapsing.%u.parameterIdentifier.standard",subBuf,nCount);
		error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue1, NULL);
		if(error<0) break;
		else
		{
			if(nValue1==42)
			{
				sprintf(buf,"%scollapsing.%u.parameterValue.unsignedMin",subBuf,nCount);
				error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue2, NULL);
				if(error>=0)
				{
					nLevel=nValue2;
				}
			}
		}
		if(error>=0 && nValue1==42 ) break;
	}
	if(nProfile>0 && nLevel>0) return 0;
	else return -1;
}

int H323Call::GetH264CapOptionParam(HPVT pvtTree,int nodeId,int& nMaxFS,int& nMBPS,char * subBuf,int type) //0 is main video ,1 is dual video
{
	int nValue1=0;
	int nValue2=0;
	int error=-1;
	int nCount=0;
	char buf[1024]={0};
	int len=strlen(subBuf);
	while(true)
	{
		nValue1=0;
		nValue2=0;
		nCount++;
		sprintf(buf,"%scollapsing.%u.parameterIdentifier.standard",subBuf,nCount);
		error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue1, NULL);
		if(error<0) break;
		else
		{
			sprintf(buf,"%scollapsing.%u.parameterValue.unsignedMin",subBuf,nCount);
			error=pvtGetByPath(pvtTree,nodeId,buf, NULL, &nValue2, NULL);
			switch(nValue1)
			{
			case 3:
				nMBPS=nValue2;
				if(error>=0 && type==0 ) m_nRemoteMainVideoOLCMaxMBPS=nValue2;
				if(error>=0 && type==1 ) m_nRemoteDualVideoOLCMaxMBPS=nValue2;
				break;
			case 4:
				nMaxFS =nValue2;
				if(error>=0 && type==0 ) m_nRemoteMainVideoOLCMaxFS=nValue2;
				if(error>=0 && type==1 ) m_nRemoteDualVideoOLCMaxFS=nValue2;
				break;
			case 5:
				if(error>=0 && type==0 ) m_nRemoteMainVideoOLCMaxDPB=nValue2;
				if(error>=0 && type==1 ) m_nRemoteDualVideoOLCMaxDPB=nValue2;
				break;
			case 6:
				if(error>=0 && type==0 ) m_nRemoteMainVideoOLCMaxBR=nValue2;
				if(error>=0 && type==1 )  m_nRemoteDualVideoOLCMaxBR=nValue2;
				break;
			default:
				break;
			}
		}
	}
	return 0;
}

int H323Call::SendCallH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	H323CallMessage CallMessage(*this);
	if(nPacketLen <=0) nPacketLen=strlen(cszPacket);
	CallMessage.SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);
	return 0;
}

void H323Call::RecvChannelMessage(H323Channel * pH323Channel,int nodeID)
{
	HPVT hVal;
	char temp[256];
	int error=-1;
	int lenData =0;
	RvBool bIsString;

	bool bActive=false;
	bool bInActive=false;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	sprintf(temp, "indication.miscellaneousIndication.type.logicalChannelActive");
	error=pvtGetByPath(hVal, nodeID, temp, NULL, &lenData, &bIsString);
	if(error >= 0) bActive=true;
	sprintf(temp, "indication.miscellaneousIndication.type.logicalChannelInactive");
	error=pvtGetByPath(hVal, nodeID, temp, NULL, &lenData, &bIsString);
	if(error >= 0) bInActive=true;

	if(bActive || bInActive )
	{
		std::string strMediaName="NOMEDIA";
		if(pH323Channel == m_pAudioRecvChannel)
		{
			strMediaName="AUDIO";
		}
		else if(pH323Channel == m_pAudioSendChannel)
		{
			strMediaName="AUDIO";
		}
		else if(pH323Channel == m_pMainVideoRecvChannel)
		{
			strMediaName="VIDEO";
		}
		else if(pH323Channel == m_pMainVideoSendChannel)
		{
			strMediaName="VIDEO";
		}
		else if(pH323Channel == m_pSecondVideoRecvChannel)
		{
			strMediaName="DUALVIDEO";
		}
		else if(pH323Channel == m_pSecondVideoSendChannel)
		{
			strMediaName="DUALVIDEO";
		}
		else if(pH323Channel == m_pH224RecvChannel)
		{
			strMediaName="DATA";
		}
		else if(pH323Channel == m_pH224SendChannel)
		{
			strMediaName="DATA";
		}

		RecvChannelInORActiveMessage(pH323Channel,bActive,bInActive);

		H323CallMessage CallMessage(*this);
		if(bActive)
			CallMessage.MediaChannelActiveStateMessage(strMediaName);
		if(bInActive)
			CallMessage.MediaChannelInActiveStateMessage(strMediaName);
	}
}

void H323Call::RecvChannelInORActiveMessage(H323Channel * pH323Channel,bool bActive,bool bInActive)
{
	if(pH323Channel == m_pMainVideoSendChannel)
	{
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		if(bActive) OutputDebugString("RecvChannelActiveMessage\n");
		if(bInActive) OutputDebugString("RecvChannelInActiveMessage\n");

		//call polycom rmx4000 need ,the send dualvideo is only send indication
		int nStatus=GetH239TokenRequestStatus();
		if(bActive && nStatus <=0)
		{
			SetH239TokenRequestStatus(1);
			if(GetH239TokenSendReqStatus() >0)
			{
				SetH239TokenSendReqStatus(0);
			}
			else
			{
				//type为1 时发响应,默认发,  0时不发(如收到指示后就不需发响应) call polycom rmx4000 need
				/*if(HasAVCONTransport() ==false) */
				OnRecvGenericRequest(pH323Channel, 0, 0,0,0);
			}
		}
		if(bInActive && nStatus >0 )
		{
			OutputDebugString("RecvChannelInActiveMessage\n");
			SetH239TokenRequestStatus(0);
		}
	}
}

void H323Call::RecvH245ControlMessage(RvPvtNodeId message,cmH245MessageType messageType)
{
	H323CallMessage CallMessage(*this);
	CallMessage.RecvH245ControlMessage(message,messageType);
}

void H323Call::OnRecvH245ControlMessage(H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	char buf[1024]={0};
	sprintf(buf,"\nType:%d,packet:%s,packet len:%d\n",nMsgType,cszPacket,nPacketLen);
	//OutputDebugString(buf);
	CallMgr::Instance().OnDAP_RecvCallH245ControlMessage(m_strCallID.c_str(),nMsgType,cszPacket,nPacketLen);
}

void H323Call::RecvH245TerminalNumberAssign(int mcuNumber, int terminalNumber)
{
	H323CallMessage CallMessage(*this);
	CallMessage.RecvTerminalNumberAssign(mcuNumber,terminalNumber);
}

void H323Call::RecvGenericMessage(cmH245MessageType messageType,int nBitrate, int terLabel, int chID, int symBreaking,int response,int respValue)
{
	H323CallMessage CallMessage(*this);
	CallMessage.RecvGenericMessage(messageType,nBitrate, terLabel, chID, symBreaking,response,respValue);
}

void H323Call::RecvFlowControlCommand(H323Channel* pH323Channel,HCHAN hsChan,RvUint32 rate)
{
	std::string strMediaName="NOMEDIA";

	if(pH323Channel == m_pAudioRecvChannel)
	{
		strMediaName="AUDIO";
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		strMediaName="AUDIO";
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		strMediaName="VIDEO";
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		strMediaName="VIDEO";
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		strMediaName="DUALVIDEO";
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		strMediaName="DUALVIDEO";
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		strMediaName="DATA";
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		strMediaName="DATA";
	}

	//A7 add 如内部处理 则不再流向H323CallMessage
	if(0)
	{
		H323CallMessage CallMessage(*this);
		CallMessage.RecvFlowControlCommand(strMediaName,rate);
	}
	else OnFlowControlCommand(pH323Channel,rate);

}

int H323Call::GetH323ChannelLogicalNumber(int type,std::string & strMediaName)
{
	int nLogicalNum=0;
	//NOMEDIA AUDIO VIDEO DUALVIDEO DATA
	if(type==0)
	{
		if(strMediaName.compare("AUDIO") ==0 && m_pAudioSendChannel)
			nLogicalNum=cmChannelGetNumber(m_pAudioSendChannel->m_hsChan);
		if(strMediaName.compare("VIDEO") ==0 && m_pMainVideoSendChannel)
			nLogicalNum=cmChannelGetNumber(m_pMainVideoSendChannel->m_hsChan);
		if(strMediaName.compare("DUALVIDEO") ==0 && m_pSecondVideoSendChannel)
			nLogicalNum=cmChannelGetNumber(m_pSecondVideoSendChannel->m_hsChan);
		if(strMediaName.compare("DATA") ==0 && m_pH224SendChannel)
			nLogicalNum=cmChannelGetNumber(m_pH224SendChannel->m_hsChan);
	}
	else 
	{
		if(strMediaName.compare("AUDIO") ==0 && m_pAudioRecvChannel)
			nLogicalNum=cmChannelGetNumber(m_pAudioRecvChannel->m_hsChan);
		if(strMediaName.compare("VIDEO") ==0 && m_pMainVideoRecvChannel)
			nLogicalNum=cmChannelGetNumber(m_pMainVideoRecvChannel->m_hsChan);
		if(strMediaName.compare("DUALVIDEO") ==0 && m_pSecondVideoRecvChannel)
			nLogicalNum=cmChannelGetNumber(m_pSecondVideoRecvChannel->m_hsChan);
		if(strMediaName.compare("DATA") ==0 && m_pH224RecvChannel)
			nLogicalNum=cmChannelGetNumber(m_pH224RecvChannel->m_hsChan);
	}

	return nLogicalNum;
}

void H323Call::GetH323ChannelLogicalNumMediaName(int type,int nLogicalNum,std::string &strMediaName)
{
	int nGetLogicalNumAudio=0,nGetLogicalNumVideo=0,nGetLogicalNumDualVideo=0,nGetLogicalNumH224=0;

	if(type==0)
	{
		if(m_pAudioSendChannel) nGetLogicalNumAudio=cmChannelGetNumber(m_pAudioSendChannel->m_hsChan);
		if(m_pMainVideoSendChannel) nGetLogicalNumVideo=cmChannelGetNumber(m_pMainVideoSendChannel->m_hsChan);
		if(m_pSecondVideoSendChannel) nGetLogicalNumDualVideo=cmChannelGetNumber(m_pSecondVideoSendChannel->m_hsChan);
		if(m_pH224SendChannel) nGetLogicalNumH224=cmChannelGetNumber(m_pH224SendChannel->m_hsChan);
	}
	else
	{
		if(m_pAudioRecvChannel) nGetLogicalNumAudio=cmChannelGetNumber(m_pAudioRecvChannel->m_hsChan);
		if(m_pMainVideoRecvChannel) nGetLogicalNumVideo=cmChannelGetNumber(m_pMainVideoRecvChannel->m_hsChan);
		if(m_pSecondVideoRecvChannel) nGetLogicalNumDualVideo=cmChannelGetNumber(m_pSecondVideoRecvChannel->m_hsChan);
		if(m_pH224RecvChannel) nGetLogicalNumH224=cmChannelGetNumber(m_pH224RecvChannel->m_hsChan);
	}

	if(nLogicalNum == nGetLogicalNumAudio)
	{
		strMediaName="AUDIO";
	}
	else if(nLogicalNum == nGetLogicalNumVideo)
	{
		strMediaName="VIDEO";
	}
	else if(nLogicalNum == nGetLogicalNumDualVideo)
	{
		strMediaName="DUALVIDEO";
	}
	else if(nLogicalNum == nGetLogicalNumH224)
	{
		strMediaName="DATA";
	}
}

void H323Call::OnAdapCallRecvChannelOpen(H323Channel * pH323Channel)
{
	ChannelMediaType	nMediaType=pH323Channel->m_nMediaType;
	int nPayload=0,nWidth=0,nHeight=0,nLevel=0;
	nPayload=pH323Channel->PayloadType();
	switch(nMediaType)
	{
	case Enum_Channel_Media_Audio:
		H323CallMgr::Instance().OnDAP_CallAudioRecvChannelOpen(m_strCallID.c_str(),pH323Channel->AudioCodecID(),nPayload);
		break;
	case Enum_Channel_Media_Video:
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallMainVideoRecvChannelOpen(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),nPayload,nWidth,nHeight,nLevel);
		break;
	case Enum_Channel_Media_DualVideo:
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelOpen(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),nPayload,nWidth,nHeight,nLevel);
		break;
	case Enum_Channel_Media_H224:
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallFECCRecvChannelOpen(m_strCallID.c_str(),nPayload);
		break;
	default:
		break;
	}
}

void H323Call::OnAdapCallChannelConnected(H323Channel * pH323Channel)
{
	ChannelMediaType	nMediaType=pH323Channel->m_nMediaType;
	int nPayload=0,nWidth=0,nHeight=0,nLevel=0;
	unsigned long ip=0;
	int port=0;
	nPayload=pH323Channel->PayloadType();
	ip=pH323Channel->m_ulRTPLocalIP;
	port=pH323Channel->m_nLocalRTPPort;

	if(pH323Channel == m_pAudioRecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected(m_strCallID.c_str(),pH323Channel->AudioCodecID(),nPayload,ip,port);
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallAudioSendChannelConnected((HDAPAPPCALL)this,m_strCallID.c_str(),pH323Channel->AudioCodecID(),nPayload,ip,port);
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),nPayload,ip,port,nWidth,nHeight,nLevel);

		//RequestMainVideoKeyFrame();
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected((HDAPAPPCALL)this,m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),nPayload,ip,port,nWidth,nHeight,nLevel);
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),nPayload,ip,port,nWidth,nHeight,nLevel);
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		pH323Channel->GetVideoFormatType(nWidth,nHeight);
		nLevel=pH323Channel->H264Level();
		H323CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected((HDAPAPPCALL)this,m_strCallID.c_str(),pH323Channel->VideoCodecType(),pH323Channel->MaxBitrate(),ip,port,nWidth,nHeight,nPayload,nLevel);
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallFECCRecvChannelConnected(m_strCallID.c_str(),ip,port,nPayload);
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallFECCSendChannelConnected((HDAPAPPCALL)this,m_strCallID.c_str(),ip,port,nPayload);
	}

}

void H323Call::OnAdapCallChannelDisConnected(H323Channel * pH323Channel)
{
	LogFile(NULL,NULL,"","","OnAdapCallChannelDisConnected         111");

	if(pH323Channel == m_pAudioRecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallAudioRecvChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallAudioSendChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallMainVideoRecvChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallMainVideoSendChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallSecondVideoSendChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		H323CallMgr::Instance().OnDAP_CallFECCRecvChannelDestroyed(m_strCallID.c_str());
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		H323CallMgr::Instance().OnDAP_CallFECCSendChannelDestroyed(m_strCallID.c_str());
	}
	//RemoveChannel(pH323Channel);
	LogFile(NULL,NULL,"","","OnAdapCallChannelDisConnected         222");

}

//A7 add 
void H323Call::OnA7ChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(pH323Channel == m_pAudioRecvChannel || pH323Channel == m_pAudioSendChannel)
	{
		OnRecvdRemoteAudioRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pMainVideoRecvChannel || pH323Channel == m_pMainVideoSendChannel)
	{
		OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
		StatisticsMainVideo(usSequence);
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel || pH323Channel == m_pSecondVideoSendChannel)
	{
		OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
		StatisticsSecondVideo(usSequence);
	}
	else if(pH323Channel == m_pH224RecvChannel || pH323Channel == m_pH224SendChannel)
	{
		m_pH224RecvChannel->OnRecvAnnexQDataProc(pRTPPacketData, nRTPPacketLen);
	}
}

void H323Call::OnAdapChannelRecvdRTPPacket(H323Channel*pH323Channel,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	if(pH323Channel == m_pAudioRecvChannel || pH323Channel == m_pAudioSendChannel)
	{
		H323CallMgr::Instance().OnDAP_RecvRemoteAudioRTPPacket(m_strCallID.c_str(),pH323Channel->AudioCodecID(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pMainVideoRecvChannel || pH323Channel == m_pMainVideoSendChannel)
	{
		H323CallMgr::Instance().OnDAP_RecvRemoteMainVideoRTPPacket(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel || pH323Channel == m_pSecondVideoSendChannel)
	{
		H323CallMgr::Instance().OnDAP_RecvRemoteSecondVideoRTPPacket(m_strCallID.c_str(),pH323Channel->VideoCodecType(),pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	}
	else if(pH323Channel == m_pH224RecvChannel || pH323Channel == m_pH224SendChannel)
	{
		m_pH224RecvChannel->OnRecvAnnexQDataProc(pRTPPacketData, nRTPPacketLen);
	}
}

void H323Call::OnRemoteCameraControl(H323Channel * pH323Channel,std::string strAction,int nValue)
{
	H323CallMessage CallMessage(*this);
	CallMessage.RecvRemoteCameraControl(strAction,nValue);
}

int H323Call::SendCameraControl(std::string strAction,int nValue)
{
	//SourceSelect SourceSwitched PresetStore PresetActive 
	if(strAction.compare("SourceSelect") ==0 )
		RemoteSourceSelect(nValue);
	else if(strAction.compare("SourceSwitched") ==0 )
		RemoteSourceSwitched(nValue);
	else if(strAction.compare("PresetStore") ==0 )
		RemoteCameraPresetStore(nValue);
	else if(strAction.compare("PresetActive") ==0 )
		RemoteCameraPresetActive(nValue);

	return 0;
}

std::string&H323Call::DisconnectReason(void)
{
	return m_strDisconnectReason;
}

int H323Call::GetCallTPKTTransportAddr(char * szAddr,int &nPort,int &nLen,cmTpktChanHandleType type)
{
	HPROTCONN hConn = cmGetTpktChanHandle(m_hsCall,type); 
	if(!hConn)
	{
		return -1;
	}
	cmTransportAddress tr;
	if(cmProtocolGetRemoteAddress(H323CallMgr::Instance().m_hApp, hConn, &tr) < 0)
	{
		return -1;
	}

	if(tr.type != cmTransportTypeIP)
	{
		return -1;
	}
	char * szAddrIP=NULL;
	struct in_addr		addr;
	addr.S_un.S_addr	=tr.ip;
	szAddrIP=inet_ntoa(addr);
	if( strlen(szAddrIP) <nLen ) strcpy(szAddr,szAddrIP);
	nPort=tr.port;
	return 0;
}

int H323Call::GetCallQ931TransportAddr(char * szAddr,int &nPort,int &nLen)
{
	return GetCallTPKTTransportAddr(szAddr,nPort,nLen,cmQ931TpktChannel);
}

int H323Call::GetCallH245TransportAddr(char * szAddr,int &nPort,int &nLen)
{
	return GetCallTPKTTransportAddr(szAddr,nPort,nLen,cmH245TpktChannel);
}

int H323Call::SendMediaChannelActive(std::string strMediaName)
{
	if(strMediaName.compare("AUDIO") ==0 )
		if(m_pAudioSendChannel) m_pAudioSendChannel->SendChannelOn();
	if(strMediaName.compare("VIDEO") ==0 )
		if(m_pMainVideoSendChannel) m_pMainVideoSendChannel->SendChannelOn();
	if(strMediaName.compare("DUALVIDEO") ==0 )
		if(m_pSecondVideoSendChannel) m_pSecondVideoSendChannel->SendChannelOn();
	if(strMediaName.compare("DATA") ==0 )
		if(m_pH224SendChannel) m_pH224SendChannel->SendChannelOn();

	return 0;
}

int H323Call::SendMediaChannelInActive(std::string strMediaName)
{
	if(strMediaName.compare("AUDIO") ==0 )
		if(m_pAudioSendChannel) m_pAudioSendChannel->SendChannelOff();
	if(strMediaName.compare("VIDEO") ==0 )
		if(m_pMainVideoSendChannel) m_pMainVideoSendChannel->SendChannelOff();
	if(strMediaName.compare("DUALVIDEO") ==0 )
		if(m_pSecondVideoSendChannel) m_pSecondVideoSendChannel->SendChannelOff();
	if(strMediaName.compare("DATA") ==0 )
		if(m_pH224SendChannel) m_pH224SendChannel->SendChannelOff();

	return 0;
}

int H323Call::CloseDualVideoOutChannel()
{
	if(m_pSecondVideoSendChannel) m_pSecondVideoSendChannel->CloseChannel();
	return 0;
}

int H323Call::CloseFECCOutChannel()
{
	if(m_pH224SendChannel) m_pH224SendChannel->CloseChannel();
	return 0;
}

int H323Call::AnswerMediaRecvChannel(int type) //1 audio 2 video 3 H239 4 Data
{
	if(type==1)
	{
		if(m_pAudioRecvChannel) cmChannelAnswer(m_pAudioRecvChannel->GetChannelHsChan());
	}
	else if(type==2)
	{
		if(m_pMainVideoRecvChannel) cmChannelAnswer(m_pMainVideoRecvChannel->GetChannelHsChan());
	}
	else if(type==3)
	{
		if(m_pSecondVideoRecvChannel) cmChannelAnswer(m_pSecondVideoRecvChannel->GetChannelHsChan());
	}
	else if(type==4)
	{
		if(m_pH224RecvChannel) cmChannelAnswer(m_pH224RecvChannel->GetChannelHsChan());
	}

	return 0;
}

int H323Call::RejectMediaRecvChannel(int type,cmRejectLcnReason reason) //1 audio 2 video 3 H239 4 Data
{
	if(type==1)
	{
		if(m_pAudioRecvChannel) cmChannelRejectReason(m_pAudioRecvChannel->GetChannelHsChan(),reason);
	}
	else if(type==2)
	{
		if(m_pMainVideoRecvChannel) cmChannelRejectReason(m_pMainVideoRecvChannel->GetChannelHsChan(),reason);
	}
	else if(type==3)
	{
		if(m_pSecondVideoRecvChannel) cmChannelRejectReason(m_pSecondVideoRecvChannel->GetChannelHsChan(),reason);
	}
	else if(type==4)
	{
		if(m_pH224RecvChannel) cmChannelRejectReason(m_pH224RecvChannel->GetChannelHsChan(),reason);
	}

	return 0;
}


void H323Call::H239VideoOutChannelInvSIDCntAdd()
{
	//audio 1 video 2 H224 3 dualvideo 4
	if(m_nInvalidSIDOpenCount >0 && m_nInvalidSIDOpenCount <4) m_nInvalidSIDOpenCount=4;
	m_nInvalidSIDOpenCount++;
}

int H323Call::GetH239VideoOutChannelInvSIDCnt()
{
	return m_nInvalidSIDOpenCount;
}

void H323Call::AcceptCallSetCapabilities(ConfInfo *pConfInfo)
{
	if (pConfInfo != NULL)
	{
		memcpy(&m_ConfInfo,pConfInfo,sizeof(ConfInfo));

		if(m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount >0) m_nSupportH239=1;

		ConfCapabilitisDefault();
		CopyVideoCapOtherParam(pConfInfo,1,1);
		m_nH235MediaUse=pConfInfo->m_nH235MediaUse;
		m_nH235AuthUse=pConfInfo->m_nH235AuthUse;
		m_nUseAlogrithmId=pConfInfo->m_nUseAlogrithmId;
	}
}

void H323Call::OnRecvRoundTripResponse(HCALL hsCall, int delay)
{
	m_nRoundTripSendCount--;
}

bool H323Call::CheckTimestamp()
{
	unsigned long nTimestamp=XGetTimestamp();

	//send genericIndication
	if(m_bHasRemoteH239Video && m_nSecondVideoDir ==IMXEC_CALL_DIR_OUT)
	{
		if (m_nLastSendGenIndTimestamp > nTimestamp)
		{
			m_nLastSendGenIndTimestamp =nTimestamp;
		}
		else if (nTimestamp - m_nLastSendGenIndTimestamp>= 10000) //10 seconds
		{
			m_nLastSendGenIndTimestamp=nTimestamp;
			SendGenericIndication();
		}
	}

	//Check media secret update
	if(GetMgrH235MediaUse() >0 && m_hSecCall!=NULL)
	{
		if (m_nLastSendSecretUpdateTS > nTimestamp)
		{
			m_nLastSendSecretUpdateTS =nTimestamp;
		}
		else if (nTimestamp - m_nLastSendSecretUpdateTS>= 1000 * 3600 *10) //10 hours
		{
			m_nLastSendGenIndTimestamp=nTimestamp;
			//int msdStatus = cmCallMasterSlaveStatus(m_hsCall); 
			//if (msdStatus == 2)
			//{
			//	SendEncryptionUpdateCommand(m_pAudioSendChannel);
			//	SendEncryptionUpdateCommand(m_pMainVideoSendChannel);
			//	SendEncryptionUpdateCommand(m_pH224SendChannel);
			//}
			//if (msdStatus != 2)
			//{
			//	SendEncryptionUpdateRequest(m_pAudioSendChannel);
			//	SendEncryptionUpdateRequest(m_pMainVideoSendChannel);
			//	SendEncryptionUpdateRequest(m_pH224SendChannel);
			//}
		}
	}
	return false;
}

void H323Call::SendRoundTripDelay(int delay) //delay seconds
{
	if(delay==0) delay=32;
	m_nRoundTripSendCount++;
	cmCallRoundTripDelay(m_hsCall,delay);
}

void H323Call::SendEncryptionUpdateRequest(H323Channel * pH323Channel) //EncryptionUpdateRequest Command
{
	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId,nodeId2,encUpdateId;
	int logicalNum=cmChannelGetNumber(pH323Channel->GetChannelHsChan());
	int synchFlag= 0;
	char slaveMasterPath[128]={0};
	int msdStatus = cmCallMasterSlaveStatus(m_hsCall); 
	if (msdStatus == 2) return;
	if(pH323Channel->Direction() == cmCapReceive) sprintf(slaveMasterPath,"direction.masterToSlave");
	else if(pH323Channel->Direction() == cmCapTransmit) sprintf(slaveMasterPath,"direction.slaveToMaster");
	else sprintf(slaveMasterPath,"direction.masterToSlave");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#endif

	pH323Channel->ChangeChannelPayloadType();
	synchFlag=pH323Channel->GetChannelPayloadType();

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nodeId = pvtBuildByPath(hVal, rootId, "command.miscellaneousCommand", 0, NULL );
	nodeId2 = pvtBuildByPath(hVal, nodeId, "logicalChannelNumber", logicalNum, NULL );
	encUpdateId = pvtBuildByPath(hVal, nodeId, "type.encryptionUpdateRequest", 0, NULL );
	nodeId2 = pvtBuildByPath(hVal, encUpdateId, "keyProtectionMethod.secureChannel", RV_FALSE, NULL );
	nodeId2 = pvtBuildByPath(hVal, encUpdateId, "keyProtectionMethod.sharedSecret", RV_TRUE, NULL );
	nodeId2 = pvtBuildByPath(hVal, encUpdateId, "keyProtectionMethod.certProtectedKey", RV_FALSE, NULL );
	nodeId2 = pvtBuildByPath(hVal, encUpdateId, "synchFlag", synchFlag, NULL );
	nodeId2 = pvtBuildByPath(hVal, nodeId, slaveMasterPath, 0, NULL );

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
		return;
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
	}
	pvtDelete(hVal,rootId);
}

void H323Call::SendEncryptionUpdateCommand(H323Channel * pH323Channel,int synchFlag) //EncryptionUpdate Command
{
	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId,nodeId2,encUpdateId;
	int logicalNum=cmChannelGetNumber(pH323Channel->GetChannelHsChan());
	int h235Version=3;
	char h235KeyEncode[256]={0};
	int  h235KeyEncLen=0;
	int encNodeId=0;
	int msdStatus = cmCallMasterSlaveStatus(m_hsCall); 
	if (msdStatus != 2) return;
	char slaveMasterPath[128]={0};
	if(pH323Channel->Direction() == cmCapReceive) sprintf(slaveMasterPath,"direction.slaveToMaster");
	else if(pH323Channel->Direction() == cmCapTransmit) sprintf(slaveMasterPath,"direction.masterToSlave");
	else sprintf(slaveMasterPath,"direction.masterToSlave");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#endif

	if (m_hSecCall == NULL) return;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	if(synchFlag == 0)
	{
		pH323Channel->ChangeChannelPayloadType();
		synchFlag=pH323Channel->GetChannelPayloadType();
	}

	unsigned char output[256]={0};
	unsigned char vector[128]={0};
	RvUint8 sharedSecret[64]={0};
	int nKeyLen=0;
	int nKeySecLen=0;
	int nCopyLen=H323CallMgr::Instance().m_h235Mgr.m_nShareSecretLen;
	unsigned int nAlgorithmId=pH323Channel->AlgorithmId();

	if(nAlgorithmId == ALGORITHM_DES_CBC) { nKeyLen=7; nKeySecLen=8; }
	else if(nAlgorithmId == ALGORITHM_TRIPLE_DES_CBC ) { nKeyLen=21; nKeySecLen=24; }
	else if(nAlgorithmId == ALGORITHM_AES_CBC ) { nKeyLen=16; nKeySecLen=16; }
	else if(nAlgorithmId == ALGORITHM_AES256_CBC ) { nKeyLen=32; nKeySecLen=32; }
	else return;

	char * buf=pH323Channel->UpdateChannelSpecifiedKey();
	memset(vector,0,sizeof(vector));
	memcpy(vector,buf,nKeyLen);
	pH323Channel->SetChannelUpdateSharedSecret((unsigned char *)buf,nKeyLen);
	//根据此通道用到的算法 取最后多少位
	memset(sharedSecret,0,sizeof(sharedSecret));
	memcpy(sharedSecret,m_strSharedSecret+nCopyLen-nKeyLen,nKeyLen);

	if(H323CallMgr::Instance().m_h235Mgr.EncryptData(sharedSecret,nKeyLen,vector,nKeySecLen,output) <=0) return;

	unsigned char OID[128] = {0};
	int OIDLen=128;
	int len=H323CallMgr::Instance().GetEncDecAlgorithmOIDFromType((EncDecAlgorithmID)nAlgorithmId,OID,OIDLen) ;
	if(len<= 0) return;

	//create the key containing the encrypted vector
	encNodeId = pvtAddRoot(hVal,cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "H235Key"), 0, NULL);
	int err=0;
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.paramS",0,NULL);
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.algorithmOID",len, (const char *)OID);
	err=pvtBuildByPath(hVal,encNodeId,"secureSharedSecret.encryptedSessionKey",nKeySecLen,(const char *)output);
	//encode to buffer
	err = cmEmEncode(hVal, encNodeId,(RvUint8 *)h235KeyEncode, (int)sizeof(h235KeyEncode), &h235KeyEncLen);
	pvtDelete(hVal, encNodeId);
	if (err < 0) return;


	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nodeId = pvtBuildByPath(hVal, rootId, "command.miscellaneousCommand", 0, NULL );
	nodeId2 = pvtBuildByPath(hVal, nodeId, "logicalChannelNumber", logicalNum, NULL );
	if(h235Version < 3)
	{
		nodeId2 = pvtBuildByPath(hVal, nodeId, "type.encryptionUpdate.synchFlag", synchFlag, NULL );
		nodeId2 = pvtBuildByPath(hVal, nodeId, "type.encryptionUpdate.h235Key", h235KeyEncLen, h235KeyEncode);
	}
	else if(h235Version ==3)
	{
		nodeId2 = pvtBuildByPath(hVal, nodeId, slaveMasterPath, 0, NULL );
		encUpdateId = pvtBuildByPath(hVal, nodeId, "type.encryptionUpdateCommand", 0, NULL );
		nodeId2 = pvtBuildByPath(hVal, encUpdateId, "encryptionSync.synchFlag", synchFlag, NULL );
		nodeId2 = pvtBuildByPath(hVal, encUpdateId, "encryptionSync.h235Key", h235KeyEncLen, h235KeyEncode);
	}

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
		pvtDelete(hVal,rootId);
		return;
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
		pvtDelete(hVal,rootId);
		return;
	}
	pvtDelete(hVal,rootId);

	pH323Channel->AddChannelEncryption();
}


void H323Call::OnRecvEncryptionUpdateRequest(H323Channel * pH323Channel,HCHAN hsChan,int messageId)
{
	HPVT hVal;
	int nodeData =0;
	RvBool  bIsString=RV_FALSE;
	int syncFlag=0;
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	nodeData= pvtGetByPath( hVal, messageId, "synchFlag", NULL, (int *)&syncFlag, &bIsString);
	if(nodeData >= 0)
	{
		pH323Channel->SetChannelUpdatePayloadType(syncFlag,1);
		SendEncryptionUpdateCommand(pH323Channel,syncFlag);
	}

}

void H323Call::OnRecvEncryptionUpdateCommand(H323Channel * pH323Channel,HCHAN hsChan,int messageId)
{
	HPVT hVal;
	int nodeData =0;
	int encLen=0;
	int encNodeId=0;
	int status=0;
	int h235Version=3;
	unsigned char h235KeyEncode[256]={0};
	int nodeId=0;
	int  lenData=-1;
	RvBool  bIsString=RV_FALSE;
	int syncFlag=0;
	unsigned int paramsIV =0;
	unsigned char strData[256]={0};
	EncDecAlgorithmID nAlgorithmId;
	int nRet=-1;

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	nodeData= pvtGetByPath( hVal, messageId, "encryptionSync.synchFlag", NULL, (int *)&syncFlag, &bIsString);
	if(nodeData >= 0) pH323Channel->SetChannelUpdatePayloadType(syncFlag,1);

	nodeId= pvtGetNodeIdByPath(hVal, messageId, "encryptionSync.h235Key");
	encLen = pvtGetString(hVal, nodeId,(RvInt32)sizeof(h235KeyEncode) -1, (RvChar*)h235KeyEncode);
	encNodeId = pvtAddRoot(hVal, cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "H235Key"), 0, NULL);
	status = cmEmDecode(hVal, encNodeId, h235KeyEncode, encLen, &encLen);
	if (status < 0)
	{
		pvtDelete(hVal, encNodeId);
		return;
	}

	nRet=pvtGetByPath(hVal,encNodeId,"secureSharedSecret.algorithmOID",NULL,&lenData, &bIsString );
	if( nRet>=0 && bIsString && lenData>0 )
	{
		if( pvtGetString( hVal, nRet, lenData, (char*)strData ) > 0)
		{
			nRet=H323CallMgr::Instance().GetEncDecAlgorithmTypeFromOID(nAlgorithmId,strData,lenData);
			if(nRet > 0) pH323Channel->m_nAlgorithmIdUpdate=nAlgorithmId;
		}
	}

	bIsString=RV_FALSE;
	nodeData= pvtGetByPath( hVal, encNodeId, "secureSharedSecret.paramS", NULL, (int *)&paramsIV, &bIsString );
	nodeData = pvtGetByPath(hVal, encNodeId, "secureSharedSecret.encryptedSessionKey", NULL, &lenData, &bIsString );
	if( nodeData && bIsString && lenData>0 )
	{
		unsigned char encoded[256];
		encLen = pvtGetString( hVal, nodeData, lenData, (char*)encoded );
		if( encLen > 0)
		{
			unsigned char output[256]={0};
			RvUint8 sharedSecret[64]={0};
			int nKeyLen=0;
			int nKeySecLen=0;
			int nCopyLen=H323CallMgr::Instance().m_h235Mgr.m_nShareSecretLen;
			nKeySecLen=encLen;
			if(nKeySecLen ==8 ) nKeyLen=7;
			else if(nKeySecLen ==24 ) nKeyLen=21;
			else if(nKeySecLen ==16 ) nKeyLen=16;
			else if(nKeySecLen ==32 ) nKeyLen=32;
			else return;

			//根据此通道用到的算法 取最后多少位
			memset(sharedSecret,0,sizeof(sharedSecret));
			memcpy(sharedSecret,m_strSharedSecret+nCopyLen-nKeyLen,nKeyLen);
			if(H323CallMgr::Instance().m_h235Mgr.DecryptData(sharedSecret,nKeyLen,encoded,nKeySecLen,output) <=0) return;
			pH323Channel->SetChannelUpdateSharedSecret(output,nKeyLen);
			SendEncryptionUpdateACK(pH323Channel,syncFlag);
			if(pH323Channel->m_nAlgorithmIdUpdate == pH323Channel->AlgorithmId())
				pH323Channel->AddChannelEncryption();
			else
			{
				pH323Channel->AddChannelEncryptionAlgoChange();
			}
		}
	}

	pvtDelete(hVal, encNodeId);

}

void H323Call::OnRecvEncryptionUpdateAck(H323Channel * pH323Channel,HCHAN hsChan,int messageId)
{

}

void H323Call::SendEncryptionUpdateACK(H323Channel * pH323Channel,int syncFlag) //encryptionUpdateAck Command
{
	HPVT hVal;
	HPST synTree;
	HPROTCONN h245ConHandle;
	int rootId ,nodeId,nodeId2;
	int logicalNum=cmChannelGetNumber(pH323Channel->GetChannelHsChan());
	int msdStatus = cmCallMasterSlaveStatus(m_hsCall); 
	if (msdStatus == 2) return;
	char slaveMasterPath[128]={0};
	if(pH323Channel->Direction() == cmCapReceive) sprintf(slaveMasterPath,"direction.masterToSlave");
	else if(pH323Channel->Direction() == cmCapTransmit) sprintf(slaveMasterPath,"direction.slaveToMaster");
	else sprintf(slaveMasterPath,"direction.masterToSlave");

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#endif

	if(syncFlag ==0)
	syncFlag=pH323Channel->GetChannelPayloadType();

	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "h245");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	nodeId = pvtBuildByPath(hVal, rootId, "command.miscellaneousCommand", 0, NULL );
	nodeId2 = pvtBuildByPath(hVal, nodeId, "encryptionUpdateAck.synchFlag", syncFlag, NULL );
	nodeId2 = pvtBuildByPath(hVal, nodeId, slaveMasterPath, 0, NULL );

	h245ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmH245TpktChannel);
	if(NULL == h245ConHandle)
	{
		return;
	}
	if( 0 > cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, h245ConHandle, rootId))
	{
	}
	pvtDelete(hVal,rootId);
}

H323Channel* H323Call::GetRelatedChannel(H323Channel* pH323Channel)
{
	H323Channel * pChannel=NULL;
	if(pH323Channel == m_pAudioRecvChannel)
	{
		pChannel=m_pAudioSendChannel;
	}
	else if(pH323Channel == m_pAudioSendChannel)
	{
		pChannel=m_pAudioRecvChannel;
	}
	else if(pH323Channel == m_pMainVideoRecvChannel)
	{
		pChannel=m_pMainVideoSendChannel;
	}
	else if(pH323Channel == m_pMainVideoSendChannel)
	{
		pChannel=m_pMainVideoRecvChannel;
	}
	else if(pH323Channel == m_pSecondVideoRecvChannel)
	{
		pChannel=m_pSecondVideoSendChannel;
	}
	else if(pH323Channel == m_pSecondVideoSendChannel)
	{
		pChannel=m_pSecondVideoRecvChannel;
	}
	else if(pH323Channel == m_pH224RecvChannel)
	{
		pChannel=m_pH224SendChannel;
	}
	else if(pH323Channel == m_pH224SendChannel)
	{
		pChannel=m_pH224RecvChannel;
	}
	return pChannel;
}

int & H323Call::GetMgrH235MediaUse()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nH235MediaUse;
#else
	return H323CallMgr::Instance().m_nH235MediaUse;
#endif

}

int & H323Call::GetMgrH235AuthUse()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nH235AuthUse;
#else
	return H323CallMgr::Instance().m_nH235AuthUse;
#endif

}

int & H323Call::GetMgrH235MediaAlogrithm()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nUseAlogrithmId;
#else
	return H323CallMgr::Instance().m_nUseAlogrithmId;
#endif

}

int & H323Call::GetMgrH235MediaEncrypSpecified()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_nMediaEncrypSpecified;
#else
	return H323CallMgr::Instance().m_nMediaEncrypSpecified;
#endif

}

char * H323Call::GetMgrH235MediaEncrypSpecifiedPWD()
{
	//mcu adapter add
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
	return m_strMediaEncrypPWD;
#else
	return H323CallMgr::Instance().m_strMediaEncrypPWD;
#endif

}

void H323Call::CopyVideoCapOtherParam0(ConfInfo *pConfInfo,int setBitrate,int setFormat)
{

}

//A7 add
void H323Call::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	if (cs == NETEC_Session::CS_CONNECTED)
	{
		TRACE("OnNETEC_NodeConnectStatusChanged NETEC_Session::CS_CONNECTED\n");
	}
	else if (cs == NETEC_Session::CS_DISCONNECTED || cs == NETEC_Session::CS_BUSY)
	{
		TRACE("OnNETEC_NodeConnectStatusChanged NETEC_Session::CS_DISCONNECTED\n");
		H323CallMgr::Instance().HangupCall(m_strCallID.c_str());
	}

#else
//终端不调挂断
#endif

}

//A7 add
bool H323Call::matchRemoteMainVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat)
{
	return false;
}

bool H323Call::matchRemoteSecondVideoResolution(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE nFormat)
{
	return false;
}

bool H323Call::IsConfMainVideoSupport(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE videoFormatType)
{
	return false;
}

void H323Call::AllowToSendSecondVideo(bool bAllow)
{
	//if (bAllow)
	//{
	//	SendGenericResponse(true);//允许

	//	if (m_bH239ReceiverOpen==false)
	//	{
	//		m_bH239ReceiverOpen=true;

	//		m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
	//	}
	//}
	//else
	//{
	//	SendGenericResponse(false);//不允许

	//	m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	//}
	if (bAllow)
	{
		SendGenericResponse(true);//允许
		if (m_bH239ReceiverOpen==false)
		{
			m_bH239ReceiverOpen=true;
			m_nSecondVideoDir=IMXEC_CALL_DIR_IN;
		}
	}
	else
	{
		SendGenericResponse(false);//不允许
		m_bH239ReceiverOpen=false;
		m_nSecondVideoDir=IMXEC_CALL_DIR_UNKOWN;
	}
	//调整视频双流比例
	SetVideoBitRate();
}

void H323Call::SetVideoBitRate()
{
	if (m_nSecondVideoDir==IMXEC_CALL_DIR_IN)
	{
		m_nSecondVideoBitRate = 256;//256K
		m_nMainVideoBitRate = m_ConfInfo.m_nConfBitrate - 64 - m_nSecondVideoBitRate;
		m_nMainVideoActualBitRate = m_nMainVideoBitRate;
		m_nSecondVideoActualBitRate = m_nSecondVideoBitRate;
		IPCLIB_LogL2("[H323Call::SetVideoBitRate] m_nMainVideoBitRate = %d, m_nSecondVideoBitRate = %d\n", m_nMainVideoBitRate, m_nSecondVideoBitRate);
	}
	else
	{
		m_nSecondVideoBitRate = 0;
		m_nMainVideoBitRate = m_ConfInfo.m_nConfBitrate - 64;
		m_nMainVideoActualBitRate = m_nMainVideoBitRate;
		m_nSecondVideoActualBitRate = m_nSecondVideoBitRate;
		m_dSecondVideoRcvLostRate = 0;
		m_nSecondVideoRcvdPacketCount = 0;
		m_nSecondVideoStartRcvdPacketTimestamp = 0;
		m_nSecondVideoStartRcvdPacketSequence = 0;
		IPCLIB_LogL2("[H323Call::SetVideoBitRate] m_nMainVideoBitRate = %d, m_nSecondVideoBitRate = %d\n", m_nMainVideoBitRate, m_nSecondVideoBitRate);
	}

	AjustVideoBitRate();
}

void H323Call::AjustVideoBitRate()
{
	//无论主流还是第二路码流丢包，都取调第一路码流
	if (m_dMainVideoRcvLostRate > MAX_LOST_PACKET_RATE || (m_nSecondVideoDir == IMXEC_CALL_DIR_IN && m_dSecondVideoRcvLostRate > MAX_LOST_PACKET_RATE))
	{
		IPCLIB_LogL2("[H323Call::AjustVideoBitRate()] m_dMainVideoRcvLostRate = %.2f%%, m_dSecondVideoRcvLostRate = %.2f%%\n", m_dMainVideoRcvLostRate * 100, m_dSecondVideoRcvLostRate * 100);
		// 丢包比例超限时下调码率
		int nMainVideoActualBitRate = m_nMainVideoActualBitRate * BITRATE_REDUCE_RATE;
		if (nMainVideoActualBitRate >= m_nMainVideoBitRate * MIN_BIRATE_RATE)
		{
			m_nMainVideoActualBitRate = nMainVideoActualBitRate;
			if(CallMgr::Instance().m_nUseAjustMediaRate >0 && HasAVCONTransport()==false )
				SendFlowControlCommand(IMXEC_CALL_MEDIA_MAINVIDEO, m_nMainVideoActualBitRate);
		}
	}
	else
	{
		// 丢包比例在超限范围内时上调码率
		int nMainVideoActualBitRate = m_nMainVideoActualBitRate * BITRATE_INCREASE_RATE;
		if (nMainVideoActualBitRate < m_nMainVideoBitRate)
		{
			m_nMainVideoActualBitRate = nMainVideoActualBitRate;
			if(CallMgr::Instance().m_nUseAjustMediaRate >0 && HasAVCONTransport()==false )
				SendFlowControlCommand(IMXEC_CALL_MEDIA_MAINVIDEO, m_nMainVideoActualBitRate);
		}
		else
		{
			if (m_nMainVideoActualBitRate != m_nMainVideoBitRate)
			{
				m_nMainVideoActualBitRate = m_nMainVideoBitRate;
				if(CallMgr::Instance().m_nUseAjustMediaRate >0 && HasAVCONTransport()==false )
					SendFlowControlCommand(IMXEC_CALL_MEDIA_MAINVIDEO, m_nMainVideoActualBitRate);
			}
		}
	}

	//20150310 no use
	//if (m_dMainVideoRcvLostRate > 0.005)
	//{
	//	RequestMainVideoKeyFrame();
	//}

	//if (m_nSecondVideoDir == IMXEC_CALL_DIR_IN && m_dSecondVideoRcvLostRate > 0.005)
	//{
	//	RequestSecondVideoKeyFrame();
	//}

	// 	if (m_dSecondVideoRcvLostRate > MAX_LOST_PACKET_RATE)
	// 	{
	// 		// 丢包比例超限时下调码率
	// 		int nSecondVideoActualBitRate = m_nSecondVideoActualBitRate * BITRATE_REDUCE_RATE;
	// 		if (nSecondVideoActualBitRate >= m_nSecondVideoBitRate * MIN_BIRATE_RATE)
	// 		{
	// 			m_nSecondVideoActualBitRate = nSecondVideoActualBitRate;
	// 			SendFlowControlCommand(IMXEC_CALL_MEDIA_H239VIDEO, m_nSecondVideoActualBitRate);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// 丢包比例在超限范围内时上调码率
	// 		int nSecondVideoActualBitRate = m_nSecondVideoActualBitRate * BITRATE_INCREASE_RATE;
	// 		if (nSecondVideoActualBitRate < m_nSecondVideoBitRate)
	// 		{
	// 			m_nSecondVideoActualBitRate = nSecondVideoActualBitRate;
	// 			SendFlowControlCommand(IMXEC_CALL_MEDIA_H239VIDEO, m_nSecondVideoActualBitRate);
	// 		}
	// 		else
	// 		{
	// 			if (m_nSecondVideoActualBitRate != m_nSecondVideoBitRate)
	// 			{
	// 				m_nSecondVideoActualBitRate = m_nSecondVideoBitRate;
	// 				SendFlowControlCommand(IMXEC_CALL_MEDIA_H239VIDEO, m_nSecondVideoActualBitRate);
	// 			}
	// 		}
	// 	}
}

void H323Call::StatisticsMainVideo(unsigned short usSequence)
{
	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp - m_nMainVideoStartRcvdPacketTimestamp > STATISTICS_TIME * 1000)
	{
		if (usSequence > m_nMainVideoStartRcvdPacketSequence && m_nMainVideoStartRcvdPacketTimestamp > 0)
		{
			int nLostCount = usSequence - m_nMainVideoStartRcvdPacketSequence - m_nMainVideoRcvdPacketCount;
			if (nLostCount > 0)
			{
				m_dMainVideoRcvLostRate = (double)nLostCount / (usSequence - m_nMainVideoStartRcvdPacketSequence);
			}
			AjustVideoBitRate();
		}
		m_dMainVideoRcvLostRate = 0;
		m_nMainVideoRcvdPacketCount = 1;
		m_nMainVideoStartRcvdPacketTimestamp = nTimestamp;
		m_nMainVideoStartRcvdPacketSequence = usSequence;
	}
	else
	{
		m_nMainVideoRcvdPacketCount++;
	}

	//20150310 no use
	//// 检查是否可以请求关键帧
	//if (m_bMainVideoReqKeyFrame && nTimestamp - m_nMainVideoLastReqKeyFrameTimestamp >= MAX_KEY_FRAME_INTERVAL * 1000)
	//{
	//	//RequestMainVideoKeyFrame();
	//	H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
	//}
}

void H323Call::StatisticsSecondVideo(unsigned short usSequence)
{
	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp - m_nSecondVideoStartRcvdPacketTimestamp > STATISTICS_TIME * 1000)
	{
		if (usSequence > m_nSecondVideoStartRcvdPacketSequence && m_nSecondVideoStartRcvdPacketTimestamp > 0)
		{
			int nLostCount = usSequence - m_nSecondVideoStartRcvdPacketSequence - m_nSecondVideoRcvdPacketCount;
			if (nLostCount > 0)
			{
				m_dSecondVideoRcvLostRate = (double)nLostCount / (usSequence - m_nSecondVideoStartRcvdPacketSequence);
			}
			AjustVideoBitRate();
		}
		m_dSecondVideoRcvLostRate = 0;
		m_nSecondVideoRcvdPacketCount = 1;
		m_nSecondVideoStartRcvdPacketTimestamp = nTimestamp;
		m_nSecondVideoStartRcvdPacketSequence = usSequence;
	}
	else
	{
		m_nSecondVideoRcvdPacketCount++;
	}

	//20150310 no use
	//// 检查是否可以请求关键帧
	//if (m_bSecondVideoReqKeyFrame && nTimestamp - m_nSecondVideoLastReqKeyFrameTimestamp >= MAX_KEY_FRAME_INTERVAL * 1000)
	//{
	//	//RequestSecondVideoKeyFrame();
	//	H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),2);
	//}
}

IPCLIB_API void enablefc(int nEnableFlowcontrol)
{
	g_nEnableFlowcontrol = nEnableFlowcontrol;
}

IPCLIB_API void showfc()
{
	IPCLIB_LogL2("EnableFlowcontrol = %d\n", g_nEnableFlowcontrol);
}

//A7 add
void H323Call::OnFlowControlCommand(H323Channel*pH323Channel, unsigned int nBitRate)
{
	//notice FlowControlCommand
	pH323Channel->m_nFlowCCBitRate =nBitRate;

	if (pH323Channel==m_pMainVideoSendChannel)
	{
		m_rH323CallMgr.OnMainVideoFlowControlCommand(m_strCallID, nBitRate);
	}
	else if (pH323Channel==m_pSecondVideoSendChannel)
	{
		m_rH323CallMgr.OnSecondVideoFlowControlCommand(m_strCallID, nBitRate);
	}
}

//A7 add 发流控命令
void H323Call::SendFlowControlCommand(IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate)
{
	if(m_rH323CallMgr.m_nSendFlowCC <=0) return;
	if(m_nEntityType ==EM_ENTITY_MCU) return;
	//if(nMaximumBitRate <=0) return;

	enablefc(1);
	if (g_nEnableFlowcontrol == 0)
	{
		return;
	}
	std::string strConfID="1001";			   //字符串 长度为1--32
	H245ControlMessageType nMsgType;
	char * cszPacket;
	int nPacketLen;
	//get from emMediaType
	int nLogicalNum=0;
	std::string strMedia="NOMEDIA";
	if (emMediaType == IMXEC_CALL_MEDIA_MAINVIDEO && m_pMainVideoRecvChannel)
	{
		strMedia="VIDEO";
		nLogicalNum = cmChannelGetNumber(m_pMainVideoRecvChannel->m_hsChan);
	}
	else if (emMediaType == IMXEC_CALL_MEDIA_H239VIDEO && m_pSecondVideoRecvChannel)
	{
		strMedia="DUALVIDEO";
		nLogicalNum = cmChannelGetNumber(m_pSecondVideoRecvChannel->m_hsChan);
	}
	else
	{
		return;
	}
	//发送逻辑通道流控消息 flowControlCommand 消息
	{
		KCmdUserDataPacket packet ;
		nMsgType=h245CONcFlowControlCommand;
		packet.SetAttribUN2("CMD",1);
		packet.SetAttrib("H245CMD","flowControlCommand");
		packet.SetAttrib("CONFID",strConfID);
		std::string strMedia="VIDEO";//媒体类型关键字  NOMEDIA AUDIO VIDEO DUALVIDEO DATA
		int nResourceID=0;//大于0设置值  0 不设置
		packet.SetAttrib("logicalChannel",strMedia);
		packet.SetAttribUN2("resourceID",nResourceID);
		packet.SetAttribUN2("maximumBitRate",nMaximumBitRate);
		std::string strPacket=packet.GetString();
		cszPacket=(char * ) strPacket.c_str();
		nPacketLen=strPacket.length()+1;
		SendCallH245ControlMessage(nMsgType,cszPacket,nPacketLen);
	}
}

void H323Call::SeRegAccountCallMark(void)
{
	m_bRegCall=true;
}

void H323Call::OnCallRecvACF(H323RegUser * pH323RegUser)
{
	if(m_bRegCall)
	{

#if defined(TAIERLAB) && (TAIERLAB == 1)
		cmCallSendCallProceeding(m_hsCall);//add tai er ru wang test use 
#endif

		cmCallAccept(m_hsCall);
		pH323RegUser->GetLocalIP(m_strLocalIP);
		m_nCallDir=IMXEC_CALL_DIR_IN;
		m_strRemoteUserNonstandardInfo=m_strRecvUserNonstandardInfo;
		m_strTelNum=pH323RegUser->GetRegPhoneNum();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
		H323CallMgr::Instance().OnDAP_CallRingIn(GetCallID(),GetUserID(),GetUserFullName(),m_strE164ID,m_strRemoteUserNonstandardInfo,m_strTelNum);

#else
		H323CallMgr::Instance().OnCallRingIn(GetCallID(),GetUserID(),GetUserFullName());
#endif
	}
}

void H323Call::LevelIncFromMaxFS(int nMaxFS,int & nLevel)
{
//#define MaxLevelCount 16
//	//1-15   1b-19   1.1-22   1.2-29  1.3-36   2-43  2.1-50  2.2-57   3-64   3.1-71   3.2-78  4-85   4.1-92   4.2-99    5-106  5.1-113 
//	int arrLevelMaxFS[MaxLevelCount][2] = {{15,99},{19,99},{22,396},{29,396},{36,396},{43,396},{50,792},{57,1620},{64,1620},
//	{71,3600},{78,5120},{85,8192},{92,8192},{99,8704},{106,22080},{113,36864}};
//	int nGetLevel=0;
//	for(int i=0;i<MaxLevelCount;i++)
//	{
//		if(nMaxFS *256 >= arrLevelMaxFS[i][1])
//		{
//			nGetLevel=arrLevelMaxFS[i][0];
//			if(nGetLevel >= nLevel)
//			{
//				nLevel=nGetLevel;
//				break;
//			}
//		}
//	}
}

void H323Call::GetResolution(int& nRes, int &nFps, int nMaxMBPS, int nMaxFs, int nLevel, int nProfile)
{

	//if( (nLevel>=57) && (nProfile==8))
	//{
	//	int nFindPro=-1;
	//	nFindPro=m_strProductID.find("HUAWEI"); //notice te30 test use productId: HUAWEI TEx0
	//	if (nFindPro>=0) nRes =VIDEO_FORMAT_1080P;
	//}

	//const unsigned char abyLevel[13] = {15, 22, 29, 36, 50, 57, 64, 71, 78, 85, 99, 106, 113};
	//const int MaxMBPSAndMaxFrame[13][2] = {{1485,99},{3000,396},{6000,396},{11880,396},{19800,792},{20250,1620},{40500,1620},
	//{108000,3600},{216000,5120},{245760,8192},{522240,8704},{589824,22080},{983040,36864}};

	//for(int i=0; i<13; i++)
	//{
	//	if (nMaxMBPS > MaxMBPSAndMaxFrame[i][0] || nMaxFs > MaxMBPSAndMaxFrame[i][1] || nLevel > abyLevel[i])
	//	{
	//		nFps = nMaxMBPS / MaxMBPSAndMaxFrame[i][1];
	//	}
	//	else
	//	{
	//		break;
	//	}
	//}
}

void H323Call::SendChannelKeepAlive(void)
{
	if(m_pAudioSendChannel && m_pAudioSendChannel->m_bGetKeepAlive) m_pAudioSendChannel->SendChannelKeepAlive();
	if(m_pMainVideoSendChannel && m_pMainVideoSendChannel->m_bGetKeepAlive) m_pMainVideoSendChannel->SendChannelKeepAlive();
	if(m_pSecondVideoSendChannel && m_pSecondVideoSendChannel->m_bGetKeepAlive) m_pSecondVideoSendChannel->SendChannelKeepAlive();
	if(m_pH224SendChannel && m_pH224SendChannel->m_bGetKeepAlive) m_pH224SendChannel->SendChannelKeepAlive();

	if(m_pAudioRecvChannel && m_pAudioRecvChannel->m_bGetKeepAlive) m_pAudioRecvChannel->SendChannelKeepAlive();
	if(m_pMainVideoRecvChannel && m_pMainVideoRecvChannel->m_bGetKeepAlive) m_pMainVideoRecvChannel->SendChannelKeepAlive();
	if(m_pSecondVideoRecvChannel && m_pSecondVideoRecvChannel->m_bGetKeepAlive) m_pSecondVideoRecvChannel->SendChannelKeepAlive();
	if(m_pH224RecvChannel && m_pH224RecvChannel->m_bGetKeepAlive) m_pH224RecvChannel->SendChannelKeepAlive();

}

void H323Call::AddCodecCapFromConfig()
{

}

bool H323Call::GetRemoteUserIP(std::string & strUserIP)
{
	HPROTCONN hConn = cmGetTpktChanHandle(m_hsCall,cmQ931TpktChannel); 
	if(!hConn)
	{
		return false;
	}
	cmTransportAddress tr;
	if(cmProtocolGetRemoteAddress(H323CallMgr::Instance().m_hApp, hConn, &tr) < 0)
	{
		return false;
	}

	if(tr.type != cmTransportTypeIP)
	{
		return false;
	}
	struct in_addr		addr;
	addr.S_un.S_addr	=tr.ip;
	strUserIP=inet_ntoa(addr);

	return true;
}

int H323Call::SendRegDRQMessage()
{
	if(/*m_hsCall ==NULL || */m_bRegCall ==false) return 0;

	int nUseGK=m_nRegUseGK;
	int nFindGKAddr=H323CallMgr::Instance().m_nFindGKAddr;
	cmTransportAddress * transGKAddr=&(H323CallMgr::Instance().m_TransportAddr);
	int nGKPort=H323CallMgr::Instance().m_nGateKeeperPort;

	int nRet = -1;
	HRAS hRas = NULL;
	const char * cszHost=m_strHost.c_str();
	char * gkIP="";
	RvUint16 port=H323CallMgr::Instance().m_nGateKeeperPort;

	cmTransportAddress tranAddr;
	memset( &tranAddr, 0, sizeof(cmTransportAddress) );

	if( nUseGK == 1 && nFindGKAddr && transGKAddr)
	{
		memcpy( &tranAddr, transGKAddr, sizeof(cmTransportAddress) );
		struct in_addr		addr;
		addr.S_un.S_addr	=tranAddr.ip;
		gkIP = inet_ntoa(addr);
		port = tranAddr.port;
	}
	else
	{
		char addr[128]={0};
		sprintf(addr,"%s:%d",cszHost,nGKPort);
		cmString2TransportAddress(addr,&tranAddr);
		gkIP=(char *)cszHost;
		port=nGKPort;
	}

	H323CallMgr * pH323CallMgr=&(H323CallMgr::Instance());
	nRet = cmRASStartTransaction(H323CallMgr::Instance().m_hApp,(HAPPRAS)pH323CallMgr, &hRas, cmRASDisengage, &tranAddr,m_hsCall);
	if(nRet < 0 )
	{
		cmRASClose( hRas );
		return -1;
	}
	if(hRas == NULL) return -1;

	cmRASDisengageReason DRQReason=cmRASDisengageReasonNormalDrop;
	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamDisengageReason,0,DRQReason, NULL);


	if(m_EPAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamEndpointID,0, sizeof(cmAlias), (char*)&m_EPAlias);
	}

	if(m_GKAlias.length >0)
	{
		nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamGatekeeperID,0, sizeof(cmAlias), (char*)&m_GKAlias);
	}

	int len1=sizeof(m_szConferenceID);
	int len2=sizeof(m_szCallEndpointID);
	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCID,0,len1,m_szConferenceID);
	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCallID,0,len2,m_szCallEndpointID);
	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamCRV,0,m_nParamCRV,NULL);

	RvBool bAnswer=RV_TRUE;
	if(m_nMasterCall <= 0) bAnswer=RV_FALSE;
	nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamAnswerCall,0,bAnswer,NULL);

	nRet = cmRASRequest(hRas);
	if(nRet < 0 )
	{
		cmRASClose(hRas);
		return -1;
	}

	cmRASClose(hRas);//modify 20140625
	return 0;
}


int H323Call::SendRegIRRMessage(HRAS hsRas,int type)
{
	if(m_hsCall ==NULL || m_bRegCall ==false) return 0;
	if(0)
	{
		HPVT hVal;
		HPST synTree;
		HPROTCONN hConHandle;
		int rootId ,nodeId;
		int error=0;
		hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
		synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "ras");
		rootId = pvtAddRoot(hVal, synTree, 0, NULL);
		if(rootId >= 0)
		{
			nodeId = pvtBuildByPath(hVal, rootId, "infoRequestResponse", 0, NULL );
			if(nodeId >=0)
			{
				error=pvtBuildByPath(hVal, nodeId, "requestSeqNum",1111, NULL );//notice
				int nodeSub=0;
				nodeSub=pvtBuildByPath(hVal, nodeId, "endpointType.vendor", 0, NULL );
				error=pvtBuildByPath(hVal, nodeSub, "vendor.t35CountryCode", H323CallMgr::Instance().m_nT35Code, NULL);
				error=pvtBuildByPath(hVal, nodeSub, "vendor.t35Extension", H323CallMgr::Instance().m_nT35Extension, NULL);
				error=pvtBuildByPath(hVal, nodeSub, "vendor.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, NULL);
				error=pvtBuildByPath(hVal, nodeSub, "productId", H323CallMgr::Instance().m_strProductID.size(), H323CallMgr::Instance().m_strProductID.c_str());
				error=pvtBuildByPath(hVal, nodeSub, "versionId", H323CallMgr::Instance().m_strProductVersionID.size(), H323CallMgr::Instance().m_strProductVersionID.c_str());
				error=pvtBuildByPath(hVal, nodeId, "endpointType.terminal", 0, NULL );
				error=pvtBuildByPath(hVal, nodeId, "endpointType.mc",RV_TRUE, NULL );
				error=pvtBuildByPath(hVal, nodeId, "endpointType.undefinedNode", 0, NULL );
				int len=strlen(m_szCallEndpointID);
				error=pvtBuildByPath(hVal, nodeId, "endpointIdentifier",len, m_szCallEndpointID);//notice
				int nPort=H323CallMgr::Instance().m_nH323ListenPort;
				cmTransportAddress CallSignalAddress;
				memset( &CallSignalAddress, 0, sizeof(cmTransportAddress) );
				char addr[128]={0};
				sprintf(addr,"%s:%u",m_strLocalIP.c_str(),nPort);
				error=cmString2TransportAddress(addr,&CallSignalAddress);
				if(error >=0)
				{
					nodeSub=pvtBuildByPath(hVal, nodeId, "rasAddress", 0, NULL );
					error=cmTransportAddress2Vt(hVal,&CallSignalAddress,nodeSub);
				}
				nPort=H323CallMgr::Instance().m_nGateKeeperPort;
				sprintf(addr,"%s:%u",m_strLocalIP.c_str(),nPort);
				error=cmString2TransportAddress(addr,&CallSignalAddress);
				if(error >=0)
				{
					nodeSub=pvtBuildByPath(hVal, nodeId, "callSignalAddress.1", 0, NULL );
					error=cmTransportAddress2Vt(hVal,&CallSignalAddress,nodeSub);
				}
				nodeSub=pvtBuildByPath(hVal, nodeId, "perCallInfo.1", 0, NULL );
				if(nodeSub >=0)
				{
					int tempNodeId = pvtBuildByPath(hVal, nodeSub, "nonStandardData", 0, 0 );
					//tmpError = pvtBuildByPath(hVal, nodeId, "nonStandardIdentifier.object", length, OID );
					int tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.t35CountryCode",H323CallMgr::Instance().m_nT35Code, 0 );
					tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.t35Extension", H323CallMgr::Instance().m_nT35Extension, 0 );
					tmpError = pvtBuildByPath(hVal, tempNodeId, "nonStandardIdentifier.h221NonStandard.manufacturerCode", H323CallMgr::Instance().m_nManufacturerCode, 0 );

					int paramLen=m_strSetUserNonstandardInfo.length();
					tmpError = pvtBuildByPath(hVal, tempNodeId, "data", paramLen, m_strSetUserNonstandardInfo.c_str() );

					int len1=strlen(m_szConferenceID);
					error=pvtBuildByPath(hVal, nodeId, "perCallInfo.1.callReferenceValue",m_nParamCRV, NULL);
					error=pvtBuildByPath(hVal, nodeSub, "conferenceID",16, m_szConferenceID);
					error=pvtBuildByPath(hVal, nodeSub, "originator",RV_FALSE, NULL);
					error=pvtBuildByPath(hVal, nodeSub, "h245", 0,NULL);
					int sub=pvtBuildByPath(hVal, nodeSub, "callSignaling.recvAddress", 0,NULL);
					sprintf(addr,"%s:%u",m_strLocalIP.c_str(),nPort);//notice
					error=cmString2TransportAddress(addr,&CallSignalAddress);
					if(error >=0)
					{
						cmTransportAddress2Vt(hVal,&CallSignalAddress,sub);
					}
					error=pvtBuildByPath(hVal, nodeSub, "callType.pointToPoint", 0,NULL);
					error=pvtBuildByPath(hVal, nodeSub, "bandWidth", m_nBandWidth *10,NULL);//notice
					error=pvtBuildByPath(hVal, nodeSub, "callModel.gatekeeperRouted", 0,NULL);
					len=strlen(m_szCallEndpointID);
					//error=pvtBuildByPath(hVal, nodeSub, "callIdentifier.guid", 16,m_szCallEndpointID);
					//error=pvtBuildByPath(hVal, nodeSub, "substituteConfIDs.1", 16,m_szCallEndpointID);
				}
				error=pvtBuildByPath(hVal, nodeId, "perCallInfo.needResponse", RV_FALSE, NULL );
				error=pvtBuildByPath(hVal, nodeId, "perCallInfo.unsolicited", RV_TRUE, NULL );
				//error=pvtBuildByPath(hVal, nodeId, "perCallInfo.1.callReferenceValue",m_nParamCRV, NULL);
				hConHandle=cmGetUdpChanHandle(m_hsCall,cmRasUdpChannel);

			}

			HPST synTree2 = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "addr");
			int rootId2 = pvtAddRoot(hVal, synTree2, 0, NULL);
			int nUseGK=m_nRegUseGK;
			int nFindGKAddr=H323CallMgr::Instance().m_nFindGKAddr;
			cmTransportAddress * transGKAddr=&(H323CallMgr::Instance().m_TransportAddr);
			int nGKPort=H323CallMgr::Instance().m_nGateKeeperPort;
			const char * cszHost=m_strHost.c_str();
			cmTransportAddress tranAddr;
			memset( &tranAddr, 0, sizeof(cmTransportAddress) );

			if( nUseGK == 1 && nFindGKAddr && transGKAddr)
			{
				memcpy( &tranAddr, transGKAddr, sizeof(cmTransportAddress) );
				struct in_addr		addr;
				addr.S_un.S_addr	=tranAddr.ip;
			}
			else
			{
				char addr[128]={0};
				sprintf(addr,"%s:%d",cszHost,nGKPort);
				cmString2TransportAddress(addr,&tranAddr);
			}
			error=cmTransportAddress2Vt(hVal,&tranAddr,rootId2);

			if(hConHandle) cmProtocolSendMessageTo(H323CallMgr::Instance().m_hApp, hConHandle, rootId,rootId2);
			pvtDelete(hVal,rootId);
			pvtDelete(hVal,rootId2);
			return 0;
		}
	}
	else
	{
		int nRet = -1;
		HRAS hRas = NULL;
		int nUseGK=m_nRegUseGK;
		int nFindGKAddr=H323CallMgr::Instance().m_nFindGKAddr;
		cmTransportAddress * transGKAddr=&(H323CallMgr::Instance().m_TransportAddr);
		int nGKPort=H323CallMgr::Instance().m_nGateKeeperPort;
		const char * cszHost=m_strHost.c_str();
		cmTransportAddress tranAddr;
		memset( &tranAddr, 0, sizeof(cmTransportAddress) );

		if( nUseGK == 1 && nFindGKAddr && transGKAddr)
		{
			memcpy( &tranAddr, transGKAddr, sizeof(cmTransportAddress) );
		}
		else
		{
			char addr[128]={0};
			sprintf(addr,"%s:%d",cszHost,nGKPort);
			cmString2TransportAddress(addr,&tranAddr);
		}

		if(m_EPAlias.length >0)
		{
			nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamEndpointID,0, sizeof(cmAlias), (char*)&m_EPAlias);
		}

		if(m_GKAlias.length >0)
		{
			//nRet = cmRASSetParam( hRas, cmRASTrStageRequest, cmRASParamGatekeeperID,0, sizeof(cmAlias), (char*)&m_GKAlias);
		}

		cmRASTransaction transact= cmRASInfo ;
		if(type ==1) transact=cmRASUnsolicitedIRR;

		H323CallMgr * pH323CallMgr=&(H323CallMgr::Instance());
		hRas = hsRas;
		if(hsRas == NULL)
		{
			nRet = cmRASStartTransaction(H323CallMgr::Instance().m_hApp,(HAPPRAS)pH323CallMgr, &hRas,transact, &tranAddr, m_hsCall);
			if(nRet < 0 )
			{
				cmRASClose( hRas );
				return -1;
			}
		}
		if(hRas == NULL) return -1;

		if(type ==0) nRet =cmRASConfirm( hRas );
		else if(type ==1) nRet =cmRASRequest( hRas );
		else return -1;

		if(nRet < 0 )
		{
			cmRASClose(hRas);
			return -1;
		}

		if(hsRas==NULL) cmRASClose( hRas );

		return nRet;
	}
}

void H323Call::NETEC_NodeStop()
{
	if (m_nNETECNodeCount >0 && NETEC_Node::IsStarted() !=0 )
	{
		char msg[128]={0};
		sprintf(msg,"NETEC_NodeStop Begin,NETECNodeCount:%d",m_nNETECNodeCount);
		LogFile(m_hsCall,this,m_strUserID.c_str(),m_strCallID.c_str(),msg);
		NETEC_Node::Stop();
		m_nNETECNodeCount --;
		sprintf(msg,"NETEC_NodeStop End,NETECNodeCount:%d",m_nNETECNodeCount);
		LogFile(m_hsCall,this,m_strUserID.c_str(),m_strCallID.c_str(),msg);
	}
}

void H323Call::NETEC_NodeStart()
{
	if (NETEC_Node::IsStarted() ==0)
	{
		char msg[128]={0};
		if (m_strPeerMCUID != "")
		{
			// added by WangLr on 20130728
			// 连接对端MCU
			NETEC_Node::SetServerIP(m_strPeerLocalIP.c_str());
			NETEC_Node::SetServerPort(m_nPeerLocalPort);

			sprintf(msg,"NETEC_NodeStart Begin,NETECNodeCount:%d",m_nNETECNodeCount);
			LogFile(m_hsCall,this,m_strUserID.c_str(),m_strCallID.c_str(),msg);

			if(NETEC_Node::Start() ==0)
			{
				m_nNETECNodeCount ++;
				sprintf(msg,"NETEC_NodeStart End,NETECNodeCount:%d",m_nNETECNodeCount);
				LogFile(m_hsCall,this,m_strUserID.c_str(),m_strCallID.c_str(),msg);
			}

			int nTimes = 0;
			while (NETEC_Node::GetConnectStatus() != NETEC_Session::CS_CONNECTED)
			{
				Sleep(100);
				// 尝试连接3秒
				if (nTimes++ > 30)
				{
					break;
				}
			}
			m_nLocalNodeID=NETEC_Node::GetNodeID();
		}
	}
}

void H323Call::SendCallReleaseComplete(cmReasonType nReason) //Send H225 User Infomation
{
	HPVT hVal;
	HPST synTree;
	HPROTCONN hQ931ConHandle;
	int rootId;
	int tmpNodeId;
	int tmpError;
	int paramLen=128;
	char paramStr[128]={0};
	char OID[10];
	int length;
	typedef struct
	{
		cmReasonType nameId;
		char name[64];
	} fieldNames;

	//H225_PROTOCOL_IDENTIFIER Same To H323Statck H225 Version
#define H225_PROTOCOL_IDENTIFIER "itu-t recommendation h(8) 2250 0 4"

	length=utlEncodeOID(sizeof(OID), OID, H225_PROTOCOL_IDENTIFIER);
	hVal = cmGetValTree(H323CallMgr::Instance().m_hApp);
	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "q931");

	fieldNames reasonNames[]=
	{
		{cmReasonTypeNoBandwidth,               "noBandwidth"},
		{cmReasonTypeGatekeeperResource,         "gatekeeperResources"},
		{cmReasonTypeUnreachableDestination,     "unreachableDestination"},
		{cmReasonTypeDestinationRejection,       "destinationRejection"},
		{cmReasonTypeInvalidRevision,            "invalidRevision"},
		{cmReasonTypeNoPermision,                "noPermission"},
		{cmReasonTypeUnreachableGatekeeper,      "unreachableGatekeeper"},
		{cmReasonTypeGatewayResource,            "gatewayResources"},
		{cmReasonTypeBadFormatAddress,           "badFormatAddress"},
		{cmReasonTypeAdaptiveBusy,               "adaptiveBusy"},
		{cmReasonTypeInConf,                     "inConf"},
		{cmReasonTypeUndefinedReason,            "undefinedReason"},
		{cmReasonTypeRouteCallToGatekeeper,      "routeCallToGatekeeper"},
		{cmReasonTypeCallForwarded,              "callForwarded"},
		{cmReasonTypeRouteCallToMC,              "routeCallToMC"},
		{cmReasonTypeFacilityCallDeflection,     "facilityCallDeflection"},
		{cmReasonTypeSecurityDenied,             "securityDenied"},
		{cmReasonTypeCalledPartyNotRegistered,   "calledPartyNotRegistered"},
		{cmReasonTypeCallerNotregistered,        "callerNotRegistered"},
		{cmReasonTypeConferenceListChoice,       "conferenceListChoice"},
		{cmReasonTypeStartH245,                  "startH245"},
		{cmReasonTypeNewConnectionNeeded,        "newConnectionNeeded"},
		{cmReasonTypeNoH245,                     "noH245"},
		{cmReasonTypeNewTokens,                  "newTokens"},
		{cmReasonTypeFeatureSetUpdate,           "featureSetUpdate"},
		{cmReasonTypeForwardedElements,          "forwardedElements"},
		{cmReasonTypeTransportedInformation,     "transportedInformation"},
		{cmReasonTypeNeededFeatureNotSupported,  "neededFeatureNotSupported"},
		{cmReasonTypeHopCountExceeded,           "hopCountExceeded"}
	};

	synTree = cmGetSynTreeByRootName(H323CallMgr::Instance().m_hApp, "q931UU");
	rootId = pvtAddRoot(hVal, synTree, 0, NULL);
	tmpNodeId = pvtBuildByPath(hVal, rootId, "h323-message-body", 0, 0 );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.protocolIdentifier", length, OID );
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.protocolIdentifier", length, OID );
	char temp[128]={0};
	sprintf(temp,"releaseComplete.reason.%s",reasonNames[nReason].name);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, temp, 0,NULL);
	int len=sizeof(m_szCallEndpointID);
	cmCallGetParam(m_hsCall,cmParamCallID,0,&len,m_szCallEndpointID);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.callIdentifier.guid", len, m_szCallEndpointID);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.callIdentifier.guid", len, m_szCallEndpointID);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.presentationIndicator.presentationAllowed", 0, NULL);
	tmpError = pvtBuildByPath(hVal, tmpNodeId, "releaseComplete.screeningIndicator.userProvidedVerifiedAndFailed", 0, NULL);
	tmpError = pvtBuildByPath(hVal, rootId, "h245Tunneling",RV_FALSE, NULL);

	hQ931ConHandle = cmGetTpktChanHandle((HCALL)m_hsCall, cmQ931TpktChannel);
	tmpError = cmProtocolSendMessage(H323CallMgr::Instance().m_hApp, hQ931ConHandle, rootId);
	pvtDelete(hVal,rootId);

	cmCallClose(m_hsCall);
	return;
}

//notice netec send ptz 
void H323Call::OnNETECRecvdPTZ(void*pData,int nLen)
{
	if(m_pH224RecvChannel)
	{
		m_pH224RecvChannel->OnRecvAnnexQDataProc((unsigned char *)pData, nLen);
	}
}

void H323Call::OnNETECRecvdCtrl(void*pData,int nLen)
{

}

//add by Jan for DTMF [20140730]
void H323Call::OnRecvDTMF(char dtmf)
{
	CallMgr::Instance().OnCallRecvdDTMF( m_strCallID.c_str(),dtmf);
}
//end [20140730]

bool H323Call::GetCallSignalAddress(std::string & strIP,int & nPort) //0 src 1 dest
{
	bool bRet=false;
	if(m_nCallDir == IMXEC_CALL_DIR_UNKOWN) return bRet;
	cmTransportAddress CallSignalAddress;
	cmCallParam addrParam = cmParamSrcCallSignalAddress;
	if(m_nCallDir == IMXEC_CALL_DIR_OUT) addrParam = cmParamDestCallSignalAddress;
	int nError=cmCallGetParam(m_hsCall,addrParam,0,0,(char*)&CallSignalAddress);
	if(nError >= 0)
	{
		if(CallSignalAddress.type == cmTransportTypeIP)
		{
			struct in_addr addr;
			addr.S_un.S_addr =CallSignalAddress.ip;
			strIP=inet_ntoa(addr);
			nPort=CallSignalAddress.port;
			bRet=true;
		}
	}
	return bRet;
}

void H323Call::AddMediaPacketization(H323Channel * pH323Channel,int nodeID)
{
	if(m_pSecondVideoSendChannel == pH323Channel || m_pMainVideoSendChannel == pH323Channel )
	{
		int error = 0;
		if(pH323Channel->VideoCodecType() == VIDEC_CODEC_H263P )
		{
			char pathBuf[256]={"request.openLogicalChannel.forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.mediaPacketization.rtpPayloadType.payloadDescriptor.rfc-number"};
			error=pvtBuildByPath( cmGetValTree(H323CallMgr::Instance().m_hApp),nodeID,pathBuf,2429, NULL);
			strcpy(pathBuf,"request.openLogicalChannel.forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.mediaPacketization.rtpPayloadType.payloadType");
			error=pvtBuildByPath( cmGetValTree(H323CallMgr::Instance().m_hApp),nodeID,pathBuf,pH323Channel->PayloadType(),NULL);
		}
		if(pH323Channel->VideoCodecType() == VIDEC_CODEC_H264 )
		{
			//unsigned char pValueOid[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x00,0x01,0}; //FU-A
			unsigned char pValueOid[10]={0x00,0x08,0x81,0x71,0x00,0x00,0x00,0x00,0}; //NAL
			char pathBuf[256]={"request.openLogicalChannel.forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.mediaPacketization.rtpPayloadType.payloadDescriptor.oid"};
			error=pvtBuildByPath( cmGetValTree(H323CallMgr::Instance().m_hApp),nodeID,pathBuf,8, (char*)pValueOid);
			strcpy(pathBuf,"request.openLogicalChannel.forwardLogicalChannelParameters.multiplexParameters.h2250LogicalChannelParameters.mediaPacketization.rtpPayloadType.payloadType");
			error=pvtBuildByPath( cmGetValTree(H323CallMgr::Instance().m_hApp),nodeID,pathBuf,pH323Channel->PayloadType(),NULL);
		}
	}
}

bool H323Call::GetMediaRtpHandle(void * hRtp)
{
	bool bHas=false;
	for(int i=0;i<m_MediaRtpPortAllocate.m_nCount;i++)
	{
		if( (m_MediaRtpPortAllocate.m_Array[i].m_hRtp==hRtp) && 
			(m_MediaRtpPortAllocate.m_Array[i].m_hRtp!=NULL) )
		{
			m_MediaRtpPortAllocate.m_Array[i].m_hRtp;
			bHas=true;
			break;
		}
	}
	return bHas;
}

int H323Call::GetVideoCapFrameParam(VIDEC_CODEC_TYPE  nCodecType,VIDEO_FORMAT_TYPE nFormat,int type)
{
	int nFrame =30;

	if(type==1)
	{
		for(int count=0;count<m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;count++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[count].m_VideoCodecType == nCodecType && 
				m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[count].m_VideoFormatType == nFormat)
			{
				nFrame =m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[count].m_nVideoFrameRate;
				break;
			}
		}
	}
	else if(type==4)
	{
		for(int count=0;count<m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;count++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[count].m_VideoCodecType == nCodecType && 
				m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[count].m_VideoFormatType == nFormat)
			{
				nFrame =m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[count].m_nVideoFrameRate;
				break;
			}
		}
	}
	return nFrame;
}

int H323Call::CompareVideoFormatType(VIDEO_FORMAT_TYPE nType1,VIDEO_FORMAT_TYPE nType2)
{
	int nResult=-1;
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
		case VIDEO_FORMAT_QVGA:
			nValue[i]= 0;
			break;
		case VIDEO_FORMAT_CIF:
			nValue[i]= 1;
			break;
		case VIDEO_FORMAT_VGA:
			nValue[i]= 2;
			break;
		case VIDEO_FORMAT_4CIF:
			nValue[i]= 3;
			break;
		case VIDEO_FORMAT_720I:
			nValue[i]= 4;
			break;
		case VIDEO_FORMAT_SVGA:
			nValue[i]= 5;
			break;
		case VIDEO_FORMAT_XGA:
			nValue[i]= 6;
			break;
		case VIDEO_FORMAT_720P:
			nValue[i]= 7;
			break;
		case VIDEO_FORMAT_1080I:
			nValue[i]= 8;
			break;
		case VIDEO_FORMAT_SXGA:
			nValue[i]= 9;
			break;
		case VIDEO_FORMAT_16CIF:
			nValue[i]= 10;
			break;
		case VIDEO_FORMAT_1080P:
			nValue[i]= 11;
			break;
		default:
			nValue[i]= 0 ;
			break;
		}
	}
	if(nValue[0] == nValue[1]) nResult=0;
	else if(nValue[0] > nValue[1]) nResult=1;
	else nResult=-1;
	return nResult;
}

bool H323Call::IsConfSecondVideoSupport(VIDEC_CODEC_TYPE videoCodecType, VIDEO_FORMAT_TYPE videoFormatType)
{
	return false;
}

void H323Call::SetCallNum(int num)
{
	m_nCallNum =num;
}

int H323Call::GetCallNum( )
{
	return m_nCallNum ;
}

int H323Call::GetLocalMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	if(nWidth >0 && nHeight >0 && m_pMainVideoSendChannel)
	{
		if(m_pMainVideoSendChannel->m_nChannelState !=cmChannelStateConnected) { nWidth=0; nHeight=0; chInfo.m_nFrame =0;}
		else
		{
			chInfo.m_nFrame = m_pMainVideoSendChannel->VideoFrameRate();
			GetVideoFormatType(m_nRemoteMainVideoFormatType,nWidth,nHeight);
		}
		//else m_pMainVideoSendChannel->GetVideoFormatType(nWidth,nHeight);
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_H323;
	chInfo.m_bChannelOpen =m_pMainVideoSendChannel ? true:false ;
	return nRet;
}

int H323Call::GetSecondVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	//notice wait ?
	if(nWidth >0 && nHeight >0 )
	{
		if(m_pSecondVideoSendChannel && m_nSecondVideoDir ==IMXEC_CALL_DIR_OUT)
		{
			if(m_pSecondVideoSendChannel->m_nChannelState != cmChannelStateConnected) { nWidth=0; nHeight=0; chInfo.m_nFrame =0;}
			else
			{
				chInfo.m_nFrame = m_pSecondVideoSendChannel->VideoFrameRate();
				GetVideoFormatType(m_nRemoteH239VideoFormatType,nWidth,nHeight);
			}
			//else m_pSecondVideoSendChannel->GetVideoFormatType(nWidth,nHeight);
		}
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_H323;
	if(m_pSecondVideoSendChannel && m_nSecondVideoDir ==IMXEC_CALL_DIR_OUT)
	{
		chInfo.m_nDualVideoDir =IMXEC_CALL_DIR_OUT;
		chInfo.m_bChannelOpen =true;
	}
	if(m_pSecondVideoRecvChannel && m_nSecondVideoDir ==IMXEC_CALL_DIR_IN)
	{
		chInfo.m_nDualVideoDir =IMXEC_CALL_DIR_IN;
		chInfo.m_bChannelOpen =true;
	}

	return nRet;
}

int H323Call::GetRemoteMainVideoResolution(int&nWidth, int&nHeight,CallChannelInfo & chInfo)
{
	int nRet=0;
	if(nWidth >0 && nHeight >0 && m_pMainVideoRecvChannel)
	{
		chInfo.m_nFrame =m_pMainVideoRecvChannel->VideoFrameRate();
		if(m_pMainVideoRecvChannel->m_nChannelState !=cmChannelStateConnected) 
		{ nWidth=0; nHeight=0; chInfo.m_nFrame =0;}
	}

	chInfo.m_nProtocol =IMXEC_CALL_PROT_H323;
	chInfo.m_bChannelOpen =m_pMainVideoRecvChannel ? true:false ;
	return nRet;
}

void H323Call::OnRcvChannelClose(H323Channel* pH323Channel)
{
	if(m_nRemoteCapChaneged >0 && m_nRemoteCapChaneged <8 )
	{
		H323Channel* pSendH323Channel=NULL;
		ChannelMediaType nMediaType =pH323Channel->m_nMediaType;
		if(nMediaType == Enum_Channel_Media_Audio) pSendH323Channel=m_pAudioSendChannel;
		else if(nMediaType == Enum_Channel_Media_Video) pSendH323Channel=m_pMainVideoSendChannel;
		else if(nMediaType == Enum_Channel_Media_DualVideo) pSendH323Channel=m_pSecondVideoSendChannel;
		else if(nMediaType == Enum_Channel_Media_H224) pSendH323Channel=m_pH224SendChannel;
		if(pSendH323Channel )
		{
			//OutputDebugString("OnRcvChannelClose\n");
			pH323Channel->ChannelRTPPause();
			H323CallMgr::Instance().OnCallH245CapChanged(GetCallID(),GetUserID(),GetUserFullName(),nMediaType);
		}
	}
}

void H323Call::OnRcvH245CapabilitiesSetChange(unsigned int nMediaType)
{
	if(nMediaType == Enum_Channel_Media_Audio && m_pAudioSendChannel)
	{
		m_pAudioSendChannel->ChannelRTPPause();
		m_pAudioSendChannel->CloseChannel();
		OpenAudioOutChannel();
	}
	else if(nMediaType == Enum_Channel_Media_Video && m_pMainVideoSendChannel)
	{
		m_pMainVideoSendChannel->ChannelRTPPause();
		m_pMainVideoSendChannel->CloseChannel();
		OpenVideoOutChannel();
	}
	else if(nMediaType == Enum_Channel_Media_DualVideo && m_pSecondVideoSendChannel)
	{
		m_pSecondVideoSendChannel->ChannelRTPPause();
		m_pSecondVideoSendChannel->CloseChannel();
		OpenH239VideoOutChannel();
	}
	else if(nMediaType == Enum_Channel_Media_H224 && m_pH224SendChannel)
	{
		m_pH224SendChannel->ChannelRTPPause();
		m_pH224SendChannel->CloseChannel();
		OpenFECCOutChannel();
	}
}

void H323Call::SetH239TokenSendReqStatus(int status)
{
	m_nSendH239TokenReq=status;
}

int H323Call::GetH239TokenSendReqStatus( )
{
	return m_nSendH239TokenReq;
}

void H323Call::TestConfInfoCapAdd()
{
	//video
	int nTestBitrate=m_nMainVideoMaxBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H264;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_CIF;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=nTestBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;

	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H264;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_4CIF;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=nTestBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;

	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H264;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_SXGA;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=nTestBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;

	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H264_SVC;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_1080P;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=nTestBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;

	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H263;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_4CIF;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=768;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;

	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoCodecType=VIDEC_CODEC_H263P;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_VideoFormatType=VIDEO_FORMAT_1080P;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoBitrate=nTestBitrate;
	m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount].m_nVideoFrameRate=30;
	m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount +1;
}

void H323Call::CheckGenerateSSRCFlag()
{
	//Added by jiangdingfeng 20140929 切换终端音频源时针对个别终端检查是否需要重新生成RTP SSRC
	m_bFlag = false;
	int nFound=-1;
	if( m_bFlag == false)
	{
		nFound=m_strProductID.find("H3C MG6060");
		if( nFound >=0 ) m_bFlag = true;
	}

	if( m_bFlag == false)
	{
		nFound=-1;
		nFound=m_strProductID.find("H3C MG6050");
		if( nFound >=0 ) m_bFlag = true;
	}

}

void H323Call::SetTerminalH460NATFwOption(int option)
{
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

#else
	m_nUseFWNAT = option;
#endif

}



int H323Call::GetVideoFormatValue(VIDEO_FORMAT_TYPE formatType)
{
	return GetMgrCapability().GetVideoFormatValue(formatType);
}

float H323Call::GetFrameRateFromH264Param(int nMaxFS,int nMBPS)
{
	return GetMgrCapability().GetFrameRateFromH264Param(nMaxFS,nMBPS);
}

VIDEO_FORMAT_TYPE H323Call::GetH264FormatTypeFromBitrate(float nFrame,int nProfile,int nBitrate)
{
	return GetMgrCapability().GetH264FormatTypeFromBitrate(nFrame,nProfile,nBitrate);
}

VIDEO_FORMAT_TYPE H323Call::GetCallFormatTypeFromH264Param(int nProfile,int nLevel,int nMaxFS,int nMBPS,int nBitrate,int nType) //nType 0 get min ,1 get max
{
	if( (nLevel>=57) && (nProfile==8))
	{
		int nFindPro=-1;
		nFindPro=m_strProductID.find("HUAWEI"); //notice te30 test use productId: HUAWEI TEx0
		if (nFindPro>=0) return VIDEO_FORMAT_1080P;
	}
	return GetMgrCapability().GetFormatTypeFromH264Param(nProfile,nLevel,nMaxFS,nMBPS,nBitrate,nType);
}


VIDEO_FORMAT_TYPE H323Call::GetFormatTypeFromLevel(int nLevel)
{
	return GetMgrCapability().GetFormatTypeFromLevel(nLevel);
}


void H323Call::CopyVideoCapOtherParam(ConfInfo *pConfInfo,int setBitrate,int setFormat)
{
	m_nMainVideoMaxBitrate =GetVideoCapMaxBitrate(pConfInfo,0);
	m_nSecondVideoMaxBitrate =GetVideoCapMaxBitrate(pConfInfo,1);
}

int H323Call::GetVideoCapMaxBitrate(ConfInfo *pConfInfo,int nDual)  //0 is main video ,1 is dual video
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

bool H323Call::GetCallH225LocalSignalAddress(std::string & strIP,int & nPort) //0 src 1 dest
{
	bool bRet=false;
	if(m_nCallDir == IMXEC_CALL_DIR_UNKOWN) return bRet;
	cmTransportAddress CallSignalAddress;
	if(m_nCallDir == IMXEC_CALL_DIR_OUT)
	{
		cmCallParam addrParam = cmParamSrcCallSignalAddress;
		int nError=cmCallGetParam(m_hsCall,addrParam,0,0,(char*)&CallSignalAddress);
		if(nError >= 0)
		{
			if(CallSignalAddress.type == cmTransportTypeIP)
			{
				struct in_addr addr;
				addr.S_un.S_addr =CallSignalAddress.ip;
				strIP=inet_ntoa(addr);
				nPort=CallSignalAddress.port;
				bRet=true;
				OutputDebugString("GetCallH225LocalSignalAddress:\n");
				OutputDebugString(strIP.c_str());
				OutputDebugString("\n");
			}
		}
	}
	return bRet;
}

bool H323Call::GetCallH245LocalSignalAddress(std::string & strIP,int & nPort)
{
	bool bRet=false;
	HPROTCONN hConn = cmGetTpktChanHandle(m_hsCall,cmH245TpktChannel); 
	if(hConn)
	{
		cmTransportAddress tr;
		if(cmProtocolGetLocalAddress(H323CallMgr::Instance().m_hApp, hConn, &tr) >= 0)
		{
			if(tr.type == cmTransportTypeIP)
			{
				struct in_addr		addr;
				addr.S_un.S_addr	=tr.ip;
				strIP=inet_ntoa(addr);
				nPort=tr.port;
				bRet=true;
				OutputDebugString("GetCallH245LocalSignalAddress:\n");
				OutputDebugString(strIP.c_str());
				OutputDebugString("\n");
			}
		}
	}
	return bRet;

}

void H323Call::LogRecvCapDapCaps(void)
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
		sprintf(msg,"H323 Recv Audio Cap Index:%d, codec:%s,  dir:%d,payload:%d,enc:%d,algo:%d\n",i, 
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
		sprintf(msg,"H323 Recv Video Cap Index:%d, codec:%s, format:%s,  dir:%d,formateValue:%d,bitrate:%d,frame:%d,payload:%d,enc:%d,algo:%d,MaxFS:%d,MaxMBPS:%d\n",i, 
		GetVideoCodecName(m_RecvCallCaps.m_aMainVideoCap[i].m_VideoCodecType,nameBuf) ,
		GetVideoFormatName(m_RecvCallCaps.m_aMainVideoCap[i].m_VideoFormatType,nameBuf2) ,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nDirection,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nFormatValue ,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoBitrate,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nVideoFrameRate,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nPayload,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nEncryption,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nAlgorithm,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nMaxFS,
		m_RecvCallCaps.m_aMainVideoCap[i].m_nMaxMBPS );
		OutputDebugString(msg);
		LogFile(NULL,NULL,"","",msg);
	}

	for(i=0; i< m_RecvCallCaps.m_nSecondVideoCapCount; i++)
	{
		sprintf(msg,"H323 Recv Dual Video Cap Index:%d, codec:%s, format:%s,  dir:%d,formateValue:%d,bitrate:%d,frame:%d,payload:%d,enc:%d,algo:%d,MaxFS:%d,MaxMBPS:%d\n",i, 
		GetVideoCodecName(m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoCodecType,nameBuf) ,
		GetVideoFormatName(m_RecvCallCaps.m_aSecondVideoCap[i].m_VideoFormatType,nameBuf2) ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nDirection ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nFormatValue ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoBitrate ,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nVideoFrameRate,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nPayload,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nEncryption,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nAlgorithm,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nMaxFS,
		m_RecvCallCaps.m_aSecondVideoCap[i].m_nMaxMBPS );

		OutputDebugString(msg);
		LogFile(NULL,NULL,"","",msg);
	}

	OutputDebugString("\n");
}

void H323Call::SetCallSourcePhysicalAddress(const char * dest,int type)
{
	std::string strSameIP="";
	bool bGet= false;
	if(type ==0)
	{
		m_strMakeDestIP =dest;
		bGet=GetOneSameDestIP3(CallMgr::Instance().m_AllLocalIPSave,strSameIP,m_strMakeDestIP);
	}
	if(bGet)
	{
		cmTransportAddress tranAddr;
		memset( &tranAddr, 0, sizeof(cmTransportAddress) );
		char addr[256]={0};
		sprintf(addr,"%s:%d",strSameIP.c_str(),0);
		cmString2TransportAddress(addr,&tranAddr);
		cmCallSetParam(m_hsCall,cmParamSourceIpAddress,0, sizeof(cmTransportAddress),(char*)&tranAddr);
		m_strMakeSourceIP =strSameIP;

		char msg[256]={0};
		sprintf(msg,"set call Source Physical Address:%s\n",strSameIP.c_str());
		OutputDebugString(msg);
	}
}

int H323Call::GetMainVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if(type ==0)
	{
		if(m_pMainVideoSendChannel) m_pMainVideoSendChannel->GetSendStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	else if(type ==1)
	{
		if(m_pMainVideoRecvChannel) m_pMainVideoRecvChannel->GetRecvStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

int H323Call::GetDualVideoPacketStatistics(int type,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if(type ==0)
	{
		if(m_pSecondVideoSendChannel && m_nSecondVideoDir==IMXEC_CALL_DIR_OUT) m_pSecondVideoSendChannel->GetSendStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	else if(type ==1)
	{
		if(m_pSecondVideoRecvChannel && m_nSecondVideoDir==IMXEC_CALL_DIR_IN) m_pSecondVideoRecvChannel->GetRecvStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}
	return 0;
}

void H323Call::RecvCapDapCapsSort(VIDEC_CODEC_TYPE codecType)
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

void H323Call::CapabilitisProvisionalNeg(VIDEC_CODEC_TYPE localType ,VIDEC_CODEC_TYPE remoteType,int nLocalPriority,bool bEncryOption)
{
	DapCallCapability	aDAPCallCapability; //本地提取临时能力集
	RecvCallCaps        bRecvCallCaps;      //远端提取临时能力集
	if (m_bHasRemoteVideo ==false)
	{
		aDAPCallCapability.m_nMainVideoCapCount =0;
		for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;ii++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii].m_VideoCodecType == localType)
			{
				memcpy(aDAPCallCapability.m_aMainVideoCap + aDAPCallCapability.m_nMainVideoCapCount, m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap +ii,sizeof(VideoCapability) );
				aDAPCallCapability.m_nMainVideoCapCount ++;
			}
		}
		bRecvCallCaps.m_nMainVideoCapCount =0;
		for (int ii=0;ii <m_RecvCallCaps.m_nMainVideoCapCount;ii++)
		{
			if(m_RecvCallCaps.m_aMainVideoCap[ii].m_VideoCodecType == remoteType)
			{
				memcpy(bRecvCallCaps.m_aMainVideoCap + bRecvCallCaps.m_nMainVideoCapCount, m_RecvCallCaps.m_aMainVideoCap +ii,sizeof(RecvVideoCap) );
				bRecvCallCaps.m_aMainVideoCap[bRecvCallCaps.m_nMainVideoCapCount].m_VideoCodecType = localType;
				bRecvCallCaps.m_nMainVideoCapCount ++;
			}
		}
	}

	if (m_bHasRemoteH239Video ==false)
	{
		aDAPCallCapability.m_nSecondVideoCapCount =0;
		for (int ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii].m_VideoCodecType == localType)
			{
				memcpy(aDAPCallCapability.m_aSecondVideoCap + aDAPCallCapability.m_nSecondVideoCapCount, m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap +ii,sizeof(VideoCapability) );
				aDAPCallCapability.m_nSecondVideoCapCount ++;
			}
		}
		bRecvCallCaps.m_nSecondVideoCapCount =0;
		for (int ii=0;ii <m_RecvCallCaps.m_nSecondVideoCapCount;ii++)
		{
			if(m_RecvCallCaps.m_aSecondVideoCap[ii].m_VideoCodecType == remoteType)
			{
				memcpy(bRecvCallCaps.m_aSecondVideoCap + bRecvCallCaps.m_nSecondVideoCapCount, m_RecvCallCaps.m_aSecondVideoCap +ii,sizeof(RecvVideoCap) );
				bRecvCallCaps.m_aSecondVideoCap[bRecvCallCaps.m_nSecondVideoCapCount].m_VideoCodecType = localType;
				bRecvCallCaps.m_nSecondVideoCapCount ++;
			}
		}
	}

	//Negotiate
	bool bResult =false;
	int ii=0;
	int jj=0;
	NegMediaCap NegItem;
	NegItem.m_bNegMedia=false;

	if(nLocalPriority ==1 || nLocalPriority ==3)
	{
		if (m_bHasRemoteVideo ==false && aDAPCallCapability.m_nMainVideoCapCount>0)
		{
			bResult =false;
			for (ii=0;ii <aDAPCallCapability.m_nMainVideoCapCount;ii++)
			{
				VideoCapability * aCap =&(aDAPCallCapability.m_aMainVideoCap[ii]);
				for (jj=0;jj <bRecvCallCaps.m_nMainVideoCapCount;jj++)
				{
					RecvVideoCap * bCap =&(bRecvCallCaps.m_aMainVideoCap[jj]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					CapabilitisNegMainVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteH239Video ==false && aDAPCallCapability.m_nSecondVideoCapCount>0)
		{
			bResult =false;
			for (ii=0;ii <aDAPCallCapability.m_nSecondVideoCapCount;ii++)
			{
				VideoCapability * aCap =&(aDAPCallCapability.m_aSecondVideoCap[ii]);
				for (jj=0;jj <bRecvCallCaps.m_nSecondVideoCapCount;jj++)
				{
					RecvVideoCap * bCap =&(bRecvCallCaps.m_aSecondVideoCap[jj]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					CapabilitisNegDualVideoResult(NegItem);
					break;
				}
			}
		}
	}
	else 
	{
		if (m_bHasRemoteVideo ==false && aDAPCallCapability.m_nMainVideoCapCount>0)
		{
			bResult =false;
			for (jj=0;jj <bRecvCallCaps.m_nMainVideoCapCount;jj++)
			{
				RecvVideoCap * aCap =&(bRecvCallCaps.m_aMainVideoCap[jj]);
				for (ii=0;ii <aDAPCallCapability.m_nMainVideoCapCount;ii++)
				{
					VideoCapability * bCap =&(aDAPCallCapability.m_aMainVideoCap[ii]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					CapabilitisNegMainVideoResult(NegItem);
					break;
				}
			}
		}

		if (m_bHasRemoteH239Video ==false && aDAPCallCapability.m_nSecondVideoCapCount>0)
		{
			bResult =false;
			for (jj=0;jj <bRecvCallCaps.m_nSecondVideoCapCount;jj++)
			{
				RecvVideoCap * aCap =&(bRecvCallCaps.m_aSecondVideoCap[jj]);
				for (ii=0;ii <aDAPCallCapability.m_nSecondVideoCapCount;ii++)
				{
					VideoCapability * bCap =&(aDAPCallCapability.m_aSecondVideoCap[ii]);
					bResult=CapabilitisVideoItemNeg(aCap,bCap,nLocalPriority,bEncryOption,NegItem);
					if(bResult) break;
				}
				if(bResult)
				{
					CapabilitisNegDualVideoResult(NegItem);
					break;
				}
			}
		}

	}

}

void H323Call::CapabilitisNegMainVideoResult(NegMediaCap & NegItem)
{
	char nameBuf[64]={0};
	char nameBuf2[64]={0};
	char msg[128]={0};
	m_bVideoHasRemoteEncryption = NegItem.m_bNegEncryption;
	m_VideoEncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
	m_nRemoteMainVideoFormatType =(VIDEO_FORMAT_TYPE)NegItem.m_VideoFormatType;
	m_nRemoteMainVideoCodecType = (VIDEC_CODEC_TYPE)NegItem.m_CodecType;
	m_nMainVideoH264Level       = GetMgrCapability().GetVideoH264Level(m_nRemoteMainVideoFormatType);
	m_nRemoteMainVideoMaxBitrate =NegItem.m_nBitrate;
	m_nRemoteMainVideoFrame      = NegItem.m_nVideoFrameRate;
	m_bHasRemoteVideo           = NegItem.m_bNegMedia;
	sprintf(msg,"CapVideoNeg codec:%s,format:%s,bitrate:%d\n",GetVideoCodecName(m_nRemoteMainVideoCodecType,nameBuf),GetVideoFormatName(m_nRemoteMainVideoFormatType,nameBuf2),NegItem.m_nBitrate);
	OutputDebugString(msg);
}

void H323Call::CapabilitisNegDualVideoResult(NegMediaCap & NegItem)
{
	char nameBuf[64]={0};
	char nameBuf2[64]={0};
	char msg[128]={0};
	m_bH239HasRemoteEncryption = NegItem.m_bNegEncryption;
	m_H239EncDecAlgoId         = (EncDecAlgorithmID)NegItem.m_nAlgorithm;
	m_nRemoteH239VideoFormatType = (VIDEO_FORMAT_TYPE)NegItem.m_VideoFormatType;
	m_nRemoteH239VideoCodecType = (VIDEC_CODEC_TYPE)NegItem.m_CodecType;
	m_nH239VideoH264Level       = GetMgrCapability().GetVideoH264Level(m_nRemoteH239VideoFormatType);
	m_nRemoteH239VideoMaxBitrate =NegItem.m_nBitrate;
	m_nRemoteSecondVideoFrame    = NegItem.m_nVideoFrameRate;
	m_bHasRemoteH239Video       = NegItem.m_bNegMedia;
	sprintf(msg,"CapDualVideoNeg codec:%s,format:%s,bitrate:%d\n",GetVideoCodecName(m_nRemoteH239VideoCodecType,nameBuf),GetVideoFormatName(m_nRemoteH239VideoFormatType,nameBuf2),NegItem.m_nBitrate);
	OutputDebugString(msg);
}

void H323Call::DapChanegCallCaps(ConfInfo *pConfInfo)
{
	if (pConfInfo)
	{
		m_CallCapability.RemoveAllCapabilitis();
		m_EncryptionCapability.RemoveAllCapabilitis();
		memcpy(&m_ConfInfo,pConfInfo,sizeof(ConfInfo));
		ConfCapabilitisDefault();
		BuildSendLocalCapabilitis();
		OnRcvH245CapabilitiesSetChange(Enum_Channel_Media_Audio);
		OnRcvH245CapabilitiesSetChange(Enum_Channel_Media_Video);
		OnRcvH245CapabilitiesSetChange(Enum_Channel_Media_DualVideo);
		OnRcvH245CapabilitiesSetChange(Enum_Channel_Media_H224);
	}
}

bool H323Call::CheckSetH264FormatFromProductId(VIDEO_FORMAT_TYPE & nFormat)
{
	int nFindPro=-1;
	int nFindVer=-1;
	bool bFind=false;
	nFindPro=m_strProductID.find("Polycom RMX 2000");
	nFindVer=m_strVersionID.find("7.0.2");
	if (nFindPro>=0 && nFindVer>=0) bFind=true;
	if(bFind)
	{
		nFormat =VIDEO_FORMAT_CIF;
	}
	return bFind;
}


void H323Call::ConfCapabilitisBitrateDefault(ConfInfo *pConfInfo)
{
	if(pConfInfo)
	{
		for (int ii=0;ii < pConfInfo->m_DAPCallCapability.m_nMainVideoCapCount;ii++)
		{
			pConfInfo->m_DAPCallCapability.m_aMainVideoCap[ii].m_nVideoBitrate =m_nMainVideoMaxBitrate;
		}

		for (int ii=0;ii < pConfInfo->m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
		{
			pConfInfo->m_DAPCallCapability.m_aSecondVideoCap[ii].m_nVideoBitrate =m_nSecondVideoMaxBitrate;
		}
	}

}

void H323Call::SetBitrateFromCaller(int nBitrate)
{
	m_nMainVideoMaxBitrate =nBitrate;
	m_nSecondVideoMaxBitrate =nBitrate;
}

void H323Call::TCSUseTerminalVideoConfCaps(void)
{
	memcpy(&m_TCSTerminalConfInfo,&m_ConfInfo,sizeof(m_ConfInfo));
	m_TCSTerminalConfInfo.m_DAPCallCapability.m_nMainVideoCapCount =0;
	m_TCSTerminalConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount =0;

	m_nTCSUseTerminal =1;
	int nDirection =m_nCapDirectionDefault;

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
	strcat(szPath,"MCUCapMedia.ini");

	VIDEC_CODEC_TYPE videoCodecPri;
	VIDEO_FORMAT_TYPE videoFormatPri;
	int nVideoFrameRate = VideoFrameBase;
	int nVideoBitrate ;
	int nDual=0;

	//add main video caps
	nDual=0;
	videoCodecPri=(VIDEC_CODEC_TYPE)GetPrivateProfileInt("CapMedia","MainVideoCodec",VIDEC_CODEC_H264,szPath);
	videoFormatPri=(VIDEO_FORMAT_TYPE)GetPrivateProfileInt("CapMedia","MainVideoFormat",VIDEO_FORMAT_4CIF,szPath);
	nVideoBitrate=GetPrivateProfileInt("CapMedia","MainVideoBitrate",384,szPath);
	nVideoFrameRate=GetPrivateProfileInt("CapMedia","MainVideoFrameRate",30,szPath);
	if(videoCodecPri >0) AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);

	//add dual video caps
	nDual=GetPrivateProfileInt("CapMedia","SupportDualVideo",0,szPath);
	if(nDual >0 && m_nH225Version >=4)
	{
		videoCodecPri=(VIDEC_CODEC_TYPE)GetPrivateProfileInt("CapMedia","DualVideoCodec",VIDEC_CODEC_H264,szPath);
		videoFormatPri=(VIDEO_FORMAT_TYPE)GetPrivateProfileInt("CapMedia","DualVideoFormat",VIDEO_FORMAT_4CIF,szPath);
		nVideoBitrate=GetPrivateProfileInt("CapMedia","DualVideoBitrate",384,szPath);
		nVideoFrameRate=GetPrivateProfileInt("CapMedia","DualVideoFrameRate",30,szPath);
		nDual=1;
		if(videoCodecPri >0) AddVideoConfInfoBaseCaps2(videoCodecPri,videoFormatPri,nVideoFrameRate,nDirection,nVideoBitrate,nDual);
	}
}

bool H323Call::CheckConfCapsSupported(ChannelMediaType nMedia,VIDEC_CODEC_TYPE codec,VIDEO_FORMAT_TYPE format)
{
	bool bRet=true;
	int ii=0;
	if(nMedia == Enum_Channel_Media_Video)
	{
		for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount;ii++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aMainVideoCap[ii].m_VideoCodecType ==codec)
			{
				break;
			}
		}
		if(ii >=m_ConfInfo.m_DAPCallCapability.m_nMainVideoCapCount) bRet=false;
	}
	else if(nMedia == Enum_Channel_Media_DualVideo)
	{
		for (ii=0;ii <m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount;ii++)
		{
			if(m_ConfInfo.m_DAPCallCapability.m_aSecondVideoCap[ii].m_VideoCodecType ==codec)
			{
				break;
			}
		}
		if(ii >=m_ConfInfo.m_DAPCallCapability.m_nSecondVideoCapCount) bRet=false;
	}

	return bRet;
}
