// VideoDrawPin.cpp: implementation of the VideoDrawPin class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <initguid.h>
#include "VideoDrawPin.h"
#include "VideoDrawFilter.h"
#include <emmintrin.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// pad YV12 format with stride
void YV12ToYV12(BYTE *pDst, BYTE *pSrc, DWORD nWidth, DWORD nHeight, DWORD nDstStride, DWORD nSrcStride,bool bSwapUV)
{
	BYTE *pDstY, *pDstU, *pDstV;
	BYTE *pSrcY, *pSrcU, *pSrcV;

	pSrcY = pSrc;
	pSrcU = pSrcY + nSrcStride * nHeight;
	pSrcV = pSrcU + ((nSrcStride * nHeight) >> 2);

	pDstY = pDst;
	pDstU = pDstY + nDstStride * nHeight;
	pDstV = pDstU + ((nDstStride * nHeight) >> 2);

	if (bSwapUV)
	{
		BYTE*pTemp=pDstU;
		pDstU=pDstV;
		pDstV=pTemp;
	}


	for (DWORD i = 0; i < (nHeight >> 1); i++)
	{
		CopyMemory(pDstY + 2*i * nDstStride, pSrcY + 2*i * nSrcStride, nWidth);
		CopyMemory(pDstY + (2*i+1) * nDstStride, pSrcY + (2*i+1) * nSrcStride, nWidth);
		CopyMemory(pDstU + i * (nDstStride >> 1), pSrcU + i * (nSrcStride >> 1), nWidth >> 1);
		CopyMemory(pDstV + i * (nDstStride >> 1), pSrcV + i * (nSrcStride >> 1), nWidth >> 1);
	}
}


// Load 16 8-bit pixels into a register, using aligned memory access
// reg   - register to store pixels in
// src   - memory pointer of the source
#define PIXCONV_LOAD_PIXEL8_ALIGNED(reg,src) \
	reg = _mm_load_si128((const __m128i *)(src));      /* load (aligned) */


// SSE2 Aligned memcpy
// dst - memory destination
// src - memory source
// len - size in bytes
#define PIXCONV_MEMCPY_ALIGNED(dst,src,len)     \
{                                             \
	__m128i reg;                                \
	__m128i *dst128 =  (__m128i *)(dst);        \
	for (int i = 0; i < len; i+=16) {           \
	PIXCONV_LOAD_PIXEL8_ALIGNED(reg,(src)+i); \
	_mm_stream_si128(dst128++, reg);          \
	}                                           \
}

void convert_nv12_nv12(unsigned char*pSrcY,unsigned char*pSrcUV,int nSrcStride,unsigned char*pDstY,unsigned char*pDstUV,int nDstStride,int nWidth,int nHeight)
{
	const int chromaHeight = (nHeight >> 1);

	int line;

	_mm_sfence();

	// Use SSE2 copy when the stride is aligned
	if ((nDstStride % 16) == 0) {
		// Copy the data
		for (line = 0; line < nHeight; line++) {
			PIXCONV_MEMCPY_ALIGNED(pDstY + nDstStride * line, pSrcY, nWidth);
			pSrcY += nSrcStride;
		}

		for (line = 0; line < chromaHeight; line++) {
			PIXCONV_MEMCPY_ALIGNED(pDstUV + nDstStride * line, pSrcUV, nWidth);
			pSrcUV += nSrcStride;
		}
	} else {
		// Copy the data
		for (line = 0; line < nHeight; line++) {
			memcpy(pDstY + nDstStride * line, pSrcY, nWidth);
			pSrcY += nSrcStride;
		}

		for (line = 0; line < chromaHeight; line++) {
			memcpy(pDstUV + nDstStride * line, pSrcUV, nWidth);
			pSrcUV += nSrcStride;
		}
	}
}

