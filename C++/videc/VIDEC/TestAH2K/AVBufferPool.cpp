#include "stdafx.h"
#include "AVBufferPool.h"

AVBufferPool::AVBufferPool(void)
{
}

AVBufferPool::~AVBufferPool(void)
{
	Flush();
}

void AVBufferPool::SetDataBuffer(AVDataBuffer*pAVDataBuffer)
{
	AVAutoLock l(&m_AVCritSec);

	m_listDataBuffer.push_back(pAVDataBuffer);
}

AVDataBuffer*AVBufferPool::GetDataBuffer(void)
{
	AVAutoLock l(&m_AVCritSec);

	if (m_listDataBuffer.size())
	{
		AVDataBuffer*pAVDataBuffer=(AVDataBuffer*)m_listDataBuffer.front();
		m_listDataBuffer.pop_front();
		return pAVDataBuffer;
	}

	return NULL;
}

int AVBufferPool::GetCount(void)
{
	AVAutoLock l(&m_AVCritSec);
	return m_listDataBuffer.size();
}

void AVBufferPool::Flush(void)
{
	AVAutoLock l(&m_AVCritSec);

	while (m_listDataBuffer.size())
	{
		AVDataBuffer*pAVDataBuffer=(AVDataBuffer*)m_listDataBuffer.front();
		m_listDataBuffer.pop_front();
		delete pAVDataBuffer;
		pAVDataBuffer=NULL;
	}
}
