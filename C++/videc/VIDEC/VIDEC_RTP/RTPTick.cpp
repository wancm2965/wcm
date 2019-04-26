#include "RTPTick.h"
#include "XUtil.h"



class RTPTickMgr
	: public XThreadBase
{
public:
	RTPTickMgr(void);
	virtual~RTPTickMgr(void);
protected:
	virtual int Start(void);
	virtual void Stop(void);

	virtual void AddRTPTick(RTPTick*pRTPTick);
	virtual void RemoveRTPTick(RTPTick*pRTPTick);

	void ThreadProcMain(void);
	bool DoTick(void);
protected:
	typedef std::map<void*,RTPTick*>	MapRTPTick;

	bool		m_bRunning;
	MapRTPTick	m_MapRTPTick;
	XCritSec	m_csMapRTPTick;

	friend class RTPTick;
};


static RTPTickMgr g_RTPTickMgr;

RTPTick::RTPTick(void)
{
}

RTPTick::~RTPTick(void)
{
}

int RTPTick::Open(void)
{
	g_RTPTickMgr.AddRTPTick(this);
	return 0;
}

void RTPTick::Close(void)
{
	g_RTPTickMgr.RemoveRTPTick(this);
}


RTPTickMgr::RTPTickMgr(void)
:m_bRunning(false)
{

}

RTPTickMgr::~RTPTickMgr(void)
{

}


int RTPTickMgr::Start(void)
{
	if (!m_bRunning)
	{
		m_bRunning=true;
		if (!StartThread())
		{
			return -1;
		}
	}

	return 0;
}

void RTPTickMgr::Stop(void)
{
	if (m_bRunning)
	{
		m_bRunning=false;
		WaitForStop();
	}
}

void RTPTickMgr::AddRTPTick(RTPTick*pRTPTick)
{
	if (pRTPTick==NULL)
		return ;

	XAutoLock l(m_csMapRTPTick);
	if (m_MapRTPTick.size()==0)
	{
		Start();
	}
	m_MapRTPTick[pRTPTick]=pRTPTick;
}

void RTPTickMgr::RemoveRTPTick(RTPTick*pRTPTick)
{
	if (pRTPTick==NULL)
		return ;

	XAutoLock l(m_csMapRTPTick);
	m_MapRTPTick.erase(pRTPTick);
	if (m_MapRTPTick.size()==0)
	{
		Stop();
	}
}

void RTPTickMgr::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		if (!DoTick())
		{
			XSleep(1);
		}
	}
}

bool RTPTickMgr::DoTick(void)
{
	bool bRet=false;
	XAutoLock l(m_csMapRTPTick);
	MapRTPTick::iterator iter=m_MapRTPTick.begin();
	while (iter!=m_MapRTPTick.end())
	{
		RTPTick*pRTPTick=iter->second;
		bRet|=pRTPTick->DoTick();

		++iter;
	}
	return false;
}
