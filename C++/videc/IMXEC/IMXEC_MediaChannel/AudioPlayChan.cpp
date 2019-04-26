#include "AudioPlayChan.h"
#include "../IMXEC_Call/CallMgr.h"
#include "AudioPlayChanMgr.h"

//Add WebRTC by zys 20160929
#pragma comment(lib,"Strmiids.lib") 
#pragma comment(lib,"Dmoguids.lib")  
#pragma comment(lib,"wmcodecdspuuid.lib") 
#pragma comment(lib,"Msdmo.lib")

VOICE_ENGINE_CODEC_ID g_emAudioCodecID = CODEC_G7221C_48K;   //云帧项目使用 CODEC_PCMA

AudioPlayChan::AudioPlayChan(void)
:m_pMediaReceiver(NULL)
,m_pAudPlayChan(NULL)
,m_pWebRTCAudio(NULL)
,m_nAudioID(0)
,m_nWebRTCPlayChan(-1)
,m_nRTPTimestamp(0)
,m_nRTPSeqNo(0)
{
	m_pnVideoSynTime=NULL;
	m_pnAudioSynTime=NULL;
	m_nSynFlag=1;
#if USE_WEBRTC
	m_nAudioMixInChannel = AudioPlayChanMgr::Instance().m_pAudioMixer->CreateInputChannel();

	m_nAudioMixChanInChannel = AudioPlayChanMgr::Instance().m_pAudioMixerChan->CreateInputChannel();
#endif
}

AudioPlayChan::~AudioPlayChan(void)
{
#if USE_WEBRTC
	AudioPlayChanMgr::Instance().m_pAudioMixer->RemoveInputChannel(m_nAudioMixInChannel);

	AudioPlayChanMgr::Instance().m_pAudioMixerChan->RemoveInputChannel(m_nAudioMixChanInChannel);
#endif
}


int AudioPlayChan::Open(unsigned long nAudChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU)
{
	m_nAudioID=nAudChanID;

	StopAudio();

	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->Close();
		delete m_pMediaReceiver;
		m_pMediaReceiver=NULL;
	}

	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	//if (NULL == m_pWebRTCAudio)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();
		m_pWebRTCAudio->Init();
	}

	if ( -1 == m_nWebRTCPlayChan)
	{
		m_nWebRTCPlayChan = m_pWebRTCAudio->CreateDecodeChannel();
		m_pWebRTCAudio->StartPlayout(m_nWebRTCPlayChan);
		AudioPlayChanMgr::Instance().m_pAudioMixer->StartInputMixing(m_nAudioMixInChannel);

		AudioPlayChanMgr::Instance().m_pAudioMixerChan->StartInputMixing(m_nAudioMixChanInChannel);
	}
	
#else
	if (m_pAudPlayChan==NULL)
	{
		AUDEC_GetEngine().AUDEC_CreatePlayChan(&m_pAudPlayChan);
		if (m_pAudPlayChan!=NULL)
		{
			if (0!=m_pAudPlayChan->Connect(m_pnVideoSynTime,m_pnAudioSynTime))
			{
				m_pAudPlayChan->ReleaseConnections();
				delete m_pAudPlayChan;
				m_pAudPlayChan=NULL;
			}
			else
			{
				m_pAudPlayChan->SetRecordSamples(this);
			}
		}
	}
#endif
	

	if ((cszPeerNodeID==NULL || strlen(cszPeerNodeID)==0) && 
		(cszPeerNATIP==NULL || strlen(cszPeerNATIP)==0) && 
		(cszPeerLocalIP==NULL || strlen(cszPeerLocalIP)==0) && 
		nPeerLocalPort==0 &&
		(cszPeerMCUID==NULL || strlen(cszPeerMCUID)==0) && 
		(cszPeerMCUIP==NULL || strlen(cszPeerMCUIP)==0) && 
		nPeerMCUPort==0)
	{
		return 0;
	}

	m_pMediaReceiver=NETEC_MediaReceiver::Create(*this);
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}

	if (nForceUseMCU!=0)
	{
		if (0!=m_pMediaReceiver->Open(cszPeerNodeID,"",0,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort))
		{
			m_pMediaReceiver->Close();
			delete m_pMediaReceiver;
			m_pMediaReceiver=NULL;

			return -1;
		}
	}
	else
	{
		if (0!=m_pMediaReceiver->Open(cszPeerNodeID,cszPeerNATIP,nPeerNATPort,cszPeerLocalIP,nPeerLocalPort,cszPeerMCUID,cszPeerMCUIP,nPeerMCUPort))
		{
			m_pMediaReceiver->Close();
			delete m_pMediaReceiver;
			m_pMediaReceiver=NULL;

			return -1;
		}
	}

	m_pMediaReceiver->SetAudioID(nAudChanID);

	CallMgr::Instance().ExcludePlayChan(nAudChanID,m_pAudPlayChan);

	return 0;
}

