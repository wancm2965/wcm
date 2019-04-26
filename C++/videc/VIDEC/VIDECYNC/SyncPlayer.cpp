#include <winsock2.h>

#include "pipeline_decode.h"
#include "VIDECYNCAPI.h"
#include <map>
#include "../VIDEC_FRE/VIDECFREAPI.h"




static bool g_bSyncInitialized=false;
static bool g_bSyncHardware=false;
static bool g_bSyncSoftware=false;


typedef std::map<void*,CDecodingPipeline*>	MapPlayer;

static MapPlayer g_MapPlayer;



VIDECYNC_API int VIDECYNC_Init(unsigned long nKey)
{
	if (g_bSyncInitialized)
	{
		return 0;
	}

	g_bSyncInitialized=true;

	unsigned long nTimestamp=timeGetTime();
	if (nTimestamp-nKey>1000)
	{
		return -1;
	}

	{		
		mfxStatus nStatus = MFX_Init();
		g_bSyncHardware = (nStatus==MFX_ERR_NONE);
	}

	return ((g_bSyncHardware || g_bSyncSoftware)?0:-1);
}


VIDECYNC_API void VIDECYNC_PlayerSetMaxCount(unsigned int nMaxPlayer)
{
	//g_nMaxPlayer=nMaxPlayer;
}

VIDECYNC_API int VIDECYNC_PlayerTry(void*hPlayer,unsigned long nWidth,unsigned long nHeight)
{
	if (!g_bSyncInitialized || (!g_bSyncHardware && !g_bSyncSoftware) || FRE_GetMaxDecoderCount()==0)
	{
		return -1;
	}

	if (nWidth>=FRE_GetHWDecodeWidth() && nHeight>=FRE_GetHWDecodeHeight())
	{

	}
	else
	{
		return -1;
	}

	if (g_MapPlayer.size()>=FRE_GetMaxDecoderCount())
	{
		return -1;
	}


	return 0;
}


VIDECYNC_API void*VIDECYNC_PlayerOpen(YNCD3D_CB_DC cbdc,void*pUserData,void* hWnd, int nWidth, int nHeight)
{
	if (!g_bSyncInitialized || (!g_bSyncHardware && !g_bSyncSoftware))
	{
		return NULL;
	}

	bool bHardware=g_bSyncHardware;
	if (g_bSyncHardware)
	{
		if (0!=VIDECYNC_PlayerTry(NULL,nWidth,nHeight))
		{
			if (!g_bSyncSoftware)
			{
				return NULL;
			}
		}
	}
	else
	{
		bHardware=false;
	}


	NEWRECORD3( CDecodingPipeline*,pPlayer ,new CDecodingPipeline(bHardware)  )
	if (pPlayer)
	{
		if (0!=pPlayer->Open(cbdc,pUserData,(HWND)hWnd,nWidth,nHeight))
		{
			pPlayer->Close();
			delete pPlayer;
			pPlayer=NULL;
		}
		else if (bHardware)
		{
			g_MapPlayer[pPlayer]=pPlayer;

			if (FRE_GetDebugLevel() > 0)
			{
				DebugOutput( logcallstack ,"VIDECYNC_PlayerOpen nWidth=%u nHeight=%u PlayerCount=%u MaxPlayer=%u IntelAdapterNum=%u\n",nWidth, nHeight,g_MapPlayer.size(),FRE_GetMaxDecoderCount(),GetIntelAdapterNum());	
			}		
		}
	}
	return pPlayer;
}

VIDECYNC_API void VIDECYNC_PlayerClose(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		g_MapPlayer.erase(pPlayer);

		pPlayer->Close();
		delete pPlayer;
		pPlayer=NULL;
	}
}

VIDECYNC_API int VIDECYNC_PlayerPlayFrame(void*hPlayer, const unsigned char*pFrameData, int nFrameLen,int nFrameRate,int&nFlagReset)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->PlayFrame(pFrameData,nFrameLen,nFrameRate,nFlagReset);
	}
	return -1;
}

VIDECYNC_API int VIDECYNC_PlayerGetWidth(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetActualWidth();
	}
	return 1280;
}

VIDECYNC_API int VIDECYNC_PlayerGetHeight(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetActualHeight();
	}
	return 720;
}

VIDECYNC_API int VIDECYNC_PlayerResize(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->Resize();
	}
	return -1;
}

VIDECYNC_API int VIDECYNC_PlayerSkipDisplay(void*hPlayer,int nFlagSkipDisplay)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->SkipDisplay(nFlagSkipDisplay);
	}
	return -1;
}

VIDECYNC_API int VIDECYNC_PlayerGetFrameRate(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetFrameRate();
	}
	return 0;
}

VIDECYNC_API int VIDECYNC_PlayerGetShowType(void*hPlayer)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->GetShowType();
	}
	return 0;
}

VIDECYNC_API bool VIDECYNC_PlayerCaptureToBMPFile(void*hPlayer,const char*cszFile)
{
	CDecodingPipeline*pPlayer=(CDecodingPipeline*)hPlayer;
	if (pPlayer)
	{
		return pPlayer->CaptureToBMPFile(cszFile);
	}
	return false;
}