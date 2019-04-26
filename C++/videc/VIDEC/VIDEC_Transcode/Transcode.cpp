#include "Transcode.h"
#include <VIDEC/VIDEC_Header.h>
#include "../VIDEC_FRE/VIDECFREHeader.h"

VIDEC_Transcode*VIDEC_Transcode::Create(VIDEC_TranscodeCallback&rCallback)
{
	//return new Transcode(rCallback);
	NEWRECORDret3(  VIDEC_Transcode* ,p , new Transcode( rCallback) )   
}


Transcode::Transcode(VIDEC_TranscodeCallback&rCallback)
:m_rCallback(rCallback)
,m_pDecoderStream(NULL)
,m_pEncoderStream(NULL)
,m_nBitrate(512)
,m_nBitrateControlType(VIDEC_NETWORK_INTERNET)
,m_nQMin(16)
,m_nQMax(36)
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
		m_pEncoderStream=VIDEC_EncoderStreamCreate(*this,false);
		if (m_pEncoderStream==NULL)
		{
			return -1;
		}
	}

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
	//VIDEC_CODEC_TYPE nCodecType=(VIDEC_CODEC_TYPE)VIDEC_HEADER_EXT_GET_CODEC_ID(pPacketData);
	//if (nCodecType==VIDEC_CODEC_H264_SVC || nCodecType==VIDEC_CODEC_H264)
	//{
	//	m_rCallback.OnVIDEC_TranscodeCallbackPacketData(this,pPacketData,nPacketLen);
	//	return 0;
	//}

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


void Transcode::OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->EncodeFrame(pImage,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,VIDEC_CODEC_H264_SVC);
	}
}

void Transcode::OnVIDEC_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	m_rCallback.OnVIDEC_TranscodeCallbackPacketData(this,pPacketData,nPacketSize);
}
