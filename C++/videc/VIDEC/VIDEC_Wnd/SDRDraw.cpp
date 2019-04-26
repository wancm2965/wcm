// SDRDraw.cpp: implementation of the SDRDraw class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "SDRDraw.h"
#include "VIDEC_csp.h"
#include "XAutoLock.h"
#include "VIDEC_Convert.h"
#include "../jpeglib/jpegutils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
,m_nFrameRate(0)
{
	m_ulLastCountFrameTimestamp=0;
	m_nFrameRateCur=0;	
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

	DebugOutput( logcallstack ,"SDRDraw::Open \n");

	hr=CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IFilterGraph, (void**)&m_pifg);
	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"SDRDraw::Open CoCreateInstance Failed\n");
		return -1;
	}
	
	hr=SetNoClock(m_pifg);

	
	NEWRECORD( m_pSDRDrawFilter ,new SDRDrawFilter() )
	NULLReturnRet( m_pSDRDrawFilter , -1 )  
	m_pSDRDrawFilter->SetImageFormat(m_nWidth,m_nHeight);
	m_pSDRDrawFilter->AddRef();
	m_pifg -> AddFilter(m_pSDRDrawFilter, NULL);


	CComPtr<IGraphBuilder> pigb;
	hr=m_pifg -> QueryInterface(IID_IGraphBuilder, (void **)&pigb);
	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"SDRDraw::Open QueryInterface Failed\n");
		return -1;
	}


	hr=CoCreateInstance(CLSID_Colour, NULL, CLSCTX_INPROC,
        IID_IBaseFilter, (void**)&m_pColour);
	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"SDRDraw::Open CoCreateInstance  CLSID_Colour Failed\n");
	}
	else
	{
		if (m_pColour!=NULL)
		{
			m_pifg -> AddFilter(m_pColour, L"Colour space convertor");
		}
	}


	hr=CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC,	IID_IBaseFilter, (void**)&m_pRenderer);		//修正新平台低分辨率下雪花点的问题
	if (FAILED(hr) || m_pRenderer==NULL)
	{
		DebugOutput( logcoreerror ,"SDRDraw::Open CoCreateInstance  CLSID_VideoMixingRenderer Failed\n");
		hr=CoCreateInstance(CLSID_VideoRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (void**)&m_pRenderer);
		if (FAILED(hr))
		{
			DebugOutput( logcoreerror ,"SDRDraw::Open CoCreateInstance CLSID_VideoRenderer Failed\n");
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
			DebugOutput( logcoreexcep ,"SDRDraw::Open ConnectDirect Failed\n");
			return -1;
		}

		if (FAILED(hr = pigb -> ConnectDirect(GetOutPin(m_pColour,0),GetInPin(m_pRenderer,0),NULL))) 
		{
			DebugOutput( logcoreexcep ,"SDRDraw::Open ConnectDirect 2 Failed\n");
			return -1;
		}
	}
	else
	{
		if (m_pRenderer)
		{
			if (FAILED(hr = pigb -> Connect(m_pSDRDrawFilter->GetPin(0),GetInPin(m_pRenderer,0)))) 
			{
				DebugOutput( logcoreexcep ,"SDRDraw::Open Connect 1 Failed\n");
				return -1;
			}
		}
		else
		{
			if (FAILED(hr = pigb -> Render(m_pSDRDrawFilter->GetPin(0)))) 
			{
				DebugOutput( logcoreexcep ,"SDRDraw::Open Render  Failed\n");
				return -1;
			}
		}
	}


	hr=m_pifg -> QueryInterface(IID_IMediaControl, (void **)&m_pimc);
	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"SDRDraw::Open QueryInterface  IID_IMediaControl Failed\n");
		return -1;
	}


    hr=m_pifg -> QueryInterface(IID_IVideoWindow, (void **)&m_pivw);
	if (FAILED(hr))
	{
		DebugOutput( logcoreexcep ,"SDRDraw::Open QueryInterface  IID_IVideoWindow Failed\n");
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


	return 0;
}

void SDRDraw::Close(void)
{

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

	DebugOutput( logcallstack ,"SDRDraw::Close \n");
}


int SDRDraw::ResetImageFormat(int nWidth,int nHeight)
{
	//DebugOutput("SDRDraw::ResetImageFormat \n");
	if (m_nWidth==nWidth && m_nHeight==nHeight)
	{
		return 0;
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	if (m_pifg==NULL)
	{
		DebugOutput( logcoreexcep ,"SDRDraw::ResetImageFormat m_pifg==NULL\n");
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
		DebugOutput( logcoreexcep ,"SDRDraw::ResetImageFormat QueryInterface failed\n");
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
			DebugOutput( logcoreexcep ,"SDRDraw::ResetImageFormat ConnectDirect failed\n");
			return -1;
		}

		if (FAILED(hr = pigb -> ConnectDirect(GetOutPin(m_pColour,0),GetInPin(m_pRenderer,0),NULL))) 
		{
			DebugOutput( logcoreexcep ,"SDRDraw::ResetImageFormat ConnectDirect failed\n");
			return -1;
		}
	}
	else
	{
		DebugOutput( logcoreexcep ,"SDRDraw::ResetImageFormat m_pColour && m_pSDRDrawFilter && m_pRenderer else\n");
		return -1;
	}

	if (m_pimc)
	{
		m_pimc->Run();
	}

	return 0;
}

char gszDebug[1024];

void SDRDraw::SetWindowPos(int x,int y,int cx,int cy)
{

	sprintf(gszDebug,"SDRDraw SetWindowPos th:%p,x= %u, y= %u, cx= %u, cy= %u,pivw= %p \n", this, x,  y,  cx, cy ,m_pivw);
	DebugOutput( logcallstack ,gszDebug);

	if (m_pivw) 
	{
		m_pivw -> SetWindowPosition(x, y, cx, cy);
		m_pivw -> put_Visible(OATRUE);
		if (cx==1 && cy ==1)
		{
			m_pivw->put_Visible(OAFALSE);			
		}
	}
}

int SDRDraw::GetFrameRate(void)
{
	//unsigned long ulCurTimestamp = timeGetTime();
	//if (m_ulLastCountFrameTimestamp == 0)
	//{
	//	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	//}
	//unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	//if (ulTemp > 2000)
	//{
	//	m_nFrameRate=0;
	//}

	//ReSetFrameRate();

	int nRate = m_nFrameRate;
	m_nFrameRate=0;
	return nRate;
}

void SDRDraw::ReSetFrameRate(void)
{
	unsigned long ulCurTimestamp = timeGetTime();
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if ((m_nFrameRateCur > 30 && ulTemp < 990) || (m_nFrameRateCur <= 30 && ulTemp < 980))
	{
		return;
	}
	
	if (ulTemp > 1500)
	{
		float f = 1000.00/ulTemp;
		m_nFrameRate=m_nFrameRateCur*f;
	}
	else
	{
		m_nFrameRate=m_nFrameRateCur;
	}
	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	m_nFrameRateCur=0;
}

bool SDRDraw::Draw(unsigned char*pData,int nLen,int nFrameRate)
{
	if (m_pSDRDrawFilter)
	{
		m_nFrameRateCur++;
		ReSetFrameRate();
		return m_pSDRDrawFilter->DeliverData(pData,nLen);
	}

	return false;
}

HRESULT SDRDraw::GetPin( IBaseFilter * pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
	//IEnumPins * pEnum;//use 160913 memory leak 
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


	//pEnum->Reset();
	//pEnum->Release(); //use 160913 memory leak 
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