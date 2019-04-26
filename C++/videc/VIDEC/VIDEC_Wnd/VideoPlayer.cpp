#include "stdafx.h"
#include <stdlib.h>
#include <memory.h>
#include "VideoPlayer.h"
#include "VIDEC_Header.h"
#include "VIDEC_HeaderV1.h"
#include "../VIDEC_API/XImage.h"
#include "XUtil.h"
#include "H264Player.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

#define MIN_BUF_MS			100
#define MAX_BUF_MS			2000
#define MAX_FRAME_INTERVAL	1000


VIDEC_Player*VIDEC_Player::Create(VIDEC_PlayerCallback&rCallback)
{
	//return new VideoPlayer(rCallback);
	NEWRECORDret3(  VIDEC_Player* ,p , new VideoPlayer( rCallback ) )   
}

void VIDEC_Player::SetMaxDXVAPlayers(unsigned int nCount)
{
	H264PlayerSetMaxCount(nCount);
}


VideoPlayer::VideoPlayer(VIDEC_PlayerCallback&rCallback)
:m_rCallback(rCallback)
,m_lpnVideoTimestamp(NULL)
,m_lpnAudioTimestamp(NULL)
,m_nSynFlag(0)
,m_nLastAudioTimestamp(0)
,m_nLastUpdateAudioTimestamp(0)
,m_nNewFrameTimestamp(0)
,m_nDecoderType(0)
,m_pVideoPlayerWnd(NULL)
{
	m_nLastFrameTimestamp=0;
	m_nLastPaintedTimestamp=0;
	m_pVQD=NULL;

	m_nLastCountTimestamp=XGetTimestamp();
	m_nTotalCount=0;
	m_nCountIndex=0;

	m_nTotalLoopCount=0;
	m_nLoopCountIndex=0;

	m_nNextSkipCount=0;
	m_nLastTS=timeGetTime();
	m_nFrameCount=0;

	m_nCurDelayInterval = 0;
	m_nCurDelaySkip = 0;
	m_nDelayInterval = 0;
	m_nDelaySkip = 0;

	m_ulLastCountFrameTimestamp=0;
	m_nRecvFrameRateCount=0;
	m_nRecvFrameRate=0;

	m_bPausePlay=false;
}

VideoPlayer::~VideoPlayer(void)
{
}

void VideoPlayer_CallbackDrawD3D9DC(const void*pUserData,void* hDC)
{
	VideoPlayer*pVideoPlayer=(VideoPlayer*)pUserData;
	if (pVideoPlayer)
	{
		pVideoPlayer->OnCallbackDrawD3D9DC(hDC);
	}
}

void VideoPlayer::OnCallbackDrawD3D9DC(void* hDC)
{
	m_rCallback.OnVIDEC_PlayerCallbackDrawD3D9DC(hDC);
}

int VideoPlayer::Open(HWND hWnd, unsigned long*lpnVideoTimestamp, unsigned long*lpnAudioTimestamp, char* pBgBmp, int bgcolr, int bgcolg, int bgcolb)
{
	m_lpnVideoTimestamp=lpnVideoTimestamp;
	m_lpnAudioTimestamp=lpnAudioTimestamp;

	NEWRECORD( m_pVideoPlayerWnd,new VideoPlayerWnd(VideoPlayer_CallbackDrawD3D9DC,this) )
	NULLReturnRet( m_pVideoPlayerWnd , -1 )  
	if (pBgBmp != NULL)
		m_pVideoPlayerWnd->SetBgBmp(pBgBmp);

	if (bgcolr != -1 || bgcolg != -1 || bgcolb != -1)
		m_pVideoPlayerWnd->SetBgRgb(bgcolr, bgcolg, bgcolb);

	if (0!=m_pVideoPlayerWnd->Open(hWnd))
	{
		m_pVideoPlayerWnd->Close();
		delete m_pVideoPlayerWnd;
		m_pVideoPlayerWnd=NULL;

		return -1;
	}

	return XDecoderStreamBase::Open();
}

