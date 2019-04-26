#include "AudioCapChan.h"
#include "XUtil.h"
#include "CapChanDevMgr.h"
#include <AUDEC/AUDEC_Header.h>
#include "PlayChanMgr.h"

//Add WebRTC by zys 20160929
#include "RTPHeader.h"
#pragma comment(lib,"Strmiids.lib") 
#pragma comment(lib,"Dmoguids.lib")  
#pragma comment(lib,"wmcodecdspuuid.lib") 
#pragma comment(lib,"Msdmo.lib")
//MyVoiceEngineInterface* g_pVoiceEngine;

AudioCapChan::AudioCapChan(void)
:m_pMediaSender(NULL)

,m_pAudCapChan(NULL)
,m_nAudioCapDevID(-2)
,m_nAudioCodecID(X_AUDIO_CODEC_G7221_14_24)
,m_pWebRTCAudio(NULL)
,m_nDecodChanID(0)
,m_nAudioSeqNo(0)
,m_nRTPTimestamp(0)
,m_nRTPSeqNo(0)

,m_nLastOpenTime(0)
,m_bAudioOpen(false)
,m_nVolume(10)

,m_pAudioMixer(NULL)
,m_bAudioMixerOpen(false)
,m_pExcludePlayChan(NULL)
{
#if USE_WEBRTC
	m_nAudioMixChanInChannel = AudioPlayChanMgr::Instance().m_pAudioMixerChan->CreateInputChannel();
#endif
}

AudioCapChan::~AudioCapChan(void)
{
#if USE_WEBRTC
	AudioPlayChanMgr::Instance().m_pAudioMixerChan->RemoveInputChannel(m_nAudioMixChanInChannel);
#endif
}


int AudioCapChan::Open(void)
{
	if (m_pMediaSender==NULL)
	{
		m_pMediaSender=NETEC_MediaSender::Create(*this);
		if (m_pMediaSender==NULL)
		{
			return -1;
		}

		if (0!=m_pMediaSender->Open())
		{
			m_pMediaSender->Close();
			delete m_pMediaSender;
			m_pMediaSender=NULL;

			return -1;
		}
	}

	return 0;
}

void AudioCapChan::Close(void)
{
	CloseAudio();

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->Close();
		delete m_pMediaSender;
		m_pMediaSender=NULL;
	}
}


int AudioCapChan::SetVolume(int nVolume)
{
	m_nVolume=nVolume;

#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		return m_pWebRTCAudio->SetMicVolume(nVolume*255/100);
	}
#else
	if (m_pAudCapChan!=NULL)
	{
		return m_pAudCapChan->SetVolume(nVolume);
	}
#endif

	return -1;
}

int AudioCapChan::GetVolume()
{
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		int nValue = m_pWebRTCAudio->GetMicVolume();
		int nTemp = nValue*100%255;
		nValue = nValue*100/255;
		if (nTemp != 0)
		{
			nValue++;
		}
		return nValue;
	}
#else
	if (m_pAudCapChan!=NULL)
	{
		return m_pAudCapChan->GetVolume();
	}
#endif

	return 0;
}

