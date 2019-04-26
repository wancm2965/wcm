//RTTimer.h
#ifndef __RTTIMER_H__
#define __RTTIMER_H__

#include <map>
#include "XAutoLock.h"
#include "XThreadBase.h"

class FrameRateCtrl
	: public XThreadBase
{
public:
	FrameRateCtrl(void);
	virtual~FrameRateCtrl(void);
public:
	virtual int Open(unsigned int nFrameRate);
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
