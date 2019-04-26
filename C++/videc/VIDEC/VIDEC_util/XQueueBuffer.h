#ifndef __XQUEUEBUFFER_H__
#define __XQUEUEBUFFER_H__

#include "XAutoLock.h"

class XQueueBuffer
{
public:
	XQueueBuffer(int nBufferLen);
	virtual ~XQueueBuffer(void);
public:
	int Read(char*pBuffer,int nLen);
	void Write(char*pData,int nLen);
	int GetBufferedSize(void);
protected:
	int GetFilledSize(void);
	int GetFreeSize(void);
	void MovePointer(char**p,int nOffset);
protected:
	XCritSec		m_csThis;
	char*			m_pBufferHead;
	char*			m_pBufferTail;
	int				m_nBufferLen;
	char*			m_pFront;
	char*			m_pBack;
	bool			m_bEmpty;
};


#endif