#include "XCapChan.h"
#include "VIDEC.h"
#include "VIDEC_Header.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

VIDEC_CapChan*VIDEC_CapChan::Create(VIDEC_CapChanCallback&rCallback)
{
	//return new XCapChan(rCallback);
	NEWRECORDret3(  VIDEC_CapChan* ,p, new XCapChan(rCallback) ) 

}

int MyGetCodecInfoWithBitrate(int& nBitrateControlType,int&nBitrate,int&nQMin,int&nQMax,VIDEC_CODEC_TYPE&CodecType)
{
	int nQuality=nBitrate;
	if (nBitrate<GetQualityCount())
	{
		return GetCodecInfo(nQuality,nBitrateControlType,nBitrate,nQMin,nQMax,CodecType,false);
	}

	int nBitrateTemp=nBitrate;
	return GetCodecInfo(nQuality,nBitrateControlType,nBitrateTemp,nQMin,nQMax,CodecType,false);
}


XCapChan::XCapChan(VIDEC_CapChanCallback&rCallback)
:m_rCallback(rCallback)
,XImageSink(this)
,XStreamSource(this)

,m_pEncoderStream(NULL)

,m_nBitrateControlType(VIDEC_NETWORK_INTERNET)
,m_CodecType(VIDEC_CODEC_H264)

,m_nBitrate(0)
,m_nQMin(16)
,m_nQMax(36)

,m_nWidth(352)
,m_nHeight(288)

,m_nClipWidth(0)
,m_nClipHeight(0)

,m_nScaleType(0)

,m_pResize(NULL)
,m_nDstBufLen(0)
,m_pDstBuf(NULL)
{
	m_ulLastCountFrameTimestamp=0;
	m_nFrameRateCur=0;	
	m_bSkipFrame=false;
	m_bForceSkipFrame=false;
	m_nFrameRate=0;
}

XCapChan::~XCapChan(void)
{

}

int XCapChan::Open(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nWidth,int nHeight)
{
	return Reset(nBitrate,nBitrateControlType,CodecType,nScaleType,nWidth,nHeight);
}

int XCapChan::Reset(int nBitrate,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nWidth,int nHeight)
{
	if (nWidth !=0 && nWidth < 32)
	{
		nWidth=32;
	}
	if (nHeight !=0 && nHeight < 32)
	{
		nHeight=32;
	}

	////?? test ,cs1950 platform use
	//struct tagHeightPair
	//{
	//	int nAct; int nTemp;
	//};

	//struct tagHeightPair  arrHP[] ={ {540,528}, {360,352}, {270,256}, {216,208},{180,176} };
	//int nCount =sizeof(arrHP) / sizeof(tagHeightPair) ;
	//for(int i=0;i<nCount;i++)
	//{
	//	if(nHeight == arrHP[i].nAct/4 )  { nHeight = arrHP[i].nTemp/4 ;  break; }
	//	else if(nHeight == arrHP[i].nAct/2 )  { nHeight = arrHP[i].nTemp/2 ;  break; }
	//	else if(nHeight == arrHP[i].nAct)  { nHeight =  arrHP[i].nTemp ;  break; }
	//}

	//char msg[128]={0};
	//sprintf(msg,"CapChan::Reset ,width:%d,height:%d,\n",nWidth,nHeight);
	//DebugOutput(msg);

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nWidth-=(m_nWidth%2);
	m_nHeight-=(m_nHeight%2);

	MyGetCodecInfoWithBitrate(nBitrateControlType,nBitrate,m_nQMin,m_nQMax,CodecType);
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;

	m_CodecType=CodecType;

	if (m_nWidth==0)
	{
		if (nScaleType<0)
			nScaleType=0;
		else if (nScaleType>2)
			nScaleType=2;
	}
	else
	{
		nScaleType=0;
	}

	m_nScaleType=nScaleType;

	if (m_pEncoderStream==NULL)
	{
		m_pEncoderStream=VIDEC_EncoderStreamCreate(*this,false);
		if (m_pEncoderStream==NULL)
		{
			return -1;
		}
	}

	return 0;
}

void XCapChan::Close(void)
{
	//DebugOutput("XCapChan  Close   0\n");

	XImageSink::Close();
	//DebugOutput("XCapChan  Close   1\n");

	XStreamSource::Close();

	if (m_pEncoderStream!=NULL)
	{
		//DebugOutput("XCapChan EncoderStream Close   1\n");
		m_pEncoderStream->Close();
		//DebugOutput("XCapChan EncoderStream Close  2\n");
		delete m_pEncoderStream;
		//DebugOutput("XCapChan EncoderStream Close  3\n");
		m_pEncoderStream=NULL;
	}

	if (m_pResize!=NULL)
	{
		//DebugOutput("XCapChan Resize Close  1\n");
		m_pResize->Close();
		delete m_pResize;
		//DebugOutput("XCapChan Resize Close  2\n");
		m_pResize=NULL;
	}
	
	if (m_pDstBuf!=NULL)
	{
		FREEMSG(m_pDstBuf);
		m_pDstBuf=NULL;
	}

	DebugOutput( logcallstack ,"XCapChan  Close  3. \n");

}