void AudioPlayChan::Close(void)
{
	CallMgr::Instance().ExcludePlayChan(m_nAudioID,NULL);

	StopAudio();

	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->Close();
		delete m_pMediaReceiver;
		m_pMediaReceiver=NULL;
	}

	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		if ( -1 != m_nWebRTCPlayChan)
		{
			m_pWebRTCAudio->StopPlayout(m_nWebRTCPlayChan);
			m_pWebRTCAudio->DestroyDecodeChannel(m_nWebRTCPlayChan);
			AudioPlayChanMgr::Instance().m_pAudioMixer->StopInputMixing(m_nAudioMixInChannel);

			AudioPlayChanMgr::Instance().m_pAudioMixerChan->StopInputMixing(m_nAudioMixChanInChannel);
			m_nWebRTCPlayChan = -1;
		}

		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}
#else
	if (NULL!=m_pAudPlayChan)
	{
		m_pAudPlayChan->ReleaseConnections();
		delete m_pAudPlayChan;
		m_pAudPlayChan=NULL;
	}
#endif
	
}

int AudioPlayChan::SetSynFlag(int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime)
{
	m_pnVideoSynTime=pnVideoSynTime;
	m_pnAudioSynTime=pnAudioSynTime;

	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->SetSynFlag(nSynFlag,m_pnVideoSynTime,m_pnAudioSynTime);
	}

	return 0;
}

unsigned long*AudioPlayChan::GetVideoSynTime(void)
{
	return m_pnVideoSynTime;
}	

unsigned long*AudioPlayChan::GetAudioSynTime(void)
{
	return m_pnAudioSynTime;
}


int AudioPlayChan::SetVolume(int nVolume)
{
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		return m_pWebRTCAudio->SetSpeakerVolume(nVolume*255/100);
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		return m_pAudPlayChan->SetVolume(nVolume);
	}
#endif

	return -1;
}

int AudioPlayChan::GetVolume()
{
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		int nValue = m_pWebRTCAudio->GetSpeakerVolume();
		int nTemp = nValue*100%255;
		nValue = nValue*100/255;
		if (nTemp != 0)
		{
			nValue++;
		}
		return nValue;
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		return m_pAudPlayChan->GetVolume();
	}
#endif

	return 0;
}

int AudioPlayChan::GetLevel(int&nLevel)
{
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		nLevel = (((int)m_pWebRTCAudio->GetSpeakerSpeechDBValue())*10/96)+10;

		if (nLevel<0)
		{
			nLevel=0;
			return -1;
		}
		return 0;
	}

#else
	if (m_pAudPlayChan!=NULL)
	{
		nLevel=m_pAudPlayChan->GetLevel();
		if (nLevel<0)
		{
			nLevel=0;
			return -1;
		}
		return 0;
	}
#endif

	nLevel=0;
	return -1;
}

void AudioPlayChan::SetMute(bool bMute)
{
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		m_pWebRTCAudio->SetSpeakerMute(bMute);
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->SetMute(bMute?1:0);
	}
#endif
}

int AudioPlayChan::StartAudio(void)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}

	return m_pMediaReceiver->StartAudio();
}

int AudioPlayChan::EnableAudio(int nEnable/*0:不接收，1:接收*/)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	int nEnableAudio=0;
	{
		XAutoLock l(m_csMapAudioPlayChanUI);
		MapAudioPlayChanUI::iterator iter=m_MapAudioPlayChanUI.begin();
		while (iter!=m_MapAudioPlayChanUI.end())
		{
			int nTempEnable=0;
			iter->second->OnAudioPlayChanUIGetAudioEnable(nTempEnable);
			++iter;

			nEnableAudio|=nTempEnable;
		}
	}

	return m_pMediaReceiver->EnableAudio(nEnableAudio);
}

