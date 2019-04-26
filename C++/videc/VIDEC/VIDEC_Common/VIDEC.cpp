//VIDEC.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ATLBASE.H>
#include <wchar.h>
#include <winsock2.h>
#include <windows.h>
#include "VIDEC.h"
#include <AVCONProduct.h>
#include <AVCONAVC/AVCONAVC.h>

#include "VIDEC_Codec.h"
#include "libxpostproc.h"
#include "../VIDEC_API/DVCDevMgr.h"
#include "../VIDEC_API/AH2KDevMgr.h"
#include "../VIDEC_API/AH400DevMgr.h"
#include "VIDEC_AH2KDev.h"
#include "VIDEC_CameraDev.h"
#include "VIDEC_AH400Dev.h"
#include "VIDEC_AH400.h"
#include "VIDEC_AH2K.h"

#include "../VIDEC_ICE/ICEWrapper.h"
#include "../VIDEC_Codecs/IntelCodecWrapper.h"
#include "../VIDEC_MediaFile/AVMSWrapper.h"
#include "../VIDEC_Wnd/H264Player.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"
#include "../VIDEC_Wnd/VideoDisplay.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>


# pragma comment(lib, "wbemuuid.lib")

extern void VIDEC_CodecInit(void);
extern void VIDEC_CodecTerminate(void);

extern HINSTANCE g_hInstance;


static DWORD g_nUsedByProductType=0;
static bool  g_bHarewareDecoder=false;
static bool	 g_bOnlyUSBCamera=false;

DWORD GetUsedByProductType(void)
{
	return g_nUsedByProductType;
}

VIDEC_API int GetHarewareDecoder(void)
{
	return (g_bHarewareDecoder?1:0);
}

bool GetOnlyUSBCamera(void)
{
	return g_bOnlyUSBCamera;
}

static unsigned long g_ulMaxKeyFrameInterval=300000; //??
VIDEC_API void VIDEC_SetMaxKeyFrameInterval(unsigned long ulMS)
{
	if (ulMS<1000)
		ulMS=1000;
	else if (ulMS>300000)
	{
		ulMS=300000;
	}
	g_ulMaxKeyFrameInterval=ulMS;
}

VIDEC_API unsigned long VIDEC_GetMaxKeyFrameInterval(void)
{
	//first find fre 
	if(FRE_GetMaxKeyFrameInterval( ) != g_ulMaxKeyFrameInterval )
		g_ulMaxKeyFrameInterval = FRE_GetMaxKeyFrameInterval();

	return g_ulMaxKeyFrameInterval;
}


static long g_nVIDEC_RefCount=0;

//int g_nMemorySize = 1024 *1024 * 300;
//int g_nThreads = 1;

