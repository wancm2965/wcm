// DSOutputPin.cpp: implementation of the DSOutputPin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <initguid.h>
#include "DSOutputPin.h"
#include "DSOutputFilter.h"
#include "VIDEC_CSP.h"
#include "DSUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DSOutputPin::DSOutputPin(char *pObjName,DSOutputFilter *pFilter,HRESULT *phr,LPCWSTR pPinName)
:CBaseOutputPin(pObjName, pFilter, pFilter, phr, pPinName)
,m_pSDRDrawFilter(pFilter)
,m_nWidth(0)
,m_nHeight(0)
{
	m_nBitCount=4;
	m_nStride=m_nWidth*m_nBitCount;
	m_nFlip=0;

	m_nFrameRate = 30;
	m_nFrameNumber = 0;
	m_nFrameLength = (10000000)/m_nFrameRate;

	m_pPaintBuffer = NULL;
	m_hPaintDC = NULL;
	m_hDibSection = NULL;
	m_hObject = NULL;
	m_hFont = NULL;
	m_hOldFont = NULL;

	m_nOverlayHeight=0;
	m_nOverlayImageSize=0;
	m_nImageSize = 0;
	m_nOverlayImageOffset = 0;

	m_szTitle[0]='\0';
	m_nCorlor=RGB(0,0,255);
}

DSOutputPin::~DSOutputPin()
{
	if (m_hPaintDC) 
	{
		if (m_hOldFont)
		{
			SelectObject(m_hPaintDC, m_hOldFont);
		}


		DeleteDC(m_hPaintDC); 
		m_hPaintDC=NULL;
	}

	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont=NULL;
	}

	if (m_hObject)
	{
		DeleteObject(m_hObject);
		m_hObject=NULL;
	}

	m_pPaintBuffer=NULL;
}

//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT DSOutputPin::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT result;
	ALLOCATOR_PROPERTIES ppropActual;

	pProperties->cBuffers = 1;
	pProperties->cbBuffer = m_nWidth * m_nHeight * 5;
	pProperties->cbPrefix = 0;
		
	result = pAlloc->SetProperties(pProperties, &ppropActual);
	if (result != S_OK)
	{
		return result;
	}

	if (ppropActual.cbBuffer < pProperties->cbBuffer)
	{
		return E_FAIL;
	}

	return S_OK;
} // DecideBufferSize

STDMETHODIMP DSOutputPin::Notify(IBaseFilter * pSender, Quality q)
{
	return S_OK;
}
//
// CheckMediaType
//
HRESULT DSOutputPin::CheckMediaType(const CMediaType *pMediaType)
{
	if (*pMediaType->Subtype()!=MEDIASUBTYPE_RGB24)
	{
		return S_FALSE;
	}
	return S_OK;
} // CheckMediaType

#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)
// (internal function) change colorspace 
HRESULT DSOutputPin::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
	if (subtype != MEDIASUBTYPE_RGB24)
	{
		return S_FALSE;
	}

	if (formattype != FORMAT_VideoInfo)
	{
		return S_FALSE;
	}

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
	m_nStride = CALC_BI_STRIDE(vih->bmiHeader.biWidth, vih->bmiHeader.biBitCount);
	m_nBitCount=vih->bmiHeader.biBitCount;
	m_nFlip=((vih->bmiHeader.biHeight>0)?1:0);

	return S_OK;
}

// set the connection media type
HRESULT DSOutputPin::SetMediaType(const CMediaType *pMediaType)
{
	return ChangeColorspace(*pMediaType->Subtype(), *pMediaType->FormatType(), pMediaType->Format());
}

//
// GetMediaType
//
// I support one type, namely the type of the input pin
// We must be connected to support the single output type
//
HRESULT DSOutputPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    if (iPosition < 0) 
	{
        return E_INVALIDARG;
    }

    // Have we run off the end of types
    if (iPosition > 0)
	{
        return VFW_S_NO_MORE_ITEMS;
    }

	VIDEOINFOHEADER * pHeader = (VIDEOINFOHEADER *) pMediaType->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
	if (pHeader == NULL) return E_OUTOFMEMORY;

	ZeroMemory(pHeader, sizeof (VIDEOINFOHEADER));
	pMediaType->SetFormatType(&FORMAT_VideoInfo);
	pHeader->rcSource.left=0;
	pHeader->rcSource.right=m_nWidth;
	pHeader->rcSource.top=0;
	pHeader->rcSource.bottom=m_nHeight;
	pHeader->rcTarget=pHeader->rcSource;
	
	//pHeader->AvgTimePerFrame=m_nFrameLength;
	BITMAPINFOHEADER*pbmiHeader = &(pHeader->bmiHeader);

	pbmiHeader->biSize = sizeof(BITMAPINFOHEADER);
	pbmiHeader->biWidth  = m_nWidth;
	pbmiHeader->biHeight = m_nHeight;
	pbmiHeader->biPlanes = 1;

	pbmiHeader->biCompression = MEDIASUBTYPE_RGB24.Data1;
	pbmiHeader->biBitCount = 24;
	pMediaType->SetSubtype(&MEDIASUBTYPE_RGB24);

	pbmiHeader->biSizeImage = GetBitmapSize(pbmiHeader);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->SetSampleSize(pbmiHeader->biSizeImage);

	memset(&m_bmpInfo,0,sizeof(BITMAPINFO));
	m_bmpInfo.bmiHeader = pHeader->bmiHeader;

	if (m_hDibSection==NULL)
	{
		LOGFONTA logFont;
		ZeroMemory(&logFont, sizeof(logFont));	
		logFont.lfWeight = FW_THIN;
		strcpy(logFont.lfFaceName, _T("Tahoma"));
		logFont.lfHeight = (LONG) (-20.00*((double)m_nWidth/320.00));
		logFont.lfQuality = NONANTIALIASED_QUALITY;

		m_nOverlayHeight=abs(logFont.lfHeight);
		m_nOverlayHeight+=(m_nOverlayHeight%2);
		m_nOverlayHeight+=8;
		m_nOverlayImageSize=m_nWidth*m_nOverlayHeight*3;

		m_nOverlayImageOffset=m_nImageSize-m_nOverlayImageSize;

		void*pPaintBuffer=NULL;
		m_bmpInfo.bmiHeader.biCompression=BI_RGB;
		m_bmpInfo.bmiHeader.biHeight=m_nOverlayHeight;
		m_bmpInfo.bmiHeader.biSizeImage=m_bmpInfo.bmiHeader.biWidth*m_bmpInfo.bmiHeader.biHeight*3;

		m_hDibSection = CreateDIBSection(m_hPaintDC, (BITMAPINFO *) &m_bmpInfo, DIB_RGB_COLORS, &pPaintBuffer, NULL, 0);
		m_pPaintBuffer=(unsigned char*)pPaintBuffer;

		HDC hDC = GetDC(NULL);
		m_hPaintDC = CreateCompatibleDC(hDC);
		SetMapMode(m_hPaintDC, GetMapMode(hDC));
		SetBkMode(m_hPaintDC, TRANSPARENT);
		SetTextColor(m_hPaintDC, m_nCorlor);


		m_hFont = CreateFontIndirect(&logFont); 
		m_hOldFont = (HFONT) SelectObject(m_hPaintDC, m_hFont);
		m_hObject = SelectObject(m_hPaintDC, m_hDibSection);
	}


	return S_OK;
} // GetMediaType