int AudioPlayChan::StopAudio(void)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	m_pMediaReceiver->StopAudio();

	return 0;
}

int AudioPlayChan::SetAudioCast(bool bAudioCast)
{
	if (m_pMediaReceiver==NULL)
	{
		return -1;
	}
	m_pMediaReceiver->SetAudioCast(bAudioCast);

	return 0;
}


/******************************************************************************
* PlayAudioData
*描述：播放音频数据
*输入：	pData			-数据指针
		nLen			-数据长度
*输出： 无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int AudioPlayChan::PlayAudioData(const void*pData,int nLen)
{
	if (m_pMediaReceiver!=NULL)
	{
		return -1;
	}

	OnNETEC_MediaReceiverCallbackAudioPacket((unsigned char*)pData,nLen);

	return 0;
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
int AudioPlayChan::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	nBitrate=0;
 #if USE_WEBRTC
 	if ((NULL != m_pWebRTCAudio) && (-1 != m_nWebRTCPlayChan))
 	{ 
 		m_pWebRTCAudio->EnableStatistics(true);
 		nBitrate=m_pWebRTCAudio->GetStatistics(m_nWebRTCPlayChan);
 	}
 #else
	if (m_pAudPlayChan!=NULL)
	{
		nBitrate=m_pAudPlayChan->GetBitrate();
	}
#endif

	if (m_pMediaReceiver!=NULL)
	{
		m_pMediaReceiver->GetAudioPacketStatistics((uint32_t &)ulTotalPackets,(uint32_t &)ulLostPackets,AvgPacketLossRate,CurPacketLossRate);
	}
	else
	{
		ulTotalPackets=0;
		ulLostPackets=0;
		AvgPacketLossRate=0.00;
		CurPacketLossRate=0.00;
	}
	
	return 0;
}

/******************************************************************************
* OnNETEC_MediaReceiverCallbackAudioPacket
*描述：接收到音频数据包的回调
*输入：	pData			-数据包指针
		nLen			-数据包长度
*输出： 无
*返回值：无
*****************************************************************************/
void AudioPlayChan::OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen)
{
	//Add WebRTC by zys 20160929
#if USE_WEBRTC
	if ( (NULL != m_pWebRTCAudio) && (-1 !=m_nWebRTCPlayChan) )
	{
		unsigned char pRTPPacket[1500];
		int nRTPPacketLen = AudecHeaderToRTPHeader(pRTPPacket,pData,nLen);

		m_pWebRTCAudio->Decode(m_nWebRTCPlayChan, pRTPPacket, nRTPPacketLen);

		AudioPlayChanMgr::Instance().m_pAudioMixer->AddRtpData(m_nAudioMixInChannel, (const char *)pRTPPacket, nRTPPacketLen);

		AudioPlayChanMgr::Instance().m_pAudioMixerChan->AddRtpData(m_nAudioMixChanInChannel, (const char *)pRTPPacket, nRTPPacketLen);
	}
#else
	if (m_pAudPlayChan!=NULL)
	{
		m_pAudPlayChan->Write(pData,nLen);
	}
#endif

	{
		bool bLoop=true;
		unsigned long nRefCount=0;
		XAutoLock l(m_csMapAudioPlayChanUI);
		MapAudioPlayChanUI::iterator iter=m_MapAudioPlayChanUI.begin();
		while (iter!=m_MapAudioPlayChanUI.end())
		{
			AudioPlayChanUI*pAudioPlayChanUI=iter->second;
			int nEnable=0;
			pAudioPlayChanUI->OnAudioPlayChanUIGetAudioEnable(nEnable);

			if (nEnable!=0)
			{
				pAudioPlayChanUI->OnAudioPlayChanUIAudioPacket(pData,nLen,nRefCount++,bLoop);
				if (bLoop==false)
				{
					break;
				}
				else
				{
					++iter;
				}
			}
			else
			{
				++iter;
			}
		}
	}
}


