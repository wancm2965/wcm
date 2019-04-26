#include "XCapChan.h"
#include <MPVideo/MPVideo.h>
#include <VIDEC/VIDEC_Header.h>

MPVideo_CapChan*MPVideo_CapChan::Create(MPVideo_CapChanCallback&rCallback)
{
	return new XCapChan(rCallback);
}

XCapChan::XCapChan(MPVideo_CapChanCallback&rCallback)
:m_rCallback(rCallback)
,XImageSink(this)
,XStreamSource(this)

,m_pEncoderStream(NULL)

,m_nQuality(2)
,m_nBitrateControlType(MPVIDEO_NETWORK_INTERNET)
,m_CodecType(VIDEC_CODEC_H264)

,m_nBitrate(0)
,m_nQMin(16)
,m_nQMax(36)

,m_nScaleType(0)
,m_nWidth(352)
,m_nHeight(288)


,m_pResize(NULL)
,m_nDstBufLen(0)
,m_pDstBuf(NULL)
{
	m_nSkipFrameCount=0;
	m_nSkipFrameIndex=0;
	m_nSubWidth = 0;
	m_nSubHeight = 0;
}

XCapChan::~XCapChan(void)
{

}

int XCapChan::Open(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight)
{
	//char szDebug[1024];
	//sprintf(szDebug,"nQuality=%d nBitrateControlType=%d CodecType=%d nScaleType=%d\n",nQuality,nBitrateControlType,CodecType,nScaleType);
	//OutputDebugString(szDebug);

	GetCodecInfo(nQuality,nBitrateControlType,m_nBitrate,m_nQMin,m_nQMax,CodecType,false);
	m_nScaleType=nScaleType;
	m_nQuality=nQuality;
	m_nBitrateControlType=nBitrateControlType;

	m_CodecType=CodecType;
	m_nSubWidth = nSubWidth;
	m_nSubHeight = nSubHeight;
	m_nWidth=0;
	m_nHeight=0;

	m_pEncoderStream=MPVideo_EncoderStreamCreate(*this,false);
	if (m_pEncoderStream==NULL)
	{
		return -1;
	}

	return 0;
}

int XCapChan::Reset(int nQuality,int nBitrateControlType,VIDEC_CODEC_TYPE CodecType,int nScaleType,int nSubWidth,int nSubHeight)
{
	//char szDebug[1024];
	//sprintf(szDebug,"nQuality=%d nBitrateControlType=%d CodecType=%d nScaleType=%d\n",nQuality,nBitrateControlType,CodecType,nScaleType);
	//OutputDebugString(szDebug);

	GetCodecInfo(nQuality,nBitrateControlType,m_nBitrate,m_nQMin,m_nQMax,CodecType,false);
	m_nScaleType=nScaleType;
	m_nSubWidth = nSubWidth;
	m_nSubHeight = nSubHeight;

	m_nQuality=nQuality;
	m_nBitrateControlType=nBitrateControlType;

	m_CodecType=CodecType;

	m_nWidth=0;
	m_nHeight=0;

	if (m_pEncoderStream==NULL)
	{
		m_pEncoderStream=MPVideo_EncoderStreamCreate(*this,false);
		if (m_pEncoderStream==NULL)
		{
			return -1;
		}
	}

	return 0;
}

void XCapChan::Close(void)
{
	XImageSink::Close();
	XStreamSource::Close();

	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->Close();
		delete m_pEncoderStream;
		m_pEncoderStream=NULL;
	}

	if (m_pResize!=NULL)
	{
		m_pResize->Close();
		delete m_pResize;
		m_pResize=NULL;
	}
	
	if (m_pDstBuf!=NULL)
	{
		free(m_pDstBuf);
		m_pDstBuf=NULL;
	}
}

int XCapChan::RequestKeyFrame(void)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->RequestKeyFrame();
	}
	return 0;
}

int XCapChan::SetSkipFrame(int nSkipFrameCount)
{
	if (nSkipFrameCount<0)
		nSkipFrameCount=0;
	else if (nSkipFrameCount>1)
		nSkipFrameCount=1;
	m_nSkipFrameCount=nSkipFrameCount;
	return 0;
}

