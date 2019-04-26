// DShowPicture.cpp: implementation of the DShowPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DShowPicture.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"

VIDEC_Picture*VIDEC_Picture::Create(void)
{
	NEWRECORD3( DShowPicture*,pDShowPicture,new DShowPicture() )
	//NULLReturnRet( pDShowPicture , pDShowPicture )  
	return pDShowPicture;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DShowPicture::DShowPicture(void)
{
	m_pGraphBuilder = NULL ;	
	m_pMediaControl = NULL;
	m_pVideoWindow = NULL;
	m_bIsWindowVisible=false;
	m_nWidth=0;
	m_nHeight=0;
}

DShowPicture::~DShowPicture(void)
{
}

int DShowPicture::Open(HWND hWnd,const char*cszPathName)
{
	HRESULT hr ;

	hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&m_pGraphBuilder );
	if ( hr != S_OK )		return -1;

	USES_CONVERSION;
	hr = m_pGraphBuilder->RenderFile(A2W(cszPathName),NULL);
	if ( hr != S_OK )		return -1;

	hr = m_pGraphBuilder->QueryInterface( IID_IMediaControl, (void **)&m_pMediaControl ) ;
	if ( hr != S_OK ) 
		return -1;

	hr = m_pGraphBuilder->QueryInterface(IID_IVideoWindow,(void **)&m_pVideoWindow);
	if(FAILED(hr)) 
		return -1;

	m_pVideoWindow -> put_Owner((OAHWND)hWnd);
	m_pVideoWindow -> put_MessageDrain((OAHWND)hWnd);
	m_pVideoWindow -> put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
	m_pVideoWindow -> put_Visible(OAFALSE);

	
	CComPtr<IBasicVideo>	pibv=NULL;
	LONG lWidth;
	hr=m_pVideoWindow->get_Width(&lWidth);
	if(FAILED(hr)) 
	{
		return -1;
	}
	else
	{
		hr=m_pGraphBuilder->QueryInterface(IID_IBasicVideo, (void **)&pibv);
		if(FAILED(hr))
		{
			return -1;
		}
	}

	long lVideoWidth=0;
	long lVideoHeight=0;
	pibv->get_VideoWidth(&lVideoWidth);
	pibv->get_VideoHeight(&lVideoHeight);
	m_nWidth=lVideoWidth;
	m_nHeight=lVideoHeight;

	m_pMediaControl->Run();
	m_pVideoWindow->put_Visible(OAFALSE);

	return 0;
}

void DShowPicture::Close(void)
{
	if( m_pMediaControl )
	{
		m_pMediaControl->Stop();
	}

	if(m_pVideoWindow)
	{
		m_pVideoWindow->put_Visible(OAFALSE);
	}

	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pVideoWindow = NULL;
}

void DShowPicture::ShowWindow(bool bShow)
{
	if (m_pMediaControl && m_pVideoWindow)
	{
		m_bIsWindowVisible=bShow;
		if (bShow)
		{
			m_pVideoWindow->put_Visible(OATRUE);
		}
		else
		{
			m_pVideoWindow->put_Visible(OAFALSE);
		}
	}
}

bool DShowPicture::IsWindowVisible(void)
{
	return m_bIsWindowVisible;
}

void DShowPicture::SetWindowPosition(int x,int y,int cx,int cy)
{
	if (m_pVideoWindow)
	{
		m_pVideoWindow->SetWindowPosition(x,y,cx,cy);
	}
}

int DShowPicture::GetWidth(void)
{
	return m_nWidth;
}

int DShowPicture::GetHeight(void)
{
	return m_nHeight;
}

