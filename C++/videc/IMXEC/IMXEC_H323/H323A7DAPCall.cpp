#include "H323CallMgr.h"
#include "H323A7DAPCall.h"
#include "XSocket.h"
#include "../IMXEC_Util/XUtil.h"
#include <NETEC/NETEC_Core.h>
#include "CallMgr.h"

H323MediaCall::H323MediaCall(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo)
: H323Call(rH323CallMgr,hsCall,pConfInfo)
{
	m_nLocalSecondVideoStarted=0;
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
}

H323MediaCall::~H323MediaCall(void)
{

}

void H323MediaCall::Close(void)
{
	H323Call::Close();
	MediaCall::Close();
}

void H323MediaCall::OnAudioRecvChannelConnected(void)
{
	if (m_pAudioRecvChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet=m_pAudioRecvChannel->GetChannelInfo(tChannelInfo);
		if (nRet>=0)
		{
			X_AUDIO_CODEC_ID AudioCodecID=tChannelInfo.AudioCodecID;
			int nPayloadType=tChannelInfo.nPayloadType;
			H323MediaCall::OpenRemoteAudioPlayChan(AudioCodecID,nPayloadType);

			//add Modified by gshzh 2013-9-27
			CallMgr::Instance().OnDAP_CallAudioRecvChannelConnected/*OnAudioRecvChannelInfo*/(m_strCallID.c_str(),AudioCodecID,0,0,0);
		}
	}
}

void H323MediaCall::OnAudioRecvChannelDestroyed(void)
{
	H323MediaCall::CloseRemoteAudioPlayChan();
}

void H323MediaCall::OnMainVideoRecvChannelConnected(void)
{
	if (m_pMainVideoRecvChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet=m_pMainVideoRecvChannel->GetChannelInfo(tChannelInfo);
		if (nRet>=0)
		{
			VIDEC_CODEC_TYPE codecType=tChannelInfo.VideoCodecType;
			int nBitrate=m_nRemoteMainVideoMaxBitrate;
			int nPayloadType=tChannelInfo.nPayloadType;
			if (codecType!=VIDEC_CODEC_COUNT)
			{
				H323MediaCall::OpenRemoteMainVideoPlayChan(codecType,nBitrate,nPayloadType);

				//add Modified by gshzh 2013-9-27
				CallMgr::Instance().OnDAP_CallMainVideoRecvChannelConnected/*OnMainVideoRecvChannelInfo*/(m_strCallID.c_str(),codecType,nBitrate,0,0,0,tChannelInfo.VideoFormatType,0);
			}
		}
	}
}

void H323MediaCall::OnMainVideoRecvChannelDestroyed(void)
{
	H323MediaCall::CloseRemoteMainVideoPlayChan();
}

void H323MediaCall::OnSecondVideoRecvChannelConnected(void)
{
	if (m_pSecondVideoRecvChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet=m_pSecondVideoRecvChannel->GetChannelInfo(tChannelInfo);
		if (nRet>=0)
		{
			VIDEC_CODEC_TYPE codecType=tChannelInfo.VideoCodecType;
			int nBitrate=m_nRemoteH239VideoMaxBitrate;
			int nPayloadType=tChannelInfo.nPayloadType;
			if (codecType!=VIDEC_CODEC_COUNT)
			{
				H323MediaCall::OpenRemoteSecondVideoPlayChan(codecType,nBitrate,nPayloadType);
				//add Modified by gshzh 2013-9-27
				CallMgr::Instance().OnDAP_CallSecondVideoRecvChannelConnected/*OnSecondVideoRecvChannelInfo*/(m_strCallID.c_str(),codecType,nBitrate,0,0,0,tChannelInfo.VideoFormatType,0);

				//CallMgr::Instance().OnSecondVideoRecvChannelInfo(m_strCallID.c_str(),codecType,tChannelInfo.VideoFormatType,nBitrate);
			}
		}
	}
}

void H323MediaCall::OnSecondVideoRecvChannelDestroyed(void)
{
	H323MediaCall::CloseRemoteSecondVideoPlayChan();
}

void H323MediaCall::OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteAudioRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
}

void H323MediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MediaCall::OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	unsigned long nTimestamp=XGetTimestamp();
	m_nLastRcvdPacketTimestamp=nTimestamp;
}

void H323MediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MediaCall::OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

	unsigned long nTimestamp=XGetTimestamp();
	m_nLastRcvdPacketTimestamp=nTimestamp;
}

