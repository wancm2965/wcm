#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include <winsock2.h>

#include "XDecoderFrameH264.h"
#include "../VIDEC_API/XImage.h"

#include "VIDEC_Header.h"
#include "XUtil.h"
#include "AVMPlayer.h"

#define USE_MT

#ifdef USE_MT
#include <AVCONAVC/AVCONAVC.h>
#else
#include "avc.h"
#endif

#include "../VIDEC_FRE/VIDECFREAPI.h"

//频繁进入退出分组导致内存增长问题加锁
// (FFMpeg存在bug，使用了全局的引用计数来控制是否释放内存资源
//  所以XDecoderFrameH264对象的成员变量锁，无法起效)
XCritSec XDecoderFrameH264::g_GroupLock;			//bcc modify


XDecoderFrameH264::XDecoderFrameH264(VIDEC_DecoderCallback&rCallback)
:XDecoderFrame(rCallback)
	
,m_pHandle(NULL)
,m_pH264RTPFrame(NULL)

,m_nLastFrameIsKeyFrame(0)
,m_bGotFirstKeyFrame(false)
,m_usSeqNo(0)
,m_nFrameRateActual(60)
{
#ifdef REC_H264
	m_pFile264=NULL;
#endif
}

XDecoderFrameH264::~XDecoderFrameH264(void)
{

}

int XDecoderFrameH264::Open(int nWidth,int nHeight)
{
	XAutoLock l(g_GroupLock);
	//m_nWidth=nWidth;
	//m_nHeight=nHeight;
#ifdef USE_MT
	m_pHandle=AVCONAVC_DecoderOpen(320,240/*nWidth,nHeight*/);
#else
	m_pHandle=avc_decoder_open(AVC_CODEC_H264,nWidth,nHeight);
#endif
	if (!m_pHandle)
	{
		return -1;
	}

	NEWRECORD( m_pH264RTPFrame,new H264RTPFrame(*this) )
	NULLReturnRet( m_pH264RTPFrame ,-1 )  
	m_pH264RTPFrame->Open(97,1400,VIDEC_HEADER_EXT_KEY_FRAME_LEN);

#ifdef REC_H264
	m_pFile264=fopen("g:\\tmp\\ipcamera.264","w+b");
#endif

	return 0;
}

void XDecoderFrameH264::Close(void)
{
	XAutoLock l(g_GroupLock);
	if (m_pHandle!=NULL)
	{
#ifdef USE_MT
		AVCONAVC_DecoderClose(m_pHandle);
#else
		avc_decoder_close(m_pHandle);
#endif
		m_pHandle=NULL;
	}

	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->Close();
		delete m_pH264RTPFrame;
		m_pH264RTPFrame=NULL;
	}

	XDecoderFrame::Close();


#ifdef REC_H264
	if (m_pFile264!=NULL)
	{
		fclose(m_pFile264);
		m_pFile264=NULL;
	}
#endif
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
	int pix_fmt = -1;
	int nGotPicture=0;

	int nUsedBytes=0;
	int nWidth=0;
	int nHeight=0;
	int nH264DeblockingFilter=0;

	int nLeftLen=nPacketSize;
	unsigned char*pLeftData=(unsigned char*)pPacketData;

#ifdef REC_H264
	if (m_pFile264!=NULL)
	{
		fwrite(&nPacketSize,1,4,m_pFile264);
		fwrite(pPacketData,1,nPacketSize,m_pFile264);
	}
#endif

	////add ,??
	//if(g_LogUtil)
	//{
	//	g_LogUtil->WriteFile(pPacketData,nPacketSize);
	//}

	while (nLeftLen>0)
	{
		try
		{
#ifdef USE_MT
			nGotPicture=AVCONAVC_DecoderDecode(m_pHandle,(unsigned char*)pLeftData,nLeftLen,pFrameData,nLineSize,&nWidth,&nHeight,&nUsedBytes,&m_nLastFrameIsKeyFrame,&qscale_table,&qstride,&pix_fmt);
#else
			nGotPicture=avc_decoder_decode(m_pHandle,(unsigned char*)pLeftData,nLeftLen,pFrameData,nLineSize,&qscale_table,&qstride,&nWidth,&nHeight,&nUsedBytes,&m_nLastFrameIsKeyFrame);
#endif

			//if( m_nLastFrameIsKeyFrame )  DebugOutput("H264 Decoder  KeyFrame.\n");

			if (nGotPicture <=0)
			{
				DebugOutput( logsoftdec ,"AVCONAVC Decoder Decode error,result:%d \n" , nGotPicture);
				//if(g_LogUtil) g_LogUtil->LogOutput("AVCONAVC Decoder Decode error,result :%d. \n",nGotPicture);
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
			//if(g_LogUtil) g_LogUtil->LogOutput("H264 DecodeFramePacket   catch return. \n");
			return -1;
		}

		if (nUsedBytes<=0)
		{
			//if(g_LogUtil) g_LogUtil->LogOutput("H264 DecodeFramePacket   nUsedBytes return. \n");
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

#if 0
			PostProcess(pFrameData,nLineSize,qscale_table,qstride);
#else
			if (m_pFrameData==NULL)
			{
				m_pFrameData=(unsigned char*)MALLOCMSG(m_nFrameSize*1.25);
				if (m_pFrameData==NULL)
				{
					//if(g_LogUtil) g_LogUtil->LogOutput("H264 DecodeFramePacket   malloc return. \n");
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

			NEWRECORD3( XImage*,pXImage ,new XImage(m_pFrameData,m_nFrameSize) )
			NULLReturnRet( pXImage ,-1 )  
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
			pXImage->SetFrameRate(m_nFrameRateActual);
			m_rCallback.OnVIDEC_DecoderCallbackFrame(this,pXImage);

			pXImage->Release();
			pXImage=NULL;
#endif
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

void XDecoderFrameH264::OnBaseRTPFrameCallbackFramePacket(BaseRTPFrame*pBaseRTPFrame,void*pPacketData,int nPacketLen,int nKeyFrame,int nFrameRate)
{
	m_nFrameRateActual = nFrameRate;

	DecodeFramePacket(pPacketData,nPacketLen);

	//char szDebug[1024]="";
	//sprintf(szDebug,"XDecoderFrameH264 nPacketLen=%d m_nLastFrameIsKeyFrame=%d\n",nPacketLen,m_nLastFrameIsKeyFrame);
	//DebugOutput(szDebug);

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
		
		m_rCallback.OnVIDEC_DecoderCallbackFramePacket(this,pFramePacketData,nHeaderLen+nPacketLen,m_nWidth,m_nHeight);
	}
}