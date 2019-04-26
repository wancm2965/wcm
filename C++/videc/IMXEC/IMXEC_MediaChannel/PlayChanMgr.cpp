#include "PlayChanMgr.h"

static PlayChanMgr g_PlayChanMgr;

VOID CALLBACK PlayChanMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	PlayChanMgr::Instance().OnTimer();
}

PlayChanMgr::PlayChanMgr(void)
:m_nTotalKBytes(0)
,m_nTotalBytes(0)
,m_nTimerID(0)
,m_bForcibleReceiving(0)
{

}

PlayChanMgr::~PlayChanMgr(void)
{

}

PlayChanMgr&PlayChanMgr::Instance(void)
{
	return g_PlayChanMgr;
}

void PlayChanMgr::AddPlayChan(PlayChan*pPlayChan)
{
	if (m_MapPlayChan.size()==0 && m_nTimerID==0)
	{
		m_nTimerID=SetTimer(NULL,100,100,PlayChanMgrTimerProc);
	}
	m_MapPlayChan[pPlayChan]=pPlayChan;
}

void PlayChanMgr::RemovePlayChan(PlayChan*pPlayChan)
{
	GetTotalBytes();

	m_MapPlayChan.erase(pPlayChan);

	if (m_MapPlayChan.size()==0 && m_nTimerID!=0)
	{
		KillTimer(NULL,m_nTimerID);
		m_nTimerID=NULL;
	}
}

void PlayChanMgr::OnTimer(void)
{
	MapPlayChan::iterator iter=m_MapPlayChan.begin();
	while (iter!=m_MapPlayChan.end())
	{
		PlayChan*pPlayChan=iter->second;
		++iter;

		pPlayChan->OnTimer();
	}
}

void PlayChanMgr::ApplyMixerScreenIndexChanged(int nMixerScreenIndex)
{
	MapPlayChan::iterator iter=m_MapPlayChan.begin();
	while (iter!=m_MapPlayChan.end())
	{
		PlayChan*pPlayChan=iter->second;
		++iter;

		pPlayChan->ApplyMixerScreenIndexChanged(nMixerScreenIndex);
	}

}

unsigned long PlayChanMgr::GetTotalKBytes(void)
{
	GetTotalBytes();

	return m_nTotalKBytes;
}

unsigned long PlayChanMgr::GetTotalBytes(void)
{
	unsigned long nTotalBytes=0;

	MapPlayChan::iterator iter=m_MapPlayChan.begin();
	while (iter!=m_MapPlayChan.end())
	{
		nTotalBytes+=iter->second->DoGetTotalBytes();
		++iter;
	}

	m_XBitrateTotal.Calculate(nTotalBytes);


	m_nTotalBytes+=nTotalBytes;
	if (m_nTotalBytes>1024)
	{
		m_nTotalKBytes+=(m_nTotalBytes/1024);
		m_nTotalBytes=m_nTotalBytes%1024;
	}

	return 0;
}

int PlayChanMgr::GetBitrate(void)
{
	GetTotalBytes();

	return m_XBitrateTotal.GetBitrate();
}


void PlayChanMgr::ClearTotalBytes(void)
{
	m_nTotalKBytes=0;
	m_nTotalBytes=0;
}

void PlayChanMgr::SetForcibleReceiving(bool bForcible)
{
	m_bForcibleReceiving=bForcible;
}

bool PlayChanMgr::GetForcibleReceiving(void)
{
	return m_bForcibleReceiving;
}