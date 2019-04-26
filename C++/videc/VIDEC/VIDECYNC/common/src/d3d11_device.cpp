/*********************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or nondisclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2014 Intel Corporation. All Rights Reserved.

**********************************************************************************/

#include "mfx_samples_config.h"

#if defined(_WIN32) || defined(_WIN64)

#include "sample_defs.h"


#if MFX_D3D11_RENDER_SUPPORT

#include <dxgi1_2.h>
#include "d3d11_device.h"

CD3D11Device::CD3D11Device(CD3D9DeviceCallback&rCallback,bool bHardWareDecoder):
    m_nViews(0),
    m_bDefaultStereoEnabled(FALSE),
    m_nSyncInterval(0),
    m_bIsA2rgb10(FALSE),
	m_rCallback(rCallback),
	m_nRefreshRate(60),
	m_bHardWareDecoder(bHardWareDecoder)
{
	m_nWidth=0;
	m_nHeight=0;
}

CD3D11Device::~CD3D11Device()
{	
    Close();
}

mfxStatus CD3D11Device::FillSCD(mfxHDL hWindow, DXGI_SWAP_CHAIN_DESC& scd)
{
    scd.Windowed = TRUE;
    scd.OutputWindow = (HWND)hWindow;
    scd.SampleDesc.Count = 1;
    scd.BufferDesc.Format = (m_bIsA2rgb10) ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;

    return MFX_ERR_NONE;
}

mfxStatus CD3D11Device::FillSCD1(DXGI_SWAP_CHAIN_DESC1& scd1)
{
    scd1.Width = 0;                                     // Use automatic sizing.
    scd1.Height = 0;
    scd1.Format = (m_bIsA2rgb10) ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_B8G8R8A8_UNORM;
    scd1.Stereo = m_nViews == 2 ? TRUE : FALSE;
    scd1.SampleDesc.Count = 1;                          // Don't use multi-sampling.
    scd1.SampleDesc.Quality = 0;
    scd1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd1.BufferCount = 2;                               // Use double buffering to minimize latency.
    scd1.Scaling = DXGI_SCALING_STRETCH;
    scd1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd1.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    return MFX_ERR_NONE;
}


mfxStatus CD3D11Device::Init(mfxHDL hWindow)
{
	mfxU16 nViews=1;
   mfxStatus sts = MFX_ERR_NONE;
    HRESULT hres = S_OK;
    m_nViews = nViews;
    if (2 < nViews)
        return MFX_ERR_UNSUPPORTED;
    m_bDefaultStereoEnabled = FALSE;
    m_nSyncInterval = 0;

	m_rect.right	= 0;
	m_rect.bottom	= 0;

	m_hWindow = (HWND)hWindow;

    static D3D_FEATURE_LEVEL FeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
  		D3D_FEATURE_LEVEL_11_0,
//  		D3D_FEATURE_LEVEL_10_1,
//  		D3D_FEATURE_LEVEL_10_0,
//  		D3D_FEATURE_LEVEL_9_3
    };
    D3D_FEATURE_LEVEL pFeatureLevelsOut;

    hres = CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)(&m_pDXGIFactory) );
    if (FAILED(hres))
        return MFX_ERR_DEVICE_FAILED;   

