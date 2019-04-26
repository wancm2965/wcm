

#ifndef __D3D_RENDER_H__
#define __D3D_RENDER_H__

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include <dxva.h>
#include <windows.h>
#include "igfx_s3dcontrol.h"
#include "hw_device.h"
#include "d3d_device.h"
#include "HPAVC_D3DRender.h"
#include "sample_defs.h"
#include "base_allocator.h"
#include "d3d_allocator.h"
#include "d3d11_device.h"
#include "d3d11_allocator.h"

#include "../HPAVC_Util/XThreadBase.h"
#include "../HPAVC_Util/XListPtr.h"
#include "../HPAVC_Util/XAutoLock.h"

#include "../../VIDEC_FRE/VIDECFREAPI.h"

struct XSurfaceBuffer
{
	XSurfaceBuffer(void)
		: pYUVData(NULL)
		, nYLen(0)
		, nUVLen(0)
		, CropW(0)
		, CropH(0)
		, CropX(0)
		, CropY(0)
		, nFrameRate(0)
		, nPitch(0)
	{
	}
	virtual~XSurfaceBuffer(void)
	{
		if(pYUVData)
		{
			FREEMSG(pYUVData);
			pYUVData=NULL;
		}
	}
	void*			pYUVData;
	int				nYLen;
	int				nUVLen;
	int				CropW;
	int				CropH;
	int				CropX;
	int				CropY;
	int				nFrameRate;
	int				nPitch;
};


class HPAVC_API CD3DRender
	:public HPAVC_D3DRender
	,public CD3D9DeviceCallback
	//,public XThreadBase
	//,public FrameRateEvent
{
public:
    CD3DRender(HPAVC_D3DRENDER_CB_DC cbdc,void*pUserData);
	CD3DRender(HPAVC_D3DRenderCallback*pCallback);
   virtual ~CD3DRender();

	//hWnd:视频显示窗口句柄，成功返回0，失败返回-1
	virtual int Open(void* hWnd/*HWND*/,bool bCreateWnd,bool bD3D11);
	//关闭显示窗口
	virtual void Close(void);
	//设置窗口位置
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags); 

	virtual void* GetWnd(void);
	//显示
	virtual int RenderFrame(unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate);
	//保持当前图象到BMP文件
	virtual bool CaptureToBMPFile(const char*cszFile);

	virtual void SetSkipDisplay(int nSkipCount);

	//显示（内部调用）
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc);

	//显示（内部调用）
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface);

	//获取显示设备句柄（内部调用）
	virtual void* GetDeviceHandle(void);

	virtual void OnCD3D9DeviceCallbackDraw(HDC hDC);

	LRESULT OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam, LPARAM lParam); 

	//virtual void ThreadProcMain(void);


protected:
	mfxStatus LoadNextFrame(mfxFrameSurface1* pSurface,unsigned char*pYV12Data[]);
	mfxStatus CreateSurfRenderder(void);
	bool EnableDwmQueuing(int nFrameRate);
protected:
	HPAVC_D3DRenderCallback*m_pCallback;
	HPAVC_D3DRENDER_CB_DC	m_cbdc;
	void*					m_pUserData;
    mfxBitstream            m_mfxBS;
	HWND					m_hWnd;
	CHWDevice*				m_hwdev;

	int						m_nWndWidth;
	int						m_nWndHeight;

	mfxFrameSurface1*		m_pSurfRenderder;
	MFXFrameAllocator*		m_pMFXAllocator;
	mfxAllocatorParams*		m_pmfxAllocatorParams;
	mfxFrameAllocResponse	m_EncResponse;  // memory allocation response for encoder

	mfxFrameSurface1*		m_pSurface;
	mfxFrameAllocator*		m_pmfxAlloc;
	HBRUSH					m_hBrush;
	unsigned long			m_nLastTS;
	unsigned long			m_nFrameCount;
	unsigned long			m_nSkipFrameCount;

	unsigned long			m_nSkipCount;

	bool					m_bD3D11;

	bool					m_bWaitStop;
	XListPtr				m_listPacketData;
	XCritSec				m_csListPacketData;

	bool					m_bDwmEnabled;
	int						m_nFrameRate;

	std::string				m_strCaptureFilePath;
};

#endif // __PIPELINE_DECODE_H__
