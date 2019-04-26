﻿// XThreadBase.h: interface for the CAVThreadclass.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XTHREADBASE_H__
#define __XTHREADBASE_H__

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#include <sys/prctl.h> 
#include <pthread.h>
#endif

class XThreadBase
{
public:
    XThreadBase(void);
    virtual ~XThreadBase(void);
public:
    bool StartThreadNew();
    bool StartThread(void);
    void WaitForStop(void);

	//设置线程名称
	void SetName(const char* name);
    /*
    此函数只供类内部调用
    */
#ifdef WIN32
    static DWORD WINAPI InitThreadProc(PVOID pObj){
        return	((XThreadBase*)pObj)->ThreadProc();
    }
#else
    static void* InitThreadProc(void*pObj){
        ((XThreadBase*)pObj)->ThreadProc();
        return 0;
    }
#endif
    /*
    此函数只供类内部调用
    */
    unsigned long ThreadProc(void);

protected:
    /*
    主线程函数，您可以在此函数中实现线程要做的事
    */
    virtual void ThreadProcMain(void) = 0;
    //protected:
public:
#ifdef WIN32
    DWORD	m_dwThreadID;		// 线程标识
    HANDLE	m_hThread;			// 线程句柄
    HANDLE	m_evStop;
#else
    pthread_t	m_thread;
    sem_t		m_semWaitForStop;
    bool		m_bThreadStopped;
#endif
};
#endif
