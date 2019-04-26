#include "XWorkThread.h"
#include "XThreadPool.h"
#include "XUtil.h"

XWorkThread::XWorkThread(void)
{
	m_bStopped=false;
	m_bRunning=false;
}

XWorkThread::~XWorkThread(void)
{

}


int XWorkThread::Start(void)
{
	{
		XAutoLock l(m_csRunning);
		m_bRunning=false;
	}

	{
		XAutoLock l(m_csStopped);
		m_bStopped=false;
	}

	XThreadPool::Instance().StartWorkThread(this);
	return 0;
}

void XWorkThread::Stop(void)
{
	{
		XAutoLock l(m_csStopped);
		m_bStopped=true;
	}

	XThreadPool::Instance().StopWorkThread(this);

	while (true)
	{
		XAutoLock l(m_csRunning);
		if (m_bRunning)
		{
			XSleep(1);
		}
		else
		{
			break;
		}
	}
}


void XWorkThread::StartRunning(void)
{
	XAutoLock l(m_csRunning);
	m_bRunning=true;
}

void XWorkThread::StopRunning(void)
{
	XAutoLock l(m_csRunning);
	m_bRunning=false;
}

bool XWorkThread::IsStopped(void)
{
	XAutoLock l(m_csStopped);
	return m_bStopped;
}
