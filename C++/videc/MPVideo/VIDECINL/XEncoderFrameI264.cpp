#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

#include "XEncoderFrameI264.h"
#include <map>


static bool g_bIntelH264EncoderInitialized=false;
static bool g_bIntelH264EncoderHardware=false;

typedef std::map<void*,XEncoderFrameI264*>	MapXEncoderFrameI264;

static MapXEncoderFrameI264 g_MapXEncoderFrameI264;
static unsigned int g_nMaxXEncoderFrameI264Count=0;

int IntelH264HardwareEncoderTry(unsigned long nWidth,unsigned long nHeight)
{
	if (!g_bIntelH264EncoderInitialized)
	{
		g_bIntelH264EncoderInitialized=true;
		mfxSession tSession;
		mfxIMPL impl = MFX_IMPL_HARDWARE;
		mfxVersion ver = {0, 1};
		mfxStatus nStatus=MFXInit(MFX_IMPL_HARDWARE,&ver,&tSession);
		MFXClose(tSession);
		if (nStatus!=MFX_ERR_NONE)
		{
			return -1;
		}
		g_bIntelH264EncoderHardware=true;
	}

	if (g_bIntelH264EncoderHardware)
	{
		if (nWidth==0 && nHeight==0)
		{
			return 0;
		}

		if (g_MapXEncoderFrameI264.size()<g_nMaxXEncoderFrameI264Count)
		{
			return 0;
		}
	}
	return -1;
}

void IntelH264HardwareEncoderSetMaxCount(unsigned int nMaxCount)
{
	g_nMaxXEncoderFrameI264Count=nMaxCount;
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

	g_MapXEncoderFrameI264[this]=this;
}

XEncoderFrameI264::~XEncoderFrameI264(void)
{
	g_MapXEncoderFrameI264.erase(this);
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

	m_pEncodingPipeLine=new CEncodingPipeline(*this);
	sInputParams tInputParams;   // input parameters from command line
	MSDK_ZERO_MEMORY(tInputParams);
	tInputParams.CodecId = MFX_CODEC_AVC;
	tInputParams.CodecProfile = (nProfile==INL_BASELINE_PROFILE?MFX_PROFILE_AVC_BASELINE:MFX_PROFILE_AVC_HIGH);


	if (tInputParams.CodecProfile==MFX_PROFILE_AVC_BASELINE)
	{
		if (nBitrate<=64)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_1;
		}
		else if (nBitrate<=192)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_11;
		}
		else if (nBitrate<=768)
		{
			tInputParams.CodecLevel=MFX_LEVEL_AVC_12;
		}
		else 
		{
			if (nWidth<=352)
			{
				tInputParams.CodecLevel=MFX_LEVEL_AVC_2;
			}
			else
			{
				tInputParams.CodecLevel=MFX_LEVEL_AVC_22;
			}
		}
	}
	else if (tInputParams.CodecProfile==MFX_PROFILE_AVC_HIGH)
	{
		if (nWidth>720)
		{
			tInputParams.CodecLevel = MFX_LEVEL_AVC_41;
		}
		else
		{
			tInputParams.CodecLevel = MFX_LEVEL_AVC_31;
		}
	}

	tInputParams.bUseHWLib = true;
	tInputParams.ColorFormat = MFX_FOURCC_NV12;
	tInputParams.nPicStruct = MFX_PICSTRUCT_FIELD_BFF;
#ifdef D3D_SURFACES_SUPPORT
	tInputParams.bd3dAlloc = true;
#endif

	//tInputParams.nTargetUsage = MFX_TARGETUSAGE_BALANCED;
	//tInputParams.nTargetUsage = MFX_TARGETUSAGE_BEST_QUALITY;
	tInputParams.nTargetUsage = MFX_TARGETUSAGE_BEST_SPEED;
	tInputParams.nWidth = nWidth;
	tInputParams.nHeight = nHeight;
	tInputParams.dFrameRate = (mfxF64)nFrameRate;
	tInputParams.nBitRate =  nBitrate;
	tInputParams.numViews = 1;


	tInputParams.ColorFormat = MFX_FOURCC_YV12;
	tInputParams.nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;

	if (MFX_ERR_NONE != m_pEncodingPipeLine->Init(&tInputParams))
	{
		g_bIntelH264EncoderHardware=false;
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


int XEncoderFrameI264::Encode(unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	if (m_pEncodingPipeLine)
	{
		unsigned char*pYUV[3];
		pYUV[0]=pData;
		pYUV[1]=pData+m_nY;
		pYUV[2]=pYUV[1]+m_nUV;

		mfxStatus sts = m_pEncodingPipeLine->Run(pYUV,nRequestKeyFrame);
		if (MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts)
		{            
			sts = m_pEncodingPipeLine->ResetDevice();
			if (sts != MFX_ERR_NONE)
			{
				return -1;
			}

			sts = m_pEncodingPipeLine->ResetMFXComponents();
			if (sts != MFX_ERR_NONE)
			{
				return -1;
			}
		}        
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