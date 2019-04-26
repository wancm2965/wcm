#ifndef __MEDIAFILECAPTURE_H__
#define __MEDIAFILECAPTURE_H__

#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_MediaFile.h>
#include <AVCONAVC/AVCONAVC_MediaFile.h>
#include "../VIDEC_API/XImageSource.h"
#include "../VIDEC_API/XImage.h"
#include "../VIDEC_csp/fastmemcpy.h"
#include "../VIDEC_API/IXResample.h"
#include <string>
#include "AVMSMediaFile.h"

#define	PLAY_STATE_STOP		0
#define	PLAY_STATE_PLAY		1
#define	PLAY_STATE_PAUSE	2 

class MediaFileCapture
	: public CWnd 
	, public VIDEC_MediaFile
	, public AVMSMediaFileCallback
	, public XImageSource
{
	DECLARE_DYNAMIC(MediaFileCapture)
public:
	MediaFileCapture(VIDEC_MediaFileCallback&rCallback);
	virtual ~MediaFileCapture(void);
public:
	int Open(HWND hParentWnd,const char*cszPathName,bool bLoop);
	void Close(void);
	int OpenFile(const char*cszPathName);
	void CloseFile(void);

	unsigned long GetDuration(void);
	unsigned long GetPosition(void);
	int Seek(unsigned long ulSeconds);
	int Pause(void);
	int Play(void);
	bool IsPlaying(void);

	bool HasAudio(void);
	int SetVolume(int nVolume);
	int SetMute(int nMute);

	bool HasVideo(void);
	int GetWidth(void);
	int GetHeight(void);

	void OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp);
	void OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen);
	void OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp);

	virtual void OnAVMSMediaFileCallbackFrame(VIDEC_Image*pImage);
	
	void SetInvert(int nInvert){};
	int GetInvert(void){return 0;};
	void SetDenoise(int nDenoise){};
	int GetDenoise(void){return 0;};
	void SetLevelFixer(int nLevelFixer){};
	int GetLevelFixer(void){return 0;};
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);

	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	VIDEC_MediaFileCallback&	m_rCallback;

	AVCONAVC_MediaFile*			m_pMediaFile;

	int							m_nPlayState;
	bool						m_bLoop;
	HWND						m_hParentWnd;


	bool						m_bMute;
	int							m_nVolume;

	unsigned long				m_nLastPlayPosition;
	bool						m_bSeeking;
	std::string					m_strFilePathName;
	unsigned long				m_nLastCapturedFrameTimestamp;

	bool						m_bPlaying;
	int							m_nDetectingError;
	bool						m_bAudio;
	bool						m_bVideo;

	IXResample*					m_pIXResample;
	int							m_nSampleRateResample;
	unsigned char*				m_pResampleBuf;
	int							m_nResampleBufLen;
};

#endif