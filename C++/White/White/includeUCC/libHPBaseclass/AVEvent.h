// AVEvent.h: interface for the AVEvent class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVEVENT_H__
#define __AVEVENT_H__


class AFX_EXT_CLASS AVEvent  
{
public:
    AVEvent(bool bManualReset = false);
	~AVEvent();
public:
	/*
	Sets the event to available (signaled) and releases any waiting threads.	
	*/
    void Set(void);
	/*
	wait to be set or timeout,ulTimeout in millionsecond,if return true,event is set,otherwise timeout
	*/
    bool Wait(unsigned long ulTimeout);
	/*
	Sets the event to unavailable (nonsignaled).
	*/
    void Reset(void);
protected:
#ifdef WIN32
	HANDLE m_hEvent;
#else
	pthread_mutex_t m_mutex;
	pthread_cond_t	m_cond;
	int				m_nEvent;
	bool			m_bManualReset;
#endif
};

#endif 