int XCapChan::RequestKeyFrame(void)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->RequestKeyFrame();
	}
	return 0;
}

int XCapChan::GetEncoderType(void)
{
	if (m_pEncoderStream!=NULL)
	{
		return m_pEncoderStream->GetEncoderType();
	}
	return 0;
}

void XCapChan::SetSkipFrame(bool bSkip)
{
	m_bForceSkipFrame = bSkip;
}

void XCapChan::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XCapChan::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XCapChan::OnReceivedPreSend(int& nSend)
{
	int nSkipFlag=0;
	OnVIDEC_EncoderStreamCallbackPreEncode(nSkipFlag);	
	nSend = (nSkipFlag==0);
}


void XCapChan::ReSetFrameRate(void)
{
	m_nFrameRateCur++;

	unsigned long ulCurTimestamp = timeGetTime();
	if (m_ulLastCountFrameTimestamp == 0)
	{
		m_ulLastCountFrameTimestamp = ulCurTimestamp;
	}
	unsigned long ulTemp = ulCurTimestamp - m_ulLastCountFrameTimestamp;
	if ((m_nFrameRateCur > 30 && ulTemp < 990) || (m_nFrameRateCur <= 30 && ulTemp < 980))
	{
		return;
	}

	if (ulTemp > 1500)
	{
		float f = 1000.00/ulTemp;
		m_nFrameRate=m_nFrameRateCur*f;
	}
	else
	{
		m_nFrameRate=m_nFrameRateCur;
	}
	m_ulLastCountFrameTimestamp = ulCurTimestamp;
	m_nFrameRateCur=0;
}

