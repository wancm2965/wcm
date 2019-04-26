#include "stdafx.h"
#include "AH2KDev.h"
#include "AH2KAPI.h"

#include <winsock2.h>
#include <windows.h>
#include <winioctl.h>
#include "resource.h"
#include "LoadHex.h"
#include <mmsystem.h>
#include <emmintrin.h>

#include "AH2KDev.h"
#include "memory.h"
#include "PcieDevEx.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

//#define AH2K_MAX_WIDTH		1920
//#define AH2K_MAX_HEIGHT		1080
//

#define CH0_DE_INTERLACE_REG	0x520
#define CH1_DE_INTERLACE_REG	0x420
#define CH2_DE_INTERLACE_REG	0x620
#define CH3_DE_INTERLACE_REG	0x720


static int	g_nAH2KDevCount  = 0;
static bool	g_bAH2kDevInited = false;

static GUID g_guidAH2K=AH2KEX_DEVEX_CLASS_GUID;


struct DEVChannel 
{
	int				used;
	DEV_EX_TYPE		dev_type;
	CHANNEL_TYPE	channel_type;

	int				channel_index;       // 卡内的编号
	int				dev;      // 
};

static struct DEVChannel m_chan[12];
static PcieDevEx g_aPcieDevEx[4];

bool GetEveryChannelHasAudio(int nDevID);

int FindDev(int chan)
{
	if (m_chan[chan].used)
		return m_chan[chan].dev;

	return -1;
}

void AH2KDev::BootLoad(int nDevID)
{
	PBYTE pBufAH2K32 = NULL;
	DWORD dwLenAH2K32 = 0;

	DEV_EX_TYPE type;

	PcieDevEx::GetDevType(nDevID, &g_guidAH2K, type);
	if (type == DT_EX_AH5112SE	||
		type == DT_EX_AH5011SE	||
		type == DT_EX_AH5111SE	||
		type == DT_EX_AH5102SE	||
		type == DT_EX_AH5002SE	||
		type == DT_EX_AH5211SE	||
		type == DT_EX_AH5210SEB)
	{
		return;
	}

	if (LoadHex(g_hInstance, IDR_HEX, &pBufAH2K32, dwLenAH2K32))
	{
		AH2KDev::BootLoad(nDevID,pBufAH2K32,dwLenAH2K32,type);
	}
}

void AH2KDev::BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType)
{
	if (nDevType == DT_EX_AH5112SE	||
		nDevType == DT_EX_AH5011SE	||
		nDevType == DT_EX_AH5111SE	||
		nDevType == DT_EX_AH5102SE	||
		nDevType == DT_EX_AH5002SE	||
		nDevType == DT_EX_AH5211SE	||
		nDevType == DT_EX_AH5210SEB)
	{
		//do not download firmware to card.
	}
	else if (nDevType == DT_EX_AH2210L	|| 
		nDevType == DT_EX_AH2205L	|| 
		nDevType == DT_EX_AH4220E	|| 
		nDevType == DT_EX_AH3102SE	|| 
		nDevType == DT_EX_AH3102SEA || 
		nDevType == DT_EX_AH3101A	|| 
		nDevType == DT_EX_AH3101EA	|| 
		nDevType == DT_EX_AH3201EA	|| 
		nDevType == DT_EX_HAVP3201	|| 
		nDevType == DT_EX_AH3111	|| 
		nDevType == DT_EX_AH3111A	|| 
		nDevType == DT_EX_AH3111EA	||
		nDevType == DT_EX_HAVP3002SM||
		nDevType == DT_EX_AH3002SE_T||
		nDevType == DT_EX_AH3110ED_T||
		nDevType == DT_EX_AH3110D_T	||
		nDevType == DT_EX_AH3111E	||
		nDevType == DT_EX_AH3002SE	||
		nDevType == DT_EX_AH3210ED	||
		nDevType == DT_EX_AH3110ED	||
		nDevType == DT_EX_AH3210D	||
		nDevType == DT_EX_AH3110D)
	{
		PcieDevEx::BootLoad(nDevID, &g_guidAH2K, pData, dwLen,true);
	}
	else
	{
		PcieDevEx::BootLoad(nDevID, &g_guidAH2K, pData, dwLen,false);
	}
}


