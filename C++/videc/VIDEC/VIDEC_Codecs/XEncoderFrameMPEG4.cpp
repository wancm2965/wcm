#include <stdlib.h>
#include <memory.h>
#include <assert.h>

#include "XEncoderFrameMPEG4.h"
#include "../AVCONAVC/AVCONAVC.h"



XEncoderFrameMPEG4::XEncoderFrameMPEG4(VIDEC_EncoderCallback&rCallback,bool bRTPPacket)
:XEncoderFrame(rCallback,bRTPPacket)
,m_hEncoder(NULL)
{
}

XEncoderFrameMPEG4::~XEncoderFrameMPEG4(void)
{
}

int XEncoderFrameMPEG4::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
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
		if (nWidth>704)
		{
			if (nMinQP<nMaxQP)
			{
				nMinQP+=1;
			}
		}
	}

	int nRet=XEncoderFrame::Open(nWidth,nHeight,nFrameRate,bDoubleField,nBitrate,nMinQP,nMaxQP,nBitrateControlType);
	if (nRet!=0)
	{
		return nRet;
	}

	int nThreadCount=2;
	m_hEncoder=AVCONAVC_EncoderOpen(nWidth,nHeight,nFrameRate,100,0,nThreadCount,nBitrate,nMinQP,nMaxQP,nFrameRate*300);
	
	if (m_hEncoder==NULL)
	{
		return -1;
	}

	return 0;
}

void XEncoderFrameMPEG4::Close(void)
{
	if (m_hEncoder!=NULL)
	{
		AVCONAVC_EncoderClose(m_hEncoder);
		m_hEncoder=NULL;
	}


	XEncoderFrame::Close();
}

bool XEncoderFrameMPEG4::DoRTPPacket(void)
{
	return false;
}

int XEncoderFrameMPEG4::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_hEncoder==NULL)
	{
		return -1;
	}

	try
	{
		int nLastQP=99;
		nPacketLen=AVCONAVC_EncoderEncode(m_hEncoder,pData,pPacketData,nPacketLen,&nKeyFrame,&nBFrame,&nLastQP);

		if (nKeyFrame!=0)
		{
			nKeyFrame=-1;
		}
	}
	catch (...)
	{
		return -1;
	}

	if (nPacketLen<0)
	{
		nPacketLen=0;
		return -1;
	}
	return 0;
}


unsigned char XEncoderFrameMPEG4::GetCodecType(void)
{
	return VIDEC_CODEC_MPEG4;
}

