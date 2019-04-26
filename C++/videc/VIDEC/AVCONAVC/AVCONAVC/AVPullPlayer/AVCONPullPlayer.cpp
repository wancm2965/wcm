#include "stdafx.h"
#include "AVCONPullPlayer.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"

AVCONAVC_API IAVPullPlayer*CreateAVPullPlayer(HWND hWnd,unsigned long ulUserMsg,const char*cszFileName,int nType,DWORD dwOffset)
{
	NEWRECORD3( AVCONPullPlayer*,pAVCONPullPlayer , new AVCONPullPlayer()  )
	if (pAVCONPullPlayer->Connect(hWnd,ulUserMsg,cszFileName)==false)
	{
		pAVCONPullPlayer->ReleaseConnections();
		delete pAVCONPullPlayer;
		pAVCONPullPlayer=NULL;
	}

	return pAVCONPullPlayer;
}

AVCONPullPlayer::AVCONPullPlayer(void)
{
	m_pMediaFile=NULL;
	m_nVolume=100;
	m_bMute=false;
	m_bPlaying=false;
	m_hWndNotify=0;
	m_ulUserMsg=0;
}

AVCONPullPlayer::~AVCONPullPlayer(void)
{

}

bool AVCONPullPlayer::Connect(HWND hWnd,unsigned long ulUserMsg,const char*cszFileName)
{
	m_hWndNotify=hWnd;
	m_ulUserMsg=ulUserMsg;

	m_pMediaFile=AVCONAVC_MediaFile::Create(*this);
	if (m_pMediaFile==NULL)
	{
		return false;
	}

	bool bTryEVR=false;
	if (0!=m_pMediaFile->Open(hWnd,cszFileName,bTryEVR))
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;

		return false;
	}

	m_bPlaying=true;
	if (m_hWndNotify && m_ulUserMsg)
	{
		::PostMessage(m_hWndNotify,m_ulUserMsg,AVPULL_PLAYER_ON_PLAY,(LPARAM)0);
	}
	return true;
}

void AVCONPullPlayer::ReleaseConnections(void)
{
	if (m_pMediaFile)
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;
	}
}

bool AVCONPullPlayer::HasVideo(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->HasAudio();
	}

	return false;
}

int	AVCONPullPlayer::GetWidth(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetVideoWidth();
	}

	return 320;
}

int	AVCONPullPlayer::GetHeight(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetVideoHeight();
	}

	return 240;
}

bool AVCONPullPlayer::SetWindowPos(int x,int y,int cx,int cy)
{
	if (m_pMediaFile)
	{
		m_pMediaFile->SetWndPos(x,y,cx,cy);
		return true;
	}

	return false;
}

void AVCONPullPlayer::SetVideoQuality(int nVideoQuality)
{
	
}


bool AVCONPullPlayer::HasAudio(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->HasAudio();
	}

	return false;
}

void AVCONPullPlayer::SetVolume(int nVolume)
{
	m_nVolume=nVolume;
	if (m_bMute)
		return;

	if (m_pMediaFile)
	{
		m_pMediaFile->SetVolume(nVolume);
	}
}

int AVCONPullPlayer::GetVolume(void)
{
	return m_nVolume;
}

void AVCONPullPlayer::SetMute(bool bMute)
{
	m_bMute=bMute;
	if (m_pMediaFile)
	{
		if (m_bMute)
		{
			m_pMediaFile->SetVolume(0);
		}
		else
		{
			m_pMediaFile->SetVolume(m_nVolume);
		}
	}
}

bool AVCONPullPlayer::GetMute(void)
{
	return m_bMute;
}

void AVCONPullPlayer::SetBalance(int nBalance)
{

}

int AVCONPullPlayer::GetBalance(void)
{
	return 0;
}

unsigned long AVCONPullPlayer::GetDuration(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetDuration();
	}

	return 0;
}

unsigned long AVCONPullPlayer::GetPosition(void)
{
	if (m_pMediaFile)
	{
		return m_pMediaFile->GetPosition();
	}

	return 0;
}

void AVCONPullPlayer::Seek(unsigned long ulSeconds)
{
	if (m_pMediaFile)
	{
		bool bPlaying=m_bPlaying;
		if (bPlaying)
			Pause();

		m_pMediaFile->Seek(ulSeconds);

		if (bPlaying)
			Play();
	}
}

void AVCONPullPlayer::Pause(void)
{
	m_bPlaying=false;
	if (m_pMediaFile)
	{
		m_pMediaFile->Pause();
	}
	if (m_hWndNotify && m_ulUserMsg)
	{
		::PostMessage(m_hWndNotify,m_ulUserMsg,AVPULL_PLAYER_ON_PAUSE,0);
	}
}

void AVCONPullPlayer::Play(void)
{
	m_bPlaying=true;
	if (m_pMediaFile)
	{
		m_pMediaFile->Play();
	}

	if (m_hWndNotify && m_ulUserMsg)
	{
		::PostMessage(m_hWndNotify,m_ulUserMsg,AVPULL_PLAYER_ON_PLAY,(LPARAM)0);
	}
}

bool AVCONPullPlayer::IsPlaying(void)
{
	return m_bPlaying;
}