/* ****************************************************************************** *\

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011 - 2013 Intel Corporation. All Rights Reserved.

\* ****************************************************************************** */

#if defined(WIN32) || defined(WIN64)

//prefast singnature used in combaseapi.h
#ifndef _PREFAST_
    #pragma warning(disable:4068)
#endif

#include "d3d_device.h"
#include "sample_defs.h"

#include "atlbase.h"

//#include <dxgi1_2.h>

/*
由于d3d9的backbuf按显示器当前分辨率大小分配会出现黑屏的情况，定位后发现是由于应用层的窗口大小超出当前分辨率，因此应用层提供该算法按该算法来分配d3d9的backbuf
*/
#include "math.h"
void GetD3DSize(int nScreenX, int nScreenY, int& nD3DX, int &nD3DY)				//应用端提供
{
	if ((((float)nScreenX/nScreenY)>((float)11/9)) && (((float)nScreenX/nScreenY) < ((float)16/9)))
	{

		nD3DX = (int)ceil((float)(nScreenY*16)/9);
		nD3DY = (int)ceil((float)(nScreenX*9)/11);
		return;
	}

	if (((float)nScreenX/nScreenY) > ((float)11/9))
	{
		nD3DX = nScreenX;
		nD3DY = (int)ceil((float)(nScreenX*9)/11);
		return;
	}

	if (((float)nScreenX/nScreenY) < ((float)16/9))
	{
		nD3DY = nScreenY;
		nD3DX = (int)ceil((float)(nScreenY*16)/9);
	}
}

int CD3D9Device::s_nD9BackBufW = 0;
int CD3D9Device::s_nD9BackBufH = 0;

CD3D9Device::CD3D9Device(CD3D9DeviceCallback&rCallback)
:m_rCallback(rCallback)
{
    m_pD3D9 = NULL;
    m_pD3DD9 = NULL;
    m_pDeviceManager9 = NULL;
    MSDK_ZERO_MEMORY(m_D3DPP);
    m_resetToken = 0;

    MSDK_ZERO_MEMORY(m_backBufferDesc);
    m_pDXVAVPS = NULL;
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
    MSDK_MEMCPY_VAR(m_VideoDesc.SampleFormat, &format, sizeof(DXVA2_ExtendedFormat));
    m_VideoDesc.SampleWidth                         = 0;
    m_VideoDesc.SampleHeight                        = 0;
    m_VideoDesc.InputSampleFreq.Numerator           = 60;
    m_VideoDesc.InputSampleFreq.Denominator         = 1;
    m_VideoDesc.OutputFrameFreq.Numerator           = 60;
    m_VideoDesc.OutputFrameFreq.Denominator         = 1;

    // init m_BltParams structure
    MSDK_MEMCPY_VAR(m_BltParams.DestFormat, &format, sizeof(DXVA2_ExtendedFormat));
    MSDK_MEMCPY_VAR(m_BltParams.BackgroundColor, &color, sizeof(DXVA2_AYUVSample16));

    // init m_Sample structure
    m_Sample.Start = 0;
    m_Sample.End = 1;
    m_Sample.SampleFormat = format;
    m_Sample.PlanarAlpha.Fraction = 0;
    m_Sample.PlanarAlpha.Value = 1;

	m_nRefreshRate = 60;
    m_bIsA2rgb10 = FALSE;
}

void CD3D9Device::GetD3d9BackBufSize(int& nWidth, int& nHeight)
{
	int nMaxW = 0;
	int nMaxH = 0;
	DISPLAY_DEVICE DisplayDevice;  
	ZeroMemory(&DisplayDevice, sizeof(DISPLAY_DEVICE));  
	DisplayDevice.cb = sizeof(DisplayDevice);  

	for (int i = 0; EnumDisplayDevices(NULL, i, &DisplayDevice, 0); i++)  
	{
		DEVMODE DevMode; 
		if (EnumDisplaySettings(DisplayDevice.DeviceName, ENUM_REGISTRY_SETTINGS, &DevMode))
		{
			if (DevMode.dmPelsWidth > nMaxW)
				nMaxW = DevMode.dmPelsWidth;
			if (DevMode.dmPelsHeight > nMaxH)
				nMaxH = DevMode.dmPelsHeight;
		}
	}
	GetD3DSize(nMaxW, nMaxH, nWidth, nHeight);
	nHeight += 4;								//由于2014.10的一个视频库对设置的窗口的高度增加了4个像素（根据日志是为了解决1080i边缘绿边的问题），因此这里需将应用的窗口高度再加4；
}

