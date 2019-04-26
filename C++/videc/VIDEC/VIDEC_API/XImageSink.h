//XImageSink.h
#ifndef __XIMAGESINK_H__
#define __XIMAGESINK_H__

#include "VIDEC.h"
#include "XAutoLock.h"
#include "XImage.h"

class XImageSink
{
public:
	XImageSink(VIDEC_ImageSink*pImageSink);
	virtual~XImageSink(void);
public:
	virtual void Close(void);
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
protected:
	VIDEC_ImageSource*	m_pImageSource;
	XCritSec			m_csImageSource;
	VIDEC_ImageSink*	m_pImageSink;
};

#endif