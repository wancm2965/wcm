//AudioMixerCapChan.h
#ifndef __AUDIOMIXERCAPCHAN_H__
#define __AUDIOMIXERCAPCHAN_H__

#include <IMXEC/IMXEC_MediaChannel.h>
#include "AudioCapChan.h"
#include "WEBRTC/X86/audio_mixer_api.h"

class AudioMixerCapChan
	: public IMXEC_AudioMixerCapChan
	, public AudioCapChanUI
	, public NETEC_MediaSenderCallback
	, public AudioMixer_Callback
{
public:
	AudioMixerCapChan(IMXEC_AudioMixerCapChanCallback&rCallback);
	virtual ~AudioMixerCapChan(void);
public:
	virtual int Open(void);
	virtual void Close(void);
	virtual unsigned long GetAudioID(void);
	virtual void ExcludePlayChan(IMXEC_PlayChan*pPlayChan);

	int SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen);

	virtual int OnMixedData (int out_channel, const char* data, int len);


	virtual void OnAudioCapChanUIAudioData(unsigned char*pData,int nLen,unsigned long nRefCount,bool&bLoop);
	virtual void OnAudioCapChanUIRtpData(unsigned char*pData,int nLen){};
	virtual void OnAudioCapChanUIRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	virtual void OnAudioCapChanUIGetForceEncode(int&nEncode);
protected:
	IMXEC_AudioMixerCapChanCallback&	m_rCallback;
	NETEC_MediaSender*		m_pMediaSender;
	unsigned long		    m_ulAudioID;
	int                     m_nAudioSeqNo;
};


#endif