char *DEV_NAME[] = {
	"ERROR DEV",
	"AVCON AH1000 %s_DEV (%d)",
	"AVCON AH2000 %s_DEV (%d)",
	"AVCON AH2000A %s_DEV (%d)", 
	"AVCON AH410 %s_DEV (%d)",
	"AVCON AH420L %s_DEV (%d)",
	"AVCON AH210 %s_DEV (%d)",
	"AVCON AH1100L %s_DEV (%d)",
	"AVCON AH2100L %s_DEV (%d)",
	"AVCON AH2200L %s_DEV (%d)",
	"AVCON AH2040 %s_DEV (%d)",
	"AVCON AH2210L %s_DEV (%d)",
	"AVCON AH4220E %s_DEV (%d)",
	"AVCON AH2205L %s_DEV (%d)",
	"AVCON AH3110L %s_DEV (%d)",
	"AVCON AH3102SE %s_DEV (%d)",
	"AVCON AH3101 %s_DEV (%d)",
	"AVCON AH3111 %s_DEV (%d)",
	"AVCON AH3111A %s_DEV (%d)",
	"AVCON AH3111E %s_DEV (%d)",
	"AVCON AH3210 %s_DEV (%d)",
	"AVCON AH3210A %s_DEV (%d)",
	"AVCON AH3111EA %s_DEV (%d)",
	"AVCON AH3101A %s_DEV (%d)",
	"AVCON AH3101EA %s_DEV (%d)",
	"AVCON AH3102SEA %s_DEV (%d)",
	"AVCON QAVP3210 %s_DEV (%d)",
	"AVCON QAVP3400 %s_DEV (%d)",
	"AVCON AH3201EA %s_DEV (%d)",
	"AVCON HAVP3201 %s_DEV (%d)",
	"AVCON HAVPM3002SM %s_DEV (%d)",
	"AVCON AH3002SE_T %s_DEV (%d)",
	"AVCON AH3110ED_T %s_DEV (%d)",
	"AVCON AH3110D_T %s_DEV (%d)",
	"AVCON AH3002SE %s_DEV (%d)",
	"AVCON AH3210ED %s_DEV (%d)",
	"AVCON AH3110ED %s_DEV (%d)",
	"AVCON AH3210D %s_DEV (%d)",
	"AVCON AH3110D %s_DEV (%d)",
	"AVCON AH5112SE %s_DEV (%d)",
	"AVCON AH5011SE %s_DEV (%d)",
	"AVCON AH5111SE %s_DEV (%d)",
	"AVCON AH5102SE %s_DEV (%d)",
	"AVCON AH5002SE %s_DEV (%d)",
	"AVCON AH5211SE %s_DEV (%d)",
	"AVCON AH5210SEB %s_DEV (%d)"
};