VIDEC_API int VIDEC_Init(unsigned long ulFlag,int nUsedByProductType)
{
	FREIPC_Init();
	//CreateVPool(g_nMemorySize,g_nThreads);

	if (g_nVIDEC_RefCount<=0)
	{
		g_nUsedByProductType=nUsedByProductType;

#ifdef _DEMO
		SYSTEMTIME ExpiredTime;
		memset(&ExpiredTime,0,sizeof(ExpiredTime));
		ExpiredTime.wYear=2009;
		ExpiredTime.wMonth=1;
		ExpiredTime.wDay=1;

		char szDebug[1024];
		sprintf(szDebug,"This version will be expired at %u:%u:%u\r\n",ExpiredTime.wYear,ExpiredTime.wMonth,ExpiredTime.wDay);
		DebugOutput( logcallstack ,szDebug);

		SYSTEMTIME CurTime;
		GetSystemTime(&CurTime);
		if (CurTime.wYear>ExpiredTime.wYear)
		{
			return -1;
		}
		if (CurTime.wYear==ExpiredTime.wYear &&
			CurTime.wMonth>ExpiredTime.wMonth)
		{
			return -1;
		}
		if (CurTime.wYear==ExpiredTime.wYear &&
			CurTime.wMonth>ExpiredTime.wMonth &&
			CurTime.wDay>ExpiredTime.wDay)
		{
			return -1;
		}
#endif

		VIDEC_CodecInit();
		VIDEC_PostProcInit();


#ifdef SUPPORT_DVC
		DVCDevMgr::Init();
#endif

		//ICE_init();

		AVMSP_init();

#ifdef SUPPORT_AH2K
		if ((ulFlag&VIDEC_INIT_FLAG_AH2K) && ((ulFlag&VIDEC_INIT_FALG_ONLY_USB_CAMERA)==0 || nUsedByProductType!=AVCON_PRODUCT_NFH))
		{
			AH2KDevMgr::Init();
		}
#endif

#ifdef SUPPORT_AH400
		if ((ulFlag&VIDEC_INIT_FLAG_AH400) && ((ulFlag&VIDEC_INIT_FALG_ONLY_USB_CAMERA)==0 || nUsedByProductType!=AVCON_PRODUCT_NFH))
		{
			AH400DevMgr::Init();
		}
#endif

		g_bOnlyUSBCamera=((ulFlag&VIDEC_INIT_FALG_ONLY_USB_CAMERA)!=0);
		g_bHarewareDecoder=(ulFlag&VIDEC_INIT_FLAG_HARDWARE_DECODER)!=0;



		AVCONAVC_Initialize();

		if (VIDEC_AH2KDev::GetDevCount())
		{
			g_nUsedByProductType=AVCON_PRODUCT_NORMAL;
			//g_bOnlyUSBCamera=false;
			g_bHarewareDecoder=false;
		}
	}

	g_nVIDEC_RefCount++;

	FRE_Init();

	//创建一个窗口，占用Overlay显示。
	CreateOverLayWnd();

	return 0;
}

VIDEC_API void VIDEC_Terminate(void)
{
	g_nVIDEC_RefCount--;

	DestoryOverLayWnd();	

	//ICE_terminate();
	FRE_Terminate();

	AVMSP_terminate();

#ifdef SUPPORT_DVC
	DVCDevMgr::Terminate();
#endif

#ifdef SUPPORT_AH2K
	AH2KDevMgr::Terminate();
#endif

#ifdef SUPPORT_AH400
	AH400DevMgr::Terminate();
#endif

	VIDEC_CodecTerminate();
	VIDEC_PostProcTerminate();

	AVCONAVC_UnInitialize();

	//DestroyVPool();
	FREIPC_Terminate();

	//char szDebug[1024];
	//sprintf(szDebug,"VIDEC_Terminate g_nVIDEC_RefCount=%d\n",g_nVIDEC_RefCount);
	//DebugOutput(szDebug);
}


VIDEC_API int GetBitrateControlTypeByNetworkType(int nNetworkType)
{
	return nNetworkType;
}

VIDEC_API int GetQualityCount(void)
{
	return 16;
}

VIDEC_API int GetNameByQuality(int nQuality,char*szName,int nMaxCount)
{
	char szTemp[128]="";

	switch (nQuality)
	{
	case 0:
		strcpy(szTemp,"32 Kbps");
		break;
	case 1:
		strcpy(szTemp,"64 Kbps");
		break;
	case 2:
		strcpy(szTemp,"128 Kbps");
		break;
	case 3:
		strcpy(szTemp,"256 Kbps");
		break;
	case 4:
		strcpy(szTemp,"384 Kbps");
		break;
	case 5:
		strcpy(szTemp,"512 Kbps");
		break;
	case 6:
		strcpy(szTemp,"768 Kbps");
		break;
	case 7:
		strcpy(szTemp,"1 Mbps");
		break;
	case 8:
		strcpy(szTemp,"1.5 Mbps");
		break;
	case 9:
		strcpy(szTemp,"2 Mbps");
		break;
	case 10:
		strcpy(szTemp,"3 Mbps");
		break;
	case 11:
		strcpy(szTemp,"4 Mbps");
		break;
	case 12:
		strcpy(szTemp,"5 Mbps");
		break;
	case 13:
		strcpy(szTemp,"6 Mbps");
		break;
	case 14:
		strcpy(szTemp,"8 Mbps");
		break;
	case 15:
		strcpy(szTemp,"12 Mbps");
		break;
	default:
		return -1;
	}

	if (nMaxCount<=0)
		return -1;

	if (szName==NULL)
	{
		return strlen(szTemp)+1;
	}

	if (nMaxCount>=strlen(szTemp)+1)
	{
		strcpy(szName,szTemp);
	}
	else
	{
		strncpy(szName,szTemp,nMaxCount);
	}

	return strlen(szName)+1;
}


