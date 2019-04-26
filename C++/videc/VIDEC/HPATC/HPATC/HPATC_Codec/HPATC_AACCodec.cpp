// HPATC_AACCodec.cpp: implementation of the HPATC_AACCodec class.
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

#include "HPATC_AACCodec.h"


#define AAC_FRAME_SIZE				768
#define AAC_ENCODER_SAMPLES			1024
#define AAC_SAMPLES_PER_FRAME		480

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HPATCEncoder*HPATCEncoder::CreateAACEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
{
	return new HPATC_AACEncoder(rHPATCEncoderNotify);
}

HPATC_AACEncoder::HPATC_AACEncoder(HPATCEncoderNotify&rHPATCEncoderNotify)
:m_rHPATCEncoderNotify(rHPATCEncoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)

,m_hEncoder(NULL)
,m_pEncodedBuf(NULL)
{
	m_pLeftSamples=NULL;
	m_nLeftSamples=0;
}

HPATC_AACEncoder::~HPATC_AACEncoder(void)
{

}

int HPATC_AACEncoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);
	//if (m_nSampleRate!=16000 ||
	//	m_nBitrate!=32000)
	//{
	//	return -1;
	//}

	m_hEncoder = faacEncOpen(m_nSampleRate, 1, &m_nSamplesPerFrame, &m_nBytesPerFrame);
	if (m_hEncoder==NULL)
		return -1;

	m_pEncodedBuf=(unsigned char*)malloc(m_nBytesPerFrame);
	if (m_pEncodedBuf==NULL)
		return -1;

	faacEncConfigurationPtr encoderConfig = faacEncGetCurrentConfiguration(m_hEncoder);

	//encoderConfig->mpegVersion = MPEG4;
	//encoderConfig->aacObjectType = LOW;
	//encoderConfig->allowMidside = false;
	//encoderConfig->useLfe = false;
	//encoderConfig->useTns = false;
	//encoderConfig->inputFormat = FAAC_INPUT_16BIT;
	//encoderConfig->outputFormat = 0;    // raw
	//encoderConfig->quantqual = 0;    // use abr
	//encoderConfig->bitRate = m_nBitrate;

	encoderConfig->mpegVersion = MPEG4;
	encoderConfig->aacObjectType=LOW;
	encoderConfig->inputFormat = FAAC_INPUT_16BIT;
	encoderConfig->outputFormat=0;//1;
	encoderConfig->useTns=false;
	encoderConfig->useLfe=false;
	encoderConfig->shortctl=SHORTCTL_NORMAL;
	encoderConfig->quantqual=100;
	encoderConfig->bandWidth=0;
	encoderConfig->bitRate=0;

	faacEncSetConfiguration(m_hEncoder, encoderConfig);

	m_pLeftSamples=(short*)malloc(AAC_ENCODER_SAMPLES*sizeof(short));
	m_nLeftSamples=0;

	//unsigned char *ppBuffer = new unsigned char[10];
	//unsigned long pSizeOfDecoderSpecificInfo = 0;
	//faacEncGetDecoderSpecificInfo(m_hEncoder,&ppBuffer ,&pSizeOfDecoderSpecificInfo);
	//delete []ppBuffer;

	return 0;
}

