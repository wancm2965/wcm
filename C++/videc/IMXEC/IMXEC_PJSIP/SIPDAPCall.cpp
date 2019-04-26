#include "SIPDAPCall.h"
#include "SIPCallMgr.h"

#include "../IMXEC_Util/XUtil.h"

SIPMediaCall::SIPMediaCall(SIPUser&rSIPUser,const std::string&strCallID,const std::string&strLocalIP,unsigned short nLocalAudioPort,unsigned short nLocalVideoPort,ConfInfo*pConfInfo)
: SIPCall(rSIPUser,strCallID,strLocalIP,nLocalAudioPort,nLocalVideoPort,pConfInfo)
{
	m_nMainVideoPacketTimestamp=0;
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
}


SIPMediaCall::~SIPMediaCall(void)
{

}

void SIPMediaCall::Close(void)
{
	SIPCall::Close();
	MediaCall::Close();

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	int nTotal=4;//notice
	for(int i=1;i<=nTotal;i++)
	{
		SIPCallMgr::Instance().OnDAP_CallMediaChannelDestroyed(i,1,m_strCallID.c_str());
		SIPCallMgr::Instance().OnDAP_CallMediaChannelDestroyed(i,2,m_strCallID.c_str());
	}
#endif

}

//仅关闭音视频媒体库 不关闭显示窗口
void SIPMediaCall::CloseMedia(void)
{
	if(m_pAudioRTPSession) m_pAudioRTPSession->PacketRecvPause();
	if(m_pVideoRTPSession) m_pVideoRTPSession->PacketRecvPause();
	if(m_pDualVideoRTPSession) m_pDualVideoRTPSession->PacketRecvPause();

	if(m_bNegCapAudio)
	{
		CloseLocalAudioCapChan();
		//MediaCall::CloseLocalAudioCapDev();
		MediaCall::CloseRemoteAudioPlayChan();
	}
	if(m_bNegCapVideo)
	{
		CloseLocalMainVideoCapChan();
		//MediaCall::CloseLocalMainVideoCapDev();
		MediaCall::CloseRemoteMainVideoPlayChan();
	}
	if(m_bNegCapDualVideo)
	{
		OnSecondVideoRecvChannelDestroyed();
		OnSecondVideoSendChannelDestroyed();
	}

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
	int nTotal=4;//notice
	for(int i=1;i<=nTotal;i++)
	{
		SIPCallMgr::Instance().OnDAP_CallMediaChannelDestroyed(i,1,m_strCallID.c_str());
		SIPCallMgr::Instance().OnDAP_CallMediaChannelDestroyed(i,2,m_strCallID.c_str());
	}
#endif

}


int SIPMediaCall::OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	OpenLocalAudioCapChan(AudioCodecID,nPayloadType);
	//return MediaCall::OpenLocalAudioCapDev(AudioCodecID,nPayloadType);
	return 0;
}

int SIPMediaCall::OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	return MediaCall::OpenRemoteAudioPlayChan(AudioCodecID,nPayloadType);
}

//int SIPMediaCall::OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType)
int SIPMediaCall::OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nNetworkType)
{
	//return MediaCall::OpenLocalMainVideoCapDev(nDevID,nWidth,nHeight,nFrameRate,codecType,nBitrate,nPayloadType);
	OpenLocalMainVideoCapChan(nWidth,nHeight,codecType,nBitrate,nPayloadType);
	return 0;
}

int SIPMediaCall::OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType)
{
	return MediaCall::OpenRemoteMainVideoPlayChan(codecType,nBitrate,nVideoPayloadType);
}


void SIPMediaCall::OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteAudioRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}

void SIPMediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	//unsigned long nTimestamp=XGetTimestamp();
	//if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
	//{
	//	m_nMainVideoPacketTimestamp=nTimestamp;
	//	RequestMainVideoKeyFrame();
	//}
}




void SIPMediaCall::OnRecvdMainVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestLocalMainVideoKeyFrame();
}

void SIPMediaCall::OnRecvdSecondVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestSecondVideoKeyFrame();
}



void SIPMediaCall::SendAudio(char*pRTPPacketData,int nRTPPacketLen)
{
	SIPCall::SendAudio(pRTPPacketData,nRTPPacketLen);
}

void SIPMediaCall::SendMainVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	SIPCall::SendMainVideo(pRTPPacketData,nRTPPacketLen);
}


