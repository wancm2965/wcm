#ifndef __XTHREADPOOL_H__
#define __XTHREADPOOL_H__

#include "XThreadBase.h"
#include "XAutoLock.h"

#include <list>

class XThreadPool;
class XThread
	: public XThreadBase
{
public:
	XThread(XThreadPool&rXThreadPool);
	virtual ~XThread(void);
public:
	bool Connect(void);
	void ReleaseConnections(void);
protected:
	void ThreadProcMain(void);
protected:
	XThreadPool&	m_rXThreadPool;
	bool			m_bWantToStop;
};

typedef std::list<XThread*>	ListXThread;

class XThreadPool
{
public:
	XThreadPool(void);
	virtual~XThreadPool(void);
public:
	virtual bool Connect(int nThreadCount);
	virtual void ReleaseConnections(void);
	virtual bool OnRunOnce(void)=0;
protected:
	ListXThread	m_ListXThread;
};


#endif