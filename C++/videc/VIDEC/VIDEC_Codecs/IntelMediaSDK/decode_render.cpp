//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//

#include <windowsx.h>

#include <dwmapi.h>
#include <mmsystem.h>

#include "decode_render.h"
#include "sample_defs.h"

#pragma warning(disable : 4100)

const UINT DWM_BUFFER_COUNT  = 4;

CD3D9Device::CD3D9Device()
{
    m_pD3D9 = NULL;
    m_pD3DD9 = NULL;
    m_pDeviceManager9 = NULL;
    MSDK_ZERO_MEMORY(m_D3DPP);
    m_resetToken = 0;

    MSDK_ZERO_MEMORY(m_backBufferDesc);
    m_pDXVAVPS = NULL;
    m_pDXVAVP_Left = NULL;
    m_pDXVAVP_Right = NULL;


    MSDK_ZERO_MEMORY(m_targetRect);

    MSDK_ZERO_MEMORY(m_VideoDesc);
    MSDK_ZERO_MEMORY(m_BltParams);
    MSDK_ZERO_MEMORY(m_Sample);

    // Initialize DXVA structures

    DXVA2_AYUVSample16 color = {         
        0x8000,          // Cr
        0x8000,          // Cb
        0x1000,          // Y
        0xffff           // Alpha
    };

    DXVA2_ExtendedFormat format =   {           // DestFormat
        DXVA2_SampleProgressiveFrame,           // SampleFormat
        DXVA2_VideoChromaSubsampling_MPEG2,     // VideoChromaSubsampling
        DXVA_NominalRange_0_255,                // NominalRange
        DXVA2_VideoTransferMatrix_BT709,        // VideoTransferMatrix
        DXVA2_VideoLighting_bright,             // VideoLighting
        DXVA2_VideoPrimaries_BT709,             // VideoPrimaries
        DXVA2_VideoTransFunc_709                // VideoTransferFunction            
    };

    // init m_VideoDesc structure
    memcpy(&m_VideoDesc.SampleFormat, &format, sizeof(DXVA2_ExtendedFormat));
    m_VideoDesc.SampleWidth                         = 0;
    m_VideoDesc.SampleHeight                        = 0;    
    m_VideoDesc.InputSampleFreq.Numerator           = 60;
    m_VideoDesc.InputSampleFreq.Denominator         = 1;
    m_VideoDesc.OutputFrameFreq.Numerator           = 60;
    m_VideoDesc.OutputFrameFreq.Denominator         = 1;
    
    // init m_BltParams structure
    memcpy(&m_BltParams.DestFormat, &format, sizeof(DXVA2_ExtendedFormat));
    memcpy(&m_BltParams.BackgroundColor, &color, sizeof(DXVA2_AYUVSample16));  
      
    // init m_Sample structure
    m_Sample.Start = 0;
    m_Sample.End = 1;
    m_Sample.SampleFormat = format;
    m_Sample.PlanarAlpha.Fraction = 0;
    m_Sample.PlanarAlpha.Value = 1;   

    m_overlaySupported = false;
}

mfxStatus CD3D9Device::Init(WindowHandle hWindow)
{
	HRESULT hr;

	m_pD3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pD3D9)
		return MFX_ERR_DEVICE_FAILED;

    ZeroMemory(&m_D3DPP, sizeof(m_D3DPP));
    m_D3DPP.Windowed = true;
    m_D3DPP.hDeviceWindow = (HWND)hWindow;

    m_D3DPP.Flags                      = D3DPRESENTFLAG_VIDEO;
    m_D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    m_D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;
    m_D3DPP.BackBufferCount            = 1;
    m_D3DPP.BackBufferFormat           = D3DFMT_X8R8G8B8;

    m_D3DPP.BackBufferWidth  = GetSystemMetrics(SM_CXSCREEN);
    m_D3DPP.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);

    //
    // Mark the back buffer lockable if software DXVA2 could be used.
    // This is because software DXVA2 device requires a lockable render target
    // for the optimal performance.
    //
    {
        m_D3DPP.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }


    
    m_D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

    hr = m_pD3D9->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        (HWND)hWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
        &m_D3DPP,
        &m_pD3DD9);
    if (FAILED(hr))
        return MFX_ERR_NULL_PTR;

    hr = m_pD3DD9->Reset(&m_D3DPP);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;    

    hr = m_pD3DD9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;    

    UINT resetToken = 0;

    hr = DXVA2CreateDirect3DDeviceManager9(&resetToken, &m_pDeviceManager9);
    if (FAILED(hr)) 
        return MFX_ERR_NULL_PTR;

    hr = m_pDeviceManager9->ResetDevice(m_pD3DD9, resetToken);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;

    m_resetToken = resetToken;

    return MFX_ERR_NONE;
}

mfxStatus CD3D9Device::Reset()
{
    HRESULT hr = NO_ERROR;

    D3DPRESENT_PARAMETERS d3dpp=m_D3DPP;
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    D3DDISPLAYMODE d3dmodeTemp;
    m_pD3DD9->GetDisplayMode(0, &d3dmodeTemp);

    d3dpp.BackBufferWidth  = d3dmodeTemp.Width;
    d3dpp.BackBufferHeight = d3dmodeTemp.Height;

    // Reset will change the parameters, so use a copy instead.
    D3DPRESENT_PARAMETERS d3dppTmp=d3dpp;
    hr = m_pD3DD9->Reset(&d3dppTmp);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;
    m_D3DPP = d3dpp;

    hr = m_pDeviceManager9->ResetDevice(m_pD3DD9, m_resetToken);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;

    return MFX_ERR_NONE;
}

void CD3D9Device::Close()
{
    MSDK_SAFE_RELEASE(m_pDXVAVP_Left);
    MSDK_SAFE_RELEASE(m_pDXVAVP_Right);
    MSDK_SAFE_RELEASE(m_pDXVAVPS);

    MSDK_SAFE_RELEASE(m_pDeviceManager9);
    MSDK_SAFE_RELEASE(m_pD3DD9);
    MSDK_SAFE_RELEASE(m_pD3D9);
}

CD3D9Device::~CD3D9Device()
{
    Close();
}

mfxStatus CD3D9Device::GetHandle(mfxHandleType type, mfxHDL *pHdl)
{
    if (MFX_HANDLE_DIRECT3D_DEVICE_MANAGER9 == type && pHdl != NULL)
    {
        *pHdl = m_pDeviceManager9;

        return MFX_ERR_NONE;
    }
    return MFX_ERR_UNSUPPORTED;
}
