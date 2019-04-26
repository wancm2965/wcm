#ifndef __XCAMERARENDEREX_H__
#define __XCAMERARENDEREX_H__

#include "XAutoLock.h"
#include "VIDEC.h"

class XCameraCapEx;
class XCameraRenderEx 
	: public CBaseRenderer
{
	enum{
		XDS_CSP_YUY2=0,
		XDS_CSP_YVYU,
		XDS_CSP_UYVY,
		XDS_CSP_IYUV,
		XDS_CSP_YV12,
		XDS_CSP_RGB24,
		XDS_CSP_RGB32,
		XDS_CSP_MJPG
	};
public:
	DECLARE_IUNKNOWN;
	XCameraRenderEx(XCameraCapEx&rCameraCap, LPUNKNOWN punk, HRESULT *phr);
	~XCameraRenderEx(void);
	    
public:
	HRESULT CheckMediaType( const CMediaType *pmtIn );
	HRESULT DoRenderSample( IMediaSample *pMediaSample );
	HRESULT SetMediaType( const CMediaType *pmt );
	HRESULT Active();
	HRESULT Inactive();

	void SetFrameRate(unsigned short usFrameRate,unsigned short nTotalFrameRate);
	void SetDstWidth(int nWidth);
	void SetDstHeight(int nHeight);
protected:
	XCameraCapEx&			m_rCameraCap;

	PBYTE				m_pMainBuffer;
	PBYTE				m_pRGBBuffer;

	int					m_nWidth;
	int					m_nHeight;

	int					m_nVirtualWidth;
	int					m_nVirtualHeight;

	int					m_nDstWidth;
	int					m_nDstHeight;

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

	void*				m_hMJPGDecoder;
	char *              m_pOffsetBuf;    //yuv offset move process buf
	int                 m_nHeightBack;
	int                 m_nCaptureImageSizeBack;
	int                 m_nOffsetLine;

};

#endif