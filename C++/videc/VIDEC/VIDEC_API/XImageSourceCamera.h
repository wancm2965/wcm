//XImageSourceCamera.h
#ifndef __XIMAGESOURCECAMERA_H__
#define __XIMAGESOURCECAMERA_H__

#include "VIDEC_CameraDev.h"
#include "XImageSource.h"
#include "XImageSink.h"
 
class XImageSourceCamera
	: public VIDEC_CameraDev
	, public XImageSource
	, public VIDEC_ImageSink
	, public XImageSink
{
public:
	XImageSourceCamera(void);
	virtual~XImageSourceCamera(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat=true);
	virtual void Close(void);
	virtual int Reset(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat=true);
	virtual int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
	
	virtual void ShowAdjustVideoWnd(HWND hWnd);
	virtual void ShowVideoInputWnd(HWND hWnd);
	virtual bool HasVideoInputWnd(void);
	virtual int TVTunerPutChannel(long lChannel);

	virtual void SetInvert(int nInvert);
	virtual int GetInvert(void);
	virtual void SetDenoise(int nDenoise);
	virtual int GetDenoise(void);
	virtual void SetLevelFixer(int nLevelFixer);
	virtual int GetLevelFixer(void);
	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);

	//设置遮掩并启动遮掩
	virtual int SetupMask(RECT *RectList, int iRectCount);
	//停止遮掩
	virtual int StopMask(void);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnReceivedImage(VIDEC_Image*pImage);
	virtual void OnReceivedPreSend(int& nSend);
protected:
	int		m_nDevID;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif