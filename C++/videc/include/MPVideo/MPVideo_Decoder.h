//MPVideo_Decoder.h
#ifndef __MPVIDEO_DECODER_H__
#define __MPVIDEO_DECODER_H__

#include <MPVideo/MPVideo_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <MPVideo/MPVideo.h>


//自动识别视频大小，帧率和编码
class MPVideo_DecoderStreamCallback
{
public:
	MPVideo_DecoderStreamCallback(void){};
	virtual~MPVideo_DecoderStreamCallback(void){};
public:
	virtual void OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage)=0;
	virtual void OnMPVideo_DecoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight){};
};

class MPVideo_DecoderStream
{
public:
	MPVideo_DecoderStream(void){};
	virtual~MPVideo_DecoderStream(void){};
public:
	virtual void Close(void)=0;
	virtual int DecodeFramePacket(void*pPacketData,int nPacketSize)=0;
	virtual int DecodeRTPPacket(void*pPacketData,int nPacketSize)=0;
};

MPVIDEO_API MPVideo_DecoderStream*MPVideo_DecoderStreamCreate(MPVideo_DecoderStreamCallback&rCallback);
MPVIDEO_API MPVideo_DecoderStream*MPVideo_DecoderStreamCreate(MPVideo_DecoderStreamCallback&rCallback,int nWidth,int nHeight,VIDEC_CODEC_TYPE CodecType);


#endif


