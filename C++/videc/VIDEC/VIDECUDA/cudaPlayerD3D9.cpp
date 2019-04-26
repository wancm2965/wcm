#include "cudaPlayerD3D9.h"
#include <dwmapi.h>

extern std::string&GetPTXSource(void);

// General check for CUDA GPU SM Capabilities for a specific device #
bool cutilDrvCudaDevCapabilities(int major_version, int minor_version, int deviceNum)
{
	int major, minor, dev;

	cuDeviceGet(&dev, deviceNum);
	cuDeviceComputeCapability(&major, &minor, dev);

	if((major > major_version) ||
		(major == major_version && minor >= minor_version))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool cudaPlayerD3D9::CreateCudaResources(void)
{
	if (CreateD3D9Device()==false)
	{
		return false;
	}

    HRESULT hr = S_OK;
    

    cuD3D9CtxCreate(&m_oContext, &m_oDevice, CU_CTX_BLOCKING_SYNC, m_pD3DDevice);

	m_pCudaModule = new CUmoduleManager(GetPTXSource(), 2, 2, 2);
	if (m_pCudaModule->GetLoalModuleOK()==false)
	{
		return false;
	}

    m_pCudaModule->GetCudaFunction("NV12ToARGB_drvapi",   &gfpNV12toARGB);

    if (CreateCudaDecoder()==false)
	{
		return false;
	}

	CreateD3D9Surface ( m_pVideoDecoder->targetWidth(), 
						m_pVideoDecoder->targetHeight() );

	CUcontext cuCurrent = NULL;
    CUresult result = cuCtxPopCurrent(&cuCurrent);
    if (result != CUDA_SUCCESS)
	{
        return false;
	}

    return ((m_pCudaModule && m_pVideoDecoder && m_pImageDX) ? true : false);
}

bool cudaPlayerD3D9::CreateD3D9(void) 
{
	// Create the D3D object.
	if( NULL == ( m_pD3D = Direct3DCreate9(D3D_SDK_VERSION) ) )
	{
		return false;
	}

    
	HMONITOR hMonitor=MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

	int nAdapter=D3DADAPTER_DEFAULT;

	int nMaxAdapter = m_pD3D->GetAdapterCount();
	int nAdapterIndex;

	// iterate over adapters until we error or find a match
	for (nAdapterIndex = 0; nAdapterIndex < nMaxAdapter; nAdapterIndex++)
	{
		HMONITOR hCurMonitor;

		// get the monitor for this adapter
		hCurMonitor = m_pD3D->GetAdapterMonitor(nAdapterIndex);

		// if we match the proposed monitor, this is it
		if (hCurMonitor == hMonitor)
		{
			nAdapter=nAdapterIndex;
			break;
		}
	}

	// Get primary display identifier
	D3DADAPTER_IDENTIFIER9 adapterId;
	HRESULT hr = m_pD3D->GetAdapterIdentifier(nAdapter, 0, &adapterId);
	if (hr!=S_OK)
	{
		return false;
	}

	CUdevice nDevID=0;
	CUresult cuStatus = cuD3D9GetDevice(&nDevID, adapterId.DeviceName); 
	if (cudaSuccess != cuStatus)
	{
		return false;
	}

	m_hMonitor=hMonitor;
	m_nAdapter=nAdapter;

    return true;
}

void cudaPlayerD3D9::DestroyD3D9(void)
{
	if (m_pD3D)
	{
		m_pD3D->Release();
		m_pD3D=NULL;
	}
}

bool cudaPlayerD3D9::CreateD3D9Device(void)
{
	if (m_pD3D==NULL)
	{
		return false;
	}

	m_pD3D->GetAdapterDisplayMode(m_nAdapter, &m_d3ddm);
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));

	m_d3dpp.Windowed               =  true; 

	m_d3dpp.BackBufferCount        = 1;
	m_d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow          = m_hWnd;
	m_d3dpp.BackBufferWidth	       = m_d3ddm.Width;
	m_d3dpp.BackBufferHeight       = m_d3ddm.Height;
	m_d3dpp.BackBufferFormat       = m_d3ddm.Format;
	m_d3dpp.FullScreen_RefreshRateInHz = 0; // set to 60 for fullscreen, and also don't forget to set Windowed to FALSE
	m_d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;// : D3DPRESENT_INTERVAL_IMMEDIATE);

	HRESULT eResult = m_pD3D->CreateDevice(m_nAdapter, D3DDEVTYPE_HAL, m_hWnd,
											D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
											&m_d3dpp, &m_pD3DDevice);
	return (eResult == S_OK);
}

