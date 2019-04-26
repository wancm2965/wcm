//HPMP_Encoder.h
#ifndef __HPMP_ENCODER_H__
#define __HPMP_ENCODER_H__

#include <HPMP/HPMP_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <HPMP/HPMP.h>


class HPMP_EncoderStreamCallback
{
public:
	HPMP_EncoderStreamCallback(void){};
	virtual~HPMP_EncoderStreamCallback(void){};
public:
	//输出AVCON的视频包
	virtual void OnHPMP_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
	virtual void OnHPMP_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
	virtual void OnHPMP_EncoderStreamCallbackPreEncode(int&nSkipFlag){nSkipFlag=0;};
};

class HPMP_EncoderStream
{
public:
	HPMP_EncoderStream(void){};
	virtual~HPMP_EncoderStream(void){};
public:
	virtual void Close(void)=0;
	virtual int EncodeFrame(HPMP_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)=0;
	virtual void RequestKeyFrame(void)=0;
};

HPMP_API HPMP_EncoderStream*HPMP_EncoderStreamCreate(HPMP_EncoderStreamCallback&rCallback,bool bRTPPacket);

#endif
