#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "AVMSWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static AVMSWrapper* g_pAVMSWrapper=NULL;
extern HINSTANCE g_hInstance;


AVMSWrapper&AVMSWrapper::Instance(void)
{
	return *g_pAVMSWrapper;
}

AVMSWrapper::AVMSWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%savmsp.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_Open,"AVMSP_Open");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_Close,"AVMSP_Close");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_Seek,"AVMSP_Seek");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_Play,"AVMSP_Play");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_Pause,"AVMSP_Pause");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_SetVolume,"AVMSP_SetVolume");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_GetDuration,"AVMSP_GetDuration");
		m_pXDllLoadLibrary->LoadFunction(pAVMSP_GetPosition,"AVMSP_GetPosition");
	}
}

AVMSWrapper::~AVMSWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool AVMSWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}

void AVMSP_init(void)
{
	//DebugOutput("AVMS_init\r\n");

	try{
		if (g_pAVMSWrapper==NULL)
		{
			NEWRECORD( g_pAVMSWrapper,new AVMSWrapper(g_hInstance) )
			NULLRecord( g_pAVMSWrapper )  
			if (!g_pAVMSWrapper->IsOK())
			{
				delete g_pAVMSWrapper;
				g_pAVMSWrapper=NULL;
			}
		}
	}catch(...){

	}
}

void AVMSP_terminate(void)
{
	//DebugOutput("AVMS_terminate\r\n");
	try{
		if (g_pAVMSWrapper)
		{
			delete g_pAVMSWrapper;
			g_pAVMSWrapper=NULL;
		}
	}catch(...){

	}
}

void*AVMSP_Open(const char*cszFile,AVMS_PLUGIN_CALLBACK_PCM_DATA cb_pcm_data,AVMS_PLUGIN_CALLBACK_VIDEO_PACKET cb_video_packet,const void*pUserData,unsigned long*lpnVideoTimestamp,unsigned long*lpnAudioTimestamp)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return NULL;

		return g_pAVMSWrapper->pAVMSP_Open(cszFile,cb_pcm_data,cb_video_packet,pUserData,lpnVideoTimestamp,lpnAudioTimestamp);
	}
	catch(...)
	{
		return NULL;
	}
}

void AVMSP_Close(void*pHandle)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return ;
		g_pAVMSWrapper->pAVMSP_Close(pHandle);
	}
	catch(...)
	{
	}
}

int AVMSP_Seek(void*pHandle,unsigned long ulSeekTime)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return -1;
		return g_pAVMSWrapper->pAVMSP_Seek(pHandle,ulSeekTime);
	}
	catch(...)
	{
		return -1;
	}
}

int AVMSP_Play(void*pHandle)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return -1;
		return g_pAVMSWrapper->pAVMSP_Play(pHandle);
	}
	catch(...)
	{
		return -1;
	}
}

int AVMSP_Pause(void*pHandle)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return -1;
		return g_pAVMSWrapper->pAVMSP_Pause(pHandle);
	}
	catch(...)
	{
		return -1;
	}
}

int AVMSP_SetVolume(void*pHandle,int nValue)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return -1;
		return g_pAVMSWrapper->pAVMSP_SetVolume(pHandle,nValue);
	}
	catch(...)
	{
		return -1;
	}
}

unsigned long AVMSP_GetDuration(void*pHandle)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return 0;
		return g_pAVMSWrapper->pAVMSP_GetDuration(pHandle);
	}
	catch(...)
	{
		return 0;
	}
}

unsigned long AVMSP_GetPosition(void*pHandle)
{
	try{
		if (g_pAVMSWrapper==NULL)
			return 0;
		return g_pAVMSWrapper->pAVMSP_GetPosition(pHandle);
	}
	catch(...)
	{
		return 0;
	}
}
