
#ifndef __HPAVC_CAPTURE_H__
#define __HPAVC_CAPTURE_H__

#include "HPAVC_Export.h"

#define HPAVC_BASELINE_PROFILE	0
#define HPAVC_MAIN_PROFILE		1
#define HPAVC_HIGH_PROFILE		2

class HPAVC_CaptureCallback
{
public:
	HPAVC_CaptureCallback(void){}
	virtual~HPAVC_CaptureCallback(void){};
public:
	virtual void OnHPAVC_CaptureCallbackDrawDC(void* hDC/*HDC*/){};
	virtual void OnHPAVC_CaptureCallbackPacketData(void*pData,int nLen,int nKeyFrame,int nWidth,int nHeight){};
};

class HPAVC_API HPAVC_Capture
{
public:
	HPAVC_Capture(void){};
	virtual~HPAVC_Capture(void){};
public:
	//成功返回0，失败返回-1
	virtual int Open(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax)=0;
	
	virtual bool IsOpen(void)=0;

	//关闭显示窗口
	virtual void Close(void)=0;

	//编码&显示
	virtual int	EncodeData(unsigned char*pData,int nLen,int nRequestKeyFrame) = 0;

	//取视频宽度
	virtual int GetVideoWidth(void)=0;

	//取视频高度
	virtual int GetVideoHeight(void)=0;

	//设置窗口位置
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)=0; 

	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile)=0;

	//设置视频属性//Hue:-180;180;0,Saturation:0;10;1,Contrast:0;10;1,Brightness -100;100;0,Brightness:-100; 100;0,DetailFactor:0;100;0,DenoiseFactor:0;100;0
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)=0;

	static HPAVC_Capture*Create(HPAVC_CaptureCallback*pCallback,void* hWnd/*HWND*/);
};


typedef void (*HPAVC_CAPTURE_CB_DC)(const void*pUserData,void* hDC/*HDC*/);
typedef void (*HPAVC_CAPTURE_CB_DATA)(const void*pUserData,void*pData,int nLen,int nKeyFrame,int nWidth,int nHeight);

HPAVC_API void*HPAVC_CaptureOpen(HPAVC_CAPTURE_CB_DC cbdc,HPAVC_CAPTURE_CB_DATA cbdata,void*pUserData,void* hWnd,int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nKeyIntMax);
HPAVC_API void HPAVC_CaptureClose(void*pHandle);
HPAVC_API int  HPAVC_CaptureEncodeData(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame);
HPAVC_API int  HPAVC_CaptureGetVideoWidth(void*pHandle);
HPAVC_API int  HPAVC_CaptureGetVideoHeigh(void*pHandle);
HPAVC_API double  HPAVC_CaptureGetFrameRate(void*pHandle);
HPAVC_API bool HPAVC_CaptureCaptureToBMPFile(void*pHandle,const char*cszFile);
HPAVC_API bool HPAVC_CaptureSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
HPAVC_API int  HPAVC_CaptureSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);


#endif
