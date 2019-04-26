

#include "stdafx.h"
#include "CUVideoDecoder.h"
#include <map>

class DevDecoderMgr
{
public:
	DevDecoderMgr(void)
	{
		m_nDevCount=0;
	}

	virtual~DevDecoderMgr(void)
	{
	}
public:
	void SetDevCount(unsigned int nDevCount)
	{
		m_nDevCount=nDevCount;
	}
	int GetBestDevID(void)
	{
		if (m_nDevCount<=1)
		{
			return 0;
		}

		if (m_MapCUVideoDecoder[0].size()<=m_MapCUVideoDecoder[1].size())
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	void AddDecoder(CUVideoDecoder*pCUVideoDecoder)
	{
		if (pCUVideoDecoder)
		{
			int nDevID=pCUVideoDecoder->GetDevID();
			if (nDevID>=0 && nDevID<2)
			{
				m_MapCUVideoDecoder[nDevID][pCUVideoDecoder]=pCUVideoDecoder;
			}
		}
	}
	void RemoveDecoder(CUVideoDecoder*pCUVideoDecoder)
	{
		if (pCUVideoDecoder)
		{
			int nDevID=pCUVideoDecoder->GetDevID();
			if (nDevID>=0 && nDevID<2)
			{
				m_MapCUVideoDecoder[nDevID].erase(pCUVideoDecoder);
			}
		}
	}
protected:
	typedef std::map<void*,CUVideoDecoder*>	MapCUVideoDecoder;

	MapCUVideoDecoder	m_MapCUVideoDecoder[2];
	unsigned int		m_nDevCount;
};

static DevDecoderMgr g_cudaDevDecoderMgr;

extern void AddIntoHWVideoDecoderMap(HWVideoDecoder*pHWVideoDecoder);
extern void RemoveFromHWVideoDecoderMap(HWVideoDecoder*pHWVideoDecoder);


////////////////////////////////////////////////////////////////////////////////
// CUVID decoder implementation
////////////////////////////////////////////////////////////////////////////////

CUVideoDecoder::CUVideoDecoder(HWVideoDecoderCallback&rCallback)
	: m_rCallback(rCallback)
	, m_cudaContext(0), m_cudaCtxLock(0)
	, m_hParser(0), m_hDecoder(0), m_hStream(0)
	, m_bForceSequenceUpdate(FALSE)

