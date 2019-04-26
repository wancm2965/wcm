#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <initguid.h>
#include "NullRender.h"


// {40ECE765-659B-4f75-BB7D-5XVID9630924CB}
DEFINE_GUID(CLSID_MyNullRender, 
0x40ece765, 0x659b, 0x4f75, 0xbb, 0x7d, 0x5f, 0xf9, 0x63, 0x9, 0x24, 0xcc);

NullRenderer::NullRenderer(LPUNKNOWN punk, HRESULT *phr, NullRendererCallback&rCallback)
: CBaseRenderer (CLSID_MyNullRender, NAME("MyNullRender"), punk, phr)
, m_rCallback(rCallback)
{
	m_nYLen=0;
	m_nUVLen=0;
	m_nWidth=0;
	m_nHeight=0;
	m_nFrameRate=30;
}

NullRenderer::~NullRenderer(void)
{
}

// make sure media type is what we want
//
HRESULT NullRenderer::CheckMediaType( const CMediaType *pmtIn )
{
	CheckPointer(pmtIn,E_POINTER);

	if (*pmtIn->FormatType() != FORMAT_VideoInfo)
	{
		return E_INVALIDARG;
	}

	if( *pmtIn->Type( ) != MEDIATYPE_Video )
	{
		return E_INVALIDARG;
	}


	VIDEOINFO *pVI = (VIDEOINFO *) pmtIn->Format();
	CheckPointer(pVI,E_UNEXPECTED);

	if( *pmtIn->Subtype( ) != MEDIASUBTYPE_YV12)
	{
		return E_INVALIDARG;
	}

	m_nWidth=abs(pVI->bmiHeader.biWidth);
	m_nHeight=abs(pVI->bmiHeader.biHeight);

	m_nYLen=m_nWidth*m_nHeight;
	m_nUVLen=m_nYLen/4;

	if (pVI->AvgTimePerFrame)
	{
		m_nFrameRate=10000000/pVI->AvgTimePerFrame;
	}

	return NOERROR; 
}

// have to ovverride this
//
HRESULT NullRenderer::DoRenderSample( IMediaSample *pMediaSample )
{
	REFERENCE_TIME nStart=0;
	REFERENCE_TIME nEnd=0;
	pMediaSample->GetTime(&nStart,&nEnd);

	unsigned char*pDstData=NULL;
	int nLen=pMediaSample->GetActualDataLength();
	m_rCallback.OnNullRendererYV12Buffer(&pDstData,nLen);

	unsigned char*pSrcData=NULL;
	pMediaSample->GetPointer(&pSrcData);

	unsigned char*pDstY=pDstData;
	unsigned char*pDstU=pDstY+m_nYLen;
	unsigned char*pDstV=pDstU+m_nUVLen;

	unsigned char*pSrcY=pSrcData;
	unsigned char*pSrcV=pSrcY+m_nYLen;
	unsigned char*pSrcU=pSrcV+m_nUVLen;

	memcpy(pDstY,pSrcY,m_nYLen);
	memcpy(pDstU,pSrcU,m_nUVLen);
	memcpy(pDstV,pSrcV,m_nUVLen);

	m_rCallback.OnNullRendererCallback(pDstData,nLen,m_nWidth,m_nHeight,m_nFrameRate,(double)nStart/10000000.0);

	return NOERROR; 
}

// have to override this
//
HRESULT NullRenderer::SetMediaType( const CMediaType *pmt )
{
	return NOERROR; 
}

// override these to receive indication of when we change
// to Pause/Play (Active) or Stop (Inactive) state.
HRESULT NullRenderer::Active()
{
	return NOERROR; 
}

HRESULT NullRenderer::Inactive()
{
	return NOERROR; 
}
