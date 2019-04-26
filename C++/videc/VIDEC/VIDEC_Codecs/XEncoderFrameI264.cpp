#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameI264.h"
#include "XUtil.h"
#include "AVCONH264Header.h"


void IntelEncoderCB(const void*pUserData,unsigned char*pData,int nLen,int nKeyFrame)
{
	XEncoderFrameI264*pXEncoderFrameI264=(XEncoderFrameI264*)pUserData;
	if (pXEncoderFrameI264)
	{
		pXEncoderFrameI264->OnIntelH264EncoderCallback(pData,nLen,nKeyFrame);
	}
}



XEncoderFrameI264::XEncoderFrameI264(VIDEC_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType)
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
	if (nBitrateControlType==VIDEC_NETWORK_PRIVATE)
	{
		nMinQP=16;
	}
	else if (nBitrateControlType==VIDEC_NETWORK_SATELLITE)
	{
		nMinQP=16;
		nMaxQP=51;
		nBitrate*=0.75;
	}
	else if (nBitrateControlType==VIDEC_NETWORK_3G)
	{
		nMinQP=16;
		nMaxQP=50;
		nBitrate*=0.75;
	}
	else if (nBitrateControlType==VIDEC_NETWORK_LAN)
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
	else //VIDEC_NETWORK_INTERNET
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

	int nMaxKeyFrameInterval=4000;//VIDEC_GetMaxKeyFrameInterval();
	if (m_bRTPPacket)
	{
		 nBitrate=(m_nBitrate>0?m_nBitrate:6144);
		 {
			 nBitrate+=(nBitrate>>1);
		 }
		NEWRECORD( m_pH264RTPFrame ,new H264RTPFrame(*this,nBitrate) )
		NULLReturnRet( m_pH264RTPFrame , -1 )  
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
		return IntelEncoderEncoding(m_pEncoder,pData[0],0,nKeyFrame);
	}

	return -1;
}

unsigned char XEncoderFrameI264::GetCodecType(void)
{
	return m_nCodecType;
}

void XEncoderFrameI264::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnVIDEC_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}

void XEncoderFrameI264::OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
{
	int nHeaderLen=(nKeyFrame!=0)?VIDEC_HEADER_EXT_KEY_FRAME_LEN:VIDEC_HEADER_EXT_LEN;
	unsigned char*pPacketData=m_pPacketBuf;

	if (nKeyFrame && m_bHardware)
	{
		unsigned char*pFrameHeader=(unsigned char*)pPacketData+nHeaderLen;
		memset(pFrameHeader,0,4);
		PAVCON_H264_HEADER pAVCONH264Header=(PAVCON_H264_HEADER)pFrameHeader;
		pAVCONH264Header->ID=0xFFFF;
		pAVCONH264Header->framerate=m_nFrameRate;
		nHeaderLen+=4;
	}

	memcpy(pPacketData+nHeaderLen,pData,nLen);
	int nPacketLen=nLen+nHeaderLen;
	OnPacketData(pPacketData,nLen,nKeyFrame,0,m_bHardware,m_nTimestamp);		//由于软编中是在OnPacketData中设置头大小，为了保证软硬流程一致，这里只需填入裸数据长度即可

	//char szDebug[1024];
	//sprintf(szDebug,"nLen=%d nKeyFrame=%d\n",nLen,nKeyFrame);
	//DebugOutput(szDebug);

	if (m_pH264RTPFrame!=NULL && nPacketLen>0)
	{
		m_pH264RTPFrame->SendFrame(pData,nLen,m_nWidth,m_nHeight,nKeyFrame,XGetTimestamp());
	}
}