int AudioCapChan::GetLevel(int&nLevel)
{
	//WebRTC
#if USE_WEBRTC
	if (NULL != m_pWebRTCAudio)
	{
		nLevel = (((int)m_pWebRTCAudio->GetMicSpeechDBValue())*10/96)+10;

		if (nLevel<0)
		{
			nLevel=0;
			return -1;
		}
		return 0;
	}

#else
	if (m_pAudCapChan!=NULL)
	{
		nLevel=m_pAudCapChan->GetLevel();
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

unsigned long AudioCapChan::GetAudioID(void)
{
	unsigned long nAudioID=0;
	if (m_pMediaSender!=NULL)
	{
		if (0!=m_pMediaSender->GetAudioID((uint32_t &)nAudioID))
		{
			nAudioID=0;
		}
	}
	return nAudioID;
}

//打开音频通道
//nDevID:音频采集设备号,>=-1从声卡中采集音频
//CodecType:编码类型
int AudioCapChan::OpenAudio(int nDevID,X_AUDIO_CODEC_ID CodecType,unsigned long nAudioID)
{
	if (nDevID<-1)
	{
		CloseAudio();
		return false;
	}

	m_nAudioCapDevID=nDevID;
	m_nAudioCodecID=CodecType;

	m_nLastOpenTime=XGetTimestamp();
	
	//Add WebRTC by zys 20160929
#if USE_WEBRTC

	//if (NULL == m_pWebRTCAudio)
	{
		m_pWebRTCAudio = MyVoiceEngineInterface::GetVoiceEngine();

		//g_pVoiceEngine = m_pWebRTCAudio;

		//Trace TraceIpcLog;
 		//VoiceEngineTrace::SetTraceCallback(&TraceIpcLog);
 		//VoiceEngineTrace::SetTraceFilter(LogFilter);

		m_pWebRTCAudio->Init();

		//g_telnet.Init();

// 		if (!IPCLIB_IsInitd())
// 		{
// 			IPCLIB_Init(false, 2015, "", "admin", "avcon");
// 			char str[256];
// 			sprintf(str,"init IPC!!!!!!!!!!!!!\n");
// 			OutputDebugString(str);
// 		}
// 		IPCLIB_RegistModule("IMXEC.dll");
	}

	m_pWebRTCAudio->SetSendCodec (g_emAudioCodecID);
	m_pWebRTCAudio->RegisterAudioCallback(this);
	m_pWebRTCAudio->StartRecording();

	AudioPlayChanMgr::Instance().m_pAudioMixerChan->StartInputMixing(m_nAudioMixChanInChannel);

// 	m_pWebRTCAudio->SetAgcStatus(true, kAgcFixedDigital);
// 	AgcConfig config = {0, 0, true};
// 	m_pWebRTCAudio->SetAgcConfig(config);
// 
// 	m_pWebRTCAudio->SetNsStatus(true, kNsModerateSuppression);
// 	m_pWebRTCAudio->SetNsConfig(1.f, 0.25f, 1);

// 	m_pWebRTCAudio->SetAgcStatus(AgcStat.AgcEnable, AgcStat.AgcMode);
// 	AgcConfig config = {AgcCfg.targetLeveldBOv, AgcCfg.digitalCompressionGaindB, AgcCfg.limiterEnable};
// 	m_pWebRTCAudio->SetAgcConfig(config);
// 
// 	m_pWebRTCAudio->SetNsStatus(NsStat.NsEnable, NsStat.NsMode);
// 	m_pWebRTCAudio->SetNsConfig(NsCfg.NsOverdive, NsCfg.NsDenoiseband, NsCfg.NsGainmap);


	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->StartAudio(nAudioID);
	}


#else
	if (m_pAudCapChan==NULL)
	{
		AUDEC_GetEngine().AUDEC_SetMicDev(nDevID);
		AUDEC_GetEngine().AUDEC_CreateCapChan(&m_pAudCapChan);
		if (m_pAudCapChan==NULL)
		{
			return -1;
		}
		AUDEC_CodecInst codecInst;
		if (0!=AUDEC_Engine::AUDEC_GetCodecByID(CodecType,&codecInst))
		{
			m_pAudCapChan->ReleaseConnections();
			delete m_pAudCapChan;
			m_pAudCapChan=NULL;

			return -1;
		}
		if (0!=m_pAudCapChan->Connect(&codecInst))
		{
			m_pAudCapChan->ReleaseConnections();
			delete m_pAudCapChan;
			m_pAudCapChan=NULL;

			return -1;
		}
		m_pAudCapChan->AddStreamOut(this);
		m_pAudCapChan->SetRecordSamples(this);
		m_pAudCapChan->StartCapture();
		m_pAudCapChan->SetVolume(m_nVolume);

		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->StartAudio(nAudioID);
		}

		if (m_bAudioMixerOpen && m_pAudioMixer==NULL)
		{
			m_pAudioMixer=AUDEC_Mixer::Create();
			if (m_pAudioMixer!=NULL)
			{
				if (0!=m_pAudioMixer->Open(m_pAudCapChan))
				{
					m_pAudioMixer->Close();
					delete m_pAudioMixer;
					m_pAudioMixer=NULL;
				}
				else 
				{
					if (m_pExcludePlayChan)
						m_pAudioMixer->ExcludePlayChan(m_pExcludePlayChan);

					int nMixerScreenIndex=GetMixerScreenIndex();
					if (nMixerScreenIndex>=0)
					{
						CapChanDevMgr::Instance().ApplyMixerScreenIndexChanged(nMixerScreenIndex);
						PlayChanMgr::Instance().ApplyMixerScreenIndexChanged(nMixerScreenIndex);
					}
				}
			}
		}
	}
	else
	{
		AUDEC_GetEngine().AUDEC_SetMicDev(nDevID);
		AUDEC_CodecInst codecInst;
		if (0!=AUDEC_Engine::AUDEC_GetCodecByID(CodecType,&codecInst))
		{
			if (m_pAudioMixer!=NULL)
			{
				m_pAudioMixer->Close();
				delete m_pAudioMixer;
				m_pAudioMixer=NULL;
			}

			m_pAudCapChan->ReleaseConnections();
			delete m_pAudCapChan;
			m_pAudCapChan=NULL;

			return -1;
		}
		if (0!=m_pAudCapChan->SetCodec(&codecInst))
		{
			if (m_pAudioMixer!=NULL)
			{
				m_pAudioMixer->Close();
				delete m_pAudioMixer;
				m_pAudioMixer=NULL;
			}

			m_pAudCapChan->ReleaseConnections();
			delete m_pAudCapChan;
			m_pAudCapChan=NULL;

			return -1;
		}
		else
		{
			int nVAD = AUDEC_GetEngine().AUDEC_GetVAD();
			AUDEC_GetEngine().AUDEC_SetVAD(nVAD);
		}
	}
#endif

	m_bAudioOpen=true;

	return 0;
}

