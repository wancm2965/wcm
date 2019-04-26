#include <stdlib.h>
#include <memory.h>
#include "XDecoderStreamBase.h"
#include <VIDEC/VIDEC_Header.h>
#include "../MPVIDEO_API/XImage.h"
#include "XUtil.h"

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

,m_nPacketWidth(0)
,m_nPacketHeight(0)

,m_ulFrameTimestamp(0)
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


#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XDecoderStreamBase::~XDecoderStreamBase(void)
{
	{
		XAutoLock l(m_csListPackets);
		while (m_ListPackets.size())
		{
			PPACKET pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();

			free(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;
		}
	}
}

int XDecoderStreamBase::Open(void)
{
	if (0!=XWorkThread::Start())
	{
		return -1;
	}

	return 0;
}

void XDecoderStreamBase::Close(void)
{
	XWorkThread::Stop();

	{
		XAutoLock l(m_csListPackets);
		while (m_ListPackets.size())
		{
			PPACKET pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();

			free(pPacket->m_pPacketData);
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
}

int XDecoderStreamBase::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return -1;
	}
#endif

	//static int nCount=0;
	//if (nCount++%100<=3)
	//{
	//	return 0;
	//}

	if (pPacketData && nPacketSize>0)
	{
		if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
		{
			unsigned short usCurSeq=VIDEC_HEADER_EXT_GET_SEQUENCE(pPacketData);

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
					if (m_ListPackets.size()>5)
					{
						PPACKET pPacketFront=(PPACKET)m_ListPackets.front();
						unsigned long ulTimestampFront=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketFront->m_pPacketData);
						unsigned long ulTimestampCurrent=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
						if (ulTimestampCurrent-ulTimestampFront>1000)
						{
							while (m_ListPackets.size())
							{
								PPACKET pPacket=(PPACKET)m_ListPackets.front();
								if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacket->m_pPacketData))
									break;
								m_ListPackets.pop_front();

								free(pPacket->m_pPacketData);
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

		{
			//解压缩的时候需要冗余的空间
			void*pPacketBuf=malloc(nPacketSize*1.25);
			if (pPacketBuf==NULL)
			{
				return -1;
			}

			memcpy(pPacketBuf,pPacketData,nPacketSize);

			PPACKET pPacket=new PACKET(pPacketBuf,nPacketSize);
			if (pPacket==NULL)
			{
				free(pPacketBuf);
				pPacketBuf=NULL;

				return -1;
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

int XDecoderStreamBase::DoDecodePacket(void*pPacketData,int nPacketSize)
{
	if (VIDEC_HEADER_EXT_IS_VALID(pPacketData))
	{
		unsigned char*pStreamData=NULL;
		int nStreamLen=0;

		int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
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
		//OutputDebugString(szDebug);

		//中途有丢包，可能是播放器做了SEEK操作
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
				return -1;
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
				bDecoderChanged=true;
			}
		}

		if (!m_bGotLastSeqNoDecode)
		{
			return -1;
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

			m_pDecoderFrame=MPVideo_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
		}

		//char szDebug[1024];
		//sprintf(szDebug,"DecoderFarme m_ulFrameTimestamp=%u %u\n",m_ulFrameTimestamp,XGetTimestamp());
		//OutputDebugString(szDebug);

		if (m_pDecoderFrame!=NULL)
		{
			int nRet = m_pDecoderFrame->DecodeFramePacket(pStreamData,nStreamLen);
			if (nRet<0 && m_codecType==VIDEC_CODEC_H264 && m_bGotImage)
			{
				m_bGotLastSeqNoDecode=false;
				m_bGotImage=false;
				m_codecType=VIDEC_CODEC_H264_SVC;
			}
		}
	}

	return -1;
}

void XDecoderStreamBase::OnMPVideo_DecoderCallbackFrame(MPVideo_Decoder*pDecoderFrame,MPVideo_Image*pImage)
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
	//OutputDebugString(szDebug);
	
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return;
	}

	pXImage->SetVirtualWidth(m_nVirtualWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetEvenFieldFirst(m_bEvenFieldFirst);
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
		//OutputDebugString(szDebug);

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
	bool bRet=false;
	PPACKET pPacket=NULL;
	{
		XAutoLock l(m_csListPackets);
		if (m_ListPackets.size())
		{
			pPacket=(PPACKET)m_ListPackets.front();
			m_ListPackets.pop_front();
		}
	}

	if (pPacket!=NULL)
	{
		unsigned long nTimestamp=XGetTimestamp();
		DoDecodePacket(pPacket->m_pPacketData,pPacket->m_nPacketSize);
		OnPreDecodingFrame(m_ulFrameTimestamp,nTimestamp);

		free(pPacket->m_pPacketData);
		delete pPacket;
		pPacket=NULL;

		bRet=true;
	}

	return bRet;
}

bool XDecoderStreamBase::IsPending(void)
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

	return true;
}