void H323MediaCall::OnRecvdMainVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestLocalMainVideoKeyFrame();
}

void H323MediaCall::OnRecvdSecondVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestSecondVideoKeyFrame();
}

void H323MediaCall::OnMainVideoSendChannelConnected(void)
{
	if(m_pMainVideoSendChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet = m_pMainVideoSendChannel->GetChannelInfo(tChannelInfo);
		if (nRet==0)
		{
			VIDEC_CODEC_TYPE codecType=tChannelInfo.VideoCodecType;
			int nPlayloadType=tChannelInfo.nPayloadType;

			int nWidth=352;
			int nHeight=288;

			VIDEO_FORMAT_TYPE VideoFormatType=tChannelInfo.VideoFormatType;
			int nBitrate=tChannelInfo.nMaxBitrate;
			switch (VideoFormatType)
			{
			case VIDEO_FORMAT_QCIF:
				{
					nWidth=176;
					nHeight=144;
				}
				break;
			case VIDEO_FORMAT_CIF:
				{
					nWidth=352;
					nHeight=288;
				}
				break;
			case VIDEO_FORMAT_4CIF:
				{
					nWidth=704;
					nHeight=576;
				}
				break;
			case VIDEO_FORMAT_16CIF:
				{
					nWidth=1408;
					nHeight=1152;
				}
				break;
			case VIDEO_FORMAT_720I:
				{
					nWidth=1280;
					nHeight=360;
				}
				break;
			case VIDEO_FORMAT_720P:
				{
					nWidth=1280;
					nHeight=720;
				}
				break;
			case VIDEO_FORMAT_1080I:
				{
					nWidth=1920;
					nHeight=540;
				}
				break;
			case VIDEO_FORMAT_1080P:
				{
					nWidth=1920;
					nHeight=1080;
				}
				break;
			case VIDEO_FORMAT_VGA:
				{
					nWidth=640;
					nHeight=480;
				}
				break;
			case VIDEO_FORMAT_SVGA:
				{
					nWidth=800;
					nHeight=600;
				}
				break;
			case VIDEO_FORMAT_XGA:
				{
					nWidth=1024;
					nHeight=768;
				}
				break;
			case VIDEO_FORMAT_SXGA:
				{
					nWidth=1280;
					nHeight=1024;
				}
				break;
			default:
				{
					if (nBitrate<=384)
					{
						nWidth=176;
						nHeight=144;
					}
					else if (nBitrate<=512)
					{
						nWidth=352;
						nHeight=288;
					}
					else if (nBitrate<=1024)
					{
						nWidth=704;
						nHeight=576;
					}
					else if (nBitrate<=2048)
					{
						nWidth=1280;
						nHeight=720;
					}
					else
					{
						nWidth=1920;
						nHeight=1080;
					}
				}
				break;
			}

			if (codecType!=VIDEC_CODEC_COUNT)
			{
				OpenLocalMainVideoCapChan(nWidth,nHeight,codecType,nBitrate,nPlayloadType);
				//add Modified by gshzh 2013-9-27
				CallMgr::Instance().OnDAP_CallMainVideoSendChannelConnected/*OnMainVideoSendChannelInfo*/(NULL,m_strCallID.c_str(),codecType,nBitrate,0,0,0,tChannelInfo.VideoFormatType,0);
				//CallMgr::Instance().OnMainVideoSendChannelInfo(m_strCallID.c_str(),codecType,tChannelInfo.VideoFormatType,nBitrate);
			}
		}
	}
}

void H323MediaCall::OnMainVideoSendChannelDestroyed(void)
{
	CloseLocalMainVideoCapChan();
}


void H323MediaCall::OnAudioSendChannelConnected(void)
{
	if(m_pAudioSendChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet = m_pAudioSendChannel->GetChannelInfo(tChannelInfo);
		if (nRet==0)
		{
			X_AUDIO_CODEC_ID AudioCodecID=tChannelInfo.AudioCodecID;
			int nPayloadType=tChannelInfo.nPayloadType;

			OpenLocalAudioCapChan(AudioCodecID,nPayloadType);
			//add Modified by gshzh 2013-9-27
			CallMgr::Instance().OnDAP_CallAudioSendChannelConnected/*OnAudioSendChannelInfo*/(NULL,m_strCallID.c_str(),AudioCodecID,0,0,0);

			//CallMgr::Instance().OnAudioSendChannelInfo(m_strCallID.c_str(),AudioCodecID);
		}
	}
}

