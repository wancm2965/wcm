#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "VQDWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static VQDWrapper* g_pVQDWrapper=NULL;
extern HINSTANCE g_hInstance;


VQDWrapper&VQDWrapper::Instance(void)
{
	return *g_pVQDWrapper;
}

VQDWrapper::VQDWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECVQD.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECVQD_Open,"VIDECVQD_Open");
		m_pXDllLoadLibrary->LoadFunction(pVIDECVQD_Close,"VIDECVQD_Close");
		m_pXDllLoadLibrary->LoadFunction(pVIDECVQD_DetectFrame,"VIDECVQD_DetectFrame");
		m_pXDllLoadLibrary->LoadFunction(pVIDECVQD_SetParams,"VIDECVQD_SetParams");
	}
}

VQDWrapper::~VQDWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VQDWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void VQDInit(void)
{
	try{
		if (g_pVQDWrapper==NULL)
		{
			NEWRECORD( g_pVQDWrapper ,new VQDWrapper(g_hInstance) )
			NULLRecord( g_pVQDWrapper )  

			if (!g_pVQDWrapper->IsOK())
			{
				delete g_pVQDWrapper;
				g_pVQDWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void VQDTerminate(void)
{
	try{
		if (g_pVQDWrapper)
		{
			delete g_pVQDWrapper;
			g_pVQDWrapper=NULL;
		}
	}catch(...){

	}
}

void*VQDOpen(unsigned long nDetectFlags)
{
	try{
		if (g_pVQDWrapper==NULL)
			return NULL;

		return g_pVQDWrapper->pVIDECVQD_Open(nDetectFlags);
	}
	catch(...)
	{
	}
	return NULL;
}

unsigned long VQDDetectFrame(void*pHandle, unsigned char *pFrameData, int nWidth, int nHeight)
{
	try{
		if (g_pVQDWrapper==NULL)
			return VQDS_DONE;
		return g_pVQDWrapper->pVIDECVQD_DetectFrame(pHandle,pFrameData,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return VQDS_DONE;
}

void VQDClose(void*pHandle)
{
	try{
		if (g_pVQDWrapper==NULL)
			return ;
		g_pVQDWrapper->pVIDECVQD_Close(pHandle);
	}
	catch(...)
	{
	}
}

void VQDSetParams(unsigned long nDetectFlag,int nThreshold)
{
	try{
		if (g_pVQDWrapper==NULL)
			return ;
		g_pVQDWrapper->pVIDECVQD_SetParams(nDetectFlag,nThreshold);
	}
	catch(...)
	{
	}
}