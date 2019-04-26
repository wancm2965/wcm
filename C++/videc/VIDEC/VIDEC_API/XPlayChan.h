//XPlayChan.h
#ifndef __XPLAYCHAN_H__
#define __XPLAYCHAN_H__

#include "VIDEC.h"
#include "XImageSource.h"
#include "VIDEC_Decoder.h"
#include "XStreamSink.h"
#include "XBitrate.h"
#include "XAutoLock.h"


class XPlayChan
	: public VIDEC_PlayChan
	, public XImageSource
	, public VIDEC_DecoderStreamCallback
	, public XStreamSink
{
public:
	XPlayChan(void);
	virtual~XPlayChan(void);
public:
	int Open(void);
	void Close(void);
	int GetAverageKBPS(void);

	int InputPacket(unsigned char*pPacketData,int nPacketLen);

	int AddImageSink(VIDEC_ImageSink*pImageSink);
	void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	void OnVIDEC_DecoderStreamCallbackFrame(VIDEC_Image*pImage);
	
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
protected:
	XCritSec				m_csDecoderStream;
	VIDEC_DecoderStream*	m_pDecoderStream;
	XBitrate				m_XBitrate;
};

#endif