void cudaPlayerD3D9::DestroyD3D9Device(void)
{
	if (m_pD3DDevice)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
}

// Initialize Direct3D Textures (allocation and initialization)
HRESULT
cudaPlayerD3D9::CreateD3D9Surface(unsigned int nWidth, unsigned int nHeight)
{
    m_pImageDX = new ImageDX(m_pD3DDevice, 
		                     nWidth, nHeight, 
		                     nWidth, nHeight, 
		                     1, 
							 ImageDX::BGRA_PIXEL_FORMAT); // ImageDX::LUMINANCE_PIXEL_FORMAT
	m_pImageDX->clear(0x80);

	m_pImageDX->setCUDAcontext(m_oContext);
	m_pImageDX->setCUDAdevice(m_oDevice);

    return S_OK;
}

HRESULT
cudaPlayerD3D9::DestroyD3D9Surface()
{
	if (m_pImageDX) 
	{
		delete m_pImageDX;
		m_pImageDX = NULL;
	}
	return S_OK;
}


bool cudaPlayerD3D9::CreateCudaDecoder( )
{
	// bind the context lock to the CUDA context
    CUresult result = cuvidCtxLockCreate(&m_CtxLock, m_oContext);
    if (result != CUDA_SUCCESS) 
	{
		return false;
    }

	m_pVideoDecoder = new VideoDecoder( m_nActualWidth,m_nActualHeight,m_d3ddm.Width,m_d3ddm.Height, m_oContext, cudaVideoCreate_PreferCUVID, m_CtxLock );

	return true;   
}

bool cudaPlayerD3D9::CreateCudaParser(void)
{
	CUVIDPARSERPARAMS oVideoParserParameters;  
	memset(&oVideoParserParameters, 0, sizeof(CUVIDPARSERPARAMS));
	oVideoParserParameters.CodecType              = cudaVideoCodec_H264;
	oVideoParserParameters.ulMaxNumDecodeSurfaces = 2;
	oVideoParserParameters.ulMaxDisplayDelay      = 4;	// this flag is needed so the parser will push frames out to the decoder as quickly as it can
	oVideoParserParameters.pUserData              = this;
	oVideoParserParameters.pfnSequenceCallback    = cudaPlayerD3D9::HandleVideoSequence;    // Called before decoding frames and/or whenever there is a format change
	oVideoParserParameters.pfnDecodePicture       = cudaPlayerD3D9::HandlePictureDecode;    // Called when a picture is ready to be decoded (decode order)
	oVideoParserParameters.pfnDisplayPicture      = cudaPlayerD3D9::HandlePictureDisplay;   // Called whenever a picture is ready to be displayed (display order)
	CUresult oResult = cuvidCreateVideoParser(&m_hVideoParser, &oVideoParserParameters);

	return (CUDA_SUCCESS == oResult);
}

void cudaPlayerD3D9::DestroyCudaParser(void)
{
	if (m_hVideoParser)
	{
		cuvidDestroyVideoParser(m_hVideoParser);
		m_hVideoParser=NULL;
	}
}

void 
cudaPlayerD3D9::DestroyCudaResources(void)
{
	DestroyD3D9Surface();
	DestroyCudaDecoder();

	if (m_pCudaModule)
	{
		delete m_pCudaModule;
		m_pCudaModule=NULL;
	}

    if (m_oContext) 
	{
		cuCtxPushCurrent(m_oContext);
		cuCtxPopCurrent(NULL);

        cuCtxDestroy(m_oContext);
        m_oContext = NULL;
    }

	DestroyD3D9Device();
}

void cudaPlayerD3D9::DestroyCudaDecoder(void)
{
	if (m_pVideoDecoder)
	{
		delete m_pVideoDecoder;
		m_pVideoDecoder=NULL;
	}

	if (m_CtxLock) 
	{
		cuvidCtxLockDestroy(m_CtxLock);
		m_CtxLock=NULL;
	}
}

		// Run the Cuda part of the computation


