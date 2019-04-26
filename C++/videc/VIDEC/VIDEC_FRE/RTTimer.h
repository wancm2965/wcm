//RTTimer.h
#ifndef __RTTIMER_H__
#define __RTTIMER_H__

#include <map>
#include "XAutoLock.h"
#include "XThreadBase.h"

#include "VIDECFREAPI.h"

#define USE_PERFORMANCE 1

#if USE_PERFORMANCE


class XHiResTimer
{
public:
	XHiResTimer() 
	{
		m_bInit = false;
		m_ulLockCount = 0;
		m_fFPSNew = 0;
	}
	~XHiResTimer() {}

	bool IsInit()
	{
		return m_bInit;
	}

	bool Init()
	{
		if (!QueryPerformanceFrequency(&m_ticksPerSecond))
		{
			// system doesn't support hi-res timer
			m_bInit = false;
			//DebugOutput("XRTimer Init Failed!\n");	
			return false;
		}
		else
		{
			m_bInit = true;
			QueryPerformanceCounter(&m_lastTime);
			//DebugOutput("XRTimer Init Success!\n");	
			return true;
		}
	} // end Init()
	int LockFPS(unsigned char targetFPS,int& nChangeFPS)
	{
		if (!m_bInit)
		{
			return -1;
		}
		if (targetFPS == 30)
		{
			m_fFPS = 30.001;
// 			m_ulLockCount++;
// 			if (m_ulLockCount > 1800)
// 			{
// 				m_ulLockCount=0;
// 				nChangeFPS=4;
// 			}
		}
		else if (targetFPS == 60 || targetFPS < 1)
		{
			m_fFPS = 60.001;
		}
		else
		{
			m_fFPS = (double)targetFPS;
		}

#if (0)
		double	 interval = (double)(m_currentTime.QuadPart - m_currentTime2.QuadPart) / (double)m_ticksPerSecond.QuadPart;
		bool bPrint = false;
		if (targetFPS == 60)
		{
			if (interval - 0.0168 > 0.00001f || (0.0164 -  interval)   > 0.00001f)
				bPrint = true;
		}
		else if (targetFPS == 30)
		{
			if (interval - 0.038 > 0.00001f || 0.031 -  interval   > 0.00001f)
				bPrint = true;
		}
		if (bPrint)
		{
			char szDebug[1024];
			sprintf(szDebug,"Time m_fps = %0.10lf  Interval = %0.10lf m_fFPS = %0.10lf this = %u\n",m_fps,interval,m_fFPS,this);
			DebugOutput(szDebug);	
		}
		m_currentTime2 = m_currentTime;


		if (targetFPS == 60)
 		{
			m_ulLockCount++;
			if (m_fFPSNew < 1)
			{
				m_fFPSNew = 60.001;
			}			
			if (m_ulLockCount > 3600)
			{
				m_ulLockCount = 0;
				if ((m_fFPSNew - 60) > 0.0001)
				{
					m_fFPSNew = 59;//59.999;
					m_ulLockCount = 3600;
				}
				else
				{
					m_fFPSNew = 60.001;
				}
			}			
			m_fFPS = m_fFPSNew;
 		}
 		else if (targetFPS == 30)
 		{
			m_ulLockCount++;
			if (m_fFPSNew < 1)
			{
				m_fFPSNew = 30.001;
			}			
			if (m_ulLockCount > 1800)
			{
				m_ulLockCount = 0;
				if ((m_fFPSNew - 30) > 0.0001)
				{
					m_fFPSNew = 29;//29.999;
					m_ulLockCount = 1800;
				}
				else
				{
					m_fFPSNew = 30.001;
				}
			}			
			m_fFPS = m_fFPSNew;
 		}
		else
		{
			m_fFPS  = (double)targetFPS;
		}
#endif
		m_fps = 0.0l;
		unsigned long ulCurQuadPart = 0;
		do {
			QueryPerformanceCounter(&m_currentTime);			
			ulCurQuadPart = m_currentTime.QuadPart - m_lastTime.QuadPart;
			if (ulCurQuadPart > 0)
			{
				m_fps = ((double)m_ticksPerSecond.QuadPart)/((double)ulCurQuadPart);
			}
		} while ((m_fps - m_fFPS) > 0.000000001);
		m_lastTime = m_currentTime;
		return 0;
	} // end LockFPS()

private:
	bool			m_bInit;
	LARGE_INTEGER	m_lastTime;
	LARGE_INTEGER	m_currentTime;
	LARGE_INTEGER	m_currentTime2;
	double			m_fps;
	double			m_fFPS;
	double			m_fFPSNew;
	LARGE_INTEGER   m_ticksPerSecond;
	unsigned long	m_ulLockCount;
};


