#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <dvdmedia.h>
#include "AudioSampleGrabber.h"


#include "growarray.h"
#include "DShowUtil.h"
#include "..\..\decoder\LAVAudio\LAVAudio.h"
#include "../../VIDEC_FRE/VIDECFREHeader.h"

#define CALLBACK_SAMPLE_RATE	32000

struct _GUID CLSID_AudioSampleGrabber;

/////////////////////// instantiation //////////////////////////
CUnknown *WINAPI 
AudioSampleGrabber::CreateInstance(LPUNKNOWN punk, HRESULT *phr, AudioSampleGrabberCallback&rCallback) {
	HRESULT hr;
	if (!phr)
		phr = &hr;
	NEWRECORD3( AudioSampleGrabber *,pNewObject , new AudioSampleGrabber(punk, phr, FALSE, rCallback) )
	if (pNewObject == NULL)
		*phr = E_OUTOFMEMORY;
	return pNewObject;
}

AudioSampleGrabber::AudioSampleGrabber(IUnknown * pOuter, HRESULT * phr, BOOL ModifiesData, AudioSampleGrabberCallback&rCallback) 
: CTransInPlaceFilter( "AudioSampleGrabber", (IUnknown*) pOuter, CLSID_AudioSampleGrabber, phr) 
, m_rCallback(rCallback)
{ 
	m_pPCMConvert=NULL;
	m_pPCMBuffer=NULL;
	m_nPCMBufLen=0;
	m_wFormatTag=WAVE_FORMAT_PCM;
	m_nChannel=1;
	m_pSrcPCMBuffer=NULL;
	m_nSrcPCMBufLen=0;
}

AudioSampleGrabber::~AudioSampleGrabber()
{ 
	if (m_pPCMConvert)
	{
		m_pPCMConvert->ReleaseConnections();
		delete m_pPCMConvert;
		m_pPCMConvert=NULL;
	}
	if (m_pPCMBuffer)
	{
		FREEMSG(m_pPCMBuffer);
		m_pPCMBuffer=NULL;
	}
	if (m_pSrcPCMBuffer)
	{
		FREEMSG(m_pSrcPCMBuffer);
		m_pSrcPCMBuffer=NULL;
	}
}


/////////////////////// CTransInPlaceFilter //////////////////////////
HRESULT 
AudioSampleGrabber::CheckInputType(const CMediaType *pmt)
{
	if (pmt->majortype != MEDIATYPE_Audio)
	{
		return S_FALSE;
	}

	if (pmt->subtype != MEDIASUBTYPE_PCM)
	{
		return S_FALSE;
	}

	if (pmt->formattype != FORMAT_WaveFormatEx)
	{
		return S_FALSE;
	}

	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pmt->Format();
	if (pwfx->wFormatTag==WAVE_FORMAT_PCM)
	{
		return S_OK;
	}

	if (pwfx->wFormatTag!=WAVE_FORMAT_EXTENSIBLE)
	{
		return S_FALSE;
	}

	if (pwfx->wBitsPerSample!=16)
	{
		return S_FALSE;
	}

	return S_OK;
}

