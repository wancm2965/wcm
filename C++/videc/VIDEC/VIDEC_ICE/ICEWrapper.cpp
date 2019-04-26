#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "ICEWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static ICEWrapper* g_pICEWrapper=NULL;
extern HINSTANCE g_hInstance;


ICEWrapper&ICEWrapper::Instance(void)
{
	return *g_pICEWrapper;
}

ICEWrapper::ICEWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECICE.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary ,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECICE_Open,"VIDECICE_Open");
		m_pXDllLoadLibrary->LoadFunction(pVIDECICE_Close,"VIDECICE_Close");
		m_pXDllLoadLibrary->LoadFunction(pVIDECICE_Process,"VIDECICE_Process");
	}
}

ICEWrapper::~ICEWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool ICEWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void ICE_init(void)
{
	try{
		if (g_pICEWrapper==NULL)
		{
			NEWRECORD( g_pICEWrapper,new ICEWrapper(g_hInstance) )
				NULLRecord( g_pICEWrapper )  
			if (!g_pICEWrapper->IsOK())
			{
				delete g_pICEWrapper;
				g_pICEWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void ICE_terminate(void)
{
	try{
		if (g_pICEWrapper)
		{
			delete g_pICEWrapper;
			g_pICEWrapper=NULL;
		}
	}catch(...){

	}
}


void*ICEOpen(int nWidth,int nHeight)
{
	try{
		if (g_pICEWrapper==NULL)
			return NULL;
		return g_pICEWrapper->pVIDECICE_Open(nWidth,nHeight,timeGetTime());
	}
	catch(...)
	{
	}
	return NULL;
}

void ICEClose(void*pHandle)
{
	try{
		if (g_pICEWrapper==NULL)
			return ;
		g_pICEWrapper->pVIDECICE_Close(pHandle);
	}
	catch(...)
	{
	}
}

int ICEProcess(void*pHandle,unsigned char*pU,unsigned char*pV)
{
	try{
		if (g_pICEWrapper==NULL)
			return -1;
		return g_pICEWrapper->pVIDECICE_Process(pHandle,pU,pV);
	}
	catch(...)
	{
	}
	return -1;
}