bool cudaPlayerD3D9::copyDecodedFrameToTexture(  CUVIDPARSERDISPINFO& oDisplayInfo, int *pbIsProgressive )
{
	if (m_pVideoDecoder==NULL || m_pImageDX==NULL)
	{
		return false;
	}

    {
		CCtxAutoLock lck  ( m_CtxLock );
		// Push the current CUDA context (only if we are using CUDA decoding path)
		CUresult result = cuCtxPushCurrent(m_oContext);

		CUdeviceptr	 pDecodedFrame[2] = { 0, 0 };
	    CUdeviceptr  pInteropFrame[2] = { 0, 0 };

		int num_fields = (oDisplayInfo.progressive_frame ? (1) : (2+oDisplayInfo.repeat_first_field));
		*pbIsProgressive = oDisplayInfo.progressive_frame;
        for (int active_field=0; active_field<num_fields; active_field++)
        {
            CUVIDPROCPARAMS oVideoProcessingParameters;
            memset(&oVideoProcessingParameters, 0, sizeof(CUVIDPROCPARAMS));
            
            oVideoProcessingParameters.progressive_frame = oDisplayInfo.progressive_frame;
            oVideoProcessingParameters.second_field      = active_field;
            oVideoProcessingParameters.top_field_first   = oDisplayInfo.top_field_first;
            oVideoProcessingParameters.unpaired_field    = (num_fields == 1);

			unsigned int nDecodedPitch = 0;

				// map decoded video frame to CUDA surfae
			m_pVideoDecoder->mapFrame(oDisplayInfo.picture_index, &pDecodedFrame[active_field], &nDecodedPitch, &oVideoProcessingParameters);

			// map DirectX texture to CUDA surface
			size_t nTexturePitch = 0;


			if (m_pImageDX) {
				// map the texture surface
				m_pImageDX->map(&pInteropFrame[active_field], &nTexturePitch, active_field);
			}
  
			// perform post processing on the CUDA surface (performs colors space conversion and post processing)
			// comment this out if we inclue the line of code seen above 
			cudaPostProcessFrame(&pDecodedFrame[active_field], nDecodedPitch, &pInteropFrame[active_field], nTexturePitch, m_pCudaModule->getModule(), gfpNV12toARGB, NULL);
			if (m_pImageDX) {
				// unmap the texture surface
				m_pImageDX->unmap(active_field);
			}

			// unmap video frame
            // unmapFrame() synchronizes with the VideoDecode API (ensures the frame has finished decoding)
			m_pVideoDecoder->unmapFrame(pDecodedFrame[active_field]);
		}

		// Detach from the Current thread
		cuCtxPopCurrent(NULL);
	} 


	return true;
}

// This is the CUDA stage for Video Post Processing.  Last stage takes care of the NV12 to ARGB
void
cudaPlayerD3D9::cudaPostProcessFrame(CUdeviceptr * ppDecodedFrame, size_t nDecodedPitch, 
                     CUdeviceptr * ppTextureData,  size_t nTexturePitch,
                     CUmodule cuModNV12toARGB, 
                     CUfunction fpCudaKernel, CUstream streamID)
{
    uint32 nWidth  = m_pVideoDecoder->targetWidth();
    uint32 nHeight = m_pVideoDecoder->targetHeight();

    // Upload the Color Space Conversion Matrices
	if (m_bUpdateCSC) {
        // CCIR 601/709
        float hueColorSpaceMat[9];
		setColorSpaceMatrix (m_eColorSpace,    hueColorSpaceMat, m_nHue);
		updateConstantMemory_drvapi( cuModNV12toARGB, hueColorSpaceMat );
		if (!m_bUpdateAll) 
			m_bUpdateCSC = false;
	}

	// TODO: Stage for handling video post processing

    // Final Stage: NV12toARGB color space conversion
	CUresult eResult;
    eResult = cudaLaunchNV12toARGBDrv  (*ppDecodedFrame, nDecodedPitch,
                                        *ppTextureData,  nTexturePitch,
                                        nWidth, nHeight, fpCudaKernel, streamID);
}

