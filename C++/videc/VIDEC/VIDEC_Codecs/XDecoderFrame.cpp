#include <winsock2.h>
#include <windows.h>

#include <stdlib.h>
#include <memory.h>

#include "XDecoderFrame.h"
#include "XDecoderFrameH263.h"
#include "XDecoderFrameH264.h"
#include "XDecoderFrameAH400.h"
#include "XDecoderFrameMPEG4.h"
#include "XDecoderFrameUser.h"
#include "../VIDEC_API/XImage.h"

VIDEC_API VIDEC_Decoder*VIDEC_DecoderCreate(VIDEC_DecoderCallback&rCallback,VIDEC_CODEC_TYPE codecType,int nWidth,int nHeight)
{
	XDecoderFrame*pXDecoderFrame=NULL;
	switch (codecType)
	{
	case VIDEC_CODEC_H263:
	case VIDEC_CODEC_H263P:
	case VIDEC_CODEC_H263PP:
		{
			NEWRECORD( pXDecoderFrame ,new XDecoderFrameH263(rCallback,(VIDEC_CODEC_H263==codecType?0:1)) )
		}
		break;
	case VIDEC_CODEC_H264:
	case VIDEC_CODEC_H264_SVC:
	case VIDEC_CODEC_DH264:
		{
			NEWRECORD( pXDecoderFrame , new XDecoderFrameH264(rCallback) )
		}
		break;
	case VIDEC_CODEC_AH400:
	case VIDEC_CODEC_HIKH:
	case VIDEC_CODEC_HIKC:
	case VIDEC_CODEC_IMKH:
	case VIDEC_CODEC_DS81:
	case VIDEC_CODEC_DS71:
	case VIDEC_CODEC_X62MF:
	case VIDEC_CODEC_MEGA:
	case VIDEC_CODEC_DS80HD:
		{
			pXDecoderFrame=CreateXDecoderFrameAH400(rCallback,codecType);
		}
		break;
	case VIDEC_CODEC_MPEG4:
		{
			NEWRECORD( pXDecoderFrame ,new XDecoderFrameMPEG4(rCallback) )
		}
		break;
	case VIDEC_CODEC_USER:
		{
			pXDecoderFrame=CreateXDecoderFrameUser(rCallback);
		}
		break;
	default:
		break;
	};
	//NULLReturnRet( pXDecoderFrame ,pXDecoderFrame )  
	if (pXDecoderFrame!=NULL)
	{
		if (0!=pXDecoderFrame->Open(nWidth,nHeight))
		{
			pXDecoderFrame->Close();
			delete pXDecoderFrame;
			pXDecoderFrame=NULL;

			if (codecType==VIDEC_CODEC_H264 || codecType==VIDEC_CODEC_H264_SVC)
			{
				NEWRECORD( pXDecoderFrame ,new XDecoderFrameH264(rCallback) )
				//NULLReturnRet( pXDecoderFrame ,pXDecoderFrame )  
				if (pXDecoderFrame!=NULL)
				{
					if (0!=pXDecoderFrame->Open(nWidth,nHeight))
					{
						pXDecoderFrame->Close();
						delete pXDecoderFrame;
						pXDecoderFrame=NULL;
					}
				}
			}
		}
	}

	return pXDecoderFrame;
}


XDecoderFrame::XDecoderFrame(VIDEC_DecoderCallback&rCallback)
:m_rCallback(rCallback)
,m_nWidth(0)
,m_nHeight(0)
,m_nFrameSize(0)
,m_pFrameData(NULL)
,m_nYImageSize(0)
,m_pIXPostProcess(NULL)
//,m_bLevelFixer(false)
,m_nFrameRate(0)
,m_bDoubleField(false)
{
}

XDecoderFrame::~XDecoderFrame(void)
{
}


void XDecoderFrame::Close(void)
{
	if (m_pFrameData)
	{
		FREEMSG(m_pFrameData);
		m_pFrameData=NULL;
	}

	if (m_pIXPostProcess)
	{
		m_pIXPostProcess->ReleaseConnections();
		delete m_pIXPostProcess;
		m_pIXPostProcess=NULL;
	}
}

int XDecoderFrame::OnResized(int nWidth,int nHeight)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_nYImageSize=m_nWidth*m_nHeight;
	m_nFrameSize=m_nYImageSize*3/2;

	m_nStride[0]=m_nWidth;
	m_nStride[1]=m_nWidth>>1;
	m_nStride[2]=m_nWidth>>1;


	if (m_pFrameData)
	{
		FREEMSG(m_pFrameData);
		m_pFrameData=NULL;
	}


	if (m_pIXPostProcess)
	{
		m_pIXPostProcess->ReleaseConnections();
		delete m_pIXPostProcess;
		m_pIXPostProcess=NULL;
	}

	return 0;
}

void XDecoderFrame::SetFrameRate(int nFrameRate)
{
	m_nFrameRate = nFrameRate;
}

int XDecoderFrame::PostProcess(unsigned char*pDecodedFrameData[],int nDecodedStride[],char*qscale_table,int qstride)
{
	if (m_pIXPostProcess==NULL)
	{
		m_pIXPostProcess=CreateIXPostProcess(m_nWidth,m_nHeight,GetPostProcQuality());
		if (m_pIXPostProcess==NULL)
			return -1;
		m_pIXPostProcess->SetLevel(GetPostProcQuality());
	}

	if (m_pIXPostProcess!=NULL)
	{
		if (m_pFrameData==NULL)
		{
			m_pFrameData=(unsigned char*)MALLOCMSG(m_nFrameSize*1.25);
			if (m_pFrameData==NULL)
			{
				return -1;
			}

			m_pData[0]=(unsigned char*)m_pFrameData;
			m_pData[1]=m_pData[0]+m_nYImageSize;
			m_pData[2]=m_pData[1]+(m_nYImageSize>>2);
		}

		if (m_pIXPostProcess->PostProcess(pDecodedFrameData,nDecodedStride,m_pData,m_nStride,m_nWidth,m_nHeight,qscale_table,qstride)==false)
		{
			return -1;
		}

		NEWRECORD3( XImage*,pXImage,new XImage(m_pFrameData,m_nFrameSize) )
		NULLReturnRet( pXImage ,-1 )  
		pXImage->AddRef();
		m_pFrameData=NULL;

		pXImage->SetActualWidth(m_nWidth);
		pXImage->SetActualHeight(m_nHeight);
		pXImage->SetVirtualWidth(m_nWidth);
		pXImage->SetVirtualHeight(m_nHeight);
		pXImage->SetDoubleField(m_bDoubleField);
		pXImage->SetEvenField(m_bDoubleField);
		pXImage->SetOddField(m_bDoubleField);
		pXImage->SetTimestamp(0);
		if (m_nFrameRate > 0)
		{
			pXImage->SetFrameRate(m_nFrameRate);
		}
		m_rCallback.OnVIDEC_DecoderCallbackFrame(this,pXImage);

		pXImage->Release();
		pXImage=NULL;

		return 0;
	}
	return -1;
}


int XDecoderFrame::GetPostProcQuality(void)
{
	return 6;
}