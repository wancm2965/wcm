/* ****************************************************************************** *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 - 2013 Intel Corporation. All Rights Reserved.

\* ****************************************************************************** */

#pragma once

#if defined( _WIN32 ) || defined ( _WIN64 )

#include "hw_device.h"

#pragma warning(disable : 4201)
#include <d3d9.h>
#include <dxva2api.h>
#include <dxva.h>
#include <windows.h>



class CD3D9Device : public CHWDevice
{
public:
    CD3D9Device(CD3D9DeviceCallback&rCallback);
    virtual ~CD3D9Device();

    virtual mfxStatus Init(mfxHDL hWindow);
    virtual mfxStatus Reset(HMONITOR hMonitor);
	virtual mfxStatus CheckMonitor(void);
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl);
    virtual mfxStatus SetHandle(mfxHandleType type, mfxHDL hdl);
    virtual mfxStatus RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,bool bSkip,unsigned char*pNV12Data,int nFrameRate,int nWidth,int nHeight,int nPitch);	
	virtual mfxStatus Present(void);

	virtual void      Close() ;
            void      DefineFormat(bool isA2rgb10) { m_bIsA2rgb10 = (isA2rgb10) ? TRUE : FALSE; }

protected:
    mfxStatus CreateVideoProcessors();
    virtual mfxStatus FillD3DPP(mfxHDL hWindow, mfxU16 nViews, D3DPRESENT_PARAMETERS &D3DPP,D3DFORMAT format);

	static void GetD3d9BackBufSize(int& nWidth, int& nHeight);					//计算多屏时不同分辨率的最大bufsize
	static int s_nD9BackBufW;													//由于所有窗口只要计算一次即可，因此用静态变量，不需要反复去算
	static int s_nD9BackBufH;
private:
    IDirect3D9Ex*               m_pD3D9;
    IDirect3DDevice9Ex*         m_pD3DD9;
    IDirect3DDeviceManager9*    m_pDeviceManager9;
    D3DPRESENT_PARAMETERS       m_D3DPP;
    UINT                        m_resetToken;
	HWND						m_hWindow;
	UINT						m_nAdapter;
	HMONITOR					m_hMonitor;
	int							m_nWidth;
	int							m_nHeight;
    D3DSURFACE_DESC                 m_backBufferDesc;

    // service required to create video processors
    IDirectXVideoProcessorService*  m_pDXVAVPS;
    // right channel processor
    IDirectXVideoProcessor*         m_pDXVAVP_Right;

    // target rectangle
    RECT                            m_targetRect;

    // various structures for DXVA2 calls
    DXVA2_VideoDesc                 m_VideoDesc;
    DXVA2_VideoProcessBltParams     m_BltParams;
    DXVA2_VideoSample               m_Sample;

    BOOL                            m_bIsA2rgb10;

	float							m_nRefreshRate;

	CD3D9DeviceCallback&			m_rCallback;
	int								m_nPresentationIntervalType;
};

#endif // #if defined( _WIN32 ) || defined ( _WIN64 )
