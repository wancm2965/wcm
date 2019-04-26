//XStreamSink.h
#ifndef __XSTREAMSINK_H__
#define __XSTREAMSINK_H__

#include <MPVideo/MPVideo.h>
#include "XAutoLock.h"

class XStreamSink
{
public:
	XStreamSink(MPVideo_StreamSink*pStreamSink);
	virtual~XStreamSink(void);
public:
	virtual void Close(void);
	virtual void OnAttachedToStreamSource(MPVideo_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(MPVideo_StreamSource*pStreamSource);
protected:
	MPVideo_StreamSource*	m_pStreamSource;
	XCritSec			m_csStreamSource;
	MPVideo_StreamSink*	m_pStreamSink;
};

#endif