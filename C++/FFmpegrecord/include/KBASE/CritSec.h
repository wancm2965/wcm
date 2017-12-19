#pragma once
#include "KBASE/KBASE_Export.h"
#ifdef WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

//---------------------------------------------------------------------------------------
//�ٽ�����:���������߳��ٽ���
class KBASE_API KCritSec  
{
public:
	KCritSec();
	virtual ~KCritSec();

	// �����ٽ���
    void Lock(void);

	// �����ٽ���
    void Unlock(void);

protected:

#ifdef WIN32
    CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t m_pMutex;
#endif
};
