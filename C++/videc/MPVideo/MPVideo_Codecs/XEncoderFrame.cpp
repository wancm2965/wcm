#include <stdlib.h>
#include <memory.h>

#include <VIDEC/VIDEC_Header.h>
#include "XEncoderFrame.h"
#include "XEncoderFrameH263.h"
#include "XEncoderFrameH263P.h"
#include "XEncoderFrameH264.h"
#include "XEncoderFrameI264.h"

#include "XUtil.h"
 
bool TryH264HardwareEncoding(int nWidth,int nHeight)
{
	//if (nWidth>=960 && nHeight>=540)
	{
		return (IntelEncoderTry(nWidth,nHeight)==0?true:false);
	}

	return false;
}

MPVIDEO_API MPVideo_Encoder*MPVideo_EncoderCreate(MPVideo_EncoderCallback&rCallback,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType,bool bRTPPacket)
{
	XEncoderFrame*pXEncoderFrame=NULL;
	bool bHardwareEncoding=false;
	switch (codecType)
	{
	case VIDEC_CODEC_H263:
		{
			pXEncoderFrame=new XEncoderFrameH263(rCallback,bRTPPacket);
		}
		break;
	case VIDEC_CODEC_H263P:
		{
			pXEncoderFrame=new XEncoderFrameH263P(rCallback,bRTPPacket);
		}
		break;
	case VIDEC_CODEC_H264:
	case VIDEC_CODEC_H264_SVC:
	default:
		{
			if (TryH264HardwareEncoding(nWidth,nHeight))
			{
				bHardwareEncoding=true;
				pXEncoderFrame=new XEncoderFrameI264(rCallback,bRTPPacket,codecType);
			}
			else
			{
				pXEncoderFrame=new XEncoderFrameH264(rCallback,bRTPPacket,codecType);
			}
		}
		break;
	}

	if (pXEncoderFrame!=NULL)
	{
		if (0!=pXEncoderFrame->Open(nWidth,nHeight,nFrameRate,bDoubleField,nBitrate,nMinQP,nMaxQP,nBitrateControlType))
		{
			pXEncoderFrame->Close();
			delete pXEncoderFrame;
			pXEncoderFrame=NULL;

			if (bHardwareEncoding)
			{
				pXEncoderFrame=new XEncoderFrameH264(rCallback,bRTPPacket,codecType);
				if (0!=pXEncoderFrame->Open(nWidth,nHeight,nFrameRate,bDoubleField,nBitrate,nMinQP,nMaxQP,nBitrateControlType))
				{
					pXEncoderFrame->Close();
					delete pXEncoderFrame;
					pXEncoderFrame=NULL;
				}
			}
		}
	}
		
	return pXEncoderFrame;
}


XEncoderFrame::XEncoderFrame(MPVideo_EncoderCallback&rCallback,bool bRTPPacket)
:m_rCallback(rCallback)
,m_bRTPPacket(bRTPPacket)

,m_nWidth(0)
,m_nHeight(0)
,m_pPacketBuf(NULL)
,m_nFrameSize(0)
,m_nYImageSize(0)

,m_usSeqNo(0)

,m_nRequestKeyFrame(1)

,m_bDoubleField(false)
,m_bEventFirst(true)

,m_nBitrate(384)
,m_nMinQP(4)
,m_nMaxQP(51)
,m_nBitrateControlType(MPVIDEO_NETWORK_INTERNET)

,m_nFrameRate(30)

,m_nLastKeyFrameTimestamp(XGetTimestamp())
{
	m_nVirtualWidth=0;
	m_nVirtualHeight=0;

	srand(XGetTimestamp());
	m_usSeqNo=rand();

	m_bHardware=false;
	m_nTimestamp=0;
}

XEncoderFrame::~XEncoderFrame(void)
{
}

int XEncoderFrame::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nYImageSize=m_nWidth*m_nHeight;
	m_nFrameSize=m_nYImageSize*3/2;

	m_nStride[0]=m_nWidth;
	m_nStride[1]=m_nWidth>>1;
	m_nStride[2]=m_nWidth>>1;

	m_bDoubleField=bDoubleField;//=false;

	m_nBitrate=nBitrate;
	m_nMinQP=nMinQP;
	m_nMaxQP=nMaxQP;
	m_nFrameRate=nFrameRate;
	m_nBitrateControlType=nBitrateControlType;

	return 0;
}

