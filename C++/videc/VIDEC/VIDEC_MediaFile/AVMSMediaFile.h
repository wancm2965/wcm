//AVMSMediaFile.h
#ifndef __AVMSMEDIAFILE_H__
#define __AVMSMEDIAFILE_H__

#include <AVCONAVC/AVCONAVC_MediaFile.h>
#include "AVMSWrapper.h"
#include "../VIDEC_Codecs/XDecoderStreamBase.h"

class AVMSMediaFileCallback
	: public AVCONAVC_MediaFileCallback
{
public:
	virtual void OnAVMSMediaFileCallbackFrame(VIDEC_Image*pImage)=0;
};

class AVMSMediaFile
	: public AVCONAVC_MediaFile
	, public XDecoderStreamBase
{
public:
	AVMSMediaFile(AVMSMediaFileCallback&rCallback);
	virtual~AVMSMediaFile(void);
public:
	virtual int Open(HWND hWndVideo,const char*cszFileName,bool& bTryEVR);
	virtual void Close(void);
	virtual void SetWndPos(int x,int y,int cx,int cy);
	virtual void Repaint(void);

	virtual unsigned long GetDuration(void);
	virtual unsigned long GetPosition(void);
	virtual int Seek(unsigned long ulSeconds);
	virtual int Pause(void);
	virtual int Play(void);

	virtual bool HasAudio(void);
	virtual int SetVolume(int nVolume);

	virtual bool HasVideo(void);
	virtual int GetVideoWidth(void);
	virtual int GetVideoHeight(void);

	void OnPCMData(unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp);
	void OnVideoPacket(unsigned char*pData,int nLen);
protected:
	virtual bool IsPending(void);
	virtual void OnPreDecodingFrame(unsigned long nFrameTimestamp,unsigned long nPaintTimestamp);
	virtual void OnDecoderStreamCallbackFrame(VIDEC_Image*pImage);

protected:
	AVMSMediaFileCallback&			m_rCallback;
	void*							m_pHandle;
	unsigned long					m_nAudioTimestamp;
	unsigned long					m_nVideoTimestamp;

	unsigned long					m_nLastAudioTimestamp;
	unsigned long					m_nLastUpdateAudioTimestamp;
	unsigned long					m_nNewFrameTimestamp;
	unsigned long					m_nLastPaintedTimestamp;
	unsigned long					m_nLastFrameTimestamp;
};


#endif
