#include "H323CallMgr.h"
#include "H323MediaCall.h"
#include "../IMXEC_Util/XUtil.h"
#include "../IMXEC_Call/CallMgr.h"
#include <NETEC/NETEC_Core.h>

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <RVINC/rtp.h>
#include <RVINC/rtcp.h>

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

extern void DebugOutput(const char* fmt,...);

H323MediaCall::H323MediaCall(H323CallMgr& rH323CallMgr,HCALL hsCall,ConfInfo*pConfInfo)
: H323Call(rH323CallMgr,hsCall,pConfInfo)
{
	m_nLocalSecondVideoStarted=0;
	m_nMainVideoPacketTimestamp=0;
	m_nSecondVideoPacketTimestamp=0;
	m_nLastRcvdPacketTimestamp=XGetTimestamp();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	m_nMainVideoRequestKeyFrameCount=0;
	m_nSecondVideoRequestKeyFrameCount=0;

#else

	m_nMainVideoRequestKeyFrameCount=0;
	m_nSecondVideoRequestKeyFrameCount=0;

	m_nCallType=IMXEC_PERSONAL_CALL;
	CallMgr::Instance().GetCallType(m_nCallType);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


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

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if (m_nSecondVideoDir==IMXEC_CALL_DIR_IN && m_pSecondVideoRecvChannel!=NULL)

#else

	if (m_pSecondVideoRecvChannel!=NULL)

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

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

#if !defined GUANGZHOU_GOV
void H323MediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MediaCall::OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	//if(H323Call::m_nRemoteMainVideoCodecType ==VIDEC_CODEC_MPEG4) SendMainVideo((char *)pRTPPacketData,nRTPPacketLen); //notice test ,use to call kedacom

	unsigned long nTimestamp=XGetTimestamp();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

if (m_pTranscodeRemoteMain!=NULL)
{
	if (nTimestamp-m_nMainVideoPacketTimestamp>=1000)
	{
		if (m_nMainVideoRequestKeyFrameCount<4)
		{
			m_nMainVideoRequestKeyFrameCount++;

			m_nMainVideoPacketTimestamp=nTimestamp;
			RequestMainVideoKeyFrame();
		}
	}
}
else
{
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))

	if (nTimestamp-m_nMainVideoPacketTimestamp>=6000 * 10 * 5)// Modified by WangLr on 2013-12-19：5分钟请求一次关键帧
#else
	if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
#endif

	{
		m_nMainVideoPacketTimestamp=nTimestamp;
		RequestMainVideoKeyFrame();
	}
}

#else

	if ((IMXEC_Call::GetEnableVideoTranscoding()!=0 || m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	//if ( IMXEC_Call::GetEnableVideoTranscoding() != 0 )
	{
		if (nTimestamp-m_nMainVideoPacketTimestamp>=1000)
		{
			if (m_nMainVideoRequestKeyFrameCount <4)
			{
				m_nMainVideoRequestKeyFrameCount++;
				m_nMainVideoPacketTimestamp=nTimestamp;
				//RequestMainVideoKeyFrame();
				if(m_pMainVideoRecvChannel && m_pMainVideoRecvChannel->m_bUseMediaNetRTP)
					H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
				OutputDebugString("RequestMainVideoKeyFrame()\n");
			}
		}
	}
	//else if ( (m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	//{
	//	if (nTimestamp-m_nMainVideoPacketTimestamp >= 6000 * 5) //modify 20140717 call to huaweiTE30
	//	{
	//		m_nMainVideoPacketTimestamp=nTimestamp;
	//		RequestMainVideoKeyFrame();
	//		OutputDebugString("RequestMainVideoKeyFrame()\n");
	//	}
	//}
	else
	{
		if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
		{
			m_nMainVideoPacketTimestamp=nTimestamp;
				//RequestMainVideoKeyFrame();
				if(m_pMainVideoRecvChannel && m_pMainVideoRecvChannel->m_bUseMediaNetRTP)
					H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
				OutputDebugString("RequestMainVideoKeyFrame()\n");
		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	m_nLastRcvdPacketTimestamp=nTimestamp;
}

void H323MediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//add H239 use
	if(m_bTokenOwnState)
		m_rH323CallMgr.OnRecvdRemoteSecondVideoRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	unsigned long nTimestamp=XGetTimestamp();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

if (m_pTranscodeRemoteSecond!=NULL)
{
	if (nTimestamp-m_nSecondVideoPacketTimestamp>=1000)
	{
		if (m_nSecondVideoRequestKeyFrameCount<4)
		{
			m_nSecondVideoRequestKeyFrameCount++;

			m_nSecondVideoPacketTimestamp=nTimestamp;
			RequestSecondVideoKeyFrame();
		}
	}
}
else
{
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))

	if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000 * 10 * 5)// Modified by WangLr on 2013-12-19：5分钟请求一次关键帧
#else
	if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)

