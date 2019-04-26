//HPATCCodec.h
#ifndef __HPATC_CODEC_H__
#define __HPATC_CODEC_H__

#include "HPATC_Export.h"

typedef struct{
	int		id;
	int		pl_type;
	char	pl_name[32];
	int		pl_freq;
	int		framesize;
	int		bitrate;
	char	desc[128];//g711u 64kbps
}HPATC_CodecInst;

HPATC_API bool HPATCGetCodecInstByID(int nID,HPATC_CodecInst*pCodecInst);

class HPATC_API HPATCResample
{
public:
	HPATCResample(void){};
	virtual~HPATCResample(void){};
public:
	static HPATCResample*Create(void);

	virtual int Connect(int nInSampleRate,int nOutSampleRate)=0;
	virtual void ReleaseConnections(void)=0;
	virtual int Convert(short*pInSamples,int nInSamples,short*pOutSamples,int&nOutSamples)=0;
};


class HPATCEncoderNotify
{
public:
	HPATCEncoderNotify(void){};
	virtual~HPATCEncoderNotify(void){};
public:
	virtual void OnHPATCEncoderNotifyOutput(unsigned char*pEncoded,int nLen,unsigned long ulTimestamp)=0;
};

class HPATC_API HPATCEncoder
{
public:
	HPATCEncoder(void){};
	virtual~HPATCEncoder(void){};
public:
	static HPATCEncoder*CreateAACEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);

	virtual int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0)=0;
	virtual void ReleaseConnections(void)=0;
	virtual int InputSamples(short*pSamples,int nSamples,unsigned long ulTimestamp)=0;
};

class HPATCDecoderNotify
{
public:
	HPATCDecoderNotify(void){};
	virtual~HPATCDecoderNotify(void){};
public:
	virtual void OnHPATCDecoderNotifyOutput(short*pSamples,int nSamples,int nSampleRate,bool bDiscontinious,unsigned long ulTimestamp)=0;
};

class HPATC_API HPATCDecoder
{
public:
	HPATCDecoder(void){};
	virtual~HPATCDecoder(void){};
public:
	static HPATCDecoder*CreateG7221Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateG729Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateGSMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateG711UDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateG711ADecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateG719Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
	static HPATCDecoder*CreateOPUSDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);

	virtual int Connect(int nSampleRate,int nSamplesPerFrame,int nBitrate,int nParam=0)=0;
	virtual void ReleaseConnections(void)=0;
	virtual int InputStream(unsigned char*pEncoded,int nLen,bool bDiscontinious,unsigned long ulTimestamp)=0;
};

//HPATCEncoder*CreateHPATC_G711UEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCEncoder*CreateHPATC_G711AEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateHPATC_G711UDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//HPATCDecoder*CreateHPATC_G711ADecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//
//HPATCEncoder*CreateHPATC_GSMEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateHPATC_GSMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXG7231Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXG7231Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXG729Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//;
//
//HPATCEncoder*CreateXL16Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXL16Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXILBCEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXILBCDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateSirenG7221Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateSirenG7221Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);


//HPATCEncoder*CreateXAMRNBEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCEncoder*CreateXAMRWBEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXAMRNBDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//HPATCDecoder*CreateXAMRWBDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXHiKG722Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXHiKG726Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//HPATCDecoder*CreateXHiKG722Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXMP3Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXMP3Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//
//HPATCDecoder*CreateHPATC_AACDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateXG722Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXG722Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//// 2009/02/13 add by ryan liu 
//HPATCEncoder*CreateXEVRCEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateXEVRCDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateSpeexEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateSpeexDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateADPCMEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateADPCMDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateHPATC_G719Encoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateHPATC_G719Decoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//HPATCEncoder*CreateHPATC_OPUSEncoder(HPATCEncoderNotify&rHPATCEncoderNotify);
//HPATCDecoder*CreateHPATC_OPUSDecoder(HPATCDecoderNotify&rHPATCDecoderNotify);
//
//int AUDEC_codec_init(void);
//void AUDEC_codec_terminate(void);


#endif