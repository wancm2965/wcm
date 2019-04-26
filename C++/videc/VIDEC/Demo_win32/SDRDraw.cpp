// SDRDraw.cpp: implementation of the SDRDraw class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "SDRDraw.h"
//#include "VIDEC_csp.h"
#include "XAutoLock.h"
//#include "VIDEC_Convert.h"
//#include "../jpeglib/jpegutils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int		g_nPresentationIntervalType_DEMO = -1;

#define HELPER_RELEASE(x) {if(x) x -> Release(); x = NULL;}
HRESULT SetNoClock(IFilterGraph *graph)
{
    // Keep a useless clock from being instantiated....
    IMediaFilter *graphF;
    HRESULT hr = graph->QueryInterface(IID_IMediaFilter, (void **) &graphF);

    if(SUCCEEDED(hr))
    {
        hr = graphF->SetSyncSource(NULL);

        graphF->Release();
    }
    else
    {
    }

    return hr;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SDRDraw::SDRDraw(int nVirtualWidth,int nVirtualHeight)
:m_pivw(NULL)
,m_pifg(NULL)
,m_pimc(NULL)
,m_pColour(NULL)
,m_pRenderer(NULL)
,m_pSDRDrawFilter(NULL)
,m_nWidth(0)
,m_nHeight(0)
,m_hWnd(NULL)
,m_nVirtualWidth(nVirtualWidth)
,m_nVirtualHeight(nVirtualHeight)
,m_nFrameRate(60)
{
	if (g_nPresentationIntervalType_DEMO == -1)
	{
		TCHAR chIniPath[1024];
		GetWindowsDirectory(chIniPath,sizeof(chIniPath));
		lstrcat(chIniPath,_T("\\VIDEC.ini"));	

		g_nPresentationIntervalType_DEMO=GetPrivateProfileInt(_T("VIDEC"), _T("PIT") , 0, chIniPath);

		//CString strValue;
		//strValue.Format(_T("%d"),g_nPresentationIntervalType_DEMO);
		//WritePrivateProfileString(_T("VIDEC"), _T("PIT")  , strValue, chIniPath);
	}
}

SDRDraw::~SDRDraw(void)
{
}


int SDRDraw::Open(HWND hWnd,int nWidth,int nHeight)
{
	m_hWnd=hWnd;
	m_nWidth=nWidth;
	m_nHeight=nHeight;


    HRESULT hr; 

	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph, (void**)&m_pifg);
	if (FAILED(hr))
	{
		return -1;
	}
	
	hr=SetNoClock(m_pifg);

	
	m_pSDRDrawFilter=new SDRDrawFilter();
	m_pSDRDrawFilter->SetImageFormat(m_nWidth,m_nHeight);
	m_pSDRDrawFilter->AddRef();
	m_pifg -> AddFilter(m_pSDRDrawFilter, NULL);


	CComPtr<IGraphBuilder> pigb;
	hr=m_pifg -> QueryInterface(IID_IGraphBuilder, (void **)&pigb);
	if (FAILED(hr))
	{
		return -1;
	}


	hr=CoCreateInstance(CLSID_Colour, NULL, CLSCTX_INPROC,
        IID_IBaseFilter, (void**)&m_pColour);
	if (FAILED(hr))
	{

	}
	else
	{
		if (m_pColour!=NULL)
		{
			m_pifg -> AddFilter(m_pColour, L"Colour space convertor");
		}
	}


	hr=CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC,	IID_IBaseFilter, (void**)&m_pRenderer);
	if (FAILED(hr) || m_pRenderer==NULL)
	{
		hr=CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pRenderer);
		if (FAILED(hr))
		{
		}
		else
		{
			m_pifg -> AddFilter(m_pRenderer, L"VideoRenderer");
		}
	}
	else
	{
		m_pifg -> AddFilter(m_pRenderer, L"VideoMixingRenderer");
	}

	if (m_pColour)
	{
		if (FAILED(hr = pigb -> ConnectDirect(m_pSDRDrawFilter->GetPin(0),GetInPin(m_pColour,0),NULL))) 
		{
			return -1;
		}

		if (FAILED(hr = pigb -> ConnectDirect(GetOutPin(m_pColour,0),GetInPin(m_pRenderer,0),NULL))) 
		{
			return -1;
		}
	}
	else
	{
		if (m_pRenderer)
		{
			if (FAILED(hr = pigb -> Connect(m_pSDRDrawFilter->GetPin(0),GetInPin(m_pRenderer,0)))) 
			{
				return -1;
			}
		}
		else
		{
			if (FAILED(hr = pigb -> Render(m_pSDRDrawFilter->GetPin(0)))) 
			{
				return -1;
			}
		}
	}


	hr=m_pifg -> QueryInterface(IID_IMediaControl, (void **)&m_pimc);
	if (FAILED(hr))
	{
		return -1;
	}


    hr=m_pifg -> QueryInterface(IID_IVideoWindow, (void **)&m_pivw);
	if (FAILED(hr))
	{
		return -1;
	}

	{
		m_pivw -> put_Owner((OAHWND)hWnd);
		m_pivw -> put_MessageDrain((OAHWND)hWnd);
		m_pivw -> put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_pivw -> put_Visible(OAFALSE);

		m_pimc->Run();

		m_pivw -> put_Visible(OAFALSE);
	}

	if (g_nPresentationIntervalType_DEMO == 0)
	{
		FrameRateEvent::Open();
	}

	return 0;
}

