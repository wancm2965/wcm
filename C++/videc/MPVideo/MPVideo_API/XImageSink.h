//XImageSink.h
#ifndef __XIMAGESINK_H__
#define __XIMAGESINK_H__

#include <MPVideo/MPVideo.h>
#include "XAutoLock.h"
#include "XImage.h"

class XImageSink
{
public:
	XImageSink(MPVideo_ImageSink*pImageSink);
	virtual~XImageSink(void);
public:
	virtual void Close(void);
	virtual void OnAttachedToImageSource(MPVideo_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource);
protected:
	MPVideo_ImageSource*	m_pImageSource;
	XCritSec			m_csImageSource;
	MPVideo_ImageSink*	m_pImageSink;
};

#endif