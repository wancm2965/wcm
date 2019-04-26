#include "AudioPlayChanMgr.h"

static AudioPlayChanMgr* g_AudioPlayChanMgr = NULL;
AudioPlayChanMgr::AudioPlayChanMgr(void)
{
#if USE_WEBRTC
	m_pAudioMixer = AudioMixerAPI::CreateMixer();
	m_pAudioMixer->Init();
	m_nAudioMixOut = m_pAudioMixer->CreateOutputChannel(CODEC_PCM_48K, this);

	m_pAudioMixerChan = AudioMixerAPI::CreateMixer();
	m_pAudioMixerChan->Init();
	m_nAudioMixChanOut = m_pAudioMixerChan->CreateOutputChannel(MIX_CODEC_PCMA, this);
#endif
}

AudioPlayChanMgr::~AudioPlayChanMgr(void)
{

}

AudioPlayChanMgr&AudioPlayChanMgr::Instance(void)
{
	if (!g_AudioPlayChanMgr)
	{
		g_AudioPlayChanMgr = new AudioPlayChanMgr;
	}
	return *g_AudioPlayChanMgr;
}

int AudioPlayChanMgr::OpenAudioPlayChan(AudioPlayChanUI*pAudioPlayChanUI,unsigned long nAudChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU,int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime)
{
	AudioPlayChan*pAudioPlayChan=NULL;
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		pAudioPlayChan=iter->second;
	}
	else
	{
		pAudioPlayChan=new AudioPlayChan();
		pAudioPlayChan->SetSynFlag(nSynFlag,pnVideoSynTime,pnAudioSynTime);
		if (0!=pAudioPlayChan->Open(nAudChanID,cszPeerNodeID,cszPeerNATIP,nPeerNATPort,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort,nForceUseMCU))
		{
			pAudioPlayChan->Close();
			delete pAudioPlayChan;
			pAudioPlayChan=NULL;

			return -1;
		}

		pAudioPlayChan->StartAudio();

		{
			XAutoLock l(m_csMapAudioPlayChan);
			m_MapAudioPlayChan[nAudChanID]=pAudioPlayChan;
		}
	}

	pAudioPlayChan->AddAudioPlayChanUI(pAudioPlayChanUI);
	return 0;
}

void AudioPlayChanMgr::CloseAudioPlayChan(AudioPlayChanUI*pAudioPlayChanUI,unsigned long nAudChanID)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		AudioPlayChan*pAudioPlayChan=iter->second;
		pAudioPlayChan->RemoveAudioPlayChanUI(pAudioPlayChanUI);
		if (pAudioPlayChan->GetAudioPlayChanUICount()<=0)
		{
			{
				XAutoLock l(m_csMapAudioPlayChan);
				m_MapAudioPlayChan.erase(iter);
			}

			pAudioPlayChan->Close();
			delete pAudioPlayChan;
			pAudioPlayChan=NULL;
		}
	}		
}

int AudioPlayChanMgr::SetSynFlag(unsigned long nAudChanID,int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->SetSynFlag(nSynFlag,pnVideoSynTime,pnAudioSynTime);
	}

	return -1;
}

int AudioPlayChanMgr::SetVolume(unsigned long nAudChanID,int nVolume)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->SetVolume(nVolume);
	}

	return -1;
}

int AudioPlayChanMgr::GetVolume(unsigned long nAudChanID)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->GetVolume();
	}

	return 0;
}

int AudioPlayChanMgr::GetLevel(unsigned long nAudChanID,int&nLevel)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->GetLevel(nLevel);
	}

	nLevel=0;
	return 0;
}

void AudioPlayChanMgr::SetMute(unsigned long nAudChanID, bool bMute)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->SetMute(bMute);
	}
}

int AudioPlayChanMgr::EnableAudio(unsigned long nAudChanID,int nEnable/*0:不接收，1:接收*/)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->EnableAudio(nEnable);
	}

	return -1;
}

int AudioPlayChanMgr::SetAudioCast(unsigned long nAudChanID,bool bAudioCast)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->SetAudioCast(bAudioCast);
	}

	return -1;
}

