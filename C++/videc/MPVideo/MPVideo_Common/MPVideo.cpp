//VIDEC.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ATLBASE.H>
#include <wchar.h>
#include <winsock2.h>
#include <windows.h>
#include <MPVideo/MPVideo.h>

#include <VIDEC/VIDEC_CodecDef.h>
#include <AVCONAVC/AVCONAVC.h>

#include <MPVideo/MPVideo_Codec.h>
#include "libxpostproc.h"

#include "../MPVideo_Codecs/IntelCodecWrapper.h"
#include "../MPVideo_Codecs/HWVideoDecoder.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vfw.h>

#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#include "../../VIDEC/VIDEC_FRE/VIDECFREAPI.h"


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

MPVIDEO_API int GetHarewareDecoder(void)
{
	return (g_bHarewareDecoder?1:0);
}

MPVIDEO_API int  MPVideo_GetSupportHardwareDecoder(void)
{
	return HWVideoDecoder::GetSupport();
}

MPVIDEO_API void MPVideo_SetHardwareDecoderMaxCount(unsigned int nMaxCount)
{
	HWVideoDecoder::SetMaxCount(nMaxCount);
}

bool GetOnlyUSBCamera(void)
{
	return g_bOnlyUSBCamera;
}

static unsigned long g_ulMaxKeyFrameInterval=4000;
MPVIDEO_API void MPVideo_SetMaxKeyFrameInterval(unsigned long ulMS)
{
	if (ulMS<1000)
		ulMS=1000;
	else if (ulMS>10000)
	{
		ulMS=10000;
	}
	g_ulMaxKeyFrameInterval=ulMS;
}

MPVIDEO_API unsigned long MPVIDEO_GetMaxKeyFrameInterval(void)
{
	return g_ulMaxKeyFrameInterval;
}


static long g_nVIDEC_RefCount=0;

MPVIDEO_API int MPVideo_Init(void)
{
	if (g_nVIDEC_RefCount<=0)
	{


		VIDEC_CodecInit();
		VIDEC_PostProcInit();



		AVCONAVC_Initialize();

		FRE_Init();

	}

	g_nVIDEC_RefCount++;

	return 0;
}

MPVIDEO_API void MPVideo_Terminate(void)
{
	g_nVIDEC_RefCount--;
	
	FRE_Terminate();

	VIDEC_CodecTerminate();
	VIDEC_PostProcTerminate();

	AVCONAVC_UnInitialize();

	
}


MPVIDEO_API int GetBitrateControlTypeByNetworkType(int nNetworkType)
{
	return nNetworkType;
}

MPVIDEO_API int GetQualityCount(void)
{
	return 14;
}

MPVIDEO_API int GetNameByQuality(int nQuality,char*szName,int nMaxCount)
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


MPVIDEO_API int GetFrameRateCount(void)
{
	return 6;
}

MPVIDEO_API int GetFrameRateByIndex(int nIndex)
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

MPVIDEO_API int GetFrameRateDefaultIndex(void)
{
	return 3;
}

#define H264_MAX_QP_GOOD			49
#define H264_MAX_QP_BETTER			40
#define H264_MAX_QP_BEST			40

#define H263_MAX_QP_GOOD			31
#define H263_MAX_QP_BETTER			22
#define H263_MAX_QP_BEST			16

MPVIDEO_API int GetCodecInfo(int&nQuality,int&nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
{
	int nBitrateControlType=MPVIDEO_NETWORK_PRIVATE;
	return GetCodecInfo(nQuality,nBitrateControlType,nBitrate,nMinQP,nMaxQP,CodecType,bRTP);
}

MPVIDEO_API int GetCodecInfo(int&nQuality,int&nBitrateControlType,int&nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
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
		else
		{
			nQuality=13;
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
	if ((MPVIDEO_NETWORK_QUALITY&ulBitrateControlType)==MPVIDEO_NETWORK_QUALITY)
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
	ulBitrateControlType=ulBitrateControlType & (~MPVIDEO_NETWORK_QUALITY);
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
	};

	return 0;
}

MPVIDEO_API int GetCodecInfoWithBitrate(int&nBitrateControlType,int nBitrate,int&nMinQP,int&nMaxQP,VIDEC_CODEC_TYPE&CodecType,bool bRTP)
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
	if ((MPVIDEO_NETWORK_QUALITY&ulBitrateControlType)==MPVIDEO_NETWORK_QUALITY)
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
	ulBitrateControlType=ulBitrateControlType & (~MPVIDEO_NETWORK_QUALITY);
	nBitrateControlType=ulBitrateControlType;

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

	return nDefaultQP;
}



MPVIDEO_API void GetBitrateRange(int nWidth,int nHeight,int&nQualityMin,int&nQualityMax)
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



MPVIDEO_API int GetSupportDXVA(void)
{
	int nValue=0;
	if (IsVistaOrAbove())
	{
		nValue=IsValidKey();
	}

	if (nValue)
	{
		nValue=HWVideoDecoder::GetSupport();
	}

	return nValue;
}


MPVIDEO_API int MPVIDEO_GetSupportHardwareEncoder(void)
{
	return (IntelEncoderTry(0,0)!=0?0:1);
}

MPVIDEO_API void MPVideo_SetHardwareEncoderMaxCount(unsigned int nMaxCount)
{
	IntelEncoderSetMaxCount(nMaxCount);
}