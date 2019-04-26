// XAutoLock.h: interface for the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XAUTOLOCK_H__
#define __XAUTOLOCK_H__

#include "XCritSec.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

class XAutoLock  
{
public:
	XAutoLock(XCritSec&rXCritSec);
	~XAutoLock(void);
protected:
    XCritSec& m_rXCritSec;
};

#endif 