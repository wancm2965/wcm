//XImageSourceAH400.h
#ifndef __XIMAGESOURCEAH400_H__
#define __XIMAGESOURCEAH400_H__

#include "VIDEC_AH400Dev.h"

#include "VIDEC_Config.h"

#ifdef SUPPORT_AH400


#include "XImageSource.h"
#include "XImageSink.h"
#include "XStreamSource.h"
#include "XStreamSink.h"

class XImageSourceAH400
	: public VIDEC_AH400Dev
	, public XImageSource
	, public VIDEC_ImageSink
	, public XImageSink
	, public XStreamSource
	, public VIDEC_StreamSink
	, public XStreamSink
{
public:
	XImageSourceAH400(void);
	virtual~XImageSourceAH400(void);
public:
	virtual int Open(int nDevID,int nWidth,int nHeight,int nFrameRate);
	virtual void Close(void);
	virtual int Reset(int nDevID,int nWidth,int nHeight,int nFrameRate);
	virtual int StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax);
	virtual int StopVideoStream(void);
	virtual int RequestKeyFrame(void);
	virtual int RequestSubKeyFrame(void);
	virtual int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
 	virtual int SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB);

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
	virtual int StartMotionDetect(void);
	virtual int StopMotionDetect(void);
	virtual int SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	virtual void OnAttachedToImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource);
	virtual void OnReceivedImage(VIDEC_Image*pImage);
	virtual void OnReceivedPreSend(int& nSend);
	virtual void OnReceivedMotionDetect(int*pResultList,int nResultCount);
	
	virtual int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
	
	virtual void OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource);
	virtual void OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen);
	virtual void OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen);
protected:
	int							m_nDevID;

#ifdef _DEMO
	unsigned long				m_ulFirstTimestamp;
#endif
};

#endif

#endif
