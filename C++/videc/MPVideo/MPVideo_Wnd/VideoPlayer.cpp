#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>
#include "VideoPlayer.h"
#include <VIDEC/VIDEC_Header.h>
#include "../MPVIDEO_API/XImage.h"
#include "XUtil.h"
#include "../MPVIDEO_Codecs/HWVideoDecoder.h"


#define MIN_BUF_MS			100
#define MAX_BUF_MS			2000
#define MAX_FRAME_INTERVAL	1000


MPVideo_Player*MPVideo_Player::Create(MPVideo_PlayerCallback&rCallback)
{
	return new VideoPlayer(rCallback);
}



VideoPlayer::VideoPlayer(MPVideo_PlayerCallback&rCallback)
:m_rCallback(rCallback)
,m_lpnVideoTimestamp(NULL)
,m_lpnAudioTimestamp(NULL)
,m_nSynFlag(0)
,m_nLastAudioTimestamp(0)
,m_nLastUpdateAudioTimestamp(0)
,m_nNewFrameTimestamp(0)
,m_pImage(NULL)
{
	m_nLastFrameTimestamp=0;
	m_nLastPaintedTimestamp=0;
}

VideoPlayer::~VideoPlayer(void)
{
}

int VideoPlayer::Open(unsigned long*lpnVideoTimestamp,unsigned long*lpnAudioTimestamp)
{
	m_lpnVideoTimestamp=lpnVideoTimestamp;
	m_lpnAudioTimestamp=lpnAudioTimestamp;

	ImageInput::Open();

	return XDecoderStreamBase::Open();
}

void VideoPlayer::Close(void)
{
	XDecoderStreamBase::Close();
	ImageInput::Close();

	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			MPVideo_Image*pImage=(MPVideo_Image*)m_listImage.front();
			m_listImage.pop_front();
			pImage->Release();
			pImage=NULL;
		}
		if (m_pImage)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
	}
}



void VideoPlayer::SetSynFlag(int nSynFlag)
{
	m_nSynFlag=nSynFlag;
}



void VideoPlayer::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}



int VideoPlayer::PlayData(void*pPacketData,int nPacketSize)
{
	if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
		m_nNewFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);

	return DecodeFramePacket(pPacketData,nPacketSize);
}

void VideoPlayer::OnDecoderStreamCallbackFrame(MPVideo_Image*pImage)
{
	if (pImage==NULL)
		return ;

	pImage->AddRef();

	XAutoLock l(m_csListImage);
	m_listImage.push_back(pImage);

	while (m_listImage.size()>3)
	{
		pImage=(MPVideo_Image*)m_listImage.front();
		m_listImage.pop_front();
		if (pImage)
		{
			pImage->Release();
			pImage=NULL;
		}
	}
}

MPVideo_Image*VideoPlayer::GetImage(void)
{
	XAutoLock l(m_csListImage);
	if (m_listImage.size())
	{
		MPVideo_Image*pImage=(MPVideo_Image*)m_listImage.front();
		m_listImage.pop_front();
		if (m_pImage)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
		m_pImage=pImage;
	}
	
	if (m_pImage)
	{
		m_pImage->AddRef();
	}
	return m_pImage;
}

bool VideoPlayer::IsPending(void)
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
			if (VIDEC_HEADER_EXT_IS_VALID(pPacketFront->m_pPacketData))
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

		if (m_nSynFlag && m_lpnAudioTimestamp!=NULL && m_lpnVideoTimestamp!=NULL)
		{
			unsigned long ulCurTimestamp=XGetTimestamp();
			bool bAudioTimestampChanged=false;

			if (m_nLastAudioTimestamp!=*m_lpnAudioTimestamp)
			{
				m_nLastAudioTimestamp=*m_lpnAudioTimestamp;
				m_nLastUpdateAudioTimestamp=ulCurTimestamp;
				bAudioTimestampChanged=true;
			}

			//新到的视频
			if (bAudioTimestampChanged && ulCurTimestamp-m_nLastUpdateAudioTimestamp<=300)
			{
				//被音频改了时间
				unsigned long ulDeltaAudio=*m_lpnAudioTimestamp-ulCurrentFrameTimestamp;
				unsigned long ulDeltaVideo=ulCurrentFrameTimestamp-*m_lpnAudioTimestamp;
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


void VideoPlayer::OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp)
{
	m_nLastPaintedTimestamp=nPaintTimestamp;
	m_nLastFrameTimestamp=nFrameTimestamp;
	if (m_lpnVideoTimestamp)
	{
		*m_lpnVideoTimestamp=nFrameTimestamp;
	}
}

int VideoPlayer::DoDecodePacket(void*pPacketData,int nPacketSize)
{
	int nRet=XDecoderStreamBase::DoDecodePacket(pPacketData,nPacketSize);

	if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
	{
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());

		m_rCallback.OnMPVideo_PlayerCallbackPlayData(pPacketData,nPacketSize);
	}

	return nRet;
}