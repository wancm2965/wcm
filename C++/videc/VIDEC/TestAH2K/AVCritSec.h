// AVCritSec.h: interface for the AVCritSec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVCRITSEC_H__
#define __AVCRITSEC_H__



class AVCritSec  
{
public:
	AVCritSec();
	virtual ~AVCritSec();
public:
    void Lock(void);
    void Unlock(void);
protected:
#ifdef WIN32
    CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t		m_pMutex;
#endif
};

#endif 