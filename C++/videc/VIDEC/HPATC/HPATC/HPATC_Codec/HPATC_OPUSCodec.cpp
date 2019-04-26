// HPATC_OPUSCodec.cpp: implementation of the HPATC_OPUSCodec class.
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
#include "HPATC_OPUSCodec.h"


#define OPUS_FRAME_SIZE					240
#define OPUS_ENCODER_SAMPLES			960
#define OPUS_SAMPLES_PER_FRAME			480



HPATCDecoder*HPATCDecoder::CreateOPUSDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_OPUSDecoder(rHPATCDecoderNotify);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HPATC_OPUSEncoder::HPATC_OPUSEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
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

HPATC_OPUSEncoder::~HPATC_OPUSEncoder(void)
{

}

int HPATC_OPUSEncoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pEncoder == NULL )
		m_pEncoder= OpenOpusEncoder(1, nSampleRate, nBitrate, nSamplesPerFrame);
	if (!m_pEncoder)
		return -1;

	m_pLeftSamples=(short*)malloc(OPUS_ENCODER_SAMPLES*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_OPUSEncoder::ReleaseConnections(void)
{
	if (m_pEncoder!=NULL)
	{
		CloseOpusEncoder(m_pEncoder);
		m_pEncoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_OPUSEncoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (pSamples!=NULL && m_pEncoder!=NULL && nSamples==OPUS_SAMPLES_PER_FRAME)
	{
		unsigned char pEncoded[OPUS_FRAME_SIZE];
		int nFrameLen=0;

		if (m_nLeftSamples+nSamples>=OPUS_ENCODER_SAMPLES)
		{
			int nLeftSamples=OPUS_ENCODER_SAMPLES-m_nLeftSamples;
			memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nLeftSamples<<1);
			//nFrameLen = OPUSEncoderFrame(m_pEncoder, m_pLeftSamples, pEncoded);
			nFrameLen = OpusEncoderFrame(m_pEncoder, m_pLeftSamples, OPUS_ENCODER_SAMPLES,  pEncoded);

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

			return -1;
		}
		return 0;
	}
	return -1;
}

HPATC_OPUSDecoder::HPATC_OPUSDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)
,m_pDecoder(NULL)
,m_bInited(false)
{
}

HPATC_OPUSDecoder::~HPATC_OPUSDecoder(void)
{

}

int HPATC_OPUSDecoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);

	if( m_pDecoder == NULL )
		m_pDecoder = OpenOpusDecoder(1, nSampleRate);
	if (!m_pDecoder)
		return -1;

	m_pLeftSamples=(short*)malloc(OPUS_SAMPLES_PER_FRAME*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_OPUSDecoder::ReleaseConnections(void)
{
	if (m_pDecoder!=NULL)
	{
		CloseOpusDecoder(m_pDecoder);
		m_pDecoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_OPUSDecoder::InputStream(unsigned char*pEncoded, int nLen, bool bDiscontinious,unsigned long ulTimestamp)
{
	if (pEncoded!=NULL && nLen > 0)
	{
		int ret = 0;
		short pSamples[960] = {0};
		if (bDiscontinious)
		{
			bDiscontinious=false;
			int nTempSamples=OPUS_ENCODER_SAMPLES;
			short*pTempSamples=pSamples;
			unsigned char pTempEncoded[OPUS_FRAME_SIZE]={0};
			OpusFillInFrame(m_pDecoder, NULL, 60, pSamples);

			while (nTempSamples>=OPUS_SAMPLES_PER_FRAME)
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,OPUS_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp-10);
				pTempSamples+=OPUS_SAMPLES_PER_FRAME;
				nTempSamples-=OPUS_SAMPLES_PER_FRAME;
			}

			if (nTempSamples>0)
			{
				memcpy(m_pLeftSamples,pTempSamples,nTempSamples<<1);
				m_nLeftSamples=nTempSamples;
			}
		}
		//else //liurui
		{
			ret = OpusDecoderFrame(m_pDecoder, pEncoded, nLen , pSamples);
			int nTempSamples=OPUS_ENCODER_SAMPLES;
			short*pTempSamples=pSamples;
			while (nTempSamples>=OPUS_SAMPLES_PER_FRAME)
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,OPUS_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious,ulTimestamp);
				pTempSamples+=OPUS_SAMPLES_PER_FRAME;
				nTempSamples-=OPUS_SAMPLES_PER_FRAME;
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

HPATCEncoder*CreateHPATC_OPUSEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
{
	return new HPATC_OPUSEncoder(rHPATCEncoderNotify);
}