HRESULT 
AudioSampleGrabber::SetMediaType(PIN_DIRECTION direction, const CMediaType *pMediaType)
{
	if (pMediaType->majortype != MEDIATYPE_Audio)
	{
		return S_FALSE;
	}

	if (pMediaType->subtype != MEDIASUBTYPE_PCM)
	{
		return S_FALSE;
	}

	if (pMediaType->formattype != FORMAT_WaveFormatEx)
	{
		return S_FALSE;
	}

	WAVEFORMATEX *pwfx = (WAVEFORMATEX *) pMediaType->Format();

	if (m_pPCMConvert)
	{
		m_pPCMConvert->ReleaseConnections();
		delete m_pPCMConvert;
		m_pPCMConvert=NULL;
	}
	if (m_pPCMBuffer)
	{
		FREEMSG(m_pPCMBuffer);
		m_pPCMBuffer=NULL;
	}
	if (m_pSrcPCMBuffer)
	{
		FREEMSG(m_pSrcPCMBuffer);
		m_pSrcPCMBuffer=NULL;
	}

	if (pwfx->wFormatTag==WAVE_FORMAT_PCM && 
		pwfx->nChannels==1 && 
		pwfx->wBitsPerSample==16 &&
		pwfx->nSamplesPerSec==CALLBACK_SAMPLE_RATE)
	{
		m_wFormatTag=WAVE_FORMAT_PCM;
		m_nChannel=1;
	}
	else
	{
		WAVEFORMATEX wfxSrc;
		memset(&wfxSrc,0,sizeof(WAVEFORMATEX));
		wfxSrc.wFormatTag		=	WAVE_FORMAT_PCM;
		wfxSrc.nSamplesPerSec	=	pwfx->nSamplesPerSec;
		wfxSrc.wBitsPerSample	=	pwfx->wBitsPerSample;
		wfxSrc.nChannels		=	1;
		wfxSrc.nBlockAlign		=	wfxSrc.nChannels*(wfxSrc.wBitsPerSample/8);
		wfxSrc.nAvgBytesPerSec	=	wfxSrc.nBlockAlign*wfxSrc.nSamplesPerSec;
		wfxSrc.cbSize			=	0;

		if (pwfx->wFormatTag==WAVE_FORMAT_PCM)
		{
			m_wFormatTag=WAVE_FORMAT_PCM;
			m_nChannel=1;
		}
		else if (pwfx->wFormatTag==WAVE_FORMAT_EXTENSIBLE)
		{
			m_wFormatTag=WAVE_FORMAT_EXTENSIBLE;
			m_nChannel=pwfx->nChannels;
			pwfx=&wfxSrc;

			m_nSrcPCMBufLen=wfxSrc.nAvgBytesPerSec*10;
			m_pSrcPCMBuffer=(PBYTE)MALLOCMSG(m_nSrcPCMBufLen);
			if (m_pSrcPCMBuffer==NULL)
			{
				return S_FALSE;
			}
		}

		if (pwfx->nSamplesPerSec!=CALLBACK_SAMPLE_RATE || 
			pwfx->wBitsPerSample!=16 ||
			pwfx->nChannels!=1 ||
			m_wFormatTag!=WAVE_FORMAT_PCM)
		{
			WAVEFORMATEX wfx;
			memset(&wfx,0,sizeof(WAVEFORMATEX));
			wfx.wFormatTag		=	WAVE_FORMAT_PCM;
			wfx.nSamplesPerSec	=	CALLBACK_SAMPLE_RATE;
			wfx.wBitsPerSample	=	16;
			wfx.nChannels		=	1;
			wfx.nBlockAlign		=	wfx.nChannels*(wfx.wBitsPerSample/8);
			wfx.nAvgBytesPerSec	=	wfx.nBlockAlign*wfx.nSamplesPerSec;
			wfx.cbSize			=	0;

			NEWRECORD( m_pPCMConvert , new PCMConvert() )
			if (!m_pPCMConvert->Connect(pwfx,&wfx))
			{
				m_pPCMConvert->ReleaseConnections();
				delete m_pPCMConvert;
				m_pPCMConvert=NULL;

				return S_FALSE;
			}
			else
			{
				m_nPCMBufLen=wfx.nAvgBytesPerSec*10;
				m_pPCMBuffer=(PBYTE)MALLOCMSG(m_nPCMBufLen);
				if (m_pPCMBuffer==NULL)
				{
					return S_FALSE;
				}
			}
		}

	}

	return S_OK;
}