// 	UINT i = 0; 
// 	IDXGIAdapter * pAdapter; 
// 	std::vector <IDXGIAdapter*> vAdapters; 
// 	while(factory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND) 
// 	{ 
// 		vAdapters.push_back(pAdapter); 
// 		++i; 
// 	} 
// 	IDXGIOutput* adapterOutput; 
// 	hres = adapter->EnumOutputs(0, &adapterOutput); 
// 	if(FAILED(hres)) 
// 	{ 
// 		return MFX_ERR_DEVICE_FAILED; 
// 	}
// 	adapter->Release(); 
// 	adapter = 0;
// 
// 	factory->Release(); 
// 	factory = 

	
	bool bCreate = false;
	if (m_bHardWareDecoder)
	{
		int nAdapter=GetIntelAdapterNum();
		hres = m_pDXGIFactory->EnumAdapters(nAdapter,&m_pAdapter);
		if (SUCCEEDED(hres))
		{
			bCreate = true;
			hres =  D3D11CreateDevice(m_pAdapter,
				D3D_DRIVER_TYPE_UNKNOWN,
				NULL,
				0,
				FeatureLevels,
				MSDK_ARRAY_LEN(FeatureLevels),
				D3D11_SDK_VERSION,
				&m_pD3D11Device,
				&pFeatureLevelsOut,
				&m_pD3D11Ctx);
		}
	}
	if (!bCreate)
	{
		hres =  D3D11CreateDevice(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			0,
			FeatureLevels,
			MSDK_ARRAY_LEN(FeatureLevels),
			D3D11_SDK_VERSION,
			&m_pD3D11Device,
			&pFeatureLevelsOut,
			&m_pD3D11Ctx);
	}	

	if (FAILED(hres))
	{
		return MFX_ERR_DEVICE_FAILED;
	}
    m_pDXGIDev = m_pD3D11Device;
    m_pDX11VideoDevice = m_pD3D11Device;
    m_pVideoContext = m_pD3D11Ctx;

    MSDK_CHECK_POINTER(m_pDXGIDev.p, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pDX11VideoDevice.p, MFX_ERR_NULL_PTR);
    MSDK_CHECK_POINTER(m_pVideoContext.p, MFX_ERR_NULL_PTR);

    // turn on multithreading for the Context
    CComQIPtr<ID3D10Multithread> p_mt(m_pVideoContext);

    if (p_mt)
        p_mt->SetMultithreadProtected(true);
    else
        return MFX_ERR_DEVICE_FAILED;

    // create swap chain only for rendering use case (hWindow != 0)
    if (hWindow)
    {
        MSDK_CHECK_POINTER(m_pDXGIFactory.p, MFX_ERR_NULL_PTR);
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};

        sts = FillSCD1(swapChainDesc);
        MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

        hres = m_pDXGIFactory->CreateSwapChainForHwnd(m_pD3D11Device,
            (HWND)hWindow,
            &swapChainDesc,
            NULL,
            NULL,
            reinterpret_cast<IDXGISwapChain1**>(&m_pSwapChain));
        if (FAILED(hres))
            return MFX_ERR_DEVICE_FAILED;
    }

    return sts;
}

mfxStatus CD3D11Device::CreateVideoProcessor(int nFrameRate,int nWidth,int nHeight)
{
    HRESULT hres = S_OK;

    int numerator = 60;
    if (nFrameRate <= numerator/4)
    {
        m_nSyncInterval = 4;
    }
    else if (nFrameRate <= numerator/3)
    {
        m_nSyncInterval = 3;
    }
    else if (nFrameRate <= numerator/2)
    {
        m_nSyncInterval = 2;
    }
    else if (nFrameRate <= numerator)
    {
        m_nSyncInterval = 1;
    }

	if (m_VideoProcessorEnum.p)
		return MFX_ERR_NONE;


    //create video processor
    D3D11_VIDEO_PROCESSOR_CONTENT_DESC ContentDesc;
    MSDK_ZERO_MEMORY( ContentDesc );

    ContentDesc.InputFrameFormat = D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE;
    ContentDesc.InputFrameRate.Numerator = 60000;
    ContentDesc.InputFrameRate.Denominator = 1000;
    ContentDesc.InputWidth  = nWidth;
    ContentDesc.InputHeight = nHeight;
    ContentDesc.OutputWidth = nWidth;
    ContentDesc.OutputHeight = nHeight;
    ContentDesc.OutputFrameRate.Numerator = 60000;
    ContentDesc.OutputFrameRate.Denominator = 1000;

    ContentDesc.Usage = D3D11_VIDEO_USAGE_PLAYBACK_NORMAL;

    hres = m_pDX11VideoDevice->CreateVideoProcessorEnumerator( &ContentDesc, &m_VideoProcessorEnum );
    if (FAILED(hres))
        return MFX_ERR_DEVICE_FAILED;

    hres = m_pDX11VideoDevice->CreateVideoProcessor( m_VideoProcessorEnum, 0, &m_pVideoProcessor );
    if (FAILED(hres))
        return MFX_ERR_DEVICE_FAILED;

    return MFX_ERR_NONE;
}

