#ifndef __AUDIOFILECAPTURE_H__
#define __AUDIOFILECAPTURE_H__

#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_AudioFile.h>
#include <AVCONAVC/AVCONAVC_AudioFile.h>
#include "XWorkThread.h"
#include "XAutoLock.h"
#include "XListPtr.h"
#include <string>

#define	PLAY_STATE_STOP		0
#define	PLAY_STATE_PLAY		1
#define	PLAY_STATE_PAUSE	2 

class XSampleBuffer
{
public:
	XSampleBuffer(void)
		: m_pBuffer(NULL)
		, m_BufferLen(0)
		, m_nSampleRate(32000)
	{
	}
	virtual~XSampleBuffer(void)
	{
		if (m_pBuffer!=NULL)
		{
			FREEMSG(m_pBuffer);
			m_pBuffer=NULL;
		}
	}
public:
	bool SetSampleBuffer(BYTE * pBuffer, int BufferLen,int nSampleRate)
	{
		if (m_pBuffer!=NULL)
		{
			FREEMSG(m_pBuffer);
			m_pBuffer=NULL;
		}

		m_nSampleRate=nSampleRate;
		m_pBuffer=(BYTE*)MALLOCMSG(BufferLen);
		if (m_pBuffer==NULL)
		{
			return false;
		}

		m_BufferLen=BufferLen;
		memcpy(m_pBuffer,pBuffer,BufferLen);
		return true;
	}

	int GetSampleRate(void)
	{
		return m_nSampleRate;
	}

	BYTE*GetBuffer(void)
	{
		return m_pBuffer;
	}
	int GetBufferLen(void)
	{
		return m_BufferLen;
	}
protected:
	BYTE*			m_pBuffer;
	int				m_BufferLen;
	int				m_nSampleRate;
};

class AudioFileCapture
	: public CWnd 
	, public VIDEC_AudioFile
	, public AVCONAVC_AudioFileCallback
	, public XWorkThread
{
	DECLARE_DYNAMIC(AudioFileCapture)
public:
	AudioFileCapture(VIDEC_AudioFileCallback&rCallback);
	virtual ~AudioFileCapture(void);
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


	void OnAVCONAVC_AudioFileCallbackPCMData(AVCONAVC_AudioFile*pAudioFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp);
	
	bool IsPending(void);
	bool Running(void);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	VIDEC_AudioFileCallback&	m_rCallback;

	AVCONAVC_AudioFile*			m_pAudioFile;

	int							m_nPlayState;
	bool						m_bLoop;


	bool						m_bMute;
	int							m_nVolume;

	bool						m_bSeeking;
	std::string					m_strFilePathName;

	bool						m_bPlaying;

	XListPtr					m_ListXSampleBuffer;
	XCritSec					m_csListXSampleBuffer;

	unsigned long				m_nSampleTimestamp;
	unsigned char*				m_pLeftSampleBuffer;
	int							m_nLeftSampleBufferLen;
	int							m_nLeftSampleDataLen;
	int							m_nSampleRate;
	int							m_nFrameLen;

	int							m_nEndCount;
};

#endif