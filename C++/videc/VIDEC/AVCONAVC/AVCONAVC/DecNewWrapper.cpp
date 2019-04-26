#include <stdafx.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include "DecNewWrapper.h"


static AVH264DECODERWrapper* g_pAVH264DECODERWrapper=NULL;
extern HMODULE g_hAVCONAVCInstance;


AVH264DECODERWrapper&AVH264DECODERWrapper::Instance(void)
{
	return *g_pAVH264DECODERWrapper;
}

AVH264DECODERWrapper::AVH264DECODERWrapper(HINSTANCE hInstance)
{
	m_bLoadFuncOK = false;

	NEWRECORD( m_pXDllLoadLibrary , new XAVCDllLoadLibrary(_T("H264Decorder.dll")) )

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pAVH264DECODER_Initialize, ("AVH264DECODER_Initialize") );
		m_pXDllLoadLibrary->LoadFunction(pAVH264DECODER_UnInitialize, ("AVH264DECODER_UnInitialize") );
		m_pXDllLoadLibrary->LoadFunction(pAVH264DECODER_DecoderOpen, ("AVH264DECODER_DecoderOpen"));
		m_pXDllLoadLibrary->LoadFunction(pAVH264DECODER_DecoderClose, ("AVH264DECODER_DecoderClose"));
		m_pXDllLoadLibrary->LoadFunction(pAVH264DECODER_DecoderDecode, ("AVH264DECODER_DecoderDecode"));
		if (m_pXDllLoadLibrary->IsOK())
			DebugOutput(logcallstack, "load H264Decorder.dll sucess!\n");
		else
			DebugOutput(logcallstack, "load H264Decorder.dll failed!\n");
	}
}

AVH264DECODERWrapper::~AVH264DECODERWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool AVH264DECODERWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}


bool AVH264DECODERInit(void)
{
	try{
		if (g_pAVH264DECODERWrapper==NULL)
		{
			NEWRECORD( g_pAVH264DECODERWrapper , new AVH264DECODERWrapper(NULL) ) 
			NULLReturnRet(g_pAVH264DECODERWrapper ,false )
			if (!g_pAVH264DECODERWrapper->IsOK() )
			{
				delete g_pAVH264DECODERWrapper;
				g_pAVH264DECODERWrapper=NULL;
				return false;
			}

			g_pAVH264DECODERWrapper->pAVH264DECODER_Initialize();
			g_pAVH264DECODERWrapper->m_bLoadFuncOK = true;
			return true;
		}
	}
	catch(...)
	{

	}

	return false;
}

void* AVH264DECODEROpen(int nWidth, int nHeight, int AVCCodecID)
{
	if (g_pAVH264DECODERWrapper && g_pAVH264DECODERWrapper->m_bLoadFuncOK)
	{
		return g_pAVH264DECODERWrapper->pAVH264DECODER_DecoderOpen(nWidth, nHeight, AVCCodecID);
	}
	return NULL;
}

void AVH264DECODERClose(void*pHandle)
{
	if (g_pAVH264DECODERWrapper && g_pAVH264DECODERWrapper->m_bLoadFuncOK)
	{
		g_pAVH264DECODERWrapper->pAVH264DECODER_DecoderClose(pHandle);
	}
}

int AVH264DECODERDecode(void*pHandle, unsigned char*pStreamData, int nStreamLen, unsigned char*pYV12[], int nStride[], int*pnWidth, int*pnHeight, int*pnUsedBytes, int*pnKeyFrame, char**ppqscale_table, int*pqstride, int* pix_fmt)
{
	if (g_pAVH264DECODERWrapper && g_pAVH264DECODERWrapper->m_bLoadFuncOK)
	{
		return g_pAVH264DECODERWrapper->pAVH264DECODER_DecoderDecode(pHandle, pStreamData, nStreamLen, pYV12, nStride, pnWidth, pnHeight, pnUsedBytes, pnKeyFrame, ppqscale_table, pqstride, pix_fmt);
	}

	return -1;
}

void AVH264DECODERTerminate(void)
{
	try{
		if (g_pAVH264DECODERWrapper)
		{
			delete g_pAVH264DECODERWrapper;
			g_pAVH264DECODERWrapper=NULL;
		}
	}
	catch(...)
	{

	}
}

