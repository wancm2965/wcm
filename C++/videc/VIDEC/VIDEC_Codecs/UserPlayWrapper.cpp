#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "UserPlayWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static UserPlayWrapper* g_pUserPlayWrapper=NULL;
extern HINSTANCE g_hInstance;


UserPlayWrapper&UserPlayWrapper::Instance(void)
{
	return *g_pUserPlayWrapper;
}

UserPlayWrapper::UserPlayWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%svidecusr.dll",szTemp);

	pUserPlayerInit=NULL;
	pUserPlayerTerminate=NULL;

	NEWRECORD( m_pXDllLoadLibrary,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pUserPlayerInit,"VIDEC_UserPlayerInit");
		m_pXDllLoadLibrary->LoadFunction(pUserPlayerTerminate,"VIDEC_UserPlayerTerminate");
		m_pXDllLoadLibrary->LoadFunction(pUserPlayerOpen,"VIDEC_UserPlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pUserPlayerInputData,"VIDEC_UserPlayerInputData");
		m_pXDllLoadLibrary->LoadFunction(pUserPlayerClose,"VIDEC_UserPlayerClose");

		if (pUserPlayerInit)
		{
			pUserPlayerInit();
		}
	}
}

UserPlayWrapper::~UserPlayWrapper()
{
	if (pUserPlayerTerminate)
	{
		pUserPlayerTerminate();
	}

	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool UserPlayWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void UserPlay_init(void)
{
	try{
		if (g_pUserPlayWrapper==NULL)
		{
			NEWRECORD( g_pUserPlayWrapper ,new UserPlayWrapper(g_hInstance) )
			//NULLRecord( g_pUserPlayWrapper )  
			if (!g_pUserPlayWrapper->IsOK())
			{
				delete g_pUserPlayWrapper;
				g_pUserPlayWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void UserPlay_terminate(void)
{
	try{
		if (g_pUserPlayWrapper)
		{
			delete g_pUserPlayWrapper;
			g_pUserPlayWrapper=NULL;
		}
	}catch(...){

	}
}

void*UserPlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
{
	try{
		if (g_pUserPlayWrapper==NULL)
			return NULL;

		return g_pUserPlayWrapper->pUserPlayerOpen(yuv_image_cb,pUser);
	}
	catch(...)
	{
		return FALSE;
	}
}

void UserPlayerInputData(void*pHandle,unsigned char*pData,int nLen)
{
	try{
		if (g_pUserPlayWrapper==NULL)
			return ;
		g_pUserPlayWrapper->pUserPlayerInputData(pHandle,pData,nLen);
	}
	catch(...)
	{
	}
}

void UserPlayerClose(void*pHandle)
{
	try{
		if (g_pUserPlayWrapper==NULL)
			return ;
		g_pUserPlayWrapper->pUserPlayerClose(pHandle);
	}
	catch(...)
	{
	}
}
