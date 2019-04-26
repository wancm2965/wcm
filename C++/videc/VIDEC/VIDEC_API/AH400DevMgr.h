//AH400DevMgr.h
#ifndef __AH400DEVMGR_H__
#define __AH400DEVMGR_H__


#include "VIDEC_Config.h"

#ifdef SUPPORT_AH400

#include <winsock2.h>
#include <windows.h>

#include "VIDEC.h"
 
class AH400DevMgr
{
public:
	static int Init(void);
	static void Terminate(void);

	static int GetFormatCount(void);
	static int GetFormatName(int nIndex,char*szName,int nMaxCount);
	static int GetFormatSize(int nIndex,int&nWidth,int&nHeight);
	static bool IsValidFormatSize(int&nWidth,int&nHeight);


	static int GetDevCount(void);
	static int GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount);

	static int SetDevCap(unsigned int nDevIndex,int nWidth,int nHeight,int nFrameRate);
	static int SetVideoParam(unsigned int nDevIndex,int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/);
 	static int SetVideoGammaRGB(unsigned int nDevIndex,int nGammaR,int nGammaG,int nGammaB);

	static int AddImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink);
	static void RemoveImageSink(unsigned int nDevIndex,VIDEC_ImageSink*pImageSink);

	static int AddStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink);
	static void RemoveStreamSink(unsigned int nDevIndex,VIDEC_StreamSink*pStreamSink);

	static int StartVideoStream(unsigned int nDevIndex,int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax);
	static int StopVideoStream(unsigned int nDevIndex);
	static int RequestKeyFrame(unsigned int nDevIndex);
	static int RequestSubKeyFrame(unsigned int nDevIndex);

	static void SetInvert(unsigned int nDevIndex,int nInvert);
	static int GetInvert(unsigned int nDevIndex);
	static void SetDenoise(unsigned int nDevIndex,int nDenoise);
	static int GetDenoise(unsigned int nDevIndex);
	static void SetLevelFixer(unsigned int nDevIndex,int nLevelFixer);
	static int GetLevelFixer(unsigned int nDevIndex);
	
	static void EnableOSD(unsigned int nDevIndex,bool bEnable);
	static bool IsOSDEnable(unsigned int nDevIndex);
	static void SetOSDText(unsigned int nDevIndex,const char*cszText,int nAlign,int nTop,int nLeft);
	static void SetOSDFont(unsigned int nDevIndex,LOGFONTA fontOSD,COLORREF crText,COLORREF crBk);
	//设置遮掩并启动遮掩
	static int SetupMask(unsigned int nDevIndex,RECT *RectList, int iRectCount);
	//停止遮掩
	static int StopMask(unsigned int nDevIndex);
	static int StartMotionDetect(unsigned int nDevIndex);
	static int StopMotionDetect(unsigned int nDevIndex);
	static int SetupMotionDetect(unsigned int nDevIndex,int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount);
};

#endif

#endif