#endif

	{
		m_nSecondVideoPacketTimestamp=nTimestamp;
		RequestSecondVideoKeyFrame();
	}
}

#else

	if ((IMXEC_Call::GetEnableVideoTranscoding()!=0 || m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	{
		if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
		{
			if (m_nSecondVideoRequestKeyFrameCount<4)
			{
				m_nSecondVideoRequestKeyFrameCount++;
				m_nSecondVideoPacketTimestamp=nTimestamp;
				//RequestSecondVideoKeyFrame();
				if(m_pSecondVideoRecvChannel && m_pSecondVideoRecvChannel->m_bUseMediaNetRTP)
					H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),2);
			}
		}		
	}
	else
	{
		if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
		{
			m_nSecondVideoPacketTimestamp=nTimestamp;
				//RequestSecondVideoKeyFrame();
				if(m_pSecondVideoRecvChannel && m_pSecondVideoRecvChannel->m_bUseMediaNetRTP)
					H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),2);

		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


}
////////////////////////////////////////////////////////
//广州市政府项目定制
#else
void H323MediaCall::OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	MediaCall::OnRecvdRemoteMainVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);
	if(H323Call::m_nRemoteMainVideoCodecType ==VIDEC_CODEC_MPEG4) SendMainVideo((char *)pRTPPacketData,nRTPPacketLen); //notice test ,use to call kedacom

	unsigned long nTimestamp=XGetTimestamp();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if (m_pTranscodeRemoteMain!=NULL)
	{
		if (nTimestamp-m_nMainVideoPacketTimestamp>=1000)
		{
			if (m_nMainVideoRequestKeyFrameCount<4)
			{
				m_nMainVideoRequestKeyFrameCount++;

				m_nMainVideoPacketTimestamp=nTimestamp;
				RequestMainVideoKeyFrame();
			}
		}
	}
	else
	{
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))

		if (nTimestamp-m_nMainVideoPacketTimestamp>=6000 * 10 * 5)// Modified by WangLr on 2013-12-19：5分钟请求一次关键帧
#else
		if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
#endif

		{
			m_nMainVideoPacketTimestamp=nTimestamp;
			RequestMainVideoKeyFrame();
		}
	}