void VideoPlayer::Close(void)
{
	XDecoderStreamBase::Close();

	if (m_pVideoPlayerWnd!=NULL)
	{
		m_pVideoPlayerWnd->Close();
		delete m_pVideoPlayerWnd;
		m_pVideoPlayerWnd=NULL;
	}

	StopVQD();
}

#ifdef _TIANZHU_CACHE_

	//天竺项目：添加一个接口，运行时设置视频缓冲
	void VideoPlayer::SetCacheFrameCount(int nCacheFrameCount)
	{
		XDecoderStreamBase::SetCacheFrameCount(nCacheFrameCount);
	}

#endif

int VideoPlayer::GetVideoWidth(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->GetVideoWidth();
	}
	return 160;
}

int VideoPlayer::GetVideoHeight(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->GetVideoHeight();
	}
	return 120;
}


HWND VideoPlayer::GetWnd(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->GetWnd();
	}
	return NULL;
}

bool VideoPlayer::IsWindowVisible(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->IsWindowVisible();
	}
	return  false;
}

bool VideoPlayer::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->SetWindowPos(hWndInsertAfter,x,y,cx,cy,nFlags);
	}

	return false;
}


void VideoPlayer::SetSynFlag(int nSynFlag)
{
	m_nSynFlag=nSynFlag;
}


double VideoPlayer::GetAverageFPS(void)
{
	double fFPS=0.0;
	//unsigned long ulTimestamp=XGetTimestamp();
	//unsigned long ulDelta=ulTimestamp-m_ulFrameCountTimestamp;
	//if (ulDelta)
	//{
	//	fFPS=(double)m_nFrameCount*1000.00/(double)ulDelta;
	//}
	//m_ulFrameCountTimestamp=ulTimestamp;
	//m_nFrameCount=0;
	return fFPS;
}

bool VideoPlayer::HasVideo(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->HasVideo();
	}

	return false;
}

bool VideoPlayer::CaptureToBMPFile(const char*cszFile)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->CaptureToBMPFile(cszFile);
	}

	return false;
}

//设置视频属性
int VideoPlayer::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}

	return -1;
}

int VideoPlayer::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}

	return -1;
}

void VideoPlayer::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		m_pVideoPlayerWnd->SetScreenWnd(nScreenIndex,nWndIndex,nFullScreen);
	}
}


int VideoPlayer::GetSupportPreview(void)
{
	if (m_pVideoPlayerWnd)
	{
		return m_pVideoPlayerWnd->GetSupportPreview();
	}

	return 0;
}


VIDEC_Image*VideoPlayer::GetImage(void)
{
	if (m_pVideoPlayerWnd!=NULL)
	{
		return m_pVideoPlayerWnd->GetImage();
	}

	return NULL;
}

void VideoPlayer::ResetImage(VIDEC_Image*pImage)
{
	XDecoderStreamBase::ResetImage(pImage);
}

void VideoPlayer::SetEnableVideo(int nEnable)
{
	if (m_pVideoPlayerWnd)
	{
		m_pVideoPlayerWnd->SetEnableVideo(nEnable);
	}
}

int VideoPlayer::GetEnableVideo(void)
{
	if (m_pVideoPlayerWnd)
	{
		return m_pVideoPlayerWnd->GetEnableVideo();
	}

	return 0;
}

int VideoPlayer::GetVideoShowType(void)
{
	if (m_pVideoPlayerWnd)
	{
		return m_pVideoPlayerWnd->GetVideoShowType();
	}
	return 0;
}

int VideoPlayer::GetVideoShowFrameRate(void)
{
	if (m_pVideoPlayerWnd)
	{
		return m_pVideoPlayerWnd->GetVideoShowFrameRate();
	}
	return 0;
}

int VideoPlayer::GetVideoRecvFrameRate(void)
{
	int nRate = m_nRecvFrameRate;
	m_nRecvFrameRate=0;
	return nRate;
}

int VideoPlayer::GetVideoDecodeFrameRate(void)
{
	
	return 0;
}

