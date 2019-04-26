// DSDisplayGraph.cpp: implementation of the DSDisplayGraph class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "DSDisplayGraph.h"
#include "DSUtils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DSDisplayGraph::DSDisplayGraph(void)
:m_pVideoWindow(NULL)
,m_pMediaController(NULL)

,m_pGraphBuilder(NULL)

,m_pColorspaceConverterFilter(NULL)
,m_pVideoRendererFilter(NULL)
,m_pSourceFilter(NULL)

,m_nWidth(0)
,m_nHeight(0)
{
	m_bRunning=false;
	m_hWndParent=NULL;
	m_bConnected=false;
}

DSDisplayGraph::~DSDisplayGraph(void)
{
}


int DSDisplayGraph::Open(HWND hWndParent)
{
	HRESULT hr;
	m_hWndParent=hWndParent;
	hr=CreateDisplayGraph();
	if (FAILED(hr))
	{
		return -1;
	}

	return 0;
}

void DSDisplayGraph::Close(void)
{
	Stop();
	Disconnect();

	SAFE_RELEASE(m_pVideoWindow);
	SAFE_RELEASE(m_pMediaController);

	SAFE_RELEASE(m_pColorspaceConverterFilter);
	SAFE_RELEASE(m_pVideoRendererFilter);
	SAFE_RELEASE(m_pSourceFilter);

	SAFE_RELEASE(m_pGraphBuilder);
}

int DSDisplayGraph::SetTitle(const char*cszTitle,unsigned long nColor)
{
	if (m_pSourceFilter)
	{
		return m_pSourceFilter->SetTitle(cszTitle,nColor);
	}
	return -1;
}

bool DSDisplayGraph::DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3],int nWidth,int nHeight)
{
	if (!m_pSourceFilter ||
		nWidth<=0 || nHeight<=0)
	{
		return false;
	}

	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight)
	{
		return false;
	}

	m_pSourceFilter->DisplayFrame(pYUV420Data,nStride);
	return true;
}

void DSDisplayGraph::SetImageFormat(int nWidth,int nHeight)
{
	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;

		//Stop();

		//Disconnect();

		m_pSourceFilter->SetImageFormat(nWidth, nHeight);

		Connect();

		Start();
	}
}

HRESULT DSDisplayGraph::Connect(void)
{
	HRESULT hr;

	{
		IMediaFilter*pMediaFilter=NULL;;
		hr = QUERY(m_pGraphBuilder, IID_IMediaFilter, pMediaFilter);
		if(SUCCEEDED(hr))
		{
			hr = pMediaFilter->SetSyncSource(NULL);
			pMediaFilter->Release();
		}
	}

	if((hr = ConnectFilters(m_pGraphBuilder, m_pSourceFilter, m_pColorspaceConverterFilter)) != S_OK){
		return hr;
	}
	if((hr = ConnectFilters(m_pGraphBuilder, m_pColorspaceConverterFilter, m_pVideoRendererFilter)) != S_OK){
		return hr;
	}

	{
		IMediaFilter*pMediaFilter=NULL;;
		hr = QUERY(m_pGraphBuilder, IID_IMediaFilter, pMediaFilter);
		if(SUCCEEDED(hr))
		{
			hr = pMediaFilter->SetSyncSource(NULL);
			pMediaFilter->Release();
		}
	}

	m_bConnected=true;

	return S_OK;
}

HRESULT DSDisplayGraph::Disconnect(void)
{
	HRESULT hr;

	m_bConnected=false;

	if((hr = DisconnectFilters(m_pGraphBuilder, m_pSourceFilter, m_pColorspaceConverterFilter)) != S_OK){
		return hr;
	}
	if((hr = DisconnectFilters(m_pGraphBuilder, m_pColorspaceConverterFilter, m_pVideoRendererFilter)) != S_OK){
		return hr;
	}

	return S_OK;
}