// Draw the final result on the screen
HRESULT cudaPlayerD3D9::drawScene(int field_num)
{
    HRESULT hr = S_OK;

    // Begin code to handle case where the D3D gets lost
    if (m_bDeviceLost)
    {
        // test the cooperative level to see if it's okay
        // to render
        if (FAILED(hr = m_pD3DDevice->TestCooperativeLevel()))
        {
            //fprintf(stderr, "TestCooperativeLevel = %08x failed, will attempt to reset\n", hr);

            // if the device was truly lost, (i.e., a fullscreen device just lost focus), wait
            // until we get it back

            if (hr == D3DERR_DEVICELOST)
			{
                //fprintf(stderr, "TestCooperativeLevel = %08x DeviceLost, will retry next call\n", hr);
                return S_OK;
            }


        }
    }

    // Normal D3D9 rendering code
    if (!m_bDeviceLost) {

		m_pD3DDevice->BeginScene();
		m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);	
		// render image
		m_pImageDX->render(field_num);
		// end the scene
		m_pD3DDevice->EndScene();
        
        hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);

        if (hr == D3DERR_DEVICELOST) 
		{
            m_bDeviceLost = true;
        }
    } 

    return S_OK;
}


void cudaPlayerD3D9::setD3D9View( )
{
	D3DVIEWPORT9 oViewport;
	oViewport.X = 0;
	oViewport.Y = 0;
	oViewport.Width  = m_d3ddm.Width;//m_nActualWidth;
	oViewport.Height = m_d3ddm.Height;//m_nActualHeight;
	oViewport.MaxZ = 1.0f;
	oViewport.MinZ = 0.0f;
            
    m_pD3DDevice->SetViewport(&oViewport);
            
    D3DMATRIX oViewMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 
                             0.0f, 1.0f, 0.0f, 0.0f, 
                             0.0f, 0.0f, 1.0f, 0.0f, 
                             0.0f, 0.0f, 0.0f, 1.0f};
                    // scale viewport to be of window width and height
    oViewMatrix._11 = 2.0f/oViewport.Width; 
    oViewMatrix._22 = 2.0f/oViewport.Height;
                    // translate viewport so that lower left corner represents
                    // (0, 0) and upper right corner is (width, height)
    oViewMatrix._41 = -1.0f - 1.0f / oViewport.Width; 
    oViewMatrix._42 = -1.0f + 1.0f / oViewport.Height;
    if (m_pD3DDevice)
	{
		m_pD3DDevice->SetTransform(D3DTS_VIEW, &oViewMatrix);
	}
}

cudaPlayerD3D9::cudaPlayerD3D9(void)
{
	m_bDeviceLost = false;

	m_bUpdateCSC  = true;
	m_bUpdateAll  = false;

	m_pDisplayInfo = NULL;


	m_CtxLock = NULL;

	cuModNV12toARGB  = 0;
	gfpNV12toARGB    = 0;

	m_oContext = 0;
	m_oDevice  = 0;

	m_eColorSpace = ITU601;
	m_nHue        = 0.0f;

	m_pVideoDecoder = 0;
	m_hVideoParser = NULL;

	m_pImageDX      = 0;

	m_hWnd = NULL;

	m_nActualWidth=0;
	m_nActualHeight=0;
	m_nFlagReset=0;

	m_nAdapter=0;
	m_bDwmEnabled = false;

	m_nFrameRate=30;

	m_hMonitor=NULL;
	m_nFlagSkipDisplay=0;
	m_bMonitorChanged=false;

	m_nStartTimestamp=0;

	m_pCudaModule=NULL;
	m_pD3DDevice=NULL;
	m_pD3D=NULL;
}

cudaPlayerD3D9::~cudaPlayerD3D9( )
{
}

int cudaPlayerD3D9::Open( HWND hWnd, int nWidth, int nHeight )
{
	CUresult cuStatus = CUDA_SUCCESS;

	m_nActualWidth=nWidth;
	m_nActualHeight=nHeight;

	m_hWnd = hWnd;



	if (CreateD3D9() == false) 
	{
		return S_FALSE;	
	}

	if (CreateCudaParser()==false)
	{
		return E_FAIL;
	}

    if (CreateCudaResources(  ) == false) 
	{
		return S_FALSE;
	}


	Resize();

	m_nStartTimestamp=timeGetTime();


	m_nLastTS=GetTickCount();
	m_nFrameCount=0;
	m_nSkipFrameCount=0;

	return S_OK;

}

void cudaPlayerD3D9::Close()
{
	if (m_hVideoParser)
	{
		CUVIDSOURCEDATAPACKET pCuvidPacket;
		ZeroMemory(&pCuvidPacket, sizeof(pCuvidPacket));

		pCuvidPacket.payload      = NULL;
		pCuvidPacket.payload_size = 0;

		pCuvidPacket.flags      = CUVID_PKT_TIMESTAMP;
		pCuvidPacket.timestamp  = timeGetTime();

		cuvidParseVideoData(m_hVideoParser, &pCuvidPacket);
	}



	DestroyCudaResources();
	DestroyCudaParser();
	DestroyD3D9();
}



