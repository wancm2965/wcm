#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "VIDECYNCWrapper.h"
#include "../VIDEC_FRE/VIDECFREHeader.h"


static VIDECYNCWrapper* g_pVIDECYNCWrapper=NULL;
static VIDECYNCD3DWrapper* g_pVIDECYNCD3DWrapper=NULL;
extern HINSTANCE g_hInstance;

int SyncSupportPlayer(void)
{
	return (g_pVIDECYNCWrapper!=NULL?1:0);
}


VIDECYNCWrapper&VIDECYNCWrapper::Instance(void)
{
	return *g_pVIDECYNCWrapper;
}

VIDECYNCWrapper::VIDECYNCWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECYNC.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary ,new XDllLoadLibrary(szPathName) )
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_Init,"VIDECYNC_Init");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerSetMaxCount,"VIDECYNC_PlayerSetMaxCount");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerTry,"VIDECYNC_PlayerTry");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerOpen,"VIDECYNC_PlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerClose,"VIDECYNC_PlayerClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerPlayFrame,"VIDECYNC_PlayerPlayFrame");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerGetWidth,"VIDECYNC_PlayerGetWidth");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerGetHeight,"VIDECYNC_PlayerGetHeight");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerResize,"VIDECYNC_PlayerResize");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerSkipDisplay,"VIDECYNC_PlayerSkipDisplay");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerGetFrameRate,"VIDECYNC_PlayerGetFrameRate");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerGetShowType,"VIDECYNC_PlayerGetShowType");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_PlayerCaptureToBMPFile,"VIDECYNC_PlayerCaptureToBMPFile");
	}
}

VIDECYNCWrapper::~VIDECYNCWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VIDECYNCWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}


int SyncInit(void)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
		{
			NEWRECORD( g_pVIDECYNCWrapper,new VIDECYNCWrapper(g_hInstance) )
			NULLRecord( g_pVIDECYNCWrapper )  
			if (!g_pVIDECYNCWrapper->IsOK() || g_pVIDECYNCWrapper->pVIDECYNC_Init(timeGetTime())!=0)
			{
				delete g_pVIDECYNCWrapper;
				g_pVIDECYNCWrapper=NULL;
				return -1;
			}
			return 0;
		}
	}catch(...){

	}

	return -1;
}

void SyncTerminate(void)
{
	try{
		if (g_pVIDECYNCWrapper)
		{
			delete g_pVIDECYNCWrapper;
			g_pVIDECYNCWrapper=NULL;
		}
	}catch(...){

	}
}

void SyncPlayerSetMaxCount(unsigned int nMaxPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return ;
		g_pVIDECYNCWrapper->pVIDECYNC_PlayerSetMaxCount(nMaxPlayer);
	}
	catch(...)
	{
	}
}

int SyncPlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return -1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerTry(hPlayer,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return -1;
}


void*SyncPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd, int nWidth, int nHeight)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return NULL;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerOpen(cbdc,pUserData,hWnd,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return NULL;
}

void SyncPlayerClose(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return ;
		g_pVIDECYNCWrapper->pVIDECYNC_PlayerClose(hPlayer);
	}
	catch(...)
	{
	}
}

int SyncPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate,int&nFlagReset)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return -1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerPlayFrame(hPlayer,pFrameData,nFrameLen,nFrameRate,nFlagReset);
	}
	catch(...)
	{
	}
	return -1;
}

int SyncPlayerGetWidth(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerGetWidth(hPlayer);
	}
	catch(...)
	{
	}
	return 1280;
}


int SyncPlayerGetHeight(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerGetHeight(hPlayer);
	}
	catch(...)
	{
	}
	return 720;
}

int SyncPlayerResize(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerResize(hPlayer);
	}
	catch(...)
	{
	}
	return -1;
}

int SyncPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 1;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerSkipDisplay(hPlayer,nFlagSkipDisplay);
	}
	catch(...)
	{
	}
	return -1;
}

int SyncPlayerGetFrameRate(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 0;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerGetFrameRate(hPlayer);
	}
	catch(...)
	{
	}
	return 0;
}
int SyncPlayerGetShowType(void*hPlayer)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 0;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerGetShowType(hPlayer);
	}
	catch(...)
	{
	}
	return 0;
}