HRESULT DSDisplayGraph::Start(void)
{
	HRESULT hr=S_OK;
	if (!m_bRunning)
	{
		m_bRunning = true;

		if (m_bConnected)
		{
			if (m_pSourceFilter)
			{
				//m_pSourceFilter->reset();
			}

			RECT rect = {0};
			GetWindowRect(m_hWndParent, &rect);
			int nWidth=rect.right-rect.left;
			int nHeight=rect.bottom-rect.top;

			m_pVideoWindow->put_Visible(OAFALSE);
			m_pVideoWindow->put_AutoShow(OAFALSE);

			m_pVideoWindow->put_Owner((OAHWND) m_hWndParent);
			m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			m_pVideoWindow->SetWindowPosition(0, 0, nWidth, nHeight);
			m_pVideoWindow->put_MessageDrain((OAHWND) m_hWndParent);

			{
				IMediaFilter*pMediaFilter=NULL;;
				hr = QUERY(m_pGraphBuilder, IID_IMediaFilter, pMediaFilter);
				if(SUCCEEDED(hr))
				{
					hr = pMediaFilter->SetSyncSource(NULL);
					pMediaFilter->Release();
				}
			}

			hr = m_pMediaController->Run();
			if (!SUCCEEDED(hr)){
			}
		}
	}
	return hr;
}


HRESULT DSDisplayGraph::Stop(void)
{
	HRESULT hr=S_OK;

	if (IsRunning())
	{
		hr = m_pMediaController->Stop();
		if (!SUCCEEDED(hr)){
		}
	}

	m_bRunning = false;

	return hr;
}

bool DSDisplayGraph::IsRunning(void)
{
	return m_bRunning;
}

HRESULT DSDisplayGraph::CreateDisplayGraph(void)
{
	HRESULT hr;

	// Create the graph builder
	hr = COCREATE(CLSID_FilterGraph, IID_IGraphBuilder, m_pGraphBuilder);
	if(FAILED(hr)) return hr;

	IMediaFilter*pMediaFilter=NULL;;
	hr = QUERY(m_pGraphBuilder, IID_IMediaFilter, pMediaFilter);
	if(SUCCEEDED(hr))
	{
		hr = pMediaFilter->SetSyncSource(NULL);
		pMediaFilter->Release();
	}

	//m_pSourceFilter = new DSOutputFilter(NULL,&hr);
	//if(FAILED(hr) || m_pSourceFilter == NULL) return hr;
	m_pSourceFilter = new DSOutputFilter();
	if(FAILED(hr) || m_pSourceFilter == NULL) return hr;
	m_pSourceFilter->AddRef();

	// Create the color space convertor filter
	hr = COCREATE(CLSID_Colour, IID_IBaseFilter, m_pColorspaceConverterFilter);
	if(FAILED(hr)) return hr;

	// Create the video renderer
	hr = COCREATE(CLSID_VideoMixingRenderer, IID_IBaseFilter, m_pVideoRendererFilter);
	if(FAILED(hr))
	{
		hr = COCREATE(CLSID_VideoRenderer, IID_IBaseFilter, m_pVideoRendererFilter);
		if(FAILED(hr)) return hr;
	}


	// Add dource filter to the graph
	hr = m_pGraphBuilder->AddFilter(m_pSourceFilter, L"DSHOW_OUTPUT");
	if(FAILED(hr)) return hr;

	// Add the color space convertor to the graph
	hr = m_pGraphBuilder->AddFilter(m_pColorspaceConverterFilter, L"COLORSPACE_CONVERTOR");
	if(FAILED(hr)) return hr;

	// Add video renderer to the graph
	hr = m_pGraphBuilder->AddFilter(m_pVideoRendererFilter, L"VIDEO_RENDERER");
	if(FAILED(hr)) return hr;


	// Find media control
	hr = QUERY(m_pGraphBuilder, IID_IMediaControl, m_pMediaController);
	if(FAILED(hr)) return hr;

	// Find IVideoWindow interface
	hr = QUERY(m_pGraphBuilder, IID_IVideoWindow, m_pVideoWindow);
	if(FAILED(hr)) return  hr;

	return hr;
}
