#include "StdAfx.h"
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include <dvdmedia.h>
#include <initguid.h>
#include "VideoConsumerFilter.h"
#include "XUtil.h"



DEFINE_GUID(CLSID_VIDEO_CONSUMER,
			0xfd501041, 0x8ebe, 0x11ce, 0x81, 0x83, 0x00, 0xaa, 0x00, 0x57, 0x7d, 0xa1);

#pragma warning(disable:4710)  // 'function': function not inlined (optimzation)


// pad YV12 format with stride
void YV12ToYV12(BYTE *pDst, BYTE *pSrc, DWORD width, DWORD height, DWORD pitch)
{
	BYTE *pdy, *pdu, *pdv;
	BYTE *psy, *psu, *psv;

	psy = pSrc;
	psu = psy + width * height;
	psv = psu + ((width * height) >> 2);

	pdy = pDst;
	pdv = pdy + pitch * height;
	pdu = pdv + ((pitch * height) >> 2);

#ifdef USE_MMX
	for (DWORD i = 0; i < height; i++)
		memcpyMMX(pdy + i * pitch, psy + i * width, width);

	for (DWORD i = 0; i < (height >> 1); i++)
		memcpyMMX(pdu + i * (pitch >> 1), psu + i * (width >> 1), width >> 1);

	for (DWORD i = 0; i < (height >> 1); i++)
		memcpyMMX(pdv + i * (pitch >> 1), psv + i * (width >> 1), width >> 1);

#else
	for (DWORD i = 0; i < (height >> 1); i++)
	{
		CopyMemory(pdy + 2*i * pitch, psy + 2*i * width, width);
		CopyMemory(pdy + (2*i+1) * pitch, psy + (2*i+1) * width, width);
		CopyMemory(pdu + i * (pitch >> 1), psu + i * (width >> 1), width >> 1);
		CopyMemory(pdv + i * (pitch >> 1), psv + i * (width >> 1), width >> 1);
	}
#endif


}

//
// CreateInstance
//
// The only allowed way to create Bouncing balls!
//
CUnknown * WINAPI VideoConsumerSource::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr,VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType)
{
	ASSERT(phr);

	CUnknown *punk = new VideoConsumerSource(lpunk, phr,pVideoConsumerCallback,nWidth,nHeight,nFrameRate,nCSPType);
	if(punk == NULL)
	{
		if(phr)
			*phr = E_OUTOFMEMORY;
	}
	return punk;

} // CreateInstance


//
// Constructor
//
// Initialise a VideoConsumerStream object so that we have a pin.
//
VideoConsumerSource::VideoConsumerSource(LPUNKNOWN lpunk, HRESULT *phr,VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType) :
CSource(NAME("VideoConsumerSource"), lpunk, CLSID_VIDEO_CONSUMER)
{
	ASSERT(phr);
	CAutoLock cAutoLock(&m_cStateLock);


	m_paStreams = (CSourceStream **) new VideoConsumerStream*[1];
	if(m_paStreams == NULL)
	{
		if(phr)
			*phr = E_OUTOFMEMORY;

		return;
	}

	m_paStreams[0] = new VideoConsumerStream(phr, this, L"VideoConsumerStream",pVideoConsumerCallback,nWidth,nHeight,nFrameRate,nCSPType);
	if(m_paStreams[0] == NULL)
	{
		if(phr)
			*phr = E_OUTOFMEMORY;

		return;
	}


} // (Deconstructor)

VideoConsumerSource::~VideoConsumerSource()    
{
	if(m_paStreams[0])
		delete m_paStreams[0];
	if(m_paStreams)
		delete m_paStreams;
} // (Deconstructor)

STDMETHODIMP VideoConsumerSource::Stop(void)
{
	if (m_paStreams[0])
	{
		m_paStreams[0]->Stop();
	}

	return CSource::Stop();
}

//
// Constructor
//
VideoConsumerStream::VideoConsumerStream(HRESULT *phr,
						 VideoConsumerSource *pParent,
						 LPCWSTR pPinName,
						 VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType) :
CSourceStream(NAME("SourceStream"),phr, pParent, pPinName),
m_nWidth(nWidth),
m_nHeight(nHeight),
m_pVideoConsumerCallback(pVideoConsumerCallback),
m_nFrameRate(nFrameRate),
m_nCSPType(nCSPType)
{
	ASSERT(phr);
	CAutoLock cAutoLock(&m_cSharedState);

	m_rtSampleTime = 0;
	m_rtRepeatTime = 10000000/nFrameRate;
	m_rtMode=10000000%nFrameRate;
	m_rtModeTotal=0;

	m_nBitCount=4;
	m_nStride=m_nWidth*m_nBitCount;
	m_nFlip=0;

} // (Constructor)


//
// Destructor
//
VideoConsumerStream::~VideoConsumerStream()
{
} // (Destructor)



