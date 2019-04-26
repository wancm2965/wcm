#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "VIDECUDAWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static VIDECUDAWrapper* g_pVIDECUDAWrapper=NULL;
extern HINSTANCE g_hInstance;

int cudaSupportPlayer(void)
{
	return (g_pVIDECUDAWrapper!=NULL?1:0);
}


VIDECUDAWrapper&VIDECUDAWrapper::Instance(void)
{
	return *g_pVIDECUDAWrapper;
}

VIDECUDAWrapper::VIDECUDAWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECUDA.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_Init,"VIDECUDA_Init");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerSetMaxCount,"VIDECUDA_PlayerSetMaxCount");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerTry,"VIDECUDA_PlayerTry");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerOpen,"VIDECUDA_PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerClose,"VIDECUDA_PlayerClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerPlayFrame,"VIDECUDA_PlayerPlayFrame");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerGetWidth,"VIDECUDA_PlayerGetWidth");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerGetHeight,"VIDECUDA_PlayerGetHeight");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerResize,"VIDECUDA_PlayerResize");
		m_pXDllLoadLibrary->LoadFunction(pVIDECUDA_PlayerSkipDisplay,"VIDECUDA_PlayerSkipDisplay");
	}
}

VIDECUDAWrapper::~VIDECUDAWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VIDECUDAWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}


int cudaInit(void)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
		{
			NEWRECORD( g_pVIDECUDAWrapper ,new VIDECUDAWrapper(g_hInstance) )
			//NULLRecord( g_pVIDECUDAWrapper )  
			if (!g_pVIDECUDAWrapper->IsOK() || g_pVIDECUDAWrapper->pVIDECUDA_Init(timeGetTime())!=0)
			{
				delete g_pVIDECUDAWrapper;
				g_pVIDECUDAWrapper=NULL;
				return -1;
			}
			return 0;
		}
	}catch(...){

	}

	return -1;
}

void cudaTerminate(void)
{
	try{
		if (g_pVIDECUDAWrapper)
		{
			delete g_pVIDECUDAWrapper;
			g_pVIDECUDAWrapper=NULL;
		}
	}catch(...){

	}
}

void cudaPlayerSetMaxCount(unsigned int nMaxPlayer)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return ;
		g_pVIDECUDAWrapper->pVIDECUDA_PlayerSetMaxCount(nMaxPlayer);
	}
	catch(...)
	{
	}
}

int cudaPlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return -1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerTry(hPlayer,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return -1;
}


void*cudaPlayerOpen(HWND hWnd, int nWidth, int nHeight)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return NULL;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerOpen(hWnd,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return NULL;
}

void cudaPlayerClose(void*hPlayer)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return ;
		g_pVIDECUDAWrapper->pVIDECUDA_PlayerClose(hPlayer);
	}
	catch(...)
	{
	}
}

int cudaPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen,int nFrameRate, int&nFlagReset)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return -1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerPlayFrame(hPlayer,pFrameData,nFrameLen,nFrameRate,nFlagReset);
	}
	catch(...)
	{
	}
	return -1;
}

int cudaPlayerGetWidth(void*hPlayer)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return 1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerGetWidth(hPlayer);
	}
	catch(...)
	{
	}
	return 1280;
}


int cudaPlayerGetHeight(void*hPlayer)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return 1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerGetHeight(hPlayer);
	}
	catch(...)
	{
	}
	return 720;
}


int cudaPlayerResize(void*hPlayer)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return 1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerResize(hPlayer);
	}
	catch(...)
	{
	}
	return -1;
}

int cudaPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	try{
		if (g_pVIDECUDAWrapper==NULL)
			return 1;
		return g_pVIDECUDAWrapper->pVIDECUDA_PlayerSkipDisplay(hPlayer,nFlagSkipDisplay);
	}
	catch(...)
	{
	}
	return -1;
}
