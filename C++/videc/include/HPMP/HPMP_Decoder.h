//HPMP_Decoder.h
#ifndef __HPMP_DECODER_H__
#define __HPMP_DECODER_H__

#include <HPMP/HPMP_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <HPMP/HPMP.h>


//自动识别视频大小，帧率和编码
class HPMP_DecoderStreamCallback
{
public:
	HPMP_DecoderStreamCallback(void){};
	virtual~HPMP_DecoderStreamCallback(void){};
public:
	virtual void OnHPMP_DecoderStreamCallbackFrame(HPMP_Image*pImage)=0;
};

class HPMP_DecoderStream
{
public:
	HPMP_DecoderStream(void){};
	virtual~HPMP_DecoderStream(void){};
public:
	virtual void Close(void)=0;
	virtual int DecodeFramePacket(void*pPacketData,int nPacketSize)=0;
};

HPMP_API HPMP_DecoderStream*HPMP_DecoderStreamCreate(HPMP_DecoderStreamCallback&rCallback);
HPMP_API HPMP_DecoderStream*HPMP_DecoderStreamCreate(HPMP_DecoderStreamCallback&rCallback,int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType);


#endif


