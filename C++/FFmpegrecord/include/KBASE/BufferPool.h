/* KBufferPool.h */

#pragma once

#include "KBASE/KBASE_Export.h"
#include "Buffer.h"
#include "CritSec.h"
#include <queue>

typedef std::queue<void*> BUFFER_QUEUE;

//---------------------------------------------------------------------------------------
class KBASE_API KBufferPool
{
public:
	KBufferPool(void);
	~KBufferPool(void);

	virtual void Push(char* pData,int nLen);
	virtual void Push(KBuffer* pBuffer);
	virtual KBuffer* Pop(void);

	virtual int GetSize(void);
	virtual void Clear(void);

protected:
	KCritSec		m_CritSec;
	BUFFER_QUEUE	m_listBuffer;
};