mfxStatus CD3D11Device::Reset(HMONITOR hMonitor)
{
    // Changing video mode back to the original state
    //MSDK_CHECK_POINTER (m_pDXGIFactory.p, MFX_ERR_NULL_PTR);
    //DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};

    //mfxStatus sts = FillSCD1(swapChainDesc);
    //MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);

    //HRESULT hres = S_OK;
    //hres = m_pDXGIFactory->CreateSwapChainForHwnd(m_pD3D11Device,
    //    (HWND)m_HandleWindow,
    //    &swapChainDesc,
    //    NULL,
    //    NULL,
    //    reinterpret_cast<IDXGISwapChain1**>(&m_pSwapChain));

    //if (FAILED(hres))
    //    return MFX_ERR_DEVICE_FAILED;

    return MFX_ERR_NONE;
}

mfxStatus CD3D11Device::CheckMonitor(void)
{
	mfxStatus hr = MFX_ERR_NONE;
	//HMONITOR hMonitor = MonitorFromWindow(m_hWindow, MONITOR_DEFAULTTONEAREST);
	//if (hMonitor!=m_hMonitor)
	//{
	//	m_hMonitor = hMonitor;
	//	hr=Reset(hMonitor);
	//	if (hr!=MFX_ERR_NONE)
	//	{
	//		DebugOutput(_T("RenderFrame Failed!\n"));
	//		return MFX_ERR_UNKNOWN;
	//	}
	//	return MFX_WRN_IN_EXECUTION;
	//}
	return hr;
}

mfxStatus CD3D11Device::GetHandle(mfxHandleType type, mfxHDL *pHdl)
{
    if (MFX_HANDLE_D3D11_DEVICE == type)
    {
        *pHdl = m_pD3D11Device.p;
        return MFX_ERR_NONE;
    }
    return MFX_ERR_UNSUPPORTED;
}

mfxStatus CD3D11Device::SetHandle(mfxHandleType /*type*/, mfxHDL /*hdl*/)
{
    return MFX_ERR_UNSUPPORTED;
}

