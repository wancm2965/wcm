

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

	//hWnd:��Ƶ��ʾ���ھ�����ɹ�����0��ʧ�ܷ���-1
	virtual int Open(void* hWnd/*HWND*/,bool bCreateWnd,bool bD3D11);
	//�ر���ʾ����
	virtual void Close(void);
	//���ô���λ��
	virtual bool SetWindowPos(void* hWndInsertAfter/*HWND*/,int x,int y,int cx,int cy,unsigned int nFlags); 

	virtual void* GetWnd(void);
	//��ʾ
	virtual int RenderFrame(unsigned char*pYV12Data[],int nWidth,int nHeight, int nFrameRate);
	//���ֵ�ǰͼ��BMP�ļ�
	virtual bool CaptureToBMPFile(const char*cszFile);

	virtual void SetSkipDisplay(int nSkipCount);

	//��ʾ���ڲ����ã�
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface,mfxFrameAllocator* pmfxAlloc);

	//��ʾ���ڲ����ã�
	virtual int RenderFrameSurface(mfxFrameSurface1* pSurface);

	//��ȡ��ʾ�豸������ڲ����ã�
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
