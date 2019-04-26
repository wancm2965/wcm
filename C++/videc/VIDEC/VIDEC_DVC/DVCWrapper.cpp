#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "DVCWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static DVCWrapper* g_pDVCWrapper=NULL;
extern HINSTANCE g_hInstance;


DVCWrapper&DVCWrapper::Instance(void)
{
	return *g_pDVCWrapper;
}

DVCWrapper::DVCWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sDVC.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary ,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pDVC_SetIP,"DVC_SetIP");
		m_pXDllLoadLibrary->LoadFunction(pDVC_GetDevCount,"DVC_GetDevCount");
		m_pXDllLoadLibrary->LoadFunction(pDVC_GetDevName,"DVC_GetDevName");
		m_pXDllLoadLibrary->LoadFunction(pDVC_Open,"DVC_Open");
		m_pXDllLoadLibrary->LoadFunction(pDVC_Close,"DVC_Close");
		m_pXDllLoadLibrary->LoadFunction(pDVC_SetParams,"DVC_SetParams");
		m_pXDllLoadLibrary->LoadFunction(pDVC_GetStatus,"DVC_GetStatus");
		m_pXDllLoadLibrary->LoadFunction(pDVC_RequestKeyFrame,"DVC_RequestKeyFrame");
		m_pXDllLoadLibrary->LoadFunction(pDVC_SendData,"DVC_SendData");
	}
}

DVCWrapper::~DVCWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool DVCWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void DVC_init(void)
{
	try{
		if (g_pDVCWrapper==NULL)
		{
			NEWRECORD( g_pDVCWrapper,new DVCWrapper(g_hInstance) )
			NULLRecord( g_pDVCWrapper )  
			if (!g_pDVCWrapper->IsOK())
			{
				delete g_pDVCWrapper;
				g_pDVCWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void DVC_terminate(void)
{
	try{
		if (g_pDVCWrapper)
		{
			delete g_pDVCWrapper;
			g_pDVCWrapper=NULL;
		}
	}catch(...){

	}
}

int DVCSetIP(const char*cszIP,unsigned short nPort,const char*cszPassword)
{
	try{
		if (g_pDVCWrapper==NULL)
			return 0;
		return g_pDVCWrapper->pDVC_SetIP(cszIP,nPort,cszPassword);
	}
	catch(...)
	{
	}
	return 0;
}

int DVCGetDevCount(void)
{
	try{
		if (g_pDVCWrapper==NULL)
			return 0;
		return g_pDVCWrapper->pDVC_GetDevCount();
	}
	catch(...)
	{
	}
	return 0;
}

int DVCGetDevName(int nDevID,char*szName,int nCount)
{
	try{
		if (g_pDVCWrapper==NULL)
			return -1;
		return g_pDVCWrapper->pDVC_GetDevName(nDevID,szName,nCount);
	}
	catch(...)
	{
	}
	return -1;
}


void*DVCOpen(int nDevID,DVC_VIDEO_CB video_cb,const void*pUserData)
{
	try{
		if (g_pDVCWrapper==NULL)
			return NULL;
		return g_pDVCWrapper->pDVC_Open(nDevID,video_cb,pUserData);
	}
	catch(...)
	{
	}
	return NULL;
}

void DVCClose(void*pHandle)
{
	try{
		if (g_pDVCWrapper==NULL)
			return ;
		g_pDVCWrapper->pDVC_Close(pHandle);
	}
	catch(...)
	{
	}
}

int DVCSetParams(void*pHandle,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	try{
		if (g_pDVCWrapper==NULL)
			return -1;
		return g_pDVCWrapper->pDVC_SetParams(pHandle,nFrameRate,nBitrate,nBitrateControlType,nResolution);
	}
	catch(...)
	{
	}
	return -1;
}

int DVCGetStatus(void*pHandle)
{
	try{
		if (g_pDVCWrapper==NULL)
			return -1;
		return g_pDVCWrapper->pDVC_GetStatus(pHandle);
	}
	catch(...)
	{
	}
	return -1;
}

int DVCRequestKeyFrame(void*pHandle)
{
	try{
		if (g_pDVCWrapper==NULL)
			return -1;
		return g_pDVCWrapper->pDVC_RequestKeyFrame(pHandle);
	}
	catch(...)
	{
	}
	return -1;
}


int DVCSendData(void*pHandle,const char*pData,int nLen)
{
	try{
		if (g_pDVCWrapper==NULL)
			return -1;
		return g_pDVCWrapper->pDVC_SendData(pHandle,pData,nLen);
	}
	catch(...)
	{
	}
	return -1;
}
