//XCapChan.h
#ifndef __XCAPCHAN_H__
#define __XCAPCHAN_H__

#include <MPVideo/MPVideo.h>
#include "XImageSink.h"
#include <MPVideo/MPVideo_Encoder.h>
#include "XStreamSource.h"
#include "MPVIDEO_convert.h"

class XCapChanRTP
	: public MPVideo_CapChanRTP
	, public XImageSink
	, public MPVideo_EncoderStreamCallback
	, public XStreamSource
{
public:
	XCapChanRTP(MPVideo_CapChanRTPCallback&rCallback);
	virtual~XCapChanRTP(void);
public:
	int Open(int nPayloadType,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	//int Reset(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType);
	int RequestKeyFrame(void);
	
	void OnAttachedToImageSource(MPVideo_ImageSource*pImageSource);
	void OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource);
	void OnReceivedImage(MPVideo_Image*pImage);

	void OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	
	int AddStreamSink(MPVideo_StreamSink*pStreamSink);
	void RemoveStreamSink(MPVideo_StreamSink*pStreamSink);
protected:
	MPVideo_CapChanRTPCallback&	m_rCallback;
	MPVideo_EncoderStream*	m_pEncoderStream;

	int						m_nFrameRate;

	VIDEC_CODEC_TYPE		m_CodecType;
	int						m_nBitrate;
	int						m_nQMin;
	int						m_nQMax;
	int						m_nBitrateControlType;

	int						m_nWidth;
	int						m_nHeight;

	MPVideo_Resize*			m_pResize;
	int						m_nDstBufLen;
	unsigned char*			m_pDstBuf;
	unsigned char*			m_dst[3];

	int						m_nPayloadType;
};


#endif
