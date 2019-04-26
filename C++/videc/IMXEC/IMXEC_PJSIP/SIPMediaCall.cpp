#include "SIPMediaCall.h"
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
		MediaCall::CloseLocalAudioCapDev();
		MediaCall::CloseRemoteAudioPlayChan();
	}
	if(m_bNegCapVideo)
	{
		m_nMainChannelState=0;
		MediaCall::CloseLocalMainVideoCapDev();
		MediaCall::CloseRemoteMainVideoPlayChan();
	}
	if(m_bNegCapDualVideo)
	{
		m_nDualChannelState=0;
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
	return MediaCall::OpenLocalAudioCapDev(AudioCodecID,nPayloadType);
}

int SIPMediaCall::OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType)
{
	return MediaCall::OpenRemoteAudioPlayChan(AudioCodecID,nPayloadType);
}

int SIPMediaCall::OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nNetworkType)
{
	m_nVideoWidthBack=nWidth;
	m_nVideoHeightBack=nHeight;
	m_nFrameRateBack=nFrameRate;
	m_nCodecTypeBack=codecType;
	m_nVideoBitrateBack=nBitrate;
	m_nVideoPayloadTypeBack=nPayloadType;
	m_nNetworkTypeBack=nNetworkType;

	return MediaCall::OpenLocalMainVideoCapDev(nDevID,nWidth,nHeight,nFrameRate,codecType,nBitrate,nPayloadType,nNetworkType);
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

	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
	{
		m_nMainVideoPacketTimestamp=nTimestamp;
		RequestMainVideoKeyFrame();
	}
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
	m_XBitrateLocalMainVideo.Calculate(nRTPPacketLen);

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
	m_XBitrateSecondVideo.Calculate(nRTPPacketLen);

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
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
	{
		m_nSecondVideoPacketTimestamp=nTimestamp;
		RequestSecondVideoKeyFrame();
	}
}



int SIPMediaCall::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	return SIPCall::GetSecondVideoDirection(nDir);
}

void SIPMediaCall::OnSecondVideoSendChannelDestroyed(void)
{
	m_nLocalSecondVideoStarted=0;
	CloseLocalSecondVideoCapDev();
}

void SIPMediaCall::OpenLocalSecondVideo(void)
{
	m_nLocalSecondVideoStarted=1;
	VIDEC_CODEC_TYPE codecType=VIDEC_CODEC_H263;
	int nDevID=SIPCallMgr::Instance().GetSecondVideoDevID(); //m_nDualVideoDevID;
	int nNetworkType=SIPCallMgr::Instance().GetSecondVideoNetworkType();
	int nFrameRate=m_nDualVideoFrameRate;

	unsigned int nRemoteVideoPort=0;
	int nVideoPayloadType=0;
	int nVideoBitrate=0;
	int nVideoWidth=0;
	int nVideoHeight=0;
	std::string strVideoPayloadName="";
	bool bGotCodecID=false;

	if(m_bHasRemoteH239Video)
	{
		std::string strRemoteIP=m_strRemoteIP;
		nRemoteVideoPort=m_nRemoteDualVideoPort;
		nVideoPayloadType=m_nDualVideoPayloadType;
		strVideoPayloadName=m_strDualVideoPayloadName;
		nVideoBitrate=m_nDualVideoBitrate;
		nVideoWidth=m_nDualVideoWidth;
		nVideoHeight=m_nDualVideoHeight;
		CheckZeroSetBitrate(nVideoWidth,nVideoHeight,nVideoBitrate);
		if(nRemoteVideoPort<=0) return ;

		if (nVideoPayloadType<255)
		{
			std::string strCodeName="";

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
		}
	}
	else
	{
		bGotCodecID=true;
		nVideoWidth=m_nVideoWidthBack;
		nVideoHeight=m_nVideoHeightBack;
		nFrameRate=m_nFrameRateBack;
		codecType=(VIDEC_CODEC_TYPE)m_nCodecTypeBack;
		nVideoBitrate=m_nVideoBitrateBack;
		nVideoPayloadType=m_nVideoPayloadTypeBack;
		nNetworkType=m_nNetworkTypeBack;
		CheckZeroSetBitrate(nVideoWidth,nVideoHeight,nVideoBitrate);
	}

	if (bGotCodecID)
	{
		SIPMediaCall::OpenLocalSecondVideoCapDev(nDevID,nVideoWidth,nVideoHeight,nFrameRate,codecType,nVideoBitrate/*,nVideoPayloadType*/,nNetworkType);
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
	else if (nTimestamp-m_nLastRcvdPacketTimestamp>= nCheckTimer) //notice 30 seconds
	{
		m_nLastRcvdPacketTimestamp=nTimestamp;
		return true;
	}

	return false;
}

void SIPMediaCall::OpenLocalMainVideoCapDev(void)
{
	int nVideoPayloadType=m_nMainVideoPayloadType;
	std::string strVideoPayloadName=m_strMainVideoPayloadName;
	int nVideoBitrate=m_nMainVideoBitrate;
	int nVideoWidth=m_nMainVideoWidth;
	int nVideoHeight=m_nMainVideoHeight;
	int nClockRate=90000;
	int nSamplePerFrame=0 ;

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

		if (bGotCodecID && m_nVideoOpenState==2)
		{
			m_nVideoWidthBack=nVideoWidth;
			m_nVideoHeightBack=nVideoHeight;
			m_nFrameRateBack=nFrameRate;
			m_nCodecTypeBack=codecType;
			m_nVideoBitrateBack=nVideoBitrate;
			m_nVideoPayloadTypeBack=nVideoPayloadType;
			m_nNetworkTypeBack=nNetworkType;
			OpenLocalMainVideoCapDev(nDevID,nVideoWidth,nVideoHeight,nFrameRate,codecType,nVideoBitrate,nVideoPayloadType,nNetworkType);
		}
	}

}

void SIPMediaCall::OpenRemoteMainPlayChan(void)
{
	//notice the not definition
	//if (HasAVCONTransport())
	//{
	//	//MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort);
	//	MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort,m_strPeerMCUID, m_nEnableMulticast);
	//}
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