#else

	//if ((IMXEC_Call::GetEnableVideoTranscoding()!=0 || m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	////if ( IMXEC_Call::GetEnableVideoTranscoding() != 0 )
	//{
	//	if (nTimestamp-m_nMainVideoPacketTimestamp>=1000)
	//	{
	//		if (m_nMainVideoRequestKeyFrameCount <4)
	//		{
	//			m_nMainVideoRequestKeyFrameCount++;
	//			m_nMainVideoPacketTimestamp=nTimestamp;
	//			RequestMainVideoKeyFrame();
	//			OutputDebugString("RequestMainVideoKeyFrame()\n");
	//		}
	//	}
	//}

	//modify 20160513 call to huaei guangzhou shizhengfu 
	if ( (m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	{
		if (nTimestamp-m_nMainVideoPacketTimestamp > 6000 )
		{
			m_nMainVideoPacketTimestamp=nTimestamp;
			RequestMainVideoKeyFrame();
			OutputDebugString("RequestMainVideoKeyFrame()\n");
		}
	}

	//else if ( (m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	//{
	//	if (nTimestamp-m_nMainVideoPacketTimestamp >= 6000 * 5) //modify 20140717 call to huaweiTE30
	//	{
	//		m_nMainVideoPacketTimestamp=nTimestamp;
	//		RequestMainVideoKeyFrame();
	//		OutputDebugString("RequestMainVideoKeyFrame()\n");
	//	}
	//}
	else
	{
		if (nTimestamp-m_nMainVideoPacketTimestamp>=6000)
		{
			m_nMainVideoPacketTimestamp=nTimestamp;
			RequestMainVideoKeyFrame();
			OutputDebugString("RequestMainVideoKeyFrame()\n");
		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	m_nLastRcvdPacketTimestamp=nTimestamp;
}

void H323MediaCall::OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp)
{
	m_nLastRcvdPacketTimestamp=XGetTimestamp();
	MediaCall::OnRecvdRemoteSecondVideoRTPPacket(pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//add H239 use
	if(m_bTokenOwnState)
		m_rH323CallMgr.OnRecvdRemoteSecondVideoRTPPacket(this,pRTPPacketData,nRTPPacketLen,nPayloadType,nRTPHeaderLen,usSequence,ulTimestamp);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	unsigned long nTimestamp=XGetTimestamp();

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	if (m_pTranscodeRemoteSecond!=NULL)
	{
		if (nTimestamp-m_nSecondVideoPacketTimestamp>=1000)
		{
			if (m_nSecondVideoRequestKeyFrameCount<4)
			{
				m_nSecondVideoRequestKeyFrameCount++;

				m_nSecondVideoPacketTimestamp=nTimestamp;
				RequestSecondVideoKeyFrame();
			}
		}
	}
	else
	{
#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))

		if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000 * 10 * 5)// Modified by WangLr on 2013-12-19：5分钟请求一次关键帧
#else
		if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)

#endif

		{
			m_nSecondVideoPacketTimestamp=nTimestamp;
			RequestSecondVideoKeyFrame();
		}
	}

#else

	//modify 20160513 call to huaei guangzhou shizhengfu 
	//if ((IMXEC_Call::GetEnableVideoTranscoding()!=0 || m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	//{
	//	if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
	//	{
	//		if (m_nSecondVideoRequestKeyFrameCount<4)
	//		{
	//			m_nSecondVideoRequestKeyFrameCount++;
	//			m_nSecondVideoPacketTimestamp=nTimestamp;
	//			RequestSecondVideoKeyFrame();
	//		}
	//	}		
	//}
	if ((IMXEC_Call::GetEnableVideoTranscoding()!=0 || m_nCallType==IMXEC_PERSONAL_CALL) && !m_bRecording)
	{
		if (nTimestamp-m_nSecondVideoPacketTimestamp > 6000)
		{
			//if (m_nSecondVideoRequestKeyFrameCount<4)
			{
				m_nSecondVideoRequestKeyFrameCount++;
				m_nSecondVideoPacketTimestamp=nTimestamp;
				RequestSecondVideoKeyFrame();
			}
		}		
	}
	else
	{
		if (nTimestamp-m_nSecondVideoPacketTimestamp>=6000)
		{
			m_nSecondVideoPacketTimestamp=nTimestamp;
			RequestSecondVideoKeyFrame();
		}
	}

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


}
#endif

void H323MediaCall::OnRecvdMainVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestLocalMainVideoKeyFrame();
}

void H323MediaCall::OnRecvdSecondVideoRequestKeyFrame(void)
{
	MediaCall::OnRequestSecondVideoKeyFrame();
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

			MediaCall::OpenLocalAudioCapDev(AudioCodecID,nPayloadType);
		}
	}
}

void H323MediaCall::OnAudioSendChannelDestroyed(void)
{
	MediaCall::CloseLocalAudioCapDev();
}

void H323MediaCall::OpenLocalMainVideoCapDev(void)
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

			int nDevID=m_rH323CallMgr.GetMainVideoDevID();
			int nFrameRate=m_rH323CallMgr.GetMainVideoDevFrameRate();
			int nNetworkType = m_rH323CallMgr.GetMainVideoNetworkType();
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
					nWidth=176;
					nHeight=144;
				}
				break;
			}


			if(m_nForceVideoWid > 0 && m_nForceVideoHei > 0)
			{
				nWidth = m_nForceVideoWid;
				nHeight = m_nForceVideoHei;
			}

			if (codecType!=VIDEC_CODEC_COUNT)
			{
				MediaCall::OpenLocalMainVideoCapDev(nDevID,nWidth,nHeight,nFrameRate,codecType,nBitrate,nPlayloadType,nNetworkType);
			}
		}
	}
}



