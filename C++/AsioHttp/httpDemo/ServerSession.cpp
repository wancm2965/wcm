#include "stdafx.h"
#include "ServerSession.h"


CServerSession::CServerSession()
{
#ifdef WIN32
	m_evOver = CreateEvent(NULL, TRUE, TRUE, NULL);
	SetEvent(m_evOver);
#else
	sem_init(&m_semWaitForOver, 0, 1);
#endif
}


CServerSession::~CServerSession()
{
#ifdef WIN32
	CloseHandle(m_evOver);
#else
	sem_destroy(&m_semWaitForOver);
#endif
}

void CServerSession::StartWait()
{
#ifdef WIN32
	ResetEvent(m_evOver);
	WaitForSingleObject(m_evOver, INFINITE);
	int nStart = 0;
#else
	sem_wait(&m_semWaitForOver);
#endif

}

void CServerSession::StopWait()
{
#ifdef WIN32
	SetEvent(m_evOver);
#else
	sem_post(&m_semWaitForOver);
#endif
}
