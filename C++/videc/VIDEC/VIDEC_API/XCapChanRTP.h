//XCapChan.h
#ifndef __XCAPCHAN_H__
#define __XCAPCHAN_H__

#include "VIDEC.h"
#include "XImageSink.h"
#include "VIDEC_Encoder.h"
#include "XStreamSource.h"
#include "VIDEC_convert.h"

class XCapChanRTP
	: public VIDEC_CapChanRTP
	, public XImageSink
	, public VIDEC_EncoderStreamCallback
	, public XStreamSource
{
public:
	XCapChanRTP(VIDEC_CapChanRTPCallback&rCallback);
	virtual~XCapChanRTP(void);
public:
	int Open(int nPayloadType,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE CodecType);
	int Open(int nPayloadType,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType);
	void Close(void);
	//int Reset(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType);
	int RequestKeyFrame(void);
	int SetBitrate(int nBitrate);
	
	void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	void OnReceivedImage(VIDEC_Image*pImage);
	void OnReceivedPreSend(int& nSend);

	void OnVIDEC_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnVIDEC_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	
	int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
protected:
	VIDEC_CapChanRTPCallback&	m_rCallback;
	VIDEC_EncoderStream*	m_pEncoderStream;

	int						m_nFrameRate;

	VIDEC_CODEC_TYPE		m_CodecType;
	int						m_nBitrate;
	int						m_nQMin;
	int						m_nQMax;
	int						m_nBitrateControlType;

	int						m_nWidth;
	int						m_nHeight;

	VIDEC_Resize*			m_pResize;
	int						m_nDstBufLen;
	unsigned char*			m_pDstBuf;
	unsigned char*			m_dst[3];

	int						m_nPayloadType;
};


#endif
