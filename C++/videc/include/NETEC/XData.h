//XData.h

#ifndef __XDATA_H__
#define __XDATA_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string>

#include "../../Include/NETEC/NETEC_TYPES.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <NETEC/XAutoLock.h>
#endif
class XData
{
public:
	XData(const void*pData,int32_t nLen);
	virtual~XData(void);
public:
	virtual void AddRef(void);
	virtual void Release(void);

	virtual void*GetData(void);
	virtual int32_t GetLen(void);
protected:
#ifdef _WIN32
#ifdef NETEC_ARM
    LONG			m_cRef;
#else
    volatile LONG	m_cRef;
#endif
#else
	volatile int32_t	m_cRef;
	XCritSec		m_csRef;
#endif

	void*			m_pData;
	int32_t				m_nDataLen;
};


#endif