HRESULT 
AudioSampleGrabber::Transform(IMediaSample *pMediaSample)
{
	long nSize = 0;
	BYTE *pData;

	if ( !pMediaSample )
		return E_FAIL;

	// Get pointer to the video buffer data
	if( FAILED(pMediaSample->GetPointer(&pData)) ) 
		return E_FAIL;
	nSize = pMediaSample->GetActualDataLength();

	REFERENCE_TIME nStart=0;
	REFERENCE_TIME nEnd=0;
	pMediaSample->GetTime(&nStart,&nEnd);

	//{
	//	char szDebug[1024];
	//	sprintf(szDebug,"A %.3f\n",(double)nStart/10000000.0);
	//	OutputDebugStringA(szDebug);
	//}

	if (m_wFormatTag==WAVE_FORMAT_EXTENSIBLE)
	{
		if (m_pSrcPCMBuffer)
		{
			int nSrcPCMDataLen=0;
			int nBytesCopy=2;
			int nBytesStride=2*m_nChannel;
			int nLeft=nSize;
			short*pSrc=(short*)pData;
			short*pDst=(short*)m_pSrcPCMBuffer;
			while (nLeft>=nBytesCopy)
			{
				int nSample=0;
				for (int i=0;i<m_nChannel;++i)
				{
					nSample+=pSrc[i];
				}
				if (nSample>32767)
				{
					nSample=32767;
				}
				else if (nSample<-32767)
				{
					nSample=-32767;
				}
				*pDst=(short)nSample;

				pSrc+=m_nChannel;
				nLeft-=nBytesStride;
				pDst+=1;
				nSrcPCMDataLen+=nBytesCopy;
			}

			if (m_pPCMConvert && m_pPCMBuffer)
			{
				int nLen=m_nPCMBufLen;
				if (m_pPCMConvert->Convert((char*)m_pSrcPCMBuffer,nSrcPCMDataLen,(char*)m_pPCMBuffer,nLen))
				{
					m_rCallback.OnAudioSampleGrabberCallback(m_pPCMBuffer,nLen,CALLBACK_SAMPLE_RATE,nStart/10000000.0);
				}
			}
			else
			{
				m_rCallback.OnAudioSampleGrabberCallback(m_pSrcPCMBuffer,nSrcPCMDataLen,CALLBACK_SAMPLE_RATE,nStart/10000000.0);
			}
		}
	}
	else
	{
		if (m_pPCMConvert && m_pPCMBuffer)
		{
			int nLen=m_nPCMBufLen;
			if (m_pPCMConvert->Convert((char*)pData,nSize,(char*)m_pPCMBuffer,nLen))
			{
				m_rCallback.OnAudioSampleGrabberCallback(m_pPCMBuffer,nLen,CALLBACK_SAMPLE_RATE,nStart/10000000.0);
			}
		}
		else
		{
			m_rCallback.OnAudioSampleGrabberCallback(pData,nSize,CALLBACK_SAMPLE_RATE,nStart/10000000.0);
		}
	}


	return S_OK;
}

HRESULT 
AudioSampleGrabber::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
	if (m_pInput->IsConnected() == FALSE) {
		return E_UNEXPECTED;
	}

	ASSERT(pAlloc);
	ASSERT(pProperties);
	HRESULT hr = NOERROR;

	pProperties->cBuffers = 1;
	pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
	//ASSERT(pProperties->cbBuffer);

	// Ask the allocator to reserve us some sample memory, NOTE the function
	// can succeed (that is return NOERROR) but still not have allocated the
	// memory that we requested, so we must check we got whatever we wanted
	ALLOCATOR_PROPERTIES Actual;
	hr = pAlloc->SetProperties(pProperties,&Actual);
	if (FAILED(hr)) {
		return hr;
	}

	//ASSERT( Actual.cBuffers >= 1 );

	if (pProperties->cBuffers > Actual.cBuffers ||
		pProperties->cbBuffer > Actual.cbBuffer) {
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT 
AudioSampleGrabber::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	// Is the input pin connected
	if (m_pInput->IsConnected() == FALSE) {
		return E_UNEXPECTED;
	}

	// This should never happen
	if (iPosition < 0) {
		return E_INVALIDARG;
	}

	// Do we have more items to offer
	if (iPosition > 0) {
		return VFW_S_NO_MORE_ITEMS;
	}

	*pMediaType = m_pInput->CurrentMediaType();

	return NOERROR;
}
