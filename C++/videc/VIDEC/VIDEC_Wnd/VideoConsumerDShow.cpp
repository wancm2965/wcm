// VideoConsumerDShow.cpp: implementation of the VideoConsumerDShow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <initguid.h>
#include <dmodshow.h>
#include <D3d9.h>
#include <Vmr9.h>
#include <evr.h>
#include <evr9.h>
#include "VideoConsumerDShow.h"
#include "VideoConsumerOld.h"
#include <VIDEC/VIDEC_AH2KDev.h>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SAFE_RELEASE(x) {if(x) x -> Release(); x = NULL;}


DEFINE_GUID(CLSID_EnhancedVideoRenderer,
			0xfa10746c, 0x9b63, 0x4b6c, 0xbc, 0x49, 0xfc, 0x30, 0xe, 0xa5, 0xf2, 0x56);

static bool g_bEVRFailed=false;
static bool g_bVMRFailed=false;

extern BOOL IsVistaOrAbove();


HRESULT MyInitializeEVR(IBaseFilter *pEVR, HWND hwnd, IMFVideoDisplayControl ** ppWc);

HRESULT MyInitWindowlessVMR(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl** ppWc); 

HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin);
IPin * GetInPin ( IBaseFilter *pFilter, int Num );
IPin * GetOutPin( IBaseFilter *pFilter, int Num );


VideoConsumer*CreateVideoConsumerDShow(VideoConsumerCallback&rCallback)
{
	//return new VideoConsumerOld(rCallback);
	return new VideoConsumerDShow(rCallback);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VideoConsumerDShow::VideoConsumerDShow(VideoConsumerCallback&rCallback)
:m_rCallback(rCallback)
,m_pFilterGraph(NULL)
,m_pRenderFilter(NULL)
,m_pColourFilter(NULL)
,m_pVideoConsumerFilter(NULL)
,m_pVideoDisplay(NULL)
,m_pWindowless(NULL)

,m_nWidth(0)
,m_nHeight(0)
,m_nFrameRate(1)

,m_hWnd(NULL)
,m_bNV12(false)

,m_nVideoRenderType(VR_SDR)
{
	m_pVideoWindow=NULL;
	m_pMediaControl=NULL;
}

VideoConsumerDShow::~VideoConsumerDShow(void)
{
}


int VideoConsumerDShow::Open(long nHandle,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType)
{
	m_hWnd=(HWND)nHandle;
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	if (nFrameRate<1)
		nFrameRate=1;
	m_nFrameRate=nFrameRate;

	m_bNV12=(nCSPType==VC_CSP_NV12);

    HRESULT hr; 

	while (1)
	{
		if (VIDEC_AH2KDev::GetDevCount()>0 && 
			VIDEC_GetEnableVideoDisplaySmoothly() &&
			IsVistaOrAbove() && 
			g_bEVRFailed==false)
		{
			hr=CoCreateInstance(CLSID_EnhancedVideoRenderer, NULL, CLSCTX_INPROC,	IID_IBaseFilter, (void**)&m_pRenderFilter);
			if (FAILED(hr)) 
			{
				g_bEVRFailed=true;
			}
			else
			{
				hr = MyInitializeEVR(m_pRenderFilter, m_hWnd, &m_pVideoDisplay);
				if (FAILED(hr))
				{
					g_bEVRFailed=true;
					return -1;
				}
				m_nVideoRenderType=VR_EVR;
				break;
			}
		}

		if (g_bVMRFailed==false)
		{
			hr=CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC,	IID_IBaseFilter, (void**)&m_pRenderFilter);
			if (FAILED(hr)) 
			{
				g_bVMRFailed=true;
			}
			else
			{
				hr = MyInitWindowlessVMR(m_pRenderFilter, m_hWnd, &m_pWindowless);
				if (FAILED(hr))
				{
					g_bVMRFailed=true;
					return -1;
				}
				m_nVideoRenderType=VR_VMR;
				nCSPType=VC_CSP_RGB;
				break;
			}
		}
		hr=CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC,	IID_IBaseFilter, (void**)&m_pRenderFilter);
		if (FAILED(hr)) 
		{
			return -1;
		}
		else
		{
			m_nVideoRenderType=VR_SDR;

			nCSPType=VC_CSP_RGB;

			hr=CoCreateInstance(CLSID_Colour, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pColourFilter);
			break;
		}	
	}

	m_pVideoConsumerFilter = (VideoConsumerSource *)VideoConsumerSource::CreateInstance(NULL, &hr,&m_rCallback,m_nWidth,m_nHeight,m_nFrameRate,nCSPType);
	if (FAILED(hr)) 
	{
		return -1;
	}
	m_pVideoConsumerFilter->AddRef();

	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph, (void**)&m_pFilterGraph);
	if (FAILED(hr)) 
	{
		return -1;
	}

	CComPtr<IGraphBuilder> pGraphBuilder;
	hr=m_pFilterGraph -> QueryInterface(IID_IGraphBuilder, (void **)&pGraphBuilder);
	if (FAILED(hr))
	{
		return -1;
	}

	if (m_pColourFilter)
	{
		pGraphBuilder->AddFilter(m_pColourFilter, L"Colour space convertor");
	}

	if (FAILED(pGraphBuilder->AddFilter(m_pRenderFilter, L"Video Renderer")))
	{
		return -1;
	}

	if (FAILED(pGraphBuilder->AddFilter(m_pVideoConsumerFilter, L"Video Source")))
	{
		return -1;
	}

	if (m_pColourFilter)
	{
		if (FAILED(hr = pGraphBuilder -> ConnectDirect(GetOutPin(m_pVideoConsumerFilter,0),GetInPin(m_pColourFilter,0),NULL))) 
		{
			return -1;
		}

		if (FAILED(hr = pGraphBuilder -> ConnectDirect(GetOutPin(m_pColourFilter,0),GetInPin(m_pRenderFilter,0),NULL))) 
		{
			return -1;
		}
	}
	else
	{
		if (FAILED(hr = pGraphBuilder -> Connect(GetOutPin(m_pVideoConsumerFilter,0),GetInPin(m_pRenderFilter,0)))) 
		{
			return -1;
		}
	}

	switch (m_nVideoRenderType)
	{
	case VR_EVR:
		if (m_pVideoDisplay)
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			m_pVideoDisplay->SetVideoPosition(NULL, &rc);
		}
		break;
	case VR_VMR:
		if (m_pWindowless)
		{
			RECT rc;
			GetClientRect(m_hWnd, &rc);
			m_pWindowless->SetVideoPosition(NULL, &rc);
		}
		break;
	default:
		{
			hr=m_pFilterGraph -> QueryInterface(IID_IVideoWindow, (void **)&m_pVideoWindow);
			if (FAILED(hr))
			{
				return -1;
			}

			if (m_pVideoWindow)
			{
				RECT rc;
				GetClientRect(m_hWnd, &rc);

				m_pVideoWindow -> put_Owner((OAHWND)m_hWnd);
				m_pVideoWindow -> put_MessageDrain((OAHWND)m_hWnd);
				m_pVideoWindow -> put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
				m_pVideoWindow -> put_Visible(OATRUE);
				m_pVideoWindow -> SetWindowPosition(0, 0, rc.right-rc.left, rc.bottom-rc.top);
			}
		}
		break;
	}

	hr=m_pFilterGraph -> QueryInterface(IID_IMediaControl, (void **)&m_pMediaControl);
	if (FAILED(hr))
	{
		return -1;
	}

	m_pMediaControl->Run();

	return 0;
}

