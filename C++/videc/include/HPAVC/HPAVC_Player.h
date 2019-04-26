
#ifndef __HPAVC_PLAYER_H__
#define __HPAVC_PLAYER_H__


#include "HPAVC_Export.h"

class HPAVC_PlayerCallback
{
public:
	HPAVC_PlayerCallback(void){}
	virtual~HPAVC_PlayerCallback(void){};
public:
	virtual void OnHPAVC_PlayerCallbackDrawDC(void* hDC/*HDC*/){};
};

class HPAVC_API HPAVC_Player
{
public:
	HPAVC_Player(void){};
	virtual~HPAVC_Player(void){};
public:
	//hWnd:视频显示窗口句柄，成功返回0，失败返回-1
	virtual int Open(void* hWnd/*HWND*/)=0;

	//关闭显示窗口
	virtual void Close(void)=0;

	//取视频宽度
	virtual int GetVideoWidth(void)=0;

	//取视频高度
	virtual int GetVideoHeight(void)=0;

	//设置窗口位置
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)=0; 

	//解码&显示
	virtual int PlayData(void*pPacketData,int nPacketSize)=0;

	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile)=0;
   
	//设置视频属性//Hue:-180;180;0,Saturation:0;10;1,Contrast:0;10;1,Brightness -100;100;0,Brightness:-100; 100;0,DetailFactor:0;100;0,DenoiseFactor:0;100;0
	virtual int SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise)=0;

	static HPAVC_Player*Create(HPAVC_PlayerCallback*pCallback);
};


typedef void (*HPAVC_PLAYER_CB_DC)(const void*pUserData,void* hDC/*HDC*/);

HPAVC_API void*HPAVC_PlayerOpen(HPAVC_PLAYER_CB_DC cbdc,void*pUserData,void* hWnd);
HPAVC_API void HPAVC_PlayerClose(void*pHandle);
HPAVC_API int  HPAVC_PlayerPlayData(void*pHandle,void*pData,int nLen);
HPAVC_API int  HPAVC_PlayerGetVideoWidth(void*pHandle);
HPAVC_API int  HPAVC_PlayerGetVideoHeigh(void*pHandle);
HPAVC_API double  HPAVC_PlayerGetFrameRate(void*pHandle);
HPAVC_API bool HPAVC_PlayerCaptureToBMPFile(void*pHandle,const char*cszFile);
HPAVC_API bool HPAVC_PlayerSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
HPAVC_API void* HPAVC_PlayerGetWnd(void*pHandle);
HPAVC_API int  HPAVC_PlayerSetVideoParam(void*pHandle,int nBrightness, int nContrast, int nSaturation, int nHue,int nDetail,int nDenoise);


#endif