void H323MediaCall::OpenLocalSecondVideo(void)
{


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
	if ((m_pSecondVideoSendChannel!=NULL || m_bVirtualMark)
		&& m_nLocalSecondVideoStarted!=0
		)
#else




	if (m_pSecondVideoSendChannel!=NULL && 
		m_nLocalSecondVideoStarted!=0)

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	{
		CHANNEL_INFO tChannelInfo;



#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

//20130322
		int nRet = -1;
		if(m_bVirtualMark)
		{
			nRet = GetChannelInfo(tChannelInfo);
		}
		else 
			nRet = m_pSecondVideoSendChannel->GetChannelInfo(tChannelInfo);

#else



		int nRet = m_pSecondVideoSendChannel->GetChannelInfo(tChannelInfo);
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

		if (nRet==0)
		{
			VIDEC_CODEC_TYPE codecType=tChannelInfo.VideoCodecType;

			int nWidth=352;
			int nHeight=288;

			int nDevID=m_rH323CallMgr.GetSecondVideoDevID();
			int nFrameRate=m_rH323CallMgr.GetSecondVideoDevFrameRate();
			int nNetworkType=m_rH323CallMgr.GetSecondVideoNetworkType();
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
					nWidth=176;
					nHeight=144;
				}
				break;
			}

			if(m_nForceDualVideoWid > 0 && m_nForceDualVideoHei > 0)
			{
				nWidth = m_nForceDualVideoWid;
				nHeight = m_nForceDualVideoHei;
			}

			if (codecType!=VIDEC_CODEC_COUNT)
			{
				H323MediaCall::OpenLocalSecondVideoCapDev(nDevID,nWidth,nHeight,nFrameRate,codecType,nBitrate,nNetworkType);
			}
		}
	}
}

void H323MediaCall::OnSecondVideoSendChannelConnected(void)
{
}
void H323MediaCall::OnSecondVideoSendChannelDestroyed(void)
{
	m_nLocalSecondVideoStarted=0;
	CloseLocalSecondVideoCapDev();
}

void H323MediaCall::OnLocalSecondVideoStarted(void)
{
	m_nLocalSecondVideoStarted=1;
	OpenLocalSecondVideo();

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
	m_XBitrateLocalMainVideo.Calculate(nRTPPacketLen);



#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
	if (HasAVCONTransport())
	{
		MediaCall::SendMainVideo(pRTPPacketData,nRTPPacketLen);
	}
	else
	{
		H323Call::SendMainVideo(pRTPPacketData,nRTPPacketLen);
	}

#else




	H323Call::SendMainVideo(pRTPPacketData,nRTPPacketLen);
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


}

void H323MediaCall::SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)
{


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
	//20130322
	if(m_bVirtualMark)
	{
		//add H239 use
		if(m_bTokenOwnState)
		{
			m_XBitrateSecondVideo.Calculate(nRTPPacketLen);
			H323CallMgr::Instance().RecvExternalH239Media(this,pRTPPacketData,nRTPPacketLen);
		}
	}
	else 
	{
		m_XBitrateSecondVideo.Calculate(nRTPPacketLen);
		if (HasAVCONTransport())
		{
			MediaCall::SendSecondVideo(pRTPPacketData,nRTPPacketLen);
		}
		else
		{
			H323Call::SendSecondVideo(pRTPPacketData,nRTPPacketLen);
		}
	}
#else




	m_XBitrateSecondVideo.Calculate(nRTPPacketLen);
	H323Call::SendSecondVideo(pRTPPacketData,nRTPPacketLen);
#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


}

int H323MediaCall::RequestMainVideoKeyFrame(void)
{
	//DebugOutput("%u [H323MediaCall::RequestMainVideoKeyFrame]\n",GetTickCount());
	return H323Call::RequestMainVideoKeyFrame();
}