void H323MediaCall::OnAudioSendChannelDestroyed(void)
{
	CloseLocalAudioCapChan();
}

void H323MediaCall::OnSecondVideoSendChannelConnected(void)
{
	if(m_pSecondVideoSendChannel!=NULL)
	{
		CHANNEL_INFO tChannelInfo;
		int nRet = m_pSecondVideoSendChannel->GetChannelInfo(tChannelInfo);
		if (nRet==0)
		{
			VIDEC_CODEC_TYPE codecType=tChannelInfo.VideoCodecType;
			int nPlayloadType=tChannelInfo.nPayloadType;

			int nWidth=352;
			int nHeight=288;

			VIDEO_FORMAT_TYPE VideoFormatType=tChannelInfo.VideoFormatType;
			int nBitrate=tChannelInfo.nMaxBitrate;
			switch (VideoFormatType)
			{
			case VIDEO_FORMAT_QCIF:
				{
					nWidth=176;
					nHeight=144;
				}
				break;
			case VIDEO_FORMAT_CIF:
				{
					nWidth=352;
					nHeight=288;
				}
				break;
			case VIDEO_FORMAT_4CIF:
				{
					nWidth=704;
					nHeight=576;
				}
				break;
			case VIDEO_FORMAT_16CIF:
				{
					nWidth=1408;
					nHeight=1152;
				}
				break;
			case VIDEO_FORMAT_720I:
				{
					nWidth=1280;
					nHeight=360;
				}
				break;
			case VIDEO_FORMAT_720P:
				{
					nWidth=1280;
					nHeight=720;
				}
				break;
			case VIDEO_FORMAT_1080I:
				{
					nWidth=1920;
					nHeight=540;
				}
				break;
			case VIDEO_FORMAT_1080P:
				{
					nWidth=1920;
					nHeight=1080;
				}
				break;
			case VIDEO_FORMAT_VGA:
				{
					nWidth=640;
					nHeight=480;
				}
				break;
			case VIDEO_FORMAT_SVGA:
				{
					nWidth=800;
					nHeight=600;
				}
				break;
			case VIDEO_FORMAT_XGA:
				{
					nWidth=1024;
					nHeight=768;
				}
				break;
			case VIDEO_FORMAT_SXGA:
				{
					nWidth=1280;
					nHeight=1024;
				}
				break;
			default:
				{
					if (nBitrate<=384)
					{
						nWidth=176;
						nHeight=144;
					}
					else if (nBitrate<=512)
					{
						nWidth=352;
						nHeight=288;
					}
					else if (nBitrate<=1024)
					{
						nWidth=704;
						nHeight=576;
					}
					else if (nBitrate<=2048)
					{
						nWidth=1280;
						nHeight=720;
					}
					else
					{
						nWidth=1920;
						nHeight=1080;
					}
				}
				break;
			}

			if (codecType!=VIDEC_CODEC_COUNT)
			{
				OpenLocalSecondVideoCapChan(nWidth,nHeight,codecType,nBitrate,nPlayloadType);
				//add Modified by gshzh 2013-9-27
				CallMgr::Instance().OnDAP_CallSecondVideoSendChannelConnected/*OnSecondVideoSendChannelInfo*/(NULL,m_strCallID.c_str(),codecType,nBitrate,0,0,0,tChannelInfo.VideoFormatType,0);

				//CallMgr::Instance().OnSecondVideoSendChannelInfo(m_strCallID.c_str(),codecType,tChannelInfo.VideoFormatType,nBitrate);
			}
		}
	}
}

void H323MediaCall::OnSecondVideoSendChannelDestroyed(void)
{
	m_nLocalSecondVideoStarted=0;
	CloseLocalSecondVideoCapChan();
}

void H323MediaCall::OnLocalSecondVideoStarted(void)
{
	m_nLocalSecondVideoStarted=1;

	H323Call::OnLocalSecondVideoStarted();
}

void H323MediaCall::OnLocalSecondVideoStop(void)
{
	OnSecondVideoSendChannelDestroyed();
	H323Call::OnLocalSecondVideoStop();
}


void H323MediaCall::SendAudio(char*pRTPPacketData,int nRTPPacketLen)
{
	H323Call::SendAudio(pRTPPacketData,nRTPPacketLen);
}