mfxStatus CD3D11Device::RenderFrame(mfxFrameSurface1 * pSrf, mfxFrameAllocator * pAlloc,bool bSkip,unsigned char*pNV12Data,int nFrameRate,int nWidth,int nHeight,int nPitch)
{
    HRESULT hres = S_OK;
    mfxStatus sts;

	if (pNV12Data==NULL && pSrf)
	{
		nFrameRate = pSrf->Info.FrameRateExtN / pSrf->Info.FrameRateExtD;
		nWidth = pSrf->Info.CropW;
		nHeight = pSrf->Info.CropH;
	}
	

    sts = CreateVideoProcessor(nFrameRate,nWidth,nHeight);
    MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);


    if (/*m_bHardWareDecoder ||*/ m_pOutputView.p == NULL)
    {
		hres = m_pSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (void**)&m_pDXGIBackBuffer.p);
		if (FAILED(hres))
			return MFX_ERR_DEVICE_FAILED;

		D3D11_VIDEO_PROCESSOR_OUTPUT_VIEW_DESC OutputViewDesc;
		OutputViewDesc.ViewDimension = D3D11_VPOV_DIMENSION_TEXTURE2D;
		OutputViewDesc.Texture2D.MipSlice = 0;
        hres = m_pDX11VideoDevice->CreateVideoProcessorOutputView(
            m_pDXGIBackBuffer,
            m_VideoProcessorEnum,
            &OutputViewDesc,
            &m_pOutputView.p );
        if (FAILED(hres))
            return MFX_ERR_DEVICE_FAILED;
    }	

	CComPtr<ID3D11VideoProcessorInputView>  m_pInputViewLeft;

	 // Creating input views for left and righ eyes
    //if (pNV12Data != NULL || /*m_bHardWareDecoder || */m_pInputViewLeft.p == NULL || m_nWidth != nWidth || m_nHeight != nHeight)
    {
		m_nWidth = nWidth;
		m_nHeight = nHeight;

		CComPtr<ID3D11Texture2D>  m_pRTTexture2D;
		if (pNV12Data != NULL)
		{
			D3D11_SUBRESOURCE_DATA frameData;
			frameData.pSysMem = pNV12Data;
			frameData.SysMemPitch = nPitch;
			frameData.SysMemSlicePitch = 0;
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc,sizeof(D3D11_TEXTURE2D_DESC));
			texDesc.Width = nWidth;
			texDesc.Height = nHeight;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_NV12;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;//D3D11_BIND_RENDER_TARGET;
			texDesc.MiscFlags = 0;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.CPUAccessFlags = 0;//D3D11_CPU_ACCESS_WRITE;
			hres = m_pD3D11Device->CreateTexture2D(&texDesc, &frameData, &m_pRTTexture2D);
			if (FAILED(hres))
				return MFX_ERR_DEVICE_FAILED;
		}
		else
		{
 			mfxHDLPair pair = {NULL};
 			sts = pAlloc->GetHDL(pAlloc->pthis, pSrf->Data.MemId, (mfxHDL*)&pair);
 			MSDK_CHECK_RESULT(sts, MFX_ERR_NONE, sts);
 			m_pRTTexture2D = reinterpret_cast<ID3D11Texture2D*>(pair.first);
		}	

		D3D11_VIDEO_PROCESSOR_INPUT_VIEW_DESC InputViewDesc;
		InputViewDesc.FourCC = 0;
		InputViewDesc.ViewDimension = D3D11_VPIV_DIMENSION_TEXTURE2D;
		InputViewDesc.Texture2D.MipSlice = 0;
		InputViewDesc.Texture2D.ArraySlice = 0;
        hres = m_pDX11VideoDevice->CreateVideoProcessorInputView(
            m_pRTTexture2D,
            m_VideoProcessorEnum,
            &InputViewDesc,
            &m_pInputViewLeft.p );

		if (FAILED(hres))
			return MFX_ERR_DEVICE_FAILED;
    }   


    //  NV12 surface to RGB backbuffer
	{
		RECT rect = {0};
		rect.right  = nWidth;
		rect.bottom = nHeight;
		m_pVideoContext->VideoProcessorSetStreamSourceRect(m_pVideoProcessor, 0, true, &rect);
		m_pVideoContext->VideoProcessorSetStreamFrameFormat( m_pVideoProcessor, 0, D3D11_VIDEO_FRAME_FORMAT_PROGRESSIVE);
	}

	D3D11_VIDEO_PROCESSOR_STREAM			m_StreamData;
	m_StreamData.Enable = TRUE;
	m_StreamData.OutputIndex = 0;
	m_StreamData.InputFrameOrField = 0;
	m_StreamData.PastFrames = 0;
	m_StreamData.FutureFrames = 0;
	m_StreamData.ppPastSurfaces = NULL;
	m_StreamData.ppFutureSurfaces = NULL;
	m_StreamData.pInputSurface = m_pInputViewLeft;
	m_StreamData.ppPastSurfacesRight = NULL;
	m_StreamData.ppFutureSurfacesRight = NULL;
	m_StreamData.pInputSurfaceRight = m_nViews == 2 ? m_pInputViewRight : NULL;	
	
    hres = m_pVideoContext->VideoProcessorBlt( m_pVideoProcessor, m_pOutputView, 0, 1, &m_StreamData );
    if (FAILED(hres))
        return MFX_ERR_DEVICE_FAILED;

	if (bSkip)
	{
		m_nSyncInterval = 0;
	}

	DXGI_PRESENT_PARAMETERS parameters = {0};
	hres = m_pSwapChain->Present1(m_nSyncInterval, 0, &parameters);
	if (FAILED(hres))
		return MFX_ERR_DEVICE_FAILED;

    return MFX_ERR_NONE;
}

mfxStatus CD3D11Device::Present(void)
{
/*	if (m_pSwapChain)
	{
		HRESULT hres = S_OK;
		DXGI_PRESENT_PARAMETERS parameters = {0};
		hres = m_pSwapChain->Present1(m_nSyncInterval, 0, &parameters);
		if (FAILED(hres))
			return MFX_ERR_DEVICE_FAILED;
	}*/	
	return MFX_ERR_NONE;
}

