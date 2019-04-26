#include <stdlib.h>
#include <memory.h>
#include "XDecoderStreamBase.h"
#include "VIDEC_Header.h"
#include "VIDEC_HeaderV1.h"
#include "../VIDEC_API/XImage.h"
#include "XUtil.h"
#include "AVCONProduct.h"
#include "VIDEC_AH2KDev.h"
#include "AVCONH264Header.h"

//add ??
#include "RawFile.h"
CLogUtil * g_LogUtil = NULL;

XDecoderStreamBase::XDecoderStreamBase(void)
:m_codecType(VIDEC_CODEC_COUNT)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_bDoubleField(false)

,m_bGotEvenKeyFrame(false)
,m_bGotOddKeyFrame(false)
,m_usLastSeqNo(0)
,m_bGotLastSeqNo(false)
,m_bEvenFieldFirst(true)

,m_bGotEvenKeyFrameDecode(false)
,m_bGotOddKeyFrameDecode(false)
,m_usLastSeqNoDecode(0)
,m_bGotLastSeqNoDecode(false)

,m_pDecoderFrameEven(NULL)
,m_pDecoderFrameOdd(NULL)
,m_bPause(false)
,m_nPacketWidth(0)
,m_nPacketHeight(0)
,m_bVideoSkipFrame(false)
,m_ulFrameTimestamp(0)
,m_bCacheFlag(false)	//天竺：缓冲状态标志量（bcz）
,m_nCacheFrameCount(0) //天竺：缓冲帧数默认设置为0
{
	m_bGotEvenKeyFrame0=false;
	m_bGotEvenKeyFrame1=false;
	m_bGotEvenKeyFrame2=false;
	m_bGotEvenKeyFrame3=false;
	m_bGotOddKeyFrame0=false;
	m_bGotOddKeyFrame1=false;
	m_bGotOddKeyFrame2=false;
	m_bGotOddKeyFrame3=false;

	m_bGotKeyFrame=false;
	m_pDecoderFrame=NULL;
	m_bGotImage=false;

	m_bOffsetInitialized=false;
	m_ulFirstTimestampOffset=0;
	m_ulFirstFrameTimestamp=0;

	m_nFlagFrameFromHardware=0;
	m_nFrameRateWidthKeyFrame=0;

	m_pResetImage=NULL;

#ifndef USE_XWORKTHREAD
	m_bRunning=false;
#endif

#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif

	m_nLastTS=timeGetTime();
	m_nFrameCount=0;
	m_bDelayFrame=false;

	m_nMilliSecond = FRE_GetDelayMilliSecond();

	m_nCountFrameIndex=0;
	m_ulLastCountFrameTimestamp=0;
	m_nFrameRateCur=0;
	m_nFrameRateCount=FRE_GetDelayFrameRate();
	m_nFrameRateActual=FRE_GetDelayFrameRate();

	float fC = m_nMilliSecond/1000.0;
	m_nDelayFrameCount = fC * m_nFrameRateActual;
}