bool GetEveryChannelHasAudio(int nDevID)
{
	if (m_chan[nDevID].dev_type==DT_EX_QAVP3400)
	{
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AH2KDev::AH2KDev(void)
:m_pDev(NULL)

,m_nScaleType(0)
{
	m_nBufLen=0;

	m_nChannelIndex=0;
	m_nChannelType=CHANNEL_TYPE_HD;

	m_nLastFrameTimestamp=timeGetTime();
	m_nResetWidth=0;
	m_nResetHeight=0;
	m_nResetFrameRate=30;

	m_video_cb=NULL;
	m_audio_cb=NULL;
	m_pUserData=NULL;

	m_nHasVideoSignal=0;
}

AH2KDev::~AH2KDev(void)
{

}

int AH2KDev::Open(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData)
{
	m_video_cb=video_cb;
	m_audio_cb=audio_cb;
	m_pUserData=pUserData;

	PcieDevEx *pDev;
	int nIndex = FindDev(nDevID);
	pDev = &g_aPcieDevEx[nIndex];
	PcieDevEx::GetDevType(nIndex, &g_guidAH2K, pDev->m_deviceType);
	m_nChannelIndex = m_chan[nDevID].channel_index;
	m_nChannelType = m_chan[nDevID].channel_type;


	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;


	if (0 != pDev->Open(nIndex, &g_guidAH2K,GetEveryChannelHasAudio(nDevID)))
	{
		return -1;
	}

	m_nLastFrameTimestamp=timeGetTime();
	pDev->SetChan(m_nChannelIndex, this);
	m_pDev = pDev;

	return 0;
}

int AH2KDev::SetParams(int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	if (m_pDev==NULL)
	{
		return -1;
	}

	if (nScaleType<0)
	{
		nScaleType=0;
	}
	else if (nScaleType>4)
	{
		nScaleType=4;
	}

	m_nScaleType=nScaleType;

	if (nWidth>AH2K_MAX_WIDTH)
	{
		nWidth=AH2K_MAX_WIDTH;
	}

	if (nHeight>AH2K_MAX_HEIGHT)
	{
		nHeight=AH2K_MAX_HEIGHT;
	}

	if (nFrameRate <= 1)
	{
		nFrameRate = 1;
	}
	else if (nFrameRate > 60)
	{
		nFrameRate = 100;
	}

	m_nResetFrameRate = nFrameRate;
	m_nResetWidth = nWidth;
	m_nResetHeight = nHeight;
	
	XAH_PCI2DSP xah_pci2dsp;
	xah_pci2dsp.channel_index = m_nChannelIndex;
	xah_pci2dsp.width	= nWidth;
	xah_pci2dsp.height	= nHeight;

	xah_pci2dsp.framerate	= nFrameRate;
	xah_pci2dsp.enable		= 1;
	//xah_pci2dsp.scale_type	= m_nScaleType;//0:F 1:I 2:Q

	xah_pci2dsp.scale_type	= m_nScaleType;//0:F 1:I 2:Q 3:de-interlace 4:non-de-interlace

	switch (m_nChannelIndex)
	{
	case 0:
		/*m_pDev->RegisterWriteULong(0,CH0_DE_INTERLACE_REG,0);
		if (nScaleType==4)*/
			m_pDev->RegisterWriteULong(0,CH0_DE_INTERLACE_REG,1);
		break;
	case 1:
		/*m_pDev->RegisterWriteULong(0,CH1_DE_INTERLACE_REG,0);
		if (nScaleType==4)*/
			m_pDev->RegisterWriteULong(0,CH1_DE_INTERLACE_REG,1);
		break;
	case 2:
		m_pDev->RegisterWriteULong(0,CH2_DE_INTERLACE_REG,0);
		if (nScaleType==4)
			m_pDev->RegisterWriteULong(0,CH2_DE_INTERLACE_REG,1);
		break;
	case 3:
		m_pDev->RegisterWriteULong(0,CH3_DE_INTERLACE_REG,0);
		if (nScaleType==4)
			m_pDev->RegisterWriteULong(0,CH3_DE_INTERLACE_REG,1);
		break;
	default: break;
	}

	m_nLastFrameTimestamp=timeGetTime();

	int nPostMsgCount=0;
	while (nPostMsgCount<4000)
	{
		if (0==m_pDev->PostMsg((PBYTE)&xah_pci2dsp,sizeof(xah_pci2dsp)))
		{
			nPostMsgCount++;
			Sleep(1);
		}
		else
		{
			break;
		}
	}

	return 0;
}


void AH2KDev::Close(void)
{
	// 通知DSP停止发送视频数据
	if (m_pDev!=NULL)
	{
		XAH_PCI2DSP xah_pci2dsp;
		xah_pci2dsp.channel_index = m_nChannelIndex;
		xah_pci2dsp.width	= 0;
		xah_pci2dsp.height	= 0;

		xah_pci2dsp.framerate	= 0;
		xah_pci2dsp.enable		= 0;
		xah_pci2dsp.scale_type	= m_nScaleType;

		int nPostMsgCount=0;
		while (nPostMsgCount<4000)
		{
			if (0==m_pDev->PostMsg((PBYTE)&xah_pci2dsp,sizeof(xah_pci2dsp)))
			{
				nPostMsgCount++;
				Sleep(1);
			}
			else
			{
				break;
			}
		}

		m_pDev->Close(m_nChannelIndex);

		m_pDev=NULL;
	}
}

void AH2KDev::ReadDataFromDsp(PIOBUF pIOBuf)
{
	PPCI_VIDEO_FRAME pVidFrame = (PPCI_VIDEO_FRAME)pIOBuf->pBuf;
	DWORD dwHeaderLen = sizeof(PCI_VIDEO_FRAME);

	if (pVidFrame->base_frame.version != FRAME_VERSION)
	{
		DebugOutput( logcoreexcep ,"AH2KDev   ReadDataFromDsp version\n");
		return;
	}

	if (pVidFrame->base_frame.channel != m_nChannelIndex)
	{
		DebugOutput( logcoreexcep ,"AH2KDev   ReadDataFromDsp channel\n");
		return;
	}

	if (pVidFrame->base_frame.frame_type==FRAME_VIDEO)
	{
		m_nHasVideoSignal=1;
	}
	else if (pVidFrame->base_frame.frame_type==FRAME_VIDEO_NO_SIGNAL)
	{
		m_nHasVideoSignal=0;
	}

	unsigned int nWidth = pVidFrame->nOutputWidth;
	unsigned int nHeight = pVidFrame->nOutputHeight;
	unsigned char nFrameRate = pVidFrame->base_frame.framerate;

	if (nWidth > AH2K_MAX_WIDTH || 
		nHeight > AH2K_MAX_HEIGHT ||
		nWidth < 160 || 
		nHeight < 120)
	{
		DebugOutput( logcoreexcep ,"AH2KDev   ReadDataFromDsp nWidth\n");
		return;
	}

	unsigned long ulTimestamp = timeGetTime();
	m_nLastFrameTimestamp=ulTimestamp;

	PBYTE pFrameData = pIOBuf->pBuf + dwHeaderLen;
	int nFrameLen=nWidth*nHeight*3/2;

	unsigned char nDspInputFrameType = pVidFrame->nInputFrameType;
	unsigned long nInputFrameType = 0;

	if (nDspInputFrameType & 0x01)
	{
		nInputFrameType |= INPUT_FRAME_TYPE_MERGED;
	}

	unsigned char nInputFrameTypeYV = (nDspInputFrameType & 0x0E)>>1;
	if ( 0 == nInputFrameTypeYV)
	{
		//nInputFrameType |= INPUT_FRAME_TYPE_422;
		int nWidth_X_Height = nWidth*nHeight;
		int nHeightLoop=(nHeight>>1); 
		int nWidthLoop=(nWidth>>4); 
		unsigned char*pSrcUV=pFrameData+nWidth_X_Height; 
		unsigned char*pDstUV=pSrcUV; 

		for( int i = 0; i < nHeightLoop; i++ ) 
		{ 
			for( int j = 0; j < nWidthLoop; j++ ) 
			{ 
				*( ( __m128i * ) pDstUV ) = _mm_avg_epu8( *( ( __m128i * ) pSrcUV ), *( ( __m128i * ) ( pSrcUV + nWidth ) ) ); 
				pSrcUV += 16; 
				pDstUV += 16; 
			} 
			pSrcUV += nWidth; 
		} 
	}

	unsigned char nInputFrameTypeI = (nDspInputFrameType & 0x10)>>4;
	if (nInputFrameTypeI)
	{
		nInputFrameType |= INPUT_FRAME_TYPE_I;
	}

	//char DebugString[1024]= "";
	//sprintf(DebugString,"the nDspInputFrameType is %d and the nInputFrameType is %u\n",nDspInputFrameType,nInputFrameType);
	//OutputDebugStringA(DebugString);

	if (m_video_cb)
	{
		if (NULL==m_video_cb(m_pUserData,pFrameData,nFrameLen,nWidth,nHeight,nFrameRate,timeGetTime(),dwHeaderLen,nInputFrameType))
		{
			pIOBuf->pBuf=NULL;
		}
	}
}


bool AH2KDev::IsExpired(void)
{
	unsigned long nTimestamp = timeGetTime();
	unsigned long nDelta=nTimestamp-m_nLastFrameTimestamp;
	if (nTimestamp<m_nLastFrameTimestamp)
	{
		m_nLastFrameTimestamp=nTimestamp;
	}
	else if (nDelta>=6000)
	{
		//char szDebug[1024];
		//sprintf(szDebug,"%u nDelta=%u nTimestamp=%u m_nLastFrameTimestamp=%u\n",(unsigned long)this,nDelta,nTimestamp,m_nLastFrameTimestamp);
		//DebugOutput(szDebug);

		m_nLastFrameTimestamp=nTimestamp;
		return true;
	}
	return false;
}

void AH2KDev::Reset(void)
{
	if(m_pDev->m_deviceType ==  DT_EX_AH5112SE	||
		m_pDev->m_deviceType == DT_EX_AH5011SE	||
		m_pDev->m_deviceType == DT_EX_AH5111SE	||
		m_pDev->m_deviceType == DT_EX_AH5102SE	||
		m_pDev->m_deviceType == DT_EX_AH5002SE	||
		m_pDev->m_deviceType == DT_EX_AH5211SE  ||
		m_pDev->m_deviceType == DT_EX_AH5210SEB) //for FPGA Card
	{
		//no need to reset while using FPGA Card
		Sleep(1);

	}
	else
	{
		{
			XAH_PCI2DSP xah_pci2dsp;
			xah_pci2dsp.channel_index = m_nChannelIndex;
			xah_pci2dsp.width	= 0;
			xah_pci2dsp.height	= 0;

			xah_pci2dsp.framerate	= 0;
			xah_pci2dsp.enable		= 0;
			xah_pci2dsp.scale_type	= m_nScaleType;

			int nPostMsgCount=0;
			while (nPostMsgCount<4000)
			{
				if (0==m_pDev->PostMsg((PBYTE)&xah_pci2dsp,sizeof(xah_pci2dsp)))
				{
					nPostMsgCount++;
					Sleep(1);
				}
				else
				{
					break;
				}
			}
		}

		{
			XAH_PCI2DSP xah_pci2dsp;
			xah_pci2dsp.channel_index = m_nChannelIndex;
			xah_pci2dsp.width	= m_nResetWidth;
			xah_pci2dsp.height	= m_nResetHeight;

			xah_pci2dsp.framerate	= m_nResetFrameRate;
			xah_pci2dsp.enable		= 1;
			xah_pci2dsp.scale_type	= m_nScaleType;//0:F 1:I 2:Q

			m_nLastFrameTimestamp=timeGetTime();

			int nPostMsgCount=0;
			while (nPostMsgCount<4000)
			{
				if (0==m_pDev->PostMsg((PBYTE)&xah_pci2dsp,sizeof(xah_pci2dsp)))
				{
					nPostMsgCount++;
					Sleep(1);
				}
				else
				{
					break;
				}
			}
		}
	}
}


void AH2KDev::OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate)
{
	if (m_audio_cb)
	{
		m_audio_cb(m_pUserData,pSamples,nSamples,nSampleRate,timeGetTime());
	}
}



int AH2K_Initialize(void)
{
	if (g_bAH2kDevInited)
	{
		return 0;
	}
	g_bAH2kDevInited=true;

	int nDevCount = PcieDevEx::GetDevExCount(&AH2KEX_DEVEX_CLASS_GUID);
	if (nDevCount <= 0)
	{
		nDevCount = PcieDevEx::GetDevExCount(&AH2K_DEVEX_CLASS_GUID);
		if (nDevCount <= 0)
		{
			return -1;
		}
		else
		{
			g_guidAH2K=AH2K_DEVEX_CLASS_GUID;
		}
	}
	else
	{
		g_guidAH2K=AH2KEX_DEVEX_CLASS_GUID;
	}

	PBYTE pBufAH2K32 = NULL;
	DWORD dwLenAH2K32 = 0;

	{
		g_nAH2KDevCount = 0;
		//if (LoadHex(g_hInstance, IDR_HEX, &pBufAH2K32, dwLenAH2K32))
		{
			for (int i = 0; i < nDevCount; i++)
			{
				DEV_EX_TYPE type;

				PcieDevEx::GetDevType(i, &g_guidAH2K, type);
				if (type != DT_EX_AH5112SE	&&
					type != DT_EX_AH5011SE	&&
					type != DT_EX_AH5111SE	&&
					type != DT_EX_AH5102SE	&&
					type != DT_EX_AH5002SE	&&
					type != DT_EX_AH5211SE 	&&
					type != DT_EX_AH5210SEB)
				{
					if (!LoadHex(g_hInstance, IDR_HEX, &pBufAH2K32, dwLenAH2K32))
						return -1;

					AH2KDev::BootLoad(i,pBufAH2K32,dwLenAH2K32,type);
				}

				switch (type)
				{
				case DT_EX_AH2K:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH2KA:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH420L:
				case DT_EX_AH410:
				case DT_EX_QAVP3400:
					for (int n = 0; n < 4; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				case DT_EX_AH210:
					for (int n = 0; n < 2; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				case DT_EX_AH1100L://VGA
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_VGA;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 1;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH2100L:
				case DT_EX_AH3110L:
				case DT_EX_AH3101:
				case DT_EX_AH3101A:
				case DT_EX_AH3101EA:
				case DT_EX_AH3110ED:
				case DT_EX_AH3110D:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 1;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH2200L:
				case DT_EX_HAVP3002SM:
				case DT_EX_AH3002SE:
				case DT_EX_AH5011SE:
					for (int n = 0; n < 2; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				case DT_EX_AH2210L:
				case DT_EX_AH2205L:
				case DT_EX_AH3102SE:
				case DT_EX_AH3102SEA:
				case DT_EX_AH3111:
				case DT_EX_AH3111A:
				case DT_EX_AH3111E:
				case DT_EX_AH3111EA:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 1;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 2;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH3210:
				case DT_EX_AH3210A:
				case DT_EX_AH3201EA:
				case DT_EX_QAVP3210:
				case DT_EX_HAVP3201:
				case DT_EX_AH3210ED:
				case DT_EX_AH3210D:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 1;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 2;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH4220E:
				case DT_EX_AH3002SE_T:
				case DT_EX_AH3110ED_T:
				case DT_EX_AH3110D_T:
				case DT_EX_AH5002SE:
					m_chan[g_nAH2KDevCount].channel_index = 0;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;

					m_chan[g_nAH2KDevCount].channel_index = 1;
					m_chan[g_nAH2KDevCount].dev_type = type;
					m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
					m_chan[g_nAH2KDevCount].dev = i;
					m_chan[g_nAH2KDevCount++].used = 1;
					break;
				case DT_EX_AH5112SE:
				case DT_EX_AH5211SE:
					for (int n=0; n<2; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					for (int n=2; n<4; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				case DT_EX_AH5111SE:
				case DT_EX_AH5102SE:
					for (int n=0; n<2; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					for (int n=2; n<3; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				case DT_EX_AH5210SEB:
					for (int n=0; n<1; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_HD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					for (int n=1; n<3; n++)
					{
						m_chan[g_nAH2KDevCount].channel_index = n;
						m_chan[g_nAH2KDevCount].dev_type = type;
						m_chan[g_nAH2KDevCount].channel_type = CHANNEL_TYPE_SD;
						m_chan[g_nAH2KDevCount].dev = i;
						m_chan[g_nAH2KDevCount++].used = 1;
					}
					break;
				}
			}
		}
	}

	return 0;
}


int AH2K_GetDevCount(void)
{
	AH2K_Initialize();

	return g_nAH2KDevCount;
}

int AH2K_GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0 || nDevID >= g_nAH2KDevCount)
		return -1;

	if (nMaxCount<=0)
		return -1;

	if (!m_chan[nDevID].used)
		return -1;

	char szTemp[128];
	size_t size;
	char szType[128]="";

	switch (m_chan[nDevID].channel_type)
	{
	case CHANNEL_TYPE_SD:
		{
			strcpy(szType,"SD");
		}
		break;
	case CHANNEL_TYPE_VGA:
		{
			strcpy(szType,"VGA");
		}
		break;
	case CHANNEL_TYPE_HD:
		{
			strcpy(szType,"HD");
		}
		break;
	default:
		return -1;
	}

	sprintf(szTemp, DEV_NAME[m_chan[nDevID].dev_type],szType,nDevID);
	size = strlen(szTemp) + 1;

	if (szName != NULL)
	{
		if (nMaxCount>=size)
		{
			strcpy(szName,szTemp);
		}
		else
		{
			strncpy(szName,szTemp,nMaxCount);
		}
	}

	return size;
}

int AH2K_HasAudio(int nDevID)
{
	if (m_chan[nDevID].dev_type==DT_EX_AH2KA		||
		m_chan[nDevID].dev_type==DT_EX_AH2205L		||
		m_chan[nDevID].dev_type==DT_EX_AH3102SE		||
		m_chan[nDevID].dev_type==DT_EX_AH3102SEA	||
		m_chan[nDevID].dev_type==DT_EX_AH3111		||
		m_chan[nDevID].dev_type==DT_EX_AH3111A		||
		m_chan[nDevID].dev_type==DT_EX_AH3111E		||
		m_chan[nDevID].dev_type==DT_EX_AH3111EA		||
		m_chan[nDevID].dev_type==DT_EX_AH3110L		||
		m_chan[nDevID].dev_type==DT_EX_AH3101		||
		m_chan[nDevID].dev_type==DT_EX_AH3101A		||
		m_chan[nDevID].dev_type==DT_EX_AH3101EA		||
		m_chan[nDevID].dev_type==DT_EX_AH3210		||
		m_chan[nDevID].dev_type==DT_EX_AH3210A		||
		m_chan[nDevID].dev_type==DT_EX_AH3201EA		||
		m_chan[nDevID].dev_type==DT_EX_HAVP3201		||
		m_chan[nDevID].dev_type==DT_EX_QAVP3210		||
		m_chan[nDevID].dev_type==DT_EX_HAVP3002SM	||
		m_chan[nDevID].dev_type==DT_EX_QAVP3400		||
		m_chan[nDevID].dev_type==DT_EX_AH3002SE_T	||
		m_chan[nDevID].dev_type==DT_EX_AH5112SE)
	{
		return 1;
	}
	
	return 0;
}

int AH2K_IsHD(int nDevID)
{
	if (nDevID<0 || nDevID >= g_nAH2KDevCount)
		return 0;

	if (!m_chan[nDevID].used)
		return 0;

	int nRet=-0;
	switch (m_chan[nDevID].channel_type)
	{
	case CHANNEL_TYPE_SD:
		{
			nRet=0;
		}
		break;
	case CHANNEL_TYPE_VGA:
		{
			nRet=1;
		}
		break;
	case CHANNEL_TYPE_HD:
		{
			nRet=1;
		}
		break;
	}

	return nRet;
}

void*AH2K_Open(int nDevID,AH2K_VIDEO_CB video_cb,AH2K_AUDIO_CB audio_cb,const void*pUserData)
{
	NEWRECORD3( AH2KDev*,pAH2KDev,new AH2KDev() )
	if (0!=pAH2KDev->Open(nDevID,video_cb,audio_cb,pUserData))
	{
		pAH2KDev->Close();
		delete pAH2KDev;
		pAH2KDev=NULL;
	}
	return pAH2KDev;
}

void AH2K_Close(void*pHandle)
{
	AH2KDev*pAH2KDev=(AH2KDev*)pHandle;
	if (pAH2KDev)
	{
		pAH2KDev->Close();
		delete pAH2KDev;
		pAH2KDev=NULL;
	}
}

int AH2K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	int nRet=-1;
	AH2KDev*pAH2KDev=(AH2KDev*)pHandle;
	if (pAH2KDev)
	{
		nRet=pAH2KDev->SetParams(nScaleType,nWidth,nHeight,nFrameRate);
	}

	return nRet;
}


int AH2K_HasVideoSignal(void*pHandle)
{
	int nRet=0;
	AH2KDev*pAH2KDev=(AH2KDev*)pHandle;
	if (pAH2KDev)
	{
		nRet=pAH2KDev->HasVideoSignal();
	}

	return nRet;
}
