//CUDAEncWrapper.h
#ifndef __CUDAENCWRAPPER_H__
#define __CUDAENCWRAPPER_H__

#include <Windows.h>
#include "NVEncoderAPI.h"

typedef int NVENCAPI tNVCreateEncoder(NVEncoder *pNVEncoder);
typedef int NVENCAPI tNVDestroyEncoder(NVEncoder hNVEncoder);
typedef int NVENCAPI tNVSetCodec(NVEncoder hNVEncoder, unsigned long dwCodecType);
typedef int NVENCAPI tNVSetParamValue(NVEncoder hNVEncoder, unsigned long dwParamType, void *pData);
typedef int NVENCAPI tNVGetParamValue(NVEncoder hNVEncoder, unsigned long dwParamType, void *pData);
typedef int NVENCAPI tNVCreateHWEncoder(NVEncoder hNVEncoder);
typedef int NVENCAPI tNVEncodeFrame(NVEncoder hNVEncoder, NVVE_EncodeFrameParams *pFrmIn, unsigned long flag, void *pData);
typedef void NVENCAPI tNVRegisterCB(NVEncoder hNVEncoder, NVVE_CallbackParams cb, void *pUserdata);

#define CUMETHOD(name) t##name *##name

typedef struct tagCUDAENC
{
	tagCUDAENC(void)
	{
		cudaencLib=NULL;
	}

	HMODULE cudaencLib;

	CUMETHOD(NVCreateEncoder);
	CUMETHOD(NVDestroyEncoder);
	CUMETHOD(NVSetCodec);
	CUMETHOD(NVSetParamValue);
	CUMETHOD(NVGetParamValue);
	CUMETHOD(NVCreateHWEncoder);
	CUMETHOD(NVEncodeFrame);
	CUMETHOD(NVRegisterCB);
}CUDAENC;

extern CUDAENC cudaenc;

int CUDAEncoderLoad(void);
void CUDAEncoderFree(void);
bool CUDAEncoderOK(void);

#endif
