#include <windows.h>
#include "D3D9Render.h"
#include <dwmapi.h>
#include "VIDECD3DAPI.h"
#include <stdio.h>

#include "../../VIDEC/VIDEC_csp/VIDEC_csp.h"


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


//-------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------

int		g_nPresentationIntervalType = -1;

D3D9Render::D3D9Render() : 
    m_hWnd(NULL),
    m_pD3D(NULL),
    m_pDevice(NULL),
	m_pSwapChain(NULL),
    m_nWidth(0),
    m_nHeight(0)
{
    ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));
	ZeroMemory(&m_rcDst, sizeof(m_rcDst));

	m_nAdapter=0;
	m_hMonitor=NULL;

	m_nFlagSkipDisplay=-1;
	m_nFrameRate=60;
	m_bDeviceLost=false;

	m_bResetImageFormat=false;

	m_nFrameLen=0;

	if (g_nPresentationIntervalType == -1)
	{
		g_nPresentationIntervalType=GetPrivateProfileInt(("VIDEC"), ("PIT") , 0, ("VIDEC.ini"));
		char strValue[1024];
		sprintf(strValue,("%d"),g_nPresentationIntervalType);
		WritePrivateProfileString(("VIDEC"), ("PIT")  , strValue, ("VIDEC.ini"));
	}
}


//-------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------

D3D9Render::~D3D9Render()
{
}

int D3D9Render::Open(HWND hWnd,int nWidth,int nHeight)
{
    if (m_pDevice)
    {
        return 0;
    }

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameLen=nWidth*nHeight/2*3;

    // Create the Direct3D object.
    if (m_pD3D == NULL)
    {
        HRESULT hr = Direct3DCreate9Ex(D3D_SDK_VERSION,&m_pD3D);

        if (m_pD3D == NULL || FAILED(hr))
        {
            return -1;
        }
    }

	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

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

	m_hMonitor=hMonitor;
	m_nAdapter=nAdapter;



    HRESULT hr = S_OK;
    D3DPRESENT_PARAMETERS pp = { 0 };
    D3DDISPLAYMODE mode = { 0 };

    hr = m_pD3D->GetAdapterDisplayMode(
        m_nAdapter, 
        &mode
        );

    if (FAILED(hr)) { goto done; }

	hr = m_pD3D->CheckDeviceType(
        m_nAdapter,
        D3DDEVTYPE_HAL,
        mode.Format,
        D3DFMT_X8R8G8B8,
        TRUE    // windowed
        );

    if (FAILED(hr)) { goto done; }

    pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	pp.BackBufferWidth  = mode.Width;
	pp.BackBufferHeight = mode.Height;
	pp.BackBufferCount = 2;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    if (g_nPresentationIntervalType == 1)
	{
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}

    pp.Windowed = TRUE;
    pp.hDeviceWindow = hWnd;
	pp.Flags = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    hr = m_pD3D->CreateDeviceEx(
        m_nAdapter,
        D3DDEVTYPE_HAL,
        hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE | D3DCREATE_MULTITHREADED,
        &pp,
		NULL,
        &m_pDevice
        );

    if (FAILED(hr)) { goto done; }

	hr = m_pDevice->ResetEx(&pp, NULL);
	if (FAILED(hr)) { goto done; }

	hr = m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (FAILED(hr)) { goto done; }


	hr=CreateSwapChains();

	if (FAILED(hr)) { goto done; }
	

    m_hWnd = hWnd;
    m_d3dpp = pp;

	m_rcDst.bottom=mode.Height;
	m_rcDst.right=mode.Width;

	FrameRateEvent::Open();

done:
	return (hr==S_OK?0:-1);
}


void D3D9Render::Close(void)
{
	FrameRateEvent::Close();

	m_bResetImageFormat=false;

	SafeRelease(&m_pDevice);
	SafeRelease(&m_pD3D);
	SafeRelease(&m_pSwapChain);
}


//-------------------------------------------------------------------
// DrawFrame
//
// Draw the video frame.
//-------------------------------------------------------------------

