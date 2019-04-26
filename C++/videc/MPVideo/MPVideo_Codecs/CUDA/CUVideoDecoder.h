#pragma once

#define MAX_DECODE_FRAMES 3
#define DISPLAY_DELAY	2
#define USE_ASYNC_COPY 1

#define CUDA_FORCE_API_VERSION 3010
#include "cuda.h"
#include "nvcuvid.h"
#include "cuda_dynlink.h"

#include "../HWVideoDecoder.h"

#define CUMETHOD(name) t##name *##name

class CUVideoDecoder
	: public HWVideoDecoder
{
public:
	CUVideoDecoder(HWVideoDecoderCallback&rCallback);
	virtual ~CUVideoDecoder(void);
public:
	int Open(int nWidth,int nHeight);
	void Close(void);
	int Decode(const unsigned char *buffer, int buflen);

	int GetWidth(void){return m_nWidth;}
	int GetHeight(void){return m_nHeight;}
	int GetDevID(void){return m_nDevID;}
private:

	int CreateCUVIDDecoder(cudaVideoCodec codec, DWORD dwWidth, DWORD dwHeight, DWORD dwDisplayWidth, DWORD dwDisplayHeight, RECT rcDisplayArea);

	// CUDA Callbacks
	static int CUDAAPI HandleVideoSequence(void *obj, CUVIDEOFORMAT *cuvidfmt);
	static int CUDAAPI HandlePictureDecode(void *obj, CUVIDPICPARAMS *cuvidpic);
	static int CUDAAPI HandlePictureDisplay(void *obj, CUVIDPARSERDISPINFO *cuviddisp);

	int Deliver(CUVIDPARSERDISPINFO *cuviddisp);

	int GetMaxGflopsGraphicsDeviceId();
private:
	CUcontext              m_cudaContext;
	CUvideoctxlock         m_cudaCtxLock;

	CUvideoparser          m_hParser;

	CUvideodecoder         m_hDecoder;
	CUVIDDECODECREATEINFO  m_VideoDecoderInfo;


	CUstream               m_hStream;


	BOOL                   m_bForceSequenceUpdate;

	BOOL                   m_bWaitForKeyframe;



	BOOL                   m_bFormatIncompatible;


	unsigned long			m_nLastTimestamp;

	HWVideoDecoderCallback&	m_rCallback;
	int						m_nWidth;
	int						m_nHeight;
	int						m_nDevID;
};


struct CUDA_DECODER
{
	CUDA_DECODER(void)
	{
		cudaLib=NULL;
		cuvidLib=NULL;
	}
	HMODULE cudaLib;
	CUMETHOD(cuInit);
	CUMETHOD(cuCtxCreate);
	CUMETHOD(cuCtxDestroy);
	CUMETHOD(cuCtxPushCurrent);
	CUMETHOD(cuCtxPopCurrent);
	CUMETHOD(cuD3D9CtxCreate);
	CUMETHOD(cuMemAllocHost);
	CUMETHOD(cuMemFreeHost);
	CUMETHOD(cuMemcpyDtoH);
	CUMETHOD(cuMemcpyDtoHAsync);
	CUMETHOD(cuStreamCreate);
	CUMETHOD(cuStreamDestroy);
	CUMETHOD(cuStreamQuery);
	CUMETHOD(cuDeviceGetCount);
	CUMETHOD(cuDriverGetVersion);
	CUMETHOD(cuDeviceGetName);
	CUMETHOD(cuDeviceComputeCapability);
	CUMETHOD(cuDeviceGetAttribute);

	HMODULE cuvidLib;
	CUMETHOD(cuvidCtxLockCreate);
	CUMETHOD(cuvidCtxLockDestroy);
	CUMETHOD(cuvidCtxLock);
	CUMETHOD(cuvidCtxUnlock);
	CUMETHOD(cuvidCreateVideoParser);
	CUMETHOD(cuvidParseVideoData);
	CUMETHOD(cuvidDestroyVideoParser);
	CUMETHOD(cuvidCreateDecoder);
	CUMETHOD(cuvidDecodePicture);
	CUMETHOD(cuvidDestroyDecoder);
	CUMETHOD(cuvidMapVideoFrame);
	CUMETHOD(cuvidUnmapVideoFrame);
};

extern struct CUDA_DECODER cuda_decoder;