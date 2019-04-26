//AVCONAVC_MediaFile.h
#ifndef __AVCONAVC_MEDIAFILE_H__
#define __AVCONAVC_MEDIAFILE_H__

#include <AVCONAVC/AVCONAVC_Export.h>

class AVCONAVC_MediaFile;
class AVCONAVC_MediaFileCallback
{
public:
	AVCONAVC_MediaFileCallback(void){};
	virtual~AVCONAVC_MediaFileCallback(void){};
public:
	virtual void OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)=0;
	virtual void OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen)=0;
	virtual void OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp)=0;
};

class AVCONAVC_API AVCONAVC_MediaFile
{
public:
	AVCONAVC_MediaFile(void){};
	virtual~AVCONAVC_MediaFile(void){};
public:
	virtual int Open(HWND hWndVideo,const char*cszFileName,bool& bTryEVR)=0;
	virtual void Close(void)=0;
	virtual void SetWndPos(int x,int y,int cx,int cy)=0;
	virtual void Repaint(void)=0;

	virtual unsigned long GetDuration(void)=0;
	virtual unsigned long GetPosition(void)=0;
	virtual int Seek(unsigned long ulSeconds)=0;
	virtual int Pause(void)=0;
	virtual int Play(void)=0;

	virtual bool HasAudio(void)=0;
	virtual int SetVolume(int nVolume)=0;

	virtual bool HasVideo(void)=0;
	virtual int GetVideoWidth(void)=0;
	virtual int GetVideoHeight(void)=0;

	static AVCONAVC_MediaFile*Create(AVCONAVC_MediaFileCallback&rCallback);
	static AVCONAVC_MediaFile*Create(AVCONAVC_MediaFileCallback&rCallback,bool bAudio,bool bVideo);
};


#endif
