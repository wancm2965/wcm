//HPMP_Codec.h
#ifndef __VIDEC_CODEC_H__
#define __VIDEC_CODEC_H__

#include <HPMP/HPMP_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <HPMP/HPMP.h>

HPMP_API void HPMP_SetMaxKeyFrameInterval(unsigned long ulMS);
HPMP_API unsigned long HPMP_GetMaxKeyFrameInterval(void);

//
class HPMP_EncoderCallback
{
public:
	HPMP_EncoderCallback(void){};
	virtual~HPMP_EncoderCallback(void){};
public:
	//输出AVCON的视频包
	virtual void OnHPMP_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen)=0;
};

class HPMP_Encoder
{
public:
	HPMP_Encoder(void){};
	virtual~HPMP_Encoder(void){};
public:
	virtual void Close(void)=0;
	virtual int EncodeFrame(void*pFrameData,int nFrameSize,int nVirtualWidth,int nVirtualHeight,bool bEventFieldFirst,bool bHardware,unsigned long nTimestamp)=0;
	virtual void RequestKeyFrame(void)=0;
};

HPMP_API HPMP_Encoder*HPMP_EncoderCreate(HPMP_EncoderCallback&rCallback,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType);

class HPMP_Decoder;
class HPMP_DecoderCallback
{
public:
	HPMP_DecoderCallback(void){};
	virtual~HPMP_DecoderCallback(void){};
public:
	virtual void OnHPMP_DecoderCallbackFrame(HPMP_Decoder*pDecoderFrame,HPMP_Image*pImage)=0;
};

class HPMP_Decoder
{
public:
	HPMP_Decoder(void){};
	virtual~HPMP_Decoder(void){};
public:
	virtual void Close(void)=0;
	virtual int DecodeFramePacket(void*pPacketData,int nPacketSize)=0;
};

HPMP_API HPMP_Decoder*HPMP_DecoderCreate(HPMP_DecoderCallback&rCallback,VIDEC_CODEC_TYPE codecType,int nWidth,int nHeight);



#endif