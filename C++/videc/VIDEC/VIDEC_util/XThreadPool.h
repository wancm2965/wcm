//XThreadPool.h
#ifndef __XTHREADPOOL_H__
#define __XTHREADPOOL_H__

#include "XAutoLock.h"
#include "XUtil.h"
#include "XListPtr.h"
#include "XThreadBase.h"
#include "XWorkThread.h"

class XThreadItemCallback
{
public:
	virtual bool OnXThreadItemCallbackRunning(void)=0;
};

class XThreadItem
	: public XThreadBase
{
public:
	XThreadItem(XThreadItemCallback&rCallback)
		: m_rCallback(rCallback)
		, m_bRunning(false)
	{
	}
	virtual~XThreadItem(void)
	{
	}
public:
	bool Start(void)
	{
		m_bRunning=true;
		return StartThread();
	}
	void Stop(void)
	{
		m_bRunning=false;
		WaitForStop();
	}
protected:
	void ThreadProcMain(void)
	{
		while (m_bRunning)
		{
			if (!m_rCallback.OnXThreadItemCallbackRunning())
			{
				XSleep(1);
			}
		}
	}
protected:
	XThreadItemCallback&	m_rCallback;
	bool					m_bRunning;
};

class XThreadPool
	: public XThreadItemCallback
{
public:
	XThreadPool(void);
	virtual~XThreadPool(void);
public:
	static XThreadPool&Instance(void);
public:
	virtual void StartWorkThread(XWorkThread*pXWorkThread);
	virtual void StopWorkThread(XWorkThread*pXWorkThread);
protected:
	virtual bool OnXThreadItemCallbackRunning(void);
protected:
	XListPtr	m_listXThreadItem;
	XListPtr	m_listXWorkThread;
	XCritSec	m_csListXWorkThread;

	int			m_nMaxThreads;
};

#endif