/******************************************************************************
* IsAudioOpen
*描述：获取是否已经打开音频
*输入：无
*输出：	nDevID		-音频采集设备号,>=-1从声卡中采集音频
		CodecType	-编码类型
*返回值：为true时已经打开音频，否则没有打开
*****************************************************************************/
bool AudioCapChan::IsAudioOpen(int&nDevID,X_AUDIO_CODEC_ID&CodecType)
{
	nDevID=m_nAudioCapDevID;
	CodecType=m_nAudioCodecID;

	return m_bAudioOpen;
}

//关闭音频通道
void AudioCapChan::CloseAudio(void)
{
	m_bAudioOpen=false;

	//Add WebRTC by zys 20160929
#if USE_WEBRTC

	if (NULL != m_pWebRTCAudio)
	{		

		m_pWebRTCAudio->StopRecording();
		m_pWebRTCAudio->DeRegisterAudioCallback();
		
		AudioPlayChanMgr::Instance().m_pAudioMixerChan->StopInputMixing(m_nAudioMixChanInChannel);
		//MyVoiceEngineInterface::DestroyVoiceEngine(m_pWebRTCAudio);
		//m_pWebRTCAudio = NULL;
	}

#else
	if (m_pAudioMixer!=NULL)
	{
		m_pAudioMixer->Close();
		delete m_pAudioMixer;
		m_pAudioMixer=NULL;
	}

	if (m_pAudCapChan!=NULL)
	{
		m_pAudCapChan->ReleaseConnections();
		delete m_pAudCapChan;
		m_pAudCapChan=NULL;
	}
#endif

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->StopAudio();
	}
}


int AudioCapChan::OpenAudioMixer(AUDEC_PlayChan*pPlayChan)
{
	m_bAudioMixerOpen=true;
	m_pExcludePlayChan=pPlayChan;

	if (m_bAudioMixerOpen && m_pAudioMixer==NULL && m_pAudCapChan!=NULL)
	{
		m_pAudioMixer=AUDEC_Mixer::Create();
		if (m_pAudioMixer!=NULL)
		{
			if (0!=m_pAudioMixer->Open(m_pAudCapChan))
			{
				m_pAudioMixer->Close();
				delete m_pAudioMixer;
				m_pAudioMixer=NULL;
			}
			else
			{
				int nMixerScreenIndex=GetMixerScreenIndex();
				if (nMixerScreenIndex>=0)
				{
					PlayChanMgr::Instance().ApplyMixerScreenIndexChanged(nMixerScreenIndex);
					CapChanDevMgr::Instance().ApplyMixerScreenIndexChanged(nMixerScreenIndex);
				}
			}
		}
	}
	
	if (m_pAudioMixer && m_pExcludePlayChan)
	{
		m_pAudioMixer->ExcludePlayChan(m_pExcludePlayChan);
	}

	return (m_pAudioMixer==NULL?-1:0);
}

void AudioCapChan::ExcludePlayChan(AUDEC_PlayChan*pPlayChan)
{
	m_pExcludePlayChan=pPlayChan;
	if (m_pAudioMixer && m_pExcludePlayChan)
	{
		m_pAudioMixer->ExcludePlayChan(m_pExcludePlayChan);
	}
}

