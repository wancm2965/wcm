#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "VIDECD3DWrapper.h"



static VIDECD3DWrapper* g_pVIDECD3DWrapper=NULL;
extern HINSTANCE g_hInstance;

int D3DSupportPlayer(void)
{
	return (g_pVIDECD3DWrapper!=NULL?1:0);
}


VIDECD3DWrapper&VIDECD3DWrapper::Instance(void)
{
	return *g_pVIDECD3DWrapper;
}

VIDECD3DWrapper::VIDECD3DWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECD3D.dll",szTemp);

	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_Init,"VIDECD3D_Init");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerOpen,"VIDECD3D_PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerClose,"VIDECD3D_PlayerClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerPlayFrame,"VIDECD3D_PlayerPlayFrame");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerGetWidth,"VIDECD3D_PlayerGetWidth");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerGetHeight,"VIDECD3D_PlayerGetHeight");
		m_pXDllLoadLibrary->LoadFunction(pVIDECD3D_PlayerSkipDisplay,"VIDECD3D_PlayerSkipDisplay");
	}
}

VIDECD3DWrapper::~VIDECD3DWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VIDECD3DWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}


int D3DInit(void)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
		{
			g_pVIDECD3DWrapper=new VIDECD3DWrapper(g_hInstance);
			if (!g_pVIDECD3DWrapper->IsOK() || g_pVIDECD3DWrapper->pVIDECD3D_Init(timeGetTime())!=0)
			{
				delete g_pVIDECD3DWrapper;
				g_pVIDECD3DWrapper=NULL;
				return -1;
			}
			return 0;
		}
	}catch(...){

	}

	return -1;
}

void D3DTerminate(void)
{
	try{
		if (g_pVIDECD3DWrapper)
		{
			delete g_pVIDECD3DWrapper;
			g_pVIDECD3DWrapper=NULL;
		}
	}catch(...){

	}
}


void*D3DPlayerOpen(HWND hWnd, int nWidth, int nHeight)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return NULL;
		return g_pVIDECD3DWrapper->pVIDECD3D_PlayerOpen(hWnd,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return NULL;
}

void D3DPlayerClose(void*hPlayer)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return ;
		g_pVIDECD3DWrapper->pVIDECD3D_PlayerClose(hPlayer);
	}
	catch(...)
	{
	}
}

int D3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen,int nFrameRate,int nWidth,int nHeight)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return -1;
		return g_pVIDECD3DWrapper->pVIDECD3D_PlayerPlayFrame(hPlayer,pFrameData,nFrameLen,nFrameRate,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return -1;
}

int D3DPlayerGetWidth(void*hPlayer)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return 1;
		return g_pVIDECD3DWrapper->pVIDECD3D_PlayerGetWidth(hPlayer);
	}
	catch(...)
	{
	}
	return 1280;
}


int D3DPlayerGetHeight(void*hPlayer)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return 1;
		return g_pVIDECD3DWrapper->pVIDECD3D_PlayerGetHeight(hPlayer);
	}
	catch(...)
	{
	}
	return 720;
}


int D3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	try{
		if (g_pVIDECD3DWrapper==NULL)
			return 1;
		return g_pVIDECD3DWrapper->pVIDECD3D_PlayerSkipDisplay(hPlayer,nFlagSkipDisplay);
	}
	catch(...)
	{
	}
	return -1;
}
