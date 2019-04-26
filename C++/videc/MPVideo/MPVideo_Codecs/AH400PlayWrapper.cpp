#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "AH400PlayWrapper.h"



static AH400PlayWrapper* g_pAH400PlayWrapper=NULL;
extern HINSTANCE g_hInstance;


AH400PlayWrapper&AH400PlayWrapper::Instance(void)
{
	return *g_pAH400PlayWrapper;
}

AH400PlayWrapper::AH400PlayWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%svidechkp.dll",szTemp);

	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pAH400PlayerOpen,"VIDEC_AH400PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pAH400PlayerInputData,"VIDEC_AH400PlayerInputData");
		m_pXDllLoadLibrary->LoadFunction(pAH400PlayerClose,"VIDEC_AH400PlayerClose");
	}
}

AH400PlayWrapper::~AH400PlayWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool AH400PlayWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void AH400Play_init(void)
{
	//OutputDebugString("AH400Play_init\r\n");

	try{
		if (g_pAH400PlayWrapper==NULL)
		{
			g_pAH400PlayWrapper=new AH400PlayWrapper(g_hInstance);
			if (!g_pAH400PlayWrapper->IsOK())
			{
				delete g_pAH400PlayWrapper;
				g_pAH400PlayWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void AH400Play_terminate(void)
{
	//OutputDebugString("AH400Play_terminate\r\n");
	try{
		if (g_pAH400PlayWrapper)
		{
			delete g_pAH400PlayWrapper;
			g_pAH400PlayWrapper=NULL;
		}
	}catch(...){

	}
}

void*AH400PlayerOpen(HIK_TYPE nType,YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
{
	try{
		if (g_pAH400PlayWrapper==NULL)
			return NULL;

		return g_pAH400PlayWrapper->pAH400PlayerOpen(nType,yuv_image_cb,pUser);
	}
	catch(...)
	{
		return FALSE;
	}
}

void AH400PlayerInputData(void*pHandle,unsigned char*pData,int nLen)
{
	try{
		if (g_pAH400PlayWrapper==NULL)
			return ;
		g_pAH400PlayWrapper->pAH400PlayerInputData(pHandle,pData,nLen);
	}
	catch(...)
	{
	}
}

void AH400PlayerClose(void*pHandle)
{
	try{
		if (g_pAH400PlayWrapper==NULL)
			return ;
		g_pAH400PlayWrapper->pAH400PlayerClose(pHandle);
	}
	catch(...)
	{
	}
}
