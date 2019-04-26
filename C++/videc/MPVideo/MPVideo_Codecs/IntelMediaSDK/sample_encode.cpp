//
//               INTEL CORPORATION PROPRIETARY INFORMATION
//  This software is supplied under the terms of a license agreement or
//  nondisclosure agreement with Intel Corporation and may not be copied
//  or disclosed except in accordance with the terms of that agreement.
//        Copyright (c) 2005-2011 Intel Corporation. All Rights Reserved.
//

#include "pipeline_encode.h"


class TestEncoder
	: public IntelH264EncoderCallback
{
public:
	TestEncoder(void)
	{
		m_pEncodingPipeLine=NULL;
	}
	virtual~TestEncoder(void)
	{

	}
public:
	int Open(int nWidth,int nHeight,int nFrameRate,int nBitrate)
	{
		m_pEncodingPipeLine=new CEncodingPipeline(*this);
		sInputParams m_sInputParams;   // input parameters from command line
		MSDK_ZERO_MEMORY(m_sInputParams);
		m_sInputParams.CodecId = MFX_CODEC_AVC;

		m_sInputParams.bUseHWLib = false;
		m_sInputParams.ColorFormat = MFX_FOURCC_NV12;
		m_sInputParams.nPicStruct = MFX_PICSTRUCT_FIELD_TFF;
		m_sInputParams.nPicStruct = MFX_PICSTRUCT_FIELD_BFF;
#ifdef D3D_SURFACES_SUPPORT
		m_sInputParams.bd3dAlloc = true;
#endif

		m_sInputParams.nTargetUsage = MFX_TARGETUSAGE_BALANCED;
		m_sInputParams.nTargetUsage = MFX_TARGETUSAGE_BEST_QUALITY;
		m_sInputParams.nTargetUsage = MFX_TARGETUSAGE_BEST_SPEED;
		m_sInputParams.nWidth = nWidth;
		m_sInputParams.nHeight = nHeight;
		m_sInputParams.dFrameRate = (mfxF64)nFrameRate;
		m_sInputParams.nBitRate =  nBitrate;
		m_sInputParams.numViews = 1;


		m_sInputParams.ColorFormat = MFX_FOURCC_YV12;
		m_sInputParams.nPicStruct = MFX_PICSTRUCT_PROGRESSIVE;

		if (MFX_ERR_NONE != m_pEncodingPipeLine->Init(&m_sInputParams))
		{
			return -1;
		}

		return 0;
	}

	void Close(void)
	{
		if (m_pEncodingPipeLine)
		{
			m_pEncodingPipeLine->Close();  
			delete m_pEncodingPipeLine;
			m_pEncodingPipeLine=NULL;
		}
	}

	int Encode(unsigned char*pYV12Data,int nYV12DataLen,int nRequestKeyFrame)
	{
		if (m_pEncodingPipeLine)
		{
			mfxStatus sts = m_pEncodingPipeLine->Run(pYV12Data,nYV12DataLen,nRequestKeyFrame);
			if (MFX_ERR_DEVICE_LOST == sts || MFX_ERR_DEVICE_FAILED == sts)
			{            
				sts = m_pEncodingPipeLine->ResetDevice();
				if (sts != MFX_ERR_NONE)
				{
					return -1;
				}

				sts = m_pEncodingPipeLine->ResetMFXComponents(&m_sInputParams);
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
	
	void OnIntelH264EncoderCallback(unsigned char*pData,int nLen,int nKeyFrame)
	{
	}
protected:
	CEncodingPipeline*	m_pEncodingPipeLine;
	sInputParams		m_sInputParams;
};
