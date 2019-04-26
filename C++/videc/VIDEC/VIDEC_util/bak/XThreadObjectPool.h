// XThreadObjectPool.h: interface for the XThreadObjectPool class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XTHREADOBJECTPOOL_H__
#define __XTHREADOBJECTPOOL_H__

#include "XThreadPool.h"
#include "XListPtr.h"

class XThreadObjectPool;
class XThreadObject
	: public XCritSec
{
public:
	XThreadObject(void);
	virtual~XThreadObject(void);
public:
	virtual bool Start(void);
	virtual void Stop(void);
	virtual bool IsPending(void);
	virtual bool WouldBeClosed(void);
	virtual bool OnRunOnce(void)=0;
protected:
	XThreadObjectPool&	m_rXThreadObjectPool;
	bool				m_bWouldBeClosed;
};

typedef XListPtr	MapXThreadObject;

class XThreadObjectPool  
	: public XThreadPool
{
public:
	XThreadObjectPool(void);
	virtual ~XThreadObjectPool(void);
public:
	static XThreadObjectPool&Instance(void);
public:
	void AddXThreadObject(XThreadObject*pXThreadObject);
	void RemoveXThreadObject(XThreadObject*pXThreadObject);
protected:
	bool OnRunOnce(void);
protected:
	XListPtr			m_ListXThreadObjectPending;
	MapXThreadObject	m_MapXThreadObjectRunning;
	XCritSec			m_csListXThreadObject;
};

unsigned long XTOInitialize(void);
void XTOUnintialize(void);


#endif