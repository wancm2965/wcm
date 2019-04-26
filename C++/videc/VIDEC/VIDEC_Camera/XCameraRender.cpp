#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <initguid.h>
#include "XCameraRender.h"
#include "XCameraCap.h"
#include "XUtil.h"
#include "VIDEC_csp.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_Convert.h"
#include "../VIDEC_csp/fastmemcpy.h"


// {40ECE765-659B-4f75-BB7D-5XVID9630924CB}
DEFINE_GUID(CLSID_CameraRender, 
0x40ece765, 0x659b, 0x4f75, 0xbb, 0x7d, 0x5f, 0xf9, 0x63, 0x9, 0x24, 0xcb);
// 30323449-0000-0010-8000-00AA00389B71  'I420' ==  MEDIASUBTYPE_I420 
// 30323449-0000-0010-8000-00AA00389B71
DEFINE_GUID(MEDIASUBTYPE_I420_NR, 
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 


//56555949-0000-0010-8000-00AA00389B71	'IYUV' == MEDIASUBTYPE_IYUV


XCameraRender::XCameraRender(XCameraCap&rCameraCap, LPUNKNOWN punk, HRESULT *phr)
: CBaseRenderer (CLSID_CameraRender, NAME("XCameraRender"), punk, phr)
, m_rCameraCap(rCameraCap)
, m_nSkipCount(0)
, m_nSkipIndex(0)
{
	m_nWidth=m_rCameraCap.GetWidth();
	m_nVirtualHeight=m_rCameraCap.GetHeight();
	m_nActualHeight=m_nVirtualHeight;

	m_nActualHeight-=(m_nActualHeight%16);

	m_nCaptureImageSize=(m_nWidth*m_nActualHeight*3)>>1;

	m_pMainBuffer=(PBYTE)MALLOCMSG(m_nCaptureImageSize+m_nCaptureImageSize/4);
	m_nXDSCSPType=0;

	m_pRGBBuffer=NULL;

	m_nFrameRate=30;

	m_nTotalFrameRate=30;
	m_nFrameCount=0;
	m_nLastFrameCountTimestamp=0;
	m_nActualFrameRate=30;

	m_nAdjustFrameRateCount=0;
}

XCameraRender::~XCameraRender(void)
{
	if (m_pRGBBuffer)
	{
		FREEMSG(m_pRGBBuffer);
		m_pRGBBuffer=NULL;
	}

	if (m_pMainBuffer)
	{
		FREEMSG(m_pMainBuffer);
		m_pMainBuffer=NULL;
	}
}

// make sure media type is what we want
//
HRESULT XCameraRender::CheckMediaType( const CMediaType *pmtIn )
{
	CheckPointer(pmtIn,E_POINTER);

	if (*pmtIn->FormatType() != FORMAT_VideoInfo)
	{
		return E_INVALIDARG;
	}

	if( *pmtIn->Type( ) != MEDIATYPE_Video )
	{
		return E_INVALIDARG;
	}

	if( *pmtIn->Subtype( ) != MEDIASUBTYPE_YV12)
	{
		if( *pmtIn->Subtype( ) != MEDIASUBTYPE_YUY2 )
		{
			if( *pmtIn->Subtype( ) != MEDIASUBTYPE_YVYU )
			{
				if( *pmtIn->Subtype( ) != MEDIASUBTYPE_UYVY )
				{
					if( *pmtIn->Subtype( ) != MEDIASUBTYPE_IYUV && *pmtIn->Subtype( ) != MEDIASUBTYPE_I420_NR)
					{
						if( *pmtIn->Subtype( ) != MEDIASUBTYPE_RGB24 )
						{
							if( *pmtIn->Subtype( ) != MEDIASUBTYPE_RGB32 )
							{
								return E_INVALIDARG;
							}
							else
							{
								m_nXDSCSPType=XDS_CSP_RGB32;
								m_pRGBBuffer=(PBYTE)MALLOCMSG(m_nWidth*m_nActualHeight*5);
							}
						}
						else
						{
							m_nXDSCSPType=XDS_CSP_RGB24;
							m_pRGBBuffer=(PBYTE)MALLOCMSG(m_nWidth*m_nActualHeight*4);
						}
					}
					else
					{
						m_nXDSCSPType=XDS_CSP_IYUV;
					}
				}
				else
				{
					m_nXDSCSPType=XDS_CSP_UYVY;
				}
			}
			else
			{
				m_nXDSCSPType=XDS_CSP_YVYU;
			}
		}
		else
		{
			m_nXDSCSPType=XDS_CSP_YUY2;
		}
	}
	else
	{
		m_nXDSCSPType=XDS_CSP_YV12;
	}


	VIDEOINFO *pVI = (VIDEOINFO *) pmtIn->Format();
	CheckPointer(pVI,E_UNEXPECTED);

	return NOERROR; 
}

// have to ovverride this
//
HRESULT XCameraRender::DoRenderSample( IMediaSample *pMediaSample )
{
	unsigned long ulCurTimestamp=XGetTimestamp();
	if (m_nLastFrameCountTimestamp==0)
	{
		m_nLastFrameCountTimestamp=ulCurTimestamp;
	}
	m_nFrameCount++;
	unsigned long ulDuration=ulCurTimestamp-m_nLastFrameCountTimestamp;
	if (ulDuration>=4000 && m_nAdjustFrameRateCount<2)
	{
		m_nTotalFrameRate=m_nFrameCount*1000/ulDuration;
		m_nFrameCount=0;
		m_nLastFrameCountTimestamp=ulCurTimestamp;

		SetFrameRate(m_nFrameRate,m_nTotalFrameRate);

		m_nAdjustFrameRateCount++;
	}

	if ((m_nSkipIndex++)%m_nSkipCount)
	{
		return NOERROR;
	}

	if (m_pMainBuffer==NULL)
	{
		m_pMainBuffer=(PBYTE)MALLOCMSG(m_nCaptureImageSize+m_nCaptureImageSize/4);
		if (m_pMainBuffer==NULL)
		{
			return NOERROR;
		}
	}
	unsigned long ulTimestamp=XGetTimestamp();

	int nSrcImageSize=m_nWidth*m_nActualHeight;

	CheckPointer(pMediaSample,E_POINTER);
	LPBYTE pBufferIn=NULL;
	pMediaSample->GetPointer( &pBufferIn );
	int nImageSize=m_nWidth*m_nActualHeight;

	switch (m_nXDSCSPType)
	{
	case XDS_CSP_YUY2:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_YUY2;
			avconConvert.input.plane[0]=pBufferIn;
			avconConvert.input.stride[0]=m_nWidth*2;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}
			break;
		}
	case XDS_CSP_YVYU:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_YVYU;
			avconConvert.input.plane[0]=pBufferIn;
			avconConvert.input.stride[0]=m_nWidth*2;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}		
			break;
		}
	case XDS_CSP_UYVY:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_UYVY;
			avconConvert.input.plane[0]=pBufferIn;
			avconConvert.input.stride[0]=m_nWidth*2;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}		
			break;
		}
	case XDS_CSP_YV12:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_I420;
			avconConvert.input.plane[0]=pBufferIn;
			avconConvert.input.stride[0]=m_nWidth;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;
			avconConvert.output.stride[1]=m_nWidth>>1;
			avconConvert.output.stride[2]=m_nWidth>>1;
			PBYTE pData[3];
			pData[0]=(PBYTE)m_pMainBuffer;
			pData[1] = pData[0] + m_nWidth*m_nActualHeight;
			pData[2] = pData[1] + ((m_nWidth*m_nActualHeight)>>2);
			avconConvert.output.plane[1] = pData[1];
			avconConvert.output.plane[2] = pData[2];

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=false;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}
			break;
		}
	case XDS_CSP_IYUV:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_I420;
			avconConvert.input.plane[0]=pBufferIn;
			avconConvert.input.stride[0]=m_nWidth;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;
			avconConvert.output.stride[1]=m_nWidth>>1;
			avconConvert.output.stride[2]=m_nWidth>>1;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}
			break;
		}
	case XDS_CSP_RGB24:
		{
			fastmemcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
			unsigned char*pYV12[3];
			pYV12[0]=m_pMainBuffer;
			pYV12[1]=pYV12[0]+m_nWidth*m_nActualHeight;
			pYV12[2]=pYV12[1]+(m_nWidth*m_nActualHeight>>2);
			RGB24_TO_YV12(m_pRGBBuffer,pYV12,m_nWidth,m_nActualHeight);
			break;
		}
	case XDS_CSP_RGB32:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_BGRA | XVID_CSP_VFLIP;
			fastmemcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
			avconConvert.input.plane[0]=m_pRGBBuffer;
			avconConvert.input.stride[0]=m_nWidth*4;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;
			avconConvert.output.stride[1]=m_nWidth/2;
			avconConvert.output.stride[2]=m_nWidth/2;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nActualHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}
			break;
		}
	default:
		return NOERROR;
	}

	unsigned char*pImageData=m_pMainBuffer;
	nImageSize=m_nCaptureImageSize;
	bool bDoubleField=false;
	int nWidth=m_nWidth;

	NEWRECORD3( XImage*,pXImage,new XImage(pImageData,nImageSize))
	NULLReturnRet( pXImage , -1 ) 
	pXImage->AddRef();
	if (pImageData==m_pMainBuffer)
	{
		m_pMainBuffer=NULL;
	}

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(m_nActualHeight);
	pXImage->SetVirtualWidth(m_nWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetDoubleField(bDoubleField);
	pXImage->SetEvenField(bDoubleField);
	pXImage->SetOddField(bDoubleField);
	pXImage->SetTimestamp(ulTimestamp);
	pXImage->SetFrameRate(m_nActualFrameRate);

	//TRACE("XCameraRender ulTimestamp=%u\n",ulTimestamp);

	m_rCameraCap.OnCaptureVideoFrame(pXImage);

	pXImage->Release();
	pXImage=NULL;

	return NOERROR; 
}

