#include "stdafx.h"
#include "SourceData.h"


CSourceData::CSourceData()
{
}


CSourceData::~CSourceData()
{
}

void CSourceData::PushData(char*pData, int nLen)
{
	if (pData == NULL || nLen < 1)
	{
		return;
	}


	//KBufferPool::Push((char*)pData, nLen);
	SSDATA*pssdata = new SSDATA();
	if (NULL != pssdata)
	{
		pssdata->pData = (char*)realloc(pssdata->pData, nLen);
		if (NULL != pssdata->pData)
		{
			memcpy(pssdata->pData, pData, nLen);
			pssdata->nLen = nLen;
			//KAutoLock l(m_secJGBufferPool);
			m_lstJGBufferPool.push(pssdata);
		}		
	}
	
}

void* CSourceData::PopData(void)
{
// 	KBuffer* pBuffer = NULL;
// 	if (KBufferPool::GetSize() > 0)
// 	{
// 		pBuffer = KBufferPool::Pop();
// 	}
// 	return pBuffer;
	KAutoLock l(m_secJGBufferPool);
	if (m_lstJGBufferPool.size()<1)
	{
		return NULL;
	}
	void*pData = m_lstJGBufferPool.front();
	m_lstJGBufferPool.pop();

	return pData;

}

int CSourceData::GetDataSize(void)
{
	/*return KBufferPool::GetSize();*/
	return m_lstJGBufferPool.size();
}

void CSourceData::ClearAllData(void)
{
	/*KBufferPool::Clear();*/
	KAutoLock l(m_secJGBufferPool);
	while (!m_lstJGBufferPool.empty())
	{
		SSDATA* pData = (SSDATA*)m_lstJGBufferPool.front();
		m_lstJGBufferPool.pop();
		if (pData)
		{
			if (pData->pData)
			{
				free(pData->pData);
				pData->pData = NULL;
			}
			delete pData;
			pData = NULL;
		}
	}
}