bool SyncPlayerCaptureToBMPFile(void*hPlayer,const char*cszFile)
{
	try{
		if (g_pVIDECYNCWrapper==NULL)
			return 0;
		return g_pVIDECYNCWrapper->pVIDECYNC_PlayerCaptureToBMPFile(hPlayer,cszFile);
	}
	catch(...)
	{
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////

VIDECYNCD3DWrapper&VIDECYNCD3DWrapper::Instance(void)
{
	return *g_pVIDECYNCD3DWrapper;
}

VIDECYNCD3DWrapper::VIDECYNCD3DWrapper(HINSTANCE hInstance)
{
	char szPathName[1024];
	char szTemp[MAX_PATH];
	GetModuleFileName(hInstance, szTemp, MAX_PATH);
	char* szEnd = strrchr(szTemp, '\\') + 1;
	*szEnd = '\0';
	sprintf(szPathName,"%sVIDECYNC.dll",szTemp);

	NEWRECORD( m_pXDllLoadLibrary,new XDllLoadLibrary(szPathName) )	
//NULLRecord( m_pXDllLoadLibrary )  
	if (m_pXDllLoadLibrary)
	{
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerOpen,"VIDECYNC_D3DPlayerOpen");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerClose,"VIDECYNC_D3DPlayerClose");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerPlayFrame,"VIDECYNC_D3DPlayerPlayFrame");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerRedraw,"VIDECYNC_D3DPlayerRedraw");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerGetWidth,"VIDECYNC_D3DPlayerGetWidth");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerGetHeight,"VIDECYNC_D3DPlayerGetHeight");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerSkipDisplay,"VIDECYNC_D3DPlayerSkipDisplay");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerGetFrameRate,"VIDECYNC_D3DPlayerGetFrameRate");
		m_pXDllLoadLibrary->LoadFunction(pVIDECYNC_D3DPlayerGetShowType,"VIDECYNC_D3DPlayerGetShowType");
	}
}

VIDECYNCD3DWrapper::~VIDECYNCD3DWrapper()
{
	if (m_pXDllLoadLibrary)
	{
		delete m_pXDllLoadLibrary;
		m_pXDllLoadLibrary=NULL;
	}
}

bool VIDECYNCD3DWrapper::IsOK(void)
{
	return (m_pXDllLoadLibrary && m_pXDllLoadLibrary->IsOK());
}


int YNCD3DInit(void)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
		{
			NEWRECORD( g_pVIDECYNCD3DWrapper ,new VIDECYNCD3DWrapper(g_hInstance) )
			NULLRecord( g_pVIDECYNCD3DWrapper )  
			if (!g_pVIDECYNCD3DWrapper->IsOK())
			{
				delete g_pVIDECYNCD3DWrapper;
				g_pVIDECYNCD3DWrapper=NULL;
				return -1;
			}
			return 0;
		}
	}catch(...){

	}

	return -1;
}

void YNCD3DTerminate(void)
{
	try{
		if (g_pVIDECYNCD3DWrapper)
		{
			delete g_pVIDECYNCD3DWrapper;
			g_pVIDECYNCD3DWrapper=NULL;
		}
	}catch(...){

	}
}


void*YNCD3DPlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,HWND hWnd,int nWidth,int nHeight)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return NULL;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerOpen(cbdc,pUserData,hWnd,nWidth,nHeight,false);
	}
	catch(...)
	{
	}
	return NULL;
}

void YNCD3DPlayerClose(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return ;
		g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerClose(hPlayer);
	}
	catch(...)
	{
	}
}

int YNCD3DPlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen,int nFrameRate,int nWidth,int nHeight)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return -1;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerPlayFrame(hPlayer,pFrameData,nFrameLen,nFrameRate,nWidth,nHeight);
	}
	catch(...)
	{
	}
	return -1;
}

void YNCD3DPlayerReDraw(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return ;
		g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerRedraw(hPlayer);
	}
	catch(...)
	{
	}
}

int YNCD3DPlayerGetWidth(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return 1;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerGetWidth(hPlayer);
	}
	catch(...)
	{
	}
	return 1280;
}


int YNCD3DPlayerGetHeight(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return 1;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerGetHeight(hPlayer);
	}
	catch(...)
	{
	}
	return 720;
}


int YNCD3DPlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return 1;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerSkipDisplay(hPlayer,nFlagSkipDisplay);
	}
	catch(...)
	{
	}
	return -1;
}

int YNCD3DPlayerGetFrameRate(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return 0;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerGetFrameRate(hPlayer);
	}
	catch(...)
	{
	}
	return 0;
}

int YNCD3DPlayerGetShowType(void*hPlayer)
{
	try{
		if (g_pVIDECYNCD3DWrapper==NULL)
			return 0;
		return g_pVIDECYNCD3DWrapper->pVIDECYNC_D3DPlayerGetShowType(hPlayer);
	}
	catch(...)
	{
	}
	return 0;
}