// have to override this
//
HRESULT XCameraRender::SetMediaType( const CMediaType *pmt )
{
	return NOERROR; 
}

// override these to receive indication of when we change
// to Pause/Play (Active) or Stop (Inactive) state.
HRESULT XCameraRender::Active()
{
	return NOERROR; 
}

HRESULT XCameraRender::Inactive()
{
	return NOERROR; 
}

void XCameraRender::SetFrameRate(unsigned short usFrameRate,unsigned short nTotalFrameRate)
{
	if (usFrameRate<=1)
	{
		m_nSkipCount=nTotalFrameRate;
		m_nFrameRate=1;
	}
	else if (usFrameRate<=5)
	{
		m_nFrameRate=5;

		if (nTotalFrameRate>=20)
		{
			m_nSkipCount=5;
		}
		else if (nTotalFrameRate>=10)
		{
			m_nSkipCount=3;
		}
		else
		{
			m_nSkipCount=2;
		}
	}
	else if (usFrameRate<=10)
	{
		m_nFrameRate=10;

		if (nTotalFrameRate>=20)
		{
			m_nSkipCount=3;
		}
		else
		{
			m_nSkipCount=2;
		}
	}
	else if (usFrameRate<=15)
	{
		m_nFrameRate=15;

		if (nTotalFrameRate>=20)
		{
			m_nSkipCount=2;
		}
		else
		{
			m_nSkipCount=1;
		}
	}
	else
	{
		m_nFrameRate=30;
		m_nSkipCount=1;
	}

	if (m_nFrameRate>=nTotalFrameRate)
	{
		m_nSkipCount=1;
		if (nTotalFrameRate>25)
		{
			m_nActualFrameRate=30;
		}
		else if (nTotalFrameRate>20)
		{
			m_nActualFrameRate=25;
		}
		else if (nTotalFrameRate>15)
		{
			m_nActualFrameRate=20;
		}
		else if (nTotalFrameRate>10)
		{
			m_nActualFrameRate=15;
		}
		else if (nTotalFrameRate>5)
		{
			m_nActualFrameRate=10;
		}
		else if (nTotalFrameRate>1)
		{
			m_nActualFrameRate=5;
		}
		else 
		{
			m_nActualFrameRate=1;
		}
	}
	else 
	{
		m_nSkipCount=(int)(((double)nTotalFrameRate/(double)usFrameRate)+0.5);
		m_nActualFrameRate=m_nFrameRate;
	}
}
