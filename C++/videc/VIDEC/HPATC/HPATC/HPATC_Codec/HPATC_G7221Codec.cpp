// HPATC_G7221Codec.cpp: implementation of the HPATC_G7221Codec class.
//
//////////////////////////////////////////////////////////////////////
#ifdef WIN32
#include <windows.h>
#else
#include <memory.h>
#endif



#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>


#include "HPATC_G7221Codec.h"

#define G7221_FRAME_SIZE				320
#define G7221_ENCODER_SAMPLES			640
#define G7221_SAMPLES_PER_FRAME			640

//
//HPATCEncoder*HPATCEncoder::CreateG7221Encoder(HPATCEncoderNotify&rHPATCEncoderNotify)
//{
//	return new HPATC_G7221Encoder(rHPATCEncoderNotify);
//}

HPATCDecoder*HPATCDecoder::CreateG7221Decoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_G7221Decoder(rHPATCDecoderNotify);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HPATC_G7221Encoder::HPATC_G7221Encoder(HPATCEncoderNotify&rHPATCEncoderNotify)
:m_rHPATCEncoderNotify(rHPATCEncoderNotify)
,m_pEncoder(NULL)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
{
}

HPATC_G7221Encoder::~HPATC_G7221Encoder(void)
{

}

int HPATC_G7221Encoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pEncoder == NULL )
		m_pEncoder = g722_1_encode_init(NULL, nBitrate, nSampleRate);
	if (!m_pEncoder)
		return -1;
	return 0;
}

void HPATC_G7221Encoder::ReleaseConnections(void)
{
	if (m_pEncoder!=NULL)
	{
		g722_1_encode_release(m_pEncoder);
		m_pEncoder=NULL;
	}
}

int HPATC_G7221Encoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (pSamples!=NULL && m_pEncoder!=NULL && nSamples==m_nSamplesPerFrame)
	{
		unsigned char pEncoded[G7221_FRAME_SIZE];
		int nFrameLen=0;
		nFrameLen = g722_1_encode(m_pEncoder, pEncoded, pSamples, m_nSamplesPerFrame);
		if (nFrameLen>0)
		m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded, nFrameLen,ulTimestamp);
		return 0;
	}
	return -1;
}

HPATC_G7221Decoder::HPATC_G7221Decoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
,m_pDecoder(NULL)
,m_bInited(false)
{
}

HPATC_G7221Decoder::~HPATC_G7221Decoder(void)
{

}

int HPATC_G7221Decoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pDecoder == NULL )
		m_pDecoder = g722_1_decode_init(NULL, nBitrate, nSampleRate);
	if (!m_pDecoder)
		return -1;

	return 0;
}

void HPATC_G7221Decoder::ReleaseConnections(void)
{
	if (m_pDecoder!=NULL)
	{
		g722_1_decode_release(m_pDecoder);
		m_pDecoder=NULL;
	}
}

int HPATC_G7221Decoder::InputStream(unsigned char*pEncoded, int nLen, bool bDiscontinious,unsigned long ulTimestamp)
{
	if (pEncoded!=NULL && nLen > 0)
	{
		int ret = 0;
		short pSamples[960] = {0};
		if (bDiscontinious)
		{
			bDiscontinious=false;
			unsigned char pTempEncoded[640]={0};
			ret = g722_1_fillin(m_pDecoder, pSamples, pTempEncoded, nLen);
			if( ret == m_nSamplesPerFrame )
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pSamples, m_nSamplesPerFrame, m_nSampleRate, bDiscontinious,ulTimestamp-10);
			}
		}
		//else
		{
			ret = g722_1_decode(m_pDecoder, pSamples, pEncoded, nLen);
			if( ret == m_nSamplesPerFrame )
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pSamples, m_nSamplesPerFrame, m_nSampleRate, bDiscontinious,ulTimestamp);
			}
		}
		return 0;
	}
	return -1;
}