void AudioPlayChan::OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	{
		XAutoLock l(m_csMapAudioPlayChanUI);
		MapAudioPlayChanUI::iterator iter=m_MapAudioPlayChanUI.begin();
		while (iter!=m_MapAudioPlayChanUI.end())
		{
			AudioPlayChanUI*pAudioPlayChanUI=iter->second;
			int nEnable=0;
			pAudioPlayChanUI->OnAudioPlayChanUIGetAudioEnable(nEnable);
			if (nEnable!=0)
			{
				pAudioPlayChanUI->OnAudioPlayChanUIRecordSamples(pSamples,nSamples,nSampleRate,nTimestamp);
			}
			++iter;
		}
	}
}


void AudioPlayChan::AddAudioPlayChanUI(AudioPlayChanUI*pAudioPlayChanUI)
{
	XAutoLock l(m_csMapAudioPlayChanUI);
	m_MapAudioPlayChanUI[pAudioPlayChanUI]=pAudioPlayChanUI;

	//char szDebug[1024];
	//sprintf(szDebug,"AudioPlayChan::AddAudioPlayChanUI %u Count=%u pAudioPlayChanUI=%u\n",(unsigned long)this,m_MapAudioPlayChanUI.size(),(unsigned long)pAudioPlayChanUI);
	//OutputDebugString(szDebug);
}

void AudioPlayChan::RemoveAudioPlayChanUI(AudioPlayChanUI*pAudioPlayChanUI)
{
	{
		XAutoLock l(m_csMapAudioPlayChanUI);
		m_MapAudioPlayChanUI.erase(pAudioPlayChanUI);

		//char szDebug[1024];
		//sprintf(szDebug,"AudioPlayChan::RemoveAudioPlayChanUI %u Count=%u pAudioPlayChanUI=%u\n",(unsigned long)this,m_MapAudioPlayChanUI.size(),(unsigned long)pAudioPlayChanUI);
		//OutputDebugString(szDebug);

		MapAudioPlayChanUI::iterator iter=m_MapAudioPlayChanUI.begin();
		if (iter!=m_MapAudioPlayChanUI.end())
		{
			unsigned long*pnVideoSynTime=NULL;
			unsigned long*pnAudioSynTime=NULL;
			int nSynFlag=0;
			pAudioPlayChanUI->OnAudioPlayChanUIGetSynFlag(nSynFlag,&pnVideoSynTime,&pnAudioSynTime);
			if (pnVideoSynTime==GetVideoSynTime() ||
				pnAudioSynTime==GetAudioSynTime())
			{
				AudioPlayChanUI*pFirstAudioPlayChanUI=iter->second;

				pFirstAudioPlayChanUI->OnAudioPlayChanUIGetSynFlag(nSynFlag,&pnVideoSynTime,&pnAudioSynTime);
				SetSynFlag(nSynFlag,pnVideoSynTime,pnAudioSynTime);
			}
		}
	}

	EnableAudio(1);
}

int AudioPlayChan::GetAudioPlayChanUICount(void)
{
	XAutoLock l(m_csMapAudioPlayChanUI);
	return m_MapAudioPlayChanUI.size();
}

AUDEC_PlayChan*AudioPlayChan::GetPlayChan(int nMixerScreenIndex,bool bMixing)
{
	if (nMixerScreenIndex<0)
		return m_pAudPlayChan;

	bool bOtherAudioCast=false;
	bool bThisAudioCast=false;

	MapAudioPlayChanUI::iterator iter=m_MapAudioPlayChanUI.begin();
	while (iter!=m_MapAudioPlayChanUI.end())
	{
		AudioPlayChanUI*pAudioPlayChanUI=iter->second;
		int nScreenIndex=0;
		bool bAudioCast=false;
		pAudioPlayChanUI->OnAudioPlayChanUIGetScreenIndex(nScreenIndex);
		pAudioPlayChanUI->OnAudioPlayChanUIGetAudioCast(bAudioCast);
		if (nScreenIndex==nMixerScreenIndex)
		{
			if (bAudioCast)
			{
				bThisAudioCast=true;
			}
		}
		else
		{
			if (bAudioCast)
			{
				bOtherAudioCast=true;
			}
		}

		++iter;
	}

	if (bMixing)
	{
		if (bThisAudioCast && bOtherAudioCast==false)
		{
			return m_pAudPlayChan;
		}
	}
	else
	{
		if (bThisAudioCast==false)
		{
			return m_pAudPlayChan;
		}
	}

	return NULL;
}

