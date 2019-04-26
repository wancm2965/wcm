#include <stdlib.h>
#include <memory.h>
#include "XDecoderStreamRTP.h"

#include "../VIDEC_API/XImage.h"
#include "XUtil.h"


VIDEC_API VIDEC_DecoderStream*VIDEC_DecoderStreamCreate(VIDEC_DecoderStreamCallback&rCallback,int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType)
{
	NEWRECORD3( XDecoderStreamRTP*,pXDecoderStream ,new XDecoderStreamRTP(rCallback) )
	NULLReturnRet( pXDecoderStream , pXDecoderStream )  
	if (pXDecoderStream->Open(nWidth,nHeight,CodecType)!=0)
	{
		pXDecoderStream->Close();
		delete pXDecoderStream;
		pXDecoderStream=NULL;
	}

	return pXDecoderStream;
}

XDecoderStreamRTP::XDecoderStreamRTP(VIDEC_DecoderStreamCallback&rCallback)
:m_rCallback(rCallback)

,m_codecType(VIDEC_CODEC_COUNT)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_bDoubleField(false)
#ifndef USE_XWORKTHREAD
,m_bRunning(false)
#endif
,m_nPacketWidth(0)
,m_nPacketHeight(0)

,m_ulFrameTimestamp(0)
{
	m_bGotKeyFrame=false;
	m_pDecoderFrame=NULL;
#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XDecoderStreamRTP::~XDecoderStreamRTP(void)
{
}

int XDecoderStreamRTP::Open(int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType)
{
	m_nPacketWidth=nWidth;
	m_nPacketHeight=nHeight;
	m_codecType=CodecType;
	m_nVirtualWidth=nWidth;
	m_nVirtualHeight=nHeight;

#ifdef USE_XWORKTHREAD
	return XWorkThread::Start();
#else
	m_bRunning=true;
	if (XThreadBase::StartThread()==false)
	{
		m_bRunning=false;
		return -1;
	}
#endif

	return 0;
}

void XDecoderStreamRTP::Close(void)
{
#ifdef USE_XWORKTHREAD
	XWorkThread::Stop();
#else
	m_bRunning=false;
	XThreadBase::WaitForStop();
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

	if (m_pDecoderFrame!=NULL)
	{
		m_pDecoderFrame->Close();
		delete m_pDecoderFrame;
		m_pDecoderFrame=NULL;
	}
}

int XDecoderStreamRTP::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	return -1;
}

int XDecoderStreamRTP::DecodeRTPPacket(void*pPacketData,int nPacketSize)
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
		//最多缓冲一秒的数据
		//{
		//	XAutoLock l(m_csListPackets);
		//	if (m_ListPackets.size()>5)
		//	{
		//		PPACKET pPacketFront=(PPACKET)m_ListPackets.front();
		//		unsigned long ulTimestampFront=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketFront->m_pPacketData);
		//		unsigned long ulTimestampCurrent=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
		//		if (ulTimestampCurrent-ulTimestampFront>1000)
		//		{
		//			while (m_ListPackets.size())
		//			{
		//				PPACKET pPacket=(PPACKET)m_ListPackets.back();
		//				if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacket->m_pPacketData))
		//					break;
		//				m_ListPackets.pop_back();

		//				FREEMSG(pPacket->m_pPacketData);
		//				delete pPacket;
		//				pPacket=NULL;
		//			}

		//			m_bGotLastSeqNo=false;
		//		}
		//	}
		//}		

		{
			//解压缩的时候需要冗余的空间
			void*pPacketBuf=MALLOCMSG(nPacketSize*1.25);
			if (pPacketBuf==NULL)
			{
				return -1;
			}

			memcpy(pPacketBuf,pPacketData,nPacketSize);

			NEWRECORD3( PPACKET ,pPacket,new PACKET(pPacketBuf,nPacketSize) )
			NULLReturnRet( pPacket , -1 )  
			if (pPacket==NULL)
			{
				FREEMSG(pPacketBuf);
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

int XDecoderStreamRTP::DoDecodePacket(void*pPacketData,int nPacketSize)
{
	if (m_pDecoderFrame==NULL)
	{
		m_pDecoderFrame=VIDEC_DecoderCreate(*this,m_codecType,m_nPacketWidth,m_nPacketHeight);
	}

	if (m_pDecoderFrame!=NULL)
	{
		return m_pDecoderFrame->DecodeRTPPacket(pPacketData,nPacketSize);
	}

	return -1;
}

void XDecoderStreamRTP::OnVIDEC_DecoderCallbackFrame(VIDEC_Decoder*pDecoderFrame,VIDEC_Image*pImage)
{

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
	pXImage->SetEvenFieldFirst(1);
	pXImage->SetDoubleField(m_bDoubleField);
	pXImage->SetEvenField(m_bDoubleField);
	pXImage->SetOddField(m_bDoubleField);

	if (pXImage->GetTimestamp()==0)
	{
		if (m_pDecoderFrame!=NULL)
		{
			pXImage->SetTimestamp(XGetTimestamp());
		}
	}

	m_rCallback.OnVIDEC_DecoderStreamCallbackFrame(pXImage);
}

void XDecoderStreamRTP::OnVIDEC_DecoderCallbackFramePacket(VIDEC_Decoder*pDecoderFrame,void*pPacketData,int nPacketLen,int nWidth,int nHeight)
{
	m_rCallback.OnVIDEC_DecoderStreamCallbackFramePacket(pPacketData,nPacketLen,nWidth,nHeight);
}

#ifdef USE_XWORKTHREAD

bool XDecoderStreamRTP::IsPending(void)
{
	XAutoLock l(m_csListPackets);
	if (m_ListPackets.size())
	{
		return true;
	}
	return false;
}

bool XDecoderStreamRTP::Running(void)
{
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
		DoDecodePacket(pPacket->m_pPacketData,pPacket->m_nPacketSize);

		FREEMSG(pPacket->m_pPacketData);
		delete pPacket;
		pPacket=NULL;

		return true;
	}

	return false;
}

#else

void XDecoderStreamRTP::ThreadProcMain(void)
{
	PPACKET pPacket=NULL;
	while (m_bRunning)
	{
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
			DoDecodePacket(pPacket->m_pPacketData,pPacket->m_nPacketSize);

			FREEMSG(pPacket->m_pPacketData);
			delete pPacket;
			pPacket=NULL;
		}
		else
		{
			XSleep(1);
		}
	}
}
#endif
