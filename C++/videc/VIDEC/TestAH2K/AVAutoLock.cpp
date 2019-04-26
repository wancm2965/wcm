// AutoLock.cpp: implementation of the AutoLock class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AVAutoLock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AVAutoLock::AVAutoLock(AVCritSec*pAVCritSec)
{
	m_pAVCritSec=pAVCritSec;
	m_pAVCritSec->Lock();
}

AVAutoLock::~AVAutoLock()
{
	m_pAVCritSec->Unlock();
}
