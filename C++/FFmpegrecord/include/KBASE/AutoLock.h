/* KAutoLock.h */

#pragma once

#include "KBASE/KBASE_Export.h"
#include "CritSec.h"

//---------------------------------------------------------------------------------------
class KBASE_API KAutoLock  
{
public:
	KAutoLock(KCritSec &rCritSec);
	~KAutoLock();

protected:
    KCritSec& m_rCritSec;
};

