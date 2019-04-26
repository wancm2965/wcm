//DecoderFilter.cpp

#include <winsock2.h>
#include <windows.h>

#include <streams.h>
#include <initguid.h>
#include <olectl.h>
 
#include "DecoderFilter.h"
#include "../VIDEC_Convert/VIDEC_convert.h"
#include "VIDEC_Header.h"
#include "VIDEC.h"


const AMOVIESETUP_MEDIATYPE sudInputPinTypes[] =
{
    { &MEDIATYPE_Video, &CLSID_VIDEC },
};

const AMOVIESETUP_MEDIATYPE sudOutputPinTypes[] =
{
    { &MEDIATYPE_Video, &MEDIASUBTYPE_YUY2 }
};
 
const AMOVIESETUP_PIN psudPins[] =
{
	{
		L"Input",           // String pin name
		FALSE,              // Is it rendered
		FALSE,              // Is it an output
		FALSE,              // Allowed none
		FALSE,              // Allowed many
		&CLSID_NULL,        // Connects to filter
		L"Output",          // Connects to pin
		sizeof(sudInputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE), // Number of types
		&sudInputPinTypes[0]	// The pin details
	},
	{ 
		L"Output",          // String pin name
		FALSE,              // Is it rendered
		TRUE,               // Is it an output
		FALSE,              // Allowed none
		FALSE,              // Allowed many
		&CLSID_NULL,        // Connects to filter
		L"Input",           // Connects to pin
		sizeof(sudOutputPinTypes) / sizeof(AMOVIESETUP_MEDIATYPE),	// Number of types
		sudOutputPinTypes	// The pin details
	}
};


const AMOVIESETUP_FILTER sudDecoderFilter =
{
	&CLSID_VIDEC,			// Filter CLSID
	VIDEC_NAME_L,			// Filter name
	MERIT_PREFERRED,		// Its merit
	sizeof(psudPins) / sizeof(AMOVIESETUP_PIN),	// Number of pins
	psudPins				// Pin details
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance

CFactoryTemplate g_Templates[] =
{
	{ 
		VIDEC_NAME_L,
		&CLSID_VIDEC,
		DecoderFilter::CreateInstance,
		NULL,
		&sudDecoderFilter
	}
};


/* note: g_cTemplates must be global; used by strmbase.lib(dllentry.cpp,dllsetup.cpp) */
int g_cTemplates = sizeof(g_Templates) / sizeof(CFactoryTemplate);


STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );
}


STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );
}

/* strmbase.lib\dllentry.obj:DllEntryPoint@12 */
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);


extern "C" BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved);


BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			VIDEC_Init(0,0);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			VIDEC_Terminate();
			break;
    }

	/* Call directshow DllEntryPoint@12 */
    return DllEntryPoint(hInst, fdwReason, lpvReserved);
}

/* create instance */

CUnknown * WINAPI DecoderFilter::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    DecoderFilter * pNewObject = new DecoderFilter(punk, phr);
    if (pNewObject == NULL)
	{
        *phr = E_OUTOFMEMORY;
    }
    return pNewObject;
}


/* query interfaces */

STDMETHODIMP DecoderFilter::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	if (riid == IID_IDecoderFilter)
	{
		return GetInterface((IDecoderFilter *) this, ppv);
	} 

	return CVideoTransformFilter::NonDelegatingQueryInterface(riid, ppv);
}

DecoderFilter::DecoderFilter(LPUNKNOWN punk, HRESULT *phr) 
	: CVideoTransformFilter(NAME("DecoderFilter"), punk, CLSID_VIDEC)
{
	m_pDecoder=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_nStride=0;
	m_nImageSize=0;
}

/* destructor */
DecoderFilter::~DecoderFilter()
{
	if (m_pDecoder!=NULL)
	{
		m_pDecoder->Close();
		delete m_pDecoder;
		m_pDecoder=NULL;
	}
}



/* check input type */
HRESULT DecoderFilter::CheckInputType(const CMediaType * mtIn)
{
	BITMAPINFOHEADER * hdr;

	if (*mtIn->Type() != MEDIATYPE_Video)
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if (*mtIn->FormatType() == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER *) mtIn->Format();
		hdr = &vih->bmiHeader;
	}
	else
	{
		return VFW_E_TYPE_NOT_ACCEPTED;
	}

	if (hdr->biHeight < 0)
	{
		//DPRINTF("colorspace: inverted input format not supported");
	}

	m_nWidth = hdr->biWidth;
	m_nHeight = hdr->biHeight;
	m_nImageSize=m_nWidth*m_nHeight*3/2;

	switch(hdr->biCompression)
	{
	case FOURCC_VIDEC :
		break;
	default :
		return VFW_E_TYPE_NOT_ACCEPTED;
	}
	return S_OK;
}

#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