int VideoPlayer::GetDecoderType(void)
{
	return m_nDecoderType;
}

void VideoPlayer::SetVideoDelay(int nMilliSecond)
{
	XDecoderStreamBase::SetVideoDelay(nMilliSecond);
}

int VideoPlayer::SetVideoSkipFrame(int nInterval,int nSkip)
{
	bool bClear = (nSkip == 98);
	if (nSkip == 99 || nSkip == 98)
	{
		m_bPausePlay=true;
	}
	else
	{
		m_bPausePlay=false;
	}

	XDecoderStreamBase::PausePlay(m_bPausePlay,bClear);
	if (m_pVideoPlayerWnd)
	{
		m_pVideoPlayerWnd->PausePlay(m_bPausePlay,bClear);
	}

	if (m_pVideoPlayerWnd && m_pVideoPlayerWnd->IsUsingGPU())
	{
		m_nDelayInterval = 0;
		m_nDelaySkip = 0;
	}
	else
	{
		m_nCurDelayInterval = 0;
		m_nCurDelaySkip = 0;
		m_nDelayInterval = nInterval;
		m_nDelaySkip = nSkip;
	}

	if (m_nDelayInterval != 1 || m_nDelaySkip != 21)
	{
		XDecoderStreamBase::SetVideoSkipFrame(m_nDelayInterval,m_nDelaySkip);
	}

	return 0;
}


void VideoPlayer::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = timeGetTime();
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if ((m_nRecvFrameRateCount > 30 && ulTemp < 990) || (m_nRecvFrameRateCount <= 30 && ulTemp < 980))
	{
		return;
	}

	if (ulTemp > 1500)
	{
		float f = 1000.00/ulTemp;
		m_nRecvFrameRate=m_nRecvFrameRateCount*f;
	}
	else
	{
		m_nRecvFrameRate=m_nRecvFrameRateCount;
	}
	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	m_nRecvFrameRateCount=0;
}

int VideoPlayer::PlayData(void*pPacketData,int nPacketSize)
{
	//if (m_bPausePlay)
	//{
	//	return 0;
	//}
	m_nRecvFrameRateCount++;
	ReSetFrameRate();

	if (VIDEC_HEADER_IS_VALID(pPacketData))
		m_nNewFrameTimestamp=VIDEC_HEADER_GET_TIMESTAMP(pPacketData);
	else if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
		m_nNewFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);

	int nR = DecodeFramePacket(pPacketData,nPacketSize);
	if (nR >= 0)
	{
		if (m_pVideoPlayerWnd && m_pVideoPlayerWnd->IsUsingGPU())
		{
			m_nDecoderType=2;
		}
		else
		{
			m_nDecoderType=1;
		}
	}
	else
	{
		m_nDecoderType=0;
	}
	return nR;
}