class FrameRateCtrl
	: public XThreadBase
{
public:
	FrameRateCtrl(void);
	virtual~FrameRateCtrl(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void SetFrameRate(unsigned int nFrameRate);
	virtual unsigned int GetFrameRate(void);
	virtual void OnFrameRate(void){};
	virtual void SetPause(bool bPause);
public:
	static unsigned int __stdcall RTTimmerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

protected:
	virtual void ThreadProcMain(void);
	virtual void OnTimer(void);

protected:
	bool			m_bRunning;
	unsigned int	m_nFrameRate;
	bool			m_bPause;
	int				m_nStepCount;
	int				m_nFrameInterval;
	int				m_nStepFrameInterval;
	UINT			m_uTimeid;
	HANDLE			m_hEvent;
	XHiResTimer		m_hirestiemr;
};

#else
class FrameRateCtrl
	: public XThreadBase
{
public:
	FrameRateCtrl(void);
	virtual~FrameRateCtrl(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual void SetFrameRate(unsigned int nFrameRate);
	virtual unsigned int GetFrameRate(void);
	virtual void OnTimer(void);
	virtual void OnFrameRate(void){};
protected:
	virtual void ThreadProcMain(void);
protected:
	bool			m_bRunning;
	unsigned int	m_nFrameRate;
	unsigned int	m_nPeriod;
	unsigned int	m_nFrameInterval;
	unsigned int	m_nTimerTotal;
	unsigned int	m_nStepIndex;
	unsigned int	m_nStepCount;

	HANDLE			m_hSemaphore;
};

class RTTimer
{
public:
	RTTimer(void);
	virtual~RTTimer(void);
public:
	static RTTimer&Instance(void);
public:
	int Start(void);
	void Stop(void);
	unsigned int GetPeriod(void);
	void OnTimer(void);

	void AddFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl);
	void RemoveFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl);
public:
	static unsigned int __stdcall RTTimmerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
protected:
	unsigned int		m_nTimerID;
	unsigned int		m_nPeriod;

	typedef std::map<void*,FrameRateCtrl*>	MapFrameRateCtrl;

	XCritSec			m_csMapFrameRateCtrl;
	MapFrameRateCtrl	m_MapFrameRateCtrl;
};


#endif


class FrameRateEventCtrl
	: public FrameRateCtrl
{
public:
	FrameRateEventCtrl(void){}
	virtual~FrameRateEventCtrl(void){}
public:
	void AddFrameRateEvent(FrameRateEvent*pFrameRateEvent)
	{
		XAutoLock l(m_csMapFrameRateEvent);
		m_MapFrameRateEvent[pFrameRateEvent]=pFrameRateEvent;
	}
	void RemoveFrameRateEvent(FrameRateEvent*pFrameRateEvent)
	{
		XAutoLock l(m_csMapFrameRateEvent);
		m_MapFrameRateEvent.erase(pFrameRateEvent);
	}
	UINT32 GetEventCount(void)
	{
		XAutoLock l(m_csMapFrameRateEvent);
		return m_MapFrameRateEvent.size();
	}
protected:
	virtual void OnFrameRate(void)
	{
		XAutoLock l(m_csMapFrameRateEvent);
		MapFrameRateEvent::iterator iter;
		for (iter=m_MapFrameRateEvent.begin();iter!=m_MapFrameRateEvent.end();++iter)
		{
			FrameRateEvent*pFrameRateEvent=iter->second;
			pFrameRateEvent->OnEvent();
		}
	}
protected:
	typedef std::map<void*,FrameRateEvent*>	MapFrameRateEvent;

	XCritSec			m_csMapFrameRateEvent;
	MapFrameRateEvent	m_MapFrameRateEvent;
};

#endif