void XCapChan::OnAttachedToImageSource(MPVideo_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XCapChan::OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XCapChan::OnReceivedImage(MPVideo_Image*pImage)
{
	if (m_nSkipFrameCount)
	{
		m_nSkipFrameIndex++;
		if (m_nSkipFrameIndex<m_nSkipFrameCount)
		{
			return ;
		}
		m_nSkipFrameIndex=0;
	}

	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (m_nScaleType>=2)
	{
		m_nScaleType=2;
	}
	else if (m_nScaleType<=0)
	{
		m_nScaleType=0;
	}

	int nActualWidth=pXImage->GetActualWidth();
	int nActualHeight=pXImage->GetActualHeight();

	bool bDoubleFieldScale=false;
	bool bScaled=false;

	if (nActualWidth>=320 && nActualHeight>=240)
	{
		int nScaleType=m_nScaleType;
		bScaled=(nScaleType>0);
		if (pXImage->GetDoubleField() && 
			((nActualWidth==704 && (nActualHeight==576 || nActualHeight==480)) ||
			(nActualWidth==640 && nActualHeight==480)))
		{
			if (nScaleType>=2)
			{
				bDoubleFieldScale=true;
				nScaleType=1;
			}
		}

		if (nScaleType>=2)
		{
			if (nActualWidth<640 || nActualHeight<480)
			{
				m_nWidth=nActualWidth>>1;
				m_nHeight=nActualHeight>>1;
			}
			else
			{
				m_nWidth=nActualWidth>>m_nScaleType;
				m_nHeight=nActualHeight>>m_nScaleType;
			}
		}
		else
		{
			m_nWidth=nActualWidth>>m_nScaleType;
			m_nHeight=nActualHeight>>m_nScaleType;
		}

		if (nScaleType>0)
		{
			if (nActualHeight==540 && nActualWidth>(nActualHeight<<1))
			{
				m_nHeight<<=1;
			}
		}

		if (bScaled)
		{
			if (m_nHeight%2)
			{
				m_nHeight--;
			}
		}
	}
	else
	{
		m_nWidth=nActualWidth;
		m_nHeight=nActualHeight;
	}

	if (bScaled && m_nSubWidth > 0 && m_nSubHeight > 0)
	{
		m_nWidth	= m_nSubWidth;
		m_nHeight	= m_nSubHeight;
	}

	if (bScaled && m_nWidth!=0 && m_nWidth<=pXImage->GetActualWidth() && m_nHeight!=0 && m_nHeight<=pXImage->GetActualHeight())
	{
		unsigned char*pData=(unsigned char*)pXImage->GetData();
		int nLen=pXImage->GetDataLen();

		if (m_pResize==NULL)
		{
			m_pResize=MPVideo_Resize::Create();
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
			m_pDstBuf=(unsigned char*)malloc(m_nDstBufLen+(m_nDstBufLen>>2));
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

		int nSrcYBufferSize = pXImage->GetActualWidth() * pXImage->GetActualHeight();
		int nSrcUVBufferSize = nSrcYBufferSize >> 2;
		unsigned char*src[3];
		src[0]=pData;
		src[1] = src[0] + nSrcYBufferSize;
		src[2] = src[1] + nSrcUVBufferSize;
		if (0!=m_pResize->Resize(src,pXImage->GetActualWidth(),pXImage->GetActualHeight()/(bDoubleFieldScale?2:1),m_dst,m_nWidth,m_nHeight))
		{
			return;
		}
		pData=m_pDstBuf;
		nLen=m_nDstBufLen;

		if (m_pEncoderStream!=NULL)
		{
			m_pDstBuf=NULL;
			XImage*pXImageNew=new XImage(pData,nLen);
			pXImageNew->AddRef();

			pXImageNew->SetActualWidth(m_nWidth);
			pXImageNew->SetActualHeight(m_nHeight);
			pXImageNew->SetVirtualWidth(pXImage->GetVirtualWidth()>>m_nScaleType);
			pXImageNew->SetVirtualHeight(pXImage->GetVirtualHeight()>>m_nScaleType);
			pXImageNew->SetEvenFieldFirst(pXImage->GetEvenFieldFirst());
			pXImageNew->SetDoubleField(bDoubleFieldScale?0:pXImage->GetDoubleField());
			pXImageNew->SetEvenField(bDoubleFieldScale?0:pXImage->GetDoubleField());
			pXImageNew->SetOddField(bDoubleFieldScale?0:pXImage->GetDoubleField());
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
			free(m_pDstBuf);
			m_pDstBuf=NULL;
		}

		if (m_pEncoderStream!=NULL)
		{
			m_pEncoderStream->EncodeFrame(pImage,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,m_CodecType);
		}
	}
}


void XCapChan::OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
	SendStreamData((unsigned char*)pPacketData,nPacketSize,nWidth,nHeight,nHeaderLen);
}

void XCapChan::OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{

}

void XCapChan::OnMPVideo_EncoderStreamCallbackPreEncode(int&nSkipFlag)
{
	m_rCallback.OnMPVideo_CapChanCallbackPreEncode(this,nSkipFlag);
}

int XCapChan::AddStreamSink(MPVideo_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XCapChan::RemoveStreamSink(MPVideo_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}
