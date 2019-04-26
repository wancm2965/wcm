#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameH264.h"
#include "XUtil.h"


#include "../AVCONAVC/AVCONAVC.h"
//#include "../VIDEC264/VIDEC_264.h"

#include "../VIDEC_FRE/VIDECFREAPI.h"


XEncoderFrameH264::XEncoderFrameH264(VIDEC_EncoderCallback&rCallback,bool bRTPPacket,VIDEC_CODEC_TYPE nCodecType)
:XEncoderFrame(rCallback,bRTPPacket)
,m_hEncoder(NULL)

,m_pH264RTPFrame(NULL)

,m_nCodecType(nCodecType)

,m_bMobile(false)
{
	m_nLastKeyFrameCount=0;

#ifdef REC_H264_YUV
	m_pFileYUV=NULL;
	m_pFile264=NULL;
#endif
}

XEncoderFrameH264::~XEncoderFrameH264(void)
{
}

int XEncoderFrameH264::Open(int nWidth,int nHeight,int nFrameRate,bool bDoubleField,int nBitrate,int nMinQP,int nMaxQP,int nBitrateControlType)
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

	int nMaxKeyFrameInterval=VIDEC_GetMaxKeyFrameInterval();
	if (m_bRTPPacket && m_nCodecType==VIDEC_CODEC_H264 && m_nWidth<=352)
	{
		m_bMobile=true;
	}
	else
	{
		m_bMobile=false;
	}
	m_bMobile=false;

#ifdef __VIDEC_264_H__
	if (m_bMobile)
	{
		m_hEncoder=VIDEC264_EncoderOpen(VIDEC_H264_CODEC_H264,m_nWidth,m_nHeight,m_nFrameRate,nBitrate);
	}
	else
#endif
	{
		m_hEncoder=AVCONAVC_EncoderOpen(m_nWidth,m_nHeight,nFrameRate,m_nCodecType==VIDEC_CODEC_H264?AVC_BASELINE_PROFILE:AVC_HIGH_PROFILE,m_bRTPPacket?1280:0,nThreadCount,nBitrate,m_nMinQP,m_nMaxQP,nFrameRate*nMaxKeyFrameInterval/1000);
	}

	if (m_hEncoder==NULL)
	{
		return -1;
	}

	if (m_bRTPPacket)
	{
		 nBitrate=(m_nBitrate>0?m_nBitrate:6144);
		 {
			 nBitrate+=(nBitrate>>1);
		 }
		NEWRECORD( m_pH264RTPFrame,new H264RTPFrame(*this,nBitrate) )
		NULLReturnRet( m_pH264RTPFrame , -1 )  
		m_pH264RTPFrame->Open(97,1400);
		m_pH264RTPFrame->RunRTPThread();
	}

#ifdef REC_H264_YUV
	//m_pFileYUV=fopen("d:\\1080I.yuv","w+b");
	m_pFile264=fopen("d:\\1080I.264","w+b");
#endif

	return 0;
}

void XEncoderFrameH264::Close(void)
{
	if (m_hEncoder!=NULL)
	{
#ifdef __VIDEC_264_H__
		if (m_bMobile)
			VIDEC264_EncoderClose(m_hEncoder);
		else
#endif
			AVCONAVC_EncoderClose(m_hEncoder);
		m_hEncoder=NULL;
	}

	if (m_pH264RTPFrame!=NULL)
	{
		m_pH264RTPFrame->Close();
		delete m_pH264RTPFrame;
		m_pH264RTPFrame=NULL;
	}

	XEncoderFrame::Close();

#ifdef REC_H264_YUV
	if (m_pFileYUV!=NULL)
	{
		fclose(m_pFileYUV);
		m_pFileYUV=NULL;
	}
	if (m_pFile264!=NULL)
	{
		fclose(m_pFile264);
		m_pFile264=NULL;
	}
#endif
}

bool XEncoderFrameH264::DoRTPPacket(void)
{
	return false;
}

int XEncoderFrameH264::DoEncodeFrame(unsigned char*pData[],int nStride[],unsigned char*pPacketData,int&nPacketLen,int&nKeyFrame,int&nBFrame)
{
	if (m_hEncoder==NULL)
	{
		return -1;
	}

#ifdef REC_H264_YUV
	if (m_pFileYUV!=NULL)
	{
		fwrite(pData[0],1,m_nWidth*m_nHeight,m_pFileYUV);
		fwrite(pData[1],1,(m_nWidth*m_nHeight>>2),m_pFileYUV);
		fwrite(pData[2],1,(m_nWidth*m_nHeight>>2),m_pFileYUV);
	}
#endif

	if( nKeyFrame )  DebugOutput( logsoftenc ,"H264 Encoder  RequestKeyFrame.\n");

	int nTempBufLen=nPacketLen;
	try
	{
		int nLastQP=0;
#ifdef __VIDEC_264_H__
		if (m_bMobile)
			nPacketLen=VIDEC264_EncoderEncode(m_hEncoder,pData,pPacketData,nPacketLen,&nKeyFrame);
		else
#endif
			nPacketLen=AVCONAVC_EncoderEncode(m_hEncoder,pData,pPacketData,nPacketLen,&nKeyFrame,&nBFrame,&nLastQP);

		if (nKeyFrame!=0)
		{
			m_nLastKeyFrameCount=0;
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

	if (m_pH264RTPFrame!=NULL && nPacketLen>0)
	{
		int nPacketLen1=nPacketLen;
		m_pH264RTPFrame->SendFrame(pPacketData,nPacketLen,m_nWidth,m_nHeight,nKeyFrame,XGetTimestamp());

		//nPacketLen=m_pH264RTPFrame->GetFrame(pPacketData,nTempBufLen);

		//char szDebug[1024]="";
		//sprintf(szDebug,"nPacketLen1=%d nPacketLen=%d\n",nPacketLen1,nPacketLen);
		//DebugOutput(szDebug);
#ifdef REC_H264_YUV
		if (m_pFile264!=NULL)
		{
			fwrite(&nPacketLen,1,4,m_pFile264);
			fwrite(pPacketData,1,nPacketLen,m_pFile264);
		}
#endif
	}

	return 0;
}

unsigned char XEncoderFrameH264::GetCodecType(void)
{
	return m_nCodecType;
}

void XEncoderFrameH264::OnBaseRTPFrameCallbackRTPPacket(void*pPacketData,int nPacketLen)
{
	m_rCallback.OnVIDEC_EncoderCallbackRTPPacket(pPacketData,nPacketLen);
}
