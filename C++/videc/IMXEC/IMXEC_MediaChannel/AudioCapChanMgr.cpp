#include "AudioCapChanMgr.h"
#include "XUtil.h"

static AudioCapChanMgr g_AudioCapChanMgr;

VOID CALLBACK AudioCapChanMgrTimerProc(HWND hwnd,UINT uMsg,UINT_PTR idEvent,DWORD dwTime)
{
	AudioCapChanMgr::Instance().OnTimer();
}

AudioCapChanMgr::AudioCapChanMgr(void)
:m_pAudioCapChan(NULL)
,m_nTimerID(0)
,m_nAudioDevID(-1)
,m_nCodecType(X_AUDIO_CODEC_G7221_14_24)
{
	m_ulAudioID=XGenerateSSRC()+(unsigned long)this;

	m_pBackgroundMusic=NULL;
}

AudioCapChanMgr::~AudioCapChanMgr(void)
{

}

AudioCapChanMgr&AudioCapChanMgr::Instance(void)
{
	return g_AudioCapChanMgr;
}

int AudioCapChanMgr::OpenAudioCapChan(AudioCapChanUI*pAudioCapChanUI,int nDevID,X_AUDIO_CODEC_ID CodecType)
{
	m_nAudioDevID=nDevID;
	m_nCodecType=CodecType;

	if (m_pAudioCapChan==NULL)
	{
		m_ulAudioID=XGenerateSSRC()+(unsigned long)this;

		m_pAudioCapChan=new AudioCapChan();
		if (0!=m_pAudioCapChan->Open())
		{
			m_pAudioCapChan->Close();
			delete m_pAudioCapChan;
			m_pAudioCapChan=NULL;

			return -1;
		}

		if ( m_nTimerID==0)
		{
			m_nTimerID=SetTimer(NULL,100,100,AudioCapChanMgrTimerProc);
		}
	}

	if (0!=m_pAudioCapChan->OpenAudio(nDevID,CodecType,m_ulAudioID))
	{
		m_pAudioCapChan->Close();
		delete m_pAudioCapChan;
		m_pAudioCapChan=NULL;
		return -1;
	}

	m_pAudioCapChan->AddAudioCapChanUI(pAudioCapChanUI);

	if (m_pBackgroundMusic)
	{
		m_pAudioCapChan->OpenAudioMixer(NULL);
		m_pAudioCapChan->StartMixingWithPlayChan(m_pBackgroundMusic->GetPlayChan());
	}

	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			int nOldDevID=nDevID;
			X_AUDIO_CODEC_ID nOldCodecType=CodecType;

			iter->second.pAudioCapChan->IsAudioOpen(nOldDevID,nOldCodecType);
			if (nOldDevID!=nDevID)
			{
				iter->second.pAudioCapChan->OpenAudio(nDevID,nOldCodecType,iter->second.nAudioID);
			}
			iter++;
		}
	}

	return 0;
}

void AudioCapChanMgr::CloseAudioCapChan(AudioCapChanUI*pAudioCapChanUI)
{
	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->RemoveAudioCapChanUI(pAudioCapChanUI);
		if (m_pAudioCapChan->GetAudioCapChanUICount()<=0)
		{
			if (m_nTimerID!=0)
			{
				KillTimer(NULL,m_nTimerID);
				m_nTimerID=NULL;
			}

			m_pAudioCapChan->Close();
			delete m_pAudioCapChan;
			m_pAudioCapChan=NULL;
		}
	}
}

int AudioCapChanMgr::SetVolume(int nVolume)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->SetVolume(nVolume);
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		return m_pAudioCapChan->SetVolume(nVolume);
	}

	return -1;
}

int AudioCapChanMgr::GetVolume()
{
	if (m_pAudioCapChan!=NULL)
	{
		return m_pAudioCapChan->GetVolume();
	}

	return 0;
}

int AudioCapChanMgr::GetLevel(int&nLevel)
{
	if (m_pAudioCapChan!=NULL)
	{
		return m_pAudioCapChan->GetLevel(nLevel);
	}

	nLevel=0;
	return -1;
}

int AudioCapChanMgr::GetAudioID(unsigned long&nAudioID)
{
	nAudioID=m_ulAudioID;
	return 0;
}

bool AudioCapChanMgr::IsAudioOpen(void)
{
	return (m_pAudioCapChan!=NULL);
}


void AudioCapChanMgr::OnTimer(void)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->OnTimer();
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->OnTimer();
	}
}


