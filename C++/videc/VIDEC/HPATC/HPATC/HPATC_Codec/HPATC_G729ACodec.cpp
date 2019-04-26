// HPATC_G729ACodec.cpp: implementation of the HPATC_G729ACodec class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "HPATC_G729ACodec.h"


#define G729A_FRAME_SIZE					10
#define G729A_ENCODER_SAMPLES				80
#define G729A_SAMPLES_PER_FRAME				160

HPATCDecoder*HPATCDecoder::CreateG729Decoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_G729ADecoder(rHPATCDecoderNotify);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HPATC_G729AEncoder::HPATC_G729AEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
:m_rHPATCEncoderNotify(rHPATCEncoderNotify)
,m_pEncoder(NULL)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
{
	m_pLeftSamples=NULL;
	m_nLeftSamples=0;
}

HPATC_G729AEncoder::~HPATC_G729AEncoder(void)
{

}

int HPATC_G729AEncoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pEncoder == NULL )
		m_pEncoder = va_g729a_init_encoder();
	if (!m_pEncoder)
		return -1;

	m_pLeftSamples=(short*)malloc(G729A_ENCODER_SAMPLES*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_G729AEncoder::ReleaseConnections(void)
{
	if (m_pEncoder!=NULL)
	{
		va_g729a_free_encoder(m_pEncoder);
		m_pEncoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_G729AEncoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (pSamples!=NULL && m_pEncoder!=NULL && nSamples==G729A_SAMPLES_PER_FRAME)
	{
		unsigned char pEncoded[20];
		int nFrameLen=0;
		int nEncodeSize = nSamples;
		short * pEncSample = pSamples;
		unsigned char * pEncTmp = pEncoded;

		while( nEncodeSize >= G729A_ENCODER_SAMPLES )
		{
			va_g729a_encoder(m_pEncoder, pEncSample, (unsigned char*)pEncTmp);
			nEncodeSize -= G729A_ENCODER_SAMPLES;
			pEncSample += G729A_ENCODER_SAMPLES;
			pEncTmp += G729A_FRAME_SIZE;
			nFrameLen += G729A_FRAME_SIZE;
		}
		if (nFrameLen>0)
			m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded, nFrameLen,ulTimestamp);
		return 0;
	}
	return -1;
}

HPATC_G729ADecoder::HPATC_G729ADecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
,m_pDecoder(NULL)
,m_bInited(false)
{
}

HPATC_G729ADecoder::~HPATC_G729ADecoder(void)
{

}

int HPATC_G729ADecoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pDecoder == NULL )
		m_pDecoder = va_g729a_init_decoder();
	if (!m_pDecoder)
		return -1;

	m_pLeftSamples=(short*)malloc(G729A_SAMPLES_PER_FRAME*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_G729ADecoder::ReleaseConnections(void)
{
	if (m_pDecoder!=NULL)
	{
		va_g729a_free_decoder(m_pDecoder);
		m_pDecoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_G729ADecoder::InputStream(unsigned char*pEncoded, int nLen, bool bDiscontinious,unsigned long ulTimestamp)
{
	if (pEncoded!=NULL && nLen > 0)
	{
		int ret = 0;
		short pSamples[960] = {0};
		if (bDiscontinious)
		{
			bDiscontinious=false;
			short*pTempSamples=pSamples;
			unsigned char pTempEncoded[20]={0};
			va_g729a_decoder(m_pDecoder, pTempEncoded, (short*)pSamples,0);
			va_g729a_decoder(m_pDecoder, pTempEncoded + 10, (short*)pSamples + 80,0);

			m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,G729A_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp-10);
		}
		//else // liurui
		{
			va_g729a_decoder(m_pDecoder, (unsigned char*)pEncoded, (short*)pSamples,0);
			va_g729a_decoder(m_pDecoder, (unsigned char*)pEncoded + 10, (short*)pSamples + 80,0);
			short*pTempSamples=pSamples;
			m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,G729A_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp);
		}
		return 0;
	}

	return -1;
}

HPATCEncoder*CreateXG729Encoder(HPATCEncoderNotify&rHPATCEncoderNotify)
{
	return new HPATC_G729AEncoder(rHPATCEncoderNotify);
}



