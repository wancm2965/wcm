#ifndef __LIBXMEDIAFILECAPTURE_H__
#define __LIBXMEDIAFILECAPTURE_H__

#include "libAVCONEC.h"

class IXMediaFileCaptureNotify
	: public IHPAVCaptureNotify
{
public:
	IXMediaFileCaptureNotify(void){};
	virtual~IXMediaFileCaptureNotify(void){};
public:
	virtual void OnIXMediaFileCaptureNotifyFilePositionChanged(unsigned long ulSeconds)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePause(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFilePlay(void)=0;
	virtual void OnIXMediaFileCaptureNotifyFileEnd(void)=0;
};

class IXMediaFileCapture
	: public IHPVideoWindow
{
public:
	IXMediaFileCapture(void){};
	virtual~IXMediaFileCapture(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual void Seek(unsigned long ulSeconds)=0;
	virtual void Pause(void)=0;
	virtual void Play(void)=0;
	virtual bool IsPlaying(void)=0;
	virtual void SetVolume(int nVolume)=0;
	virtual int GetVolume(void)=0;
	virtual void SetMute(bool bMute=true)=0;
	virtual bool GetMute(void)=0;
	virtual void SetBalance(int nBalance)=0;
	virtual int GetBalance(void)=0;
	virtual int GetAudioLevel(void)=0;//建议每隔200毫秒调用一次

	virtual bool HasAudio(void)=0;
	virtual bool HasVideo(void)=0;
	virtual double GetAudioBitrate(void)=0;
	virtual void CaptureVideoKeyFrame(bool bSub)=0;
	virtual double GetVideoBitrate(void)=0;

	static IXMediaFileCapture*Create(IXMediaFileCaptureNotify&rNotify,HWND hWnd,const TCHAR*cszMediaFile,int nFrameRate,int nQuality,VIDEC_CODEC_TYPE codecType,bool bLoop,bool bAutoQuality);
};


#endif