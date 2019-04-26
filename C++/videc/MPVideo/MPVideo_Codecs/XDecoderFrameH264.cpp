#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <winsock2.h>

#include "XDecoderFrameH264.h"
#include "../MPVIDEO_API/XImage.h"

#include <VIDEC/VIDEC_Header.h>
#include "XUtil.h"
#include "AVMPlayer.h"

#include <AVCONAVC/AVCONAVC.h>
#include "../../VIDEC/VIDEC_FRE/VIDECFREAPI.h"


XDecoderFrameH264::XDecoderFrameH264(MPVideo_DecoderCallback&rCallback)
:XDecoderFrame(rCallback)
	
,m_pHandle(NULL)
,m_pH264RTPFrame(NULL)

,m_nLastFrameIsKeyFrame(0)
,m_bGotFirstKeyFrame(false)
,m_usSeqNo(0)
{
	m_nLastTS=XGetTimestamp();

}

XDecoderFrameH264::~XDecoderFrameH264(void)
{

}

int XDecoderFrameH264::Open(int nWidth,int nHeight)
{
	//m_nWidth=nWidth;
	//m_nHeight=nHeight;
	m_pHandle=AVCONAVC_DecoderOpen(nWidth,nHeight);

	if (!m_pHandle)
	{
		return -1;
	}

	DebugOutput("------------AVCONAVC Decoder Open Handle:%p,Width:%d,Height:%d,this=%p \n",m_pHandle,nWidth,nHeight,this);

	m_pH264RTPFrame=new H264RTPFrame(*this);
	m_pH264RTPFrame->Open(97,1400,VIDEC_HEADER_EXT_KEY_FRAME_LEN);

	return 0;
}

void XDecoderFrameH264::Close(void)
{
	if (m_pHandle!=NULL)
	{
		AVCONAVC_DecoderClose(m_pHandle);
		m_pHandle=NULL;
	}

	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->Close();
		delete m_pH264RTPFrame;
		m_pH264RTPFrame=NULL;
	}

	XDecoderFrame::Close();
}

void ImageCopy(unsigned char*pDst,int nDstStride,unsigned char*pSrc,int nSrcStride,int nHeight)
{
	int nWidth=(nDstStride>nSrcStride?nSrcStride:nDstStride);
	for (int i=0;i<nHeight;i++)
	{
		memcpy(pDst,pSrc,nWidth);
		pDst+=nDstStride;
		pSrc+=nSrcStride;
	}
}

void ImageCopy(unsigned char*pDst,int nDstStride,unsigned char*pSrc,int nSrcStride,int nWidth,int nHeight)
{
	for (int i=0;i<nHeight;i++)
	{
		memcpy(pDst,pSrc,nWidth);
		pDst+=nDstStride;
		pSrc+=nSrcStride;
	}
}

extern const unsigned char *avc_find_startcode(const unsigned char *p, const unsigned char *end);

