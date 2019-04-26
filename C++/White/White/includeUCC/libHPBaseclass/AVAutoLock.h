// AVAutoLock.h: interface for the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVAUTOLOCK_H__
#define __AVAUTOLOCK_H__
#include "AVCritSec.h"

class AFX_EXT_CLASS AVAutoLock  
{
public:
	AVAutoLock(AVCritSec*pAVCritSec);
	~AVAutoLock();
protected:
    AVCritSec * m_pAVCritSec;
};

#endif 