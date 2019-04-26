#include "stdafx.h"
#include "VIDECINLAPI.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include "XEncoderFrameI264.h"
#include "./IntelMediaSDK/pipeline_decode.h"

static bool g_bIntelH264DecoderHardware=false;
static bool g_bIntelH264DecoderInitialized=false;


void VIDECINL_EncoderSetMaxCount(unsigned int nMaxCount)
{
	IntelH264HardwareEncoderSetMaxCount(nMaxCount);
}

VIDECINL_API int VIDECINL_EncoderTry(int nWidth,int nHeight)
{
	return IntelH264HardwareEncoderTry(nWidth,nHeight);
}

VIDECINL_API void*VIDECINL_EncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData,int nKey)
{
	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return NULL;
	}
	XEncoderFrameI264*pXEncoderFrameI264=new XEncoderFrameI264();
	if (0!=pXEncoderFrameI264->Open(nWidth,nHeight,nFrameRate,nProfile,nBitrate,nQMin,nQMax,nKeyIntMax,encoder_cb,pUserData))
	{
		pXEncoderFrameI264->Close();
		delete pXEncoderFrameI264;
		pXEncoderFrameI264=NULL;
	}

	return pXEncoderFrameI264;
}

VIDECINL_API void VIDECINL_EncoderClose(void*pHandle)
{
	XEncoderFrameI264*pXEncoderFrameI264=(XEncoderFrameI264*)pHandle;
	if (pXEncoderFrameI264)
	{
		pXEncoderFrameI264->Close();
		delete pXEncoderFrameI264;
		pXEncoderFrameI264=NULL;
	}
}

VIDECINL_API int VIDECINL_EncoderEncoding(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	XEncoderFrameI264*pXEncoderFrameI264=(XEncoderFrameI264*)pHandle;
	if (pXEncoderFrameI264)
	{
		return pXEncoderFrameI264->Encode(pData,nLen,nRequestKeyFrame);
	}
	return -1;
}


VIDECINL_ALIGNED_MALLOC intel_aligned_malloc=NULL;
VIDECINL_ALIGNED_FREE intel_aligned_free=NULL;

void*my_aligned_malloc(size_t size,size_t alignment)
{
	if (intel_aligned_malloc)
	{
		return intel_aligned_malloc(size,alignment);
	}

	return _aligned_malloc(size,alignment);
}

void my_aligned_free(void*p)
{
	if (intel_aligned_free)
	{
		intel_aligned_free(p);
	}
	else
	{
		_aligned_free(p);
	}
}

VIDECINL_API int VIDECINL_DecoderTry(VIDECINL_ALIGNED_MALLOC aligned_malloc,VIDECINL_ALIGNED_FREE aligned_free)
{
	intel_aligned_malloc=aligned_malloc;
	intel_aligned_free=aligned_free;

	if (g_bIntelH264DecoderInitialized==false)
	{
		g_bIntelH264DecoderInitialized=true;

		mfxSession tSession;
		mfxIMPL impl = MFX_IMPL_HARDWARE;
		mfxVersion ver = {3, 1};    
		mfxStatus nStatus=MFXInit(MFX_IMPL_HARDWARE,&ver,&tSession);
		MFXClose(tSession);
		g_bIntelH264DecoderHardware = (nStatus==MFX_ERR_NONE);
	}

	return (g_bIntelH264DecoderHardware?0:-1);
}

VIDECINL_API void* VIDECINL_DecoderOpen(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData,int nKey)
{
	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return NULL;
	}
	CDecodingPipeline*pPipeline=new CDecodingPipeline();
	if (0!=pPipeline->Open(nWidth,nHeight,decoder_cb,pUserData))
	{
		pPipeline->Close();
		delete pPipeline;
		pPipeline=NULL;
	}
	return pPipeline;
}

VIDECINL_API void VIDECINL_DecoderClose(void*pHandle)
{
	CDecodingPipeline*pPipeline=(CDecodingPipeline*)pHandle;
	if (pPipeline)
	{
		pPipeline->Close();
		delete pPipeline;
		pPipeline=NULL;
	}
}

VIDECINL_API int VIDECINL_DecoderDecode(void*pHandle,const unsigned char*pData,int nLen)
{
	CDecodingPipeline*pPipeline=(CDecodingPipeline*)pHandle;
	if (pPipeline)
	{
		return pPipeline->Decode(pData,nLen);
	}

	return -1;
}