mfxStatus CD3D9Device::FillD3DPP(mfxHDL hWindow, mfxU16 nViews, D3DPRESENT_PARAMETERS &D3DPP,D3DFORMAT format)
{
    mfxStatus sts = MFX_ERR_NONE;

    D3DPP.Windowed = true;
    D3DPP.hDeviceWindow = (HWND)hWindow;

    D3DPP.Flags                      = D3DPRESENTFLAG_VIDEO;
    D3DPP.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	//if (m_nPresentationIntervalType == 1)
	//{
	//	D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_ONE;
	//}
	//else
	{
		D3DPP.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

    D3DPP.BackBufferCount            = 1;
    D3DPP.BackBufferFormat           = format;//(m_bIsA2rgb10) ? D3DFMT_A2R10G10B10 : D3DFMT_X8R8G8B8;

	if (s_nD9BackBufW <= 0 || s_nD9BackBufH <= 0)
	{
		GetD3d9BackBufSize(s_nD9BackBufW, s_nD9BackBufH);
		
		char tmp[64];
		sprintf_s(tmp, "GetD3DSize  w:%d h:%d\n", s_nD9BackBufW, s_nD9BackBufH);
		OutputDebugStringA(tmp);
	}

	if (s_nD9BackBufW <= 0 || s_nD9BackBufH <= 0)
		OutputDebugStringA("GetD3dSize 0!\n");

	D3DPP.BackBufferWidth  = s_nD9BackBufW;
	D3DPP.BackBufferHeight = s_nD9BackBufH;

    //
    // Mark the back buffer lockable if software DXVA2 could be used.
    // This is because software DXVA2 device requires a lockable render target
    // for the optimal performance.
    //
    {
        D3DPP.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    }

//     bool isOverlaySupported = false;
//     if (2 == nViews && !isOverlaySupported)
//         return MFX_ERR_UNSUPPORTED;


    D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;

    return sts;
}

mfxStatus CD3D9Device::Init(mfxHDL hWindow)
{
    mfxStatus sts = MFX_ERR_NONE;

	m_hWindow = (HWND)hWindow;

	HRESULT hr = S_OK;

	if (m_pD3D9 == NULL)
	{
		hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D9);
		if (!m_pD3D9 || FAILED(hr))
			return MFX_ERR_DEVICE_FAILED;	
	}
	else
	{
		MSDK_SAFE_RELEASE(m_pD3DD9);
	}

	HMONITOR hMonitor = MonitorFromWindow((HWND)hWindow, MONITOR_DEFAULTTONEAREST);
	int nAdapter=D3DADAPTER_DEFAULT;
	int nMaxAdapter = m_pD3D9->GetAdapterCount();
	int nAdapterIndex;
	// iterate over adapters until we error or find a match
	for (nAdapterIndex = 0; nAdapterIndex < nMaxAdapter; nAdapterIndex++)
	{
		HMONITOR hCurMonitor;
		hCurMonitor = m_pD3D9->GetAdapterMonitor(nAdapterIndex);
		if (hCurMonitor == hMonitor)
		{
			nAdapter=nAdapterIndex;
			break;
		}
	}
	m_hMonitor=hMonitor;
	m_nAdapter=nAdapter;

	D3DDISPLAYMODE d3ddm;
	hr = m_pD3D9->GetAdapterDisplayMode(nAdapter, &d3ddm);
	if (FAILED(hr)) 
	{
		return MFX_ERR_DEVICE_FAILED;
	}
	m_nRefreshRate=d3ddm.RefreshRate;
	/*
	//////////////////////////////////////////////////////////////////////////
	IDXGIFactory* factory; 
	HRESULT result; 
	IDXGIAdapter* adapter; 
	IDXGIOutput* adapterOutput; 
	unsigned int numModes, i, numerator, denominator; 
	DXGI_MODE_DESC* displayModeList; 
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory); 
	if(SUCCEEDED(result)) 
	{ 
		result = factory->EnumAdapters(nAdapter, &adapter); 
		if(SUCCEEDED(result)) 
		{ 
			// 得到主适配器的输出. 
			result = adapter->EnumOutputs(0, &adapterOutput); 
			if(SUCCEEDED(result)) 
			{
				result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL); 
				if(SUCCEEDED(result)) 
				{ 
					displayModeList = new DXGI_MODE_DESC[numModes]; 
					if(displayModeList) 
					{ 
						result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList); 
						if(SUCCEEDED(result)) 
						{ 
							for(i=0; i<numModes; i++) 
							{ 
								if(displayModeList[i].Width == (unsigned int)d3ddm.Width && displayModeList[i].Height == (unsigned int)d3ddm.Height) 
								{
									numerator = displayModeList[i].RefreshRate.Numerator; 
									denominator = displayModeList[i].RefreshRate.Denominator;
									float fRate = ((float)numerator)/((float)denominator);
									if(abs(fRate - m_nRefreshRate) < 0.9)
									{
										m_nRefreshRate = fRate;
										DebugOutput("============Adapter = %u RefreshRate = %0.10lf this = %u============\n",nAdapter,m_nRefreshRate,this);
										//break;
									}
								} 
							} 		
						}
						delete [] displayModeList; 
						displayModeList = 0;
					}
				}
				adapterOutput->Release(); 
				adapterOutput = 0;
			}
			adapter->Release(); 
			adapter = 0;
		}
		factory->Release(); 
		factory = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	*/
	ZeroMemory(&m_D3DPP, sizeof(m_D3DPP));
	sts = FillD3DPP(hWindow, 1, m_D3DPP,d3ddm.Format);
	MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

	D3DCAPS9  caps;
	int       vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	hr = m_pD3D9->GetDeviceCaps(nAdapter, D3DDEVTYPE_HAL, &caps);
	if (SUCCEEDED(hr))
	{
		if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		{
			vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		} 	
	}

	vp |= D3DCREATE_MULTITHREADED;// | D3DCREATE_FPU_PRESERVE;//

	hr = m_pD3D9->CreateDeviceEx(
		nAdapter,
		D3DDEVTYPE_HAL,
		m_hWindow,
		vp,//D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
		&m_D3DPP,
		NULL,
		&m_pD3DD9);
	if (FAILED(hr))
		return MFX_ERR_NULL_PTR;

    if(hWindow)
    {
        hr = m_pD3DD9->ResetEx(&m_D3DPP, NULL);
        if (FAILED(hr))
            return MFX_ERR_UNDEFINED_BEHAVIOR;
        hr = m_pD3DD9->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
        if (FAILED(hr))
            return MFX_ERR_UNDEFINED_BEHAVIOR;
    }
	if (m_pDeviceManager9 == NULL)
	{
		hr = DXVA2CreateDirect3DDeviceManager9(&m_resetToken, &m_pDeviceManager9);
		if (FAILED(hr))
			return MFX_ERR_NULL_PTR;
	}

    hr = m_pDeviceManager9->ResetDevice(m_pD3DD9, m_resetToken);
    if (FAILED(hr))
        return MFX_ERR_UNDEFINED_BEHAVIOR;

    return MFX_ERR_NONE;
}

