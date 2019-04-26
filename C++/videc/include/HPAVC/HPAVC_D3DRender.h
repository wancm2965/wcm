
#ifndef __HPAVC_D3DRENDER_H__
#define __HPAVC_D3DRENDER_H__


#include "HPAVC_Export.h"

#include "mfx/mfxvideo.h"


mfxStatus	HPAVC_API MFX_Init(void);
int			HPAVC_API GetIntelAdapterNum(void);
mfxIMPL		HPAVC_API GetIntelAdapterIMPL(void);

class HPAVC_D3DRenderCallback
{
public:
	HPAVC_D3DRenderCallback(void){}
	virtual~HPAVC_D3DRenderCallback(void){};
public:
	virtual void OnHPAVC_D3DRenderCallbackDrawDC(void* hDC/*HDC*/){};
};

class HPAVC_API HPAVC_D3DRender
{
public:
	HPAVC_D3DRender(void){};
	virtual~HPAVC_D3DRender(void){};
public:
	//hWnd:视频显示窗口句柄，成功返回0，失败返回-1
	virtual int Open(void* hWnd/*HWND*/,bool bCreateWnd = true,bool bD3D11 = true)=0;

	//关闭显示窗口
	virtual void Close(void)=0;

	//设置窗口位置
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags)=0; 
	virtual void* GetWnd(void)=0;
	//显示
	virtual int RenderFrame(unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate)=0;

	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile)=0;

	//设置跳帧的数量
	virtual void SetSkipDisplay(int nSkipCount)=0;

	//显示（内部调用）
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc)=0;
	//显示（内部调用）
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface)=0;
	//获取显示设备句柄（内部调用）
	virtual void* GetDeviceHandle(void)=0;      

	static HPAVC_D3DRender*Create(HPAVC_D3DRenderCallback*pCallback);
};

typedef void (*HPAVC_D3DRENDER_CB_DC)(const void*pUserData,void* hDC/*HDC*/);

HPAVC_API void*HPAVC_D3DRenderOpen(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData,void* hWnd, bool bCreateWnd = true,bool bD3D11 = true);
HPAVC_API void HPAVC_D3DRenderClose(void*pHandle);
HPAVC_API int  HPAVC_D3DRenderPlayFrame(void*pHandle, unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate);
HPAVC_API int  HPAVC_D3DRenderPlaySurface(void*pHandle, mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc);
HPAVC_API bool HPAVC_D3DRenderSetWindowPos(void*pHandle,void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags);
HPAVC_API void* HPAVC_D3DRenderGetWnd(void*pHandle);
HPAVC_API int  HPAVC_D3DRenderSkipDisplay(void*pHandle,int nSkipCount/*本次跳帧的数量*/);
HPAVC_API bool HPAVC_D3DRenderCaptureToBMPFile(void*pHandle,const char*cszFile);

#endif
