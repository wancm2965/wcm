#include "stdafx.h"
#include "AVMSMediaFile.h"
#include "XUtil.h"
#include "VIDEC_Header.h"
#include "VIDEC_HeaderV1.h"

#define MIN_BUF_MS			100
#define MAX_BUF_MS			2000
#define MAX_FRAME_INTERVAL	1000

void AVMSP_PCM_DATA(const void*pUserData,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)
{
	AVMSMediaFile*pAVMSMediaFile=(AVMSMediaFile*)pUserData;
	if (pAVMSMediaFile)
	{
		pAVMSMediaFile->OnPCMData(pPCMData,nLen,nSampleRate,nTimestamp);
	}
}

void AVMSP_VIDEO_PACKET(const void*pUserData,unsigned char*pData,int nLen)
{
	AVMSMediaFile*pAVMSMediaFile=(AVMSMediaFile*)pUserData;
	if (pAVMSMediaFile)
	{
		pAVMSMediaFile->OnVideoPacket(pData,nLen);
	}
}


AVMSMediaFile::AVMSMediaFile(AVMSMediaFileCallback&rCallback)
:m_rCallback(rCallback)
,m_pHandle(NULL)
{
	m_nAudioTimestamp=0;
	m_nVideoTimestamp=0;

	m_nLastAudioTimestamp=0;
	m_nLastUpdateAudioTimestamp=0;
	m_nNewFrameTimestamp=0;
	m_nLastPaintedTimestamp=0;
	m_nLastFrameTimestamp=0;
}

AVMSMediaFile::~AVMSMediaFile(void)
{

}


int AVMSMediaFile::Open(HWND hWndVideo,const char*cszFileName,bool& bTryEVR)
{
	m_pHandle=AVMSP_Open(cszFileName,AVMSP_PCM_DATA,AVMSP_VIDEO_PACKET,this,&m_nVideoTimestamp,&m_nAudioTimestamp);
	if (m_pHandle==NULL)
	{
		return -1;
	}

	return XDecoderStreamBase::Open();
}

void AVMSMediaFile::Close(void)
{
	if (m_pHandle)
	{
		AVMSP_Close(m_pHandle);
		m_pHandle=NULL;
	}

	XDecoderStreamBase::Close();
}

void AVMSMediaFile::SetWndPos(int x,int y,int cx,int cy)
{

}

void AVMSMediaFile::Repaint(void)
{

}

unsigned long AVMSMediaFile::GetDuration(void)
{
	if (m_pHandle)
	{
		return AVMSP_GetDuration(m_pHandle)/1000;
	}
	return 0;
}

unsigned long AVMSMediaFile::GetPosition(void)
{
	if (m_pHandle)
	{
		return AVMSP_GetPosition(m_pHandle)/1000;
	}
	return 0;
}

int AVMSMediaFile::Seek(unsigned long ulSeconds)
{
	if (m_pHandle)
	{
		return AVMSP_Seek(m_pHandle,ulSeconds*1000);
	}
	return -1;
}

int AVMSMediaFile::Pause(void)
{
	if (m_pHandle)
	{
		return AVMSP_Pause(m_pHandle);
	}
	return -1;
}

int AVMSMediaFile::Play(void)
{
	if (m_pHandle)
	{
		return AVMSP_Play(m_pHandle);
	}
	return -1;
}

bool AVMSMediaFile::HasAudio(void)
{
	return true;
}

int AVMSMediaFile::SetVolume(int nVolume)
{
	if (m_pHandle)
	{
		return AVMSP_SetVolume(m_pHandle,nVolume);
	}
	return -1;
}

bool AVMSMediaFile::HasVideo(void)
{
	return true;
}

int AVMSMediaFile::GetVideoWidth(void)
{
	return 0;
}

int AVMSMediaFile::GetVideoHeight(void)
{
	return 0;
}

void AVMSMediaFile::OnPCMData(unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)
{
	m_rCallback.OnAVCONAVC_MediaFileCallbackPCMData(this,pPCMData,nLen,nSampleRate,nTimestamp);
}

void AVMSMediaFile::OnVideoPacket(unsigned char*pData,int nLen)
{
	XDecoderStreamBase::DecodeFramePacket(pData,nLen);
}