int cudaPlayerD3D9::PlayFrame( const unsigned char *pFrameData, int nFrameLen , int nFrameRate, int&nFlagReset) 
{
	int nKeyFrame=nFlagReset;
	nFlagReset=0;

	if (m_nFlagReset)
	{
		m_nSkipFrameCount++;
		nFlagReset=1;
		return -1;
	}

	// Begin code to handle case where the D3D gets lost
	if (m_bDeviceLost)
	{
		if (m_pD3DDevice!=NULL)
		{
			HRESULT hr;
			if (FAILED(hr = m_pD3DDevice->TestCooperativeLevel()))
			{
				if (hr == D3DERR_DEVICELOST)
				{
					m_nSkipFrameCount++;

					return S_OK;
				}
			}
		}

		if (nKeyFrame)
		{
			m_nSkipFrameCount++;

			nFlagReset=1;
			return -1;
		}
	}

	if (m_bMonitorChanged)
	{
		nFlagReset=1;
	}

	if (m_nFrameRate!=nFrameRate)
	{
		m_nFrameRate=nFrameRate;
		m_bDwmEnabled=false;
	}

#if (0)
	{
		m_nFrameCount++;
		if (GetTickCount()-m_nLastTS > 20000)
		{
			char szDebug[1024];
			sprintf(szDebug,"CUDAPlayerD3D9::PlayFrame nFrameCount=%u nSkipFrameCount=%u this = %u \n",m_nFrameCount,m_nSkipFrameCount,this);
			OutputDebugString(szDebug);

			m_nFrameCount=0;
			m_nSkipFrameCount = 0;
			m_nLastTS=GetTickCount();
		}
	}
#endif

	if (m_hVideoParser)
	{
		CUVIDSOURCEDATAPACKET pCuvidPacket;
		ZeroMemory(&pCuvidPacket, sizeof(pCuvidPacket));

		pCuvidPacket.payload      = pFrameData;
		pCuvidPacket.payload_size = nFrameLen;

		pCuvidPacket.flags      = CUVID_PKT_TIMESTAMP;
		pCuvidPacket.timestamp  = timeGetTime();


		CUresult oResult = cuvidParseVideoData(m_hVideoParser, &pCuvidPacket);
		if( oResult == CUDA_SUCCESS )
		{
			return 0;
		}
	}

	return -1;
}

int cudaPlayerD3D9::GetActualWidth(void)
{
	return m_nActualWidth;
}

int cudaPlayerD3D9::GetActualHeight(void)
{
	return m_nActualHeight;
}

int cudaPlayerD3D9::Resize(void)
{
	return 0;
}

int cudaPlayerD3D9::SkipDisplay(int nFlagSkipDisplay)
{
	m_nFlagSkipDisplay=nFlagSkipDisplay;
	if (m_nFlagSkipDisplay==0)
	{
		m_bDwmEnabled=false;
	}
	return 0;
}

int  cudaPlayerD3D9::RenderVideoFrame(CUVIDPARSERDISPINFO* pDisplayInfo)
{
	if (!m_pDisplayInfo)
	{
		setD3D9View();
	}
	m_pDisplayInfo=pDisplayInfo;

	int nLoolCount=0;

	if (m_nFlagSkipDisplay==-1)
	{
		m_nFlagSkipDisplay=0;
		nLoolCount=30;
		m_bDwmEnabled=false;
	}

	if (m_nFlagSkipDisplay)
	{
		m_nSkipFrameCount++;

		if (m_nFlagSkipDisplay==0x10000000)
		{
			m_nFlagSkipDisplay=0;
		}
		m_bDwmEnabled=false;
		return S_OK;
	}

	if (!IsWindowVisible(m_hWnd))
	{
		m_nSkipFrameCount++;

		m_bDwmEnabled=false;
		return S_OK;
	}

	RECT rect;
	GetClientRect(m_hWnd, &rect);
	if (IsRectEmpty(&rect))
	{
		m_nSkipFrameCount++;

		m_bDwmEnabled=false;
		return S_OK;
	}

	if (m_nStartTimestamp)
	{
		unsigned long nTimestamp=timeGetTime();
		if (nTimestamp-m_nStartTimestamp>=500)
		{
			m_nStartTimestamp=0;
			m_bDwmEnabled=false;
		}
	}

	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	if (hMonitor!=m_hMonitor)
	{
		m_bMonitorChanged=true;
	}

	EnableDwmQueuing();


	if ( !m_bDeviceLost ) 
	{
		int bIsProgressive = 1;
		if (copyDecodedFrameToTexture( *pDisplayInfo, &bIsProgressive ))
		{
			while (1)
			{
				unsigned long nStartTimestamp=timeGetTime();
				drawScene(0);
				if (nLoolCount--<1)
				{
					break;
				}
				unsigned long nEndTimestamp=timeGetTime();
				unsigned long nDelta=nEndTimestamp-nStartTimestamp;
				if (nDelta>20)
				{
					break;
				}

				//if (nLoolCount>0)
				//{
				//	char szDebug[1024];
				//	sprintf(szDebug,"nDelta=%u\n",nDelta);
				//	OutputDebugString(szDebug);
				//}
			}
		}
	}

	return S_OK;
}



