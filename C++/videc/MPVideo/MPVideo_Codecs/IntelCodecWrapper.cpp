#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "IntelCodecWrapper.h"
#include "XUtil.h"



static IntelCodecWrapper* g_pIntelCodecWrapper=NULL;
extern HINSTANCE g_hInstance;

extern BOOL IsVistaOrAbove();


IntelCodecWrapper&IntelCodecWrapper::Instance(void)
{
	return *g_pIntelCodecWrapper;
}

IntelCodecWrapper::IntelCodecWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%svidecinl.dll",szTemp);

	pVIDECINL_EncoderSetMaxCount=NULL;
	pVIDECINL_EncoderTry=NULL;
	pVIDECINL_EncoderOpen=NULL;
	pVIDECINL_EncoderClose=NULL;
	pVIDECINL_EncoderEncoding=NULL;

	pVIDECINL_DecoderTry=NULL;
	pVIDECINL_DecoderOpen=NULL;
	pVIDECINL_DecoderClose=NULL;
	pVIDECINL_DecoderDecode=NULL;


	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_EncoderSetMaxCount,"VIDECINL_EncoderSetMaxCount");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_EncoderTry,"VIDECINL_EncoderTry");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_EncoderOpen,"VIDECINL_EncoderOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_EncoderClose,"VIDECINL_EncoderClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_EncoderEncoding,"VIDECINL_EncoderEncoding");

		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_DecoderTry,"VIDECINL_DecoderTry");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_DecoderOpen,"VIDECINL_DecoderOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_DecoderClose,"VIDECINL_DecoderClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECINL_DecoderDecode,"VIDECINL_DecoderDecode");
	}
}

IntelCodecWrapper::~IntelCodecWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool IntelCodecWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void intel_codec_init(void)
{
	if (!IsVistaOrAbove())
		return;

	try{
		if (g_pIntelCodecWrapper==NULL)
		{
			g_pIntelCodecWrapper=new IntelCodecWrapper(g_hInstance);
			if (!g_pIntelCodecWrapper->IsOK())
			{
				delete g_pIntelCodecWrapper;
				g_pIntelCodecWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void intel_codec_uninit(void)
{
	try{
		if (g_pIntelCodecWrapper)
		{
			delete g_pIntelCodecWrapper;
			g_pIntelCodecWrapper=NULL;
		}
	}catch(...){

	}
}

void IntelEncoderSetMaxCount(unsigned int nMaxCount)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return ;
		g_pIntelCodecWrapper->pVIDECINL_EncoderSetMaxCount(nMaxCount);
	}
	catch(...)
	{
	}
}

int IntelEncoderTry(int nWidth,int nHeight)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return -1;
		return g_pIntelCodecWrapper->pVIDECINL_EncoderTry(nWidth,nHeight);
	}
	catch(...)
	{
	}

	return -1;
}

void*IntelEncoderOpen(int nWidth,int nHeight,int nFrameRate,int nProfile,int nBitrate,int nQMin,int nQMax,int nKeyIntMax,VIDECINL_ENCODER_CB encoder_cb,void*pUserData)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return NULL;
		return g_pIntelCodecWrapper->pVIDECINL_EncoderOpen(nWidth,nHeight,nFrameRate,nProfile,nBitrate,nQMin,nQMax,nKeyIntMax,encoder_cb,pUserData,XGetTimestamp());
	}
	catch(...)
	{
	}

	return NULL;
}

void IntelEncoderClose(void*pHandle)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return ;
		g_pIntelCodecWrapper->pVIDECINL_EncoderClose(pHandle);
	}
	catch(...)
	{
	}
}

int IntelEncoderEncoding(void*pHandle,unsigned char*pData,int nLen,int nRequestKeyFrame)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return -1;
		return g_pIntelCodecWrapper->pVIDECINL_EncoderEncoding(pHandle,pData,nLen,nRequestKeyFrame);
	}
	catch(...)
	{
	}

	return -1;
}



void*my_aligned_malloc(size_t size,size_t alignment)
{
	return _aligned_malloc(size,alignment);
}

void my_aligned_free(void*p)
{
	_aligned_free(p);
}

int IntelDecoderTry(void)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return -1;
		return g_pIntelCodecWrapper->pVIDECINL_DecoderTry(my_aligned_malloc,my_aligned_free);
	}
	catch(...)
	{
	}

	return -1;
}

void* IntelDecoderOpen(int nWidth,int nHeight,VIDECINL_DECODER_CB decoder_cb,void*pUserData)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return NULL;
		return g_pIntelCodecWrapper->pVIDECINL_DecoderOpen(nWidth,nHeight,decoder_cb,pUserData,XGetTimestamp());
	}
	catch(...)
	{
	}

	return NULL;
}

void IntelDecoderClose(void*pHandle)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return ;
		g_pIntelCodecWrapper->pVIDECINL_DecoderClose(pHandle);
	}
	catch(...)
	{
	}
}

int IntelDecoderDecode(void*pHandle,const unsigned char*pData,int nLen)
{
	try{
		if (g_pIntelCodecWrapper==NULL)
			return -1;
		return g_pIntelCodecWrapper->pVIDECINL_DecoderDecode(pHandle,pData,nLen);
	}
	catch(...)
	{
	}

	return -1;
}
