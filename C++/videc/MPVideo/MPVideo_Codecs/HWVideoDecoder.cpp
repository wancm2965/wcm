#include "stdafx.h"
#include "HWVideoDecoder.h"
#include "./CUDA/CUVideoDecoder.h"
#include "XAutoLock.h"
#include "IntelCodecWrapper.h"
#include "IntelVideoDecoder.h"

#include "../../VIDEC/VIDEC_FRE/VIDECFREAPI.h"

#include <map>


typedef std::map<void*,HWVideoDecoder*>		MapHWVideoDecoder;

static MapHWVideoDecoder	g_MapHWVideoDecoder;
static XCritSec				g_csMapHWVideoDecoder;

static unsigned int g_nMaxHWVideoDecoderCount=0;
static bool g_bCUDAH264DecoderHardware=false;
static bool g_bIntelH264DecoderHardware=false;


struct CUDA_DECODER cuda_decoder;


#define GET_PROC_EX(name, lib)                         \
	cuda_decoder.name = (t##name *)GetProcAddress(lib, #name); \
	if (cuda_decoder.name == NULL) {                           \
	return E_FAIL; \
	}

#define GET_PROC_CUDA(name) GET_PROC_EX(name, cuda_decoder.cudaLib)
#define GET_PROC_CUVID(name) GET_PROC_EX(name, cuda_decoder.cuvidLib)

int LoadCUDADecoderFuncRefs(void)
{
	cuda_decoder.cudaLib = LoadLibrary("nvcuda.dll");
	if (cuda_decoder.cudaLib == NULL) 
	{
		return -1;
	}

	GET_PROC_CUDA(cuInit);
	GET_PROC_CUDA(cuCtxCreate);
	GET_PROC_CUDA(cuCtxDestroy);
	GET_PROC_CUDA(cuCtxPushCurrent);
	GET_PROC_CUDA(cuCtxPopCurrent);
	GET_PROC_CUDA(cuD3D9CtxCreate);
	GET_PROC_CUDA(cuMemAllocHost);
	GET_PROC_CUDA(cuMemFreeHost);
	GET_PROC_CUDA(cuMemcpyDtoH);
	GET_PROC_CUDA(cuMemcpyDtoHAsync);
	GET_PROC_CUDA(cuStreamCreate);
	GET_PROC_CUDA(cuStreamDestroy);
	GET_PROC_CUDA(cuStreamQuery);
	GET_PROC_CUDA(cuDeviceGetCount);
	GET_PROC_CUDA(cuDriverGetVersion);
	GET_PROC_CUDA(cuDeviceGetName);
	GET_PROC_CUDA(cuDeviceComputeCapability);
	GET_PROC_CUDA(cuDeviceGetAttribute);

	// Load CUVID function
	cuda_decoder.cuvidLib = LoadLibrary("nvcuvid.dll");
	if (cuda_decoder.cuvidLib == NULL) 
	{
		return -1;
	}

	GET_PROC_CUVID(cuvidCtxLockCreate);
	GET_PROC_CUVID(cuvidCtxLockDestroy);
	GET_PROC_CUVID(cuvidCtxLock);
	GET_PROC_CUVID(cuvidCtxUnlock);
	GET_PROC_CUVID(cuvidCreateVideoParser);
	GET_PROC_CUVID(cuvidParseVideoData);
	GET_PROC_CUVID(cuvidDestroyVideoParser);
	GET_PROC_CUVID(cuvidCreateDecoder);
	GET_PROC_CUVID(cuvidDecodePicture);
	GET_PROC_CUVID(cuvidDestroyDecoder);
	GET_PROC_CUVID(cuvidMapVideoFrame);
	GET_PROC_CUVID(cuvidUnmapVideoFrame);

	CUresult cuStatus = CUDA_SUCCESS;

	cuStatus = cuda_decoder.cuInit(0);
	if (cuStatus != CUDA_SUCCESS)
	{
		return -1;
	}

	return 0;
}



void AddIntoHWVideoDecoderMap(HWVideoDecoder*pHWVideoDecoder)
{
	XAutoLock l(g_csMapHWVideoDecoder);
	g_MapHWVideoDecoder[pHWVideoDecoder]=pHWVideoDecoder;
}

void RemoveFromHWVideoDecoderMap(HWVideoDecoder*pHWVideoDecoder)
{
	XAutoLock l(g_csMapHWVideoDecoder);
	g_MapHWVideoDecoder.erase(pHWVideoDecoder);
}


HWVideoDecoder::HWVideoDecoder(void)
{
}

HWVideoDecoder::~HWVideoDecoder(void)
{
}

HWVideoDecoder*HWVideoDecoder::Create(HWVideoDecoderCallback&rCallback)
{
	if (g_bIntelH264DecoderHardware)
	{
		return new IntelVideoDecoder(rCallback);
	}

// 	if (g_bCUDAH264DecoderHardware)
// 	{
// 		return new CUVideoDecoder(rCallback);
// 	}

	return NULL;
}

void HWVideoDecoder::Load(void)
{
// 	if (0!=LoadCUDADecoderFuncRefs())
// 	{
// 		if (cuda_decoder.cudaLib)
// 		{
// 			FreeLibrary(cuda_decoder.cudaLib);
// 			cuda_decoder.cudaLib=NULL;
// 		}
// 
// 		if (cuda_decoder.cuvidLib)
// 		{
// 			FreeLibrary(cuda_decoder.cuvidLib);
// 			cuda_decoder.cuvidLib=NULL;
// 		}
// 
// 		g_bCUDAH264DecoderHardware=false;
// 	}
// 	else
// 	{
// 		g_bCUDAH264DecoderHardware=true;
// 	}

	if (0!=IntelDecoderTry())
	{
		g_bIntelH264DecoderHardware=false;
	}
	else
	{
		g_bIntelH264DecoderHardware=true;
	}
}

void HWVideoDecoder::Unload(void)
{
	if (cuda_decoder.cudaLib)
	{
		FreeLibrary(cuda_decoder.cudaLib);
		cuda_decoder.cudaLib=NULL;
	}

	if (cuda_decoder.cuvidLib)
	{
		FreeLibrary(cuda_decoder.cuvidLib);
		cuda_decoder.cuvidLib=NULL;
	}
}

int HWVideoDecoder::TryDecoder(unsigned long nWidth,unsigned long nHeight)
{
	if (g_bCUDAH264DecoderHardware==false && g_bIntelH264DecoderHardware==false)
	{
		return -1;
	}

	if (nWidth>=FRE_GetHWDecodeWidth() && nHeight>=FRE_GetHWDecodeHeight())
	{

	}
	else
	{
		return -1;
	}
	{
		XAutoLock l(g_csMapHWVideoDecoder);
		if (g_MapHWVideoDecoder.size()<FRE_GetMaxDecoderCount())
		{
			return 0;
		}
	}
	return -1;
}


void HWVideoDecoder::SetMaxCount(unsigned int nMaxCount)
{
	g_nMaxHWVideoDecoderCount=nMaxCount;
}

int HWVideoDecoder::GetSupport(void)
{
	return ((g_bCUDAH264DecoderHardware || g_bIntelH264DecoderHardware)?1:0);
}