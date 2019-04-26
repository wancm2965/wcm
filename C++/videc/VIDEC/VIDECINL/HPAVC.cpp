
#include "HPAVC.h"

//#include <tchar.h>
//#include <wingdi.h>
//
#include <windowsx.h>
//#include <dwmapi.h>
//#include <mmsystem.h>

#pragma warning(disable : 4100)

bool	g_bRegisterClass = false;


static int			g_nAdapterNum = 0;
static mfxIMPL		g_nAdapterIMPL = MFX_IMPL_UNSUPPORTED;
static bool			g_bInitialized=false;
static mfxStatus	g_nInitStatus = MFX_ERR_NONE;

int GetIntelAdapterNum(void)
{
	MFX_Init();
	return g_nAdapterNum;
}

mfxIMPL GetIntelAdapterIMPL(void)
{
	MFX_Init();
	return g_nAdapterIMPL;
}

mfxStatus MFX_Init(void)
{
	if (!g_bInitialized)
	{
		g_nAdapterNum = 0;
		g_nAdapterIMPL = MFX_IMPL_HARDWARE;
		{
			mfxSession auxSession;
			mfxVersion ver = { {0, 1 }};
			g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
			MFXClose(auxSession);
		}
		if (g_nInitStatus != MFX_ERR_NONE)
		{
			g_nAdapterNum = 1;
			g_nAdapterIMPL = MFX_IMPL_HARDWARE2;
			{
				mfxSession auxSession;
				mfxVersion ver = { {0, 1 }};
				g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
				MFXClose(auxSession);
			}
			if (g_nInitStatus != MFX_ERR_NONE)
			{
				g_nAdapterNum = 2;
				g_nAdapterIMPL = MFX_IMPL_HARDWARE3;
				{
					mfxSession auxSession;
					mfxVersion ver = { {0, 1 }};
					g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
					MFXClose(auxSession);
				}				
				if (g_nInitStatus != MFX_ERR_NONE)
				{
					g_nAdapterNum = 3;
					g_nAdapterIMPL = MFX_IMPL_HARDWARE4;
					{
						mfxSession auxSession;
						mfxVersion ver = { {0, 1 }};
						g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
						MFXClose(auxSession);
					}
					//if (g_nInitStatus != MFX_ERR_NONE)
					//{
					//	g_nAdapterNum = 4;
					//	g_nAdapterIMPL = MFX_IMPL_HARDWARE5;
					//	{
					//		mfxSession auxSession;
					//		mfxVersion ver = { {0, 1 }};
					//		g_nInitStatus = MFXInit(g_nAdapterIMPL, &ver, &auxSession);
					//		MFXClose(auxSession);
					//	}
					//}
				}				
			}
		}
		if (g_nInitStatus != MFX_ERR_NONE)
		{
			g_nAdapterNum=0;
			g_nAdapterIMPL=MFX_IMPL_UNSUPPORTED;
		}
		//DebugOutput( logcallstack ,"MFX_Init: g_nAdapterNum = %d g_nAdapterIMPL =%d\n",g_nAdapterNum,g_nAdapterIMPL);
		g_bInitialized = true;
	}
	return g_nInitStatus;
}
