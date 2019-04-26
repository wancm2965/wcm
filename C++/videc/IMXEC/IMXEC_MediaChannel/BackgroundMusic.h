//BackgroundMusic.h
#ifndef __BACKGROUNDMUSIC_H__
#define __BACKGROUNDMUSIC_H__

#include <IMXEC/IMXEC_MediaChannel.h>
#include <VIDEC/VIDEC_AudioFile.h>
#include <AUDEC/AUDEC_Engine.h>

class BackgroundMusic
	: public IMXEC_BackgroundMusic
	, public VIDEC_AudioFileCallback
{
public:
	BackgroundMusic(IMXEC_BackgroundMusicCallback&rCallback);
	virtual ~BackgroundMusic(void);
public:
	int Open(const char*cszPathName,bool bLoop);
	void Close(void);
	int OpenFile(const char*cszPathName);
	void CloseFile(void);

	unsigned long GetDuration(void);
	unsigned long GetPosition(void);
	int Seek(unsigned long ulSeconds);
	int Pause(void);
	int Play(void);
	bool IsPlaying(void);

	int SetVolume(int nVolume);
	int SetMute(int nMute);

	AUDEC_PlayChan*GetPlayChan(void){return m_pPlayChan;};

	void OnVIDEC_AudioFileCallbackPositionChanged(VIDEC_AudioFile*pAudioFile,unsigned long ulSeconds);
	void OnVIDEC_AudioFileCallbackEnd(VIDEC_AudioFile*pAudioFile);
	void OnVIDEC_AudioFileCallbackPCMData(VIDEC_AudioFile*pAudioFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long ulTimestamp);
protected:
	IMXEC_BackgroundMusicCallback&	m_rCallback;
	VIDEC_AudioFile*				m_pAudioFile;
	AUDEC_PlayChan*					m_pPlayChan;

	unsigned char*					m_pAudioBuffer;
	int								m_nAudioBufferLen;
	unsigned short					m_nSequence;
};

#endif
