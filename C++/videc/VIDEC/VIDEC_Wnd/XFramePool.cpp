#include "stdafx.h"
#include "XFramePool.h"

XFramePool::XFramePool(void)
{
} 

XFramePool::~XFramePool(void)
{
	FlushPool();
}

void XFramePool::PushFrame(XImage*pXImage)
{
	pXImage->AddRef();
	XAutoLock l(m_csListXFrame);
	m_listXFrame.push_back(pXImage);
}

void XFramePool::LockPool(void)
{
	m_csListXFrame.Lock();
}

bool XFramePool::GetFrame(XImage**ppXImage)
{
	if (m_listXFrame.size())
	{
		*ppXImage=(XImage*)m_listXFrame.front();
		(*ppXImage)->AddRef();
		return true;
	}
	return false;
}

void XFramePool::UnlockPool(void)
{
	m_csListXFrame.UnLock();
}

void XFramePool::PopFrame(void)
{
	XAutoLock l(m_csListXFrame);
	if (m_listXFrame.size())
	{
		XImage*pXImage=(XImage*)m_listXFrame.front();
		m_listXFrame.pop_front();
		pXImage->Release();
		pXImage=NULL;
	}
}

int XFramePool::GetFrameCount(void)
{
	XAutoLock l(m_csListXFrame);
	return (m_listXFrame.size());
}

void XFramePool::FlushPool(void)
{
	XAutoLock l(m_csListXFrame);
	while (m_listXFrame.size()>0)
	{
		XImage*pXImage=(XImage*)m_listXFrame.front();
		m_listXFrame.pop_front();
		pXImage->Release();
		pXImage=NULL;
	}
}

void XFramePool::FlushPoolToNormal(unsigned long ulNormalInterval)
{
	XAutoLock l(m_csListXFrame);

	unsigned long ulLastTimestamp=0;
	if (m_listXFrame.size()>3)
	{
		XImage*pXImage=(XImage*)m_listXFrame.back();
		ulLastTimestamp=pXImage->GetTimestamp();
	}
	else
	{
		return;
	}

	while (m_listXFrame.size())
	{
		XImage*pXImage=(XImage*)m_listXFrame.front();
		if (ulLastTimestamp-pXImage->GetTimestamp()>ulNormalInterval)
		{
			m_listXFrame.pop_front();
			pXImage->Release();
			pXImage=NULL;
		}
		else
		{
			break;
		}
	}
}