void HPATC_AACEncoder::ReleaseConnections(void)
{
	if (m_hEncoder!=NULL)
	{
		faacEncClose(m_hEncoder);
		m_hEncoder=NULL;
	}

	if (m_pEncodedBuf!=NULL)
	{
		free(m_pEncodedBuf);
		m_pEncodedBuf=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_AACEncoder::InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)
{
	if (pSamples!=NULL && m_hEncoder!=NULL /*&& nSamples==AAC_SAMPLES_PER_FRAME*/)
	{
		//	unsigned char pEncoded[2048];
		//int nFrameLen=faacEncEncode(m_hEncoder, (int*)pSamples, nSamples, m_pEncodedBuf, m_nBytesPerFrame);

		//	if (nFrameLen>0)
		//		m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(m_pEncodedBuf,nFrameLen);

		unsigned char pEncoded[AAC_FRAME_SIZE];
		int nFrameLen=0;

		if (m_nLeftSamples+nSamples>=AAC_ENCODER_SAMPLES)
		{
			int nLeftSamples=AAC_ENCODER_SAMPLES-m_nLeftSamples;
			memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nLeftSamples<<1);

			nFrameLen=faacEncEncode(m_hEncoder, (int*)m_pLeftSamples, AAC_ENCODER_SAMPLES, pEncoded, m_nBytesPerFrame);
			if (nFrameLen>0)
				m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded,nFrameLen,ulTimestamp);

			m_nLeftSamples=nSamples-nLeftSamples;
			if (m_nLeftSamples>0)
			{
				if(m_nLeftSamples > AAC_ENCODER_SAMPLES)
				{
					memcpy(m_pLeftSamples,pSamples+nLeftSamples,AAC_ENCODER_SAMPLES<<1);
					nFrameLen=faacEncEncode(m_hEncoder, (int*)m_pLeftSamples, AAC_ENCODER_SAMPLES, pEncoded, m_nBytesPerFrame);
					if (nFrameLen>0)
						m_rHPATCEncoderNotify.OnHPATCEncoderNotifyOutput(pEncoded,nFrameLen,ulTimestamp+5);

					m_nLeftSamples-=AAC_ENCODER_SAMPLES;
					nLeftSamples+=AAC_ENCODER_SAMPLES;
				}
				memcpy(m_pLeftSamples,pSamples+nLeftSamples,m_nLeftSamples<<1);			
			}
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
/*
HPATC_AACDecoder::HPATC_AACDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
:m_rHPATCDecoderNotify(rHPATCDecoderNotify)
,m_nSampleRate(0)
,m_nSamplesPerFrame(0)
,m_nBitrate(0)
,m_nBytesPerFrame(0)

,m_hDecoder(NULL)
,m_bInited(false)
{
	m_pLeftSamples=NULL;
	m_nLeftSamples=0;
}

HPATC_AACDecoder::~HPATC_AACDecoder(void)
{

}

int HPATC_AACDecoder::Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam)
{
	m_nSampleRate=nSampleRate;
	m_nSamplesPerFrame=nSamplesPerFrame;
	m_nBitrate=nBitrate;
	m_nBytesPerFrame=(m_nSamplesPerFrame<<1);
	if (m_nSampleRate!=16000 ||
		m_nBitrate!=32000)
	{
		//return -1;
	}

	m_hDecoder=faacDecOpen();
	if (m_hDecoder==NULL)
		return -1;


	faacDecConfigurationPtr config=faacDecGetCurrentConfiguration(m_hDecoder);
	
    config->defObjectType=LC;
    config->defSampleRate=m_nSampleRate;
    config->outputFormat=FAAD_FMT_16BIT;
	faacDecSetConfiguration(m_hDecoder,config);

	m_pLeftSamples=(short*)malloc(AAC_SAMPLES_PER_FRAME*sizeof(short));
	m_nLeftSamples=0;

	return 0;
}

void HPATC_AACDecoder::ReleaseConnections(void)
{
	if (m_hDecoder!=NULL)
	{
		faacDecClose(m_hDecoder);
		m_hDecoder=NULL;
	}

	if (m_pLeftSamples)
	{
		free(m_pLeftSamples);
		m_pLeftSamples=NULL;
	}
}

int HPATC_AACDecoder::InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious)
{
	if (pEncoded!=NULL && nLen > 0)
	{
		if (!m_bInited)
		{
			unsigned long ulSampleRate=0;
			unsigned char nChannels=0;
			faacDecInit(m_hDecoder,pEncoded,nLen,&ulSampleRate,&nChannels);
			m_nSampleRate=ulSampleRate;
			m_bInited=true;
		}
		short*pSamples=(short*)faacDecDecode(m_hDecoder,&m_decFrameInfo,pEncoded,nLen);

		if (pSamples!=NULL && m_decFrameInfo.error==0 && m_decFrameInfo.samples > 0)
		{
			// {{ liurui : stereo convert to mono : 2011/06/21
			if(m_decFrameInfo.channels == 2 )
			{
				int i;
				int data_int32 = 0;
				short monoBuffer[1024] = {0};
				for (i = 0; i < 1024; i++)
				{
					data_int32 =  (pSamples[i*2] + pSamples[i*2 + 1])>>1;
					if (data_int32 > 32767) {
						data_int32 = 32767;
					} else if (data_int32 < -32768) {
						data_int32 = -32768;
					}
					monoBuffer[i] = static_cast<short>(data_int32);
				}
				memcpy(pSamples, monoBuffer, 2048);
				m_decFrameInfo.samples = m_decFrameInfo.samples>>1;
			}
			// }} liurui
			//m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pSamples,m_decFrameInfo.samples,m_nSampleRate,bDiscontinious);
			//return 0;

			int nSamples = m_decFrameInfo.samples;
			short*pTempSamples=pSamples;
			int nTempSamples=nSamples;
			if (m_nLeftSamples)
			{
				if (m_nLeftSamples+nSamples>=AAC_SAMPLES_PER_FRAME)
				{
					int nLeftSamples=AAC_SAMPLES_PER_FRAME-m_nLeftSamples;
					memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nLeftSamples<<1);
					m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(m_pLeftSamples,AAC_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious);
					m_nLeftSamples=0;
					pTempSamples+=nLeftSamples;
					nTempSamples-=nLeftSamples;
				}
				else
				{
					memcpy(m_pLeftSamples+m_nLeftSamples,pSamples,nSamples<<1);
					m_nLeftSamples+=nSamples;
					return 0;
				}
			}

			while (nTempSamples>=AAC_SAMPLES_PER_FRAME)
			{
				m_rHPATCDecoderNotify.OnHPATCDecoderNotifyOutput(pTempSamples,AAC_SAMPLES_PER_FRAME,m_nSampleRate,bDiscontinious);
				pTempSamples+=AAC_SAMPLES_PER_FRAME;
				nTempSamples-=AAC_SAMPLES_PER_FRAME;
			}

			if (nTempSamples>0)
			{
				memcpy(m_pLeftSamples,pTempSamples,nTempSamples<<1);
				m_nLeftSamples=nTempSamples;
			}
		}
	}

	return -1;
}



HPATCDecoder*CreateHPATC_AACDecoder(HPATCDecoderNotify&rHPATCDecoderNotify)
{
	return new HPATC_AACDecoder(rHPATCDecoderNotify);
}
*/