/* get list of supported output colorspaces */
HRESULT DecoderFilter::GetMediaType(int iPosition, CMediaType *pMediaType)
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
	}

	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth  = m_nWidth;
	bmih->biHeight = m_nHeight;
	bmih->biPlanes = 1;
	bmih->biCompression = MEDIASUBTYPE_YUY2.Data1;
	bmih->biBitCount = 16;
	pMediaType->SetSubtype(&MEDIASUBTYPE_YUY2);

	bmih->biSizeImage = GetBitmapSize(bmih);

	pMediaType->SetType(&MEDIATYPE_Video);
	pMediaType->SetTemporalCompression(FALSE);
	pMediaType->SetSampleSize(bmih->biSizeImage);

	m_nStride = CALC_BI_STRIDE(bmih->biWidth, bmih->biBitCount);

	return S_OK;
}


/* (internal function) change colorspace */
#define CALC_BI_STRIDE(width,bitcount)  ((((width * bitcount) + 31) & ~31) >> 3)

HRESULT DecoderFilter::ChangeColorspace(GUID subtype, GUID formattype, void * format)
{
	DWORD biWidth;

	if (formattype == FORMAT_VideoInfo)
	{
		VIDEOINFOHEADER * vih = (VIDEOINFOHEADER * )format;
		biWidth = vih->bmiHeader.biWidth;
		m_nStride = CALC_BI_STRIDE(vih->bmiHeader.biWidth, vih->bmiHeader.biBitCount);
	}
	else
	{
		return S_FALSE;
	}

	if (subtype != MEDIASUBTYPE_YUY2)
	{
		return S_FALSE;
	}

	return S_OK;
}


/* set output colorspace */

HRESULT DecoderFilter::SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt)
{
	if (direction == PINDIR_OUTPUT)
	{
		return ChangeColorspace(*pmt->Subtype(), *pmt->FormatType(), pmt->Format());
	}
	
	return S_OK;
}


/* check input<->output compatiblity */

HRESULT DecoderFilter::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
	return S_OK;
}


/* alloc output buffer */

HRESULT DecoderFilter::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	HRESULT result;
	ALLOCATOR_PROPERTIES ppropActual;

	pProperties->cBuffers = 1;
	pProperties->cbBuffer = m_nWidth * m_nHeight * 4;
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
}


/* decode frame */

HRESULT DecoderFilter::Transform(IMediaSample *pIn, IMediaSample *pOut)
{
	if (m_pDecoder==NULL)
	{
		m_pDecoder=VIDEC_StreamDecoder::Create();
		if (m_pDecoder==NULL)
		{
			return S_FALSE;
		}
		if (0!=m_pDecoder->Open())
		{
			m_pDecoder->Close();
			delete m_pDecoder;
			m_pDecoder=NULL;
			return S_FALSE;
		}
	}

	AM_MEDIA_TYPE * mtOut;
	pOut->GetMediaType(&mtOut);
	if (mtOut != NULL)
	{
		HRESULT result;

		result = ChangeColorspace(mtOut->subtype, mtOut->formattype, mtOut->pbFormat);
		DeleteMediaType(mtOut);

		if (result != S_OK)
		{
			return result;
		}
	}

	PBYTE pPacketData=NULL;
	int nPacketLen=0;
	void* pYUVData=NULL;
	PBYTE pOutBuf=NULL;
	int nWidth=0;
	int nHeight=0;

	nPacketLen = pIn->GetActualDataLength();
	if (pIn->GetPointer((BYTE**)&pPacketData) != S_OK)
	{
		return S_FALSE;
	}

	if (pOut->GetPointer((BYTE**)&pOutBuf) != S_OK)
	{
		return S_FALSE; 
	}

	if (0!=m_pDecoder->Decode(pPacketData,nPacketLen,&pYUVData,nWidth,nHeight))
	{
		return S_FALSE;
	}

	if (m_nWidth!=nWidth || m_nHeight!=nHeight)
	{
		return S_FALSE;
	}

	if (pIn->IsPreroll() != S_OK)
	{
		unsigned char*src[3];
		int src_stride[3];
		src[0]=(PBYTE)pYUVData;
		src[1]=src[0]+m_nWidth*m_nHeight;
		src[2]=src[1]+((m_nWidth*m_nHeight)>>2);
		src_stride[0]=m_nWidth;
		src_stride[1]=m_nWidth>>1;
		src_stride[2]=m_nWidth>>1;

		unsigned char*dst[3];
		int dst_stride[3];
		dst[0]=pOutBuf;
		dst_stride[0]=m_nStride;
		dst_stride[1]=m_nStride>>1;
		dst_stride[2]=m_nStride>>1;

		yuv_to_yuy2(src,src_stride,dst,dst_stride,m_nWidth,m_nHeight);
	}

	if (VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData))
	{
		pOut->SetSyncPoint(TRUE);
	}

	if (pIn->IsPreroll() == S_OK) 
	{
		return S_FALSE;
	}

	return S_OK;
}