void VideoPlayer::OnDecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	if (m_bPausePlay)
	{
		return;
	}

	int nDelaySkip = m_nDelaySkip;
	int nDelayInterval = m_nDelayInterval;

	if (m_nDelayInterval == 1 && m_nDelaySkip == 21)
	{
		nDelaySkip=1;
		nDelayInterval=1;
	}

	if (nDelayInterval > 0 && nDelaySkip > 0)
	{
		if (m_nCurDelayInterval >= nDelayInterval)
		{
			m_nCurDelaySkip++;

			if (m_nCurDelaySkip >= nDelaySkip)
			{
				m_nCurDelayInterval = 0;
				m_nCurDelaySkip = 0;
			}
			return;
		}
		else
		{
			m_nCurDelayInterval++;
		}
	}

	if (m_pVideoPlayerWnd!=NULL)
	{
		bool bEnableD3D = !(nDelayInterval > 0 && nDelaySkip > 0);
		if (m_nDelayInterval == 1 && m_nDelaySkip == 21)
		{
			bEnableD3D=true;
		}
		m_pVideoPlayerWnd->SetEnableD3D(bEnableD3D);
		m_pVideoPlayerWnd->OnReceivedImage(pImage);
	}

	{
		XAutoLock l(m_csVQD);
		if (m_pVQD)
		{
			m_pVQD->OnReceivedImage(pImage);
		}
	}
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

	if (m_pVideoPlayerWnd)
	{
		if (m_pVideoPlayerWnd->IsTryingGPU())
		{
			return false;
		}

		if (m_pVideoPlayerWnd->IsUsingGPU() && 
			m_nFlagFrameFromHardware)
		{
			return true;
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

		unsigned long nThresholdCount=1;
		if (ulFrameInterval<=50)
		{
			nThresholdCount=1;
		}
		//else if (ulFrameInterval<=160)
		//{
		//	nThresholdCount=2;
		//}
		else
		{
			nThresholdCount=0;
		}
		
		//m_nCacheFrameCount变量为缓冲帧数，此变量为0或者负数，则表示不需要缓冲
		if (0 >= m_nCacheFrameCount)
		{
			if (ulBufferedMS>MIN_BUF_MS || nDataCount>nThresholdCount)
			{
				nFastCount=(nDataCount>nThresholdCount?(nDataCount-nThresholdCount):0);//加速播放

				ulDelayInterval+=nFastCount;
			}
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
	unsigned long nCurrentCount=0;
	unsigned long nTimestamp=XGetTimestamp();
	{
		XAutoLock l(m_csListPackets);
		nCurrentCount=m_ListPackets.size();
	}

	if (nTimestamp-m_nLastCountTimestamp>=1000)
	{
		m_nLastCountTimestamp=nTimestamp;
		m_nTotalCount+=nCurrentCount;
		m_nCountIndex++;

		m_nTotalLoopCount+=nCurrentCount;
		m_nLoopCountIndex++;

		//char szDebug[1024];
		//sprintf(szDebug,"m_nTotalCount=%u Count=%u m_nCountIndex=%u m_nTotalLoopCount=%u m_nLoopCountIndex=%u nFrameTimestamp=%u\n",m_nTotalCount,nCurrentCount,m_nCountIndex,m_nTotalLoopCount,m_nLoopCountIndex,nFrameTimestamp);
		//DebugOutput(szDebug);

		if (m_nLoopCountIndex>=10)
		{
			//if (m_nTotalLoopCount<4 && nCurrentCount==0)
			//{
			//	if (m_nFlagFrameFromHardware && m_pVideoPlayerWnd)
			//	{
			//		m_pVideoPlayerWnd->RepeatFrame();
			//	}
			//}
			m_nTotalLoopCount=0;
			m_nLoopCountIndex=0;
		}

		if (m_nCountIndex>=4)
		{
			unsigned long nThreshold=20;
			if (m_nTotalCount>=nThreshold && nCurrentCount>2)
			{
				if (m_pVideoPlayerWnd)
				{
					m_pVideoPlayerWnd->SkipOneFrame();
				}
				m_nNextSkipCount=nCurrentCount-(nThreshold>>2);
			}
			m_nTotalCount=0;
			m_nCountIndex=0;
		}
	}
	else if(m_nNextSkipCount>0)
	{
		m_nNextSkipCount--;
		if (m_pVideoPlayerWnd)
		{
			m_pVideoPlayerWnd->SkipOneFrame();
		}
	}


	m_nLastPaintedTimestamp=nPaintTimestamp;
	m_nLastFrameTimestamp=nFrameTimestamp;
	if (m_lpnVideoTimestamp)
	{
		*m_lpnVideoTimestamp=nFrameTimestamp;
	}
}

void VideoPlayer::ReDraw(void)
{
	if (m_pVideoPlayerWnd)
	{
		m_pVideoPlayerWnd->RepeatFrame();
	}
}

int VideoPlayer::DoDecodePacket(void*pPacketData,int nPacketSize,int nFrameRate)
{
	if (m_bPausePlay)
	{
		return 0;
	}

	if (FRE_GetDebugLevel() > 0)
	{
		m_nFrameCount++;
		unsigned long nCurTS=timeGetTime();
		if (nCurTS-m_nLastTS >= 20000)
		{
			DebugOutput( logthread ,"VideoPlayer::DoDecodePacket FrameCount=%u CurTime=%u this = %u \n",m_nFrameCount,nCurTS,this);			
			m_nFrameCount=0;
			m_nLastTS=nCurTS;
		}
	}

	int nRet=0;
	int nTryGPU=0;

	if (m_nDelayInterval > 0 && m_nDelaySkip > 0)
	{

	}
	else
	{
		while (1)
		{
			if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
			{
				VIDEC_CODEC_TYPE nCodecID=(VIDEC_CODEC_TYPE)VIDEC_HEADER_EXT_GET_CODEC_ID(pPacketData);
				if (nCodecID==VIDEC_CODEC_H264 || nCodecID==VIDEC_CODEC_H264_SVC)
				{
					int nKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData);
					if (nKeyFrame)
					{
						int nWidth=VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pPacketData);
						int nHeight=VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pPacketData);
						if (m_pVideoPlayerWnd->CanUseGPU(nWidth,nHeight,VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pPacketData),VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pPacketData)))
						{
							m_pVideoPlayerWnd->TurnOnGPU(nWidth,nHeight);
						}
						else
						{
							m_pVideoPlayerWnd->TurnOffGPU();
							break;
						}
					}

					if (m_pVideoPlayerWnd->IsTryingGPU())
					{
						return 100;
					}

					if (m_pVideoPlayerWnd->IsUsingGPU())
					{
						m_ulFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
						int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);

						if (nKeyFrame && *((unsigned char*)pPacketData+nHeaderLen) == 0xFF && *((unsigned char*)pPacketData+nHeaderLen+1) == 0xFF)			//针对编码端在编码出来的数据前添加4个字节表示帧率进行处理，使其还原硬解数据
						{
							nHeaderLen += 4;
						}

						int nFrameRateNew=nFrameRate;
						//{{屏蔽原因：编码是输入的帧率，跟实际的帧率会出现不一致。
						//if (m_nFrameRateWidthKeyFrame && m_nFlagFrameFromHardware)
						//{
						//	nFrameRateNew=m_nFrameRateWidthKeyFrame;
						//}
						//}}
						m_pVideoPlayerWnd->PlayFrame((unsigned char*)pPacketData+nHeaderLen,nPacketSize-nHeaderLen,nKeyFrame,nFrameRateNew);
						nTryGPU=1;

						if (m_pVideoPlayerWnd->IsTryingGPU())
						{
							return 100;
						}
					}
				}
				else
				{
					m_pVideoPlayerWnd->TurnOffGPU();
				}
			}
			break;
		}
	}


	if (!nTryGPU)
	{
		nRet=XDecoderStreamBase::DoDecodePacket(pPacketData,nPacketSize,nFrameRate);
	}

	if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
	{
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,XGetTimestamp());

		m_rCallback.OnVIDEC_PlayerCallbackPlayData(pPacketData,nPacketSize);
	}

	return nRet;
}

int VideoPlayer::StartVQD(unsigned long nDetectFlags)
{
	StopVQD();

	VIDEC_VQD*pVQD=VIDEC_VQD::Create(*this);
	if (pVQD==NULL)
	{
		return -1;
	}
	if (0!=pVQD->Open(nDetectFlags))
	{
		pVQD->Close();
		delete pVQD;
		pVQD=NULL;

		return -1;
	}

	{
		XAutoLock l(m_csVQD);
		m_pVQD=pVQD;
	}
	return 0;
}

void VideoPlayer::StopVQD(void)
{
	VIDEC_VQD*pVQD=m_pVQD;
	{
		XAutoLock l(m_csVQD);
		m_pVQD=NULL;
	}

	if (pVQD)
	{
		pVQD->Close();
		delete pVQD;
		pVQD=NULL;
	}
}

void VideoPlayer::OnVIDEC_VQDCallbackResult(unsigned long nDetectResult)
{
	m_rCallback.OnVIDEC_PlayerCallbackVQDResult(nDetectResult);

	StopVQD();
}