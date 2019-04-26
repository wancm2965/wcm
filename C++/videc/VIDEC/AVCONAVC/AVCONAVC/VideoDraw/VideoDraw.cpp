#include "stdafx.h"
#include <d3d9.h>
#include <dxva2api.h>

#include <streams.h>
#include <atlbase.h>
#include <evr.h>
#include <uuids.h>
#include "moreuuids.h"

#include <math.h>
#include <mmsystem.h>
#include "VideoDraw.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"

#define SAFE_RELEASE(x) {if(x) x -> Release(); x = NULL;}

extern HRESULT InitializeEVR(IBaseFilter *pEVR, HWND hwnd, IMFVideoDisplayControl ** ppWc); 
extern HRESULT InitWindowlessVMR(IBaseFilter *pVMR, HWND hwnd, IVMRWindowlessControl** ppWc); 
extern CComPtr<IPin> GetPin(IBaseFilter*pFilter,LPCOLESTR pinname);

AVCONAVC_VideoDraw*AVCONAVC_VideoDraw::Create(void)
{
	//return new VideoDraw();
	NEWRECORDret3(AVCONAVC_VideoDraw*, p, new VideoDraw()  ) 
}

VideoDraw::VideoDraw(void)
{
	m_hWndVideo=NULL;
	m_nWidth=320;
	m_nHeight=240;
	m_bNV12=false;

	m_pVideoDisplay=NULL;
	m_pWindowless=NULL;
	m_pVideoDrawFilter=NULL;
}

VideoDraw::~VideoDraw(void)
{

}


int VideoDraw::Open(HWND hWndVideo,int nWidth,int nHeight,bool bNV12,bool& bTryEVR)
{
	HRESULT hr=S_OK;

	m_hWndVideo=hWndVideo;
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_bNV12=bNV12;

	hr=m_pGraph.CoCreateInstance(CLSID_FilterGraph);
	if (hr!=S_OK)
	{
		return -1;
	}

	CComPtr<ICaptureGraphBuilder2> pBuilder;
	hr=pBuilder.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	if (hr!=S_OK)
	{
		return -1;
	}
	hr=pBuilder->SetFiltergraph(m_pGraph);
	if (hr!=S_OK)
	{
		return -1;
	}

	NEWRECORD( m_pVideoDrawFilter , new VideoDrawFilter(m_nWidth, m_nHeight, m_bNV12) )
	m_pVideoDrawFilter->AddRef();
	hr=m_pGraph->AddFilter(m_pVideoDrawFilter,L"Video Draw Filter Source");
	if (hr!=S_OK)
	{
		return -1;
	}

	while (1)
	{
		if (bTryEVR)
		{
			CComPtr<IBaseFilter> pVideoRender;

			hr=pVideoRender.CoCreateInstance(CLSID_EnhancedVideoRenderer);
			if (hr==S_OK)
			{
				hr = InitializeEVR(pVideoRender, m_hWndVideo, &m_pVideoDisplay);
				if (hr==S_OK)
				{
					hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (EVR)");
					if (hr!=S_OK)
					{
						bTryEVR=false;
						return -1;
					}

					hr=m_pGraph->ConnectDirect(m_pVideoDrawFilter->GetPin(0),GetPin(pVideoRender,L"EVR Input0"),NULL);
					if (hr!=S_OK)
					{
						bTryEVR=false;
						return -1;
					}
					break;
				}
			}
			bTryEVR=false;
		}

		{
			CComPtr<IBaseFilter> pVideoRender;

			hr=pVideoRender.CoCreateInstance(CLSID_VideoMixingRenderer);
			if (hr==S_OK)
			{
				hr = InitWindowlessVMR(pVideoRender, m_hWndVideo, &m_pWindowless);
				if (hr==S_OK)
				{
					hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (VMR)");
					if (hr!=S_OK)
					{
						return -1;
					}

					hr=m_pGraph->ConnectDirect(m_pVideoDrawFilter->GetPin(0),GetPin(pVideoRender,L"VMR Input0"),NULL);
					if (hr!=S_OK)
					{
						return -1;
					}
					break;
				}
			}
		}

		{
			CComPtr<IBaseFilter> pVideoRender;

			hr=pVideoRender.CoCreateInstance(CLSID_VideoRenderer);
			if (hr!=S_OK)
			{
				return -1;
			}

			hr=m_pGraph->AddFilter(pVideoRender,L"Video Renerer (SDR)");
			if (hr!=S_OK)
			{
				return -1;
			}

			hr=m_pGraph->ConnectDirect(m_pVideoDrawFilter->GetPin(0),GetPin(pVideoRender,L"Input"),NULL);
			if (hr!=S_OK)
			{
				return -1;
			}
		}
		break;
	}

	RECT rc;
	::GetClientRect(m_hWndVideo,&rc);
	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->SetVideoPosition(NULL, &rc);
	}
	else if (m_pWindowless)
	{
		m_pWindowless->SetVideoPosition(NULL, &rc);
		m_pWindowless->DisplayModeChanged();
	}
	else
	{
		CComQIPtr<IVideoWindow,&IID_IVideoWindow> pVideoWindow(m_pGraph);
		if (pVideoWindow)
		{
			pVideoWindow -> put_Owner((OAHWND)m_hWndVideo);
			pVideoWindow -> put_MessageDrain((OAHWND)m_hWndVideo);
			pVideoWindow -> put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			pVideoWindow -> put_Visible(OATRUE);

			pVideoWindow -> SetWindowPosition(0, 0, rc.right-rc.left, rc.bottom-rc.top);
		}
	}

	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	if (pMediaControl!=NULL)
	{
		hr=pMediaControl -> Run ();
	}

	return 0;
}

void VideoDraw::Close(void)
{
	CComQIPtr<IMediaControl,&IID_IMediaControl> pMediaControl(m_pGraph);
	if (pMediaControl)
	{
		pMediaControl->Stop();
	}

	SAFE_RELEASE(m_pWindowless);
	SAFE_RELEASE(m_pVideoDisplay);
	SAFE_RELEASE(m_pVideoDrawFilter);
}

void VideoDraw::Repaint(void)
{
	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->RepaintVideo();
	}
	else if (m_pWindowless)
	{
		HDC hDC=::GetDC(m_hWndVideo);
		m_pWindowless->RepaintVideo(m_hWndVideo, hDC);
		::ReleaseDC(m_hWndVideo,hDC);
	}
}

void VideoDraw::SetWndPos(int x,int y,int cx,int cy)
{
	RECT rc;
	rc.left=x;
	rc.right=x+cx;
	rc.top=y;
	rc.bottom=y+cy;

	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->SetVideoPosition(NULL, &rc);
	}
	else if (m_pWindowless)
	{
		m_pWindowless->SetVideoPosition(NULL, &rc);
		m_pWindowless->DisplayModeChanged();
	}
	else
	{
		CComQIPtr<IVideoWindow,&IID_IVideoWindow> pVideoWindow(m_pGraph);
		if (pVideoWindow)
		{
			pVideoWindow -> SetWindowPosition(0, 0,rc.right-rc.left, rc.bottom-rc.top);
		}
	}
}

int VideoDraw::GetWidth(void)
{
	return m_nWidth;
}

int VideoDraw::GetHeight(void)
{
	return m_nHeight;
}

bool VideoDraw::GetNV12(void)
{
	return m_bNV12;
}

int VideoDraw::Draw(unsigned char*pYUV,int nStride)
{
	if (m_pVideoDrawFilter)
	{
		return m_pVideoDrawFilter->Draw(pYUV,nStride);
	}
	return -1;
}
