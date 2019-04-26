//AudioCapChanMgr.h
#ifndef __AUDIOCAPCHANMGR_H__
#define __AUDIOCAPCHANMGR_H__

#include "AudioCapChan.h"
#include "BackgroundMusic.h"

class AudioCapChanMgr
{
public:
	AudioCapChanMgr(void);
	virtual~AudioCapChanMgr(void);
public:
	static AudioCapChanMgr&Instance(void);
public:
	int OpenAudioCapChan(AudioCapChanUI*pAudioCapChanUI,int nDevID,X_AUDIO_CODEC_ID CodecType);
	void CloseAudioCapChan(AudioCapChanUI*pAudioCapChanUI);
	int SetVolume(int nVolume);
	int GetVolume();
	int GetLevel(int&nLevel);
	int GetAudioID(unsigned long&nAudioID);
	bool IsAudioOpen(void);

	
	void OnTimer(void);

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
	int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	int OpenMixingAudio(unsigned long nPlayAudioID,AudioCapChanUI*pAudioCapChanUI,AUDEC_PlayChan*pPlayChan,X_AUDIO_CODEC_ID nCodecID);
	void CloseMixingAudio(unsigned long nPlayAudioID,AudioCapChanUI*pAudioCapChanUI);
	void ExcludePlayChan(unsigned long nPlayAudioID,AUDEC_PlayChan*pPlayChan);
	int GetMixingAudioID(unsigned long nPlayAudioID,unsigned long&nMixingAudioID);
	
	void StartMixingWithPlayChan(AUDEC_PlayChan*pPlayChan);
	void StopMixingWithPlayChan(AUDEC_PlayChan*pPlayChan);
	
	void StartMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal);
	void StopMixingWithXCapChanExternal(AUDEC_CapChanExternal*pXCapChanExternal);

	X_AUDIO_CODEC_ID GetCodecID(void){return m_nCodecType;};

	BackgroundMusic*CreateBackgroundMusic(IMXEC_BackgroundMusicCallback&rCallback);
	void OnBackgroundMusicOpen(BackgroundMusic*pBackgroundMusic);
	void OnBackgroundMusicClose(BackgroundMusic*pBackgroundMusic);
protected:
	int					m_nAudioDevID;
	X_AUDIO_CODEC_ID	m_nCodecType;

	AudioCapChan*		m_pAudioCapChan;
	unsigned long		m_ulAudioID;
	unsigned int		m_nTimerID;

	typedef struct tagMIXING_AUDIO
	{
		AudioCapChan*	pAudioCapChan;
		unsigned long	nAudioID;
	}MIXING_AUDIO;
	typedef std::map<unsigned long,MIXING_AUDIO>	MapMixingAudio;

	MapMixingAudio	m_MapMixingAudio;

	BackgroundMusic*	m_pBackgroundMusic;
};

#endif
