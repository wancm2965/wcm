#ifndef __MEDIAFILE_H__
#define __MEDIAFILE_H__

#include <AVCONAVC/AVCONAVC_MediaFile.h>
#include "VideoDecoder.h"
#include "AudioSampleGrabber.h"

#include <string>
#include "XAutoLock.h"
#include "XListPtr.h"
#include "PCMConvert.h"

#include "../AVPullPlayer/MemStream.h"
#include "NullRender.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"


typedef enum 
{
	VRT_NULL=0,
	VRT_SDR,
	VRT_VMR,
	VRT_EVR
}VIDEO_RENDER_TYPE;


class XSampleBuffer
{
public:
	XSampleBuffer(void)
		: m_dblSampleTime(0.0)
		, m_pBuffer(NULL)
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
	bool SetSampleBuffer(double dblSampleTime, BYTE * pBuffer, long BufferLen,int nSampleRate)
	{
		if (m_pBuffer!=NULL)
		{
			FREEMSG(m_pBuffer);
			m_pBuffer=NULL;
		}

		m_nSampleRate=nSampleRate;
		m_dblSampleTime=dblSampleTime;
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

	double GetSampleTime(void)
	{
		return m_dblSampleTime;
	}
	BYTE*GetBuffer(void)
	{
		return m_pBuffer;
	}
	long GetBufferLen(void)
	{
		return m_BufferLen;
	}
protected:
	double			m_dblSampleTime;
	BYTE*			m_pBuffer;
	long			m_BufferLen;
	int				m_nSampleRate;
};

class CLAVSplitter;
class CLAVAudio;
class CLAVVideo;

class MediaFile
	: public AVCONAVC_MediaFile
	, public NullRendererCallback
	, public AudioSampleGrabberCallback
{
public:
	MediaFile(AVCONAVC_MediaFileCallback&rCallback,bool bAudio=true,bool bVideo=true);
	virtual ~MediaFile(void);
public:
	int Open(HWND hWndVideo,const char*cszFileName,bool& bTryEVR);
	void Close(void);
	int OpenFile(const char*cszPathName,bool& bTryEVR);
	void CloseFile(void);
	void SetWndPos(int x,int y,int cx,int cy);
	void Repaint(void);

	unsigned long GetDuration(void);
	unsigned long GetPosition(void);
	int Seek(unsigned long ulSeconds);
	int Pause(void);
	int Play(void);

	bool HasAudio(void);
	int SetVolume(int nVolume);

	bool HasVideo(void);
	int GetVideoWidth(void);
	int GetVideoHeight(void);
	
	void OnNullRendererYV12Buffer(unsigned char**ppBuffer,int nBufferLen);
	void OnNullRendererCallback(unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,double dblSampleTime);
	void OnAudioSampleGrabberCallback(unsigned char*pData,int nLen,int nSampleRate,double dblSampleTime);
protected:
	void DoCheckAudioSample(void);
	void OnCaputuredPCM(unsigned char*pPCMData,int nLen,int nSampleRate,double dblSampleTime);
protected:
	AVCONAVC_MediaFileCallback&	m_rCallback;
	CComPtr<IGraphBuilder>		m_pGraph;
	AudioSampleGrabber*			m_pAudioSampleGrabber;
	VIDEO_RENDER_TYPE			m_nVideoRenderType;
	IMFVideoDisplayControl*		m_pVideoDisplay;
	IVMRWindowlessControl*		m_pWindowless;

	int							m_nVideoWidth;
	int							m_nVideoHeight;
	HWND						m_hWndVideo;

	std::string					m_strFilePathName;

	bool						m_bHasAudio;
	bool						m_bHasVideo;
	bool						m_bAudio;
	bool						m_bVideo;

	double						m_dblLastVideoSampleTime;
	XListPtr					m_ListXSampleBuffer;
	XCritSec					m_csListXSampleBuffer;


	IFileReader*				m_pFileReader;
	CMemStream*					m_pMemStream;
	CMemReader*					m_pMemReader;

	CLAVSplitter*				m_pLAVSplitter;
	CLAVAudio*					m_pLAVAudio;
	CLAVVideo*					m_pLAVVideo;
	NullRenderer*				m_pVideoNullRenderer;
	bool						m_bLoadFailed;
};

#endif