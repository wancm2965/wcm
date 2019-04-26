#ifndef __XBUFFERPOOL_H__
#define __XBUFFERPOOL_H__

#include "XListPtr.h"
#include "XDataBuffer.h"
#include "XAutoLock.h"

class XBufferPool
{
public:
	XBufferPool(void);
	virtual ~XBufferPool(void);
public:
	virtual void SetDataBuffer(XDataBuffer*pXDataBuffer);
	virtual XDataBuffer*GetDataBuffer(void);
	virtual int GetCount(void);
	virtual void Flush(void);
protected:
	XCritSec			m_csListDataBuffer;
	XListPtr			m_listDataBuffer;
};

#endif