void XCapChan::OnReceivedImage(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (VIDEC_GetEnableDonotEncodeNoSignalVideo() && pXImage->GetVideoSignal()==0)
	{
		return ;
	}

	int nActualWidth=pXImage->GetActualWidth();
	int nActualHeight=pXImage->GetActualHeight();
	
	switch (m_nScaleType)
	{
	case 1:
		m_nWidth=nActualWidth>>1;
		if ((nActualWidth==1920 && nActualHeight==540) ||
			(nActualWidth==704 && nActualHeight==288) ||
			(nActualWidth==704 && nActualHeight==240))
		{
			m_nHeight=nActualHeight;
		}
		else
		{
			m_nHeight=nActualHeight>>1;
		}
		m_nWidth-=(m_nWidth%2);
		m_nHeight-=(m_nHeight%2);
		break;
	case 2:
		m_nWidth=nActualWidth>>2;
		if ((nActualWidth==1920 && nActualHeight==540) ||
			(nActualWidth==704 && nActualHeight==288) ||
			(nActualWidth==704 && nActualHeight==240))
		{
			m_nHeight=nActualHeight>>1;
		}
		else
		{
			m_nHeight=nActualHeight>>2;
		}
		m_nWidth-=(m_nWidth%2);
		m_nHeight-=(m_nHeight%2);
		break;
	default:
		break;
	}
	if (nActualWidth==1920 && nActualHeight==1080)
	{
		if (m_nWidth == 960)
		{
			m_nHeight = 540;
		}
		else if (m_nWidth == 480)
		{
			m_nHeight = 270;
		}
	}

	int nNewVirtualHeight = m_nHeight;

	if ((nActualWidth==1920 && nActualHeight==540) ||
		(nActualWidth==704 && nActualHeight==288) ||
		(nActualWidth==704 && nActualHeight==240))
	{
		if (m_nWidth*2 == nActualWidth)
		{
			m_nHeight = nActualHeight/2;
			nNewVirtualHeight=nActualHeight;
		}
		if (m_nWidth*4 == nActualWidth)
		{
			m_nHeight = nActualHeight/4;
			nNewVirtualHeight = nActualHeight/2;
		}
	}

	nNewVirtualHeight-=(nNewVirtualHeight%2);
	m_nHeight-=(m_nHeight%2);

	if (m_nScaleType == 2 || m_nScaleType == 1 || m_bForceSkipFrame)
	{
		ReSetFrameRate();

		if (m_nFrameRate > 40)
		{
			m_bSkipFrame = !m_bSkipFrame;
		}
		else
		{
			m_bSkipFrame=false;
		}

		if (m_bSkipFrame)
		{
			return;
		}
	}

	//DebugOutput("XCapChan::OnReceivedImage m_nScaleType=%d nActualWidth=%d m_nWidth=%d nActualHeight=%d m_nHeight=%d this=%u\n",m_nScaleType,nActualWidth,m_nWidth,nActualHeight,m_nHeight,this);


 	if (m_nWidth!=0 && m_nWidth<=nActualWidth && m_nHeight!=0 && m_nHeight<=nActualHeight)
	{
		unsigned char*pData=(unsigned char*)pXImage->GetData();
		int nLen=pXImage->GetDataLen();

		m_nClipWidth=m_nWidth*nActualHeight/m_nHeight;
		m_nClipWidth-=(m_nClipWidth%2);
		if (m_nClipWidth>nActualWidth)
		{
			m_nClipWidth=nActualWidth;
			m_nClipHeight=m_nHeight*nActualWidth/m_nWidth;
			m_nClipHeight-=(m_nClipHeight%2);
			if (m_nClipHeight>nActualHeight)
			{
				m_nClipHeight=nActualHeight;
			}
		}
		else
		{
			m_nClipHeight=nActualHeight;
		}

		if (m_pResize==NULL)
		{
			m_pResize=VIDEC_Resize::Create();
			if (m_pResize!=NULL)
			{
				if (0!=m_pResize->Open())
				{
					m_pResize->Close();
					delete m_pResize;
					m_pResize=NULL;
				}
			}
		}

		if (m_pDstBuf==NULL)
		{
			m_nDstBufLen=m_nWidth*m_nHeight*3/2;
			m_pDstBuf=(unsigned char*)MALLOCMSG(m_nDstBufLen+(m_nDstBufLen>>2));
			if (m_pDstBuf!=NULL)
			{
				int nDstYBufferSize = m_nWidth * m_nHeight;
				int nDstUVBufferSize = nDstYBufferSize >> 2;
				m_dst[0] = m_pDstBuf;
				m_dst[1] = m_dst[0] + nDstYBufferSize;
				m_dst[2] = m_dst[1] + nDstUVBufferSize;
			}
		}

		if (m_pResize==NULL || m_pDstBuf==NULL)
		{
			return;
		}

		int nSrcSkipLines=((nActualHeight-m_nClipHeight)>>1);
		nSrcSkipLines-=(nSrcSkipLines%2);
		//nSrcSkipLines=0;//Áô×óÉÏ½Ç
		int nSrcSkipYLine=nActualWidth*nSrcSkipLines;
		int nSrcSkipUVLine=(nSrcSkipYLine>>2);
		int nSrcYOffset=(nActualWidth-m_nClipWidth)>>1;
		nSrcYOffset-=(nSrcYOffset%2);
		//nSrcYOffset=0;//Áô×óÉÏ½Ç
		int nSrcUVOffset=(nSrcYOffset>>1);

		int nSrcYBufferSize = nActualWidth * nActualHeight;
		int nSrcUVBufferSize = nSrcYBufferSize >> 2;
		unsigned char*src[3];
		src[0]=pData;
		src[1] = src[0] + nSrcYBufferSize;
		src[2] = src[1] + nSrcUVBufferSize;

		src[0]+=nSrcYOffset+nSrcSkipYLine;
		src[1]+=nSrcUVOffset+nSrcSkipUVLine;
		src[2]+=nSrcUVOffset+nSrcSkipUVLine;

		if (0!=m_pResize->Resize(src,m_nClipWidth,m_nClipHeight,m_dst,m_nWidth,m_nHeight,nActualWidth,m_nWidth))
		{
			DebugOutput( logcoreerror ,"Received image Resize  return.\n");
			return;
		}
		pData=m_pDstBuf;
		nLen=m_nDstBufLen;

		if (m_pEncoderStream!=NULL)
		{
			m_pDstBuf=NULL;
			NEWRECORD3( XImage*,pXImageNew , new XImage(pData,nLen) );
			NULLReturn( pXImageNew ) 
			pXImageNew->AddRef();

			pXImageNew->SetActualWidth(m_nWidth);
			pXImageNew->SetActualHeight(m_nHeight);
			pXImageNew->SetVirtualWidth(m_nWidth);
			pXImageNew->SetVirtualHeight(nNewVirtualHeight);
			pXImageNew->SetEvenFieldFirst(1);
			pXImageNew->SetDoubleField(0);
			pXImageNew->SetEvenField(0);
			pXImageNew->SetOddField(0);
			pXImageNew->SetTimestamp(pXImage->GetTimestamp());
			pXImageNew->SetTimestampOffset(pXImage->GetTimestampOffset());
			pXImageNew->SetFrameRate(pXImage->GetFrameRate());
			pXImageNew->SetHardware(pXImage->GetHardware());

			m_pEncoderStream->EncodeFrame(pXImageNew,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,m_CodecType);
			
			pXImageNew->Release();
			pXImageNew=NULL;
		}
	}
	else
	{
		if (m_pResize!=NULL)
		{
			m_pResize->Close();
			delete m_pResize;
			m_pResize=NULL;
		}
		if (m_pDstBuf!=NULL)
		{
			FREEMSG(m_pDstBuf);
			m_pDstBuf=NULL;
		}

		if (m_pEncoderStream!=NULL)
		{
			m_pEncoderStream->EncodeFrame(pImage,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,m_CodecType);
		}
	}
}


void XCapChan::OnVIDEC_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
	SendStreamData((unsigned char*)pPacketData,nPacketSize,nWidth,nHeight,nHeaderLen);
}

void XCapChan::OnVIDEC_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{

}

void XCapChan::OnVIDEC_EncoderStreamCallbackPreEncode(int&nSkipFlag)
{
	m_rCallback.OnVIDEC_CapChanCallbackPreEncode(this,nSkipFlag);
}

int XCapChan::AddStreamSink(VIDEC_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XCapChan::RemoveStreamSink(VIDEC_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}