void XEncoderFrame::Close(void)
{
	if (m_pPacketBuf)
	{
		free(m_pPacketBuf);
		m_pPacketBuf=NULL;
	}
}


int XEncoderFrame::EncodeFrame(void*pFrameData,int nFrameSize,int nVirtualWidth,int nVirtualHeight,bool bEventFieldFirst,bool bHardware,unsigned long nTimestamp)
{
	if (m_pPacketBuf==NULL)
	{
		m_pPacketBuf=(unsigned char*)malloc(m_nFrameSize);
		if (m_pPacketBuf==NULL)
			return -1;
	}

	m_bHardware=bHardware;
	m_nTimestamp=nTimestamp;

	m_bEventFirst=bEventFieldFirst;
	m_nVirtualWidth=nVirtualWidth;
	m_nVirtualHeight=nVirtualHeight;

	unsigned char*pData[3];
	unsigned char*pPacketData=NULL;

	pData[0]=(unsigned char*)pFrameData;
	pData[1]=pData[0]+m_nYImageSize;
	pData[2]=pData[1]+(m_nYImageSize>>2);

	pPacketData=m_pPacketBuf+VIDEC_HEADER_EXT_KEY_FRAME_LEN;
	int nPacketLen=m_nFrameSize;
	int nKeyFrame=0;

	if (m_nRequestKeyFrame)
	{
		m_nRequestKeyFrame=0;
		nKeyFrame=1;

		m_nLastKeyFrameTimestamp=XGetTimestamp();
	}
	else
	{
		unsigned long nTempTimestamp=XGetTimestamp();
		if (nTempTimestamp-m_nLastKeyFrameTimestamp>=MPVIDEO_GetMaxKeyFrameInterval())
		{
			m_nLastKeyFrameTimestamp=nTempTimestamp;
			if (GetCodecType()!=VIDEC_CODEC_H264 &&
				GetCodecType()!=VIDEC_CODEC_H264_SVC)
			{
				nKeyFrame=1;
			}
		}
	}

	int nBFrame=0;
	if (0!=DoEncodeFrame(pData,m_nStride,pPacketData,nPacketLen,nKeyFrame,nBFrame))
	{
		nPacketLen=0;
	}

	if (AsyncEncoding())
	{
		return 0;
	}

	if (nPacketLen>0)
	{
		int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
		pPacketData-=nHeaderLen;

		OnPacketData(pPacketData,nPacketLen,nKeyFrame,nBFrame,bHardware,nTimestamp);

		return 0;
	}

	return -1;
}

void XEncoderFrame::OnPacketData(unsigned char*pPacketData,int nPacketLen,int nKeyFrame,int nBFrame,bool bHardware,unsigned long nTimestamp)
{
	int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;

	VIDEC_HEADER_EXT_RESET(pPacketData);
	VIDEC_HEADER_EXT_SET_CODEC_ID(pPacketData,GetCodecType());
	VIDEC_HEADER_EXT_SET_EVEN_FIELD_FIRST(pPacketData,m_bEventFirst?1:0);
	VIDEC_HEADER_EXT_SET_DOUBLE_FIELD(pPacketData,m_bDoubleField?1:0);
	VIDEC_HEADER_EXT_SET_HARDWARE(pPacketData,bHardware?1:0);
	VIDEC_HEADER_EXT_SET_KEYFRAME(pPacketData,(nKeyFrame!=0?1:0));
	VIDEC_HEADER_EXT_SET_BFRAME(pPacketData,(nBFrame!=0?1:0));

	VIDEC_HEADER_EXT_SET_SEQUENCE(pPacketData,m_usSeqNo++);
	VIDEC_HEADER_EXT_SET_TIMESTAMP(pPacketData,nTimestamp);
	if (nKeyFrame)
	{
		VIDEC_HEADER_EXT_SET_ACTUAL_WIDTH(pPacketData,m_nWidth);
		VIDEC_HEADER_EXT_SET_ACTUAL_HEIGHT(pPacketData,m_nHeight);
		VIDEC_HEADER_EXT_SET_VIRTUAL_WIDTH(pPacketData,m_nVirtualWidth);
		VIDEC_HEADER_EXT_SET_VIRTUAL_HEIGHT(pPacketData,m_nVirtualHeight);
	}


	m_rCallback.OnMPVideo_EncoderCallbackFramePacket(pPacketData,nHeaderLen+nPacketLen);

}

void XEncoderFrame::RequestKeyFrame(void)
{
	m_nRequestKeyFrame=1;
}