XDecoderStreamBase::~XDecoderStreamBase(void)
{
	{
		XAutoLock l(m_csListPackets);
		while (m_ListPackets.size())
		{
			PPACKET pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();

			FREEMSG(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;
		}
	}

	VIDEC_Image*pResetImage=NULL;
	{
		XAutoLock l(m_csResetImage);
		pResetImage=m_pResetImage;
		m_pResetImage=NULL;
	}
	if (pResetImage)
	{
		pResetImage->Release();
		pResetImage=NULL;
	}
}

int XDecoderStreamBase::Open(void)
{
#ifdef USE_XWORKTHREAD
	if (0!=XWorkThread::Start())
	{
		return -1;
	}
#else
	m_bRunning=true;
	if (!StartThread())
	{
		return -1;
	}
#endif

	if (m_nMilliSecond > 0)
	{
		FrameRateEvent::Open();
		FrameRateEvent::SetFrameRate(m_nFrameRateActual);
	}

	return 0;
}

void XDecoderStreamBase::Close(void)
{
	if (m_nMilliSecond > 0)
	{
		FrameRateEvent::Close();
	}

#ifdef USE_XWORKTHREAD
	XWorkThread::Stop();
#else
	m_bRunning=false;
	WaitForStop();
#endif

	{
		XAutoLock l(m_csListPackets);
		while (m_ListPackets.size())
		{
			PPACKET pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();

			FREEMSG(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;
		}
	}

	if (m_pDecoderFrameEven!=NULL)
	{
		m_pDecoderFrameEven->Close();
		delete m_pDecoderFrameEven;
		m_pDecoderFrameEven=NULL;
	}

	if (m_pDecoderFrameOdd!=NULL)
	{
		m_pDecoderFrameOdd->Close();
		delete m_pDecoderFrameOdd;
		m_pDecoderFrameOdd=NULL;
	}

	if (m_pDecoderFrame!=NULL)
	{
		m_pDecoderFrame->Close();
		delete m_pDecoderFrame;
		m_pDecoderFrame=NULL;
	}

	VIDEC_Image*pResetImage=NULL;
	{
		XAutoLock l(m_csResetImage);
		pResetImage=m_pResetImage;
		m_pResetImage=NULL;
	}
	if (pResetImage)
	{
		pResetImage->Release();
		pResetImage=NULL;
	}
}

void XDecoderStreamBase::SetVideoSkipFrame(int nInterval,int nSkip)
{
	m_bVideoSkipFrame = (nInterval > 0 && nSkip > 0);
}

void XDecoderStreamBase::SetVideoDelay(int nMilliSecond)
{
	if (nMilliSecond == 0 && m_nMilliSecond > 0)
	{
		FrameRateEvent::Close();
	}
	else if (m_nMilliSecond==0 && nMilliSecond > 0)
	{
		FrameRateEvent::Open();
		FrameRateEvent::SetFrameRate(m_nFrameRateActual);
		m_ulLastCountFrameTimestamp=0;
	}
	m_nMilliSecond = nMilliSecond;
	float fC = m_nMilliSecond/1000.0;
	m_nDelayFrameCount = fC * m_nFrameRateActual;
	m_bDelayFrame=false;
}

void XDecoderStreamBase::PausePlay(bool bPause,bool bClear)
{
	m_bPause=bPause;
	if (bClear)
	{
		XAutoLock l(m_csListPackets);
		while (m_ListPackets.size())
		{
			PPACKET pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();

			FREEMSG(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;
		}	
	}
}

void XDecoderStreamBase::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = timeGetTime();
	m_nFrameRateCur++;
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if (ulTemp < 2000)
	{
		return;
	}
	else if (ulTemp > 2500)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
		return;
	}
	
	{
		m_nFrameRateCur = m_nFrameRateCur*1000/ulTemp;
		int nAbs = m_nFrameRateCount-m_nFrameRateCur;
		if (20<=abs(nAbs))
		{
			m_nCountFrameIndex = 0;
		} 
		else if (10<=abs(nAbs))
		{
			m_nCountFrameIndex = 1;
		}
		else if (5<=abs(nAbs))
		{
			m_nCountFrameIndex = 2;
		} 

		m_nFrameRateCount = (m_nFrameRateCount*m_nCountFrameIndex+m_nFrameRateCur)/(m_nCountFrameIndex+1);
		if (m_nCountFrameIndex < 3)
		{
			m_nCountFrameIndex++;
		}

		m_ulLastCountFrameTimestamp = ulCurTimestamp;
		m_nFrameRateCur=0;
		m_nFrameRateActual = m_nFrameRateCount;

		if (m_nFrameRateActual>50)
		{
			m_nFrameRateActual=60;
		}
		else if (m_nFrameRateActual>30)
		{
			m_nFrameRateActual=50;
		}
		else if (m_nFrameRateActual>25)
		{
			m_nFrameRateActual=30;
		}
		else if (m_nFrameRateActual>20)
		{
			m_nFrameRateActual=25;
		}
		else if (m_nFrameRateActual>15)
		{
			m_nFrameRateActual=20;
		}
		else if (m_nFrameRateActual>10)
		{
			m_nFrameRateActual=15;
		}
		else if (m_nFrameRateActual>5)
		{
			m_nFrameRateActual=10;
		}
		else if (m_nFrameRateActual>2)
		{
			m_nFrameRateActual=5;
		}
		else
		{
			m_nFrameRateActual=1;
		}

		FrameRateEvent::SetFrameRate(m_nFrameRateActual);

		float fC = m_nMilliSecond/1000.0;
		m_nDelayFrameCount = fC * m_nFrameRateActual;
	}
}

int XDecoderStreamBase::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (m_nMilliSecond > 0)
	{
		ReSetFrameRate();
	}

	/*if (FRE_GetDebugLevel() > 0)
	{
		m_nFrameCount++;
		unsigned long nCurTS=timeGetTime();
		if (nCurTS-m_nLastTS >= 20000)
		{
			DebugOutput("XDecoderStreamBase::DecodeFramePacket FrameRate=%u FrameCount=%u CurTime=%u this = %u \n",m_nFrameRateActual,m_nFrameCount,nCurTS,this);			
			m_nFrameCount=0;
			m_nLastTS=nCurTS;
		}
	}*/

	if (pPacketData && nPacketSize>0)
	{
		if (m_nMilliSecond == 0 && FRE_GetPending() == 0 && m_bVideoSkipFrame==false)
		{
			if (VIDEC_HEADER_IS_VALID(pPacketData))
			{
				m_nFlagFrameFromHardware=0;

				unsigned short usCurSeq=VIDEC_HEADER_GET_SEQUENCE(pPacketData);
				//中途有丢包，可能是播放器做了SEEK操作
				if (m_bGotLastSeqNo)
				{
					m_usLastSeqNo++;
					if (m_usLastSeqNo!=usCurSeq)
					{
						m_bGotLastSeqNo=false;
					}
				}

				bool bKeyFrame=VIDEC_HEADER_GET_KEY_FRAME(pPacketData)!=0;
				bool bSubKeyFrame0=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;

				if (!m_bGotLastSeqNo)
				{
					if (bKeyFrame && bSubKeyFrame0)
					{
						m_bGotLastSeqNo=true;
						m_usLastSeqNo=usCurSeq;

						m_bGotEvenKeyFrame=false;
						m_bGotOddKeyFrame=false;
					}
					else
					{
						return -1;
					}
				}

				if (bKeyFrame && bSubKeyFrame0)
				{
					//最多缓冲一秒的数据
					{
						XAutoLock l(m_csListPackets);
						if (m_ListPackets.size()>10)
						{
							PPACKET pPacketFront=(PPACKET)m_ListPackets.front();
							unsigned long ulTimestampFront=VIDEC_HEADER_GET_TIMESTAMP(pPacketFront->m_pPacketData);
							unsigned long ulTimestampCurrent=VIDEC_HEADER_GET_TIMESTAMP(pPacketData);
							if (ulTimestampCurrent-ulTimestampFront>2000)
							{
								while (m_ListPackets.size())
								{
									PPACKET pPacket=(PPACKET)m_ListPackets.front();
									if (VIDEC_HEADER_GET_SUB_KEYFRAME3(pPacket->m_pPacketData))
										break;
									m_ListPackets.pop_front();

									FREEMSG(pPacket->m_pPacketData);
									delete pPacket;
									pPacket=NULL;
								}

								m_bGotLastSeqNo=false;
								m_bGotEvenKeyFrame=false;
								m_bGotOddKeyFrame=false;
							}
						}
					}		

					if (VIDEC_HEADER_GET_EVEN_KEYFRAME(pPacketData) && VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData))
					{
						m_bGotEvenKeyFrame=true;
					}
					if (VIDEC_HEADER_GET_ODD_KEYFRAME(pPacketData) && VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData))
					{
						m_bGotOddKeyFrame=true;
					}
				}

				if (!m_bGotLastSeqNo)
				{
					return -1;
				}

				if (!m_bGotEvenKeyFrame && !m_bGotOddKeyFrame)
				{
					return -1;
				}
			}
			else if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
			{
				unsigned short usCurSeq=VIDEC_HEADER_EXT_GET_SEQUENCE(pPacketData);
				unsigned long nFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);

				m_nFlagFrameFromHardware=VIDEC_HEADER_EXT_GET_HARDWARE(pPacketData);

				//中途有丢包，可能是播放器做了SEEK操作
				if (m_bGotLastSeqNo)
				{
					m_usLastSeqNo++;


					if (m_usLastSeqNo!=usCurSeq)
					{
						if (m_usLastSeqNo==(unsigned short)(usCurSeq-1))
						{
							m_usLastSeqNo=usCurSeq;

						}
						else
						{
							m_bGotLastSeqNo=false;
						}
					}
				}

				bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
				if (bKeyFrame)
				{
					VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_GET_CODEC_ID(pPacketData);
					if (codecType==VIDEC_CODEC_H264 ||
						codecType==VIDEC_CODEC_H264_SVC)
					{
						PAVCON_H264_HEADER pAVCONH264Header=(PAVCON_H264_HEADER)((unsigned char*)pPacketData+VIDEC_HEADER_EXT_GET_LEN(pPacketData));
						if (pAVCONH264Header->ID==0xFFFF)
						{
							m_nFrameRateWidthKeyFrame=pAVCONH264Header->framerate;
						}
						else
						{
							m_nFrameRateWidthKeyFrame=0;
						}
					}
					else
					{
						m_nFrameRateWidthKeyFrame=0;
					}
				}

				if (!m_bGotLastSeqNo)
				{
					if (bKeyFrame)
					{
						m_bGotLastSeqNo=true;
						m_usLastSeqNo=usCurSeq;
					}
					else
					{
						return -1;
					}
				}

				if (bKeyFrame)
				{
					//最多缓冲一秒的数据
					{
						XAutoLock l(m_csListPackets);
						if (m_ListPackets.size()>10)
						{
							PPACKET pPacketFront=(PPACKET)m_ListPackets.front();
							unsigned long ulTimestampFront=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketFront->m_pPacketData);
							unsigned long ulTimestampCurrent=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
							if (ulTimestampCurrent-ulTimestampFront>2000)
							{
								while (m_ListPackets.size())
								{
									PPACKET pPacket=(PPACKET)m_ListPackets.front();
									if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacket->m_pPacketData))
										break;
									m_ListPackets.pop_front();

									FREEMSG(pPacket->m_pPacketData);
									delete pPacket;
									pPacket=NULL;
								}
							}
						}
					}		

					m_bGotKeyFrame=true;
				}

				if (!m_bGotLastSeqNo)
				{
					return -1;
				}

				if (!m_bGotKeyFrame)
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}
		/*else 
		{
			if (m_nMilliSecond > 0)
			{
				if (VIDEC_HEADER_IS_VALID(pPacketData) || VIDEC_HEADER_EXT_IS_VALID(pPacketData))
				{
					bool bKeyFrame=VIDEC_HEADER_GET_KEY_FRAME(pPacketData)!=0;
					if (bKeyFrame)
					{					
						XAutoLock l(m_csListPackets);
						if (m_ListPackets.size() >= (m_nDelayFrameCount * 4))
						{
							while (m_ListPackets.size() > m_nDelayFrameCount * 2)
							{
								PPACKET pPacket=(PPACKET)m_ListPackets.front();
								if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacket->m_pPacketData))
									break;
								m_ListPackets.pop_front();
								FREEMSG(pPacket->m_pPacketData);
								delete pPacket;
								pPacket=NULL;
							}
						}
					}
				}
				else
				{
					return -1;
				}
			}
		}*/

		{
			//解压缩的时候需要冗余的空间
			void*pPacketBuf=MALLOCMSG(nPacketSize*1.25);
			if (pPacketBuf==NULL)
			{
				return -1;
			}

			memcpy(pPacketBuf,pPacketData,nPacketSize);

			NEWRECORD3( PPACKET ,pPacket,new PACKET(pPacketBuf,nPacketSize,60) )
//NULLReturnRet( pPacket , -1 )  
			if (pPacket==NULL)
			{
				FREEMSG(pPacketBuf);
				pPacketBuf=NULL;

				return -1;
			}
			else if (m_nMilliSecond > 0)
			{
				pPacket->m_ulRecvTime = timeGetTime();
			}

			{
				XAutoLock l(m_csListPackets);
				m_ListPackets.push_back(pPacket);
			}
		}
	}

	return 0;
}

