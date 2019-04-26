#ifndef __XFRAMEPOOL_H__
#define __XFRAMEPOOL_H__

#include "XListPtr.h"
#include "XAutoLock.h"
#include "VIDEC.h"
#include "../VIDEC_API/XImage.h"

class XFramePool
{
public:
	XFramePool(void);
	virtual ~XFramePool(void);
public:
	virtual void PushFrame(XImage*pXImage);
	void LockPool(void);
	bool GetFrame(XImage**ppXImage);
	void UnlockPool(void);
	void PopFrame(void);
	int GetFrameCount(void);
	void FlushPool(void);

	void FlushPoolToNormal(unsigned long ulNormalInterval);
protected:
	XCritSec		m_csListXFrame;
	XListPtr		m_listXFrame;
};

#endif