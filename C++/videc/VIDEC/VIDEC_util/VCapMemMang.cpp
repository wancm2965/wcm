#include "VCapMemMang.h"
#include "Windows.h"
#include "XUtil.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"


VIDEC_MemMang* VIDEC_MemMang::Create()
{
	NEWRECORD3( CVCapMemMang* ,pMemMang , new CVCapMemMang(FRAME_SIZE_LEN, FRAME_COUNT) )
	NULLReturnRet( pMemMang , pMemMang )  
	if (pMemMang->MallocMemory())
	{
		delete pMemMang;
		pMemMang = NULL;
		DebugOutput( logcoreerror ,"VIDEC_MemMang malloc failed!\n");
	}
	return pMemMang;
}

CVCapMemMang::CVCapMemMang(int nSize, int nCount)
: m_nSize(nSize)
, m_nCount(nCount)
{
	for (int i = 0; i < m_nCount; i++)
	{
		m_pMem[i] = NULL;
	}
}

CVCapMemMang::~CVCapMemMang(void)
{
	for (int i = 0; i < m_nCount; i++)
	{
		if (m_pMem[i])
		{
			delete m_pMem[i];
			m_pMem[i] = NULL;
		}
	}
}

int CVCapMemMang::MallocMemory()
{
	//for (int i = 0; i < m_nCount; i++)
	//{
	//	NEWRECORD( m_pMem[i] , new unsigned char[m_nSize + 1] )
	//	//NULLReturnRet( m_pMem[i] , 1 )  
	//	if (m_pMem[i])
	//		SetMemCanUse(m_pMem[i] + 1);
	//	else
	//		return 1;
	//}
	return 0;
}

void CVCapMemMang::SetMemNotUse(unsigned char* pMem)
{
	*pMem = 1;
}

unsigned char* CVCapMemMang::GetMemory()
{
	unsigned char* pMem = NULL;
	for (int i = 0; i < m_nCount; i++)
	{
		if (m_pMem[i] && *(m_pMem[i]) == 0)
		{
			pMem = m_pMem[i];
			break;
		}
	}
	return pMem;
}