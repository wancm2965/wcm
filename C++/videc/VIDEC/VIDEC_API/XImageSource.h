//XImageSource.h
#ifndef __XIMAGESOURCE_H__
#define __XIMAGESOURCE_H__


#include "VIDEC.h"
#include "XAutoLock.h"
#include "XListPtr.h"


class XImageSource
{
public:
	XImageSource(VIDEC_ImageSource*pImageSource);
	virtual~XImageSource(void);
public:
	virtual void Close(void);
	virtual int PreSendImage(void);
	virtual int SendImage(VIDEC_Image*pImage);
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	virtual int GetImageSinkCount(void);

	virtual int SendMotionDetect(int*pResultList,int nResultCount);
	virtual int SendAudioSamples(short*pSamples,int nSamples,int nSampleRate);
protected:
	VIDEC_ImageSource*	m_pImageSource;
	
	XListPtr			m_ListImageSink;
	XCritSec			m_csListImageSink;
};

#endif
