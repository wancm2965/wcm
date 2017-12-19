#pragma once
#include "KBASE\BufferPool.h"
#include "KBASE\AutoLock.h"
struct SSDATA
{
	char* pData;
	int nLen;
	SSDATA()
	{
		pData = NULL;
		nLen = 0;
	}
};

class CSourceData :
	public KBufferPool
{
public:
	CSourceData();
	~CSourceData();

	void PushData(char*pData, int nLen);
	//KBuffer* PopData(void);
	void* PopData(void);
	int GetDataSize(void);
	void ClearAllData(void);


public:
	KCritSec		m_secJGBufferPool;
	BUFFER_QUEUE	m_lstJGBufferPool;
};

