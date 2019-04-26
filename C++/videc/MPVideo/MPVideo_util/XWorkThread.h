//XWorkThread.h
#ifndef __XWORKTHREAD_H__
#define __XWORKTHREAD_H__

#include "XAutoLock.h"

class XWorkThread
{
public:
	XWorkThread(void);
	virtual~XWorkThread(void);
public:
	virtual int Start(void);
	virtual void Stop(void);

	virtual void StartRunning(void);
	virtual void StopRunning(void);
	virtual bool IsStopped(void);
	virtual bool IsPending(void)=0;
	virtual bool Running(void)=0;
protected:
	XCritSec	m_csStopped;
	bool		m_bStopped;
	XCritSec	m_csRunning;
	bool		m_bRunning;
};

#endif