void CD3D11Device::Close()
{
	if (2 == m_nViews && !m_bDefaultStereoEnabled)
		m_pDisplayControl->SetStereoEnabled(FALSE);
}

#if 0

D3DClass::D3DClass(void) 
{

	m_swapChain = 0; 
	m_device = 0; 
	m_deviceContext = 0; 
	m_renderTargetView = 0; 
	m_depthStencilBuffer = 0; 
	m_depthStencilState = 0; 
	m_depthStencilView = 0; 
	m_rasterState = 0;

}

D3DClass::D3DClass(const D3DClass& other) 
{ 
}



D3DClass::~D3DClass(void) 
{ 
}

//Initialize�����������D3D���õ����д��롣

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear) 
{ 
	HRESULT result; 
	IDXGIFactory* factory; 
	IDXGIAdapter* adapter; 
	IDXGIOutput* adapterOutput; 
	unsigned int numModes, i, numerator, denominator, stringLength; 
	DXGI_MODE_DESC* displayModeList; 
	DXGI_ADAPTER_DESC adapterDesc; 
	int error; 
	DXGI_SWAP_CHAIN_DESC swapChainDesc; 
	D3D_FEATURE_LEVEL featureLevel; 
	ID3D11Texture2D* backBufferPtr; 
	D3D11_TEXTURE2D_DESC depthBufferDesc; 
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc; 
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc; 
	D3D11_RASTERIZER_DESC rasterDesc; 
	D3D11_VIEWPORT viewport; 
	float fieldOfView, screenAspect;


	// ���洹ֱͬ������ 
	m_vsync_enabled = vsync;

	// ����һ��<a href="http://www.it165.net/Pro/pkgame/" target="_blank" class="keylink">DirectX</a> graphics interface factory. 
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// �ýӿڹ�������һ�����Կ������� 
	result = factory->EnumAdapters(0, &adapter); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// �õ��������������. 
	result = adapter->EnumOutputs(0, &adapterOutput); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	//�õ��ʺ� DXGI_FORMAT_R8G8B8A8_UNORM ����ʾģʽ. 
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	displayModeList = new DXGI_MODE_DESC[numModes]; 
	if(!displayModeList) 
	{ 
		return false; 
	}

	// ������ʾģʽ��displayModeList�� 
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	//����������ʾģʽ���õ�ˢ������������ֵnumerator �� denominato 
	for(i=0; i<numModes; i++) 
	{ 
		if(displayModeList[i].Width == (unsigned int)screenWidth) 
		{ 
			if(displayModeList[i].Height == (unsigned int)screenHeight) 
			{ 
				numerator = displayModeList[i].RefreshRate.Numerator; 
				denominator = displayModeList[i].RefreshRate.Denominator; 
			} 
		} 
	} 
	// �õ��Կ����� 
	result = adapter->GetDesc(&adapterDesc); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// �����Դ��С. 
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//�����Կ�������

	//wcstombs_s, wide charת��Ϊchar 
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128); 
	if(error != 0) 
	{ 
		return false; 
	} 
	// �ͷ���ʾģʽ�б� 
	delete [] displayModeList; 
	displayModeList = 0;

	//�ͷ����������. 
	adapterOutput->Release(); 
	adapterOutput = 0;

	//�ͷ������� 
	adapter->Release(); 
	adapter = 0;

	// �ͷŽӿڹ���. 
	factory->Release(); 
	factory = 0;

	// ��ʼ������������ 
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ��1���󻺳� 
	swapChainDesc.BufferCount = 1;

	//֡����Ĵ�С��Ӧ�ó��򴰿ڴ�С���. 
	swapChainDesc.BufferDesc.Width = screenWidth; 
	swapChainDesc.BufferDesc.Height = screenHeight;

	// �󻺳��surface�ĸ�ʽΪDXGI_FORMAT_R8G8B8A8_UNORM. 
	// surface��ÿ��������4���޷��ŵ�8bit[ӳ�䵽0-1]����ʾ��NORM��ʾ��һ���� 
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ���ʹ�ô�ֱͬ�������ú󻺳��ˢ����.

	//ˢ���ʾ���һ���ӰѺ󻺳���������Ļ�ϻ����Ĵ����� 
	//���������ֱͬ����������ˢ���ʣ���fps�ǹ̶��� 

	//if(m_vsync_enabled) 
	{ 
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60000; 
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1000; 
	} 
