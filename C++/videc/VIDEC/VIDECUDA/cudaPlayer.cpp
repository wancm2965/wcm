#include "cudaPlayerD3D9.h"
#include "cudaPlayer.h"
#include "VIDECUDAAPI.h"
#include "resource.h"
#include "LoadHex.h"
#include <map>

static std::string g_strPTXSource="";

extern bool cutilDrvCudaDevCapabilities(int major_version, int minor_version, int deviceNum);



static bool g_bCUDAInitialized=false;
static bool g_bCUDAHardware=false;

typedef std::map<void*,cudaPlayerD3D9*>	MapCudaPlayer;

static MapCudaPlayer g_MapCudaPlayer;
static unsigned int g_nMaxPlayer=0;


std::string&GetPTXSource(void)
{
	return g_strPTXSource;
}



VIDECUDA_API int VIDECUDA_Init(unsigned long nKey)
{
	if (g_bCUDAInitialized)
	{
		return 0;
	}

	g_bCUDAInitialized=true;

	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return -1;
	}

	CUresult cuStatus = CUDA_SUCCESS;
	cuStatus = cuInit(0);
	if (cuStatus != CUDA_SUCCESS)
	{
		return -1;
	}

	int nDev=0;
	int nDeviceCount = 0;

	cuDeviceGetCount(&nDeviceCount);
	if (nDeviceCount<=0)
	{
		return -1;
	}

	int nTCC=0;
	for (int i=0; i < nDeviceCount; i++)
	{
		int bSupported = cutilDrvCudaDevCapabilities(1, 1, i);
		if (!bSupported) 
		{
			return -1;
		}

		cuDeviceGet(&nDev, i);

		cuDeviceGetAttribute( &nTCC ,  CU_DEVICE_ATTRIBUTE_TCC_DRIVER, nDev );

		if (nTCC) 
		{
			continue;
		}
		else 
		{
			break;
		}
	}

	if (nTCC)
	{
		return -1;
	}

	PBYTE pData = NULL;
	DWORD nDataLen = 0;

	if (LoadHex(g_hInstance, IDR_HEX, &pData, nDataLen))
	{
		char*szSource=(char*)malloc(nDataLen+1);
		memcpy(szSource,pData,nDataLen);
		szSource[nDataLen]='\0';
		g_strPTXSource=(char*)szSource;
		free(szSource);
	}
	else
	{
		return -1;
	}

	g_bCUDAHardware=true;

	return 0;
}


VIDECUDA_API void VIDECUDA_PlayerSetMaxCount(unsigned int nMaxPlayer)
{
	g_nMaxPlayer=nMaxPlayer;
}

VIDECUDA_API int VIDECUDA_PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight)
{
	if (!g_bCUDAHardware || !g_bCUDAInitialized || g_nMaxPlayer==0)
	{
		return -1;
	}

	if ((nWidth==1920 && nHeight==1080) ||
		(nWidth==1920 && nHeight==1088) ||
		(nWidth==1920 && nHeight==540) ||
		(nWidth==1920 && nHeight==544) ||
		(nWidth==1280 && nHeight==540) ||
		(nWidth==1280 && nHeight==544) ||
		(nWidth==960 && nHeight==540) ||
		(nWidth==960 && nHeight==544) ||
		(nWidth==640 && nHeight==480) ||
		(nWidth==1280 && nHeight==720))
	{

	}
	else
	{
		return -1;
	}


	unsigned long nCurrent=0;
	MapCudaPlayer::iterator iter;
	for (iter=g_MapCudaPlayer.begin();iter!=g_MapCudaPlayer.end();++iter)
	{
		cudaPlayerD3D9*pPlayer=iter->second;
		if (pPlayer!=(cudaPlayerD3D9*)hPlayer)
		{
			unsigned long nTempWidth=pPlayer->GetActualWidth();
			unsigned long nTempHeight=pPlayer->GetActualHeight();

			nCurrent+=nTempWidth*nTempHeight;
		}
	}


	nCurrent+=nWidth*nHeight;

	if (nCurrent>g_nMaxPlayer*1920*1088)
	{
		return -1;
	}

	return 0;
}


VIDECUDA_API void*VIDECUDA_PlayerOpen(HWND hWnd, int nWidth, int nHeight)
{
	switch (nHeight)
	{
	case 1080:
		nHeight=1088;
		break;
	case 540:
		nHeight=544;
		break;
	}

	//char szDebug[1024];
	//sprintf(szDebug,"nWidth=%d nHeight=%d\n",nWidth,nHeight);
	//OutputDebugString(szDebug);

	if (0!=VIDECUDA_PlayerTry(NULL,nWidth,nHeight))
	{
		return NULL;
	}

	cudaPlayerD3D9*pPlayer=new cudaPlayerD3D9();
	if (pPlayer)
	{
		if (0!=pPlayer->Open(hWnd,nWidth,nHeight))
		{
			pPlayer->Close();
			delete pPlayer;
			pPlayer=NULL;
		}
		else
		{
			g_MapCudaPlayer[pPlayer]=pPlayer;
		}
	}
	return pPlayer;
}

VIDECUDA_API void VIDECUDA_PlayerClose(void*hPlayer)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		g_MapCudaPlayer.erase(pPlayer);

		pPlayer->Close();
		delete pPlayer;
		pPlayer=NULL;
	}
}

VIDECUDA_API int VIDECUDA_PlayerPlayFrame(void*hPlayer, const unsigned char *pFrameData, int nFrameLen, int nFrameRate, int&nFlagReset)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		//unsigned long nStart=timeGetTime();
		int nRet=pPlayer->PlayFrame(pFrameData,nFrameLen,nFrameRate,nFlagReset);
		//int nEnd=timeGetTime();
		//unsigned long nDelta=nEnd-nStart;
		//char szDebug[1024];
		//sprintf(szDebug,"nStart=%u nDelta=%u\n",nStart,nDelta);
		//OutputDebugString(szDebug);

		return nRet;
	}
	return -1;
}

VIDECUDA_API int VIDECUDA_PlayerGetWidth(void*hPlayer)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetActualWidth();
	}
	return 1280;
}

VIDECUDA_API int VIDECUDA_PlayerGetHeight(void*hPlayer)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetActualHeight();
	}
	return 720;
}

VIDECUDA_API int VIDECUDA_PlayerResize(void*hPlayer)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->Resize();
	}
	return -1;
}


VIDECUDA_API int VIDECUDA_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	cudaPlayerD3D9*pPlayer=(cudaPlayerD3D9*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->SkipDisplay(nFlagSkipDisplay);
	}
	return -1;
}

