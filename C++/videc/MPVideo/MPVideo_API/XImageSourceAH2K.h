//XImageSourceAH2K.h
#ifndef __XIMAGESOURCEAH2K_H__
#define __XIMAGESOURCEAH2K_H__

#include "MPVIDEO_Config.h"
#include "MPVIDEO_AH2KDev.h"

#ifdef SUPPORT_AH2K

#include "XImageSource.h"
#include "XImageSink.h"
#include "XStreamSource.h"
#include "XStreamSink.h"
 
class XImageSourceAH2K
	: public MPVIDEO_AH2KDev
	, public XImageSource
	, public MPVIDEO_ImageSink
	, public XImageSink
	, public XStreamSource
	, public MPVIDEO_StreamSink
	, public XStreamSink
{
public:
	XImageSourceAH2K(void);
	virtual~XImageSourceAH2K(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDenoise=false);
	virtual void Close(void);
	virtual int Reset(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDenoise=false);
	virtual int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
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
	virtual int HasVideoSignal(void);

	
	virtual void EnableOSD(bool bEnable);
	virtual bool IsOSDEnable(void);
	virtual void SetOSDText(const char*cszText,int nAlign);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	
	virtual int AddImageSink(MPVIDEO_ImageSink*pImageSink);
	virtual void RemoveImageSink(MPVIDEO_ImageSink*pImageSink);
	
	virtual void OnAttachedToImageSource(MPVIDEO_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(MPVIDEO_ImageSource*pImageSource);
	virtual void OnReceivedImage(MPVIDEO_Image*pImage);
	virtual void OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate);
	
	virtual int AddStreamSink(MPVIDEO_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink);
	
	virtual void OnAttachedToStreamSource(MPVIDEO_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(MPVIDEO_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedSubStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedAudioStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen);
protected:
	int		m_nDevID;
	int		m_nHasVideoSignal;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif

#endif