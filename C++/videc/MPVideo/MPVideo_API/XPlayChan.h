//XPlayChan.h
#ifndef __XPLAYCHAN_H__
#define __XPLAYCHAN_H__

#include <MPVideo/MPVideo.h>
#include "XImageSource.h"
#include <MPVideo/MPVideo_Decoder.h>
#include "XStreamSink.h"
#include "XBitrate.h"
#include "XAutoLock.h"


class XPlayChan
	: public MPVideo_PlayChan
	, public XImageSource
	, public MPVideo_DecoderStreamCallback
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

	int AddImageSink(MPVideo_ImageSink*pImageSink);
	void RemoveImageSink(MPVideo_ImageSink*pImageSink);
	
	void OnMPVideo_DecoderStreamCallbackFrame(MPVideo_Image*pImage);
	
	virtual void OnAttachedToStreamSource(MPVideo_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(MPVideo_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(MPVideo_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
protected:
	XCritSec				m_csDecoderStream;
	MPVideo_DecoderStream*	m_pDecoderStream;
	XBitrate				m_XBitrate;
};

#endif

