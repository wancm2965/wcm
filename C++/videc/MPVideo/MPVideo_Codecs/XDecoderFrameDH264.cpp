#include <winsock2.h>
#include <windows.h>

#include "XDecoderFrameDH264.h"
#include "XAutoLock.h"
#include "../MPVIDEO_API/XImage.h"
#include "XUtil.h"
#include <VIDEC/VIDEC_Header.h>


#include "DH264PlayWrapper.h"


void DH264_YUV_ImageCallback(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

XDecoderFrame*CreateXDecoderFrameDH264(MPVideo_DecoderCallback&rCallback)
{
	return new XDecoderFrameDH264(rCallback);
}


XDecoderFrameDH264::XDecoderFrameDH264(MPVideo_DecoderCallback&rCallback)
:XDecoderFrame(rCallback)

,m_pImageBuf(NULL)
,m_nImageBufLen(0)								

,m_pHandle(NULL)
{
}

XDecoderFrameDH264::~XDecoderFrameDH264(void)
{
}

int XDecoderFrameDH264::Open(int nWidth,int nHeight)
{
	//m_nWidth=nWidth;
	//m_nHeight=nHeight;

	m_pHandle=DH264PlayerOpen(DH264_YUV_ImageCallback,this);
	if (m_pHandle==NULL)
	{
		return -1;
	}


	return 0;
}

void XDecoderFrameDH264::Close(void)
{
	if (m_pHandle!=NULL)
	{
		DH264PlayerClose(m_pHandle);
		m_pHandle=NULL;
	}

	if (m_pImageBuf!=NULL)
	{
		free(m_pImageBuf);
		m_pImageBuf=NULL;
	}
}

int XDecoderFrameDH264::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (m_pHandle!=NULL)
	{
		DH264PlayerInputData(m_pHandle,(unsigned char*)pPacketData,nPacketSize);
	}

	return 0;
}

int XDecoderFrameDH264::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	if (pPacketData!=NULL && nPacketSize>=12)
	{
		DecodeFramePacket((unsigned char*)(pPacketData)+12,nPacketSize-12);
	}
	return 0;
}

void XDecoderFrameDH264::OnImageData(unsigned char*pData,int nLen,int nWidth,int nHeight,long nStamp)
{
	//char szDebug[1024];
	//sprintf(szDebug,"nStamp=%d %u\n",nStamp,XGetTimestamp());
	//OutputDebugString(szDebug);

	if (m_pImageBuf==NULL || m_nImageBufLen<nLen)
	{
		if (m_pImageBuf!=NULL)
		{
			free(m_pImageBuf);
			m_pImageBuf=NULL;
		}

		m_nImageBufLen=nLen;
		m_pImageBuf=(unsigned char*)malloc(m_nImageBufLen+(m_nImageBufLen>>2));
		if (m_pImageBuf==NULL)
		{
			return;
		}
	}
	
	memcpy(m_pImageBuf,pData,nLen);

	XImage*pXImage=new XImage(m_pImageBuf,nLen);
	pXImage->AddRef();
	m_pImageBuf=NULL;

	pXImage->SetActualWidth(nWidth);
	pXImage->SetActualHeight(nHeight);
	pXImage->SetVirtualWidth(nWidth);
	pXImage->SetVirtualHeight(nHeight);
	pXImage->SetDoubleField(false);
	pXImage->SetEvenField(false);
	pXImage->SetOddField(false);
	pXImage->SetTimestamp(0);
	pXImage->SetTimestampOffset(nStamp);

	m_rCallback.OnMPVideo_DecoderCallbackFrame(this,pXImage);

	pXImage->Release();
	pXImage=NULL;
}

void DH264_YUV_ImageCallback(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp)
{
	XDecoderFrameDH264*pXDecoderFrameDH264=(XDecoderFrameDH264*)pUser;
	if (pXDecoderFrameDH264)
	{
		pXDecoderFrameDH264->OnImageData(pYUVData,nYUVDataLen,nWidth,nHeight,nStamp);
	}
}
