#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "DH264PlayWrapper.h"



static DH264PlayWrapper* g_pDH264PlayWrapper=NULL;
extern HINSTANCE g_hInstance;


DH264PlayWrapper&DH264PlayWrapper::Instance(void)
{
	return *g_pDH264PlayWrapper;
}

DH264PlayWrapper::DH264PlayWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%svidecdhp.dll",szTemp);

	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pDH264PlayerOpen,"MPVIDEO_DH264PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pDH264PlayerInputData,"MPVIDEO_DH264PlayerInputData");
		m_pXDllLoadLibrary->LoadFunction(pDH264PlayerClose,"MPVIDEO_DH264PlayerClose");
	}
}

DH264PlayWrapper::~DH264PlayWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool DH264PlayWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void DH264Play_init(void)
{
	try{
		if (g_pDH264PlayWrapper==NULL)
		{
			g_pDH264PlayWrapper=new DH264PlayWrapper(g_hInstance);
			if (!g_pDH264PlayWrapper->IsOK())
			{
				delete g_pDH264PlayWrapper;
				g_pDH264PlayWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void DH264Play_terminate(void)
{
	try{
		if (g_pDH264PlayWrapper)
		{
			delete g_pDH264PlayWrapper;
			g_pDH264PlayWrapper=NULL;
		}
	}catch(...){

	}
}

void*DH264PlayerOpen(YUV_IMAGE_CALLBACK yuv_image_cb,void*pUser)
{
	try{
		if (g_pDH264PlayWrapper==NULL)
			return NULL;

		return g_pDH264PlayWrapper->pDH264PlayerOpen(yuv_image_cb,pUser);
	}
	catch(...)
	{
		return FALSE;
	}
}

void DH264PlayerInputData(void*pHandle,unsigned char*pData,int nLen)
{
	try{
		if (g_pDH264PlayWrapper==NULL)
			return ;
		g_pDH264PlayWrapper->pDH264PlayerInputData(pHandle,pData,nLen);
	}
	catch(...)
	{
	}
}

void DH264PlayerClose(void*pHandle)
{
	try{
		if (g_pDH264PlayWrapper==NULL)
			return ;
		g_pDH264PlayWrapper->pDH264PlayerClose(pHandle);
	}
	catch(...)
	{
	}
}
