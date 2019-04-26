#ifndef __AUDIOFILE_H__
#define __AUDIOFILE_H__

#include <AVCONAVC/AVCONAVC_AudioFile.h>
#include "AudioSampleGrabber.h"

#include <string>
#include "PCMConvert.h"


class CLAVSplitter;
class CLAVAudio;

class AudioFile
	: public AVCONAVC_AudioFile
	, public AudioSampleGrabberCallback
{
public:
	AudioFile(AVCONAVC_AudioFileCallback&rCallback);
	virtual ~AudioFile(void);
public:
	int Open(const char*cszFileName);
	void Close(void);
	int OpenFile(const char*cszPathName);
	void CloseFile(void);

	unsigned long GetDuration(void);
	unsigned long GetPosition(void);
	int Seek(unsigned long ulSeconds);
	int Pause(void);
	int Play(void);

	int SetVolume(int nVolume);

	void OnAudioSampleGrabberCallback(unsigned char*pData,int nLen,int nSampleRate,double dblSampleTime);
protected:
	AVCONAVC_AudioFileCallback&	m_rCallback;
	CComPtr<IGraphBuilder>		m_pGraph;
	AudioSampleGrabber*			m_pAudioSampleGrabber;


	std::string					m_strFilePathName;

	CLAVSplitter*				m_pLAVSplitter;
	CLAVAudio*					m_pLAVAudio;
};

#endif