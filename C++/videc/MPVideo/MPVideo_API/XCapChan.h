//XCapChan.h
#ifndef __XCAPCHAN_H__
#define __XCAPCHAN_H__

#include <MPVideo/MPVideo.h>
#include "XImageSink.h"
#include <MPVideo/MPVideo_Encoder.h>
#include "XStreamSource.h"
#include "MPVIDEO_convert.h"

class XCapChan
	: public MPVideo_CapChan
	, public XImageSink
	, public MPVideo_EncoderStreamCallback
	, public XStreamSource
{
public:
	XCapChan(MPVideo_CapChanCallback&rCallback);
	virtual~XCapChan(void);
public:
	int Open(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight);
	void Close(void);
	int Reset(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight);
	int RequestKeyFrame(void);
	int GetBitrate(void);
	int SetSkipFrame(int nSkipFrameCount);
	
	void OnAttachedToImageSource(MPVideo_ImageSource*pImageSource);
	void OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource);
	void OnReceivedImage(MPVideo_Image*pImage);

	void OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnMPVideo_EncoderStreamCallbackPreEncode(int&nSkipFlag);

	int AddStreamSink(MPVideo_StreamSink*pStreamSink);
	void RemoveStreamSink(MPVideo_StreamSink*pStreamSink);
protected:
	MPVideo_CapChanCallback&	m_rCallback;
	MPVideo_EncoderStream*	m_pEncoderStream;

	int						m_nFrameRate;
	int						m_nQuality;

	VIDEC_CODEC_TYPE		m_CodecType;
	int						m_nBitrate;
	int						m_nQMin;
	int						m_nQMax;
	int						m_nBitrateControlType;

	int						m_nScaleType;
	int						m_nWidth;
	int						m_nHeight;

	MPVideo_Resize*			m_pResize;
	int						m_nDstBufLen;
	unsigned char*			m_pDstBuf;
	unsigned char*			m_dst[3];

	int						m_nSkipFrameCount;
	int						m_nSkipFrameIndex;

	int						m_nSubWidth;
	int						m_nSubHeight;
};


#endif
