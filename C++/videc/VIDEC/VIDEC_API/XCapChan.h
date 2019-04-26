//XCapChan.h
#ifndef __XCAPCHAN_H__
#define __XCAPCHAN_H__

#include "VIDEC.h"
#include "XImageSink.h"
#include "VIDEC_Encoder.h"
#include "XStreamSource.h"
#include "VIDEC_convert.h"

class XCapChan
	: public VIDEC_CapChan
	, public XImageSink
	, public VIDEC_EncoderStreamCallback
	, public XStreamSource
{
public:
	XCapChan(VIDEC_CapChanCallback&rCallback);
	virtual~XCapChan(void);
public:
	int Open(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nWidth,int nHeight);
	void Close(void);
	int Reset(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nWidth,int nHeight);
	int RequestKeyFrame(void);
	int GetEncoderType(void);
	void SetSkipFrame(bool bSkip);
	int GetBitrate(void);
	
	void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	void OnReceivedImage(VIDEC_Image*pImage);
	void OnReceivedPreSend(int& nSend);

	void OnVIDEC_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnVIDEC_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight);
	void OnVIDEC_EncoderStreamCallbackPreEncode(int&nSkipFlag);

	int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
	
	void ReSetFrameRate(void);
protected:
	VIDEC_CapChanCallback&	m_rCallback;
	VIDEC_EncoderStream*	m_pEncoderStream;

	int						m_nFrameRate;

	VIDEC_CODEC_TYPE		m_CodecType;
	int						m_nBitrate;
	int						m_nQMin;
	int						m_nQMax;
	int						m_nBitrateControlType;

	int						m_nWidth;
	int						m_nHeight;

	int						m_nClipWidth;
	int						m_nClipHeight;

	int						m_nScaleType;

	VIDEC_Resize*			m_pResize;
	int						m_nDstBufLen;
	unsigned char*			m_pDstBuf;
	unsigned char*			m_dst[3];

	unsigned long			m_ulLastCountFrameTimestamp;
	unsigned long			m_nFrameRateCur;
	bool					m_bSkipFrame;
	bool					m_bForceSkipFrame;
};


#endif