/******************************************************************************
* GetAudioPacketStatistics
*描述：获取音频的包的统计数据
*输入：	无
*输出： ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int AudioCapChanMgr::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	if (m_pAudioCapChan!=NULL)
	{
		return m_pAudioCapChan->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}

	nBitrate=0;
	ulTotalPackets=0;
	ulLostPackets=0;
	AvgPacketLossRate=0.00;
	CurPacketLossRate=0.00;

	return -1;
}

int AudioCapChanMgr::OpenMixingAudio(unsigned long nPlayAudioID,AudioCapChanUI*pAudioCapChanUI,AUDEC_PlayChan*pPlayChan,X_AUDIO_CODEC_ID nCodecID)
{
	MapMixingAudio::iterator iter=m_MapMixingAudio.find(nPlayAudioID);
	if (iter!=m_MapMixingAudio.end())
	{
		AudioCapChan*pAudioCapChan=iter->second.pAudioCapChan;
		pAudioCapChan->AddAudioCapChanUI(pAudioCapChanUI);

		pAudioCapChan->OpenAudio(m_nAudioDevID,nCodecID,pAudioCapChan->GetAudioID());

		return 0;
	}

	AudioCapChan*pAudioCapChan=new AudioCapChan();
	if (0!=pAudioCapChan->Open())
	{
		pAudioCapChan->Close();
		delete pAudioCapChan;
		pAudioCapChan=NULL;

		return -1;
	}

	unsigned long nAudioID=0;
	if (0!=pAudioCapChan->OpenAudio(m_nAudioDevID,nCodecID,nAudioID))
	{
		pAudioCapChan->Close();
		delete pAudioCapChan;
		pAudioCapChan=NULL;
		return -1;
	}

	pAudioCapChan->AddAudioCapChanUI(pAudioCapChanUI);

	MIXING_AUDIO tMixingAudio;
	tMixingAudio.pAudioCapChan=pAudioCapChan;
	tMixingAudio.nAudioID=pAudioCapChan->GetAudioID();

	m_MapMixingAudio[nPlayAudioID]=tMixingAudio;

	pAudioCapChan->OpenAudioMixer(pPlayChan);

	return 0;
}

void AudioCapChanMgr::CloseMixingAudio(unsigned long nPlayAudioID,AudioCapChanUI*pAudioCapChanUI)
{
	MapMixingAudio::iterator iter=m_MapMixingAudio.find(nPlayAudioID);
	if (iter!=m_MapMixingAudio.end())
	{
		MIXING_AUDIO tMixingAudio=iter->second;

		AudioCapChan*pAudioCapChan=tMixingAudio.pAudioCapChan;
		pAudioCapChan->RemoveAudioCapChanUI(pAudioCapChanUI);
		if (pAudioCapChan->GetAudioCapChanUICount()<=0)
		{
			pAudioCapChan->Close();
			delete pAudioCapChan;
			pAudioCapChan=NULL;

			m_MapMixingAudio.erase(iter);
		}
	}
}

void AudioCapChanMgr::ExcludePlayChan(unsigned long nPlayAudioID,AUDEC_PlayChan*pPlayChan)
{
	MapMixingAudio::iterator iter=m_MapMixingAudio.find(nPlayAudioID);
	if (iter!=m_MapMixingAudio.end())
	{
		iter->second.pAudioCapChan->ExcludePlayChan(pPlayChan);
	}
}

int AudioCapChanMgr::GetMixingAudioID(unsigned long nPlayAudioID,unsigned long&nMixingAudioID)
{
	MapMixingAudio::iterator iter=m_MapMixingAudio.find(nPlayAudioID);
	if (iter!=m_MapMixingAudio.end())
	{
		MIXING_AUDIO tMixingAudio=iter->second;
		nMixingAudioID=tMixingAudio.nAudioID;
		return 0;
	}

	nMixingAudioID=0;
	return -1;
}

void AudioCapChanMgr::StartMixingWithPlayChan(AUDEC_PlayChan*pPlayChan)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->StartMixingWithPlayChan(pPlayChan);
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->StartMixingWithPlayChan(pPlayChan);
	}
}

void AudioCapChanMgr::StopMixingWithPlayChan(AUDEC_PlayChan*pPlayChan)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->StopMixingWithPlayChan(pPlayChan);
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->StopMixingWithPlayChan(pPlayChan);
	}
}

void AudioCapChanMgr::StartMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->StartMixingWithXCapChanExternal(pXCapChanExternal);
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->StartMixingWithXCapChanExternal(pXCapChanExternal);
	}
}

void AudioCapChanMgr::StopMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal)
{
	{
		MapMixingAudio::iterator iter=m_MapMixingAudio.begin();
		while (iter!=m_MapMixingAudio.end())
		{
			iter->second.pAudioCapChan->StopMixingWithXCapChanExternal(pXCapChanExternal);
			iter++;
		}
	}

	if (m_pAudioCapChan!=NULL)
	{
		m_pAudioCapChan->StopMixingWithXCapChanExternal(pXCapChanExternal);
	}
}

BackgroundMusic*AudioCapChanMgr::CreateBackgroundMusic(IMXEC_BackgroundMusicCallback&rCallback)
{
	if (m_pBackgroundMusic)
	{
		return NULL;
	}

	m_pBackgroundMusic=new BackgroundMusic(rCallback);
	return m_pBackgroundMusic;
}

void AudioCapChanMgr::OnBackgroundMusicOpen(BackgroundMusic*pBackgroundMusic)
{
	if (pBackgroundMusic==m_pBackgroundMusic)
	{
		if (m_pAudioCapChan)
		{
			m_pAudioCapChan->OpenAudioMixer(NULL);
			m_pAudioCapChan->StartMixingWithPlayChan(pBackgroundMusic->GetPlayChan());
		}
	}
}

void AudioCapChanMgr::OnBackgroundMusicClose(BackgroundMusic*pBackgroundMusic)
{
	if (pBackgroundMusic==m_pBackgroundMusic)
	{
		if (m_pAudioCapChan)
		{
			m_pAudioCapChan->StopMixingWithPlayChan(pBackgroundMusic->GetPlayChan());
		}
		m_pBackgroundMusic=NULL;
	}
}