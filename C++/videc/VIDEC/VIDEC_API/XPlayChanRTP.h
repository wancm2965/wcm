//XPlayChanRTP.h
#ifndef __XPLAYCHANRTP_H__
#define __XPLAYCHANRTP_H__

#include "VIDEC.h"
#include "XImageSource.h"
#include "VIDEC_Codec.h"
#include "VIDEC_Decoder.h"
#include "XStreamSource.h"


class XPlayChanRTP
	: public VIDEC_PlayChanRTP
	, public XImageSource
	, public VIDEC_DecoderStreamCallback
	, public XStreamSource
{
public:
	XPlayChanRTP(void);
	virtual~XPlayChanRTP(void);
public:
	int Open(int nPayloadType,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	
	void OnRecvdRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);

	int AddImageSink(VIDEC_ImageSink*pImageSink);
	void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	void OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage);
	void OnVIDEC_DecoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);

	int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
protected:
	int						m_nPayloadType;
	VIDEC_DecoderStream*	m_pDecoderStream;
};

#endif

