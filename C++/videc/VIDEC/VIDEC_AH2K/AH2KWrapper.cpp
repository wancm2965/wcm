#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "AH2KWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static AH2KWrapper* g_pAH2KWrapper=NULL;
extern HINSTANCE g_hInstance;


AH2KWrapper&AH2KWrapper::Instance(void)
{
	return *g_pAH2KWrapper;
}

AH2KWrapper::AH2KWrapper(HINSTANCE hInstance)
{
	//先查找C:\Windows\System32 目录，如果没有就查找安装目录下的AH2K.dll
	{
		char szPathName[1024]="C:/Windows/System32/AH2K.dll";
		NEWRECORD( m_pXDllLoadLibrary , new XDllLoadLibrary(szPathName) );

		NULLReturn( m_pXDllLoadLibrary  )  

		if (m_pXDllLoadLibrary)
		{
			m_pXDllLoadLibrary->LoadFunction(pAH2K_GetDevCount,"AH2K_GetDevCount");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_GetDevName,"AH2K_GetDevName");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_HasAudio,"AH2K_HasAudio");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_IsHD,"AH2K_IsHD");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_Open,"AH2K_Open");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_Close,"AH2K_Close");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_SetParams,"AH2K_SetParams");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_SetMemory,"AH2K_SetMemory");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_HasVideoSignal,"AH2K_HasVideoSignal");
		}

		if (IsOK())
		{
			return;
		}
		else
		{
			if (m_pXDllLoadLibrary)
			{
				delete m_pXDllLoadLibrary;
				m_pXDllLoadLibrary=NULL;
			}
		}
	}

	{
		char szPathName[1024];
		char szTemp[MAX_PATH];
		GetModuleFileName(hInstance, szTemp, MAX_PATH);
		char* szEnd = strrchr(szTemp, '\\') + 1;
		*szEnd = '\0';
		sprintf(szPathName,"%sAH2K.dll",szTemp);

		NEWRECORD( m_pXDllLoadLibrary ,new XDllLoadLibrary(szPathName) );
NULLReturn( m_pXDllLoadLibrary  )  
		if (m_pXDllLoadLibrary)
		{
			m_pXDllLoadLibrary->LoadFunction(pAH2K_GetDevCount,"AH2K_GetDevCount");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_GetDevName,"AH2K_GetDevName");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_HasAudio,"AH2K_HasAudio");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_IsHD,"AH2K_IsHD");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_Open,"AH2K_Open");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_Close,"AH2K_Close");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_SetParams,"AH2K_SetParams");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_SetMemory,"AH2K_SetMemory");
			m_pXDllLoadLibrary->LoadFunction(pAH2K_HasVideoSignal,"AH2K_HasVideoSignal");
		}
	}
}

AH2KWrapper::~AH2KWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool AH2KWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void AH2K_init(void)
{
	try{
		if (g_pAH2KWrapper==NULL)
		{
			NEWRECORD( g_pAH2KWrapper , new AH2KWrapper(g_hInstance) );
			NULLReturn( g_pAH2KWrapper  )  

			if (!g_pAH2KWrapper->IsOK())
			{
				delete g_pAH2KWrapper;
				g_pAH2KWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void AH2K_terminate(void)
{
	try{
		if (g_pAH2KWrapper)
		{
			delete g_pAH2KWrapper;
			g_pAH2KWrapper=NULL;
		}
	}catch(...){

	}
}


int AH2KGetDevCount(void)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return 0;
		return g_pAH2KWrapper->pAH2K_GetDevCount();
	}
	catch(...)
	{
	}
	return 0;
}

int AH2KGetDevName(int nDevID,char*szName,int nCount)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return -1;
		return g_pAH2KWrapper->pAH2K_GetDevName(nDevID,szName,nCount);
	}
	catch(...)
	{
	}
	return -1;
}

int AH2KHasAudio(int nDevID)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return 0;
		return g_pAH2KWrapper->pAH2K_HasAudio(nDevID);
	}
	catch(...)
	{
	}
	return 0;
}

int AH2KIsHD(int nDevID)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return 0;
		return g_pAH2KWrapper->pAH2K_IsHD(nDevID);
	}
	catch(...)
	{
	}
	return 0;
}

void*AH2KOpen(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return NULL;
		return g_pAH2KWrapper->pAH2K_Open(nDevID,video_cb,audio_cb,pUserData);
	}
	catch(...)
	{
	}
	return NULL;
}

void AH2KClose(void*pHandle)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return ;
		g_pAH2KWrapper->pAH2K_Close(pHandle);
	}
	catch(...)
	{
	}
}

int AH2KSetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return -1;
		return g_pAH2KWrapper->pAH2K_SetParams(pHandle,nScaleType,nWidth,nHeight,nFrameRate);
	}
	catch(...)
	{
	}
	return -1;
}

void AH2KSetMemory(AH2K_MALLOC Malloc,AH2K_FREE Free)
{
	try{
		if (g_pAH2KWrapper==NULL)
			return ;
		g_pAH2KWrapper->pAH2K_SetMemory(Malloc,Free);
	}
	catch(...)
	{
	}
}

int AH2KHasVideoSignal(void*pHandle)
{
	try{
		if (g_pAH2KWrapper==NULL && g_pAH2KWrapper->pAH2K_HasVideoSignal)
			return 1;
		return g_pAH2KWrapper->pAH2K_HasVideoSignal(pHandle);
	}
	catch(...)
	{
	}
	return 1;
}
