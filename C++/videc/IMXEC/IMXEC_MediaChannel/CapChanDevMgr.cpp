#include "CapChanDevMgr.h"
#include "AudioCapChanMgr.h"

static CapChanDevMgr g_CapChanDevMgr;

VOID CALLBACK CapChanDevMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	CapChanDevMgr::Instance().OnTimer();
}

CapChanDevMgr::CapChanDevMgr(void)
:m_nTimerID(0)
{
	m_bEnableAudioSend=true;
	m_bEnableVideoSend=true;
	
	m_nTotalKBytes=0;
	m_nTotalBytes=0;

	m_nFlagVideoAutoFormat=0;

	m_nSpeakerMute=0;
}

CapChanDevMgr::~CapChanDevMgr(void)
{

}

CapChanDevMgr&CapChanDevMgr::Instance(void)
{
	return g_CapChanDevMgr;
}

void CapChanDevMgr::AddCapChanDev(CapChanDev*pCapChanDev)
{
	if (m_MapCapChanDev.size()==0 && m_nTimerID==0)
	{
		m_nTimerID=SetTimer(NULL,100,100,CapChanDevMgrTimerProc);
	}
	m_MapCapChanDev[pCapChanDev]=pCapChanDev;
}

void CapChanDevMgr::ReleaseCapChanDev(CapChanDev*pCapChanDev)
{
	GetTotalBytes();

	m_MapCapChanDev.erase(pCapChanDev);

	if (m_MapCapChanDev.size()==0 && m_nTimerID!=0)
	{
		KillTimer(NULL,m_nTimerID);
		m_nTimerID=NULL;
	}
}

void CapChanDevMgr::OnTimer(void)
{
	MapCapChanDev::iterator iter=m_MapCapChanDev.begin();
	while (iter!=m_MapCapChanDev.end())
	{
		CapChanDev*pCapChanDev=iter->second;
		++iter;

		pCapChanDev->OnTimer();
	}
}

int CapChanDevMgr::GetMixerScreenIndex(void)
{
	int nMixerScreenIndex=-1;
	MapCapChanDev::iterator iter=m_MapCapChanDev.begin();
	while (iter!=m_MapCapChanDev.end())
	{
		CapChanDev*pCapChanDev=iter->second;
		nMixerScreenIndex=pCapChanDev->GetMixerScreenIndex();
		if (nMixerScreenIndex>=0)
			break;
		++iter;
	}
	return nMixerScreenIndex;
}

void CapChanDevMgr::ApplyMixerScreenIndexChanged(int nMixerScreenIndex)
{
	MapCapChanDev::iterator iter=m_MapCapChanDev.begin();
	while (iter!=m_MapCapChanDev.end())
	{
		CapChanDev*pCapChanDev=iter->second;
		++iter;

		pCapChanDev->ApplyMixerScreenIndexChanged(nMixerScreenIndex);
	}

}

void CapChanDevMgr::EnableAudioSend(bool bEnable)
{
	m_bEnableAudioSend=bEnable;
}

bool CapChanDevMgr::IsEnableAudioSend(void)
{
	return m_bEnableAudioSend;
}

void CapChanDevMgr::EnableVideoSend(bool bEnable)
{
	m_bEnableVideoSend=bEnable;
}

bool CapChanDevMgr::IsEnableVideoSend(void)
{
	return m_bEnableVideoSend;
}

unsigned long CapChanDevMgr::GetTotalKBytes(void)
{
	GetTotalBytes();

	return m_nTotalKBytes;
}

unsigned long CapChanDevMgr::GetTotalBytes(void)
{
	unsigned long nTotalBytes=0;

	MapCapChanDev::iterator iter=m_MapCapChanDev.begin();
	while (iter!=m_MapCapChanDev.end())
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

int CapChanDevMgr::GetBitrate(void)
{
	GetTotalBytes();

	return m_XBitrateTotal.GetBitrate();
}

void CapChanDevMgr::ClearTotalBytes(void)
{
	m_nTotalKBytes=0;
	m_nTotalBytes=0;
}

void CapChanDevMgr::SetVideoAutoFormat(int nFlagAuto)
{
	m_nFlagVideoAutoFormat=(nFlagAuto!=0?1:0);
}

int CapChanDevMgr::GetVideoAutoFormat(void)
{
	return m_nFlagVideoAutoFormat;
}

void CapChanDevMgr::SetSpeakerMute(int nMute)
{
	m_nSpeakerMute=nMute;

	MapCapChanDev::iterator iter=m_MapCapChanDev.begin();
	while (iter!=m_MapCapChanDev.end())
	{
		CapChanDev*pCapChanDev=iter->second;
		++iter;

		pCapChanDev->SetSpeakerMute(nMute);
	}
}

int CapChanDevMgr::GetSpeakerMute(void)
{
	return m_nSpeakerMute;
}