// 	else 
// 	{ 
// 		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; 
// 		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; 
// 	}

	// ���ú󻺳����; 
	// ���ǵ���ȾĿ�껺��Ϊ�󻺳塣 
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �󻺳�����Ĵ��ھ��. 
	swapChainDesc.OutputWindow = hwnd;

	// ��ʹ�ö��ز��� 
	swapChainDesc.SampleDesc.Count = 1; 
	swapChainDesc.SampleDesc.Quality = 0;

	// ����ȫ�����ߴ���ģʽ. 
	if(fullscreen) 
	{ 
		swapChainDesc.Windowed = false; 
	} 
	else 
	{ 
		swapChainDesc.Windowed = true; 
	}

	// �趨ɨ����ordering�Լ�����Ϊunspecified. 
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// �󻺳����ݳ��ֵ���Ļ�󣬷��������� 
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�����ñ�־ 
	swapChainDesc.Flags = 0;

	// ����feature levelΪD3D11 
	// ����Կ���֧��D3D11,�����ܹ�ͨ���������������ʹ��D3D10,����9. 
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// �������������豸�Լ��豸������. 
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// �õ��������еĺ󻺳�ָ��. 
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// �ú󻺳崴����ȾĿ����ͼ. 
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	//�ͷź󻺳�.(���ü�����1) 
	backBufferPtr->Release(); 
	backBufferPtr = 0;

	// ��ʼ����Ȼ�������. 

	D3D11_TEXTURE2D_DESC depthBufferDesc; 
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//������Ȼ������� 
	depthBufferDesc.Width = screenWidth; 
	depthBufferDesc.Height = screenHeight; 
	depthBufferDesc.MipLevels = 1; //������Ȼ���Ϊ1 
	depthBufferDesc.ArraySize = 1; //������Ȼ���Ϊ1������������2�������и�����; 
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	depthBufferDesc.SampleDesc.Count = 1; 
	depthBufferDesc.SampleDesc.Quality = 0; 
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT; 
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; 
	depthBufferDesc.CPUAccessFlags = 0; 
	depthBufferDesc.MiscFlags = 0;

	// ������Ȼ���. 
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer); 
	if(FAILED(result)) 
	{ 
		return false;

	}

	// ��ʼ�����ģ��״̬����. 
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// �������ģ��״̬����. 
	depthStencilDesc.DepthEnable = true; 
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //D3D11_DEPTH_WRITE_MASK_ZERO��ֹд��Ȼ��� 
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true; 
	depthStencilDesc.StencilReadMask = 0xFF; 
	depthStencilDesc.StencilWriteMask = 0xFF;

	// ����front face ����ʹ�õ�ģ���������. 
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; 
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR; 
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; 
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ����back face����ʹ�õ�ģ�����ģʽ. 
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; 
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR; 
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; 
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �������ģ��״̬ 
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState); 
	if(FAILED(result)) 
	{ 
		return false;

	}

	// �������ģ��״̬��ʹ����Ч 
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ��ʼ�����ģ����ͼ. 
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// �������ģ����ͼ����. 
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; 
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// �������ģ����ͼ. 
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	// ����ȾĿ����ͼ����Ȼ��嵽��Ⱦ����. 
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// ���ù�դ��������ָ���������α���Ⱦ. 
	rasterDesc.AntialiasedLineEnable = false; 
	rasterDesc.CullMode = D3D11_CULL_BACK; 
	rasterDesc.DepthBias = 0; 
	rasterDesc.DepthBiasClamp = 0.0f; 
	rasterDesc.DepthClipEnable = true; 
	rasterDesc.FillMode = D3D11_FILL_SOLID; 
	rasterDesc.FrontCounterClockwise = false; 
	rasterDesc.MultisampleEnable = false; 
	rasterDesc.ScissorEnable = false; 
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ������դ��״̬. 
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState); 
	if(FAILED(result)) 
	{ 
		return false; 
	}

	//���ù�դ��״̬��ʹ����Ч 
	m_deviceContext->RSSetState(m_rasterState);


	// �����ӿڣ���ʾȫ���󻺳����� 
	viewport.Width = (float)screenWidth; 
	viewport.Height = (float)screenHeight; 
	viewport.MinDepth = 0.0f; 
	viewport.MaxDepth = 1.0f; 
	viewport.TopLeftX = 0.0f; 
	viewport.TopLeftY = 0.0f;

	// �����ӿ� 
	m_deviceContext->RSSetViewports(1, &viewport);

	// ����͸��ͶӰ���� 
	fieldOfView = (float)D3DX_PI / 4.0f; 
	screenAspect = (float)screenWidth / (float)screenHeight;

	// ����͸��ͶӰ����. 
	//D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

	//��ʼ��world����Ϊ��λ����.

	//�þ���ʵ�־ֲ����굽���������ת�� 
	D3DXMatrixIdentity(&m_worldMatrix);


	// ��������ͶӰ������Ҫ����ʵʩ2D��Ⱦ. 
	//D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;


}

