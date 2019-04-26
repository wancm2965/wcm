#ifndef __XCAMERARENDER_H__
#define __XCAMERARENDER_H__

#include "XAutoLock.h"
#include "VIDEC.h"

class XCameraCap;
class XCameraRender 
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
	XCameraRender(XCameraCap&rCameraCap);
	~XCameraRender(void);
public:
	HRESULT CheckMediaType( GUID subType);
	HRESULT DoRenderSample( IMediaSample *pMediaSample );

	void SetFrameRate(unsigned short usFrameRate,unsigned short usTotalFrameRate);
protected:
	XCameraCap&			m_rCameraCap;

	PBYTE				m_pMainBuffer;
	PBYTE				m_pRGBBuffer;

	int					m_nWidth;
	int					m_nHeight;

	int					m_nXDSCSPType;

	int					m_nCaptureImageSize;

	unsigned long		m_nSkipCount;
	unsigned long		m_nSkipIndex;
};

#endif