int SIPMediaCall::RequestMainVideoKeyFrame(void)
{
	//return SIPCall::RequestMainVideoKeyFrame();
	int nRet=0;
	if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) nRet =SIPMainVideoSendPictureFastUpdate();
	return nRet;
}


void SIPMediaCall::SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	SIPCall::SendSecondVideo(pRTPPacketData,nRTPPacketLen);
}



int SIPMediaCall::RequestSecondVideoKeyFrame(void)
{
	//return SIPCall::RequestSecondVideoKeyFrame();
	int nRet=0;
	if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) nRet =SIPDualVideoSendPictureFastUpdate();
	return nRet;
}

void SIPMediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	//m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	//unsigned long nTimestamp=XGetTimestamp();
	//if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
	//{
	//	m_nSecondVideoPacketTimestamp=nTimestamp;
	//	RequestSecondVideoKeyFrame();
	//}
}



int SIPMediaCall::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	return SIPCall::GetSecondVideoDirection(nDir);
}

void SIPMediaCall::OnSecondVideoSendChannelDestroyed(void)
{
	m_nLocalSecondVideoStarted=0;
	CloseLocalSecondVideoCapChan();
	//CloseLocalSecondVideoCapDev();
}

void SIPMediaCall::OpenLocalSecondVideo(void)
{
	m_nLocalSecondVideoStarted=1;

	std::string strRemoteIP=m_strRemoteIP;
	unsigned int nRemoteVideoPort=m_nRemoteDualVideoPort;
	int nVideoPayloadType=m_nDualVideoPayloadType;
	std::string&strVideoPayloadName=m_strDualVideoPayloadName;
	int nVideoBitrate=m_nDualVideoBitrate;
	int nVideoWidth=m_nDualVideoWidth;
	int nVideoHeight=m_nDualVideoHeight;
	CheckZeroSetBitrate(nVideoWidth,nVideoHeight,nVideoBitrate);

	if(nRemoteVideoPort<=0) return ;

	if (nVideoPayloadType<255)
	{
		std::string strCodeName="";

		bool bGotCodecID=false;
		VIDEC_CODEC_TYPE codecType=VIDEC_CODEC_H263;
		int nDevID=m_nDualVideoDevID;
		int nFrameRate=m_nDualVideoFrameRate;

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
			OpenLocalSecondVideoCapChan(nVideoWidth,nVideoHeight,codecType,nVideoBitrate,nVideoPayloadType);
			//SIPMediaCall::OpenLocalSecondVideoCapDev(nDevID,nVideoWidth,nVideoHeight,nFrameRate,codecType,nVideoBitrate/*,nVideoPayloadType*/);
		}
	}
}

void SIPMediaCall::OnSecondVideoSendChannelConnected(void)
{
}

void SIPMediaCall::OnSecondVideoRecvChannelConnected(VIDEC_CODEC_TYPE codecType,int nBitrate,int nVideoPayloadType)
{
	SIPMediaCall::OpenRemoteSecondVideoPlayChan(codecType,nBitrate,nVideoPayloadType);
}

void SIPMediaCall::OnSecondVideoRecvChannelDestroyed(void)
{
	SIPMediaCall::CloseRemoteSecondVideoPlayChan();
}

bool SIPMediaCall::IsExpired(int nCheckTimer)
{
	//if (HasAVCONTransport())
	//{
	//	return false;
	//}

	unsigned long nTimestamp=XGetTimestamp();
	if (m_nLastRcvdPacketTimestamp>nTimestamp)
	{
		m_nLastRcvdPacketTimestamp=nTimestamp;
	}
	else if (nTimestamp-m_nLastRcvdPacketTimestamp >= nCheckTimer)
	{
		m_nLastRcvdPacketTimestamp=nTimestamp;
		return true;
	}

	return false;
}

int SIPMediaCall::OnNETECRequestMainVideoKeyFrame(void)
{
	int nRet=0;
	if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) nRet =SIPMainVideoSendPictureFastUpdate();
	return nRet;
}

int SIPMediaCall::OnNETECRequestSecondVideoKeyFrame(void)
{
	int nRet=0;
	if(SIPCallMgr::Instance().m_nSendPicFastUpdate >0 ) nRet =SIPDualVideoSendPictureFastUpdate();
	return nRet;
}
