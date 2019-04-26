// XAutoLock.h: interface for the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XAUTOLOCK_H__
#define __XAUTOLOCK_H__

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#endif

class XCritSec  
{
public:
	XCritSec(void);
	virtual ~XCritSec(void);
public:
	void Lock(void);
	void UnLock(void);
protected:
#ifdef _WIN32
	CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t		m_pMutex;
#endif
};

class XAutoLock  
{
public:
	XAutoLock(const XCritSec&rXCritSec) ;
	~XAutoLock(void);
protected:
    XCritSec& m_rXCritSec;
};

#endif 