#include "XBufferPool.h"

XBufferPool::XBufferPool(void)
{
}

XBufferPool::~XBufferPool(void)
{
	Flush();
}

void XBufferPool::SetDataBuffer(XDataBuffer*pXDataBuffer)
{
	XAutoLock l(m_csListDataBuffer);

	m_listDataBuffer.push_back(pXDataBuffer);
}

XDataBuffer*XBufferPool::GetDataBuffer(void)
{
	XAutoLock l(m_csListDataBuffer);

	if (m_listDataBuffer.size())
	{
		XDataBuffer*pXDataBuffer=(XDataBuffer*)m_listDataBuffer.front();
		m_listDataBuffer.pop_front();
		return pXDataBuffer;
	}

	return NULL;
}

int XBufferPool::GetCount(void)
{
	XAutoLock l(m_csListDataBuffer);
	return m_listDataBuffer.size();
}

void XBufferPool::Flush(void)
{
	XAutoLock l(m_csListDataBuffer);

	while (m_listDataBuffer.size())
	{
		XDataBuffer*pXDataBuffer=(XDataBuffer*)m_listDataBuffer.front();
		m_listDataBuffer.pop_front();
		pXDataBuffer->Release();
		pXDataBuffer=NULL;
	}
}