VIDEC_API int GetFrameRateCount(void)
{
	return 6;
}

VIDEC_API int GetFrameRateByIndex(int nIndex)
{
	int nFrameRate=15;
	switch (nIndex)
	{
	case 0:
		nFrameRate=1;
		break;
	case 1:
		nFrameRate=5;
		break;
	case 2:
		nFrameRate=10;
		break;
	case 3:
		nFrameRate=15;
		break;
	case 4:
		nFrameRate=30;
		break;
	case 5:
		nFrameRate=60;
		break;
	}
	return nFrameRate;
}

VIDEC_API int GetFrameRateDefaultIndex(void)
{
	return 3;
}

#define H264_MAX_QP_GOOD			49
#define H264_MAX_QP_BETTER			40
#define H264_MAX_QP_BEST			40

#define H263_MAX_QP_GOOD			31
#define H263_MAX_QP_BETTER			22
#define H263_MAX_QP_BEST			16

VIDEC_API int GetCodecInfo(int&nQuality,int&nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
{
	int nBitrateControlType=VIDEC_NETWORK_PRIVATE;
	return GetCodecInfo(nQuality,nBitrateControlType,nBitrate,nMinQP,nMaxQP,CodecType,bRTP);
}

VIDEC_API int GetCodecInfo(int&nQuality,int&nBitrateControlType,int&nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
{
	if (nQuality>GetQualityCount())
	{
		if (nQuality<=32)
		{
			nQuality=0;
		}
		else if (nQuality<=64)
		{
			nQuality=1;
		}
		else if (nQuality<=128)
		{
			nQuality=2;
		}
		else if (nQuality<=256)
		{
			nQuality=3;
		}
		else if (nQuality<=384)
		{
			nQuality=4;
		}
		else if (nQuality<=512)
		{
			nQuality=5;
		}
		else if (nQuality<=768)
		{
			nQuality=6;
		}
		else if (nQuality<=1024)
		{
			nQuality=7;
		}
		else if (nQuality<=1536)
		{
			nQuality=8;
		}
		else if (nQuality<=2048)
		{
			nQuality=9;
		}
		else if (nQuality<=3072)
		{
			nQuality=10;
		}
		else if (nQuality<=4096)
		{
			nQuality=11;
		}
		else if (nQuality<=5120)
		{
			nQuality=12;
		}
		else if (nQuality<=6144)
		{
			nQuality=13;
		}
		else if (nQuality<=8192)
		{
			nQuality=14;
		}
		else
		{
			nQuality=15;
		}
	}

	if (bRTP)
	{
		if (CodecType!=VIDEC_CODEC_H264 && 
			CodecType!=VIDEC_CODEC_H263P &&
			CodecType!=VIDEC_CODEC_H263 
#ifdef ENABLE_VIDEC_CODEC_H261
			&& CodecType!=VIDEC_CODEC_H261
#endif
			)
		{
			if (CodecType==VIDEC_CODEC_H264_SVC)
			{
				CodecType=(VIDEC_CODEC_H264);
			}
			else
			{
				CodecType=(VIDEC_CODEC_H263);
			}
		}
	}
	else
	{
		if (
#ifdef ENABLE_VIDEC_CODEC_LSCC
			CodecType!=VIDEC_CODEC_LSCC && 
#endif
			CodecType!=VIDEC_CODEC_H264 && 
			CodecType!=VIDEC_CODEC_MPEG4 && 
			CodecType!=VIDEC_CODEC_H264_SVC && 
			CodecType!=VIDEC_CODEC_H263P &&
			CodecType!=VIDEC_CODEC_H263 &&
			CodecType!=VIDEC_CODEC_AH400 &&
			CodecType!=VIDEC_CODEC_HIKC &&
			CodecType!=VIDEC_CODEC_IMKH &&
			CodecType!=VIDEC_CODEC_DS81 &&
			CodecType!=VIDEC_CODEC_DS71 &&
			CodecType!=VIDEC_CODEC_X62MF &&
			CodecType!=VIDEC_CODEC_MEGA &&
			CodecType!=VIDEC_CODEC_HIKH)
		{
			CodecType=(VIDEC_CODEC_H264);
		}
	}
 
	if (nQuality<0 || nQuality>=GetQualityCount())
		nQuality=2;


	int nH264MaxQP=H264_MAX_QP_GOOD;
	int nH263MaxQP=H263_MAX_QP_GOOD;
	unsigned long ulBitrateControlType=nBitrateControlType;
	if ((VIDEC_NETWORK_QUALITY&ulBitrateControlType)==VIDEC_NETWORK_QUALITY)
	{
		if (nQuality>=6)
		{
			nH264MaxQP=H264_MAX_QP_BEST;
			nH263MaxQP=H263_MAX_QP_BEST;
		}
		else
		{
			nH264MaxQP=H264_MAX_QP_BETTER;
			nH263MaxQP=H263_MAX_QP_BETTER;
		}
	}
	else
	{
		if (GetUsedByProductType()==AVCON_PRODUCT_RS)
		{
			if (nQuality>=6)
			{
				nH264MaxQP=H264_MAX_QP_BEST;
				nH263MaxQP=H263_MAX_QP_BEST;
			}
			else
			{
				nH264MaxQP=H264_MAX_QP_BETTER;
				nH263MaxQP=H263_MAX_QP_BETTER;
			}
		}
		else
		{
			if (nQuality>=6)
			{
				nH264MaxQP=H264_MAX_QP_BETTER;
				nH263MaxQP=H263_MAX_QP_BETTER;
			}
			else
			{
				nH264MaxQP=H264_MAX_QP_GOOD;
				nH263MaxQP=H263_MAX_QP_GOOD;
			}
		}
	}
	ulBitrateControlType=ulBitrateControlType & (~VIDEC_NETWORK_QUALITY);
	nBitrateControlType=ulBitrateControlType;

	


	switch (nQuality)
	{
	case 0://32
		nBitrate=32;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=36;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=24;
			nMaxQP=28;
		}
		else
		{
			nMinQP=16;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 1://64
		nBitrate=64;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=34;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=22;
			nMaxQP=26;
		}
		else
		{
			nMinQP=14;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 2://128
		nBitrate=128;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=33;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=21;
			nMaxQP=26;
		}
		else
		{
			nMinQP=13;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 3://256
		nBitrate=256;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=32;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=20;
			nMaxQP=26;
		}
		else
		{
			nMinQP=12;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 4://384
		nBitrate=384;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=31;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=19;
			nMaxQP=26;
		}
		else
		{
			nMinQP=11;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 5://512
		nBitrate=512;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=30;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=18;
			nMaxQP=26;
		}
		else
		{
			nMinQP=10;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 6://768
		nBitrate=768;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=29;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=17;
			nMaxQP=26;
		}
		else
		{
			nMinQP=9;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 7://1024
		nBitrate=1024;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=28;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=16;
			nMaxQP=26;
		}
		else
		{
			nMinQP=8;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 8://1536
		nBitrate=1536;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=27;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=15;
			nMaxQP=26;
		}
		else
		{
			nMinQP=7;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 9://2048
		nBitrate=2048;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=26;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=14;
			nMaxQP=26;
		}
		else
		{
			nMinQP=6;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 10://3072
		nBitrate=3072;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=25;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=13;
			nMaxQP=26;
		}
		else
		{
			nMinQP=5;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 11://4096
		nBitrate=4096;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=24;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=13;
			nMaxQP=26;
		}
		else
		{
			nMinQP=4;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 12://5120
		nBitrate=5120;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=22;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=13;
			nMaxQP=26;
		}
		else
		{
			nMinQP=3;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 13://6144
		nBitrate=6144;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=18;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=12;
			nMaxQP=26;
		}
		else
		{
			nMinQP=2;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 14://8192
		nBitrate=8192;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=16;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=10;
			nMaxQP=26;
		}
		else
		{
			nMinQP=2;
			nMaxQP=nH263MaxQP;
		}
		break;
	case 15://12288
		nBitrate=12288;
		if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
		{
			nMinQP=14;
			nMaxQP=nH264MaxQP;
		}
		else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
		{
			nMinQP=8;
			nMaxQP=26;
		}
		else
		{
			nMinQP=2;
			nMaxQP=nH263MaxQP;
		}
		break;
	};

	return 0;
}

VIDEC_API int GetCodecInfoWithBitrate(int&nBitrateControlType,int nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
{
	if (bRTP)
	{
		if (CodecType!=VIDEC_CODEC_H264 && 
			CodecType!=VIDEC_CODEC_H263P &&
			CodecType!=VIDEC_CODEC_H263PP &&
			CodecType!=VIDEC_CODEC_H263 
#ifdef ENABLE_VIDEC_CODEC_H261
			&& CodecType!=VIDEC_CODEC_H261
#endif
			)
		{
			if (CodecType==VIDEC_CODEC_H264_SVC)
			{
				CodecType=VIDEC_CODEC_H264;
			}
			else
			{
				CodecType=(VIDEC_CODEC_H263);
			}
		}
	}
	else
	{
		if (
#ifdef ENABLE_VIDEC_CODEC_LSCC
			CodecType!=VIDEC_CODEC_LSCC && 
#endif
			CodecType!=VIDEC_CODEC_H264 && 
			CodecType!=VIDEC_CODEC_H264_SVC && 
			CodecType!=VIDEC_CODEC_H263P &&
			CodecType!=VIDEC_CODEC_H263 &&
			CodecType!=VIDEC_CODEC_AH400 &&
			CodecType!=VIDEC_CODEC_HIKC &&
			CodecType!=VIDEC_CODEC_IMKH &&
			CodecType!=VIDEC_CODEC_DS81 &&
			CodecType!=VIDEC_CODEC_DS71 &&
			CodecType!=VIDEC_CODEC_X62MF &&
			CodecType!=VIDEC_CODEC_MEGA &&
			CodecType!=VIDEC_CODEC_HIKH)
		{
			CodecType=(VIDEC_CODEC_H264);
		}
	}

	int nH264MaxQP=H264_MAX_QP_GOOD;
	int nH263MaxQP=H263_MAX_QP_GOOD;
	unsigned long ulBitrateControlType=nBitrateControlType;
	if ((VIDEC_NETWORK_QUALITY&ulBitrateControlType)==VIDEC_NETWORK_QUALITY)
	{
		if (nBitrate>=768)
		{
			nH264MaxQP=H264_MAX_QP_BEST;
			nH263MaxQP=H263_MAX_QP_BEST;
		}
		else
		{
			nH264MaxQP=H264_MAX_QP_BETTER;
			nH263MaxQP=H263_MAX_QP_BETTER;
		}
	}
	else
	{
		if (GetUsedByProductType()==AVCON_PRODUCT_RS)
		{
			if (nBitrate>=768)
			{
				nH264MaxQP=H264_MAX_QP_BEST;
				nH263MaxQP=H263_MAX_QP_BEST;
			}
			else
			{
				nH264MaxQP=H264_MAX_QP_BETTER;
				nH263MaxQP=H263_MAX_QP_BETTER;
			}
		}
		else
		{
			if (nBitrate>=768)
			{
				nH264MaxQP=H264_MAX_QP_BETTER;
				nH263MaxQP=H263_MAX_QP_BETTER;
			}
			else
			{
				nH264MaxQP=H264_MAX_QP_GOOD;
				nH263MaxQP=H263_MAX_QP_GOOD;
			}
		}
	}
	ulBitrateControlType=ulBitrateControlType & (~VIDEC_NETWORK_QUALITY);
	nBitrateControlType=ulBitrateControlType;

	if (CodecType==VIDEC_CODEC_H264 || CodecType==VIDEC_CODEC_H264_SVC )
	{
		nMinQP=14;
		nMaxQP=nH264MaxQP;
	}
	else if (CodecType==VIDEC_CODEC_AH400 || CodecType==VIDEC_CODEC_HIKH || CodecType==VIDEC_CODEC_HIKC || CodecType==VIDEC_CODEC_IMKH || CodecType==VIDEC_CODEC_DS81 || CodecType==VIDEC_CODEC_DS71 || CodecType==VIDEC_CODEC_X62MF || CodecType==VIDEC_CODEC_MEGA)
	{
		nMinQP=8;
		nMaxQP=26;
	}
	else
	{
		nMinQP=2;
		nMaxQP=nH263MaxQP;
	}

	return 0;
}


int GetH264DefaultQP(int nBitrate)
{
	int nDefaultQP=28;
	nBitrate/=1000;
	if(nBitrate<=32)
		nDefaultQP=36;
	else if (nBitrate<=64)
		nDefaultQP=34;
	else if (nBitrate<=128)
		nDefaultQP=33;
	else if (nBitrate<=256)
		nDefaultQP=32;
	else if (nBitrate<=384)
		nDefaultQP=31;
	else if (nBitrate<=512)
		nDefaultQP=30;
	else if (nBitrate<=768)
		nDefaultQP=29;
	else if (nBitrate<=1024)
		nDefaultQP=28;
	else if (nBitrate<=1536)
		nDefaultQP=27;
	else if (nBitrate<=2048)
		nDefaultQP=26;
	else if (nBitrate<=3072)
		nDefaultQP=25;
	else if (nBitrate<=4096)
		nDefaultQP=24;
	else if (nBitrate<=5120)
		nDefaultQP=22;
	else if (nBitrate<=6144)
		nDefaultQP=18;
	else if (nBitrate<=8192)
		nDefaultQP=16;
	else if (nBitrate<=12288)
		nDefaultQP=14;

	return nDefaultQP;
}

VIDEC_API int VIDEC_GetBrightessRange(int&nMin,int&nMax)
{
	nMin=0;
	nMax=255;
	return 0;
}

VIDEC_API int VIDEC_GetDefaultBrightness(void)
{
	return 128;
}

VIDEC_API int VIDEC_GetContrastRange(int&nMin,int&nMax)
{
	nMin=0;
	nMax=127;
	return 0;
}

VIDEC_API int VIDEC_GetDefaultContrast(void)
{
	return 64;
}

VIDEC_API int VIDEC_GetSaturationRange(int&nMin,int&nMax)
{
	nMin=0;
	nMax=127;
	return 0;
}

VIDEC_API int VIDEC_GetDefaultSaturation(void)
{
	return 64;
}

VIDEC_API int VIDEC_GetHueRange(int&nMin,int&nMax)
{
	nMin=0;
	nMax=255;
	return 0;
}

VIDEC_API int VIDEC_GetDefaultHue(void)
{
	return 0;
}

VIDEC_API int VIDEC_GetGammaRange(int&nMin,int&nMax)
{
	nMin=1;
	nMax=400;
	return 0;
}

VIDEC_API int VIDEC_GetDefaultGamma(void)
{
	return 100;
}

//取锐化范围
VIDEC_API int VIDEC_GetSharpenRange(int&nMin,int&nMax)
{
	nMin=0;
	nMax=127;
	return 0;
}
//取锐化的缺省值
VIDEC_API int VIDEC_GetDefaultSharpen(void)
{
	return 0;
}


VIDEC_API int VIDEC_GetDevCount(void)
{
	int nDevCnt=0;

	int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
	nDevCnt+=nTempDevCnt;

	nTempDevCnt=VIDEC_CameraDev::GetDevCount();
	nDevCnt+=nTempDevCnt;

	nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
	nDevCnt+=nTempDevCnt;

	return nDevCnt;
}

VIDEC_API int VIDEC_GetDevName(int nDevID,char*szName,int nMaxCount)
{
	int nRet=-1;
	while (true)
	{
		int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
		if (nDevID < nTempDevCnt)
		{
			if (nDevID>=0)
			{
				nRet=VIDEC_AH2KDev::GetDevName(nDevID,szName,nMaxCount);
				break;
			}
		}

		nDevID-=nTempDevCnt;
		nTempDevCnt=VIDEC_CameraDev::GetDevCount();
		if (nDevID < nTempDevCnt)
		{
			if (nDevID>=0)
			{
				nRet=VIDEC_CameraDev::GetDevName(nDevID,szName,127);
				break;
			}
		}

		nDevID-=nTempDevCnt;
		nTempDevCnt=VIDEC_AH400Dev::GetDevCount();
		if (nDevID < nTempDevCnt)
		{
			if (nDevID>=0)
			{
				nRet=VIDEC_AH400Dev::GetDevName(nDevID,szName,127);
				break;
			}
		}
		break;
	}
	return nDevID;
}

VIDEC_API int VIDEC_GetDevType(int nDevID,VIDEC_DEV_TYPE&nDevType)
{
	if (nDevID<0)
		return -1;

	int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
	int nTempDevID=nDevID-nTempDevCnt;
	if (nTempDevID<0)
	{
		return VIDEC_AH2KDev::GetDevType(nDevID,nDevType);
	}

	nDevType=VIDEC_DEV_SD;
	return 0;
}

VIDEC_API void* VIDEC_GetDevHandle(int nDevID)
{
#ifdef SUPPORT_AH400
	if (nDevID<0)
		return (void*)0xFFFFFFFF;

	int nTempDevCnt=VIDEC_AH2KDev::GetDevCount();
	nDevID=nDevID-nTempDevCnt;
	if (nDevID<0)
	{
		return (void*)0xFFFFFFFF;
	}

	nTempDevCnt=VIDEC_CameraDev::GetDevCount();
	nDevID=nDevID-nTempDevCnt;
	if (nDevID<0)
	{
		return (void*)0xFFFFFFFF;
	}

	return VIDEC_AH400::GetDevHandle(nDevID);
#else
	return (void*)0xFFFFFFFF;
#endif
}



VIDEC_API void GetBitrateRange(int nWidth,int nHeight,int&nQualityMin,int&nQualityMax)
{
	nQualityMin=0;
	nQualityMax=GetQualityCount()-1;
	if (nHeight>=768)
	{
		nQualityMin=3;
		nQualityMax=9;
	}
	else if (nHeight>=720)
	{
		nQualityMin=4;
		nQualityMax=9;
	}
	else if (nHeight>=600)
	{
		nQualityMin=3;
		nQualityMax=8;
	}
	else if (nHeight>=576)
	{
		nQualityMin=2;
		nQualityMax=7;
	}
	else if (nHeight>=540)
	{
		nQualityMin=3;
		nQualityMax=8;
	}
	else if (nHeight>=480)
	{
		nQualityMin=2;
		nQualityMax=6;
	}
	else if (nHeight>=288)
	{
		nQualityMin=1;
		nQualityMax=6;
	}
	else if (nHeight>=240)
	{
		nQualityMin=1;
		nQualityMax=5;
	}
	else if (nHeight>=144)
	{
		nQualityMin=0;
		nQualityMax=3;
	}
}

BOOL IsVistaOrAbove()
{
	//only check once then cache the result
	static BOOL checked = FALSE;
	static BOOL result  = FALSE;
	OSVERSIONINFO osver;

	if (!checked)
	{
		checked = TRUE;

		osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		if (GetVersionEx( &osver ) &&
			osver.dwPlatformId == VER_PLATFORM_WIN32_NT &&
			(osver.dwMajorVersion >= 6 ) )
			result = TRUE;
	}

	return result;
}

int IsValidKey(void)
{
	HRESULT hres;
	int nRet=0;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres =  CoInitialize(0); 
	if (FAILED(hres))
	{
		return nRet;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	// Note: If you are using Windows 2000, you need to specify -
	// the default authentication credentials for a user by using
	// a SOLE_AUTHENTICATION_LIST structure in the pAuthList ----
	// parameter of CoInitializeSecurity ------------------------

	hres =  CoInitializeSecurity(
		NULL, 
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
		);


	if (FAILED(hres))
	{
		CoUninitialize();
		return nRet;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,             
		0, 
		CLSCTX_INPROC_SERVER, 
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return nRet;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres))
	{
		pLoc->Release();     
		CoUninitialize();
		return nRet;                // Program has failed.
	}

	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
		);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();     
		CoUninitialize();
		return nRet;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"), 
		bstr_t("SELECT * FROM SoftwareLicensingProduct"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return nRet;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
			&pclsObj, &uReturn);

		if(0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;
		VariantInit(&vtProp);
		hr = pclsObj->Get(L"LicenseStatus", 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr))
		{
			if (1 == vtProp.uintVal)
			{
				nRet=1;

				VariantClear(&vtProp);
				// Get the value of the Name property
				VariantInit(&vtProp);
				hr = pclsObj->Get(L"ID", 0, &vtProp, 0, 0);

				if (wcscmp(vtProp.bstrVal,L"e1a8296a-db37-44d1-8cce-7bc961d59c54")!=0)
				{
					nRet=0;
				}

				VariantClear(&vtProp);

				VariantInit(&vtProp);
				hr = pclsObj->Get(L"PartialProductKey", 0, &vtProp, 0, 0);

				if (wcscmp(vtProp.bstrVal,L"8K2YG")!=0)
				{
					nRet=0;
				}

				VariantClear(&vtProp);
				break;
			}
		}
	}

	// Cleanup
	// ========
	if(NULL != pSvc)
	{
		pSvc->Release();
	}
	if(NULL != pLoc)
	{
		pLoc->Release();
	}
	if(NULL != pEnumerator)
	{
		pEnumerator->Release();
	}
	if(NULL != pclsObj)
	{
		pclsObj->Release();
	}
	CoUninitialize();

	return nRet;   // Program successfully completed.
}



