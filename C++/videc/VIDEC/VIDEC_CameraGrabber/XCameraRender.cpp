#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <initguid.h>
#include <qedit.h>
#include "XCameraRender.h"
#include "XCameraCap.h"
#include "XUtil.h"
#include "VIDEC_csp.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_Convert.h"


// 30323449-0000-0010-8000-00AA00389B71  'I420' ==  MEDIASUBTYPE_I420 
// 30323449-0000-0010-8000-00AA00389B71
DEFINE_GUID(MEDIASUBTYPE_I420_NR, 
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 


//56555949-0000-0010-8000-00AA00389B71	'IYUV' == MEDIASUBTYPE_IYUV


XCameraRender::XCameraRender(XCameraCap&rCameraCap)
: m_rCameraCap(rCameraCap)
, m_nSkipCount(0)
, m_nSkipIndex(0)
{
	m_nWidth=m_rCameraCap.GetWidth();
	m_nHeight=m_rCameraCap.GetHeight();

	m_nCaptureImageSize=(m_nWidth*m_nHeight*3)>>1;

	m_pMainBuffer=(PBYTE)malloc(m_nCaptureImageSize+m_nCaptureImageSize/4);
	m_nXDSCSPType=0;

	m_pRGBBuffer=NULL;
}

XCameraRender::~XCameraRender(void)
{
	if (m_pRGBBuffer)
	{
		free(m_pRGBBuffer);
		m_pRGBBuffer=NULL;
	}

	if (m_pMainBuffer)
	{
		free(m_pMainBuffer);
		m_pMainBuffer=NULL;
	}
}

// make sure media type is what we want
//
HRESULT XCameraRender::CheckMediaType( GUID subType)
{
	if( subType != MEDIASUBTYPE_YV12)
	{
		if( subType != MEDIASUBTYPE_YUY2 )
		{
			if( subType != MEDIASUBTYPE_YVYU )
			{
				if( subType != MEDIASUBTYPE_UYVY )
				{
					if( subType != MEDIASUBTYPE_IYUV && subType != MEDIASUBTYPE_I420_NR)
					{
						if( subType != MEDIASUBTYPE_RGB24 )
						{
							if( subType != MEDIASUBTYPE_RGB32 )
							{
								return E_INVALIDARG;
							}
							else
							{
								m_nXDSCSPType=XDS_CSP_RGB32;
								m_pRGBBuffer=(PBYTE)malloc(m_nWidth*m_nHeight*5);
							}
						}
						else
						{
							m_nXDSCSPType=XDS_CSP_RGB24;
							m_pRGBBuffer=(PBYTE)malloc(m_nWidth*m_nHeight*4);
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

	return NOERROR; 
}

// have to ovverride this
//
HRESULT XCameraRender::DoRenderSample( IMediaSample *pMediaSample )
{
	if ((m_nSkipIndex++)%m_nSkipCount)
	{
		return NOERROR;
	}

	if (m_pMainBuffer==NULL)
	{
		m_pMainBuffer=(PBYTE)malloc(m_nCaptureImageSize+m_nCaptureImageSize/4);
		if (m_pMainBuffer==NULL)
		{
			return NOERROR;
		}
	}
	unsigned long ulTimestamp=XGetTimestamp();

	int nSrcImageSize=m_nWidth*m_nHeight;

	CheckPointer(pMediaSample,E_POINTER);
	LPBYTE pBufferIn=NULL;
	pMediaSample->GetPointer( &pBufferIn );
	int nImageSize=m_nWidth*m_nHeight;

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
			avconConvert.height=m_nHeight;
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
			avconConvert.height=m_nHeight;
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
			avconConvert.height=m_nHeight;
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
			pData[1] = pData[0] + m_nWidth*m_nHeight;
			pData[2] = pData[1] + ((m_nWidth*m_nHeight)>>2);
			avconConvert.output.plane[1] = pData[1];
			avconConvert.output.plane[2] = pData[2];

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nHeight;
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
			avconConvert.height=m_nHeight;
			avconConvert.interlacing=0;
			if (avcon_convert(&avconConvert)<0)
			{
				return NOERROR;
			}
			break;
		}
	case XDS_CSP_RGB24:
		{
			memcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
			unsigned char*pYV12[3];
			pYV12[0]=m_pMainBuffer;
			pYV12[1]=pYV12[0]+m_nWidth*m_nHeight;
			pYV12[2]=pYV12[1]+(m_nWidth*m_nHeight>>2);
			RGB24_TO_YV12(m_pRGBBuffer,pYV12,m_nWidth,m_nHeight);
			break;
		}
	case XDS_CSP_RGB32:
		{
			avcon_convert_t avconConvert;
			memset(&avconConvert,0,sizeof(avcon_convert_t));

			avconConvert.input.csp=XVID_CSP_BGRA | XVID_CSP_VFLIP;
			memcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
			avconConvert.input.plane[0]=m_pRGBBuffer;
			avconConvert.input.stride[0]=m_nWidth*4;

			avconConvert.output.csp=XVID_CSP_YV12;
			avconConvert.output.plane[0]=m_pMainBuffer;
			avconConvert.output.stride[0]=m_nWidth;
			avconConvert.output.stride[1]=m_nWidth/2;
			avconConvert.output.stride[2]=m_nWidth/2;

			avconConvert.width=m_nWidth;
			avconConvert.height=m_nHeight;
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

	XImage*pXImage=new XImage(pImageData,nImageSize);
	pXImage->AddRef();
	if (pImageData==m_pMainBuffer)
	{
		m_pMainBuffer=NULL;
	}

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(m_nHeight);
	pXImage->SetVirtualWidth(m_nWidth);
	pXImage->SetVirtualHeight(m_nHeight);
	pXImage->SetDoubleField(bDoubleField);
	pXImage->SetEvenField(bDoubleField);
	pXImage->SetOddField(bDoubleField);
	pXImage->SetTimestamp(ulTimestamp);

	//TRACE("XCameraRender ulTimestamp=%u\n",ulTimestamp);

	m_rCameraCap.OnCaptureVideoFrame(pXImage);

	pXImage->Release();
	pXImage=NULL;

	return NOERROR; 
}


void XCameraRender::SetFrameRate(unsigned short usFrameRate,unsigned short usTotalFrameRate)
{
	if (usFrameRate<=1)
	{
		m_nSkipCount=usTotalFrameRate;
	}
	else if (usFrameRate<=5)
	{
		m_nSkipCount=(usTotalFrameRate==25?5:6);
	}
	else if (usFrameRate<=10)
	{
		m_nSkipCount=3;
	}
	else if (usFrameRate<=15)
	{
		m_nSkipCount=2;
	}
	else
	{
		m_nSkipCount=1;
	}
}