int XDecoderStreamBase::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	return -1;
}

int XDecoderStreamBase::DoDecodePacket(void*pPacketData,int nPacketSize,int nFrameRate)
{
	bool bGetKeyFrame =false;
	if (VIDEC_HEADER_IS_VALID(pPacketData))
	{
		unsigned char*pStreamData=NULL;
		int nStreamLen=0;

		int nHeaderLen=VIDEC_HEADER_GET_LEN(pPacketData);
		pStreamData=(unsigned char*)pPacketData+nHeaderLen;
		nStreamLen=nPacketSize-nHeaderLen;

		VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_GET_CODEC_ID(pPacketData);

		if (codecType>=VIDEC_CODEC_COUNT)
		{
			return -1;
		}

		unsigned short usCurSeqDecode=VIDEC_HEADER_GET_SEQUENCE(pPacketData);
		//中途有丢包，可能是播放器做了SEEK操作
		if (m_bGotLastSeqNoDecode)
		{
			m_usLastSeqNoDecode++;
			if (m_usLastSeqNoDecode!=usCurSeqDecode)
			{
				m_bGotLastSeqNoDecode=false;

				//char szDebug[1024];
				//sprintf(szDebug,"1m_usLastSeqNoDecode=%u usCurSeqDecode=%u\n",m_usLastSeqNoDecode,usCurSeqDecode);
				//DebugOutput(szDebug);
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			bool bKeyFrame=VIDEC_HEADER_GET_KEY_FRAME(pPacketData)!=0;
			bool bSubKeyFrame0=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;
			if (bKeyFrame && bSubKeyFrame0)
			{
				m_bGotLastSeqNoDecode=true;

				//char szDebug[1024];
				//sprintf(szDebug,"2m_usLastSeqNoDecode=%u usCurSeqDecode=%u\n",m_usLastSeqNoDecode,usCurSeqDecode);
				//DebugOutput(szDebug);

				m_usLastSeqNoDecode=usCurSeqDecode;

				m_bGotEvenKeyFrameDecode=false;
				m_bGotOddKeyFrameDecode=false;

				m_bGotEvenKeyFrame0=false;
				m_bGotEvenKeyFrame1=false;
				m_bGotEvenKeyFrame2=false;
				m_bGotEvenKeyFrame3=false;
				m_bGotOddKeyFrame0=false;
				m_bGotOddKeyFrame1=false;
				m_bGotOddKeyFrame2=false;
				m_bGotOddKeyFrame3=false;
			}
			else
			{
				return -1;
			}
		}

		m_ulFrameTimestamp=VIDEC_HEADER_GET_TIMESTAMP(pPacketData);
		m_bEvenFieldFirst=VIDEC_HEADER_GET_EVEN_FIELD_FIRST(pPacketData)!=0;
		m_bDoubleField=VIDEC_HEADER_GET_DOUBLE_FIELD(pPacketData)!=0;
		unsigned int nEvenSize=VIDEC_HEADER_GET_EVEN_SIZE(pPacketData);
		unsigned int nOddSize=VIDEC_HEADER_GET_ODD_SIZE(pPacketData);

		bool bDecoderChanged=false;

		if (VIDEC_HEADER_GET_KEY_FRAME(pPacketData))
		{
			unsigned short usWidth=VIDEC_HEADER_GET_ACTUAL_WIDTH(pPacketData);
			unsigned short usHeight=VIDEC_HEADER_GET_ACTUAL_HEIGHT(pPacketData);
			m_nVirtualWidth=VIDEC_HEADER_GET_VIRTUAL_WIDTH(pPacketData);
			m_nVirtualHeight=VIDEC_HEADER_GET_VIRTUAL_HEIGHT(pPacketData);
			if (usWidth!=m_nPacketWidth || m_nPacketHeight!=usHeight)
			{
				m_nPacketWidth=usWidth;
				m_nPacketHeight=usHeight;
				//bDecoderChanged=true;
			}

			bool bGotEvenKeyFrame=VIDEC_HEADER_GET_EVEN_KEYFRAME(pPacketData)!=0;
			if (!m_bGotEvenKeyFrameDecode)
			{
				m_bGotEvenKeyFrameDecode=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;
			}
			if ( bGotEvenKeyFrame && m_bGotEvenKeyFrameDecode)
			{
				m_bGotEvenKeyFrameDecode=true;
				if (VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData))
				{
					m_bGotEvenKeyFrame0=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME1(pPacketData))
				{
					m_bGotEvenKeyFrame1=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME2(pPacketData))
				{
					m_bGotEvenKeyFrame2=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME3(pPacketData))
				{
					m_bGotEvenKeyFrame3=true;
				}

				//char szDebug[1024];
				//sprintf(szDebug,"m_bGotEvenKeyFrame0=%u m_bGotEvenKeyFrame1=%u m_bGotEvenKeyFrame2=%u m_bGotEvenKeyFrame3=%u\n",m_bGotEvenKeyFrame0,m_bGotEvenKeyFrame1,m_bGotEvenKeyFrame2,m_bGotEvenKeyFrame3);
				//DebugOutput(szDebug);


				int nISliceIndex=0;
				if (m_bGotEvenKeyFrame0)
					nISliceIndex++;
				if (m_bGotEvenKeyFrame1)
					nISliceIndex++;
				if (m_bGotEvenKeyFrame2)
					nISliceIndex++;
				if (m_bGotEvenKeyFrame3)
					nISliceIndex++;
			}

			bool bGotOddKeyFrame=VIDEC_HEADER_GET_ODD_KEYFRAME(pPacketData)!=0;
			if (!m_bGotOddKeyFrameDecode)
			{
				m_bGotOddKeyFrameDecode=VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData)!=0;
			}
			if ( bGotOddKeyFrame && m_bGotOddKeyFrameDecode)
			{
				m_bGotOddKeyFrameDecode=true;
				if (VIDEC_HEADER_GET_SUB_KEYFRAME0(pPacketData))
				{
					m_bGotOddKeyFrame0=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME1(pPacketData))
				{
					m_bGotOddKeyFrame1=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME2(pPacketData))
				{
					m_bGotOddKeyFrame2=true;
				}
				if (VIDEC_HEADER_GET_SUB_KEYFRAME3(pPacketData))
				{
					m_bGotOddKeyFrame3=true;
				}
			}

			//m_bGotLastSeqNoDecode=true;
			//m_usLastSeqNoDecode=usCurSeqDecode;
		}

		if (!m_bGotLastSeqNoDecode)
		{
			return -1;
		}
		
		if (!m_bGotEvenKeyFrameDecode && !m_bGotOddKeyFrameDecode)
		{
			return -1;
		}

		if (codecType!=m_codecType)
		{
			m_codecType=codecType;
			bDecoderChanged=true;
		}

		if (bDecoderChanged ||
			m_pDecoderFrameEven==NULL || 
			(m_bDoubleField && m_pDecoderFrameOdd==NULL))
		{
			if (m_pDecoderFrameEven!=NULL)
			{
				m_pDecoderFrameEven->Close();
				delete m_pDecoderFrameEven;
				m_pDecoderFrameEven=NULL;
			}
			m_pDecoderFrameEven=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
			if (m_bDoubleField)
			{
				if (m_pDecoderFrameOdd!=NULL)
				{
					m_pDecoderFrameOdd->Close();
					delete m_pDecoderFrameOdd;
					m_pDecoderFrameOdd=NULL;
				}
				m_pDecoderFrameOdd=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
			}
		}

		if (m_bDoubleField)
		{
			int nRet=0;
			if (m_bEvenFieldFirst)
			{
				if (nEvenSize && m_bGotEvenKeyFrameDecode)
				{
					if (m_pDecoderFrameEven!=NULL)
					{
						nRet|=m_pDecoderFrameEven->DecodeFramePacket(pStreamData,nEvenSize);
					}
				}
				if (nOddSize && m_bGotOddKeyFrameDecode)
				{
					if (m_pDecoderFrameOdd!=NULL)
					{
						nRet|=m_pDecoderFrameOdd->DecodeFramePacket(pStreamData+nEvenSize,nOddSize);
					}
				}
			}
			else
			{
				if (nOddSize && m_bGotOddKeyFrameDecode)
				{
					if (m_pDecoderFrameOdd!=NULL)
					{
						nRet|=m_pDecoderFrameOdd->DecodeFramePacket(pStreamData+nEvenSize,nOddSize);
					}
				}

				if (nEvenSize && m_bGotEvenKeyFrameDecode)
				{
					if (m_pDecoderFrameEven!=NULL)
					{
						nRet|=m_pDecoderFrameEven->DecodeFramePacket(pStreamData,nEvenSize);
					}
				}
			}

			return nRet;
		}
		else
		{
			if (m_pDecoderFrameEven!=NULL)
			{
				return m_pDecoderFrameEven->DecodeFramePacket(pStreamData,nStreamLen);
			}
		}
	}
	else if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
	{
		if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData))
		{
			bGetKeyFrame =true;
			DebugOutput( logsoftdec ,"DoDecodePacket is key frame  . \n");
			if(m_pRawFile) m_pRawFile->LogOutput("DoDecodePacket is key frame  . \n");
		}

		unsigned char*pStreamData=NULL;
		int nStreamLen=0;

		int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);

		if (bGetKeyFrame && *((unsigned char*)pPacketData+nHeaderLen) == 0xFF && *((unsigned char*)pPacketData+nHeaderLen+1) == 0xFF)			//针对编码端在编码出来的数据前添加4个字节表示帧率进行处理，使其还原软解数据
		{
			nHeaderLen += 4;
		}

		pStreamData=(unsigned char*)pPacketData+nHeaderLen;
		nStreamLen=nPacketSize-nHeaderLen;

		VIDEC_CODEC_TYPE codecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_EXT_GET_CODEC_ID(pPacketData);

		if (codecType>=VIDEC_CODEC_COUNT)
		{
			return -1;
		}

		unsigned short usCurSeqDecode=VIDEC_HEADER_EXT_GET_SEQUENCE(pPacketData);

		//char szDebug[1024];
		//sprintf(szDebug,"usCurSeqDecode=%u Timestamp=%u nKeyFrame=%d Main=%d Sub=%d QSub=%d\n",usCurSeqDecode,VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData),VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData),
		//	VIDEC_HEADER_EXT_GET_MAIN_FRAME(pPacketData),VIDEC_HEADER_EXT_GET_SUB_FRAME(pPacketData),VIDEC_HEADER_EXT_GET_QSUB_FRAME(pPacketData));
		//DebugOutput(szDebug);

		//中途有丢包，可能是播放器做了SEEK操作
		if (m_bVideoSkipFrame==false)
		{
			if (m_bGotLastSeqNoDecode)
			{
				m_usLastSeqNoDecode++;
				if (m_usLastSeqNoDecode!=usCurSeqDecode)
				{
					if (m_usLastSeqNoDecode==(unsigned short)(usCurSeqDecode-1))
					{
						m_usLastSeqNoDecode=usCurSeqDecode;
					}
					else
					{
						m_bGotLastSeqNoDecode=false;
					}
				}
			}

			if (!m_bGotLastSeqNoDecode)
			{
				bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
				if (bKeyFrame)
				{
					m_bGotLastSeqNoDecode=true;
					m_usLastSeqNoDecode=usCurSeqDecode;
				}
				else
				{
					if(g_LogUtil) g_LogUtil->LogOutput("DoDecodePacket   GotLastSeqNoDecode . \n");
					return -1;
				}
			}		
		}

		m_ulFrameTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
		m_bEvenFieldFirst=VIDEC_HEADER_EXT_GET_EVEN_FIELD_FIRST(pPacketData)!=0;
		m_bDoubleField=VIDEC_HEADER_EXT_GET_DOUBLE_FIELD(pPacketData)!=0;

		bool bDecoderChanged=false;
		int nKeyFrame=0;

		if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData))
		{
			nKeyFrame=1;

			unsigned short usWidth=VIDEC_HEADER_EXT_GET_ACTUAL_WIDTH(pPacketData);
			unsigned short usHeight=VIDEC_HEADER_EXT_GET_ACTUAL_HEIGHT(pPacketData);
			m_nVirtualWidth=VIDEC_HEADER_EXT_GET_VIRTUAL_WIDTH(pPacketData);
			m_nVirtualHeight=VIDEC_HEADER_EXT_GET_VIRTUAL_HEIGHT(pPacketData);
			if (usWidth!=m_nPacketWidth || m_nPacketHeight!=usHeight)
			{
				m_nPacketWidth=usWidth;
				m_nPacketHeight=usHeight;
			//	bDecoderChanged=true;
			}
		}

		if (m_bVideoSkipFrame==false)
		{
			if (!m_bGotLastSeqNoDecode)
			{
				if(g_LogUtil) g_LogUtil->LogOutput("DoDecodePacket   GotLastSeqNoDecode 2. \n");
				return -1;
			}
		}

		if (codecType!=m_codecType)
		{
			m_codecType=codecType;
			bDecoderChanged=true;
		}

		if (bDecoderChanged || m_pDecoderFrame==NULL)
		{
			m_bGotImage=false;

			if (m_pDecoderFrame!=NULL)
			{
				m_pDecoderFrame->Close();
				delete m_pDecoderFrame;
				m_pDecoderFrame=NULL;
			}

			m_pDecoderFrame=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
		}

		//char szDebug[1024];
		//sprintf(szDebug,"DecoderFarme m_ulFrameTimestamp=%u %u\n",m_ulFrameTimestamp,XGetTimestamp());
		//DebugOutput(szDebug);

		if (m_pDecoderFrame!=NULL)
		{
			//add ??
			bool bGetRawData =false;
			if(  1 && m_pRawFile)
			{
				//m_pRawFile->WriteFile(pPacketData,nPacketSize,bGetKeyFrame);
				bGetRawData = m_pRawFile->GetRawFileData(&pStreamData,&nStreamLen);
				if(!bGetRawData) return 0;
			}

			int nRet = m_pDecoderFrame->DecodeFramePacket(pStreamData,nStreamLen);
			if (nRet<0 && (m_codecType==VIDEC_CODEC_H264 || m_codecType==VIDEC_CODEC_H264_SVC) && m_bGotImage)
			{
				m_bGotLastSeqNoDecode=false;
				m_bGotImage=false;
			}

			//add ??
			if(1 && m_pRawFile && bGetRawData )
			{
				m_pRawFile->ReleaseRawDataBuf();
			}
		}
	}

	return 0;
}

