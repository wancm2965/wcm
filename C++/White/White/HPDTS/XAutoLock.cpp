// AutoLock.cpp: implementation of the AutoLock class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "XAutoLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XAutoLock::XAutoLock(XCritSec&rXCritSec)
:m_rXCritSec(rXCritSec)
{
	m_rXCritSec.Lock();
}

XAutoLock::~XAutoLock()
{
	m_rXCritSec.UnLock();
}