//Add WebRTC by zys 20160929
int AudioPlayChan::AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen)
{
	X_AUDIO_CODEC_ID nCodecID = (X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pAudecData);
	*m_pnAudioSynTime = AUDEC_HEADER_GET_TIMESTAMP(pAudecData); //唇音同步

	std::string strCodecName="gsm";
	int nSampleRate=8000;
	int nBitrate=13200;
	int nPayloadType=0;
	int nSamplesPerFrame=160;

	switch (nCodecID)
	{
	case X_AUDIO_CODEC_G711U:
		{
			strCodecName="PCMU";
			nSampleRate=8000;
			nBitrate=64000;
			nPayloadType=0;
			nSamplesPerFrame=160;
		}
		break;
	case X_AUDIO_CODEC_G711A:
		{
			strCodecName="PCMA";
			nSampleRate=8000;
			nBitrate=64000;
			nPayloadType=8;
			nSamplesPerFrame=160;
		}
		break;
	case X_AUDIO_CODEC_G722_64:
		{
			strCodecName="G722";
			nSampleRate=16000;
			nBitrate=64000;
			nPayloadType=9;
			nSamplesPerFrame=160;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_7_24://16khz
	case X_AUDIO_CODEC_G7221_7_24://16khz
		{
			strCodecName="g7221";
			nSampleRate=16000;
			nBitrate=24000;
			nPayloadType=121;
			nSamplesPerFrame=320;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_7_32://16khz
	case X_AUDIO_CODEC_G7221_7_32://16khz
		{
			strCodecName="g7221";
			nSampleRate=16000;
			nBitrate=32000;
			nPayloadType=122;
			nSamplesPerFrame=320;
		}
		break;


	case X_AUDIO_CODEC_SIREN_G7221_14_24:
	case X_AUDIO_CODEC_G7221_14_24://32khz
		{
			strCodecName="g7221";
			nSampleRate=32000;
			nBitrate=24000;
			nPayloadType=123;
			nSamplesPerFrame=640;
		}
		break;

	case X_AUDIO_CODEC_SIREN_G7221_14_32:
	case X_AUDIO_CODEC_G7221_14_32:
		{
			strCodecName="g7221";
			nSampleRate=32000;
			nBitrate=32000;
			nPayloadType=124;
			nSamplesPerFrame=640;
		}
		break;
	case X_AUDIO_CODEC_SIREN_G7221_14_48:
	case X_AUDIO_CODEC_G7221_14_48:
		{
			strCodecName = "g7221";
			nSampleRate = 32000;
			nBitrate = 48000;
			nPayloadType = 125;
			nSamplesPerFrame = 640;
		}
		break;
	case X_AUDIO_CODEC_OPUS_FWB_48:
		{
			strCodecName = "opus";
			nSampleRate = 48000;
			nBitrate = 64000;
			nPayloadType = 120;
			nSamplesPerFrame = 960;
		}
		break;


	default:
		return -1;
	};


	int nHeaderLen=AUDEC_HEADER_GET_LEN(pAudecData);
	int nPayloadLen=nAudecLen-nHeaderLen;
	if (nPayloadLen<=0 || (nPayloadLen+RTP_HEADER_LEN)>1500)
	{
		return -1;
	}

	unsigned short nSeqNo=AUDEC_HEADER_GET_SEQUENCE(pAudecData);

	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pRTPData;
	memset(pRTPHeader,0,RTP_HEADER_LEN);
	pRTPHeader->version=RTP_VERSION;
	pRTPHeader->type=nPayloadType;
	pRTPHeader->marker=AUDEC_HEADER_GET_SYN_POINT(pAudecData);
	pRTPHeader->timestamp=htonl(m_nRTPTimestamp);
	pRTPHeader->ssrc=htonl((unsigned long)this);
	pRTPHeader->sequence=htons(m_nRTPSeqNo++);
	m_nRTPTimestamp+=nSamplesPerFrame;

	memcpy(pRTPData+RTP_HEADER_LEN,(unsigned char*)pAudecData+nHeaderLen,nPayloadLen);

	return nPayloadLen+RTP_HEADER_LEN;
}