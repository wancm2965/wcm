/* ****************************************************************************** *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 - 2013 Intel Corporation. All Rights Reserved.

\* ****************************************************************************** */

#pragma once

#if defined( _WIN32 ) || defined ( _WIN64 )

#include "sample_defs.h" // defines MFX_D3D11_RENDER_SUPPORT

#if MFX_D3D11_RENDER_SUPPORT
#include "hw_device.h"
#include <windows.h>
#include <d3d11.h>
#include <atlbase.h>

#include <dxgi1_2.h>

class CD3D11Device: public CHWDevice
{
public:
    CD3D11Device(CD3D9DeviceCallback&rCallback,bool bHardWareDecoder);
    virtual ~CD3D11Device();
    virtual mfxStatus Init(mfxHDL hWindow);
    virtual mfxStatus Reset(HMONITOR hMonitor);
	virtual mfxStatus CheckMonitor(void);
    virtual mfxStatus GetHandle(mfxHandleType type, mfxHDL *pHdl);
    virtual mfxStatus SetHandle(mfxHandleType type, mfxHDL hdl);
    virtual mfxStatus RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,bool bSkip,unsigned char*pNV12Data,int nFrameRate,int nWidth,int nHeight,int nPitch);
	virtual mfxStatus Present(void);
    virtual void      UpdateTitle(double /*fps*/) { }
    virtual void      Close();
            void      DefineFormat(bool isA2rgb10) { m_bIsA2rgb10 = (isA2rgb10) ? TRUE : FALSE; }	

protected:
    virtual mfxStatus FillSCD(mfxHDL hWindow, DXGI_SWAP_CHAIN_DESC& scd);
    virtual mfxStatus FillSCD1(DXGI_SWAP_CHAIN_DESC1& scd);
    mfxStatus CreateVideoProcessor(int nFrameRate,int nWidth,int nHeight);

    CComPtr<ID3D11Device>                   m_pD3D11Device;
    CComPtr<ID3D11DeviceContext>            m_pD3D11Ctx;
    CComQIPtr<ID3D11VideoDevice>            m_pDX11VideoDevice;
    CComQIPtr<ID3D11VideoContext>           m_pVideoContext;
    CComPtr<ID3D11VideoProcessorEnumerator> m_VideoProcessorEnum;

    CComQIPtr<IDXGIDevice1>                 m_pDXGIDev;
    CComQIPtr<IDXGIAdapter>                 m_pAdapter;

    CComPtr<IDXGIFactory2>                  m_pDXGIFactory;

    CComPtr<IDXGISwapChain1>                m_pSwapChain;
    CComPtr<ID3D11VideoProcessor>           m_pVideoProcessor;

private:
    CComPtr<ID3D11VideoProcessorInputView>  m_pInputViewRight;
    CComPtr<ID3D11VideoProcessorOutputView> m_pOutputView;

    CComPtr<ID3D11Texture2D>                m_pDXGIBackBuffer;
    CComPtr<ID3D11Texture2D>                m_pTempTexture;
    CComPtr<IDXGIDisplayControl>            m_pDisplayControl;
    CComPtr<IDXGIOutput>                    m_pDXGIOutput;
    mfxU16                                  m_nViews;
    BOOL                                    m_bDefaultStereoEnabled;
    UINT                                    m_nSyncInterval;
    BOOL                                    m_bIsA2rgb10;

	CD3D9DeviceCallback&					m_rCallback;

	int										m_nRefreshRate;
	HWND									m_hWindow;
	UINT									m_nAdapter;
	HMONITOR								m_hMonitor;

	RECT									m_rect;
	bool									m_bHardWareDecoder;

	int										m_nWidth;
	int										m_nHeight;
};

#if 0

//D3D的一些头文件 
#include <dxgi.h> 
#include <d3dcommon.h> 
#include <d3d11.h> 
#include <d3dx10math.h>


class D3DClass 
{ 
public: 
	D3DClass(void); 
	D3DClass(const D3DClass&); 
	~D3DClass(void);

	bool Initialize(int, int, bool, HWND, bool, float, float); 
	void Shutdown();

	void BeginScene(float, float, float, float); 
	void EndScene();

	ID3D11Device* GetDevice(); 
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&); 
	void GetWorldMatrix(D3DXMATRIX&); 
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

private: 
	bool m_vsync_enabled; //是否启用垂直同步 
	int m_videoCardMemory; //显存大小 
	char m_videoCardDescription[128]; //显卡名字 
	IDXGISwapChain* m_swapChain; //交换链对象 
	ID3D11Device* m_device; //设备对象 
	ID3D11DeviceContext* m_deviceContext; //设备上下文对象 
	ID3D11RenderTargetView* m_renderTargetView; //渲染目标视图 
	ID3D11Texture2D* m_depthStencilBuffer; 
	ID3D11DepthStencilState* m_depthStencilState; 
	ID3D11DepthStencilView* m_depthStencilView; //深度目标视图 
	ID3D11RasterizerState* m_rasterState; //渲染状态 
	D3DXMATRIX m_projectionMatrix; //投影矩阵 
	D3DXMATRIX m_worldMatrix; //世界坐标系矩阵 
	D3DXMATRIX m_orthoMatrix; //正交投影矩阵


}; 

#endif

#endif //#if defined( _WIN32 ) || defined ( _WIN64 )
#endif //#if MFX_D3D11_RENDER_SUPPORT
