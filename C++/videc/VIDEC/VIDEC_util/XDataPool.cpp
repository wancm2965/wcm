// XDataPool.cpp: implementation of the XDataPool class.
//
//////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "XDataPool.h"
#include "XAutoLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XDataPool::XDataPool(int nMargin)
:m_nMargin(nMargin)
{
	if (m_nMargin<0)
	{
		m_nMargin=0;
	}
}

XDataPool::~XDataPool(void)
{
	FlushPool();
}

void XDataPool::PushBack(void*pData,int nLen)
{
	if (pData==NULL || nLen<=0)
		return;

	XAutoLock l(m_csListXDataBuffer);
	NEWRECORD3( XDataBuffer*,pXDataBuffer,new XDataBuffer )
	//NULLReturn( pXDataBuffer )  
	if (pXDataBuffer)
	{
		pXDataBuffer->pBuffer=MALLOCMSG(nLen+m_nMargin);
		if (pXDataBuffer->pBuffer)
		{
			memcpy(pXDataBuffer->pBuffer,pData,nLen);
			pXDataBuffer->nLen=nLen;
			m_ListXDataBuffer.push_back(pXDataBuffer);
			return;
		}
		delete pXDataBuffer;
		pXDataBuffer=NULL;
	}
}

void XDataPool::LockPool(void)
{
	m_csListXDataBuffer.Lock();
}

void XDataPool::UnlockPool(void)
{
	m_csListXDataBuffer.UnLock();
}

bool XDataPool::get_front(void**ppData,int&nLen)
{
	if (m_ListXDataBuffer.size())
	{
		XDataBuffer*pXDataBuffer=(XDataBuffer*)m_ListXDataBuffer.front();
		*ppData=pXDataBuffer->pBuffer;
		nLen=pXDataBuffer->nLen;
		return true;
	}
	return false;
}

void XDataPool::PopFront(void)
{
	XAutoLock l(m_csListXDataBuffer);
	if (m_ListXDataBuffer.size())
	{
		XDataBuffer*pXDataBuffer=(XDataBuffer*)m_ListXDataBuffer.front();
		m_ListXDataBuffer.pop_front();
		delete pXDataBuffer;
		pXDataBuffer=NULL;
	}
}

int XDataPool::GetDataCount(void)
{
	XAutoLock l(m_csListXDataBuffer);
	return (m_ListXDataBuffer.size());
}

void XDataPool::FlushPool(void)
{
	XAutoLock l(m_csListXDataBuffer);
	while (m_ListXDataBuffer.size()>0)
	{
		XDataBuffer*pXDataBuffer=(XDataBuffer*)m_ListXDataBuffer.front();
		m_ListXDataBuffer.pop_front();
		delete pXDataBuffer;
		pXDataBuffer=NULL;
	}
}
