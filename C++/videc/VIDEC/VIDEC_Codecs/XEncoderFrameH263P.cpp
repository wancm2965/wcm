#include <stdlib.h>
#include <memory.h>

#include "XEncoderFrameH263P.h"
//#include "ravevc.h"
#include "XUtil.h"
//#include "VIDEC_avc.h"
#include "avc.h"

XEncoderFrameH263P::XEncoderFrameH263P(VIDEC_EncoderCallback&rCallback,bool bRTPPacket)
:XEncoderFrame(rCallback,bRTPPacket)
,m_hEncoder(NULL)
,m_pH263PRTPFrame(NULL)
{
}

XEncoderFrameH263P::~XEncoderFrameH263P(void)
{
}

int XEncoderFrameH263P::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
{
	if (nBitrateControlType==VIDEC_NETWORK_PRIVATE ||
		nBitrateControlType==VIDEC_NETWORK_SATELLITE ||
		nBitrateControlType==VIDEC_NETWORK_3G)
	{
		nMinQP=2;
	}
	else if (nBitrateControlType==VIDEC_NETWORK_LAN)
	{
		//nMaxQP=nMinQP;
		//nBitrate=0;
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

	int nRet=XEncoderFrame::Open(nWidth,nHeight,nFrameRate,bDoubleField,nBitrate,nMinQP,nMaxQP,nBitrateControlType);
	if (nRet!=0)
	{
		return nRet;
	}

	m_hEncoder=avc_encoder_open(AVC_CODEC_H263P,m_nWidth,m_nHeight,1280,NULL,0);
	if (m_hEncoder==NULL)
	{
		return -1;
	}

	if (m_bRTPPacket)
	{
		NEWRECORD( m_pH263PRTPFrame ,new H263PRTPFrame(*this,(nBitrate>0?nBitrate:6144)) )
		NULLReturnRet( m_pH263PRTPFrame , -1 )  
		m_pH263PRTPFrame->Open(97,1400);
		m_pH263PRTPFrame->RunRTPThread();
	}


	m_VBRControl.Reset(nMinQP,nMaxQP,nBitrate);

	return 0;
}

void XEncoderFrameH263P::Close(void)
{
	if (m_hEncoder!=NULL)
	{
		avc_encoder_close(m_hEncoder);
		m_hEncoder=NULL;
	}

	XEncoderFrame::Close();

	if (m_pH263PRTPFrame!=NULL)
	{
		m_pH263PRTPFrame->Close();
		delete m_pH263PRTPFrame;
		m_pH263PRTPFrame=NULL;
	}
}

bool XEncoderFrameH263P::DoRTPPacket(void)
{
	return false;
}

int XEncoderFrameH263P::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_hEncoder==NULL)
	{
		return -1;
	}

	int nCanEncThisFrame=1;
	int nQuant=m_nMinQP;
	if (m_nBitrateControlType!=VIDEC_NETWORK_LAN)
	{
		nQuant=m_VBRControl.GetNextQP(nCanEncThisFrame);
// 		if (!nCanEncThisFrame)
// 			return -1;
	}

	int nTempBufLen=nPacketLen;
	try
	{
		nPacketLen=avc_encoder_encode(m_hEncoder,pData,pPacketData,nPacketLen,nQuant,&nKeyFrame);
		if (nKeyFrame)
			nKeyFrame=-1;
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
	else if (nPacketLen>0)
	{
		m_VBRControl.Calculate(nPacketLen);
	}

	if (m_pH263PRTPFrame!=NULL && nPacketLen>0)
	{
		m_pH263PRTPFrame->SendFrame(pPacketData,nPacketLen,m_nWidth,m_nHeight,nKeyFrame,XGetTimestamp());
	}


	return 0;
}


unsigned char XEncoderFrameH263P::GetCodecType(void)
{
	return VIDEC_CODEC_H263P;
}


void XEncoderFrameH263P::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnVIDEC_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}