void AudioCapChan::StartMixingWithPlayChan(AUDEC_PlayChan*pPlayChan)
{
	if (m_pAudioMixer)
	{
		m_pAudioMixer->StartMixingWithPlayChan(pPlayChan);
	}
}

void AudioCapChan::StopMixingWithPlayChan(AUDEC_PlayChan*pPlayChan)
{
	if (m_pAudioMixer)
	{
		m_pAudioMixer->StopMixingWithPlayChan(pPlayChan);
	}
}

void AudioCapChan::StartMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal)
{
	if (m_pAudioMixer)
	{
		m_pAudioMixer->StartMixingWithXCapChanExternal(pXCapChanExternal);
	}
}

void AudioCapChan::StopMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal)
{
	if (m_pAudioMixer)
	{
		m_pAudioMixer->StopMixingWithXCapChanExternal(pXCapChanExternal);
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
int AudioCapChan::GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	nBitrate=m_XBitrateAudio.GetBitrate();

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->GetAudioPacketStatistics((uint32_t &)ulTotalPackets,(uint32_t &)ulLostPackets,AvgPacketLossRate,CurPacketLossRate);
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


bool AudioCapChan::Write(const void*data,int len)
{
	unsigned char*pData=(unsigned char*)data;
	int nLen=len;

	int nTempLen=nLen;
	if (AUDEC_HEADER_GET_USED_FEC(pData))
	{
		nTempLen=nTempLen>>1;
	}
	m_XBitrateAudio.Calculate(nTempLen);

	if (CapChanDevMgr::Instance().IsEnableAudioSend())
	{
		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->SendAudio(pData,nLen);
		}
	}

	{
		bool bLoop=true;
		unsigned long nRefCount=0;
		XAutoLock l(m_csMapAudioCapChanUI);
		MapAudioCapChanUI::iterator iter=m_MapAudioCapChanUI.begin();
		while (iter!=m_MapAudioCapChanUI.end())
		{
			iter->second->OnAudioCapChanUIAudioData(pData,nLen,nRefCount++,bLoop);
			if (bLoop==false)
			{
				break;
			}
			else
			{
				++iter;
			}
		}
	}

	return true;
}

void AudioCapChan::OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
	XAutoLock l(m_csMapAudioCapChanUI);
	MapAudioCapChanUI::iterator iter=m_MapAudioCapChanUI.begin();
	while (iter!=m_MapAudioCapChanUI.end())
	{
		iter->second->OnAudioCapChanUIRecordSamples(pSamples,nSamples,nSampleRate,nTimestamp);
		++iter;
	}
}


void AudioCapChan::AddAudioCapChanUI(AudioCapChanUI*pAudioCapChanUI)
{
	if (pAudioCapChanUI)
	{
		XAutoLock l(m_csMapAudioCapChanUI);
		m_MapAudioCapChanUI[pAudioCapChanUI]=pAudioCapChanUI;
	}
}

void AudioCapChan::RemoveAudioCapChanUI(AudioCapChanUI*pAudioCapChanUI)
{
	if (pAudioCapChanUI)
	{
		XAutoLock l(m_csMapAudioCapChanUI);
		m_MapAudioCapChanUI.erase(pAudioCapChanUI);
	}
}

int AudioCapChan::GetAudioCapChanUICount(void)
{
	XAutoLock l(m_csMapAudioCapChanUI);
	return m_MapAudioCapChanUI.size();
}

int AudioCapChan::GetMixerScreenIndex(void)
{
	return CapChanDevMgr::Instance().GetMixerScreenIndex();
}

