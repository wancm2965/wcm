#include "aac_plus.h"
#include "aacdecoder.h"
#include "FFR_bitbuffer.h"
#include <stdlib.h>

#define INPUT_BUF_SIZE	(6144*2/8)

struct AAC_PLUS_DECODER
{
	float			TimeDataFloat[4*AAC_SAMPLES_FER_FRAME];
	unsigned int	inBuf[INPUT_BUF_SIZE/(sizeof(int))];
	struct BIT_BUF	bitBuf;
	HANDLE_BIT_BUF	hBitBuf;
	AACDECODER		aacDecoder;
	SBRBITSTREAM	streamSBR;

};

void* aac_plus_init_decoder()
{
	int i=0;
	struct AAC_PLUS_DECODER* pAACDecoder=(struct AAC_PLUS_DECODER*)malloc(sizeof(struct AAC_PLUS_DECODER)); 
	if (pAACDecoder)
	{
		for (i=0;i<4*AAC_SAMPLES_FER_FRAME;i++)
		{
			pAACDecoder->TimeDataFloat[i]=0.0;
		}
		pAACDecoder->hBitBuf=CreateBitBuffer(&pAACDecoder->bitBuf,(unsigned char*)pAACDecoder->inBuf,INPUT_BUF_SIZE);
		pAACDecoder->aacDecoder=CAacDecoderOpen(pAACDecoder->hBitBuf,&pAACDecoder->streamSBR,pAACDecoder->TimeDataFloat);
		if (pAACDecoder->aacDecoder==0)
		{
			aac_plus_free_decoder(pAACDecoder);
			pAACDecoder=0;
		}
		else
		{
			if (0!=CAacDecoderInit(pAACDecoder->aacDecoder,AAC_SAMPLE_RATE,AAC_BITRATE))
			{
				aac_plus_free_decoder(pAACDecoder);
				pAACDecoder=0;
			}
		}
	}

	return pAACDecoder;
}

void aac_plus_decoder(void*handle,unsigned char *bitstream,int numBytes,short *synth_short,int*numSamples)
{
	struct AAC_PLUS_DECODER* pAACDecoder=(struct AAC_PLUS_DECODER*)handle;
	int i;
	int frameSize=0;
	int sampleRate=0;
	int numChannels=0;
	char channelMode=0;
	for (i=0;i<numBytes;i++)
	{
		WriteBits(pAACDecoder->hBitBuf,bitstream[i],8);
	}


	if (0==CAacDecoder_DecodeFrame(pAACDecoder->aacDecoder,&frameSize,&sampleRate,&numChannels,&channelMode,1))
	{
		*numSamples=frameSize*numChannels;
		for (i=0;i<*numSamples;i++)
		{
			if (pAACDecoder->TimeDataFloat[i]<-32768.0)
			{
				pAACDecoder->TimeDataFloat[i]=-32768.0;
			}
			else if (pAACDecoder->TimeDataFloat[i]>32767.0)
			{
				pAACDecoder->TimeDataFloat[i]=32767.0;
			}

			synth_short[i]=(short)pAACDecoder->TimeDataFloat[i];
		}
	}
	else
	{
		*numSamples=0;
	}
}

void aac_plus_free_decoder(void*handle)
{
	struct AAC_PLUS_DECODER* pAACDecoder=(struct AAC_PLUS_DECODER*)handle;

	if (pAACDecoder->aacDecoder)
	{
		CAacDecoderClose(pAACDecoder->aacDecoder);
	}

	free(pAACDecoder);
	pAACDecoder=0;
}

