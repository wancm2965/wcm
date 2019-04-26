//XImageSourceAH2K.h
#ifndef __XIMAGESOURCEAH2K_H__
#define __XIMAGESOURCEAH2K_H__

#include "VIDEC_Config.h"
#include "VIDEC_AH2KDev.h"

#ifdef SUPPORT_AH2K

#include "XImageSource.h"
#include "XImageSink.h"
#include "XStreamSource.h"
#include "XStreamSink.h"
 
class XImageSourceAH2K
	: public VIDEC_AH2KDev
	, public XImageSource
	, public VIDEC_ImageSink
	, public XImageSink
	, public XStreamSource
	, public VIDEC_StreamSink
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
	virtual void SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft);
	virtual void SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnReceivedImage(VIDEC_Image*pImage);
	virtual void OnReceivedPreSend(int& nSend);
	virtual void OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate);
	
	virtual int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
	
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen);
protected:
	int		m_nDevID;
	int		m_nHasVideoSignal;

#ifdef _DEMO
	unsigned long					m_ulFirstTimestamp;
#endif
};

#endif

#endif