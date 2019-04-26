//AH2KDev.h
#ifndef __AH2KDEV_H__
#define __AH2KDEV_H__

#include "MPVIDEO_Config.h"

#ifdef SUPPORT_AH2K

#include <winsock2.h>
#include <windows.h>

#include "MPVIDEO_AH2K.h"
#include "libXPreProc.h"
#include "XImageSource.h"
#include "XStreamSource.h"

#include "XThreadBase.h"
#include "XAutoLock.h"
#include "XListPtr.h"

#define AH2KDEV_USE_THREAD

class AH2KDev
	: public MPVIDEO_AH2KCallback
	, public IXPreProcessNotify
	, public XImageSource
	, public MPVIDEO_ImageSource
	, public XStreamSource
	, public MPVIDEO_StreamSource

#ifdef AH2KDEV_USE_THREAD
	, public XThreadBase
#endif
{
public:
	AH2KDev(unsigned int nDevIndex);
	virtual~AH2KDev(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate,bool bDenoise=false);
	void Close(void);
	int Reset(int nWidth,int nHeight,int nFrameRate,bool bDenoise=false);

	int SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/);
 	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
  	
	virtual int StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax);
	virtual int RequestKeyFrame(void);
	virtual int StopVideoStream(void);

	virtual void SetInvert(int nInvert);
	virtual int GetInvert(void);
	virtual void SetDenoise(int nDenoise);
	virtual int GetDenoise(void);
	virtual void SetLevelFixer(int nLevelFixer);
	virtual int GetLevelFixer(void);
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);

	bool CanBeClosed(void);
	
	void OnMPVIDEO_AH2KCallbackImage(MPVIDEO_Image*pImage);
	virtual void OnMPVIDEO_AH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate);

	void OnIXPreProcessNotifyOutputData(MPVIDEO_Image*pImage);
	
	virtual int AddImageSink(MPVIDEO_ImageSink*pImageSink);
	virtual void RemoveImageSink(MPVIDEO_ImageSink*pImageSink);
	
	virtual int AddStreamSink(MPVIDEO_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink);

#ifdef AH2KDEV_USE_THREAD
protected:
	void ThreadProcMain(void);
protected:
	bool				m_bRunning;
	XListPtr			m_listImage;
	XCritSec			m_csListImage;
#endif

protected:
	MPVIDEO_AH2K*			m_pAH2K;
	IXPreProcess*		m_pIXPreProcess;

	unsigned int		m_nDevIndex;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nFrameRate;
	bool				m_bDenoise;
};

#endif

#endif