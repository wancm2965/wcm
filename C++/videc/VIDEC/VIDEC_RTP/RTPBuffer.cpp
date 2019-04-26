#include <stdlib.h>
#include <memory.h>

#include <map>
//#include <NETEC/XAutoLock.h>
#include "XAutoLock.h"
#include "RTPBuffer.h"

#define MAX_RTP_BUFFER_LEN			1500
#define MAX_FREE_RTP_BUFFER_COUNT	600	


class RTPMemory
{
public:
	RTPMemory(void);
	virtual ~RTPMemory(void);
public:
	RTPBuffer*GetFreeRTPBuffer(void);
	void SetFreeRTPBuffer(RTPBuffer*pRTPBuffer);
protected:
	typedef std::map<void*,RTPBuffer*>	MapRTPBuffer;

	MapRTPBuffer	m_MapBuffer;
	XCritSec		m_csMapBuffer;
};


static RTPMemory g_RTPMemory;

RTPMemory::RTPMemory(void)
{
}

RTPMemory::~RTPMemory(void)
{
	XAutoLock l(m_csMapBuffer);
	MapRTPBuffer::iterator iter=m_MapBuffer.begin();
	while (iter!=m_MapBuffer.end())
	{
		RTPBuffer*pRTPBuffer=iter->second;
		++iter;

		delete pRTPBuffer;
		pRTPBuffer=NULL;
	}
	m_MapBuffer.clear();
}

RTPBuffer*RTPMemory::GetFreeRTPBuffer(void)
{
	RTPBuffer*pRTPBuffer=NULL;
	{
		XAutoLock l(m_csMapBuffer);
		MapRTPBuffer::iterator iter=m_MapBuffer.begin();
		if (iter!=m_MapBuffer.end())
		{
			pRTPBuffer=iter->second;
			m_MapBuffer.erase(iter);
		}
	}

	if (pRTPBuffer==NULL)
	{
		NEWRECORD( pRTPBuffer,new RTPBuffer() )
		//NULLReturnRet( pRTPBuffer , pRTPBuffer )  
	}

	return pRTPBuffer;
}

void RTPMemory::SetFreeRTPBuffer(RTPBuffer*pRTPBuffer)
{
	if (pRTPBuffer!=NULL)
	{
		XAutoLock l(m_csMapBuffer);
		if (m_MapBuffer.size()>MAX_FREE_RTP_BUFFER_COUNT)
		{
			delete pRTPBuffer;
			pRTPBuffer=NULL;
		}
		else
		{
			m_MapBuffer[pRTPBuffer]=pRTPBuffer;
		}
	}
}


RTPBuffer*GetFreeRTPBuffer(void)
{
	return g_RTPMemory.GetFreeRTPBuffer();
}


static int g_nRTPBufferCount=0;

RTPBuffer::RTPBuffer(void)
: m_pData(NULL)
, m_nDataLen(0)
#ifdef _WIN32
, m_cRef(0)
#endif
{
	m_pData=(unsigned char*)MALLOCMSG(MAX_RTP_BUFFER_LEN);

#ifndef _WIN32
	m_cRef=0;
#endif

	//char szDebug[1024];
	//sprintf(szDebug,"g_nRTPBufferCount=%d\n",++g_nRTPBufferCount);
	//DebugOutput(szDebug);

}

RTPBuffer::~RTPBuffer(void)
{
	if (m_pData!=NULL)
	{
		FREEMSG(m_pData);
		m_pData=NULL;
	}

	//char szDebug[1024];
	//sprintf(szDebug,"g_nRTPBufferCount=%d\n",--g_nRTPBufferCount);
	//DebugOutput(szDebug);
}

void RTPBuffer::AddRef(void)
{
#ifdef _WIN32
    LONG nRef = InterlockedIncrement( &m_cRef );
#else
	XAutoLock l(m_csRef);
	int nRef = ++m_cRef;
#endif
}

void RTPBuffer::Release(void)
{
#ifdef _WIN32
	LONG nRef = InterlockedDecrement( &m_cRef );
#else
	XAutoLock l(m_csRef);
	int nRef = --m_cRef;
#endif
    if (nRef == 0) 
	{
		m_nDataLen=0;

		g_RTPMemory.SetFreeRTPBuffer(this);
	}
}

void*RTPBuffer::GetData(void)
{
	return m_pData;
}

int RTPBuffer::GetLen(void)
{
	return m_nDataLen;
}

void RTPBuffer::SetLen(int nLen)
{
	m_nDataLen=nLen;
}

int RTPBuffer::CopyData(void*pData,int nLen)
{
	if (m_pData==NULL || pData==NULL || nLen<=0)
	{
		m_nDataLen=0;

		return m_nDataLen;
	}

	if (nLen>MAX_RTP_BUFFER_LEN)
	{
		nLen=MAX_RTP_BUFFER_LEN;
	}

	memcpy(m_pData,pData,nLen);

	m_nDataLen=nLen;
	return m_nDataLen;
}
