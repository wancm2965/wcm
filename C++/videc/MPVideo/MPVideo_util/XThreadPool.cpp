#include "XThreadPool.h"

static XThreadPool g_XThreadPool;

static inline int DetectNumberOfWorkers(void) 
{
#if _WIN32||_WIN64
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return static_cast<int>(si.dwNumberOfProcessors);
#else
	return 24;
#endif
}


XThreadPool::XThreadPool(void)
{
	int nCount=DetectNumberOfWorkers();
	if (nCount<12)
	{
		nCount=12;
	}
	m_nMaxThreads=nCount*2;
}

XThreadPool::~XThreadPool(void)
{

}


XThreadPool&XThreadPool::Instance(void)
{
	return g_XThreadPool;
}


void XThreadPool::StartWorkThread(XWorkThread*pXWorkThread)
{
	XAutoLock l(m_csListXWorkThread);
	XListPtr::iterator iter=m_listXWorkThread.find(pXWorkThread);
	if (iter==m_listXWorkThread.end())
	{
		m_listXWorkThread.push_back(pXWorkThread);

		while (m_listXThreadItem.size()<m_nMaxThreads && m_listXThreadItem.size()<m_listXWorkThread.size()*2)
		{
			XThreadItem*pXThreadItem=new XThreadItem(*this);
			if (pXThreadItem->Start())
			{
				m_listXThreadItem.push_back(pXThreadItem);
			}
			else
			{
				pXThreadItem->Stop();
				delete pXThreadItem;
				pXThreadItem=NULL;

				break;
			}
		}
	}
}

void XThreadPool::StopWorkThread(XWorkThread*pXWorkThread)
{
	{
		XAutoLock l(m_csListXWorkThread);
		m_listXWorkThread.remove(pXWorkThread);
	}

	XThreadItem*pXThreadItem=NULL;
	while (true)
	{
		{
			XAutoLock l(m_csListXWorkThread);
			if (m_listXThreadItem.size()>m_listXWorkThread.size()*2)
			{
				pXThreadItem=(XThreadItem*)m_listXThreadItem.front();
				m_listXThreadItem.pop_front();
			}

		}
		if (pXThreadItem)
		{
			pXThreadItem->Stop();
			delete pXThreadItem;
			pXThreadItem=NULL;
		}
		else
		{
			break;
		}
	}
}

bool XThreadPool::OnXThreadItemCallbackRunning(void)
{
	bool bRet=false;

	XWorkThread*pXWorkThread=NULL;
	{
		XAutoLock l(m_csListXWorkThread);
		if (m_listXWorkThread.size())
		{
			pXWorkThread=(XWorkThread*)m_listXWorkThread.front();
			m_listXWorkThread.pop_front();

			if (pXWorkThread->IsStopped())
			{
				return bRet;
			}

			pXWorkThread->StartRunning();
		}
	}

	if (pXWorkThread)
	{
		if (pXWorkThread->IsPending())
		{
			bRet=pXWorkThread->Running();
		}

		{
			XAutoLock l(m_csListXWorkThread);
			pXWorkThread->StopRunning();
			if (!pXWorkThread->IsStopped())
			{
				XListPtr::iterator iter=m_listXWorkThread.find(pXWorkThread);
				if (iter==m_listXWorkThread.end())
				{
					m_listXWorkThread.push_back(pXWorkThread);
				}
			}
		}
	}
	return bRet;
}
