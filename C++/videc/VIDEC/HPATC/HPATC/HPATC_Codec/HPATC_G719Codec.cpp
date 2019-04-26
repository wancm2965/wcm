// HPATC_G719Codec.cpp: implementation of the HPATC_G719Codec class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef WIN32
#include <windows.h>
#else
#include <memory.h>
#endif

#include "HPATC_G719Codec.h"


#define G719_FRAME_SIZE					240
#define G719_ENCODER_SAMPLES			960
#define G719_SAMPLES_PER_FRAME			960//480


HPATCDecoder*HPATCDecoder::CreateG719Decoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_G719Decoder(rHPATCDecoderNotify);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HPATC_G719Encoder::HPATC_G719Encoder(HPATCEncoderNotify&rHPATCEncoderNotify)
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

HPATC_G719Encoder::~HPATC_G719Encoder(void)
{

}

int HPATC_G719Encoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pEncoder == NULL )
		m_pEncoder = OpenG719Encoder(nBitrate, nSamplesPerFrame);
	if (!m_pEncoder)
		return -1;

	m_pLeftSamples=(short*)malloc(G719_ENCODER_SAMPLES*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_G719Encoder::ReleaseConnections(void)
{
	if (m_pEncoder!=NULL)
	{
		CloseG719Encoder(m_pEncoder);
		m_pEncoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_G719Encoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (pSamples!=NULL && m_pEncoder!=NULL ) //&& nSamples==G719_SAMPLES_PER_FRAME)
	{
		unsigned char pEncoded[G719_FRAME_SIZE];
		int nFrameLen=0;

		if (m_nLeftSamples+nSamples>=G719_ENCODER_SAMPLES)
		{
			int nLeftSamples=G719_ENCODER_SAMPLES-m_nLeftSamples;
			memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nLeftSamples<<1);
			nFrameLen = G719EncoderFrame(m_pEncoder, m_pLeftSamples, pEncoded);

			if (nFrameLen>0)
				m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded, nFrameLen,ulTimestamp);

			m_nLeftSamples=nSamples-nLeftSamples;
			if (m_nLeftSamples>0)
				memcpy(m_pLeftSamples,pSamples+nLeftSamples,m_nLeftSamples<<1);
		}
		else
		{
			memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nSamples<<1);
			m_nLeftSamples+=nSamples;
			return -1; //liurui
		}
		return 0;
	}
	return -1;
}

HPATC_G719Decoder::HPATC_G719Decoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
,m_pDecoder(NULL)
,m_bInited(false)
{
}

HPATC_G719Decoder::~HPATC_G719Decoder(void)
{

}

int HPATC_G719Decoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pDecoder == NULL )
		m_pDecoder = OpenG719Decoder(nBitrate, nSamplesPerFrame);
	if (!m_pDecoder)
		return -1;

	m_pLeftSamples=(short*)malloc(G719_SAMPLES_PER_FRAME*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_G719Decoder::ReleaseConnections(void)
{
	if (m_pDecoder!=NULL)
	{
		CloseG719Decoder(m_pDecoder);
		m_pDecoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_G719Decoder::InputStream(unsigned char*pEncoded, int nLen, bool bDiscontinious,unsigned long ulTimestamp)
{
	if (pEncoded!=NULL && nLen > 0)
	{
		int ret = 0;
		short pSamples[960] = {0};
		if (bDiscontinious)
		{
			bDiscontinious=false;
			int nTempSamples=G719_ENCODER_SAMPLES;
			short*pTempSamples=pSamples;
			unsigned char pTempEncoded[G719_FRAME_SIZE]={0};
			ret = G719DecoderFrame(m_pDecoder, pTempEncoded, pSamples, 1);

			while (nTempSamples>=G719_SAMPLES_PER_FRAME)
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,G719_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp-10);
				pTempSamples+=G719_SAMPLES_PER_FRAME;
				nTempSamples-=G719_SAMPLES_PER_FRAME;
			}

			if (nTempSamples>0)
			{
				memcpy(m_pLeftSamples,pTempSamples,nTempSamples<<1);
				m_nLeftSamples=nTempSamples;
			}
		}
		//else // liurui
		{
			ret = G719DecoderFrame(m_pDecoder, pEncoded, pSamples, 0);
			int nTempSamples=G719_ENCODER_SAMPLES;
			short*pTempSamples=pSamples;
			while (nTempSamples>=G719_SAMPLES_PER_FRAME)
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,G719_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp);
				pTempSamples+=G719_SAMPLES_PER_FRAME;
				nTempSamples-=G719_SAMPLES_PER_FRAME;
			}
			if (nTempSamples>0)
			{
				memcpy(m_pLeftSamples,pTempSamples,nTempSamples<<1);
				m_nLeftSamples=nTempSamples;
			}
		}
		return 0;
	}

	return -1;
}

HPATCEncoder*CreateHPATC_G719Encoder(HPATCEncoderNotify&rHPATCEncoderNotify)
{
	return new HPATC_G719Encoder(rHPATCEncoderNotify);
}