bool AVMSMediaFile::IsPending(void)
{
	int nDataCount=0;
	{
		XAutoLock l(m_csListPackets);
		nDataCount=m_ListPackets.size();
		if (nDataCount<=0)
		{
			return false;
		}
	}

	{
		unsigned long ulCurrentTimestamp=XGetTimestamp();
		unsigned long ulCurrentFrameTimestamp=0;

		m_csListPackets.Lock();
		if (m_ListPackets.size()>0)
		{
			PPACKET pPacketFront=(PPACKET)m_ListPackets.front();
			if (VIDEC_HEADER_IS_VALID(pPacketFront->m_pPacketData))
			{
				ulCurrentFrameTimestamp=VIDEC_HEADER_GET_TIMESTAMP(pPacketFront->m_pPacketData);
			}
			else if (VIDEC_HEADER_EXT_IS_VALID(pPacketFront->m_pPacketData))
			{
				ulCurrentFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketFront->m_pPacketData);
			}
			else
			{
				m_csListPackets.UnLock();
				return true;
			}

		}
		m_csListPackets.UnLock();

		unsigned long ulVideoFast=0;

		{
			unsigned long ulCurTimestamp=XGetTimestamp();
			bool bAudioTimestampChanged=false;

			if (m_nLastAudioTimestamp!=m_nAudioTimestamp)
			{
				m_nLastAudioTimestamp=m_nAudioTimestamp;
				m_nLastUpdateAudioTimestamp=ulCurTimestamp;
				bAudioTimestampChanged=true;
			}

			//新到的视频
			if (bAudioTimestampChanged && ulCurTimestamp-m_nLastUpdateAudioTimestamp<=300)
			{
				//被音频改了时间
				unsigned long ulDeltaAudio=m_nAudioTimestamp-ulCurrentFrameTimestamp;
				unsigned long ulDeltaVideo=ulCurrentFrameTimestamp-m_nAudioTimestamp;
				unsigned long ulDelta=(ulDeltaAudio>ulDeltaVideo?ulDeltaVideo:ulDeltaAudio);

				if (ulDelta<200)
				{
					//偏差在可以容忍的范围内
				}
				else
				{
					if (ulDelta==ulDeltaAudio)
					{
						//音频快了，视频需要加速播放
						ulVideoFast=ulDelta-200;
					}
					else
					{
						//视频快了，视频需要减速播放
						if (ulDelta<MAX_BUF_MS/2)
						{
							return false;
						}
					}
				}
			}
		}


		unsigned long nFastCount=0;
		bool bSlow=false;
		unsigned long ulBufferedMS=m_nNewFrameTimestamp-ulCurrentFrameTimestamp;

		unsigned long ulDelayInterval=(unsigned long)(ulCurrentTimestamp-m_nLastPaintedTimestamp);
		unsigned long ulFrameInterval=(unsigned long)(ulCurrentFrameTimestamp-m_nLastFrameTimestamp);
		if (ulFrameInterval>=MAX_FRAME_INTERVAL)
			ulFrameInterval=0;

		unsigned long nThresholdCount=3;
		if (ulFrameInterval<=50)
		{
			nThresholdCount=3;
		}
		else if (ulFrameInterval<=160)
		{
			nThresholdCount=2;
		}
		else
		{
			nThresholdCount=1;
		}

		if (ulBufferedMS>MIN_BUF_MS || nDataCount>nThresholdCount)
		{
			nFastCount=(nDataCount>nThresholdCount?(nDataCount-nThresholdCount):0);//加速播放

			ulDelayInterval+=nFastCount;
		}

		if (ulVideoFast)
		{
			ulFrameInterval-=(ulFrameInterval>>2);
		}

		if (ulFrameInterval<10)
			ulFrameInterval=10;

		if ((ulDelayInterval>=MAX_FRAME_INTERVAL))
		{
			return true;
		}

		if (ulDelayInterval>=ulFrameInterval)
		{
			return true;
		}
	}

	return false;
}

void AVMSMediaFile::OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp)
{
	m_nLastPaintedTimestamp=nPaintTimestamp;
	m_nLastFrameTimestamp=nFrameTimestamp;
	if (m_nVideoTimestamp)
	{
		m_nVideoTimestamp=nFrameTimestamp;
	}
}

void AVMSMediaFile::OnDecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	m_rCallback.OnAVMSMediaFileCallbackFrame(pImage);
}