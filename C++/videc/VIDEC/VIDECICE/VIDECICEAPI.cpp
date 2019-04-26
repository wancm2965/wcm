#include "stdafx.h"
#include "VIDECICEAPI.h"
#include "LoadHex.h"
#include "resource.h"
#include "color_enhanc_skin.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

extern HINSTANCE g_hInstance;

static unsigned char*g_pSkinBinData=NULL;

void*VIDECICE_Open(int nWidth,int nHeight,unsigned long nKey)
{
	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return NULL;
	}

	if (g_pSkinBinData==NULL)
	{
		PBYTE pData = NULL;
		DWORD nLen = 0;

		if (LoadResourceData(g_hInstance, IDR_BIN, "BIN", &pData, nLen))
		{
			g_pSkinBinData=pData;
		}
	}

	return image_color_enhanc_open_fast(nWidth,nHeight,g_pSkinBinData);
}


void VIDECICE_Close(void*pHandle)
{
	image_color_enhanc_exit_fast(pHandle);
}

int VIDECICE_Process(void*pHandle,unsigned char*pU,unsigned char*pV)
{
	image_color_enhanc_fast(pHandle,pU,pV);
	return 0;
}

