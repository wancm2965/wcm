#pragma once
#include "AVThread.h"
#include "AVBufferPool.h"

class YUVWriter 
	: public AVThread
	, public AVBufferPool
{
public:
	YUVWriter(void);
	virtual ~YUVWriter(void);
public:
	int Open(const char*cszPathName);
	void Close(void);
	int Write(const void*pData,int nLen);
protected:
	void ThreadProcMain(void);
protected:
	FILE*				m_pFileYUV;
	bool				m_bRunning;
};
