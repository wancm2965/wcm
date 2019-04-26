// XThreadBase.cpp: implementation of the XThreadBase class.
//
//////////////////////////////////////////////////////////////////////
#include "XThreadBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XThreadBase::XThreadBase()
{
#ifndef _WIN32
	m_thread = NULL;
	m_bThreadStopped=true;
#else
	m_dwThreadID=0;
	m_hThread=NULL;
	m_evStop=NULL;
#endif
}

XThreadBase::~XThreadBase()
{
}

bool XThreadBase::StartThread()
{
#ifndef _WIN32
	sem_init(&m_semWaitForStop,0,1);

	if (pthread_create(&m_thread,NULL,InitThreadProc,(void*)this)!=0)
		return false;
	sem_wait(&m_semWaitForStop);
	m_bThreadStopped=false;
	return true;
#else
	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
	SetEvent(m_evStop);

	//如果线程已经创建，则不需要再创建
	if (!m_hThread){ 
		//创建并启动线程函数
		m_hThread = CreateThread(
					NULL,			//the handle cannot be inherited. 
                    0,				//the default Thread Stack Size is set
                    InitThreadProc,	//线程函数
                    this,			//线程函数的参数
                    0,				//使线程函数创建完后立即启动
                    &m_dwThreadID	//receives the thread identifier
					);
                
        }//end if (!m_hThread...

	if (m_hThread)
		ResetEvent(m_evStop);

	SetThreadPriority(m_hThread,THREAD_PRIORITY_HIGHEST);

	return	m_hThread != NULL;
#endif
}

void XThreadBase::WaitForStop()
{
#ifndef _WIN32
	if (m_bThreadStopped==false)
	{
		sem_wait(&m_semWaitForStop);
		m_bThreadStopped=true;
		pthread_join(m_thread,NULL);
		sem_destroy(&m_semWaitForStop);
	}
#else
		
	WaitForSingleObject(m_evStop,INFINITE);

	// 返回线程句柄
	HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
	if (hThread) 
	{
		CloseHandle(hThread);
	}// end if (hThread...
	
	CloseHandle(m_evStop);
#endif
}

unsigned long  XThreadBase::ThreadProc()
{
	ThreadProcMain();
#ifndef _WIN32
	sem_post(&m_semWaitForStop);
#else
	SetEvent(m_evStop);
#endif
	return 0;
}

