//XPlayChanRTP.cpp
#include "XPlayChanRTP.h"
#include "XUtil.h"
#include <VIDEC/VIDEC_Header.h>

MPVideo_PlayChanRTP*MPVideo_PlayChanRTP::Create(void)
{
	return new XPlayChanRTP();
}

XPlayChanRTP::XPlayChanRTP(void)
:XImageSource(this)
,XStreamSource(this)

,m_nPayloadType(34)
,m_pDecoderStream(NULL)
{

}

XPlayChanRTP::~XPlayChanRTP(void)
{

}

int XPlayChanRTP::Open(int nPayloadType,VIDEC_CODEC_TYPE CodecType)
{
	if (m_pDecoderStream==NULL)
	{
		m_pDecoderStream=MPVideo_DecoderStreamCreate(*this,0,0,CodecType);
		if (m_pDecoderStream==NULL)
		{
			return -1;
		}
	}
	return 0;
}

void XPlayChanRTP::Close(void)
{
	XImageSource::Close();
	XStreamSource::Close();

	if (m_pDecoderStream!=NULL)
	{
		m_pDecoderStream->Close();
		delete m_pDecoderStream;
		m_pDecoderStream=NULL;
	}
}

int XPlayChanRTP::AddImageSink(MPVideo_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XPlayChanRTP::RemoveImageSink(MPVideo_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}


void XPlayChanRTP::OnRecvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	if (m_pDecoderStream!=NULL)
	{
		m_pDecoderStream->DecodeRTPPacket(pRTPPacketData,nRTPPacketLen);
	}
}

void XPlayChanRTP::OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage)
{
	SendImage(pImage);
}
	
void XPlayChanRTP::OnMPVideo_DecoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
	XStreamSource::SendStreamData((unsigned char*)pPacketData,nPacketSize,nWidth,nHeight,nHeaderLen);
}

int XPlayChanRTP::AddStreamSink(MPVideo_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XPlayChanRTP::RemoveStreamSink(MPVideo_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}
