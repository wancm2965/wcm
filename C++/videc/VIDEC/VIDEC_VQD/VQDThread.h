//VQDThread.h
#ifndef __VQDTHREAD_H__
#define __VQDTHREAD_H__

#include "VQDWrapper.h"
#include "XThreadBase.h"
#include <VIDEC/VIDEC_VQD.h>
#include "../VIDEC_API/XImageSink.h"
#include "XListPtr.h"
#include "XAutoLock.h"

class VQDThread
	: public XThreadBase
	, public VIDEC_VQD
	, public XImageSink
{
public:
	VQDThread(VIDEC_VQDCallback&rCallback);
	virtual~VQDThread(void);
public:
	int Open(int nDetectFlags);
	void Close(void);
	void OnReceivedImage(VIDEC_Image*pImage);
	void OnReceivedPreSend(int& nSend);
	void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	bool IsDetecting(void);
protected:
	void ThreadProcMain(void);
protected:
	VIDEC_VQDCallback&	m_rCallback;

	void*				m_pHandle;
	bool				m_bRunning;
	unsigned long		m_nDetectFlags;
	unsigned long		m_nDetectResult;
	XCritSec			m_csListImage;
	XListPtr			m_listImage;
};

#endif