void XDecoderStreamBase::OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage)
{
	if (m_pDecoderFrameEven==pDecoderFrame)
	{
		if (m_bGotEvenKeyFrame0 &&
			m_bGotEvenKeyFrame1 &&
			m_bGotEvenKeyFrame2 &&
			m_bGotEvenKeyFrame3)
		{
		}
		else
		{
			return;
		}
	}
	else if (m_pDecoderFrameOdd==pDecoderFrame)
	{
		if (m_bGotOddKeyFrame0 &&
			m_bGotOddKeyFrame1 &&
			m_bGotOddKeyFrame2 &&
			m_bGotOddKeyFrame3)
		{
		}
		else
		{
			return;
		}
	}
	else if (m_pDecoderFrame==pDecoderFrame)
	{

	}
	else
	{
		return;
	}

	//char szDebug[1024];
	//sprintf(szDebug,"3 m_bGotEvenKeyFrame0=%u m_bGotEvenKeyFrame1=%u m_bGotEvenKeyFrame2=%u m_bGotEvenKeyFrame3=%u\n",m_bGotEvenKeyFrame0,m_bGotEvenKeyFrame1,m_bGotEvenKeyFrame2,m_bGotEvenKeyFrame3);
	//DebugOutput(szDebug);
	
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return;
	}

	pXImage->SetVirtualWidth(m_nVirtualWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetEvenFieldFirst(m_bEvenFieldFirst);
	bool bDoubleField = pXImage->GetDoubleField();
	if (!bDoubleField)
	{
		pXImage->SetDoubleField(m_bDoubleField);
		if (m_pDecoderFrame!=NULL)
		{
			pXImage->SetEvenField(m_bDoubleField);
			pXImage->SetOddField(m_bDoubleField);
		}
		else
		{
			pXImage->SetEvenField((m_bDoubleField && m_pDecoderFrameEven==pDecoderFrame));
			pXImage->SetOddField((m_bDoubleField && m_pDecoderFrameOdd==pDecoderFrame));
		}
	}

	if (pXImage->GetTimestamp()==0)
	{
		unsigned long ulTimestampOffset=pXImage->GetTimestampOffset();
		if (!m_bOffsetInitialized && ulTimestampOffset!=0)
		{
			m_bOffsetInitialized=true;
			m_ulFrameTimeDelta=ulTimestampOffset-m_ulFrameTimestamp;
		}

		unsigned long ulFrameTimestamp=m_ulFrameTimestamp;
		if (ulTimestampOffset!=0)
		{
			ulFrameTimestamp=ulTimestampOffset-m_ulFrameTimeDelta;
		}

		//char szDebug[1024];
		//sprintf(szDebug,"ulFrameTimestamp=%u m_ulFrameTimestamp=%u ulTimestampOffset=%u\n",ulFrameTimestamp,m_ulFrameTimestamp,ulTimestampOffset);
		//DebugOutput(szDebug);

		if (m_pDecoderFrame!=NULL)
		{
			pXImage->SetTimestamp(ulFrameTimestamp);
		}
		else
		{
			if (m_bEvenFieldFirst)
			{
				if (m_pDecoderFrameEven==pDecoderFrame)
				{
					pXImage->SetTimestamp(ulFrameTimestamp);
				}
				else if (m_pDecoderFrameOdd==pDecoderFrame)
				{
					pXImage->SetTimestamp(ulFrameTimestamp+20);
				}
			}
			else
			{
				if (m_pDecoderFrameEven==pDecoderFrame)
				{
					pXImage->SetTimestamp(ulFrameTimestamp+20);
				}
				else if (m_pDecoderFrameOdd==pDecoderFrame)
				{
					pXImage->SetTimestamp(ulFrameTimestamp);
				}
			}
		}
	}

	OnDecoderStreamCallbackFrame(pXImage);
	m_bGotImage=true;
}

