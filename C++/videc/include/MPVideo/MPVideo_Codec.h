//MPVIDEO_Codec.h
#ifndef __VIDEC_CODEC_H__
#define __VIDEC_CODEC_H__

#include <MPVideo/MPVideo_Export.h>
#include <VIDEC/VIDEC_CodecDef.h>
#include <MPVideo/MPVideo.h>

MPVIDEO_API void MPVideo_SetMaxKeyFrameInterval(unsigned long ulMS);
MPVIDEO_API unsigned long MPVIDEO_GetMaxKeyFrameInterval(void);

//
class MPVideo_EncoderCallback
{
public:
	MPVideo_EncoderCallback(void){};
	virtual~MPVideo_EncoderCallback(void){};
public:
	//输出AVCON的视频包
	virtual void OnMPVideo_EncoderCallbackFramePacket(void*pPacketData,int nPacketLen)=0;
	virtual void OnMPVideo_EncoderCallbackRTPPacket(void*pPacketData,int nPacketSize)=0;
};

class MPVideo_Encoder
{
public:
	MPVideo_Encoder(void){};
	virtual~MPVideo_Encoder(void){};
public:
	virtual void Close(void)=0;
	virtual int EncodeFrame(void*pFrameData,int nFrameSize,int nVirtualWidth,int nVirtualHeight,bool bEventFieldFirst,bool bHardware,unsigned long nTimestamp)=0;
	virtual void RequestKeyFrame(void)=0;
	virtual bool DoRTPPacket(void)=0;
};

MPVIDEO_API MPVideo_Encoder*MPVideo_EncoderCreate(MPVideo_EncoderCallback&rCallback,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,bool bRTPPacket);

class MPVideo_Decoder;
class MPVideo_DecoderCallback
{
public:
	MPVideo_DecoderCallback(void){};
	virtual~MPVideo_DecoderCallback(void){};
public:
	virtual void OnMPVideo_DecoderCallbackFrame(MPVideo_Decoder*pDecoderFrame,MPVideo_Image*pImage)=0;
	virtual void OnMPVideo_DecoderCallbackFramePacket(MPVideo_Decoder*pDecoderFrame,void*pPacketData,int nPacketLen,int nWidth,int nHeight){};
};

class MPVideo_Decoder
{
public:
	MPVideo_Decoder(void){};
	virtual~MPVideo_Decoder(void){};
public:
	virtual void Close(void)=0;
	virtual int DecodeFramePacket(void*pPacketData,int nPacketSize)=0;
	virtual int DecodeRTPPacket(void*pPacketData,int nPacketSize)=0;
};

MPVIDEO_API MPVideo_Decoder*MPVideo_DecoderCreate(MPVideo_DecoderCallback&rCallback,VIDEC_CODEC_TYPE codecType,int nWidth,int nHeight);



#endif