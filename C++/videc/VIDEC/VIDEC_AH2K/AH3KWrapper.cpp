#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "AH3KWrapper.h"



static AH3KWrapper* g_pAH3KWrapper=NULL;
extern HINSTANCE g_hInstance;


AH3KWrapper&AH3KWrapper::Instance(void)
{
	return *g_pAH3KWrapper;
}

AH3KWrapper::AH3KWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sAH3K.dll",szTemp);

	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pAH3K_GetDevCount,"AH3K_GetDevCount");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_GetDevName,"AH3K_GetDevName");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_HasAudio,"AH3K_HasAudio");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_IsHD,"AH3K_IsHD");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_Open,"AH3K_Open");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_Close,"AH3K_Close");
		m_pXDllLoadLibrary->LoadFunction(pAH3K_SetParams,"AH3K_SetParams");
	}
}

AH3KWrapper::~AH3KWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool AH3KWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void AH3K_init(void)
{
	try{
		if (g_pAH3KWrapper==NULL)
		{
			g_pAH3KWrapper=new AH3KWrapper(g_hInstance);
			if (!g_pAH3KWrapper->IsOK())
			{
				delete g_pAH3KWrapper;
				g_pAH3KWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void AH3K_terminate(void)
{
	try{
		if (g_pAH3KWrapper)
		{
			delete g_pAH3KWrapper;
			g_pAH3KWrapper=NULL;
		}
	}catch(...){

	}
}


int AH3KGetDevCount(void)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return 0;
		return g_pAH3KWrapper->pAH3K_GetDevCount();
	}
	catch(...)
	{
	}
	return 0;
}

int AH3KGetDevName(int nDevID,char*szName,int nCount)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return -1;
		return g_pAH3KWrapper->pAH3K_GetDevName(nDevID,szName,nCount);
	}
	catch(...)
	{
	}
	return -1;
}

int AH3KHasAudio(int nDevID)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return 0;
		return g_pAH3KWrapper->pAH3K_HasAudio(nDevID);
	}
	catch(...)
	{
	}
	return 0;
}

int AH3KIsHD(int nDevID)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return 0;
		return g_pAH3KWrapper->pAH3K_IsHD(nDevID);
	}
	catch(...)
	{
	}
	return 0;
}

void*AH3KOpen(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return NULL;
		return g_pAH3KWrapper->pAH3K_Open(nDevID,video_cb,audio_cb,pUserData);
	}
	catch(...)
	{
	}
	return NULL;
}

void AH3KClose(void*pHandle)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return ;
		g_pAH3KWrapper->pAH3K_Close(pHandle);
	}
	catch(...)
	{
	}
}

int AH3KSetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	try{
		if (g_pAH3KWrapper==NULL)
			return -1;
		return g_pAH3KWrapper->pAH3K_SetParams(pHandle,nScaleType,nWidth,nHeight,nFrameRate);
	}
	catch(...)
	{
	}
	return -1;
}
