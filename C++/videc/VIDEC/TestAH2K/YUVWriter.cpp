#include "StdAfx.h"
#include "YUVWriter.h"
#include "AVUtil.h"

YUVWriter::YUVWriter(void)
{
	m_pFileYUV=NULL;
	m_bRunning=false;
}

YUVWriter::~YUVWriter(void)
{
}

int YUVWriter::Open(const char*cszPathName)
{
	m_pFileYUV=fopen(cszPathName,"w+b");
	if (m_pFileYUV==NULL)
		return -1;

	m_bRunning=true;
	if (StartThread())
	{
		return 0;
	}
	return -1;
}

void YUVWriter::Close(void)
{
	m_bRunning=false;
	WaitForStop();
	if (m_pFileYUV!=NULL)
	{
		fclose(m_pFileYUV);
		m_pFileYUV=NULL;
	}
}

int YUVWriter::Write(const void*pData,int nLen)
{
	AVDataBuffer*pBuf=new AVDataBuffer();
	pBuf->StoreData((char*)pData,nLen);
	SetDataBuffer(pBuf);
	return 0;
}

void YUVWriter::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		AVDataBuffer*pBuf=GetDataBuffer();
		if (pBuf!=NULL)
		{
			if (m_pFileYUV!=NULL)
			{
				fwrite(pBuf->GetData(),1,pBuf->GetLen(),m_pFileYUV);
			}			
			delete pBuf;
			pBuf=NULL;
		}
		else
		{
			AVSleep(1);
		}
	}
}
