#include "SDDev.h"


SDDev::SDDev(unsigned int nDevIndex)
:m_nDevIndex(nDevIndex)

,m_nWidth(0)
,m_nHeight(0)
,m_nFrameRate(15)

,m_bAutoFrameRate(false)
{

}

SDDev::~SDDev(void)
{

}

bool SDDev::CanBeClosed(void)
{
	int nRefCount=0;
	nRefCount+=XOutputStreamMgr::GetOutputStreamCount();
	nRefCount+=XOutputImageMgr::GetOutputImageCount();

	return nRefCount==0;
}

