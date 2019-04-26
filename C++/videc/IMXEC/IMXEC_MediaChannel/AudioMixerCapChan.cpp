#include "AudioMixerCapChan.h"
#include "AudioCapChanMgr.h"
#include "AudioPlayChanMgr.h"
#include "PlayChan.h"
#include "XUtil.h"
#include "CapChanDevMgr.h"
#include <AUDEC/AUDEC_Header.h>
#include "RTPHeader.h"

AudioMixerCapChan::AudioMixerCapChan(IMXEC_AudioMixerCapChanCallback&rCallback)
:m_rCallback(rCallback),
m_pMediaSender(NULL),
m_nAudioSeqNo(0)
{
	m_ulAudioID=XGenerateSSRC()+(unsigned long)this;
}

AudioMixerCapChan::~AudioMixerCapChan(void)
{

}

int AudioMixerCapChan::Open(void)
{
#if USE_WEBRTC
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

		m_pMediaSender->StartAudio(m_ulAudioID);
	}

	AudioPlayChanMgr::Instance().AddAudioMixerChanCB(this);

	return 0;
#else
	unsigned long nKeyID=(unsigned long)this;
	nKeyID|=0xFF000000;
	if (0!=AudioCapChanMgr::Instance().OpenMixingAudio(nKeyID,this,NULL,AudioCapChanMgr::Instance().GetCodecID()))
	{
		return -1;
	}

	return 0;
#endif
}

void AudioMixerCapChan::Close(void)
{
#if USE_WEBRTC
	AudioPlayChanMgr::Instance().RemoveAudioMixerChanCB(this);

	if (m_pMediaSender!=NULL)
	{
		m_pMediaSender->Close();
		delete m_pMediaSender;
		m_pMediaSender=NULL;
	}
	
#else
	unsigned long nKeyID=(unsigned long)this;
	nKeyID|=0xFF000000;

	AudioCapChanMgr::Instance().CloseMixingAudio(nKeyID,this);
#endif
}

unsigned long AudioMixerCapChan::GetAudioID(void)
{
#if USE_WEBRTC
	unsigned long nAudioID=0;
	if (m_pMediaSender!=NULL)
	{
		if (0!=m_pMediaSender->GetAudioID((uint32_t &)nAudioID))
		{
			nAudioID=0;
		}
	}
	return nAudioID;
#else
	unsigned long nAudioID=0;
	unsigned long nKeyID=(unsigned long)this;
	nKeyID|=0xFF000000;

	AudioCapChanMgr::Instance().GetMixingAudioID(nKeyID,nAudioID);

	return nAudioID;
#endif
}

void AudioMixerCapChan::ExcludePlayChan(IMXEC_PlayChan*pPlayChan)
{
#if USE_WEBRTC

#else
	if (pPlayChan)
	{
		unsigned long nKeyID=(unsigned long)this;
		nKeyID|=0xFF000000;

		AudioCapChanMgr::Instance().ExcludePlayChan(nKeyID,((PlayChan*)pPlayChan)->GetPlayChan());
	}
#endif
}


void AudioMixerCapChan::OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop)
{
	
}

int AudioMixerCapChan::OnMixedData (int out_channel, const char* data, int len)
{
	unsigned char pAudecBuffer[1500];
	//将rtp头替换为audec头
	int nAudecPacketLen = SetAudecHeader(pAudecBuffer,data,len);

	unsigned char*pData=(unsigned char*)pAudecBuffer;
	int nLen=nAudecPacketLen;

	if (CapChanDevMgr::Instance().IsEnableAudioSend())
	{
		if (m_pMediaSender!=NULL)
		{
			m_pMediaSender->SendAudio(pData,nLen);
		}
	}
	return 0;

	//m_rCallback.OnIMXEC_AudioMixerCapChanCallbackAudioSamples(pSamples,nSamples,nSampleRate,nSampleRate);
}

int AudioMixerCapChan::SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen)
{
	int nDataLen = nlen;
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
	memcpy(pData+AUDEC_HEADER_VER_01_LEN, pRtpData , nDataLen);
	return nPacketLen;
}

void AudioMixerCapChan::OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
#if USE_WEBRTC

#else
	m_rCallback.OnIMXEC_AudioMixerCapChanCallbackAudioSamples(pSamples,nSamples,nSampleRate,nSampleRate);
#endif
}

void AudioMixerCapChan::OnAudioCapChanUIGetForceEncode(int&nEncode)
{

}

IMXEC_AudioMixerCapChan*IMXEC_AudioMixerCapChan::Create(IMXEC_AudioMixerCapChanCallback&rCallback)
{
	return new AudioMixerCapChan(rCallback);
}