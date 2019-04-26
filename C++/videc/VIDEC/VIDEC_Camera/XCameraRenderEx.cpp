#include "stdafx.h"
#include <streams.h>
#include <atlbase.h>
#include <initguid.h>
#include "XCameraRenderEx.h"
#include "XCameraCapEx.h"
#include "XUtil.h"
#include "VIDEC_csp.h"
#include "../VIDEC_API/XImage.h"
#include "VIDEC_Convert.h"
#include "../VIDEC_csp/fastmemcpy.h"
#include <AVCONAVC/AVCONAVC.h>
#include <emmintrin.h>
#include "../VIDEC_FRE/VIDECFREAPI.h"


// {40ECE765-659B-4f75-BB7D-5XVID9630924CB}
DEFINE_GUID(CLSID_CameraRender, 
0x40ece765, 0x659b, 0x4f75, 0xbb, 0x7d, 0x5f, 0xf9, 0x63, 0x9, 0x24, 0xcb);
// 30323449-0000-0010-8000-00AA00389B71  'I420' ==  MEDIASUBTYPE_I420 
// 30323449-0000-0010-8000-00AA00389B71
DEFINE_GUID(MEDIASUBTYPE_I420_NR, 
0x30323449, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71); 


//56555949-0000-0010-8000-00AA00389B71	'IYUV' == MEDIASUBTYPE_IYUV


XCameraRenderEx::XCameraRenderEx(XCameraCapEx&rCameraCap, LPUNKNOWN punk, HRESULT *phr)
: CBaseRenderer (CLSID_CameraRender, NAME("XCameraRenderEx"), punk, phr)
, m_rCameraCap(rCameraCap)
, m_nSkipCount(0)
, m_nSkipIndex(0)
{
	m_nWidth=0;
	m_nHeight=0;
	m_nDstWidth=0;
	m_nDstHeight=0;
	m_nVirtualWidth=m_nWidth;
	m_nVirtualHeight=m_nHeight;

	m_nCaptureImageSize=(m_nWidth*m_nHeight*3)>>1;

	m_pMainBuffer=NULL;//(PBYTE)MALLOCMSG(m_nCaptureImageSize+m_nCaptureImageSize/4);
	m_nXDSCSPType=0;

	m_pRGBBuffer=NULL;

	m_nFrameRate=30;

	m_nTotalFrameRate=30;
	m_nFrameCount=0;
	m_nLastFrameCountTimestamp=0;
	m_nActualFrameRate=30;

	m_nAdjustFrameRateCount=0;

	m_hMJPGDecoder=NULL;

	//yuv offset move process buf ,notice MaxOffsetWHSize 
	m_pOffsetBuf =NULL;
#define  MaxOffsetWHSize   1920*1200*2  
	NEWRECORD( m_pOffsetBuf ,new char[MaxOffsetWHSize] )
	//NULLRecord( m_pOffsetBuf ) 
	m_nHeightBack =0;
	m_nCaptureImageSizeBack =0;
	m_nOffsetLine =0;

}

XCameraRenderEx::~XCameraRenderEx(void)
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

	if (m_hMJPGDecoder)
	{
		AVCONAVC_DecoderClose(m_hMJPGDecoder);
		m_hMJPGDecoder=NULL;
	}

	//
	if(m_pOffsetBuf)
	{
		delete [] m_pOffsetBuf;
		m_pOffsetBuf =NULL;
	}

}

// make sure media type is what we want
//
HRESULT XCameraRenderEx::CheckMediaType( const CMediaType *pmtIn )
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


	VIDEOINFO *pVI = (VIDEOINFO *) pmtIn->Format();
	CheckPointer(pVI,E_UNEXPECTED);

	m_nWidth=abs(pVI->bmiHeader.biWidth);
	m_nHeight=abs(pVI->bmiHeader.biHeight);

	m_nVirtualWidth=m_nWidth;
	m_nVirtualHeight=m_nHeight;

	m_nCaptureImageSize=(m_nWidth*m_nHeight*3)>>1;

	if (*pmtIn->Subtype( ) == MEDIASUBTYPE_MJPG)
	{
		m_nXDSCSPType=XDS_CSP_MJPG;
		return NOERROR;
	}

	if (m_nHeight==360 && m_nWidth==640)
	{
		m_nHeight-=8;
	}
	else if (m_nHeight==540 && m_nWidth==960)
	{
		m_nHeight-=12;
		m_nVirtualHeight=540;
	}
	else if (m_nHeight==1080 && m_nWidth==1920)
	{
		m_nHeight-=8;
	}


	m_nCaptureImageSize=(m_nWidth*m_nHeight*3)>>1;
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
							}
						}
						else
						{
							m_nXDSCSPType=XDS_CSP_RGB24;
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

	if(m_nXDSCSPType ==XDS_CSP_YV12)
	{
		int nOffsetLine =m_nOffsetLine;
		if (*pmtIn->Subtype( ) == MEDIASUBTYPE_MJPG)
		{
			m_nOffsetLine =0;
		}

		if (m_nHeight==360 -8 && m_nWidth==640)
		{
			m_nHeight +=8;
			m_nOffsetLine =8;
		}
		else if (m_nHeight==540 -12 && m_nWidth==960)
		{
			m_nHeight +=12;
			m_nOffsetLine =12;
		}
		else if (m_nHeight==1080 -8 && m_nWidth==1920)
		{
			m_nHeight +=8;
			m_nOffsetLine =32;
		}


		m_nCaptureImageSize=(m_nWidth* m_nHeight *3)>>1;
	}

	return NOERROR; 
}