int XDecoderFrameH264::DecodeFramePacket(void*pPacketData,int nPacketSize)
{
	if (!m_pHandle)
		return -1;

	unsigned char*pFrameData[3];
	int nLineSize[3];
	char*qscale_table=NULL;
	int qstride=0;
	int nGotPicture=0;

	int nUsedBytes=0;
	int nWidth=0;
	int nHeight=0;
	int nH264DeblockingFilter=0;

	int nLeftLen=nPacketSize;
	unsigned char*pLeftData=(unsigned char*)pPacketData;

	//add log
	//if (FRE_GetDebugLevel() > 0)
	{
		unsigned long nCurTS=XGetTimestamp();
		if (nCurTS-m_nLastTS>=20000)
		{
			DebugOutput("-----------AVCONAVC Decoder Decode  PacketSize =%u \t this=%p \n",nPacketSize,this);
			m_nLastTS=nCurTS;
		}
	}

	while (nLeftLen>0)
	{
		try
		{
			nGotPicture=AVCONAVC_DecoderDecode(m_pHandle,(unsigned char*)pLeftData,nLeftLen,pFrameData,nLineSize,&nWidth,&nHeight,&nUsedBytes,&m_nLastFrameIsKeyFrame,&qscale_table,&qstride);

			//add log 
			if (nGotPicture <=0)
			{
				DebugOutput("AVCONAVC Decoder Decode error,result:%d \n" , nGotPicture);
			}

			if (!m_bGotFirstKeyFrame)
			{
				if (m_nLastFrameIsKeyFrame)
				{
					m_bGotFirstKeyFrame=true;
				}
			}
		}
		catch(...)
		{
			return -1;
		}

		if (nUsedBytes<=0)
		{
			return -1;
		}

		nLeftLen-=nUsedBytes;
		pLeftData+=nUsedBytes;

		if (nGotPicture>0 && m_bGotFirstKeyFrame)
		{
			if (m_nWidth!=nWidth ||
				m_nHeight!=nHeight)
			{
				OnResized(nWidth,nHeight);

			}

			if (m_pFrameData==NULL)
			{
				m_pFrameData=(unsigned char*)malloc(m_nFrameSize*1.25);
				if (m_pFrameData==NULL)
				{
					return -1;
				}

				m_pData[0]=(unsigned char*)m_pFrameData;
				m_pData[1]=m_pData[0]+m_nYImageSize;
				m_pData[2]=m_pData[1]+(m_nYImageSize>>2);
			}


			{
				ImageCopy(m_pData[0],m_nStride[0],pFrameData[0],nLineSize[0],m_nHeight);
				ImageCopy(m_pData[1],m_nStride[1],pFrameData[1],nLineSize[1],m_nHeight>>1);
				ImageCopy(m_pData[2],m_nStride[1],pFrameData[2],nLineSize[1],m_nHeight>>1);
			}

			XImage*pXImage=new XImage(m_pFrameData,m_nFrameSize);
			pXImage->AddRef();
			m_pFrameData=NULL;

			pXImage->SetActualWidth(m_nWidth);
			pXImage->SetActualHeight(m_nHeight);
			pXImage->SetVirtualWidth(m_nWidth);
			pXImage->SetVirtualHeight(m_nHeight);
			pXImage->SetDoubleField(false);
			pXImage->SetEvenField(false);
			pXImage->SetOddField(false);
			pXImage->SetTimestamp(0);

			m_rCallback.OnMPVideo_DecoderCallbackFrame(this,pXImage);

			pXImage->Release();
			pXImage=NULL;
		}
		else if (nGotPicture<=0)
		{
			const unsigned char *pTempData=(const unsigned char *)pPacketData;
			const unsigned char *pDataAfterStartCode;
			pDataAfterStartCode = avc_find_startcode(pTempData, pTempData + nPacketSize);
			if (pDataAfterStartCode)
			{
				while(!*(pDataAfterStartCode++));

				unsigned char nal = pDataAfterStartCode[0];
				unsigned char type = (nal & 0x1f);
				if (type==7 || type==8)
				{
					nGotPicture=1;
				}
			}
		}
	}

	return (nGotPicture>0?0:-1);
}

int XDecoderFrameH264::GetPostProcQuality(void)
{
	return 0;
}


int XDecoderFrameH264::DecodeRTPPacket(void*pPacketData,int nPacketSize)
{
	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->OnRecvdRTPPacket(pPacketData,nPacketSize);
	}

	return 0;
}

void XDecoderFrameH264::OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame)
{
	DecodeFramePacket(pPacketData,nPacketLen);

	//char szDebug[1024]="";
	//sprintf(szDebug,"XDecoderFrameH264 nPacketLen=%d m_nLastFrameIsKeyFrame=%d\n",nPacketLen,m_nLastFrameIsKeyFrame);
	//OutputDebugString(szDebug);

	if (m_bGotFirstKeyFrame)
	{
		int nKeyFrame=m_nLastFrameIsKeyFrame;
		unsigned char*pFramePacketData=(unsigned char*)pPacketData;
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pFramePacketData-=nHeaderLen;

		VIDEC_HEADER_EXT_RESET(pFramePacketData);
		VIDEC_HEADER_EXT_SET_CODEC_ID(pFramePacketData,VIDEC_CODEC_H264);
		VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pFramePacketData,1);
		VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pFramePacketData,0);
		VIDEC_HEADER_EXT_SET_HARDWARE(pFramePacketData,0);
		VIDEC_HEADER_EXT_SET_KEYFRAME(pFramePacketData,(nKeyFrame!=0?1:0));
		VIDEC_HEADER_EXT_SET_BFRAME(pFramePacketData,0);

		VIDEC_HEADER_EXT_SET_SEQUENCE(pFramePacketData,m_usSeqNo++);
		VIDEC_HEADER_EXT_SET_TIMESTAMP(pFramePacketData,XGetTimestamp());
		if (nKeyFrame)
		{
			VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pFramePacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pFramePacketData,m_nHeight);
			VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pFramePacketData,m_nWidth);
			VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pFramePacketData,m_nHeight);
		}
		
		m_rCallback.OnMPVideo_DecoderCallbackFramePacket(this,pFramePacketData,nHeaderLen+nPacketLen,m_nWidth,m_nHeight);
	}
}