void H323MediaCall::SendMainVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	H323Call::SendMainVideo(pRTPPacketData,nRTPPacketLen);
}

void H323MediaCall::SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)
{
	H323Call::SendSecondVideo(pRTPPacketData,nRTPPacketLen);
}

int H323MediaCall::RequestMainVideoKeyFrame(void)
{
	return H323Call::RequestMainVideoKeyFrame();
}

int H323MediaCall::RequestSecondVideoKeyFrame(void)
{
	return H323Call::RequestSecondVideoKeyFrame();
}

int H323MediaCall::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	return H323Call::GetSecondVideoDirection(nDir);
}

bool H323MediaCall::HasAVCONTransport(void)
{
	if (NETEC_Core::GetUDPPort()==0)
	{
		return false;
	}
	//if (m_nPeerAudioID && m_nPeerVideoID/* && m_strPeerLocalIP.size() && m_nPeerLocalPort*/)
	if (m_nPeerAudioID && m_nPeerVideoID && m_nPeerSecondVideoID/* && m_strPeerLocalIP.size() && m_nPeerLocalPort*/)
	{
		return true;
	}

	return false;
}

bool H323MediaCall::IsExpired(int nCheckTimer) //notice 30 seconds)
{
	if (HasAVCONTransport())
	{
		return false;
	}

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

void H323MediaCall::OpenReceiverFromRemote(void)
{
	if (HasAVCONTransport())
	{
		unsigned long ulLocalIP=H323CallMgr::Instance().GetLocalIPAddress(m_strLocalIP);
		std::string strMCUIP=XSocket::htoa(ntohl(ulLocalIP));
		//OutputDebugString("GetLocalIPAddress strMCUIP:\n");
		//OutputDebugString(strMCUIP.c_str());
		//OutputDebugString("\n");
		int nPort=0;
		GetCallH245LocalSignalAddress(strMCUIP,nPort);
		unsigned short nMCUPort=NETEC_Core::GetUDPPort();
		std::string strMCUID=H323CallMgr::Instance().m_strMCUID;

		if (m_strMCUID == "")
		{
			// 接的是plink图像
			MediaCall::OpenReceiverFromRemote(m_strPeerNodeID.c_str(),m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_nPeerAudioID,m_nPeerVideoID,m_nPeerSecondVideoID,strMCUIP.c_str(),nMCUPort,strMCUID.c_str());
			//MediaCall::OpenReceiverFromRemote(m_strPeerNodeID.c_str(),m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_nPeerAudioID,m_nPeerVideoID,strMCUIP.c_str(),nMCUPort,strMCUID.c_str());
			//MediaCall::OpenReceiverFromRemote(m_strPeerNodeID.c_str(),m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_nPeerAudioID,m_nPeerVideoID,strMCUIP.c_str(),nMCUPort,"LOCAL_MCU");
		}
		else
		{
			// 接的是级联MCU的图像
			MediaCall::OpenReceiverFromRemote(m_strPeerNodeID.c_str(),m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_nPeerAudioID,m_nPeerVideoID,m_nPeerSecondVideoID,strMCUIP.c_str(),nMCUPort,m_strMCUID.c_str());
			//MediaCall::OpenReceiverFromRemote(m_strPeerNodeID.c_str(),m_strPeerLocalIP.c_str(),m_nPeerLocalPort,m_nPeerAudioID,m_nPeerVideoID,strMCUIP.c_str(),nMCUPort,m_strMCUID.c_str());
		}
	}
}


//notice netec send ptz 
int H323MediaCall::NETECSendCtrl(void*pData,int nLen)
{
	return MediaCall::MediaSendCtrl(pData,nLen);
}

int H323MediaCall::NETECSendPTZ(void*pData,int nLen)
{
	return MediaCall::MediaSendPTZ(pData,nLen);
}

void H323MediaCall::OnNETEC_RecvdPTZ(void*pData,int nLen)
{
	H323Call::OnNETECRecvdPTZ(pData,nLen);
}

void H323MediaCall::OnNETEC_RecvdCtrl(void*pData,int nLen)
{
	H323Call::OnNETECRecvdCtrl(pData,nLen);
}

int H323MediaCall::OnNETECRequestMainVideoKeyFrame(void)
{
	//H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
	return 0;
}

int H323MediaCall::OnNETECRequestSecondVideoKeyFrame(void)
{
	//H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),2);
	return 0;
}
