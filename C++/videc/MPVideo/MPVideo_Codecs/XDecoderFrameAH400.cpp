#include <winsock2.h>
#include <windows.h>

#include "XDecoderFrameAH400.h"
#include "XAutoLock.h"
#include "../MPVIDEO_API/XImage.h"
#include "XUtil.h"
#include "AVCONProduct.h"
#include <VIDEC/VIDEC_Header.h>


#include "AH400PlayWrapper.h"

bool			g_bUserError=false;
unsigned long	g_nOpenCount=0;

#define ERROR_PERCENT	10

extern DWORD GetUsedByProductType(void);


unsigned char g_pHiKHeader[40]={
			0x34,0x48,0x4b,0x48,0xfe,
			0xb3,0xd0,0xd6,0x08,0x03,
			0x04,0x20,0x00,0x00,0x00,
			0x00,0x03,0x10,0x01,0x10,
			0x01,0x10,0x10,0x00,0x80,
			0x3e,0x00,0x00,0x60,0x01,
			0x20,0x01,0x11,0x10,0x00,
			0x00,0x00,0x00,0x00,0x00};


void AH400_YUV_ImageCallback(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp);

XDecoderFrame*CreateXDecoderFrameAH400(MPVideo_DecoderCallback&rCallback,VIDEC_CODEC_TYPE CodecType)
{
	return new XDecoderFrameAH400(rCallback,CodecType);
}


XDecoderFrameAH400::XDecoderFrameAH400(MPVideo_DecoderCallback&rCallback,VIDEC_CODEC_TYPE CodecType)
:XDecoderFrame(rCallback)

,m_pImageBuf(NULL)
,m_nImageBufLen(0)								

,m_pHandle(NULL)

,m_CodecType(CodecType)
{
	m_bUserError=false;

	//char szDebug[1024];
	//sprintf(szDebug,"m_CodecType=%d\n",m_CodecType);
	//OutputDebugString(szDebug);
}

XDecoderFrameAH400::~XDecoderFrameAH400(void)
{
}

int XDecoderFrameAH400::Open(int nWidth,int nHeight)
{
	HIK_TYPE nType=HIK_HC;

	if (m_CodecType==VIDEC_CODEC_HIKH)
	{
		nType=HIK_H;
	}
	else if (m_CodecType==VIDEC_CODEC_HIKC)
	{
		nType=HIK_CAMERA;
	}
	else if (m_CodecType==VIDEC_CODEC_IMKH)
	{
		nType=HIK_IMKH;
	}
	else if (m_CodecType==VIDEC_CODEC_DS81)
	{
		nType=HIK_DS81;
	}
	else if (m_CodecType==VIDEC_CODEC_DS71)
	{
		nType=HIK_DS71;
	}
	else if (m_CodecType==VIDEC_CODEC_X62MF)
	{
		nType=HIK_X62MF;
	}
	else if (m_CodecType==VIDEC_CODEC_MEGA)
	{
		nType=HIK_MEGA;
	}
	else if (m_CodecType==VIDEC_CODEC_DS80HD)
	{
		nType=HIK_DS80HD;
	}
	else
	{
		nType=HIK_HC;
	}
	m_pHandle=AH400PlayerOpen(nType,AH400_YUV_ImageCallback,this);

	if (m_pHandle==NULL)
	{
		return -1;
	}

	if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
		GetUsedByProductType()==AVCON_PRODUCT_PICC)
	{
		if (g_bUserError)
		{
			m_bUserError=(++g_nOpenCount)%ERROR_PERCENT==0?true:false;
		}
		else
		{
			m_bUserError=false;
		}
	}


	return 0;
}

void XDecoderFrameAH400::Close(void)
{
	if (m_pHandle!=NULL)
	{
		AH400PlayerClose(m_pHandle);
		m_pHandle=NULL;
	}

	if (m_pImageBuf!=NULL)
	{
		free(m_pImageBuf);
		m_pImageBuf=NULL;
	}
}

int XDecoderFrameAH400::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (GetUsedByProductType()==AVCON_PRODUCT_RS ||
		GetUsedByProductType()==AVCON_PRODUCT_PICC)
	{
		if (m_bUserError)
			return 0;
	}

	if (m_pHandle!=NULL)
	{
		AH400PlayerInputData(m_pHandle,(unsigned char*)pPacketData,nPacketSize);
	}

	return 0;
}

int XDecoderFrameAH400::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	if (pPacketData!=NULL && nPacketSize>=12)
	{
		DecodeFramePacket((unsigned char*)(pPacketData)+12,nPacketSize-12);
	}
	return 0;
}

void XDecoderFrameAH400::OnImageData(unsigned char*pData,int nLen,int nWidth,int nHeight,long nStamp)
{
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


	int nYSize=nWidth*nHeight;
	int nUVSize=(nYSize>>2);
	memcpy(m_pImageBuf,pData,nYSize);
	memcpy(m_pImageBuf+nYSize,pData+nYSize+nUVSize,nUVSize);
	memcpy(m_pImageBuf+nYSize+nUVSize,pData+nYSize,nUVSize);

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

void AH400_YUV_ImageCallback(void*pUser,unsigned char*pYUVData,int nYUVDataLen,int nWidth,int nHeight,long nStamp)
{
	XDecoderFrameAH400*pXDecoderFrameAH400=(XDecoderFrameAH400*)pUser;
	if (pXDecoderFrameAH400)
	{
		pXDecoderFrameAH400->OnImageData(pYUVData,nYUVDataLen,nWidth,nHeight,nStamp);
	}
}
