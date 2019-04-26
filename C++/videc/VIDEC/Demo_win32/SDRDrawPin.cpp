// SDRDrawPin.cpp: implementation of the SDRDrawPin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <initguid.h>
#include "SDRDrawPin.h"
#include "SDRDrawFilter.h"
//#include "VIDEC_convert.h"
#include "XUtil.h"
#include "../VIDEC_csp/VIDEC_CSP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SDRDrawPin::SDRDrawPin(char *pObjName,SDRDrawFilter *pFilter,HRESULT *phr,LPCWSTR pPinName)
:CBaseOutputPin(pObjName, pFilter, pFilter, phr, pPinName)
,m_pSDRDrawFilter(pFilter)
,m_nWidth(320)
,m_nHeight(240)
{
	m_nBitCount=4;
	m_nStride=m_nWidth*m_nBitCount;
	m_nFlip=0;
}

SDRDrawPin::~SDRDrawPin()
{
}

//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT SDRDrawPin::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
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

//
// CheckMediaType
//
HRESULT SDRDrawPin::CheckMediaType(const CMediaType *pMediaType)
{
	if (*(pMediaType->Subtype()) != MEDIASUBTYPE_RGB24)
	{
		return S_FALSE;
	}

	return S_OK;
} // CheckMediaType

#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)
// (internal function) change colorspace 
HRESULT SDRDrawPin::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
	//DWORD biWidth;

	if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
		//biWidth = vih->bmiHeader.biWidth;
		m_nStride = CALC_BI_STRIDE(vih->bmiHeader.biWidth, vih->bmiHeader.biBitCount);
		m_nBitCount=vih->bmiHeader.biBitCount;
		m_nFlip=((vih->bmiHeader.biHeight>0)?1:0);
		//
		//char szDebug[1024];
		//sprintf(szDebug,"0 nWidth=%d,nBitCount=%d nStride=%d\n",vih->bmiHeader.biWidth,vih->bmiHeader.biBitCount,m_nStride);
		//OutputDebugString(szDebug);
	}
	else
	{
		return S_FALSE;
	}

	if (subtype != MEDIASUBTYPE_RGB24)
	{
		return S_FALSE;
	}

	return S_OK;
}

// set the connection media type
HRESULT SDRDrawPin::SetMediaType(const CMediaType *pMediaType)
{
	return ChangeColorspace(*pMediaType->Subtype(), *pMediaType->FormatType(), pMediaType->Format());
}

//
// GetMediaType
//
// I support one type, namely the type of the input pin
// We must be connected to support the single output type
//
HRESULT SDRDrawPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    // Have we run off the end of types
    if (iPosition > 0) {
        return VFW_S_NO_MORE_ITEMS;
    }

	BITMAPINFOHEADER * bmih;
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) pMediaType->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
		if (vih == NULL) return E_OUTOFMEMORY;

		ZeroMemory(vih, sizeof (VIDEOINFOHEADER));
		bmih = &(vih->bmiHeader);
		pMediaType->SetFormatType(&FORMAT_VideoInfo);
		vih->rcSource.left=0;
		vih->rcSource.right=m_nWidth;
		vih->rcSource.top=0;
		vih->rcSource.bottom=m_nHeight;
		vih->rcTarget=vih->rcSource;
		
		//vih->AvgTimePerFrame=166666;
	}

	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth  = m_nWidth;
	bmih->biHeight = m_nHeight;
	bmih->biPlanes = 1;

	bmih->biCompression = MEDIASUBTYPE_RGB24.Data1;
	bmih->biBitCount = 24;
	pMediaType->SetSubtype(&MEDIASUBTYPE_RGB24);

	bmih->biSizeImage = GetBitmapSize(bmih);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->SetSampleSize(bmih->biSizeImage);

	return S_OK;
} // GetMediaType

void SDRDrawPin::SetImageFormat(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
}

bool SDRDrawPin::DisplayFrame(unsigned char*pData,int nLen)
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

	unsigned char*src[3];
	int src_stride[3];
	src[0]=pData;
	src[1]=src[0]+m_nWidth*m_nHeight;
	src[2]=src[1]+((m_nWidth*m_nHeight)>>2);
	src_stride[0]=m_nWidth;
	src_stride[1]=m_nWidth>>1;
	src_stride[2]=m_nWidth>>1;

	int nRet=-1;
	nRet=YV12_TO_RGB24(src,pBufferOut,m_nWidth,m_nHeight,m_nStride,m_nFlip);
	if (nRet!=0)
	{
		pSample->Release();
		return false;
	}

	LONGLONG llTimeStart=10000;
	LONGLONG llTimeEnd=llTimeStart+1;
	
	pSample->SetTime(&llTimeStart,&llTimeEnd);

	Deliver(pSample);

	pSample->Release();

	//char szDebug[1024]="";
	//sprintf(szDebug,"SDRDrawPin::DisplayFrame %u\n",XGetTimestamp());
	//OutputDebugString(szDebug);

	return true;
}