extern void ImageCopy(unsigned char*pDst,int nDstStride,unsigned char*pSrc,int nSrcStride,int nWidth,int nHeight);

void YUV422To420UV(unsigned char*pDst,int nDstStride,unsigned char*pSrc,int nSrcStride,int nWidth,int nHeight)
{
#if (1)
	{
		int nWidthLoop=(nWidth>>4);

		for( int i = 0; i < nHeight; i++ )
		{
			unsigned char*pSrcUV=pSrc;
			unsigned char*pDstUV=pDst;
			for( int j = 0; j < nWidthLoop; j++ )
			{
				*( ( __m128i * ) pDstUV ) = _mm_avg_epu8( *( ( __m128i * ) pSrcUV ), *( ( __m128i * )  ( pSrcUV + nSrcStride ) ) );
				pSrcUV += 16;
				pDstUV += 16;
			}
			pSrc += (nSrcStride<<1);
			pDst += nDstStride;
		}
	}
#else
	for (int i=0;i<nHeight;i++)
	{
		memcpy(pDst,pSrc,nWidth);
		pSrc += (nSrcStride<<1);
		pDst += nDstStride;
	}
#endif
}

/*
{

}
*/

// have to ovverride this
//
HRESULT XCameraRenderEx::DoRenderSample( IMediaSample *pMediaSample )
{
	unsigned long ulCurTimestamp=XGetTimestamp();
	if (m_nLastFrameCountTimestamp==0)
	{
		m_nLastFrameCountTimestamp=ulCurTimestamp;
	}
	if (m_nFrameRate <=30)
	{
		m_nFrameCount++;
		unsigned long ulDuration=ulCurTimestamp-m_nLastFrameCountTimestamp;
		if (ulDuration>=10000 && m_nAdjustFrameRateCount<2)
		{
			m_nTotalFrameRate=m_nFrameCount*1000/ulDuration;
			m_nFrameCount=0;
			m_nLastFrameCountTimestamp=ulCurTimestamp;

			SetFrameRate(m_nFrameRate,m_nTotalFrameRate);

			m_nAdjustFrameRateCount++;
		}
	}

	if ((m_nSkipIndex++)%m_nSkipCount)
	{
		return NOERROR;
	}

	if (m_pMainBuffer==NULL)
	{
		m_pMainBuffer=(PBYTE)MALLOCMSG(m_nCaptureImageSize+m_nCaptureImageSize/4 );
		if (m_pMainBuffer==NULL)
		{
			return NOERROR;
		}
	}

	if (m_nXDSCSPType==XDS_CSP_RGB32)
	{
		if (m_pRGBBuffer==NULL)
		{
			m_pRGBBuffer=(PBYTE)MALLOCMSG(m_nWidth*m_nHeight*5  );
		}
		if (m_pRGBBuffer==NULL)
		{
			return NOERROR;
		}
	}

	if (m_nXDSCSPType==XDS_CSP_RGB24)
	{
		if (m_pRGBBuffer==NULL)
		{
			m_pRGBBuffer=(PBYTE)MALLOCMSG(m_nWidth*m_nHeight*4  );
		}
		if (m_pRGBBuffer==NULL)
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
			//memcpy(m_pMainBuffer,(unsigned char*)pBufferIn,m_nWidth*m_nHeight *3/2);
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
			pData[2] = pData[0] + m_nWidth*m_nHeight;    //天创恒达 RBS 虚拟摄像头 红变蓝 UV颠倒下
			pData[1] = pData[2] + ((m_nWidth*m_nHeight)>>2);
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
			fastmemcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
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
			fastmemcpy(m_pRGBBuffer,pBufferIn,pMediaSample->GetActualDataLength());
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
	case XDS_CSP_MJPG:
		{
			if (m_hMJPGDecoder==NULL)
			{
				m_hMJPGDecoder=AVCONAVC_DecoderOpen(m_nWidth,m_nHeight,AVC_CODEC_MJPG);
			}

			if (m_hMJPGDecoder)
			{
				unsigned char*pFrameData[3];
				int nLineSize[3];
				char*qscale_table=NULL;
				int qstride=0;
				int pix_fmt = -1;
				int nGotPicture=0;

				int nUsedBytes=0;
				int nWidth=0;
				int nHeight=0;
				int nKeyFrame=0;

				try
				{
					nGotPicture=AVCONAVC_DecoderDecode(m_hMJPGDecoder,(unsigned char*)pBufferIn,pMediaSample->GetActualDataLength(),pFrameData,nLineSize,&nWidth,&nHeight,&nUsedBytes,&nKeyFrame,&qscale_table,&qstride,&pix_fmt);
					if (nGotPicture>0)
					{
						int nYImageSize=m_nWidth*m_nHeight;
						unsigned char* pData[3];
						pData[0]=(unsigned char*)m_pMainBuffer;
						pData[1]=pData[0]+nYImageSize;
						pData[2]=pData[1]+(nYImageSize>>2);
						//DebugOutput(logcoreerror,"this:%p,mainbuffer:%p,pd0:%p,pd1:%p,pd2:%p,wid:%d,hei:%d,gotpic:%d,nline0:%d,nline1:%d ,nline2:%d ,pFrameData[1] :%p ,pFrameData[2] :%p\n", 
						//	this,m_pMainBuffer,pData[0],pData[1],pData[2],nWidth,nHeight,nGotPicture ,nLineSize[0],nLineSize[1],nLineSize[2],pFrameData[1],pFrameData[2]) ;
						if (pix_fmt == 12)			//yuvj420p
						{
							ImageCopy(pData[0],m_nWidth,pFrameData[0],nLineSize[0],m_nWidth,m_nHeight);
							ImageCopy(pData[1],m_nWidth>>1,pFrameData[1],nLineSize[1],m_nWidth>>1,m_nHeight>>1);
							ImageCopy(pData[2],m_nWidth>>1,pFrameData[2],nLineSize[2],m_nWidth>>1,m_nHeight>>1);
						}
						else if (pix_fmt == 13)		//yuvj422p
						{
							ImageCopy(pData[0],m_nWidth,pFrameData[0],nLineSize[0],m_nWidth,m_nHeight);
							YUV422To420UV(pData[1],m_nWidth>>1,pFrameData[1],nLineSize[1],m_nWidth>>1,m_nHeight>>1);
							YUV422To420UV(pData[2],m_nWidth>>1,pFrameData[2],nLineSize[2],m_nWidth>>1,m_nHeight>>1);
						}
					}
					else
					{
						return NOERROR;
					}
				}
				catch(...)
				{
					return NOERROR;
				}
			}
		}
		break;
	default:
		{
		}
		return NOERROR;
	}

	unsigned char*pImageData=m_pMainBuffer;
	nImageSize=m_nCaptureImageSize;
	bool bDoubleField=false;
	int nWidth=m_nWidth;
	int nHeight=m_nHeight;

	if (m_nDstWidth!=0 && m_nDstHeight!=0)
	{
		if (m_nDstWidth<=m_nWidth && m_nDstHeight<m_nHeight)
		{
			unsigned char*pSrcY=pImageData;
			unsigned char*pSrcU=pSrcY+m_nWidth*m_nHeight;
			unsigned char*pSrcV=pSrcU+m_nWidth*m_nHeight/4;

			nImageSize=m_nDstWidth*m_nDstHeight*3/2;
			pImageData=(unsigned char*)MALLOCMSG(nImageSize+m_nDstWidth*m_nDstHeight  );
			if (pImageData==NULL)
			{
				DebugOutput(logcoreerror,"DoRenderSample pImageData malloc error return.\n");
				return NOERROR;
			}

			unsigned char*pDstY=pImageData;
			unsigned char*pDstU=pDstY+m_nDstWidth*m_nDstHeight;
			unsigned char*pDstV=pDstU+m_nDstWidth*m_nDstHeight/4;

			int nHalfDstHeight=m_nDstHeight/2;
			int nHalfDstWidth=m_nDstWidth/2;
			int nHalfWidth=m_nWidth/2;

			int nOffsetHeight=(m_nHeight-m_nDstHeight)/2;
			int nOffsetWidth=(m_nWidth-m_nDstWidth)/2;
			pSrcY+=nOffsetHeight*m_nWidth+nOffsetWidth;
			nOffsetHeight/=2;
			nOffsetWidth/=2;
			pSrcU+=nOffsetHeight*nHalfWidth+nOffsetWidth;
			pSrcV+=nOffsetHeight*nHalfWidth+nOffsetWidth;

			for (int i=0;i<nHalfDstHeight;++i)
			{
				memcpy(pDstY,pSrcY,m_nDstWidth);
				pDstY+=m_nDstWidth;
				pSrcY+=m_nWidth;
				memcpy(pDstY,pSrcY,m_nDstWidth);
				pDstY+=m_nDstWidth;
				pSrcY+=m_nWidth;

				memcpy(pDstU,pSrcU,nHalfDstWidth);
				pDstU+=nHalfDstWidth;
				pSrcU+=nHalfWidth;

				memcpy(pDstV,pSrcV,nHalfDstWidth);
				pDstV+=nHalfDstWidth;
				pSrcV+=nHalfWidth;
			}

			nImageSize=m_nDstWidth*m_nDstHeight*3/2;
			nWidth=m_nDstWidth;
			nHeight=m_nDstHeight;

			m_nVirtualWidth=nWidth;
			m_nVirtualHeight=nHeight;
		}
	}


	if(m_nXDSCSPType ==XDS_CSP_YV12 && m_nOffsetLine >0 )
	{
		if( m_nWidth*m_nHeight *3/2 >= MaxOffsetWHSize )
		{
			DebugOutput(logmodother,"m_nWidth*m_nHeight *3/2 >= MaxOffsetWHSize \n");
			goto COMMIT ;
		}

		if(m_pOffsetBuf)
		{
			int nOffset =m_nWidth *m_nOffsetLine; //offset pixels
			//change size  
			if(m_nCaptureImageSizeBack <=0)
			{
				m_nCaptureImageSizeBack = m_nCaptureImageSize - nOffset *3/2;

			}
			if(m_nHeightBack <= 0)
			{
				m_nHeightBack = m_nHeight - m_nOffsetLine ;

			}

			//Y U V position
			int nY=m_nWidth*m_nHeight;
			int nUV=nY/4;
			unsigned char*pYUV[3];
			pYUV[0]=(unsigned char*)pImageData;
			pYUV[1]=(unsigned char*)pImageData+nY;
			pYUV[2]=pYUV[1]+nUV;

			//Y U V offset position
			pYUV[0]=pYUV[0]+nOffset;
			pYUV[1]=pYUV[1]+nOffset /4;
			pYUV[2]=pYUV[2]+nOffset /4;

			//copy to temp buffer
			fastmemcpy(m_pOffsetBuf ,pYUV[0],nY-nOffset);
			fastmemcpy(m_pOffsetBuf +nY-nOffset,pYUV[1],nUV-nOffset/4);
			fastmemcpy(m_pOffsetBuf +nY-nOffset +nUV-nOffset/4,pYUV[2],nUV-nOffset/4);
			//copy to pImageData
			memset(pImageData,0,nImageSize);
			fastmemcpy(pImageData,m_pOffsetBuf,nY+nUV*2 - nOffset *3/2);

			//change size
			nHeight = m_nHeightBack;
			m_nVirtualHeight = m_nHeightBack;
			nImageSize = m_nCaptureImageSizeBack; //??
		}
	}

COMMIT:

	NEWRECORD3( XImage*,pXImage ,new XImage(pImageData,nImageSize))
	NULLReturnRet( pXImage , -1 ) 
	pXImage->AddRef();
	if (pImageData==m_pMainBuffer)
	{
		m_pMainBuffer=NULL;
	}

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(nHeight);
	pXImage->SetVirtualWidth(m_nVirtualWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetDoubleField(bDoubleField);
	pXImage->SetEvenField(bDoubleField);
	pXImage->SetOddField(bDoubleField);
	pXImage->SetTimestamp(ulTimestamp);
	pXImage->SetFrameRate(m_nActualFrameRate);

	//TRACE("XCameraRenderEx ulTimestamp=%u\n",ulTimestamp);

	m_rCameraCap.OnCaptureVideoFrame(pXImage);

	pXImage->Release();
	pXImage=NULL;

	return NOERROR; 
}

// have to override this
//
HRESULT XCameraRenderEx::SetMediaType( const CMediaType *pmt )
{
	return NOERROR; 
}

// override these to receive indication of when we change
// to Pause/Play (Active) or Stop (Inactive) state.
HRESULT XCameraRenderEx::Active()
{
	return NOERROR; 
}

HRESULT XCameraRenderEx::Inactive()
{
	return NOERROR; 
}

void XCameraRenderEx::SetFrameRate(unsigned short usFrameRate,unsigned short nTotalFrameRate)
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
		m_nFrameRate=usFrameRate;
		m_nSkipCount=1;
	}


	if (m_nFrameRate>=nTotalFrameRate)
	{
		m_nSkipCount=1;
		if (nTotalFrameRate>30)
		{
			m_nActualFrameRate=nTotalFrameRate;
		}
		else if (nTotalFrameRate>25)
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

void XCameraRenderEx::SetDstWidth(int nWidth)
{
	m_nDstWidth=nWidth;
}

void XCameraRenderEx::SetDstHeight(int nHeight)
{
	m_nDstHeight=nHeight;
}
