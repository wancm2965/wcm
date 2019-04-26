// XDataBuffer.cpp: implementation of the XDataBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "XDataBuffer.h"


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