int DSOutputPin::SetTitle(const char*cszTitle,unsigned long nColor)
{
	if (cszTitle)
	{
		int nLen=strlen(cszTitle)+1;
		if (nLen>1023)
		{
			nLen=1023;
		}
		strncpy(m_szTitle,cszTitle,nLen);
	}
	else
	{
		m_szTitle[0]='\0';
	}
	m_nCorlor=nColor;

	if (m_hPaintDC)
	{
		SetTextColor(m_hPaintDC, m_nCorlor);
	}

	return 0;
}


HRESULT DSOutputPin::GetImageFormat(int&nWidth,int&nHeight)
{
	nWidth=m_nWidth;
	nHeight=m_nHeight;
	return S_OK;
}

HRESULT DSOutputPin::SetImageFormat(int nWidth, int nHeight)
{
	if (m_nWidth==nWidth && m_nHeight==nHeight)
	{
		return S_FALSE;
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameNumber=0;
	m_nImageSize=m_nWidth*m_nHeight*3;

	if (m_hPaintDC) 
	{
		if (m_hOldFont)
		{
			SelectObject(m_hPaintDC, m_hOldFont);
		}

		DeleteDC(m_hPaintDC); 
		m_hPaintDC=NULL;
	}

	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont=NULL;
	}

	if (m_hObject)
	{
		DeleteObject(m_hObject);
		m_hObject=NULL;
	}

	m_pPaintBuffer=NULL;
	m_hDibSection=NULL;


	return S_OK;
}

bool DSOutputPin::DisplayFrame(unsigned char*pYUV420Data[3],int nStride[3])
{
	IMediaSample* pSample=NULL;

	GetDeliveryBuffer(&pSample,NULL,NULL,0);
	if (pSample==NULL)
	{
		return false;
	}

	AM_MEDIA_TYPE *pMediaType;
	pSample->GetMediaType(&pMediaType);
	if (pMediaType!=NULL)
	{
		HRESULT result=ChangeColorspace(pMediaType->subtype,pMediaType->formattype,pMediaType->pbFormat);
		DeleteMediaType(pMediaType);
		if (result!=S_OK)
		{
			pSample->Release();
			return false;
		}
	}

	PBYTE pBufferOut=NULL;
	if (pSample->GetPointer(&pBufferOut)!=S_OK) 
	{
		pSample->Release();
		return false;
	}

	int nRet=YV12_TO_RGB24(pYUV420Data,pBufferOut,m_nWidth,m_nHeight,m_nStride,m_nFlip);
	if (nRet!=0)
	{
		pSample->Release();
		return false;
	}


	if (m_szTitle && strlen(m_szTitle)>0 && m_pPaintBuffer)
	{
		CopyMemory(m_pPaintBuffer, pBufferOut+m_nOverlayImageOffset, m_nOverlayImageSize);

		if( TextOut( m_hPaintDC, 0, 0, m_szTitle, strlen(m_szTitle)))
		{
			CopyMemory(pBufferOut+m_nOverlayImageOffset, m_pPaintBuffer, m_nOverlayImageSize);
		}
	}

	//REFERENCE_TIME rtStart = m_nFrameNumber * m_nFrameLength;
	//REFERENCE_TIME rtStop  = rtStart + m_nFrameLength;

	//m_nFrameNumber++;

	//pSample->SetTime(&rtStart, &rtStop);
	//pSample->SetPreroll(FALSE);
	//pSample->SetDiscontinuity(FALSE);
	//pSample->SetSyncPoint(TRUE);

	LONGLONG llTimeStart=10000;
	LONGLONG llTimeEnd=llTimeStart+1;

	pSample->SetTime(&llTimeStart,&llTimeEnd);


	Deliver(pSample);

	pSample->Release();

	return true;
}