int D3D9Render::DrawFrame(unsigned char*pFrameData,int nFrameLen,int nFrameRate,int nWidth,int nHeight)
{
    HRESULT hr = S_OK;

	D3DLOCKED_RECT lr;

    IDirect3DSurface9 *pSurf = NULL;
	IDirect3DSurface9 *pBB = NULL;

	if (m_nFlagSkipDisplay==0x10000000)
	{
		m_nFlagSkipDisplay=0;
		OutputDebugString("DrawFrame 0\n");
		return 0;
	}


	if (m_nFlagSkipDisplay==-1)
	{
		m_nFlagSkipDisplay=0;
	}


	if (m_nFlagSkipDisplay)
	{
		OutputDebugString("DrawFrame 1\n");
		return 0;
	}

	if (nFrameRate!=m_nFrameRate)
	{
		m_nFrameRate=nFrameRate;
		FrameRateEvent::SetFrameRate(m_nFrameRate);
	}


	if (!IsWindowVisible(m_hWnd))
	{
		OutputDebugString("DrawFrame 2\n");
		return 0;
	}

	RECT rect;
	GetClientRect(m_hWnd, &rect);
	if (IsRectEmpty(&rect))
	{
		OutputDebugString("DrawFrame 3\n");
		return 0;
	}

	if (m_nWidth!=nWidth || m_nHeight!=nHeight)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameLen=nWidth*nHeight/2*3;
		m_bResetImageFormat=true;
	}

	if (m_bResetImageFormat)
	{
		m_bResetImageFormat=false;
		SafeRelease(&m_pSwapChain);

		hr=CreateSwapChains();
		if (hr!=S_OK)
		{
			OutputDebugString("DrawFrame 4\n");
			return 0;
		}
	}


	if (m_pDevice == NULL || m_pSwapChain==NULL)
	{
		Close();
		hr=ResetDevice();
		if (hr!=S_OK)
		{
			OutputDebugString("DrawFrame 5\n");
			return 0;
		}
	}

	if (g_nPresentationIntervalType != 1)
	{
		FrameRateEvent::WaitEvent();
	}

	if (m_bDeviceLost)
	{
		// Check the current status of D3D9 device.
		hr = m_pDevice->TestCooperativeLevel();

		switch (hr)
		{
		case D3D_OK:
			{
				m_bDeviceLost=false;
			}
			break;
		case D3DERR_DEVICELOST:
			{
				OutputDebugString("DrawFrame 6\n");
				return 0;
			}
		case D3DERR_DEVICENOTRESET:
			hr = ResetDevice();
			if (hr!=S_OK)
			{
				OutputDebugString("DrawFrame 7\n");
				return 0;
			}
			break;
		}
	}

	HMONITOR hMonitor = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	if (hMonitor!=m_hMonitor)
	{
		Close();
		hr=ResetDevice();
		if (hr!=S_OK)
		{
			OutputDebugString("DrawFrame 8\n");
			return 0;
		}
	}

	hr = m_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSurf);

    if (FAILED(hr)) { goto done; }

    hr = pSurf->LockRect(&lr, NULL, D3DLOCK_NOSYSLOCK );

    if (FAILED(hr)) { goto done; }


	unsigned char*pYV12[3];
	pYV12[0]=pFrameData;
	pYV12[1]=pYV12[0]+m_nWidth*m_nHeight;
	pYV12[2]=pYV12[1]+m_nWidth*m_nHeight/4;

	YV12_TO_RGB32(pYV12,(BYTE*)lr.pBits,m_nWidth,m_nHeight,lr.Pitch,0);

    hr = pSurf->UnlockRect();

    if (FAILED(hr)) { goto done; }

	{
		unsigned long nStartTimestamp=timeGetTime();
		// Color fill the back buffer.
		hr = m_pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBB);

		if (FAILED(hr)) { goto done; }
		hr = m_pDevice->StretchRect(pSurf, NULL, pBB, &m_rcDst, D3DTEXF_LINEAR);

		if (FAILED(hr)) { goto done; }


		hr = m_pDevice->Present(NULL, NULL, NULL, NULL);
		if (hr == D3DERR_DEVICELOST) 
		{
			m_bDeviceLost = true;
		}
	}
done:
    SafeRelease(&pSurf);
	SafeRelease(&pBB);
	return (hr==S_OK?0:-1);
}

