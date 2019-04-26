#include "stdafx.h"
#include "VIDECINLAPI.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#include "XEncoderFrameI264.h"
//#include "pipeline_decode.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"
#include <map>
#include "XAutoLock.h"
#include "HPAVC.h"
static bool g_bIntelH264DecoderHardware=false;
static bool g_bIntelH264DecoderInitialized=false;


typedef std::map<void*,XEncoderFrameI264*>	MapXEncoderFrameI264;

XCritSec			 g_csMapXEncoderFrameI264;
MapXEncoderFrameI264 g_MapXEncoderFrameI264;

void IntelH264GetEncoderCount(int& nCurCount,int& nMaxCount)
{
	{
		XAutoLock l(g_csMapXEncoderFrameI264);
		nCurCount = g_MapXEncoderFrameI264.size();
	}
	nMaxCount = FRE_GetMaxEncoderCount();
}

int IntelH264HardwareEncoderTry(unsigned long nWidth,unsigned long nHeight)
{
	mfxIMPL impl = GetIntelAdapterIMPL();

	if (impl == MFX_IMPL_UNSUPPORTED)
	{
		DebugOutput( logcoreexcep ,"MFX_IMPL_UNSUPPORTED IntelH264HardwareEncoderTry return -1 \n");
		return -1;
	}

	if (FRE_GetForceIntelEncoder() == 0 && impl == 2)
	{
		DebugOutput( logcoreexcep ,"FRE_GetForceIntelEncoder==0 && imp1 ==2 IntelH264HardwareEncoderTry \n");
		return -1;
	}

	if (nWidth==0 && nHeight==0)
	{
		return 0;
	}

	if (nWidth>=FRE_GetHWEncodeWidth() && nHeight>=FRE_GetHWEncodeHeight())
	{

	}
	else
	{
		DebugOutput( logcoreexcep ,"width < IntelH264HardwareEncoderTry return -1 \n");
		return -1;
	}
	{
		XAutoLock l(g_csMapXEncoderFrameI264);
		if (g_MapXEncoderFrameI264.size()<FRE_GetMaxEncoderCount())
		{
			return 0;
		}
	}
	DebugOutput( logcoreexcep ,"IntelH264HardwareEncoderTry end. width =%ld,height =%ld \n",nWidth,nHeight);
	return -1;
}

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
	//test ??
	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		DebugOutput( logcoreexcep ,"VIDECINL_EncoderOpen timestamp. width =%ld,height =%ld \n",nWidth,nHeight);
		//return NULL;
	}

	DebugOutput( logcallstack ,"VIDECINL_EncoderOpen begin. width =%ld,height =%ld \n",nWidth,nHeight);

	NEWRECORD3( XEncoderFrameI264*,pXEncoderFrameI264, new XEncoderFrameI264() )
	if (0!=pXEncoderFrameI264->Open(nWidth,nHeight,nFrameRate,nProfile,nBitrate,nQMin,nQMax,nKeyIntMax,encoder_cb,pUserData))
	{
		pXEncoderFrameI264->Close();
		delete pXEncoderFrameI264;
		pXEncoderFrameI264=NULL;
		DebugOutput( logcoreexcep ,"VIDECINL_EncoderOpen Failed. width =%ld,height =%ld \n",nWidth,nHeight);
	}

	{
		XAutoLock l(g_csMapXEncoderFrameI264);
		g_MapXEncoderFrameI264[pXEncoderFrameI264]=pXEncoderFrameI264;
	}

	if (FRE_GetDebugLevel() > 0)
	{
		int nCurCount=0,nMaxCount=0;
		IntelH264GetEncoderCount(nCurCount,nMaxCount);

		char szDebug[1024];
		sprintf(szDebug,"VIDECINL_EncoderOpen nWidth=%u nHeight=%u FrameRate=%u EncoderCount=%u MaxEncoder=%u IntelAdapterNum=%u\n",nWidth, nHeight,nFrameRate,nCurCount,nMaxCount,GetIntelAdapterNum());
		DebugOutput( logcallstack ,szDebug);
	}


	return pXEncoderFrameI264;
}


VIDECINL_API void VIDECINL_EncoderClose(void*pHandle)
{
	DebugOutput( logcallstack ,"VIDECINL_EncoderClose \n");
	XEncoderFrameI264*pXEncoderFrameI264=(XEncoderFrameI264*)pHandle;
	if (pXEncoderFrameI264)
	{
		{
			XAutoLock l(g_csMapXEncoderFrameI264);
			g_MapXEncoderFrameI264.erase(pXEncoderFrameI264);
		}

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
		if( nRequestKeyFrame )  DebugOutput( logcallstack ,"I264 Encoder  RequestKeyFrame.\n");
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

//VIDECINL_API int VIDECINL_DecoderTry(VIDECINL_ALIGNED_MALLOC aligned_malloc,VIDECINL_ALIGNED_FREE aligned_free)
//{
//	intel_aligned_malloc=aligned_malloc;
//	intel_aligned_free=aligned_free;
//
//	if (g_bIntelH264DecoderInitialized==false)
//	{
//		g_bIntelH264DecoderInitialized=true;
//   
//		mfxStatus nStatus=MFX_Init();		
//		g_bIntelH264DecoderHardware = (nStatus==MFX_ERR_NONE);
//	}
//
//	return (g_bIntelH264DecoderHardware?0:-1);
//}
//
//VIDECINL_API void* VIDECINL_DecoderOpen(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData,int nKey)
//{
//	unsigned long nTimestamp=timeGetTime();
//	if (nTimestamp-nKey>1000)
//	{
//		return NULL;
//	}
//	NEWRECORD3( CDecodingPipeline*,pPipeline , new CDecodingPipeline() )
//	if (0!=pPipeline->Open(nWidth,nHeight,decoder_cb,pUserData))
//	{
//		pPipeline->Close();
//		delete pPipeline;
//		pPipeline=NULL;
//	}
//
//	if (FRE_GetDebugLevel() > 0)
//	{
//		char szDebug[1024];
//		sprintf(szDebug,"VIDECINL_DecoderOpen nWidth=%u nHeight=%u IntelAdapterNum=%u\n",nWidth, nHeight,GetIntelAdapterNum());
//		DebugOutput( logcallstack ,szDebug);
//	}
//
//	return pPipeline;
//}
//
//VIDECINL_API void VIDECINL_DecoderClose(void*pHandle)
//{
//	CDecodingPipeline*pPipeline=(CDecodingPipeline*)pHandle;
//	if (pPipeline)
//	{
//		pPipeline->Close();
//		delete pPipeline;
//		pPipeline=NULL;
//	}
//}
//
//VIDECINL_API int VIDECINL_DecoderDecode(void*pHandle,const unsigned char*pData,int nLen)
//{
//	CDecodingPipeline*pPipeline=(CDecodingPipeline*)pHandle;
//	if (pPipeline)
//	{
//		return pPipeline->Decode(pData,nLen);
//	}
//
//	return -1;
//}
