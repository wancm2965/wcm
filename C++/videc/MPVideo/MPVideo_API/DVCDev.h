//DVCDev.h
#ifndef __DVCDEV_H__
#define __DVCDEV_H__

#include "VIDEC_Config.h"

#ifdef SUPPORT_DVC

#include <winsock2.h>
#include <windows.h>

#include "VIDEC_DVC.h"
#include "XImageSource.h"
#include "XStreamSource.h"

class DVCDev
	: public VIDEC_DVCCallback
	, public XImageSource
	, public VIDEC_ImageSource
	, public XStreamSource
	, public VIDEC_StreamSource
{
public:
	DVCDev(unsigned int nDevIndex);
	virtual~DVCDev(void);
public:
	int Open(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);
	void Close(void);
	int Reset(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution);

	int SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/);
 	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);
  	
	virtual int RequestKeyFrame(void);

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
	
	void OnVIDEC_DVCCallbackImage(VIDEC_Image*pImage);
	void OnVIDEC_DVCCallbackVideoStreamData(unsigned char*pData,int nLen,int nKeyFrame,int nWidth,int nHeight);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	virtual int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
protected:
	VIDEC_DVC*			m_pDVC;

	unsigned int		m_nDevIndex;

	int					m_nBitrate;
	int					m_nBitrateControlType;
	int					m_nFrameRate;
	int					m_nResolution;
};

#endif

#endif