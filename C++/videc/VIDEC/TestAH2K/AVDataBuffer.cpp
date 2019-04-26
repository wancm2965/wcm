// AVDataBuffer.cpp: implementation of the AVDataBuffer class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVDataBuffer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AVDataBuffer::AVDataBuffer()
{
	m_pData=NULL;
	m_nLen=0;
	m_nBufferSize=0;
}

AVDataBuffer::~AVDataBuffer()
{
	if (m_pData)
	{
		free(m_pData);
		m_pData=NULL;
	}
}


bool AVDataBuffer::StoreData(char*pData,int nLen)
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
		m_pData=(char*)malloc(m_nBufferSize);
		if (m_pData==NULL)
			return false;
	}

	memcpy(m_pData,pData,nLen);
	m_nLen=nLen;

	return true;
}