#include "XCapChanRTP.h"
#include <MPVideo/MPVideo.h>
#include <VIDEC/VIDEC_Header.h>
#include "../MPVIDEO_Codecs/BaseRTPFrame.h"

MPVideo_CapChanRTP*MPVideo_CapChanRTP::Create(MPVideo_CapChanRTPCallback&rCallback)
{
	return new XCapChanRTP(rCallback);
}

XCapChanRTP::XCapChanRTP(MPVideo_CapChanRTPCallback&rCallback)
:m_rCallback(rCallback)
,XImageSink(this)
,XStreamSource(this)

,m_pEncoderStream(NULL)

,m_nBitrateControlType(MPVIDEO_NETWORK_PRIVATE)
,m_CodecType(VIDEC_CODEC_H264)

,m_nBitrate(0)
,m_nQMin(16)
,m_nQMax(36)

,m_nWidth(352)
,m_nHeight(288)


,m_pResize(NULL)
,m_nDstBufLen(0)
,m_pDstBuf(NULL)

,m_nPayloadType(97)
{

}

XCapChanRTP::~XCapChanRTP(void)
{

}

int XCapChanRTP::Open(int nPayloadType,int nWidth,int nHeight,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE CodecType)
{
	m_nPayloadType=nPayloadType;
	if (CodecType==VIDEC_CODEC_H263)
	{
		m_nPayloadType=34;
	}
	else if (CodecType==VIDEC_CODEC_H261)
	{
		m_nPayloadType=31;
	}

	int nBitrateControlType=MPVIDEO_NETWORK_PRIVATE|MPVIDEO_NETWORK_QUALITY;
	m_nBitrate=nBitrate;
	GetCodecInfoWithBitrate(nBitrateControlType,m_nBitrate,m_nQMin,m_nQMax,CodecType,true);

	m_CodecType=CodecType;

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_pEncoderStream=MPVideo_EncoderStreamCreate(*this,true);
	if (m_pEncoderStream==NULL)
	{
		return -1;
	}

	return 0;
}


void XCapChanRTP::Close(void)
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

int XCapChanRTP::RequestKeyFrame(void)
{
	if (m_pEncoderStream!=NULL)
	{
		m_pEncoderStream->RequestKeyFrame();
	}
	return 0;
}


void XCapChanRTP::OnAttachedToImageSource(MPVideo_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XCapChanRTP::OnDetachedFromImageSource(MPVideo_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XCapChanRTP::OnReceivedImage(MPVideo_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if ((m_nWidth!=0 && m_nWidth!=pXImage->GetActualWidth()) ||
		(m_nHeight!=0 && m_nHeight!=pXImage->GetActualHeight()) ||
		pXImage->GetDoubleField())
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

		bool bDoubleField=pXImage->GetDoubleField();
		int nSrcYBufferSize = pXImage->GetActualWidth() * pXImage->GetActualHeight();
		int nSrcUVBufferSize = nSrcYBufferSize >> 2;
		unsigned char*src[3];
		src[0]=pData;
		src[1] = src[0] + nSrcYBufferSize;
		src[2] = src[1] + nSrcUVBufferSize;
		if (0!=m_pResize->Resize(src,pXImage->GetActualWidth(),pXImage->GetActualHeight()/(bDoubleField?2:1),m_dst,m_nWidth,m_nHeight))
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
			pXImageNew->SetVirtualWidth(m_nWidth);
			pXImageNew->SetVirtualHeight(m_nHeight);
			pXImageNew->SetEvenFieldFirst(0);
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
			free(m_pDstBuf);
			m_pDstBuf=NULL;
		}

		if (m_pEncoderStream!=NULL)
		{
			m_pEncoderStream->EncodeFrame(pImage,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,m_CodecType);
		}
	}
}


void XCapChanRTP::OnMPVideo_EncoderStreamCallbackFramePacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	bool bKeyFrame=VIDEC_HEADER_EXT_GET_KEYFRAME(pPacketData)!=0;
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pPacketData);
	SendStreamData((unsigned char*)pPacketData,nPacketSize,nWidth,nHeight,nHeaderLen);
}

void XCapChanRTP::OnMPVideo_EncoderStreamCallbackRTPPacket(void*pPacketData,int nPacketSize,int nWidth,int nHeight)
{
	PRTP_HEADER pRTPHeader=(PRTP_HEADER)pPacketData;
	pRTPHeader->type=m_nPayloadType;
	m_rCallback.OnMPVideo_CapChanRTPCallbackPacketData(this,pPacketData,nPacketSize);
}

int XCapChanRTP::AddStreamSink(MPVideo_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XCapChanRTP::RemoveStreamSink(MPVideo_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}
