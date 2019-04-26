//AVCONAVC_AudioFile.h
#ifndef __AVCONAVC_AUDIOFILE_H__
#define __AVCONAVC_AUDIOFILE_H__

#include <AVCONAVC/AVCONAVC_Export.h>

class AVCONAVC_AudioFile;
class AVCONAVC_AudioFileCallback
{
public:
	AVCONAVC_AudioFileCallback(void){};
	virtual~AVCONAVC_AudioFileCallback(void){};
public:
	virtual void OnAVCONAVC_AudioFileCallbackPCMData(AVCONAVC_AudioFile*pAudioFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)=0;
};

class AVCONAVC_API AVCONAVC_AudioFile
{
public:
	AVCONAVC_AudioFile(void){};
	virtual~AVCONAVC_AudioFile(void){};
public:
	virtual int Open(const char*cszFileName)=0;
	virtual void Close(void)=0;

	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual int Seek(unsigned long ulSeconds)=0;
	virtual int Pause(void)=0;
	virtual int Play(void)=0;

	virtual int SetVolume(int nVolume)=0;

	static AVCONAVC_AudioFile*Create(AVCONAVC_AudioFileCallback&rCallback);
};


#endif
