#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameN264.h"
#include "XUtil.h"
#include <map>
#include "./CUDA/CUDAEncWrapper.h"


static bool g_bCUDAH264EncoderInitialized=false;
static bool g_bCUDAH264EncoderHardware=false;

typedef std::map<void*,XEncoderFrameN264*>	MapXEncoderFrameN264;

static MapXEncoderFrameN264 g_MapXEncoderFrameN264;
static unsigned int g_nMaxXEncoderFrameN264Count=1;

bool TryCUDAH264HardwareEncoding(void)
{
	if (!g_bCUDAH264EncoderInitialized)
	{
		if (CUDAEncoderOK() && 0)
		{
			g_bCUDAH264EncoderHardware=true;
		}
		else
		{
			g_bCUDAH264EncoderHardware=false;
		}
	}

	if (g_bCUDAH264EncoderHardware)
	{
		if (g_MapXEncoderFrameN264.size()<g_nMaxXEncoderFrameN264Count)
		{
			return true;
		}
	}
	return false;;
}


XEncoderFrameN264::XEncoderFrameN264(MPVideo_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType)
:XEncoderFrame(rCallback,bRTPPacket)

,m_pH264RTPFrame(NULL)

,m_nCodecType(nCodecType)
{
	m_pCUDAVideoEncoder=NULL;

	g_MapXEncoderFrameN264[this]=this;
}

XEncoderFrameN264::~XEncoderFrameN264(void)
{
	g_MapXEncoderFrameN264.erase(this);
}

int XEncoderFrameN264::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
{
	int nCBR=1;
	if (nBitrateControlType==MPVIDEO_NETWORK_PRIVATE)
	{
		nMinQP=16;
		nCBR=0;
	}
	else if (nBitrateControlType==MPVIDEO_NETWORK_SATELLITE)
	{
		nMinQP=16;
		nMaxQP=51;
		nBitrate*=0.75;
		nCBR=1;
	}
	else if (nBitrateControlType==MPVIDEO_NETWORK_3G)
	{
		nMinQP=16;
		nMaxQP=50;
		nBitrate*=0.75;
		nCBR=1;
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
		nCBR=0;
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
		nCBR=0;
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

	m_pCUDAVideoEncoder=new CUDAVideoEncoder(*this);
	if (0 != m_pCUDAVideoEncoder->Open(nWidth,nHeight,nFrameRate,nBitrate,nCBR,nMinQP,(m_nCodecType==VIDEC_CODEC_H264?0:1)))
	{
		g_bCUDAH264EncoderHardware=false;
		return -1;
	}

	return 0;
}

void XEncoderFrameN264::Close(void)
{
	if (m_pCUDAVideoEncoder)
	{
		m_pCUDAVideoEncoder->Close();  
		delete m_pCUDAVideoEncoder;
		m_pCUDAVideoEncoder=NULL;
	}

	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->Close();
		delete m_pH264RTPFrame;
		m_pH264RTPFrame=NULL;
	}

	XEncoderFrame::Close();
}

bool XEncoderFrameN264::DoRTPPacket(void)
{
	return false;
}

int XEncoderFrameN264::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_pCUDAVideoEncoder)
	{
		if (m_pCUDAVideoEncoder->EncodeFrame(pData[0],nKeyFrame))
		{
			return 0;
		}
	}

	return -1;
}

unsigned char XEncoderFrameN264::GetCodecType(void)
{
	return m_nCodecType;
}

void XEncoderFrameN264::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnMPVideo_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}

void XEncoderFrameN264::OnCUDAVideoEncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
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