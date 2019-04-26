#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameI264.h"
#include "XUtil.h"


void IntelEncoderCB(const void*pUserData,unsigned char*pData,int nLen,int nKeyFrame)
{
	XEncoderFrameI264*pXEncoderFrameI264=(XEncoderFrameI264*)pUserData;
	if (pXEncoderFrameI264)
	{
		pXEncoderFrameI264->OnIntelH264EncoderCallback(pData,nLen,nKeyFrame);
	}
}



XEncoderFrameI264::XEncoderFrameI264(MPVideo_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType)
:XEncoderFrame(rCallback,bRTPPacket)

,m_pH264RTPFrame(NULL)

,m_nCodecType(nCodecType)
{
	m_pEncoder=NULL;
}

XEncoderFrameI264::~XEncoderFrameI264(void)
{
}

int XEncoderFrameI264::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
{
	if (nBitrateControlType==MPVIDEO_NETWORK_PRIVATE)
	{
		nMinQP=16;
	}
	else if (nBitrateControlType==MPVIDEO_NETWORK_SATELLITE)
	{
		nMinQP=16;
		nMaxQP=51;
		nBitrate*=0.75;
	}
	else if (nBitrateControlType==MPVIDEO_NETWORK_3G)
	{
		nMinQP=16;
		nMaxQP=50;
		nBitrate*=0.75;
	}
	else if (nBitrateControlType==MPVIDEO_NETWORK_LAN)
	{
		nBitrate+=(nBitrate>>1);
		if (m_nWidth>704)
		{
			if (nMinQP<nMaxQP)
			{
				nMinQP+=1;
			}
		}
	}
	else //MPVIDEO_NETWORK_INTERNET
	{
		//nBitrate+=(nBitrate>>1);
		if (m_nWidth>704)
		{
			if (nMinQP<nMaxQP)
			{
				nMinQP+=1;
			}
		}
	}

	int nRet=XEncoderFrame::Open(nWidth,nHeight,nFrameRate,bDoubleField, nBitrate, nMinQP, nMaxQP, nBitrateControlType);
	if (nRet!=0)
	{
		return nRet;
	}

	int nThreadCount=4;
	if (nFrameRate<=15 || (m_nWidth<640 && m_nHeight<480))
		nThreadCount=1;

	if (nFrameRate==15 && m_nWidth==1920 && m_nHeight==1080)
		nThreadCount=2;

	int nMaxKeyFrameInterval=MPVIDEO_GetMaxKeyFrameInterval();
	if (m_nCodecType==VIDEC_CODEC_H264)
	{
		nMaxKeyFrameInterval=8000;
	}

	if (m_bRTPPacket)
	{
		 nBitrate=(m_nBitrate>0?m_nBitrate:6144);
		 {
			 nBitrate+=(nBitrate>>1);
		 }
		m_pH264RTPFrame=new H264RTPFrame(*this,nBitrate);
		m_pH264RTPFrame->Open(97,1400);
		m_pH264RTPFrame->RunRTPThread();
	}

	m_pEncoder=IntelEncoderOpen(nWidth,nHeight,nFrameRate,m_nCodecType==VIDEC_CODEC_H264?INL_BASELINE_PROFILE:INL_HIGH_PROFILE,nBitrate,nMinQP,nMaxQP,nMaxKeyFrameInterval,IntelEncoderCB,this);

	return (m_pEncoder?0:-1);
}

void XEncoderFrameI264::Close(void)
{
	if (m_pEncoder)
	{
		IntelEncoderClose(m_pEncoder);  
		m_pEncoder=NULL;
	}

	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->Close();
		delete m_pH264RTPFrame;
		m_pH264RTPFrame=NULL;
	}

	XEncoderFrame::Close();
}

bool XEncoderFrameI264::DoRTPPacket(void)
{
	return false;
}

int XEncoderFrameI264::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_pEncoder)
	{
		IntelEncoderEncoding(m_pEncoder,pData[0],0,nKeyFrame);

		return 0;
	}

	return -1;
}

unsigned char XEncoderFrameI264::GetCodecType(void)
{
	return m_nCodecType;
}

void XEncoderFrameI264::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnMPVideo_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}

void XEncoderFrameI264::OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
{
	int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
	unsigned char*pPacketData=m_pPacketBuf;
	memcpy(pPacketData+nHeaderLen,pData,nLen);
	int nPacketLen=nLen;
	OnPacketData(pPacketData,nPacketLen,nKeyFrame,0,m_bHardware,m_nTimestamp);

	//char szDebug[1024];
	//sprintf(szDebug,"nLen=%d nKeyFrame=%d\n",nLen,nKeyFrame);
	//OutputDebugString(szDebug);

	if (m_pH264RTPFrame!=NULL && nPacketLen>0)
	{
		m_pH264RTPFrame->SendFrame(pPacketData,nPacketLen,m_nWidth,m_nHeight,nKeyFrame,XGetTimestamp());
	}
}