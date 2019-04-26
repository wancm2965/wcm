//AudioPlayChanMgr.h
#ifndef __AUDIOPLAYCHANMGR_H__
#define __AUDIOPLAYCHANMGR_H__

#include "AudioPlayChan.h"
#include "XAutoLock.h"
#include "WEBRTC/X86/audio_mixer_api.h"
#include "vector"
using namespace std;

class AudioPlayChanMgr : public AudioMixer_Callback
{
public:
	AudioPlayChanMgr(void);
	virtual~AudioPlayChanMgr(void);
public:
	static AudioPlayChanMgr&Instance(void);
public:

	int OpenAudioPlayChan(AudioPlayChanUI*pAudioPlayChanUI,unsigned long nAudChanID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU,int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime);
	void CloseAudioPlayChan(AudioPlayChanUI*pAudioPlayChanUI,unsigned long nAudChanID);
	
	int SetSynFlag(unsigned long nAudChanID,int nSynFlag,unsigned long*pnVideoSynTime,unsigned long*pnAudioSynTime);

	int SetVolume(unsigned long nAudChanID,int nVolume);
	int GetVolume(unsigned long nAudChanID);
	int GetLevel(unsigned long nAudChanID,int&nLevel);
	void SetMute(unsigned long nAudChanID, bool bMute);
	int EnableAudio(unsigned long nAudChanID,int nEnable/*0:不接收，1:接收*/);
	int GetAudioPacketStatistics(unsigned long nAudChanID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	int PlayAudioData(unsigned long nAudChanID,const void*pData,int nLen);

	int SetAudioCast(unsigned long nAudChanID,bool bAudioCast);

	AUDEC_PlayChan*GetPlayChan(unsigned long nAudChanID,int nMixerScreenIndex,bool bMixing);

public:
	virtual int OnMixedData (int out_channel, const char* rtp_data, int len);

public:
	void AddAudioMixerCB(AudioMixer_Callback* pAudioMixerCallback);
	void RemoveAudioMixerCB(AudioMixer_Callback* pAudioMixerCallback);

	void AddAudioMixerChanCB(AudioMixer_Callback* pAudioMixerCallback);
	void RemoveAudioMixerChanCB(AudioMixer_Callback* pAudioMixerCallback);


protected:
	typedef std::map<unsigned long,AudioPlayChan*>	MapAudioPlayChan;

	XCritSec			m_csMapAudioPlayChan;
	MapAudioPlayChan	m_MapAudioPlayChan;

public:
	AudioMixerAPI* m_pAudioMixer;
	int m_nAudioMixOut;
	vector<AudioMixer_Callback*> m_arrAudioMixerCB;
	XCritSec				m_csAVIWriter;

	//混音通道相关
public:
	AudioMixerAPI* m_pAudioMixerChan;
	int m_nAudioMixChanOut;
	vector<AudioMixer_Callback*> m_arrAudioMixerChanCB;
	XCritSec				m_csAudioMixerChan;
};

#endif