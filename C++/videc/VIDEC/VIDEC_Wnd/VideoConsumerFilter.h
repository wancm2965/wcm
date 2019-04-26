//VideoConsumerFilter.h
#ifndef __VIDEOCONSUMERFILTER_H__
#define __VIDEOCONSUMERFILTER_H__

#include "VideoConsumer.h"

// The class managing the output pin
class VideoConsumerStream;


//------------------------------------------------------------------------------
// Class VideoConsumerSource
//
// This is the main class for the bouncing ball filter. It inherits from
// CSource, the DirectShow base class for source filters.
//------------------------------------------------------------------------------
class VideoConsumerSource : public CSource
{
public:
    // The only allowed way to create Bouncing balls!
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr,VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType);

	~VideoConsumerSource();
public:
	STDMETHODIMP Stop(void);
private:
    // It is only allowed to to create these objects with CreateInstance
    VideoConsumerSource(LPUNKNOWN lpunk, HRESULT *phr,VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType);
}; // VideoConsumerSource


//------------------------------------------------------------------------------
// Class VideoConsumerStream
//
// This class implements the stream which is used to output the bouncing ball
// data from the source filter. It inherits from DirectShows's base
// CSourceStream class.
//------------------------------------------------------------------------------
class VideoConsumerStream : public CSourceStream  
{
public:

    VideoConsumerStream(HRESULT *phr, VideoConsumerSource *pParent, LPCWSTR pPinName,VideoConsumerCallback* pVideoConsumerCallback,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType);
    ~VideoConsumerStream();

    // plots a ball into the supplied video frame
    HRESULT FillBuffer(IMediaSample *pms);

    // Ask for buffers of the size appropriate to the agreed media type
    HRESULT DecideBufferSize(IMemAllocator *pIMemAlloc,
                             ALLOCATOR_PROPERTIES *pProperties);

    // Because we calculate the ball there is no reason why we
    // can't calculate it in any one of a set of formats...
    HRESULT CheckMediaType(const CMediaType *pMediaType);
    HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	// Resets the stream time to zero
	HRESULT OnThreadCreate(void);

    // Quality control notifications sent to us
    STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
protected:
	VideoConsumerCallback*	m_pVideoConsumerCallback;
	int				m_nWidth;
	int				m_nHeight;
	int				m_nFrameRate;

	int				m_nStride;
	int				m_nBitCount;
	int				m_nFlip;

	CMediaType		m_mt;


	REFERENCE_TIME	m_rtRepeatTime;                  // Time in msec between frames
	REFERENCE_TIME	m_rtMode;
	REFERENCE_TIME	m_rtModeTotal;
	REFERENCE_TIME	m_rtSampleTime;            // The time stamp for each sample

	CCritSec m_cSharedState;            // Lock on m_rtSampleTime and m_Ball

	
	VC_CSP_TYPE		m_nCSPType;
}; // VideoConsumerStream

#endif