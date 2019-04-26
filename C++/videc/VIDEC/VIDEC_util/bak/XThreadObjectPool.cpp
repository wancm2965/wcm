// XThreadObjectPool.cpp: implementation of the XThreadObjectPool class.
//
//////////////////////////////////////////////////////////////////////

#include "XThreadObjectPool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XThreadObject::XThreadObject(void)
:m_rXThreadObjectPool(XThreadObjectPool::Instance())
,m_bWouldBeClosed(false)
{

}

XThreadObject::~XThreadObject(void)
{

}

bool XThreadObject::Start(void)
{
	m_bWouldBeClosed=false;
	m_rXThreadObjectPool.AddXThreadObject(this);
	return true;
}

void XThreadObject::Stop(void)
{
	//TRACE("%u XThreadObject::Stop\n",(unsigned long)this);
	//请求结束
	{
		XAutoLock l(*this);
		m_bWouldBeClosed=true;
	}

	m_rXThreadObjectPool.RemoveXThreadObject(this);

	//等待结束
	{
		XAutoLock l(*this);
	}
	//TRACE("%u ~XThreadObject::Stop\n",(unsigned long)this);
}

bool XThreadObject::IsPending(void)
{
	if (m_bWouldBeClosed)
	{
		return false;
	}

	return true;
}

bool XThreadObject::WouldBeClosed(void)
{
	return m_bWouldBeClosed;
}


static XThreadObjectPool g_XThreadObjectPool;
static bool g_bXTOInitialized=false;

unsigned long XTOInitialize(void)
{
	if (!g_bXTOInitialized)
	{
		if (!g_XThreadObjectPool.Connect(5))
		{
			g_XThreadObjectPool.ReleaseConnections();
			return 1;
		}
		g_bXTOInitialized=true;
	}
	return 0;
}

void XTOUnintialize(void)
{
	if (g_bXTOInitialized)
	{
		g_bXTOInitialized=false;
		g_XThreadObjectPool.ReleaseConnections();
	}
}

XThreadObjectPool&XThreadObjectPool::Instance(void)
{
	return g_XThreadObjectPool;
}



XThreadObjectPool::XThreadObjectPool(void)
{

}

XThreadObjectPool::~XThreadObjectPool(void)
{

}

void XThreadObjectPool::AddXThreadObject(XThreadObject*pXThreadObject)
{
	XAutoLock l(m_csListXThreadObject);
	m_ListXThreadObjectPending.push_back(pXThreadObject);
}

void XThreadObjectPool::RemoveXThreadObject(XThreadObject*pXThreadObject)
{
	XAutoLock l(m_csListXThreadObject);
	{
		XListPtr::iterator it=m_ListXThreadObjectPending.find(pXThreadObject);
		if (it!=m_ListXThreadObjectPending.end())
		{
			m_ListXThreadObjectPending.erase(it);
		}
	}

	{
		XListPtr::iterator it=m_MapXThreadObjectRunning.find(pXThreadObject);
		if (it!=m_MapXThreadObjectRunning.end())
		{
			m_MapXThreadObjectRunning.erase(it);
		}
	}
}

bool XThreadObjectPool::OnRunOnce(void)
{
	bool bRet=false;
	XThreadObject*pXThreadObject=NULL;
	{
		XAutoLock l(m_csListXThreadObject);
		for (XListPtr::iterator it=m_ListXThreadObjectPending.begin();it!=m_ListXThreadObjectPending.end();++it)
		{
			pXThreadObject=(XThreadObject*)(*it);

			pXThreadObject->Lock();
			if (pXThreadObject->IsPending())
			{
				m_ListXThreadObjectPending.erase(it);
				m_MapXThreadObjectRunning.push_back(pXThreadObject);
				break;
			}
			else
			{
				pXThreadObject->UnLock();
				pXThreadObject=NULL;
			}
		}
	}

	if (pXThreadObject!=NULL)
	{
		bRet=pXThreadObject->OnRunOnce();

		{
			if (!pXThreadObject->WouldBeClosed())
			{
				XAutoLock l(m_csListXThreadObject);
				//m_MapXThreadObjectRunning.erase(pXThreadObject);
				{
					XListPtr::iterator itRunning=m_MapXThreadObjectRunning.find(pXThreadObject);
					if (itRunning!=m_MapXThreadObjectRunning.end())
					{
						m_MapXThreadObjectRunning.erase(itRunning);
					}
				}
				m_ListXThreadObjectPending.push_back(pXThreadObject);
			}
			pXThreadObject->UnLock();
		}
	}

	return bRet;
}