void VideoConsumerDShow::Close(void)
{
	if (m_pMediaControl)
	{
		m_pMediaControl->Stop();
	}

	if(m_pRenderFilter)
	{
		if (m_pFilterGraph)
			m_pFilterGraph->RemoveFilter(m_pRenderFilter);
		SAFE_RELEASE(m_pRenderFilter);
	}

	if(m_pColourFilter)
	{
		if (m_pColourFilter)
			m_pFilterGraph->RemoveFilter(m_pColourFilter);
		SAFE_RELEASE(m_pColourFilter);
	}


	if(m_pVideoConsumerFilter)
	{
		if (m_pFilterGraph)
			m_pFilterGraph->RemoveFilter(m_pVideoConsumerFilter);
		SAFE_RELEASE(m_pVideoConsumerFilter);
	}

	SAFE_RELEASE(m_pVideoWindow);
	SAFE_RELEASE(m_pMediaControl);
	SAFE_RELEASE(m_pVideoDisplay);
	SAFE_RELEASE(m_pWindowless);
	SAFE_RELEASE(m_pFilterGraph);
}


void VideoConsumerDShow::Repaint(void)
{
	if (m_pVideoDisplay!=NULL)
	{
		HRESULT hr;
		hr=m_pVideoDisplay->RepaintVideo();
		hr=hr;
	}
	
	if (m_pWindowless!=NULL)
	{
		HRESULT hr;
		HDC hDC=::GetDC(m_hWnd);
		hr=m_pWindowless->RepaintVideo(m_hWnd, hDC);
		::ReleaseDC(m_hWnd,hDC);
		hr=hr;
	}
}

