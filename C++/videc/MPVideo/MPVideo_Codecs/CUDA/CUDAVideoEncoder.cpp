#include "CUDAVideoEncoder.h"
#include "CUDAEncWrapper.h"

// NVCUVENC callback function to signal the start of bitstream that is to be encoded
static unsigned char* _stdcall HandleAcquireBitStream(int *pBufferSize, void *pUserData)
{
	CUDAVideoEncoder *pVideoEncoder=(CUDAVideoEncoder*)pUserData;
	if (pVideoEncoder) 
	{
		return pVideoEncoder->AcquireBitStream(pBufferSize);
	} 

	*pBufferSize=0;
	return NULL;
}

//NVCUVENC callback function to signal that the encoded bitstream is ready to be written to file
static void _stdcall HandleReleaseBitStream(int nBytesInBuffer, unsigned char *cb,void *pUserData)
{
	CUDAVideoEncoder *pVideoEncoder=(CUDAVideoEncoder*)pUserData;
	if (pVideoEncoder) 
	{
		pVideoEncoder->ReleaseBitStream(nBytesInBuffer,cb);
	}
}

//NVCUVENC callback function to signal that the encoding operation on the frame has started
static void _stdcall HandleOnBeginFrame(const NVVE_BeginFrameInfo *pbfi, void *pUserData)
{
	int nNextKeyFrame=0;
	if (pbfi->nPicType==NVVE_PIC_TYPE_IFRAME)
	{
		nNextKeyFrame=1;
	}
	CUDAVideoEncoder *pVideoEncoder=(CUDAVideoEncoder*)pUserData;
	if (pVideoEncoder) 
	{
		pVideoEncoder->NextKeyFrame(nNextKeyFrame);
	}
}

//NVCUVENC callback function signals that the encoding operation on the frame has finished
static void _stdcall HandleOnEndFrame(const NVVE_EndFrameInfo *pefi, void *pUserData)
{
}


CUDAVideoEncoder::CUDAVideoEncoder(CUDAVideoEncoderCallback&rCallback)
: m_rCallback(rCallback),
  m_pPacketBuffer(NULL),
  m_pPacketData(NULL)
{
	m_nGPUCount=0;
	m_nGPUID=0;
	m_nWidth=0;
	m_nHeight=0;
	m_nPacketBufferSize=0;
}


CUDAVideoEncoder::~CUDAVideoEncoder()
{ 
}

int
CUDAVideoEncoder::Open(int nWidth,int nHeight, int nFrameRate,int nBitrate, int nCBR, int nQMin,unsigned char nHigh)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_nPacketBufferSize=nWidth*nHeight;
	m_pPacketBuffer = (unsigned char*)malloc(128+m_nPacketBufferSize);
	if (m_pPacketBuffer==NULL)
	{
		return -1;
	}
	m_pPacketData=m_pPacketBuffer+128;


	// Create the Encoder API Interface
    HRESULT hr = cudaenc.NVCreateEncoder(&m_pEncoder);
	if (FAILED(hr))
		return -1;

	hr = cudaenc.NVSetCodec(m_pEncoder, NV_CODEC_TYPE_H264); 
	if (FAILED(hr))
		return -1;

	hr = GetParamValue(NVVE_GET_GPU_COUNT, &m_nGPUCount);   
	if (FAILED(hr))
		return -1;

	hr = SetParamValue(NVVE_FORCE_GPU_SELECTION, &m_nGPUID);  
	if (FAILED(hr))
		return -1;

	int nSize[2];
	nSize[0]=m_nWidth;
	nSize[1]=m_nHeight;

	SetParamValue(NVVE_OUT_SIZE,&nSize);
	SetParamValue(NVVE_IN_SIZE,&nSize);

	unsigned char nProfileLevel=0x0a;
	unsigned char nProfileName=0x42;//Baseline
	if (nHigh==0)
	{
		nProfileName=0x42;//Baseline
		if (nBitrate<=64)
		{
			nProfileLevel=0x0a;	//1.0
		}
		else if (nBitrate<=192)
		{
			nProfileLevel=0x0b;	//1.1
		}
		else if (nBitrate<=768)
		{
			nProfileLevel=0x0c;	//1.2
		}
		else 
		{
			if (nWidth<=352)
			{
				nProfileLevel=0x14;	//2.0
			}
			else
			{
				nProfileLevel=0x16;	//2.2
			}
		}
	}
	else
	{
		nProfileName=0x64;//High
		if (nWidth>720)
		{
			nProfileLevel=0x29;	//4.1
		}
		else
		{
			nProfileLevel=0x1f;	//3.1
		}
	}

	unsigned long nProfileValue=(nProfileLevel<<8)|nProfileName;
	SetParamValue(NVVE_PROFILE_LEVEL,&nProfileValue);

	int anFrameRate[2];
	anFrameRate[0]=nFrameRate*1000;
	anFrameRate[1]=1001;
	SetParamValue(NVVE_FRAME_RATE,&anFrameRate);

	int nPInterval=1;
	SetParamValue(NVVE_P_INTERVAL,&nPInterval);

	int nIDRPeriod=nFrameRate*4;
	SetParamValue(NVVE_IDR_PERIOD,&nIDRPeriod);


	int nForceIntra=0;
	SetParamValue(NVVE_FORCE_INTRA,&nForceIntra);
	int nForceIDR=0;
	SetParamValue(NVVE_FORCE_IDR,&nForceIDR);
	int nClearStat=0;
	SetParamValue(NVVE_CLEAR_STAT,&nClearStat);

	int nDisableCabac=0;
	SetParamValue(NVVE_DISABLE_CABAC,&nDisableCabac);
	int nNaluFramingType=0;
	SetParamValue(NVVE_CONFIGURE_NALU_FRAMING_TYPE,&nNaluFramingType);
	int nDisableSPSPPS=0;
	SetParamValue(NVVE_DISABLE_SPS_PPS,&nDisableSPSPPS);

	int nDeblockMode=1;
	SetParamValue(NVVE_DEBLOCK_MODE,&nDeblockMode);
	int nDeinterlaceMode=0;
	SetParamValue(NVVE_SET_DEINTERLACE,&nDeinterlaceMode);

	NVVE_FIELD_MODE nFieldMode=MODE_FRAME;
	SetParamValue(NVVE_FIELD_ENC_MODE,&nFieldMode);
	int nDynamicGOP=0;
	SetParamValue(NVVE_DYNAMIC_GOP,&nDynamicGOP);

	int nAspectRatio[3];
	nAspectRatio[0]=nWidth;
	nAspectRatio[1]=nHeight;
	nAspectRatio[2]=0;
	SetParamValue(NVVE_ASPECT_RATIO,nAspectRatio);

	NVVE_RateCtrlType nRateCtrlType=RC_CBR;
	if (nCBR)
	{
		nRateCtrlType=RC_CBR;
	}
	else
	{
		nRateCtrlType=RC_VBR_MINQP;
	}
	SetParamValue(NVVE_RC_TYPE,	&nRateCtrlType);

	nBitrate*=1000; 
	SetParamValue(NVVE_AVG_BITRATE,&nBitrate);
	SetParamValue(NVVE_PEAK_BITRATE,&nBitrate);

	int nMinQPIntra=nQMin-3;
	SetParamValue(NVVE_QP_LEVEL_INTRA,&nMinQPIntra);
	SetParamValue(NVVE_QP_LEVEL_INTER_P,&nQMin);
	SetParamValue(NVVE_QP_LEVEL_INTER_B,&nQMin);


	NVVE_GPUOffloadLevel eGPUOffloadLevel=NVVE_GPU_OFFLOAD_ALL;
	NVVE_GPUOffloadLevel eMaxOffloadLevel = NVVE_GPU_OFFLOAD_DEFAULT;

	hr = GetParamValue(NVVE_GPU_OFFLOAD_LEVEL_MAX, &eMaxOffloadLevel);
	if(FAILED(hr))
		return -1;

	if (eGPUOffloadLevel > eMaxOffloadLevel) 
	{
		eGPUOffloadLevel = eMaxOffloadLevel;
	}

	hr = SetParamValue(NVVE_GPU_OFFLOAD_LEVEL, &eGPUOffloadLevel);
	if(FAILED(hr))
		return -1;


	{
		NVVE_CallbackParams sCBParams      = {0};

		memset(&sCBParams,0,sizeof(NVVE_CallbackParams));
		sCBParams.pfnacquirebitstream = HandleAcquireBitStream;
		sCBParams.pfnonbeginframe     = HandleOnBeginFrame;
		sCBParams.pfnonendframe       = HandleOnEndFrame;
		sCBParams.pfnreleasebitstream = HandleReleaseBitStream;

		cudaenc.NVRegisterCB(m_pEncoder, sCBParams, this); //register the callback structure

		HRESULT hr = cudaenc.NVCreateHWEncoder(m_pEncoder);
		if(FAILED(hr))
			return -1;
	}

	return 0;
}