mfxStatus CD3D9Device::CheckMonitor(void)
{
	mfxStatus hr = MFX_ERR_NONE;
	HMONITOR hMonitor = MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST);
	if (hMonitor!=m_hMonitor)
	{
		hr=Init(m_hWindow);
		if (hr!=MFX_ERR_NONE)
		{
			OutputDebugString(_T("RenderFrame Failed!\n"));
			return MFX_ERR_UNKNOWN;
		}
		return MFX_WRN_IN_EXECUTION;
	}
	return hr;
}

mfxStatus CD3D9Device::Reset(HMONITOR hMonitor)
{
    return MFX_ERR_NONE;
}

void CD3D9Device::Close()
{
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

mfxStatus CD3D9Device::SetHandle(mfxHandleType type, mfxHDL hdl)
{
    return MFX_ERR_UNSUPPORTED;
}

static int g_nCallbackDC = -1;

int  FRE_GetCallbackDC(void)
{
	if (g_nCallbackDC == -1)
	{
		TCHAR chIniPath[1024];
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		lstrcat(chIniPath,_T("\\VIDEC.ini"));	

		g_nCallbackDC=GetPrivateProfileInt(_T("VIDEC"), _T("CallbackDC") ,0, chIniPath);		

		//TCHAR strValue[128];
		//_stprintf(strValue,_T("%d"),g_nCallbackDC);		
		//WritePrivateProfileString(_T("VIDEC"), _T("CallbackDC")  , strValue, chIniPath);
	}
	return g_nCallbackDC;
}

mfxStatus CD3D9Device::RenderFrame(mfxFrameSurface1 * pSurface, mfxFrameAllocator * pmfxAlloc,bool bSkip,unsigned char*pNV12Data,int nFrameRate,int nWidth,int nHeight,int nPitch)
{
    HRESULT hr = S_OK;
	if (m_pD3DD9 == NULL)
	{
		return MFX_ERR_UNKNOWN;
	}
	

    MSDK_CHECK_POINTER(pSurface, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pDeviceManager9, MFX_ERR_NOT_INITIALIZED);
	MSDK_CHECK_POINTER(pmfxAlloc, MFX_ERR_NULL_PTR);

    hr = m_pD3DD9->TestCooperativeLevel();

    switch (hr)
    {
        case D3D_OK :
            break;

        case D3DERR_DEVICELOST :
        {
            return MFX_ERR_DEVICE_LOST;
        }

        case D3DERR_DEVICENOTRESET :
            {
            return MFX_ERR_UNKNOWN;
        }

        default :
        {
            return MFX_ERR_UNKNOWN;
        }
    }

    // source rectangle
    RECT source = { pSurface->Info.CropX, pSurface->Info.CropY,
                    pSurface->Info.CropX + pSurface->Info.CropW,
                    pSurface->Info.CropY + pSurface->Info.CropH };

    CComPtr<IDirect3DSurface9> pBackBuffer;
    hr = m_pD3DD9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	if (pBackBuffer == NULL)
	{
		return MFX_ERR_UNKNOWN;
	}
	
    D3DSURFACE_DESC dsc ;
    pBackBuffer->GetDesc(&dsc);

    RECT dest = {0};
    GetClientRect(m_D3DPP.hDeviceWindow, &m_targetRect);
    dest = m_targetRect;

    hr = m_pD3DD9->StretchRect((IDirect3DSurface9*)pSurface->Data.MemId, &source, pBackBuffer, &dest, D3DTEXF_LINEAR);
    if (FAILED(hr))
    {
        return MFX_ERR_UNKNOWN;
    }

	if (FRE_GetCallbackDC() == 1)
	{
		HDC hDC=NULL;
		pBackBuffer->GetDC(&hDC);
		if (hDC)
		{
			m_rCallback.OnCD3D9DeviceCallbackDraw(hDC);

			pBackBuffer->ReleaseDC(hDC);
			hDC=NULL;
		}
	}
	
	

	//return MFX_ERR_NONE;

     if (SUCCEEDED(hr))
     {
         hr = m_pD3DD9->Present(&m_targetRect, &m_targetRect, NULL, NULL);
     }
 
     return SUCCEEDED(hr) ? MFX_ERR_NONE : MFX_ERR_DEVICE_FAILED;
}

mfxStatus CD3D9Device::Present(void)
{
	//m_pD3DD9->Present(&m_targetRect, &m_targetRect, NULL, NULL);
	return MFX_ERR_NONE;
// 	HRESULT hr = S_OK;
// 	if (m_pD3DD9)
// 	{
// 		RECT targetRect = {0};
// 		GetClientRect(m_D3DPP.hDeviceWindow, &targetRect);
//		hr = m_pD3DD9->Present(&m_targetRect, &m_targetRect, NULL, NULL);
//	}	
//	return SUCCEEDED(hr) ? MFX_ERR_NONE : MFX_ERR_DEVICE_FAILED;
}

mfxStatus CD3D9Device::CreateVideoProcessors()
{
   MSDK_SAFE_RELEASE(m_pDXVAVP_Right);

   HRESULT hr ;

   ZeroMemory(&m_backBufferDesc, sizeof(m_backBufferDesc));
   IDirect3DSurface9 *backBufferTmp = NULL;
   hr = m_pD3DD9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferTmp);
   if (NULL != backBufferTmp)
       backBufferTmp->GetDesc(&m_backBufferDesc);
   MSDK_SAFE_RELEASE(backBufferTmp);

   if (SUCCEEDED(hr))
   {
       // Create DXVA2 Video Processor Service.
       hr = DXVA2CreateVideoService(m_pD3DD9,
           IID_IDirectXVideoProcessorService,
           (void**)&m_pDXVAVPS);
   }

   if (SUCCEEDED(hr))
   {
       hr = m_pDXVAVPS->CreateVideoProcessor(DXVA2_VideoProcProgressiveDevice,
           &m_VideoDesc,
           m_D3DPP.BackBufferFormat,
           1,
           &m_pDXVAVP_Right);
   }

   return SUCCEEDED(hr) ? MFX_ERR_NONE : MFX_ERR_DEVICE_FAILED;
}

#endif // #if defined(WIN32) || defined(WIN64)