int CUDAAPI cudaPlayerD3D9::HandleVideoSequence(void *pUserData, CUVIDEOFORMAT * pFormat)
{
    cudaPlayerD3D9 * pPlayerD3D9 = reinterpret_cast<cudaPlayerD3D9 *>(pUserData);
    
	if (pPlayerD3D9->m_nActualWidth!=pFormat->coded_width ||
		pPlayerD3D9->m_nActualHeight!=pFormat->coded_height)
	{
		pPlayerD3D9->m_nFlagReset=1;

		pPlayerD3D9->m_nActualWidth=pFormat->coded_width;
		pPlayerD3D9->m_nActualHeight=pFormat->coded_height;
	}
    
    return 1;
}

int CUDAAPI cudaPlayerD3D9::HandlePictureDecode(void * pUserData, CUVIDPICPARAMS * pPicParams)
{
    cudaPlayerD3D9 * pParserData = reinterpret_cast<cudaPlayerD3D9 *>(pUserData);
    
    pParserData->m_pVideoDecoder->decodePicture(pPicParams);
    
    return true;
}

int CUDAAPI cudaPlayerD3D9::HandlePictureDisplay(void *pUserData, CUVIDPARSERDISPINFO * pDisplayInfo)
{
    cudaPlayerD3D9 * pParserData = reinterpret_cast<cudaPlayerD3D9 *>(pUserData);

	pParserData->RenderVideoFrame(pDisplayInfo); 

    return 1;
}


bool cudaPlayerD3D9::EnableDwmQueuing()
{
	HRESULT hr;

	// DWM queuing is enabled already.
	if (m_bDwmEnabled)
	{
		return true;
	}

	// Check to see if DWM is currently enabled.
	BOOL bDWM = FALSE;

	hr = DwmIsCompositionEnabled(&bDWM);

	if (FAILED(hr))
	{
		return false;
	}

	// DWM queuing is disabled when DWM is disabled.
	if (!bDWM)
	{
		m_bDwmEnabled = false;
		return false;
	}

	// Retrieve DWM refresh count of the last vsync.
	DWM_TIMING_INFO dwmti = {0};

	dwmti.cbSize = sizeof(dwmti);

	hr = DwmGetCompositionTimingInfo(NULL, &dwmti);

	if (FAILED(hr))
	{
		return false;
	}

	// Enable DWM queuing from the next refresh.
	DWM_PRESENT_PARAMETERS dwmpp = {0};

	dwmpp.cbSize                    = sizeof(dwmpp);
	dwmpp.fQueue                    = TRUE;
	dwmpp.cRefreshStart             = dwmti.cRefresh + 1;
	dwmpp.cBuffer                   = 2; //maximum depth of DWM queue
	dwmpp.fUseSourceRate            = TRUE;
	dwmpp.cRefreshesPerFrame        = 1;
	dwmpp.eSampling                 = DWM_SOURCE_FRAME_SAMPLING_LAST;//DWM_SOURCE_FRAME_SAMPLING_POINT;
	dwmpp.rateSource.uiDenominator  = 1;
	dwmpp.rateSource.uiNumerator    = m_nFrameRate; 


	hr = DwmSetPresentParameters(m_hWnd, &dwmpp);

	if (FAILED(hr))
	{
		return false;
	}

	// DWM queuing is enabled.
	m_bDwmEnabled = true;

	return true;
}