int H323MediaCall::RequestSecondVideoKeyFrame(void)
{
	//DebugOutput("%u [H323MediaCall::RequestSecondVideoKeyFrame]\n",GetTickCount());
	return H323Call::RequestSecondVideoKeyFrame();
}

int H323MediaCall::GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir)
{
	return H323Call::GetSecondVideoDirection(nDir);
}
//
//void H323MediaCall::OpenRemoteMainPlayChan(void)
//{
//	if (HasAVCONTransport())
//	{
//
//#if ( ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )
//		MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort);
//
//#else
//
//		MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort,m_strPeerMCUID, m_nEnableMulticast);
//#endif
//
//	}
//}
//
//
//
////#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//
//void H323MediaCall::OpenRemoteSecondPlayChan(void)
//{
//	if (HasAVCONTransport())
//	{
//		MediaCall::OpenRemoteSecondPlayChan(m_nPeerSecondVideoID,m_strPeerLocalIP,m_nPeerLocalPort,m_strPeerMCUID, m_nEnableMulticast);
//	}
//}
//
//void H323MediaCall::CloseRemoteSecondPlayChan()
//{
//	if (HasAVCONTransport())
//	{
//		MediaCall::CloseRemoteSecondPlayChanMedia();
//	}
//}
//
////#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)
//


void H323MediaCall::OpenRemoteMainPlayChan(void)
{
	if (HasAVCONTransport())
	{

#if ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1 ) )
		MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort);

#else

		MediaCall::OpenRemoteMainPlayChan(m_nPeerAudioID,m_nPeerVideoID,m_strPeerLocalIP,m_nPeerLocalPort,m_strPeerMCUID, m_nEnableMulticast);
#endif

	}
}



//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

void H323MediaCall::OpenRemoteSecondPlayChan(void)
{
	if (HasAVCONTransport())
	{
#if ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1 ) )
#else

		MediaCall::OpenRemoteSecondPlayChan(m_nPeerSecondVideoID,m_strPeerLocalIP,m_nPeerLocalPort,m_strPeerMCUID, m_nEnableMulticast);
#endif

	}
}

void H323MediaCall::CloseRemoteSecondPlayChan()
{
	if (HasAVCONTransport())
	{
#if ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1 ) )
#else

		MediaCall::CloseRemoteSecondPlayChanMedia();
#endif

	}
}

//#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


bool H323MediaCall::HasAVCONTransport(void)
{

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

return false;

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



	if (NETEC_Core::GetUDPPort()==0)
	{
		return false;
	}

	if (m_nPeerAudioID && m_nPeerVideoID && m_nPeerSecondVideoID && m_strPeerLocalIP.size() && m_nPeerLocalPort)
	{
		return true;
	}

	return false;
}

bool H323MediaCall::IsExpired(int nCheckTimer)
{
	//回环加媒体流联合检测 此设置为TRUE
	if (HasAVCONTransport())
	{
		return true;
	}
	if(!m_pAudioSendChannel && !m_pMainVideoSendChannel )
	{
		return true;
	}
	unsigned long nTimestamp=XGetTimestamp();
	if (m_nLastRcvdPacketTimestamp>nTimestamp)
	{
		m_nLastRcvdPacketTimestamp=nTimestamp;
	}
	else if (nTimestamp-m_nLastRcvdPacketTimestamp>= nCheckTimer)
	{
		m_nLastRcvdPacketTimestamp=nTimestamp;
		return true;
	}

	return false;
}

//notice netec send ptz
#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

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

//add by Jan for DTMF [20140730]
void H323MediaCall::OnAudioRecvDTMF(char dtmf)
{
	H323Call::OnRecvDTMF(dtmf);
}
//end [20140730]

#endif

int H323MediaCall::OnNETECRequestMainVideoKeyFrame(void)
{
	H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),1);
	return 0;
}

int H323MediaCall::OnNETECRequestSecondVideoKeyFrame(void)
{
	H323CallMgr::Instance().OnCallKeyFrameRequest(m_strCallID.c_str(),2);
	return 0;
}

