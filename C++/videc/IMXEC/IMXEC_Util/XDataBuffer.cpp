// XDataBuffer.cpp: implementation of the XDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#ifdef WIN32
#include <winsock2.h>
#endif

#include "XDataBuffer.h"
#include "XUtil.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XDataBuffer::XDataBuffer(int nExtraLen)
{
	m_pData=NULL;
	m_nLen=0;
	m_nBufferSize=0;

	m_nExtraLen=nExtraLen;
	if (m_nExtraLen<0)
	{
		m_nExtraLen=0;
	}
}

XDataBuffer::~XDataBuffer()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData=NULL;
	}
}

void XDataBuffer::Release(void)
{
	if (m_pData)
	{
		free(m_pData);
		m_pData=NULL;
	}

	delete this;
}

bool XDataBuffer::StoreData(char*pData,int nLen)
{
	m_nLen=0;

	if (pData == NULL ||
		nLen<=0)
	{
		return true;
	}

	if (m_pData && nLen>m_nBufferSize)
	{
		free(m_pData);
		m_pData=NULL;

	}

	if (m_pData==NULL)
	{
		m_nBufferSize=nLen;
		m_pData=(char*)malloc(m_nBufferSize+m_nExtraLen);
		if (m_pData==NULL)
			return false;
	}

	memcpy(m_pData,pData,nLen);
	m_nLen=nLen;

	return true;
}