void VideoConsumerDShow::SetWndPos(int x,int y,int cx,int cy)
{
	switch (m_nVideoRenderType)
	{
	case VR_EVR:
		if (m_pVideoDisplay!=NULL)
		{
			RECT rc;
			rc.left=x;
			rc.top=y;
			rc.right=x+cx;
			rc.bottom=y+cy;

			HRESULT hr;
			hr=m_pVideoDisplay->SetVideoPosition(NULL, &rc);
			hr=hr;
		}
		break;
	case VR_VMR:
		if (m_pWindowless!=NULL)
		{
			RECT rc;
			rc.left=x;
			rc.top=y;
			rc.right=x+cx;
			rc.bottom=y+cy;

			HRESULT hr;
			hr=m_pWindowless->SetVideoPosition(NULL, &rc);
			hr=hr;
		}
		break;
	default:
		{
			if (m_pVideoWindow)
			{
				m_pVideoWindow -> SetWindowPosition(x, y, cx, cy);
				m_pVideoWindow -> put_Visible(OATRUE);
			}
		}
		break;
	}
}

// Initialize the EVR filter. 

HRESULT MyInitializeEVR( 
					  IBaseFilter *pEVR,              // Pointer to the EVR
					  HWND hwnd,                      // Clipping window
					  IMFVideoDisplayControl** ppDisplayControl
					  ) 
{ 
	IMFGetService *pGS = NULL;
	IMFVideoDisplayControl *pDisplay = NULL;

	HRESULT hr = pEVR->QueryInterface(IID_PPV_ARGS(&pGS)); 
	if (FAILED(hr))
	{
		goto done;
	}

	hr = pGS->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&pDisplay));
	if (FAILED(hr))
	{
		goto done;
	}

	// Set the clipping window.
	hr = pDisplay->SetVideoWindow(hwnd);
	if (FAILED(hr))
	{
		goto done;
	}

	// Preserve aspect ratio by letter-boxing
	hr = pDisplay->SetAspectRatioMode(MFVideoARMode_None);
	if (FAILED(hr))
	{
		goto done;
	}

	// Return the IMFVideoDisplayControl pointer to the caller.
	*ppDisplayControl = pDisplay;
	(*ppDisplayControl)->AddRef();

done:
	SAFE_RELEASE(pGS);
	SAFE_RELEASE(pDisplay);
	return hr; 
} 

//-----------------------------------------------------------------------------
// MyInitWindowlessVMR
// Initialize the VMR-7 for windowless mode. 
//-----------------------------------------------------------------------------

HRESULT MyInitWindowlessVMR( 
						  IBaseFilter *pVMR,				// Pointer to the VMR
						  HWND hwnd,						// Clipping window
						  IVMRWindowlessControl** ppWC	// Receives a pointer to the VMR.
						  ) 
{ 

	IVMRFilterConfig* pConfig = NULL; 
	IVMRWindowlessControl *pWC = NULL;

	HRESULT hr = S_OK;

	// Set the rendering mode.  
	hr = pVMR->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig); 
	if (SUCCEEDED(hr)) 
	{ 
		hr = pConfig->SetRenderingMode(VMRMode_Windowless); 

		DWORD dwPrefs=0;
		hr = pConfig->GetRenderingPrefs(&dwPrefs); 
		dwPrefs|=RenderPrefs_ForceOffscreen;
		hr = pConfig->SetRenderingPrefs(dwPrefs); 
	}

	// Query for the windowless control interface.
	if (SUCCEEDED(hr))
	{
		hr = pVMR->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWC);
	}

	// Set the clipping window.
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetVideoClippingWindow(hwnd);
	}

	// Preserve aspect ratio by letter-boxing
	if (SUCCEEDED(hr))
	{
		hr = pWC->SetAspectRatioMode(VMR_ARMODE_NONE);
	}

	// Return the IVMRWindowlessControl pointer to the caller.
	if (SUCCEEDED(hr))
	{
		*ppWC = pWC;
		(*ppWC)->AddRef();
	}

	SAFE_RELEASE(pConfig);
	SAFE_RELEASE(pWC);

	return hr; 
} 


HRESULT GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
	CComPtr< IEnumPins > pEnum;
	*ppPin = NULL;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if(FAILED(hr)) 
		return hr;

	ULONG ulFound;
	IPin *pPin;
	hr = E_FAIL;

	while(S_OK == pEnum->Next(1, &pPin, &ulFound))
	{
		PIN_DIRECTION pindir = (PIN_DIRECTION)3;
		pPin->QueryDirection(&pindir);
		if(pindir == dirrequired)
		{
			if(iNum == 0)
			{
				*ppPin = pPin;
				// Found requested pin, so clear error
				hr = S_OK;
				break;
			}
			iNum--;
		} 

		pPin->Release();
	} 

	return hr;
}

IPin * GetInPin( IBaseFilter * pFilter, int Num )
{
	CComPtr< IPin > pComPin;
	GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
	return pComPin;
}


IPin * GetOutPin( IBaseFilter * pFilter, int Num )
{
	CComPtr< IPin > pComPin;
	GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
	return pComPin;
}