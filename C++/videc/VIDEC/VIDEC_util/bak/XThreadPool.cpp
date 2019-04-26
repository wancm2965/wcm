#include "XThreadPool.h"


XThread::XThread(XThreadPool&rXThreadPool)
:m_rXThreadPool(rXThreadPool)
,m_bWantToStop(false)
{
}

XThread::~XThread(void)
{
}


bool XThread::Connect(void)
{
	m_bWantToStop=false;
	return StartThread();
}

void XThread::ReleaseConnections(void)
{
	m_bWantToStop=true;
	WaitForStop();
}

void XThread::ThreadProcMain(void)
{
	while (!m_bWantToStop)
	{
		if (!m_rXThreadPool.OnRunOnce())
		{
#ifdef _WIN32
			Sleep(1);
#else
			usleep(1000);
#endif
		}
	}
}


XThreadPool::XThreadPool(void)
{

}

XThreadPool::~XThreadPool(void)
{

}


bool XThreadPool::Connect(int nThreadCount)
{
	if (nThreadCount<1)
	{
		nThreadCount=1;
	}
	else if (nThreadCount>50)
	{
		nThreadCount=50;
	}

	for (int i=0;i<nThreadCount;++i)
	{
		XThread*pXThread=new XThread(*this);
		if (pXThread->Connect())
		{	
			m_ListXThread.push_back(pXThread);
		}
		else
		{
			pXThread->ReleaseConnections();
			delete pXThread;
			pXThread=NULL;
		}
	}

	return (m_ListXThread.size()>0);
}

void XThreadPool::ReleaseConnections(void)
{
	while (m_ListXThread.size())
	{
		XThread*pXThread=m_ListXThread.front();
		m_ListXThread.pop_front();
		pXThread->ReleaseConnections();
		delete pXThread;
		pXThread=NULL;
	}
}