void convert_yv12_yv12(unsigned char*pSrcY,unsigned char*pSrcU,unsigned char*pSrcV,int nSrcStride,unsigned char*pDstY,unsigned char*pDstU,unsigned char*pDstV,int nDstStride,int nWidth,int nHeight)
{
	const int chromaHeight = (nHeight >> 1);
	const int chromaWidth = (nWidth >> 1);
	const int chromaSrcStride = (nSrcStride >> 1);
	const int chromaDstStride = (nDstStride >> 1);

	int line;

	_mm_sfence();

	// Use SSE2 copy when the stride is aligned
	if ((nDstStride % 16) == 0) {
		// Copy the data
		for (line = 0; line < nHeight; line++) {
			PIXCONV_MEMCPY_ALIGNED(pDstY + nDstStride * line, pSrcY, nWidth);
			pSrcY += nSrcStride;
		}

		for (line = 0; line < chromaHeight; line++) {
			PIXCONV_MEMCPY_ALIGNED(pDstU + chromaDstStride * line, pSrcU, chromaWidth);
			pSrcU += chromaSrcStride;
			PIXCONV_MEMCPY_ALIGNED(pDstV + chromaDstStride * line, pSrcV, chromaWidth);
			pSrcV += chromaSrcStride;
		}
	} else {
		// Copy the data
		for (line = 0; line < nHeight; line++) {
			memcpy(pDstY + nDstStride * line, pSrcY, nWidth);
			pSrcY += nSrcStride;
		}

		for (line = 0; line < chromaHeight; line++) {
			memcpy(pDstU + chromaDstStride * line, pSrcU, chromaWidth);
			pSrcU += chromaSrcStride;
			memcpy(pDstV + chromaDstStride * line, pSrcV, chromaWidth);
			pSrcV += chromaSrcStride;
		}
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VideoDrawPin::VideoDrawPin(int nWidth,int nHeight,LPCSTR pObjName,VideoDrawFilter *pFilter,HRESULT *phr,LPCWSTR pPinName,bool bNV12)
:CBaseOutputPin(pObjName, pFilter, pFilter, phr, pPinName)
,m_pVideoDrawFilter(pFilter)
,m_nWidth(nWidth)
,m_nHeight(nHeight)
,m_bNV12(bNV12)
{
	m_nStride=m_nWidth;
	m_bSwapUV=true;
}

VideoDrawPin::~VideoDrawPin()
{
}

//
// DecideBufferSize
//
// This has to be present to override the PURE virtual class base function
//
HRESULT VideoDrawPin::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT result;
	ALLOCATOR_PROPERTIES ppropActual;

	pProperties->cBuffers = 1;
	pProperties->cbBuffer = m_nWidth * m_nHeight * 2;
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
HRESULT VideoDrawPin::CheckMediaType(const CMediaType *pMediaType)
{
	return S_OK;
} // CheckMediaType

// (internal function) change colorspace 
HRESULT VideoDrawPin::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
	if (formattype != FORMAT_VideoInfo)
	{
		return S_FALSE;
	}

	VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;

	if (m_bNV12)
	{
		if (subtype != MEDIASUBTYPE_NV12)
		{
			return S_FALSE;
		}
		m_nStride = vih->bmiHeader.biWidth;
	}
	else
	{
		if (subtype != MEDIASUBTYPE_YV12)
		{
			return S_FALSE;
		}
		m_nStride = vih->bmiHeader.biWidth;
	}

	return S_OK;
}

// set the connection media type
HRESULT VideoDrawPin::SetMediaType(const CMediaType *pMediaType)
{
	return ChangeColorspace(*pMediaType->Subtype(), *pMediaType->FormatType(), pMediaType->Format());
}

//
// GetMediaType
//
// I support one type, namely the type of the input pin
// We must be connected to support the single output type
//
HRESULT VideoDrawPin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    if (iPosition < 0) {
        return E_INVALIDARG;
    }

    // Have we run off the end of types
    if (iPosition > 1) {
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

		vih->AvgTimePerFrame=33333;
	}

	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth  = m_nWidth;
	bmih->biHeight = m_nHeight;
	bmih->biPlanes = 1;

	if (m_bNV12)
	{
		bmih->biCompression = MEDIASUBTYPE_NV12.Data1;
		bmih->biBitCount = 12;
		pMediaType->SetSubtype(&MEDIASUBTYPE_NV12);
	}
	else
	{
		bmih->biCompression = MEDIASUBTYPE_YV12.Data1;
		bmih->biBitCount = 12;
		pMediaType->SetSubtype(&MEDIASUBTYPE_YV12);
	}

	bmih->biSizeImage = GetBitmapSize(bmih);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->SetSampleSize(bmih->biSizeImage);

	return S_OK;
} // GetMediaType


extern void convert_nv12_nv12(unsigned char*pSrcY,unsigned char*pSrcUV,int nSrcStride,unsigned char*pDstY,unsigned char*pDstUV,int nDstStride,int nWidth,int nHeight);

int VideoDrawPin::Draw(unsigned char*pYUV,int nStride)
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

	else
	{
		if (m_bNV12)
		{
			if (nStride==m_nStride)
			{
				memcpy(pBufferOut,pYUV,m_nHeight*m_nStride*3/2);
			}
			else
			{
				unsigned char*pImageY=(unsigned char*)pYUV;
				unsigned char*pImageUV=pImageY+nStride*m_nHeight;
				convert_nv12_nv12(pImageY,pImageUV,nStride,pBufferOut,pBufferOut+m_nHeight*m_nStride,m_nStride,m_nWidth,m_nHeight);
			}
		}
		else
		{
			YV12ToYV12(pBufferOut,pYUV,m_nWidth,m_nHeight,m_nStride,nStride,m_bSwapUV);
		}	
	}



	REFERENCE_TIME nTimeStart=10000*timeGetTime();
	REFERENCE_TIME nTimeEnd=nTimeStart+150000;

	//pSample->SetDiscontinuity(TRUE);
	//pSample->SetSyncPoint(TRUE);
	pSample->SetMediaTime(&nTimeStart,&nTimeEnd);
	Deliver(pSample);

	pSample->Release();

	return true;
}

