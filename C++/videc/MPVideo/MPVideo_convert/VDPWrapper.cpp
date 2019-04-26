#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "VDPWrapper.h"
#include "XUtil.h"



static VDPWrapper* g_pVDPWrapper=NULL;
extern HINSTANCE g_hInstance;


VDPWrapper&VDPWrapper::Instance(void)
{
	return *g_pVDPWrapper;
}

VDPWrapper::VDPWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECVDP.dll",szTemp);

	pVDResizeOpen=NULL;
	pVDResizeGetFrame=NULL;
	pVDResizeClose=NULL;
	pVDResampleGetFrame=NULL;


	m_pXDllLoadLibrary=new XDllLoadLibrary(szPathName);

	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVDResizeOpen,"VDResize_Open");
		m_pXDllLoadLibrary->LoadFunction(pVDResizeGetFrame,"VDResize_GetFrame");
		m_pXDllLoadLibrary->LoadFunction(pVDResizeClose,"VDResize_Close");
		m_pXDllLoadLibrary->LoadFunction(pVDResampleGetFrame,"VDResample_GetFrame");
	}
}

VDPWrapper::~VDPWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VDPWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void VDResizeInit(void)
{
	try{
		if (g_pVDPWrapper==NULL)
		{
			g_pVDPWrapper=new VDPWrapper(g_hInstance);
			if (!g_pVDPWrapper->IsOK())
			{
				delete g_pVDPWrapper;
				g_pVDPWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void VDResizeUninit(void)
{
	try{
		if (g_pVDPWrapper)
		{
			delete g_pVDPWrapper;
			g_pVDPWrapper=NULL;
		}
	}catch(...){

	}
}

void* VDResizeOpen(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride)
{
	try{
		if (g_pVDPWrapper==NULL)
			return NULL;
		return g_pVDPWrapper->pVDResizeOpen(nSrcWidth,  nSrcHeight,  nSrcStride,  nDstWidth,  nDstHeight,  nDstStride);
	}
	catch(...)
	{
	}

	return NULL;
}


void VDResizeGetFrame(void*pHandle,unsigned char*srcData[], unsigned char*dstData[])
{
	try{
		if (g_pVDPWrapper==NULL)
			return;
		return g_pVDPWrapper->pVDResizeGetFrame(pHandle,srcData,dstData);
	}
	catch(...)
	{
	}
}


void VDResizeClose(void*pHandle)
{
	try{
		if (g_pVDPWrapper==NULL)
			return ;
		g_pVDPWrapper->pVDResizeClose(pHandle);
	}
	catch(...)
	{
	}
}

bool VDResampleGetFrame(int nSrcWidth, int nSrcHeight, int nSrcStride, int nDstWidth, int nDstHeight, int nDstStride, unsigned char*srcData[], unsigned char*dstData[])
{
	try{
		if (g_pVDPWrapper==NULL)
			return false;
		return g_pVDPWrapper->pVDResampleGetFrame(nSrcWidth, nSrcHeight, nSrcStride, nDstWidth, nDstHeight, nDstStride, srcData, dstData);
	}
	catch(...)
	{
	}
	return false;
}