void D3DClass::Shutdown() 
{ 
	// �ͷŽ�������Դǰ��������Ϊ����ģʽ��������ܻ�����쳣. 
	if(m_swapChain) 
	{ 
		m_swapChain->SetFullscreenState(false, NULL); 
	}

	if(m_rasterState) 
	{ 
		m_rasterState->Release(); 
		m_rasterState = 0; 
	}

	if(m_depthStencilView) 
	{ 
		m_depthStencilView->Release(); 
		m_depthStencilView = 0; 
	}

	if(m_depthStencilState) 
	{ 
		m_depthStencilState->Release(); 
		m_depthStencilState = 0; 
	}

	if(m_depthStencilBuffer) 
	{ 
		m_depthStencilBuffer->Release(); 
		m_depthStencilBuffer = 0; 
	}

	if(m_renderTargetView) 
	{ 
		m_renderTargetView->Release(); 
		m_renderTargetView = 0; 
	}

	if(m_deviceContext) 
	{ 
		m_deviceContext->Release(); 
		m_deviceContext = 0; 
	}

	if(m_device) 
	{ 
		m_device->Release(); 
		m_device = 0; 
	}

	if(m_swapChain) 
	{ 
		m_swapChain->Release(); 
		m_swapChain = 0; 
	}

	return; 
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha) 
{ 
	float color[4];


	// ��������󻺳���ɫ. 
	color[0] = red; 
	color[1] = green; 
	color[2] = blue; 
	color[3] = alpha;

	//����󻺳�. 
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	//�����Ȼ���. 
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return; 
}


void D3DClass::EndScene() 
{ 
	//��Ⱦ��ɺ󣬰Ѻ󻺳�����present��ǰ���� 
	if(m_vsync_enabled) 
	{ 
		// ������Ļˢ����. 
		m_swapChain->Present(1, 0); 
	} 
	else 
	{ 
		// �����ܿ��present�󻺳�. 
		m_swapChain->Present(0, 0); 
	}

	return; 
}

ID3D11Device* D3DClass::GetDevice() 
{ 
	return m_device; 
}


ID3D11DeviceContext* D3DClass::GetDeviceContext() 
{ 
	return m_deviceContext; 
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix) 
{ 
	projectionMatrix = m_projectionMatrix; 
	return; 
}


void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix) 
{ 
	worldMatrix = m_worldMatrix; 
	return; 
}


void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix) 
{ 
	orthoMatrix = m_orthoMatrix; 
	return; 
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory) 
{ 
	strcpy_s(cardName, 128, m_videoCardDescription); 
	memory = m_videoCardMemory; 
	return; 
} 


#endif

#endif // #if MFX_D3D11_RENDER_SUPPORT
#endif // #if defined(_WIN32) || defined(_WIN64)
