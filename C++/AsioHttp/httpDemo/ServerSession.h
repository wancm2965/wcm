#pragma once
#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#endif


class CServerSession
{
public:
	CServerSession();
	~CServerSession();

	void StartWait();
	void StopWait();

private:
#ifdef WIN32
	HANDLE m_evOver;
#else
	sem_t m_semWaitForOver;
#endif
};

