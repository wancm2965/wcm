//XPlayChanRTP.h
#ifndef __XPLAYCHANRTP_H__
#define __XPLAYCHANRTP_H__

#include <MPVideo/MPVideo.h>
#include "XImageSource.h"
#include <MPVideo/MPVideo_Codec.h>
#include <MPVideo/MPVideo_Decoder.h>
#include "XStreamSource.h"


class XPlayChanRTP
	: public MPVideo_PlayChanRTP
	, public XImageSource
	, public MPVideo_DecoderStreamCallback
	, public XStreamSource
{
public:
	XPlayChanRTP(void);
	virtual~XPlayChanRTP(void);
public:
	int Open(int nPayloadType,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	
	void OnRecvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);

	int AddImageSink(MPVideo_ImageSink*pImageSink);
	void RemoveImageSink(MPVideo_ImageSink*pImageSink);
	
	void OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage);
	void OnMPVideo_DecoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);

	int AddStreamSink(MPVideo_StreamSink*pStreamSink);
	void RemoveStreamSink(MPVideo_StreamSink*pStreamSink);
protected:
	int						m_nPayloadType;
	MPVideo_DecoderStream*	m_pDecoderStream;
};

#endif

