#include "Transcode.h"
#include <VIDEC/VIDEC_Header.h>


MPVideo_Transcode*MPVideo_Transcode::Create(MPVideo_TranscodeCallback&rCallback)
{
	return new Transcode(rCallback);
}


Transcode::Transcode(MPVideo_TranscodeCallback&rCallback)
:m_rCallback(rCallback)
,m_pDecoderStream(NULL)
,m_pEncoderStream(NULL)
,m_nBitrate(512)
,m_nBitrateControlType(MPVIDEO_NETWORK_INTERNET)
,m_nQMin(16)
,m_nQMax(36)
,m_nSkipFrameCount(0)
,m_nSkipFrameIndex(0)
{

}

Transcode::~Transcode(void)
{

}


int Transcode::Open(int nBitrate,int nBitrateControlType)
{
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;
	VIDEC_CODEC_TYPE nCodecType=VIDEC_CODEC_H264_SVC;
	
	GetCodecInfo(nBitrate,m_nBitrateControlType,m_nBitrate,m_nQMin,m_nQMax,nCodecType,false);

	if (m_pEncoderStream==NULL)
	{
		m_pEncoderStream=MPVideo_EncoderStreamCreate(*this,false);
		if (m_pEncoderStream==NULL)
		{
			return -1;
		}
	}

	if (m_pDecoderStream==NULL)
	{
		m_pDecoderStream=MPVideo_DecoderStreamCreate(*this);
		if (m_pDecoderStream==NULL)
		{
			return -1;
		}
	}

	return 0;
}

void Transcode::Close(void)
{
	if (m_pDecoderStream!=NULL)
	{
		m_pDecoderStream->Close();
		delete m_pDecoderStream;
		m_pDecoderStream=NULL;
	}
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->Close();
		delete m_pEncoderStream;
		m_pEncoderStream=NULL;
	}
}

int Transcode::TranscodePacketData(void*pPacketData,int nPacketLen)
{
	if (m_pDecoderStream!=NULL)
	{
		m_pDecoderStream->DecodeFramePacket(pPacketData,nPacketLen);
	}
	return 0;
}

void Transcode::RequestKeyFrame(void)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->RequestKeyFrame();
	}
}

void Transcode::SetSkipFrameCount(int nSkipFrameCount)
{
	if (nSkipFrameCount<0)
		nSkipFrameCount=0;
	else if (nSkipFrameCount>2)
		nSkipFrameCount=2;
	m_nSkipFrameCount=nSkipFrameCount;
}


void Transcode::OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage)
{
	if (m_nSkipFrameCount)
	{
		m_nSkipFrameIndex++;
		if (m_nSkipFrameIndex<m_nSkipFrameCount)
		{
			return;
		}
		m_nSkipFrameIndex=0;
	}

	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->EncodeFrame(pImage,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,VIDEC_CODEC_H264_SVC);
	}
}

void Transcode::OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	m_rCallback.OnMPVideo_TranscodeCallbackPacketData(this,pPacketData,nPacketSize);
}
