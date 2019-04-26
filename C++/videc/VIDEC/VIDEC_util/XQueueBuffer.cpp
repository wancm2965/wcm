
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "XQueueBuffer.h"


XQueueBuffer::XQueueBuffer(int nBufferLen)
{
	m_nBufferLen=nBufferLen;
	m_pBufferHead=(char*)MALLOCMSG(nBufferLen);
	m_pBufferTail=m_pBufferHead+m_nBufferLen-1;
	m_pBack=m_pFront=m_pBufferHead;
	m_bEmpty=true;
}

XQueueBuffer::~XQueueBuffer(void)
{
	if (m_pBufferHead)
	{
		FREEMSG(m_pBufferHead);
		m_pBufferHead=NULL;
	}
}

int XQueueBuffer::Read(char*pBuffer,int nLen)
{
	XAutoLock l(m_csThis);

	int nFilledSize=GetFilledSize();

	//TRACE("Before Read Filled Size=%d Free Size=%d Buffer Len=%d \n",GetFilledSize(),GetFreeSize(),m_nBufferLen);

	if(nFilledSize==0)
	{
		return 0;
	}

	int nRead=nLen;
	if (nRead>nFilledSize)
	{
		nRead=nFilledSize;
	}

	int nTop=m_pBufferTail-m_pFront+1;
	if (nTop>=nRead)
	{
		memcpy(pBuffer,m_pFront,nRead);
	}
	else
	{
		memcpy(pBuffer,m_pFront,nTop);
		memcpy(pBuffer+nTop,m_pBufferHead,nRead-nTop);
	}

	MovePointer(&m_pFront,nRead);

	if (m_pFront==m_pBack)
	{
		m_bEmpty=true;
		//TRACE("Empty\n");
	}

	//TRACE("After Read Filled Size=%d Free Size=%d Buffer Len=%d Want Read=%d Actually Read=%d\n",GetFilledSize(),GetFreeSize(),m_nBufferLen,nLen,nRead);

	return nRead;
}

void XQueueBuffer::Write(char*pData,int nLen)
{
	XAutoLock l(m_csThis);

	//TRACE("Before Write Filled Size=%d Free Size=%d Buffer Len=%d \n",GetFilledSize(),GetFreeSize(),m_nBufferLen);

	int nFreeSize=GetFreeSize();
	if (nFreeSize<nLen)
	{
		MovePointer(&m_pFront,nLen-nFreeSize);
	}
	
	int nTop=m_pBufferTail-m_pBack+1;
	if (nTop>=nLen)
	{
		memcpy(m_pBack,pData,nLen);
	}
	else
	{
		memcpy(m_pBack,pData,nTop);
		memcpy(m_pBufferHead,pData+nTop,nLen-nTop);
	}

	MovePointer(&m_pBack,nLen);
	m_bEmpty=false;
	
	//TRACE("After Write Filled Size=%d Free Size=%d Buffer Len=%d Write=%d\n",GetFilledSize(),GetFreeSize(),m_nBufferLen,nLen);
}

int XQueueBuffer::GetBufferedSize(void)
{
	XAutoLock l(m_csThis);
	return GetFilledSize();
}

int XQueueBuffer::GetFilledSize(void)
{
	int nFilledSize=(m_nBufferLen+(m_pBack-m_pFront))%m_nBufferLen;
	if (nFilledSize==0 && !m_bEmpty)
	{
		nFilledSize=m_nBufferLen;
	}
	return nFilledSize;
}

int XQueueBuffer::GetFreeSize(void)
{
	int nFreeSize=(m_nBufferLen+(m_pFront-m_pBack))%m_nBufferLen;
	if (nFreeSize==0 && m_bEmpty)
	{
		nFreeSize=m_nBufferLen;
	}
	return nFreeSize;
}

void XQueueBuffer::MovePointer(char**p,int nOffset)
{
	(*p)+=nOffset;
	if ((*p)>m_pBufferTail)
	{
		(*p)=m_pBufferHead+((*p)-m_pBufferTail-1);
	}
}
