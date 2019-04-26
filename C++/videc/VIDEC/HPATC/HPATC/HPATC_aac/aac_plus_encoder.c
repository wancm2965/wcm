#include "aac_plus.h"
#include "aacenc.h"
#include "sbr_main.h"


void* aac_plus_init_encoder()
{
	int nError=0;
	struct AAC_ENCODER*pAACEncoder = 0;

	AACENC_CONFIG config;
	AacInitDefaultConfig(&config);
	config.bitRate=AAC_BITRATE;
	config.nChannelsIn=1;
	config.nChannelsOut=1;
	config.bandWidth=0;
	config.sampleRate=AAC_SAMPLE_RATE;

	nError= AacEncOpen(&pAACEncoder,config);
	if (nError)
	{
		AacEncClose(pAACEncoder);
		pAACEncoder=0;
	}


	return pAACEncoder;
}

void aac_plus_encoder(void*handle,short *speech, unsigned char *bitstream,int*numOutBytes)
{
	struct AAC_ENCODER*pAACEncoder = (struct AAC_ENCODER*)handle;
	unsigned char ancBytes[MAX_PAYLOAD_SIZE];
	unsigned int numAncBytes=MAX_PAYLOAD_SIZE;
	unsigned int* outBytes=(unsigned int*)bitstream;

	int i;
	float inBuf[AAC_SAMPLES_FER_FRAME];
	for (i=0;i<AAC_SAMPLES_FER_FRAME;i++)
	{
		inBuf[i]=(float)speech[i];
	}


	if (0!=AacEncEncode(pAACEncoder,inBuf,1,ancBytes,&numAncBytes,outBytes,numOutBytes))
	{
		*numOutBytes=0;
	}
}

void aac_plus_free_encoder(void*handle)
{
	struct AAC_ENCODER*pAACEncoder = (struct AAC_ENCODER*)handle;
	if (pAACEncoder)
	{
		AacEncClose(pAACEncoder);
		pAACEncoder=0;
	}
}
