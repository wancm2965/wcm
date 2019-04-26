//XStreamSink.h
#ifndef __XSTREAMSINK_H__
#define __XSTREAMSINK_H__

#include "VIDEC.h"
#include "XAutoLock.h"

class XStreamSink
{
public:
	XStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual~XStreamSink(void);
public:
	virtual void Close(void);
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
protected:
	VIDEC_StreamSource*	m_pStreamSource;
	XCritSec			m_csStreamSource;
	VIDEC_StreamSink*	m_pStreamSink;
};

#endif