void AudioCapChan::OnTimer(void)
{
	if (m_pMediaSender==NULL || (m_pMediaSender!=NULL && m_pMediaSender->IsAudioEnable()>0))
	{
		//Add WebRTC by zys 20160929
#if USE_WEBRTC
		if (m_pWebRTCAudio==NULL)
		{
			unsigned long nAudioID=0;
			//参数类型错误 zys 20160926
			//m_pMediaSender->GetAudioID(nAudioID);
			m_pMediaSender->GetAudioID((unsigned int&)nAudioID);
			OpenAudio(m_nAudioCapDevID,m_nAudioCodecID,nAudioID);
		}
#else
		if (m_pAudCapChan==NULL)
		{
			unsigned long nAudioID=0;
			m_pMediaSender->GetAudioID((uint32_t &)nAudioID);
			OpenAudio(m_nAudioCapDevID,m_nAudioCodecID,nAudioID);
		}
#endif
	}
	else if (XGetTimestamp()-m_nLastOpenTime>=4000)
	{
		int nForceEncode=0;
		MapAudioCapChanUI::iterator iter=m_MapAudioCapChanUI.begin();
		while (iter!=m_MapAudioCapChanUI.end())
		{
			int nTemp=0;
			iter->second->OnAudioCapChanUIGetForceEncode(nTemp);
			++iter;

			nForceEncode|=nTemp;
		}

		if (m_pAudCapChan!=NULL && nForceEncode==0)
		{
			if (m_pAudioMixer!=NULL)
			{
				m_pAudioMixer->Close();
				delete m_pAudioMixer;
				m_pAudioMixer=NULL;
			}

			m_pAudCapChan->ReleaseConnections();
			delete m_pAudCapChan;
			m_pAudCapChan=NULL;
		}
	}
}

//Add WebRTC by zys 20160929
bool AudioCapChan::SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options)
{
	XAutoLock l(m_csMapAudioCapChanUI);
	MapAudioCapChanUI::iterator iter=m_MapAudioCapChanUI.begin();
	while (iter!=m_MapAudioCapChanUI.end())
	{
		iter->second->OnAudioCapChanUIRtpData((unsigned char *)packet,length);
		++iter;
	}

	AudioPlayChanMgr::Instance().m_pAudioMixerChan->AddRtpData(m_nAudioMixChanInChannel, (const char *)packet, length);

 	unsigned char pAudecBuffer[1500];
 	//将rtp头替换为audec头
 	int nAudecPacketLen = SetAudecHeader(pAudecBuffer,packet,length);

	unsigned char*pData=(unsigned char*)pAudecBuffer;
	int nLen=nAudecPacketLen;

	int nTempLen=length;
	if (AUDEC_HEADER_GET_USED_FEC(pData))
	{
		nTempLen=nTempLen>>1;
	}
	m_XBitrateAudio.Calculate(nTempLen);

	if (CapChanDevMgr::Instance().IsEnableAudioSend())
	{
		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->SendAudio(pData,nLen);
		}
	}

	{
		bool bLoop=true;
		unsigned long nRefCount=0;
		XAutoLock l(m_csMapAudioCapChanUI);
		MapAudioCapChanUI::iterator iter=m_MapAudioCapChanUI.begin();
		while (iter!=m_MapAudioCapChanUI.end())
		{
			iter->second->OnAudioCapChanUIAudioData(pData,nLen,nRefCount++,bLoop);
			if (bLoop==false)
			{
				break;
			}
			else
			{
				++iter;
			}
		}
	}

	return true;
}

bool AudioCapChan::SendRtcp(const uint8_t* packet, size_t length)
{
	return true;
}

int AudioCapChan::SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen)
{
	int nDataLen = nlen-RTP_HEADER_LEN;
	int nPacketLen=nDataLen+AUDEC_HEADER_VER_01_LEN;
	if (nDataLen<=0 || nPacketLen>1500)
	{
		return -1;
	}

	AUDEC_HEADER_RESET(pData);
	AUDEC_HEADER_SET_SEQUENCE(pData, m_nAudioSeqNo++);
	AUDEC_HEADER_SET_TIMESTAMP(pData,  XGetTimestamp());
	unsigned short usAudioCodecID = 0;
	switch(g_emAudioCodecID)
	{
	case CODEC_PCMA:
		{
			usAudioCodecID = X_AUDIO_CODEC_G711A;
		}
	case CODEC_G7221C_48K:
		{
			usAudioCodecID = X_AUDIO_CODEC_G7221_14_48;
		}
	default:
		break;
	}
	AUDEC_HEADER_SET_CODEC_ID(pData, usAudioCodecID);  //默认使用G7221 16K
	AUDEC_HEADER_SET_FEC(pData, 1);
	AUDEC_HEADER_SET_SYN_POINT(pData, 1);
	AUDEC_HEADER_SET_DO_NOT_SYN(pData, 0);
	AUDEC_HEADER_SET_USED_FEC(pData,0);

	unsigned char *pRtpData = (unsigned char *)pSrcData;
	memcpy(pData+AUDEC_HEADER_VER_01_LEN, pRtpData+RTP_HEADER_LEN , nDataLen);
	return nPacketLen;
}