AUDEC_PlayChan*AudioPlayChanMgr::GetPlayChan(unsigned long nAudChanID,int nMixerScreenIndex,bool bMixing)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->GetPlayChan(nMixerScreenIndex,bMixing);
	}

	return NULL;
}


int AudioPlayChanMgr::GetAudioPacketStatistics(unsigned long nAudChanID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->GetAudioPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
	}

	return -1;
}

int AudioPlayChanMgr::PlayAudioData(unsigned long nAudChanID,const void*pData,int nLen)
{
	XAutoLock l(m_csMapAudioPlayChan);
	MapAudioPlayChan::iterator iter=m_MapAudioPlayChan.find(nAudChanID);
	if (iter!=m_MapAudioPlayChan.end())
	{
		return iter->second->PlayAudioData(pData,nLen);
	}

	return -1;
}

int AudioPlayChanMgr::OnMixedData (int out_channel, const char* rtp_data, int len)
{
	if (m_nAudioMixOut == out_channel)
	{
		XAutoLock l(m_csAVIWriter);
		std::vector<AudioMixer_Callback*>::iterator itr = m_arrAudioMixerCB.begin();
		while(itr != m_arrAudioMixerCB.end())
		{
			(*itr)->OnMixedData(out_channel, rtp_data, len);
			itr++;
		}
	}
	else if (m_nAudioMixChanOut == out_channel)
	{
		XAutoLock l(m_csAudioMixerChan);
		std::vector<AudioMixer_Callback*>::iterator itr = m_arrAudioMixerChanCB.begin();
		while(itr != m_arrAudioMixerChanCB.end())
		{
			(*itr)->OnMixedData(out_channel, rtp_data, len);
			itr++;
		}
	}
	return 0;
}

void AudioPlayChanMgr::AddAudioMixerCB(AudioMixer_Callback* pAudioMixerCallback)
{
	XAutoLock l(m_csAVIWriter);
	m_arrAudioMixerCB.push_back(pAudioMixerCallback);
	if (m_arrAudioMixerCB.size() == 1)
	{
		m_pAudioMixer->StartOutputMixing(m_nAudioMixOut);
	}
}

void AudioPlayChanMgr::RemoveAudioMixerCB(AudioMixer_Callback* pAudioMixerCallback)
{
	XAutoLock l(m_csAVIWriter);
	std::vector<AudioMixer_Callback*>::iterator itr = m_arrAudioMixerCB.begin();
	while(itr != m_arrAudioMixerCB.end())
	{
		if (*itr == pAudioMixerCallback)
		{
			m_arrAudioMixerCB.erase(itr);
			break;
		}
		itr++;
	}
	if (m_arrAudioMixerCB.size() == 0)
	{
		m_pAudioMixer->StopOutputMixing(m_nAudioMixOut);
	}
}

void AudioPlayChanMgr::AddAudioMixerChanCB(AudioMixer_Callback* pAudioMixerCallback)
{
	XAutoLock l(m_csAudioMixerChan);
	m_arrAudioMixerChanCB.push_back(pAudioMixerCallback);
	if (m_arrAudioMixerChanCB.size() == 1)
	{
		m_pAudioMixerChan->StartOutputMixing(m_nAudioMixChanOut);
	}
}

void AudioPlayChanMgr::RemoveAudioMixerChanCB(AudioMixer_Callback* pAudioMixerCallback)
{
	XAutoLock l(m_csAudioMixerChan);
	std::vector<AudioMixer_Callback*>::iterator itr = m_arrAudioMixerChanCB.begin();
	while(itr != m_arrAudioMixerChanCB.end())
	{
		if (*itr == pAudioMixerCallback)
		{
			m_arrAudioMixerChanCB.erase(itr);
			break;
		}
		itr++;
	}
	if (m_arrAudioMixerChanCB.size() == 0)
	{
		m_pAudioMixerChan->StopOutputMixing(m_nAudioMixChanOut);
	}
}