	, m_bFormatIncompatible(FALSE)
	, m_bWaitForKeyframe(FALSE)
	, m_nWidth(0)
	, m_nHeight(0)
	, m_nDevID(0)
{
	m_nLastTimestamp=timeGetTime();
	AddIntoHWVideoDecoderMap(this);
}

CUVideoDecoder::~CUVideoDecoder(void)
{
}

void CUVideoDecoder::Close(void)
{
	RemoveFromHWVideoDecoderMap(this);

	if (m_hDecoder) 
	{
		cuda_decoder.cuvidDestroyDecoder(m_hDecoder);
		m_hDecoder = 0;
	}

	if (m_hParser) 
	{
		cuda_decoder.cuvidDestroyVideoParser(m_hParser);
		m_hParser = 0;
	}
	if (m_hStream) 
	{
		cuda_decoder.cuStreamDestroy(m_hStream);
		m_hStream = 0;
	}

	if (m_cudaCtxLock) 
	{
		cuda_decoder.cuvidCtxLockDestroy(m_cudaCtxLock);
		m_cudaCtxLock = 0;
	}

	if (m_cudaContext) 
	{
		cuda_decoder.cuCtxDestroy(m_cudaContext);
		m_cudaContext = 0;
	}

	g_cudaDevDecoderMgr.RemoveDecoder(this);
}


// Beginning of GPU Architecture definitions
static int _ConvertSMVer2CoresDrvApi(int major, int minor)
{
	// Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
	typedef struct 
	{
		int SM; // 0xMm (hexidecimal notation), M = SM Major version, and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] =
	{
		{ 0x10,   8 },
		{ 0x11,   8 },
		{ 0x12,   8 },
		{ 0x13,   8 },
		{ 0x20,  32 },
		{ 0x21,  48 },
		{ 0x30, 192 },
		{   -1,  -1 }
	};

	int index = 0;
	while (nGpuArchCoresPerSM[index].SM != -1) 
	{
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor) ) 
		{
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}
	return -1;
}

int CUVideoDecoder::GetMaxGflopsGraphicsDeviceId()
{
	CUdevice current_device = 0, max_perf_device = 0;
	int device_count     = 0, sm_per_multiproc = 0;
	int max_compute_perf = 0, best_SM_arch     = 0;
	int major = 0, minor = 0, multiProcessorCount, clockRate;
	int bTCC = 0, version;
	char deviceName[256];

	cuda_decoder.cuDeviceGetCount(&device_count);
	if (device_count <= 0)
		return -1;

	g_cudaDevDecoderMgr.SetDevCount(device_count);

	cuda_decoder.cuDriverGetVersion(&version);

	// Find the best major SM Architecture GPU device that are graphics devices
	while ( current_device < device_count ) 
	{
		cuda_decoder.cuDeviceGetName(deviceName, 256, current_device);
		cuda_decoder.cuDeviceComputeCapability(&major, &minor, current_device);

		if (version >= 3020)
		{
			cuda_decoder.cuDeviceGetAttribute(&bTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, current_device);
		} 
		else 
		{
			// Assume a Tesla GPU is running in TCC if we are running CUDA 3.1
			if (deviceName[0] == 'T') bTCC = 1;
		}
		if (!bTCC) 
		{
			if (major > 0 && major < 9999)
			{
				best_SM_arch = max(best_SM_arch, major);
			}
		}
		current_device++;
	}

	// Find the best CUDA capable GPU device
	current_device = 0;
	while( current_device < device_count ) 
	{
		cuda_decoder.cuDeviceGetAttribute(&multiProcessorCount, CU_DEVICE_ATTRIBUTE_MULTIPROCESSOR_COUNT, current_device);
		cuda_decoder.cuDeviceGetAttribute(&clockRate, CU_DEVICE_ATTRIBUTE_CLOCK_RATE, current_device);
		cuda_decoder.cuDeviceComputeCapability(&major, &minor, current_device);

		if (version >= 3020) 
		{
			cuda_decoder.cuDeviceGetAttribute(&bTCC, CU_DEVICE_ATTRIBUTE_TCC_DRIVER, current_device);
		} 
		else 
		{
			// Assume a Tesla GPU is running in TCC if we are running CUDA 3.1
			if (deviceName[0] == 'T') bTCC = 1;
		}

		if (major == 9999 && minor == 9999)
		{
			sm_per_multiproc = 1;
		} 
		else 
		{
			sm_per_multiproc = _ConvertSMVer2CoresDrvApi(major, minor);
		}

		// If this is a Tesla based GPU and SM 2.0, and TCC is disabled, this is a contendor
		if (!bTCC) // Is this GPU running the TCC driver?  If so we pass on this
		{
			int compute_perf = multiProcessorCount * sm_per_multiproc * clockRate;
			if(compute_perf > max_compute_perf) 
			{
				// If we find GPU with SM major > 2, search only these
				if (best_SM_arch > 2) 
				{
					// If our device = dest_SM_arch, then we pick this one
					if (major == best_SM_arch)
					{
						max_compute_perf  = compute_perf;
						max_perf_device   = current_device;
					}
				} 
				else 
				{
					max_compute_perf  = compute_perf;
					max_perf_device   = current_device;
				}
			}
		}
		++current_device;
	}
	return max_perf_device;
}


int CUVideoDecoder::Open(int nWidth,int nHeight)
{
	CUresult cuStatus = CUDA_SUCCESS;

	cuStatus = cuda_decoder.cuInit(0);
	if (cuStatus != CUDA_SUCCESS)
	{
		return -1;
	}
	
	m_nWidth=nWidth;
	m_nHeight=nHeight;

	int best_device = GetMaxGflopsGraphicsDeviceId();
	best_device=g_cudaDevDecoderMgr.GetBestDevID();
	m_nDevID=best_device;

	cuStatus = CUDA_SUCCESS;

	cuStatus = cuda_decoder.cuCtxCreate(&m_cudaContext, CU_CTX_SCHED_BLOCKING_SYNC, best_device);
	int major, minor;
	cuda_decoder.cuDeviceComputeCapability(&major, &minor, best_device);

	if (cuStatus == CUDA_SUCCESS) 
	{
		// Switch to a floating context
		CUcontext curr_ctx = NULL;
		cuStatus = cuda_decoder.cuCtxPopCurrent(&curr_ctx);
		if (cuStatus != CUDA_SUCCESS) 
		{
			return -1;
		}
		cuStatus = cuda_decoder.cuvidCtxLockCreate(&m_cudaCtxLock, m_cudaContext);
		if (cuStatus != CUDA_SUCCESS)
		{
			return -1;
		}
	}
	else 
	{
		return -1;
	}

	if (!m_cudaContext) 
	{
		return -1;
	}


	cudaVideoCodec cudaCodec = cudaVideoCodec_H264;

	m_bWaitForKeyframe = TRUE;
	m_bFormatIncompatible = FALSE;

	// Create the CUDA Video Parser
	CUVIDPARSERPARAMS oVideoParserParameters;
	ZeroMemory(&oVideoParserParameters, sizeof(CUVIDPARSERPARAMS));
	oVideoParserParameters.CodecType              = cudaCodec;
	oVideoParserParameters.ulMaxNumDecodeSurfaces = MAX_DECODE_FRAMES;
	oVideoParserParameters.ulMaxDisplayDelay      = DISPLAY_DELAY;
	oVideoParserParameters.pUserData              = this;
	oVideoParserParameters.pfnSequenceCallback    = CUVideoDecoder::HandleVideoSequence;    // Called before decoding frames and/or whenever there is a format change
	oVideoParserParameters.pfnDecodePicture       = CUVideoDecoder::HandlePictureDecode;    // Called when a picture is ready to be decoded (decode order)
	oVideoParserParameters.pfnDisplayPicture      = CUVideoDecoder::HandlePictureDisplay;   // Called whenever a picture is ready to be displayed (display order)
	oVideoParserParameters.ulErrorThreshold       = 0;


	CUVIDEOFORMATEX cuVideoFormatEx;
	memset(&cuVideoFormatEx, 0, sizeof(CUVIDEOFORMATEX));



	oVideoParserParameters.pExtVideoInfo = &cuVideoFormatEx;
	CUresult oResult = cuda_decoder.cuvidCreateVideoParser(&m_hParser, &oVideoParserParameters);
	if (oResult != CUDA_SUCCESS) 
	{
		return -1;
	}

	cuda_decoder.cuvidCtxLock(m_cudaCtxLock, 0);
	oResult = cuda_decoder.cuStreamCreate(&m_hStream, 0);
	cuda_decoder.cuvidCtxUnlock(m_cudaCtxLock, 0);
	if (oResult != CUDA_SUCCESS) 
	{
		return -1;
	}

	RECT rcDisplayArea = {0, 0, nWidth, nHeight};
	int nRet = CreateCUVIDDecoder(cudaCodec, nWidth, nHeight, nWidth, nHeight, rcDisplayArea);
	if (nRet!=0) 
	{
		return -1;
	}

	m_bForceSequenceUpdate = TRUE;

	m_nLastTimestamp=timeGetTime();

	g_cudaDevDecoderMgr.AddDecoder(this);

	return 0;
}

int CUVideoDecoder::CreateCUVIDDecoder(cudaVideoCodec codec, DWORD dwWidth, DWORD dwHeight, DWORD dwDisplayWidth, DWORD dwDisplayHeight, RECT rcDisplayArea)
{
	int nRet=0;

	cuda_decoder.cuvidCtxLock(m_cudaCtxLock, 0);
	CUVIDDECODECREATEINFO *dci = &m_VideoDecoderInfo;

	if (m_hDecoder)
	{
		cuda_decoder.cuvidDestroyDecoder(m_hDecoder);
		m_hDecoder = 0;
	}
	ZeroMemory(dci, sizeof(*dci));
	dci->ulWidth             = dwWidth;
	dci->ulHeight            = dwHeight;
	dci->ulNumDecodeSurfaces = MAX_DECODE_FRAMES;
	dci->CodecType           = codec;
	dci->ChromaFormat        = cudaVideoChromaFormat_420;
	dci->OutputFormat        = cudaVideoSurfaceFormat_NV12;
	dci->DeinterlaceMode     = cudaVideoDeinterlaceMode_Weave;
	dci->ulNumOutputSurfaces = 1;

	dci->ulTargetWidth       = dwDisplayWidth;
	dci->ulTargetHeight      = dwDisplayHeight;

	dci->display_area.left   = (short)rcDisplayArea.left;
	dci->display_area.right  = (short)rcDisplayArea.right;
	dci->display_area.top    = (short)rcDisplayArea.top;
	dci->display_area.bottom = (short)rcDisplayArea.bottom;

	dci->ulCreationFlags     = cudaVideoCreate_PreferCUVID;
	dci->vidLock             = m_cudaCtxLock;

	// create the decoder
	CUresult oResult = cuda_decoder.cuvidCreateDecoder(&m_hDecoder, dci);
	if (oResult != CUDA_SUCCESS) 
	{
		nRet = -1;
	}
	cuda_decoder.cuvidCtxUnlock(m_cudaCtxLock, 0);

	return nRet;
}


int CUDAAPI CUVideoDecoder::HandleVideoSequence(void *obj, CUVIDEOFORMAT *cuvidfmt)
{
	CUVideoDecoder *pCUVideoDecoder = static_cast<CUVideoDecoder *>(obj);

	CUVIDDECODECREATEINFO *dci = &pCUVideoDecoder->m_VideoDecoderInfo;

	if ((cuvidfmt->codec != dci->CodecType)
		|| (cuvidfmt->coded_width != dci->ulWidth)
		|| (cuvidfmt->coded_height != dci->ulHeight)
		|| (cuvidfmt->display_area.right != dci->ulTargetWidth)
		|| (cuvidfmt->display_area.bottom != dci->ulTargetHeight)
		|| (cuvidfmt->chroma_format != dci->ChromaFormat)
		|| pCUVideoDecoder->m_bForceSequenceUpdate)
	{
		pCUVideoDecoder->m_bForceSequenceUpdate = FALSE;
		RECT rcDisplayArea = {cuvidfmt->display_area.left, cuvidfmt->display_area.top, cuvidfmt->display_area.right, cuvidfmt->display_area.bottom};
		pCUVideoDecoder->CreateCUVIDDecoder(cuvidfmt->codec, cuvidfmt->coded_width, cuvidfmt->coded_height, cuvidfmt->display_area.right, cuvidfmt->display_area.bottom, rcDisplayArea);
	}

	if (cuvidfmt->chroma_format != cudaVideoChromaFormat_420)
	{
		pCUVideoDecoder->m_bFormatIncompatible = TRUE;
	}


	return TRUE;
}

int CUDAAPI CUVideoDecoder::HandlePictureDecode(void *obj, CUVIDPICPARAMS *cuvidpic)
{
	CUVideoDecoder *pCUVideoDecoder = reinterpret_cast<CUVideoDecoder *>(obj);

	if (pCUVideoDecoder->m_bWaitForKeyframe) 
	{
		if (cuvidpic->intra_pic_flag)
		{
			pCUVideoDecoder->m_bWaitForKeyframe = FALSE;
		}
		else 
		{
			return FALSE;
		}
	}

	cuda_decoder.cuvidCtxLock(pCUVideoDecoder->m_cudaCtxLock, 0);
	__try 
	{
		CUresult cuStatus = cuda_decoder.cuvidDecodePicture(pCUVideoDecoder->m_hDecoder, cuvidpic);
	} 
	__except(1) 
	{
	}
	cuda_decoder.cuvidCtxUnlock(pCUVideoDecoder->m_cudaCtxLock, 0);

	return TRUE;
}

int CUDAAPI CUVideoDecoder::HandlePictureDisplay(void *obj, CUVIDPARSERDISPINFO *cuviddisp)
{
	CUVideoDecoder *pCUVideoDecoder = reinterpret_cast<CUVideoDecoder *>(obj);

	pCUVideoDecoder->Deliver(cuviddisp);

	return TRUE;
}


int CUVideoDecoder::Deliver(CUVIDPARSERDISPINFO *cuviddisp)
{
	CUdeviceptr devPtr = 0;
	unsigned int pitch = 0, width = 0, height = 0;
	CUVIDPROCPARAMS vpp;
	CUresult cuStatus = CUDA_SUCCESS;

	memset(&vpp, 0, sizeof(vpp));
	vpp.progressive_frame = cuviddisp->progressive_frame;
	vpp.top_field_first = cuviddisp->top_field_first;
	vpp.second_field = 0;

	cuda_decoder.cuvidCtxLock(m_cudaCtxLock, 0);
	cuStatus = cuda_decoder.cuvidMapVideoFrame(m_hDecoder, cuviddisp->picture_index, &devPtr, &pitch, &vpp);
	if (cuStatus != CUDA_SUCCESS)
	{
		goto cuda_fail;
	}

	width = m_VideoDecoderInfo.display_area.right;
	height = m_VideoDecoderInfo.display_area.bottom;
	int size = pitch * height * 3 / 2;

	unsigned char*pNV12Data=NULL;

	cuStatus = cuda_decoder.cuMemAllocHost((void **)&pNV12Data, size);
	if (cuStatus != CUDA_SUCCESS) 
	{
		goto cuda_fail;
	}

	// Copy memory from the device into the staging area
	if (pNV12Data)
	{
#if USE_ASYNC_COPY
		cuStatus = cuda_decoder.cuMemcpyDtoHAsync(pNV12Data, devPtr, size, m_hStream);
		if (cuStatus != CUDA_SUCCESS) 
		{
			goto cuda_fail;
		}
		while (CUDA_ERROR_NOT_READY == cuda_decoder.cuStreamQuery(m_hStream)) 
		{
			Sleep(1);
		}
#else
		cuStatus = cuda_decoder.cuMemcpyDtoH(pNV12Data, devPtr, size);
		if (cuStatus != CUDA_SUCCESS) 
		{
			goto cuda_fail;
		}
#endif
	} 
	else 
	{
		goto cuda_fail;
	}
	cuda_decoder.cuvidUnmapVideoFrame(m_hDecoder, devPtr);
	cuda_decoder.cuvidCtxUnlock(m_cudaCtxLock, 0);

	// Assign the buffer to the LAV Frame bufers
	int nDataLen = height * pitch * 3 / 2;

	unsigned long nTimestamp=timeGetTime();
	unsigned long nDelta=nTimestamp-m_nLastTimestamp;
	m_nLastTimestamp=nTimestamp;

	unsigned char*pData=(unsigned char*)_aligned_malloc(nDataLen,128);

	if (pData)
	{
		memcpy(pData,pNV12Data,nDataLen);
		m_rCallback.OnHWVideoDecoderCallback(pData,nDataLen,width,height,pitch);
	}
	else
	{
		if (pData)
		{
			_aligned_free(pData);
		}
	}

	if (pNV12Data)
	{
		cuStatus =cuda_decoder.cuMemFreeHost(pNV12Data);
		pNV12Data=NULL;
	}

	//char szDebug[1024];
	//sprintf(szDebug,"Deliver %u nDelta=%u\n",nTimestamp,nDelta);
	//OutputDebugStringA(szDebug);

	return 0;

cuda_fail:
	cuda_decoder.cuvidUnmapVideoFrame(m_hDecoder, devPtr);
	cuda_decoder.cuvidCtxUnlock(m_cudaCtxLock, 0);

	if (pNV12Data)
	{
		cuStatus =cuda_decoder.cuMemFreeHost(pNV12Data);
		pNV12Data=NULL;
	}

	return -1;
}


int CUVideoDecoder::Decode(const unsigned char *buffer, int buflen)
{
	CUresult result;

	CUVIDSOURCEDATAPACKET pCuvidPacket;
	ZeroMemory(&pCuvidPacket, sizeof(pCuvidPacket));

	pCuvidPacket.payload      = buffer;
	pCuvidPacket.payload_size = buflen;

	pCuvidPacket.flags     |= CUVID_PKT_TIMESTAMP;
	pCuvidPacket.timestamp  = timeGetTime();

	cuda_decoder.cuvidCtxLock(m_cudaCtxLock, 0);
	__try
	{
		result = cuda_decoder.cuvidParseVideoData(m_hParser, &pCuvidPacket);
	} 
	__except(1) 
	{
	}
	cuda_decoder.cuvidCtxUnlock(m_cudaCtxLock, 0);

	if (m_bFormatIncompatible) 
	{
		return -1;
	}

	return 0;
}
