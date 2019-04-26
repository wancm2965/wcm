#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameI264.h"


#include "../VIDEC_FRE/VIDECFREAPI.h"




//#include "XUtil.h"
#ifdef _WIN32
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif


unsigned long XGetTimestamp(void)
{
#ifdef WIN32
	return ::timeGetTime();//ºÁÃë
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec/1000; 
#endif
}



void IntelH264HardwareEncoderSetMaxCount(unsigned int nMaxCount)
{
	//g_nMaxXEncoderFrameI264Count=nMaxCount;
}


XEncoderFrameI264::XEncoderFrameI264(void)
{
	m_pEncodingPipeLine=NULL;
	m_encoder_cb=NULL;
	m_pUserData=NULL;
	m_nWidth=0;
	m_nHeight=0;
	m_nY=0;
	m_nUV=0;
	m_nLastResetIMSDKTimestamp =XGetTimestamp();
}

XEncoderFrameI264::~XEncoderFrameI264(void)
{
	
}

int XEncoderFrameI264::Open(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData)
{
	if (encoder_cb==NULL)
		return -1;

	m_encoder_cb=encoder_cb;
	m_pUserData=pUserData;

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nY=nWidth*nHeight;
	m_nUV=m_nY/4;

	NEWRECORD( m_pEncodingPipeLine , new CEncodingPipeline(*this) )
	sInputParams tInputParams;   // input parameters from command line
	MSDK_ZERO_MEMORY(tInputParams);

	tInputParams.CodecId = MFX_CODEC_AVC;
	tInputParams.bUseHWLib = true;
	tInputParams.isV4L2InputEnabled = false;
	tInputParams.nNumFrames = 0;
	tInputParams.nWidth = nWidth;
	tInputParams.nHeight = nHeight;
	tInputParams.nDstWidth = nWidth;
	tInputParams.nDstHeight = nHeight;
	tInputParams.nBitRate = nBitrate;
	tInputParams.dFrameRate = nFrameRate;
	tInputParams.nTargetUsage = MFX_TARGETUSAGE_BALANCED;
	tInputParams.ColorFormat = MFX_FOURCC_YV12;
	tInputParams.nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;
	tInputParams.nAsyncDepth = 4;
	tInputParams.nRateControlMethod = MFX_RATECONTROL_CBR;

	if (MFX_ERR_NONE != m_pEncodingPipeLine->Init(&tInputParams))
	{
		
		return -1;
	}

	return 0;
}

void XEncoderFrameI264::Close(void)
{
	if (m_pEncodingPipeLine)
	{
		m_pEncodingPipeLine->Close();  
		delete m_pEncodingPipeLine;
		m_pEncodingPipeLine=NULL;
	}
}

#define  ITELMSDKSTAMP    1000 * 60 * 10 

int XEncoderFrameI264::Encode(unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	if (m_pEncodingPipeLine)
	{
		//test
		//unsigned long nTempTimestamp=XGetTimestamp();
		//if (nTempTimestamp- m_nLastResetIMSDKTimestamp >= ITELMSDKSTAMP)
		//{
		//	m_pEncodingPipeLine->Reset("XEncoderFrameI264::Encode Reset");
		//	m_nLastResetIMSDKTimestamp =nTempTimestamp;
		//	return 0;
		//}

		unsigned char*pYUV[3];
		pYUV[0]=pData;
		pYUV[1]=pData+m_nY;
		pYUV[2]=pYUV[1]+m_nUV;

		mfxStatus sts = m_pEncodingPipeLine->Run(pYUV,nRequestKeyFrame);
		if (sts != MFX_ERR_NONE)
		{
			return -1;
		}

		return 0;
	}

	return -1;
}

void XEncoderFrameI264::OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
{
	if (m_encoder_cb)
	{
		m_encoder_cb(m_pUserData,pData,nLen,nKeyFrame);
	}
}