void SDRDraw::Close(void)
{
	if (g_nPresentationIntervalType_DEMO == 0)
	{
		FrameRateEvent::Close();
	}
	if (m_pimc){
		m_pimc->Stop();
	}

	if (m_pivw)
	{
		m_pivw -> put_Visible(OAFALSE);
	}

	HELPER_RELEASE(m_pimc);
	HELPER_RELEASE(m_pivw);
	HELPER_RELEASE(m_pColour);
	HELPER_RELEASE(m_pRenderer);
	HELPER_RELEASE(m_pSDRDrawFilter);
	HELPER_RELEASE(m_pifg);
}


int SDRDraw::ResetImageFormat(int nWidth,int nHeight)
{
	if (m_nWidth==nWidth && m_nHeight==nHeight)
	{
		return 0;
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	if (m_pifg==NULL)
	{
		return -1;
	}


	if (m_pimc){
		m_pimc->Stop();
	}


	HRESULT hr; 

	CComPtr<IGraphBuilder> pigb;
	hr=m_pifg -> QueryInterface(IID_IGraphBuilder, (void **)&pigb);
	if (FAILED(hr))
	{
		return -1;
	}

	if (m_pRenderer)
	{
		pigb->Disconnect(GetInPin(m_pRenderer,0));
	}

	if (m_pColour)
	{
		pigb->Disconnect(GetOutPin(m_pColour,0));
		pigb->Disconnect(GetInPin(m_pColour,0));
	}

	if (m_pSDRDrawFilter)
	{
		pigb->Disconnect(m_pSDRDrawFilter->GetPin(0));
	}

	if (m_pColour && m_pSDRDrawFilter && m_pRenderer)
	{
		m_pSDRDrawFilter->SetImageFormat(m_nWidth,m_nHeight);

		if (FAILED(hr = pigb -> ConnectDirect(m_pSDRDrawFilter->GetPin(0),GetInPin(m_pColour,0),NULL))) 
		{
			return -1;
		}

		if (FAILED(hr = pigb -> ConnectDirect(GetOutPin(m_pColour,0),GetInPin(m_pRenderer,0),NULL))) 
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	if (m_pimc)
	{
		m_pimc->Run();
	}

	return 0;
}

void SDRDraw::SetWindowPos(int x,int y,int cx,int cy)
{
	if (m_pivw) 
	{
		m_pivw -> SetWindowPosition(x, y, cx, cy);
		m_pivw -> put_Visible(OATRUE);
	}
}

bool SDRDraw::Draw(unsigned char*pData,int nLen, int nWidth,int nHeight,int nFrameRate)
{
	ResetImageFormat(nWidth,nHeight);

	if (g_nPresentationIntervalType_DEMO == 0)
	{
		if (m_nFrameRate!=nFrameRate)
		{
			m_nFrameRate=nFrameRate;
			FrameRateEvent::SetFrameRate(m_nFrameRate);
		}		
		FrameRateEvent::WaitEvent();
	}

	if (m_pSDRDrawFilter)
	{
		return m_pSDRDrawFilter->DeliverData(pData,nLen);
	}

	return false;
}

HRESULT SDRDraw::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
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

IPin * SDRDraw::GetInPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_INPUT, Num, &pComPin);
    return pComPin;
}


IPin * SDRDraw::GetOutPin( IBaseFilter * pFilter, int Num )
{
    CComPtr< IPin > pComPin;
    GetPin(pFilter, PINDIR_OUTPUT, Num, &pComPin);
    return pComPin;
}