//XImageSource.h
#ifndef __XIMAGESOURCE_H__
#define __XIMAGESOURCE_H__


#include <MPVideo/MPVideo.h>
#include "XAutoLock.h"
#include "XListPtr.h"


class XImageSource
{
public:
	XImageSource(MPVideo_ImageSource*pImageSource);
	virtual~XImageSource(void);
public:
	virtual void Close(void);
	virtual int SendImage(MPVideo_Image*pImage);
	virtual int AddImageSink(MPVideo_ImageSink*pImageSink);
	virtual void RemoveImageSink(MPVideo_ImageSink*pImageSink);
	virtual int GetImageSinkCount(void);

	virtual int SendMotionDetect(int*pResultList,int nResultCount);
	virtual int SendAudioSamples(short*pSamples,int nSamples,int nSampleRate);
protected:
	MPVideo_ImageSource*	m_pImageSource;
	
	XListPtr			m_ListImageSink;
	XCritSec			m_csListImageSink;
};

#endif
