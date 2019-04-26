//XPlayChan.cpp
#include "XPlayChan.h"
#include "XUtil.h"
#include "VIDEC_Header.h"

VIDEC_PlayChan*VIDEC_PlayChan::Create(void)
{
	//return new XPlayChan();
	NEWRECORDret3(  VIDEC_PlayChan* ,p , new XPlayChan()  )  
}

XPlayChan::XPlayChan(void)
:XImageSource(this)
,XStreamSink(this)
,m_pDecoderStream(NULL)
{

}

XPlayChan::~XPlayChan(void)
{

}


int XPlayChan::Open(void)
{
	if (m_pDecoderStream==NULL)
	{
		m_pDecoderStream=VIDEC_DecoderStreamCreate(*this);
		if (m_pDecoderStream==NULL)
		{
			return -1;
		}
	}

	return 0;
}

void XPlayChan::Close(void)
{
	XStreamSink::Close();
	XImageSource::Close();

	VIDEC_DecoderStream*pDecoderStream=m_pDecoderStream;
	{
		XAutoLock l(m_csDecoderStream);
		m_pDecoderStream=NULL;
	}

	if (pDecoderStream!=NULL)
	{
		pDecoderStream->Close();
		delete pDecoderStream;
		pDecoderStream=NULL;
	}
}

int XPlayChan::GetAverageKBPS(void)
{
	return m_XBitrate.GetBitrate();
}

//static unsigned long ulLastTimestamp=0;
int XPlayChan::InputPacket(unsigned char*pPacketData,int nPacketLen)
{
	//static unsigned long ulLastTimestamp=0;
	//unsigned long ulTimestamp=VIDEC_HEADER_EXT_GET_TIMESTAMP(pPacketData);
	//unsigned long ulDelta=ulTimestamp-ulLastTimestamp;
	////if (ulDelta>100)
	//{
	//	char szDebug[1024]="";
	//	sprintf(szDebug,"XPlayChan::InputPacket %u %u\n",ulTimestamp-ulLastTimestamp,ulTimestamp);
	//	DebugOutput(szDebug);
	//	ulLastTimestamp=ulTimestamp;
	//}

	m_XBitrate.Calculate(nPacketLen);

	XAutoLock l(m_csDecoderStream);
	if (m_pDecoderStream!=NULL)
	{
		return m_pDecoderStream->DecodeFramePacket(pPacketData,nPacketLen);
	}
	return -1;
}

int XPlayChan::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XPlayChan::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XPlayChan::OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	SendImage(pImage);
}


void XPlayChan::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XStreamSink::OnAttachedToStreamSource(pStreamSource);
}

void XPlayChan::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XStreamSink::OnDetachedFromStreamSource(pStreamSource);
}

void XPlayChan::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	InputPacket(pData,nLen);
}