void CUDAVideoEncoder::Close(void)
{
	if (m_pEncoder) 
	{
		cudaenc.NVDestroyEncoder(m_pEncoder);
		m_pEncoder = NULL;
	}


	if (m_pPacketBuffer)
	{
		free(m_pPacketBuffer);
		m_pPacketBuffer=NULL;
	}
	m_pPacketData=NULL;
	m_nPacketBufferSize=0;
}

// If dptr_VideoFrame is != 0, then this is from Device Memory.
// Otherwise we will assume that video is coming from system host memory
bool
CUDAVideoEncoder::EncodeFrame(unsigned char*pFrameData,int nRequestKeyFrame)
{
	HRESULT hr = S_OK;

	NVVE_EncodeFrameParams      efparams;
	efparams.Height           = m_nHeight;
	efparams.Width            = m_nWidth;
	efparams.Pitch            = m_nWidth;
	efparams.PictureStruc     = FRAME_PICTURE; 
	efparams.SurfFmt          = IYUV;
	efparams.progressiveFrame = 0;
	efparams.repeatFirstField = 0;
	efparams.topfieldfirst    = 0;
	efparams.bLast			  = 0;
	efparams.picBuf			  = pFrameData;

	// Copies video frame from system memory, and passes it as a System pointer to the API
	hr = cudaenc.NVEncodeFrame(m_pEncoder, &efparams, (nRequestKeyFrame?0x04:0), NULL);

    if(FAILED(hr)) 
	{
		return false;
    }

	return true;
}


unsigned char* CUDAVideoEncoder::AcquireBitStream(int *pBufferSize)
{
	*pBufferSize = m_nPacketBufferSize;
	return m_pPacketData;
}

void CUDAVideoEncoder::ReleaseBitStream(int nBytesInBuffer, unsigned char *cb)
{
	m_rCallback.OnCUDAVideoEncoderCallback(cb,nBytesInBuffer,m_nNextKeyFrame);
}

HRESULT 
CUDAVideoEncoder::GetParamValue(DWORD dwParamType, void *pData)
{
	HRESULT hr = S_OK;
	hr = cudaenc.NVGetParamValue(m_pEncoder, dwParamType, pData);
	return hr;
}



HRESULT 
CUDAVideoEncoder::SetParamValue(DWORD dwParamType, void *pData)
{
	HRESULT hr = S_OK;
	hr = cudaenc.NVSetParamValue(m_pEncoder, dwParamType, pData);
	return hr;
}
