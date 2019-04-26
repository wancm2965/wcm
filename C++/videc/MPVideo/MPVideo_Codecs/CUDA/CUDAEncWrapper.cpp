#include "CUDAEncWrapper.h"

CUDAENC cudaenc;


#define GET_PROC_EX(name, lib)                         \
	cudaenc.name = (t##name *)GetProcAddress(lib, #name); \
	if (cudaenc.name == NULL) {                           \
	return -1; \
	}

#define GET_PROC_CUDAENC(name) GET_PROC_EX(name, cudaenc.cudaencLib)

int CUDAEncoderLoad(void)
{
	cudaenc.cudaencLib = LoadLibrary("nvcuvenc.dll");
	if (cudaenc.cudaencLib == NULL)
	{
		return -1;
	}

	GET_PROC_CUDAENC(NVCreateEncoder);
	GET_PROC_CUDAENC(NVDestroyEncoder);
	GET_PROC_CUDAENC(NVSetCodec);
	GET_PROC_CUDAENC(NVSetParamValue);
	GET_PROC_CUDAENC(NVGetParamValue);
	GET_PROC_CUDAENC(NVCreateHWEncoder);
	GET_PROC_CUDAENC(NVEncodeFrame);
	GET_PROC_CUDAENC(NVRegisterCB);

	return 0;
}

void CUDAEncoderFree(void)
{
	if (cudaenc.cudaencLib)
	{
		FreeLibrary(cudaenc.cudaencLib);
		cudaenc.cudaencLib=NULL;
	}
}

bool CUDAEncoderOK(void)
{
	return (cudaenc.cudaencLib != NULL);
}


//
//int cuCreateEncoder(NVEncoder *pNVEncoder)
//{
//	return -1;
//}
//
//int cuDestroyEncoder(NVEncoder hNVEncoder)
//{
//	return -1;
//}
//
//int cuSetCodec(NVEncoder hNVEncoder, unsigned long dwCodecType)
//{
//	return -1;
//}
//
//int cuSetParamValue(NVEncoder hNVEncoder, unsigned long dwParamType, void *pData)
//{
//	return -1;
//}
//
//int cuGetParamValue(NVEncoder hNVEncoder, unsigned long dwParamType, void *pData)
//{
//	return -1;
//}
//
//int cuSetDefaultParam(NVEncoder hNVEncoder)
//{
//	return -1;
//}
//
//int cuCreateHWEncoder(NVEncoder hNVEncoder)
//{
//	return -1;
//}
//
//int cuEncodeFrame(NVEncoder hNVEncoder, NVVE_EncodeFrameParams *pFrmIn, unsigned long flag, void *pData)
//{
//	return -1;
//}
//
//void cuRegisterCB(NVEncoder hNVEncoder, NVVE_CallbackParams cb, void *pUserdata)
//{
//}
//
