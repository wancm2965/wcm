#ifndef __XCAMERARENDER_H__
#define __XCAMERARENDER_H__

#include "XAutoLock.h"
#include "VIDEC.h"

class XCameraCap;
class XCameraRender 
	: public CBaseRenderer
{
	enum{
		XDS_CSP_YUY2=0,
		XDS_CSP_YVYU,
		XDS_CSP_UYVY,
		XDS_CSP_IYUV,
		XDS_CSP_YV12,
		XDS_CSP_RGB24,
		XDS_CSP_RGB32
	};
public:
	DECLARE_IUNKNOWN;
	XCameraRender(XCameraCap&rCameraCap, LPUNKNOWN punk, HRESULT *phr);
	~XCameraRender(void);
	    
public:
	HRESULT CheckMediaType( const CMediaType *pmtIn );
	HRESULT DoRenderSample( IMediaSample *pMediaSample );
	HRESULT SetMediaType( const CMediaType *pmt );
	HRESULT Active();
	HRESULT Inactive();

	void SetFrameRate(unsigned short usFrameRate,unsigned short nTotalFrameRate);
protected:
	XCameraCap&			m_rCameraCap;

	PBYTE				m_pMainBuffer;
	PBYTE				m_pRGBBuffer;

	int					m_nWidth;
	int					m_nActualHeight;
	int					m_nVirtualHeight;

	int					m_nXDSCSPType;

	int					m_nCaptureImageSize;

	unsigned long		m_nSkipCount;
	unsigned long		m_nSkipIndex;

	int					m_nFrameRate;

	int					m_nTotalFrameRate;
	int					m_nFrameCount;
	unsigned long		m_nLastFrameCountTimestamp;
	int					m_nActualFrameRate;
	int					m_nAdjustFrameRateCount;
};

#endif