//
// FillBuffer
//
// Plots a ball into the supplied video buffer
//
HRESULT VideoConsumerStream::FillBuffer(IMediaSample *pSample)
{
	CheckPointer(pSample,E_POINTER);

	BYTE *pData;
	long lDataLen;

	pSample->GetPointer(&pData);
	lDataLen = pSample->GetSize();

	{
		CAutoLock cAutoLockShared(&m_cSharedState);

		int nStride=m_nWidth;
		int nBitCount=0;
		if (m_nCSPType==VC_CSP_RGB)
		{
			nStride=m_nStride;
			nBitCount=m_nBitCount;
		}
		else
		{
			VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *)m_mt.Format();
			nStride=pvi->bmiHeader.biWidth;
		}
		
		if (m_pVideoConsumerCallback)
		{
			int nFrameRate=m_nFrameRate;
			m_pVideoConsumerCallback->OnVideoConsumerCallbackPull(pData,m_nWidth,m_nHeight,nStride,nFrameRate,nBitCount,m_nFlip);
			if (nFrameRate!=m_nFrameRate)
			{
				m_nFrameRate=nFrameRate;
				m_rtRepeatTime = 10000000/nFrameRate;
				m_rtMode=10000000%nFrameRate;
				m_rtModeTotal=0;

				//char szDebug[1024];
				//sprintf(szDebug,"nFrameRate=%d\n",nFrameRate);
				//OutputDebugString(szDebug);
			}
		}


		// The current time is the sample's start
		REFERENCE_TIME rtStart = m_rtSampleTime;
		// Increment to find the finish time
		m_rtSampleTime += m_rtRepeatTime;

		m_rtModeTotal+=m_rtMode;

		if (m_rtModeTotal>=m_nFrameRate)
		{
			m_rtSampleTime+=1;
			m_rtModeTotal-=m_nFrameRate;
		}		
		pSample->SetTime((REFERENCE_TIME *) &rtStart,(REFERENCE_TIME *) &m_rtSampleTime);
	}

	//pSample->SetSyncPoint(TRUE);
	return NOERROR;

} // FillBuffer

//
// OnThreadCreate
//
// As we go active reset the stream time to zero
//
HRESULT VideoConsumerStream::OnThreadCreate()
{
	CAutoLock cAutoLockShared(&m_cSharedState);
	m_rtSampleTime = 0;

	return NOERROR;

} // OnThreadCreate


//
// Notify
//
// Alter the repeat rate according to quality management messages sent from
// the downstream filter (often the renderer).  Wind it up or down according
// to the flooding level - also skip forward if we are notified of Late-ness
//
STDMETHODIMP VideoConsumerStream::Notify(IBaseFilter * pSender, Quality q)
{
	return NOERROR;
} // Notify


//
// GetMediaType
//
//
HRESULT VideoConsumerStream::GetMediaType(int iPosition,CMediaType *pmt)
{
	CheckPointer(pmt,E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	if(iPosition < 0)
	{
		return E_INVALIDARG;
	}

	// Have we run off the end of types?
	if(iPosition > (m_nCSPType==VC_CSP_RGB?2:0))
	{
		return VFW_S_NO_MORE_ITEMS;
	}


	pmt->InitMediaType();
	pmt->SetType(&MEDIATYPE_Video);

	if (m_nCSPType==VC_CSP_RGB)
	{
		VIDEOINFOHEADER * pvi = (VIDEOINFOHEADER *) pmt->ReallocFormatBuffer(sizeof(VIDEOINFOHEADER));
		ZeroMemory(pvi, sizeof (VIDEOINFOHEADER));
		pmt->SetFormatType(&FORMAT_VideoInfo);

		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biBitCount = 12;
		pvi->bmiHeader.biWidth = m_nWidth;
		pvi->bmiHeader.biHeight = m_nHeight;
		pvi->bmiHeader.biSizeImage = DIBSIZE(pvi->bmiHeader);
		pvi->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
		pvi->AvgTimePerFrame = 10000000 / m_nFrameRate;

		switch (iPosition)
		{
		case 0:
			pvi->bmiHeader.biCompression = MEDIASUBTYPE_RGB32.Data1;
			pvi->bmiHeader.biBitCount = 32;
			pmt->SetSubtype(&MEDIASUBTYPE_RGB32);
			break;
		case 1:
			pvi->bmiHeader.biCompression = MEDIASUBTYPE_RGB24.Data1;
			pvi->bmiHeader.biBitCount = 24;
			pmt->SetSubtype(&MEDIASUBTYPE_RGB24);
			break;
		case 2:
			pvi->bmiHeader.biCompression = MEDIASUBTYPE_RGB565.Data1;
			pvi->bmiHeader.biBitCount = 16;
			pmt->SetSubtype(&MEDIASUBTYPE_RGB565);
			break;
		}
		pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
		pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);
		SetRect(&pvi->rcSource, 0, 0, m_nWidth, m_nHeight);
		SetRect(&pvi->rcTarget, 0, 0, m_nWidth, m_nHeight);
	}
	else
	{
		VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *)pmt->AllocFormatBuffer(sizeof(VIDEOINFOHEADER2));
		ZeroMemory(pvi, sizeof(VIDEOINFOHEADER2));
		pmt->SetFormatType(&FORMAT_VideoInfo2);

		pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pvi->bmiHeader.biPlanes = 1;
		pvi->bmiHeader.biBitCount = 12;
		pvi->bmiHeader.biWidth = m_nWidth;
		pvi->bmiHeader.biHeight = m_nHeight;
		pvi->bmiHeader.biSizeImage = DIBSIZE(pvi->bmiHeader);
		pvi->AvgTimePerFrame = 10000000 / m_nFrameRate;

		pvi->dwInterlaceFlags = 0; 

		pvi->dwCopyProtectFlags = AMCOPYPROTECT_RestrictDuplication;
		pvi->dwPictAspectRatioX = (DWORD)pvi->bmiHeader.biWidth;
		pvi->dwPictAspectRatioY = (DWORD)pvi->bmiHeader.biHeight;

		if (m_nCSPType==VC_CSP_NV12)
		{
			pmt->SetSubtype(&MEDIASUBTYPE_NV12);
			pvi->bmiHeader.biCompression = MAKEFOURCC('N','V','1','2');
		}
		else
		{
			pmt->SetSubtype(&MEDIASUBTYPE_YV12);
			pvi->bmiHeader.biCompression = MAKEFOURCC('Y','V','1','2');
		}
		pmt->SetSampleSize(pvi->bmiHeader.biSizeImage);
		SetRect(&pvi->rcSource, 0, 0, m_nWidth, m_nHeight);
		SetRect(&pvi->rcTarget, 0, 0, m_nWidth, m_nHeight);
	}

	return NOERROR;

} // GetMediaType

