//AH400Dev.h
#ifndef __AH400DEV_H__
#define __AH400DEV_H__


#include "VIDEC_Config.h"

#ifdef SUPPORT_AH400

#include <winsock2.h>
#include <windows.h>

#include "VIDEC_AH400.h"
#include "libXPreProc.h"
#include "XImageSource.h"
#include "XStreamSource.h"

class AH400Dev
	: public VIDEC_AH400Callback
	, public IXPreProcessNotify
	, public XImageSource
	, public VIDEC_ImageSource
	, public XStreamSource
	, public VIDEC_StreamSource
{
public:
	AH400Dev(unsigned int nDevIndex);
	virtual~AH400Dev(void);
public:
	int Open(int nWidth,int nHeight,int nFrameRate);
	void Close(void);
	int Reset(int nWidth,int nHeight,int nFrameRate);
	int StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub=false,int nSubScaleType=1,int nSubBitrate=0,int nSubQuant=0,int nSubQMin=0,int nSubQMax=0);
	int StopVideoStream(void);
	int RequestKeyFrame(void);
	int RequestSubKeyFrame(void);

	 
	int SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
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

	bool CanBeClosed(void);
	
	void OnVIDEC_AH400CallbackImage(VIDEC_Image*pImage);
	void OnVIDEC_AH400CallbackVideoStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	void OnVIDEC_AH400CallbackVideoSubStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen);
	void OnVIDEC_AH400CallbackAudioStreamData(unsigned char*pData,int nLen);
	void OnVIDEC_AH400CallbackMotionDetected(int*pResultList,int nResultCount);

	void OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage);
	
	virtual int AddImageSink(VIDEC_ImageSink*pImageSink);
	virtual void RemoveImageSink(VIDEC_ImageSink*pImageSink);
	
	virtual int AddStreamSink(VIDEC_StreamSink*pStreamSink);
	virtual void RemoveStreamSink(VIDEC_StreamSink*pStreamSink);
protected:
	VIDEC_AH400*		m_pAH400;
	IXPreProcess*		m_pIXPreProcess;

	unsigned int		m_nDevIndex;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nFrameRate;

	unsigned char*		m_pStreamBuf;
	int					m_nStreamBufLen;
	unsigned short		m_usSeqNo;

	bool				m_bEnableHikOSD;
};

#endif

#endif