//-------------------------------------------------------------------
// ResetDevice
//
// Resets the Direct3D device.
//-------------------------------------------------------------------

HRESULT D3D9Render::ResetDevice(void)
{
    HRESULT hr = S_OK;

    if (m_pDevice)
    {
        D3DPRESENT_PARAMETERS d3dpp = m_d3dpp;

        hr = m_pDevice->Reset(&d3dpp);

        if (FAILED(hr))
        {
            Close();
        }
    }

    if (m_pDevice == NULL)
    {
        if (Open(m_hWnd,m_nWidth,m_nHeight)==0)
		{
			hr=S_OK;
		}
		else
		{
			hr=S_FALSE;
		}

        if (FAILED(hr)) { goto done; }
    }

done:

   return hr;
}


HRESULT D3D9Render::CreateSwapChains(void)
{
	HRESULT hr = S_OK;

	D3DPRESENT_PARAMETERS pp = { 0 };


	pp.BackBufferWidth  = m_nWidth;
	pp.BackBufferHeight = m_nHeight;
	pp.Windowed = TRUE;
	pp.hDeviceWindow = m_hWnd;
	pp.BackBufferFormat = D3DFMT_X8R8G8B8;
	pp.BackBufferCount = 2;
	pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if (g_nPresentationIntervalType == 1)
	{
		pp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	}
	else
	{
		pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	pp.Flags = D3DPRESENTFLAG_VIDEO | D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	hr = m_pDevice->CreateAdditionalSwapChain(&pp, &m_pSwapChain);

	return hr;
}

int D3D9Render::SkipDisplay(int nFlagSkipDisplay)
{
	m_nFlagSkipDisplay=nFlagSkipDisplay;
	return 0;
}


int D3D9Render::GetWidth(void)
{
	return m_nWidth;
}

int D3D9Render::GetHeight(void)
{
	return m_nHeight;
}


static bool g_bVIDECD3DInitialized=false;
static bool g_bVIDECD3DHardware=false;

VIDECD3D_API int VIDECD3D_Init(unsigned long nKey)
{
	if (g_bVIDECD3DInitialized)
	{
		return 0;
	}

	g_bVIDECD3DInitialized=true;

	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return -1;
	}

	g_bVIDECD3DHardware=true;

	return 0;
}

VIDECD3D_API void*VIDECD3D_PlayerOpen(HWND hWnd, int nWidth, int nHeight)
{
	if (!g_bVIDECD3DHardware || !g_bVIDECD3DInitialized)
	{
		return NULL;
	}

	D3D9Render*pD3D9Render=new D3D9Render();
	if (0!=pD3D9Render->Open(hWnd,nWidth,nHeight))
	{
		pD3D9Render->Close();
		delete pD3D9Render;
		pD3D9Render=NULL;
	}
	return pD3D9Render;
}

VIDECD3D_API void VIDECD3D_PlayerClose(void*hPlayer)
{
	D3D9Render*pD3D9Render=(D3D9Render*)hPlayer;
	if (pD3D9Render)
	{
		pD3D9Render->Close();
		delete pD3D9Render;
		pD3D9Render=NULL;
	}
}

VIDECD3D_API int VIDECD3D_PlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int nWidth,int nHeight)
{
	D3D9Render*pD3D9Render=(D3D9Render*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->DrawFrame((unsigned char*)pFrameData,nFrameLen,nFrameRate,nWidth,nHeight);
	}

	return -1;
}

VIDECD3D_API int VIDECD3D_PlayerGetWidth(void*hPlayer)
{
	D3D9Render*pD3D9Render=(D3D9Render*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetWidth();
	}

	return 1280;
}

VIDECD3D_API int VIDECD3D_PlayerGetHeight(void*hPlayer)
{
	D3D9Render*pD3D9Render=(D3D9Render*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->GetHeight();
	}

	return 720;
}

VIDECD3D_API int VIDECD3D_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	D3D9Render*pD3D9Render=(D3D9Render*)hPlayer;
	if (pD3D9Render)
	{
		return pD3D9Render->SkipDisplay(nFlagSkipDisplay);
	}

	return -1;
}