#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

//
// CheckMediaType
//
// We will accept 8, 16, 24 or 32 bit video formats, in any
// image size that gives room to bounce.
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT VideoConsumerStream::CheckMediaType(const CMediaType *pMediaType)
{
	CheckPointer(pMediaType,E_POINTER);

	CAutoLock lock_it(m_pLock);


	if ((pMediaType->majortype != MEDIATYPE_Video) ||
		((pMediaType->formattype != FORMAT_VideoInfo) &&
		(pMediaType->formattype != FORMAT_VideoInfo2)))
	{
		return S_FALSE;
	}

	if (m_nCSPType==VC_CSP_NV12)
	{
		if (pMediaType->subtype != MEDIASUBTYPE_NV12)
		{
			return S_FALSE;
		}
	}
	else if (m_nCSPType==VC_CSP_YV12)
	{
		if (pMediaType->subtype != MEDIASUBTYPE_YV12)
		{
			return S_FALSE;
		}
	}
	else if (m_nCSPType==VC_CSP_RGB)
	{
		if (pMediaType->subtype != MEDIASUBTYPE_RGB565 &&
			pMediaType->subtype != MEDIASUBTYPE_RGB24 &&
			pMediaType->subtype != MEDIASUBTYPE_RGB32)
		{

			return S_FALSE;
		}

		if (pMediaType->formattype == FORMAT_VideoInfo)
		{
			VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )pMediaType->Format();
			m_nStride = CALC_BI_STRIDE(vih->bmiHeader.biWidth, vih->bmiHeader.biBitCount);
			m_nBitCount=vih->bmiHeader.biBitCount;
			m_nFlip=((vih->bmiHeader.biHeight>0)?1:0);
		}
	}
	else
	{
		return S_FALSE;
	}


	SetMediaType(pMediaType);
	m_mt=*pMediaType;

	return S_OK;

} // CheckMediaType


//
// DecideBufferSize
//
// This will always be called after the format has been sucessfully
// negotiated. So we have a look at m_mt to see what size image we agreed.
// Then we can ask for buffers of the correct size to contain them.
//
HRESULT VideoConsumerStream::DecideBufferSize(IMemAllocator *pAlloc,
									  ALLOCATOR_PROPERTIES *pProperties)
{
	CheckPointer(pAlloc,E_POINTER);
	CheckPointer(pProperties,E_POINTER);

	CAutoLock cAutoLock(m_pFilter->pStateLock());
	HRESULT hr = NOERROR;

	pProperties->cbBuffer = m_nWidth * m_nHeight * (m_nCSPType==VC_CSP_RGB?5:3);
	pProperties->cBuffers = 1;

	ALLOCATOR_PROPERTIES Actual;

	hr = pAlloc->SetProperties(pProperties,&Actual);
	if(FAILED(hr))
	{
		return hr;
	}
	return hr;
} // DecideBufferSize
