//MPVideo_Encoder.h
#ifndef __MPVIDEO_ENCODER_H__
#define __MPVIDEO_ENCODER_H__

#include <MPVideo/MPVideo_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <MPVideo/MPVideo.h>


class MPVideo_EncoderStreamCallback
{
public:
	MPVideo_EncoderStreamCallback(void){};
	virtual~MPVideo_EncoderStreamCallback(void){};
public:
	//输出AVCON的视频包
	virtual void OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
	virtual void OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)=0;
	virtual void OnMPVideo_EncoderStreamCallbackPreEncode(int&nSkipFlag){nSkipFlag=0;};
};

class MPVideo_EncoderStream
{
public:
	MPVideo_EncoderStream(void){};
	virtual~MPVideo_EncoderStream(void){};
public:
	virtual void Close(void)=0;
	virtual int EncodeFrame(MPVideo_Image*pImage,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,VIDEC_CODEC_TYPE codecType)=0;
	virtual void RequestKeyFrame(void)=0;
};

MPVIDEO_API MPVideo_EncoderStream*MPVideo_EncoderStreamCreate(MPVideo_EncoderStreamCallback&rCallback,bool bRTPPacket);

#endif