bool XDecoderStreamBase::Running(void)
{
	bool bWaite = true;
	bool bRet=false;
	PPACKET pPacket=NULL;
	{
		XAutoLock l(m_csListPackets);
		int nSize = m_ListPackets.size();
		if (nSize < 1)
		{
			return bRet;
		}

		if (m_nMilliSecond == 0)
		{
			pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();
		}
		else
		{			
			if (nSize >= m_nDelayFrameCount)
			{
				m_bDelayFrame=true;
			}
			else if (nSize < 3)
			{
				m_bDelayFrame=false;
			}

			if (nSize < m_nDelayFrameCount && m_bDelayFrame == false)
			{
				PPACKET pPacket=(PPACKET)m_ListPackets.front();
				if (pPacket!=NULL)
				{
					unsigned long ulRecvTime = pPacket->m_ulRecvTime;
					if (timeGetTime() - ulRecvTime < (m_nMilliSecond + 500))
					{
						if(g_LogUtil) g_LogUtil->LogOutput("DecoderStreamBase Running ulRecvTime return. \n");
						return bRet;
					}
				}
				else
				{
					if(g_LogUtil) g_LogUtil->LogOutput("DecoderStreamBase Running ulRecvTime return 2. \n");
					return bRet;
				}
			}
			else if (nSize > m_nDelayFrameCount + 10)
			{
				bWaite = false;
			}

			pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();
		}
	}

	if (pPacket!=NULL)
	{
		unsigned long nTimestamp=XGetTimestamp();
		if (100==DoDecodePacket(pPacket->m_pPacketData,pPacket->m_nPacketSize,pPacket->m_nFrameRate))
		{
			XAutoLock l(m_csListPackets);
			m_ListPackets.push_front(pPacket);
		}
		else
		{
			if (FRE_GetPending()==0 && m_nMilliSecond == 0  && m_bVideoSkipFrame==false)
			{
				OnPreDecodingFrame(m_ulFrameTimestamp,nTimestamp);
			}

			FREEMSG(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;

			bRet=true;
		}

		if (m_nMilliSecond > 0 && bWaite)
		{
			FrameRateEvent::WaitEvent();
		}
	}
	return bRet;
}

void XDecoderStreamBase::ReDraw(void)
{

}

bool XDecoderStreamBase::IsPending(void)
{
	{
		VIDEC_Image*pResetImage=NULL;
		{
			XAutoLock l(m_csResetImage);
			pResetImage=m_pResetImage;
			m_pResetImage=NULL;
		}
		if (pResetImage)
		{
			OnDecoderStreamCallbackFrame(pResetImage);

			pResetImage->Release();
			pResetImage=NULL;
		}
	}
	int nDataCount=0;
	{
		XAutoLock l(m_csListPackets);
		nDataCount=m_ListPackets.size();
		if (nDataCount<=0)
		{
			return false;
		}
	}

	return true;
}

void XDecoderStreamBase::ResetImage(VIDEC_Image*pImage)
{
	{
		VIDEC_Image*pResetImage=NULL;
		{
			XAutoLock l(m_csResetImage);
			pResetImage=m_pResetImage;
			m_pResetImage=pImage;
			m_pResetImage->AddRef();
		}
		if (pResetImage)
		{
			pResetImage->Release();
			pResetImage=NULL;
		}
	}

}

#ifdef _TIANZHU_CACHE_

	void XDecoderStreamBase::SetCacheFrameCount(int FrameCount)
	{
		m_nCacheFrameCount = FrameCount;
	}

#endif

#ifndef USE_XWORKTHREAD
void XDecoderStreamBase::ThreadProcMain(void)
{
	//add ??
	//CRawFileCall  RawFileCall;
	m_pRawFile = NULL;
	g_LogUtil = NULL;
	//m_pRawFile = new CRawFile(/*RawFileCall*/);
	//g_LogUtil = new CLogUtil(1);
	if(g_LogUtil) g_LogUtil->LogOutput("%p_ThreadProcMain start. \n",this);

	while (m_bRunning)
	{
#ifdef _TIANZHU_CACHE_
		//天竺缓冲问题:缓冲控制（bcz）
		if (0 == m_ListPackets.size())
		{
			m_bCacheFlag = false;
		}

		if (false == m_bCacheFlag)
		{
			if (m_ListPackets.size() < m_nCacheFrameCount)
			{
				continue;
			}
			m_bCacheFlag = true;
		}
#endif

		if(m_pRawFile)
		{
			m_pRawFile->CheckWrite();
			//m_pRawFile->CopyAllFileDataToOneFile();
		}

		if (m_bPause)
		{
			XSleep(1);
			continue;
		}

		if (FRE_GetPending()==0 && m_nMilliSecond == 0  && m_bVideoSkipFrame==false)
		{
			if (IsPending())
			{
				Running();
			}
			else
			{
				ReDraw();
				XSleep(1);
			}
		}
		else
		{
			if (!Running())
			{
				ReDraw();
				XSleep(1);
			}
		}
	}

	//add ,??
	if(m_pRawFile)
	{
		delete m_pRawFile;
		m_pRawFile =NULL;
	}
	if(g_LogUtil)
	{
		delete g_LogUtil;
		g_LogUtil =NULL;
	}

}
#endif