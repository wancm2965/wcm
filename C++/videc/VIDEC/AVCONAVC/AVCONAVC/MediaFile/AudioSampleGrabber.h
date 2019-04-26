#pragma once

#include "PCMConvert.h"

class AudioSampleGrabberCallback
{
public:
	virtual void OnAudioSampleGrabberCallback(unsigned char*pData,int nLen,int nSampleRate,double dblSampleTime)=0;
};

// define the filter class
class AudioSampleGrabber 
	: public CTransInPlaceFilter
{
private:
	AudioSampleGrabberCallback&	m_rCallback;
	PCMConvert*					m_pPCMConvert;
	PBYTE						m_pPCMBuffer;
	int							m_nPCMBufLen;
	int							m_nChannel;

	WORD						m_wFormatTag;
	PBYTE						m_pSrcPCMBuffer;
	int							m_nSrcPCMBufLen;
public:
	// instantiation
	AudioSampleGrabber( IUnknown * pOuter, HRESULT * phr, BOOL ModifiesData, AudioSampleGrabberCallback&rCallback );
	~AudioSampleGrabber();
	static CUnknown *WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr, AudioSampleGrabberCallback&rCallback);

	// IUnknown
	DECLARE_IUNKNOWN;

	// CTransInPlaceFilter
	HRESULT CheckInputType(const CMediaType *pmt);
	HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *pmt);
	HRESULT Transform(IMediaSample *pMediaSample);
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut) 
	{
		return NOERROR; 
	}
};