VIDEC_API int GetSupportDXVA(void)
{
	int nValue=0;
	if (IsVistaOrAbove())
	{
		nValue=VIDEC_AH2K::GetDevCount();
		if (nValue==0)
		{
			nValue=IsValidKey();
		}
		nValue=1;
	}

	if (nValue)
	{
		nValue=H264SupportPlayer();
	}

	return nValue;
}

static int g_nEnableVideoDisplaySmoothly=1;
VIDEC_API void VIDEC_SetEnableVideoDisplaySmoothly(int nEnable)
{
	g_nEnableVideoDisplaySmoothly=(nEnable?1:0);
}

VIDEC_API int VIDEC_GetEnableVideoDisplaySmoothly(void)
{
	return g_nEnableVideoDisplaySmoothly;
}

static int g_nEnableImageColorEnhance=0;
VIDEC_API void VIDEC_SetEnableImageColorEnhance(int nEnable)
{
	g_nEnableImageColorEnhance=(nEnable?1:0);
}

VIDEC_API int VIDEC_GetEnableImageColorEnhance(void)
{
	return g_nEnableImageColorEnhance;
}

static int g_nEnableDonotEncodeNoSignalVideo=0;
VIDEC_API void VIDEC_SetEnableDonotEncodeNoSignalVideo(int nEnable)
{
	g_nEnableDonotEncodeNoSignalVideo=(nEnable?1:0);
}

VIDEC_API int VIDEC_GetEnableDonotEncodeNoSignalVideo(void)
{
	return g_nEnableDonotEncodeNoSignalVideo;
}

VIDEC_API int VIDEC_GetSupportHardwareEncoder(void)
{
	return (IntelEncoderTry(0,0)!=0?0:1);
}

VIDEC_API void VIDEC_SetHardwareEncoderMaxCount(unsigned int nMaxCount)
{
	IntelEncoderSetMaxCount(nMaxCount);
}
