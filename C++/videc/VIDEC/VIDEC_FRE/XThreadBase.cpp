// XThreadBase.cpp: implementation of the XThreadBase class.
//
//////////////////////////////////////////////////////////////////////
#include "XThreadBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XThreadBase::XThreadBase()
{
#ifndef WIN32
	sem_init(&m_semWaitForStop,0,1);
	m_thread = -1;
	m_bThreadStopped=true;
#else
	m_dwThreadID=0;
	m_hThread=NULL;
	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
	SetEvent(m_evStop);
#endif
}

XThreadBase::~XThreadBase()
{
#ifndef WIN32
	sem_destroy(&m_semWaitForStop);
#else
	CloseHandle(m_evStop);
#endif
}

bool XThreadBase::StartThread()
{
#ifndef WIN32
	if (pthread_create(&m_thread,NULL,InitThreadProc,(void*)this)!=0)
		return false;
	sem_wait(&m_semWaitForStop);
	m_bThreadStopped=false;
	return true;
#else
	//����߳��Ѿ�����������Ҫ�ٴ���
	if (!m_hThread){ 
		//�����������̺߳���
		m_hThread = CreateThread(
					NULL,			//the handle cannot be inherited. 
                    0,				//the default Thread Stack Size is set
                    InitThreadProc,	//�̺߳���
                    this,			//�̺߳����Ĳ���
                    0,				//ʹ�̺߳������������������
                    &m_dwThreadID	//receives the thread identifier
					);
                
        }//end if (!m_hThread...

	if (m_hThread)
		ResetEvent(m_evStop);

	SetThreadPriority(m_hThread,THREAD_PRIORITY_TIME_CRITICAL);

	return	m_hThread != NULL;
#endif
}

void XThreadBase::WaitForStop()
{
#ifndef WIN32
	if (m_bThreadStopped==false)
	{
		sem_wait(&m_semWaitForStop);
		m_bThreadStopped=true;
		pthread_join(m_thread,NULL);
	}
#else
		
	WaitForSingleObject(m_evStop,INFINITE);

	if (m_hThread) 
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
#endif
}

unsigned long  XThreadBase::ThreadProc()
{
	ThreadProcMain();
#ifndef WIN32
	sem_post(&m_semWaitForStop);
#else
	SetEvent(m_evStop);
#endif
	return 0;
}

