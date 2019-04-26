#include <winsock2.h>
#include <mmsystem.h>
#include "RTTimer.h"

FrameRateCtrl::FrameRateCtrl(void)
{
	m_nPeriod=1;
	m_bRunning=false;
	m_nTimerTotal=0;
	m_nStepIndex=0;
	m_nStepCount=0;
	SetFrameRate(30);
	m_hSemaphore = CreateSemaphore(NULL,0,0xffff,NULL);//初时值为0
}

FrameRateCtrl::~FrameRateCtrl(void)
{
	CloseHandle(m_hSemaphore);
	m_hSemaphore = NULL;
}

int FrameRateCtrl::Open(unsigned int nFrameRate)
{
	if (m_bRunning)
	{
		return 0;
	}

	if (m_hSemaphore==NULL)
	{
		return -1;
	}

	RTTimer::Instance().AddFrameRateCtrl(this);

	m_nPeriod=RTTimer::Instance().GetPeriod();

	SetFrameRate(nFrameRate);
	m_bRunning=true;
	if (!StartThread())
	{
		m_bRunning=false;
		return -1;
	}


	return 0;
}

void FrameRateCtrl::Close(void)
{
	RTTimer::Instance().RemoveFrameRateCtrl(this);

	m_bRunning=false;
	if (m_hSemaphore)
	{
		ReleaseSemaphore(m_hSemaphore,1,NULL);
	}
	WaitForStop();
}

void FrameRateCtrl::SetFrameRate(unsigned int nFrameRate)
{
	if (nFrameRate<=1)
	{
		nFrameRate=1;
	}
	else if (nFrameRate<=5)
	{
		nFrameRate=5;
	}
	else if (nFrameRate<=10)
	{
		nFrameRate=10;
	}
	else if (nFrameRate<=15)
	{
		nFrameRate=15;
	}
	else if (nFrameRate<=25)
	{
		nFrameRate=25;
	}
	else
	{
		nFrameRate=30;
	}

	m_nFrameRate=nFrameRate;
	m_nFrameInterval=1000/m_nFrameRate;

	if (m_nPeriod==1 && nFrameRate==30)
	{
		m_nStepCount=2;
	}
	else
	{
		m_nStepCount=0;
	}
	m_nStepIndex=0;
}

unsigned int FrameRateCtrl::GetFrameRate(void)
{
	return m_nFrameRate;
}

void FrameRateCtrl::OnTimer(void)
{
	if (m_hSemaphore)
	{
		ReleaseSemaphore(m_hSemaphore,1,NULL);
	}
}

void FrameRateCtrl::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		DWORD dwRet = WaitForSingleObject(m_hSemaphore, INFINITE);
		switch (dwRet)
		{
		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
			{
			}
			break;
		case WAIT_OBJECT_0:
			{
				m_nTimerTotal+=m_nPeriod;
				if (m_nTimerTotal>=m_nFrameInterval)
				{
					if (m_nStepCount)
					{
						m_nStepIndex++;
						if (m_nStepIndex>m_nStepCount)
						{
							if (m_nTimerTotal>=(m_nFrameInterval+1))
							{
								m_nStepIndex=0;
								OnFrameRate();
								m_nTimerTotal=0;
							}
						}
						else
						{
							OnFrameRate();
							m_nTimerTotal=0;
						}
					}
					else
					{
						OnFrameRate();
						m_nTimerTotal=0;
					}
				}
			}
			break;
		}
	}
}


static RTTimer g_RTTimer;
RTTimer&RTTimer::Instance(void)
{
	return g_RTTimer;
}

RTTimer::RTTimer(void)
{
	m_nTimerID=0;
	m_nPeriod=0;
}

RTTimer::~RTTimer(void)
{

}

int RTTimer::Start(void)
{
	if (m_nTimerID)
	{
		return 0;
	}

	TIMECAPS	 tTimeCaps;
	unsigned int nInterval;
	unsigned int nPeriod;
	unsigned int nTimerType;
	MMRESULT nResult;

	// 取得系统定时器的能力
	nResult = timeGetDevCaps( &tTimeCaps, sizeof(TIMECAPS) );
	if( nResult != TIMERR_NOERROR )
	{
		return -1;
	}

	nInterval = 1;
	if( nInterval < tTimeCaps.wPeriodMin )
	{
		nInterval = tTimeCaps.wPeriodMin;
	}
	if( nInterval > tTimeCaps.wPeriodMax )
	{
		nInterval = tTimeCaps.wPeriodMax;
	}

	// 为我们的应用程序设置定时器的最高精度
	nPeriod = min( max(tTimeCaps.wPeriodMin,1), tTimeCaps.wPeriodMax );
	nResult = timeBeginPeriod( nPeriod );
	if( nResult != TIMERR_NOERROR )
	{
		return -1;
	}

	// 启动多媒体定时器(多媒体定时器运行在它自己的线程中)
	nTimerType = 0;//TIME_ONESHOT//TIME_PERIODIC
	nTimerType |= TIME_PERIODIC;
	nTimerType |= TIME_CALLBACK_FUNCTION;
	nResult = timeSetEvent(nInterval,nPeriod,(LPTIMECALLBACK)RTTimmerFunc,(DWORD)this,nTimerType);
	if( nResult == NULL )
	{
		DWORD dwLastError = GetLastError();

		nResult = timeEndPeriod( nPeriod );
		if( nResult != TIMERR_NOERROR )
		{
		}

		return -1;
	}

	m_nTimerID = nResult;
	m_nPeriod = nPeriod;

	return 0;
}

void RTTimer::Stop(void)
{
	if ( m_nTimerID )
	{
		timeKillEvent( m_nTimerID );
		m_nTimerID = 0;

	}

	if ( m_nPeriod )
	{
		timeEndPeriod( m_nPeriod );
		m_nPeriod = 0;
	}
}

unsigned int RTTimer::GetPeriod(void)
{
	return m_nPeriod;
}

void RTTimer::OnTimer(void)
{
	XAutoLock l(m_csMapFrameRateCtrl);
	MapFrameRateCtrl::iterator iter;
	for (iter=m_MapFrameRateCtrl.begin();iter!=m_MapFrameRateCtrl.end();++iter)
	{
		FrameRateCtrl*pFrameRateCtrl=iter->second;
		pFrameRateCtrl->OnTimer();
	}
}

void RTTimer::AddFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl)
{
	{
		XAutoLock l(m_csMapFrameRateCtrl);
		m_MapFrameRateCtrl[pFrameRateCtrl]=pFrameRateCtrl;
	}

	RTTimer::Instance().Start();
}

void RTTimer::RemoveFrameRateCtrl(FrameRateCtrl*pFrameRateCtrl)
{
	{
		XAutoLock l(m_csMapFrameRateCtrl);
		m_MapFrameRateCtrl.erase(pFrameRateCtrl);
	}
	if (m_MapFrameRateCtrl.size()==0)
	{
		RTTimer::Instance().Stop();
	}
}



unsigned int __stdcall RTTimer::RTTimmerFunc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	RTTimer*pRTTimer=(RTTimer*)dwUser;
	pRTTimer->OnTimer();
	return 0;
}
