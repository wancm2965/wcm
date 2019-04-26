// XAH2K.cpp: implementation of the XAH2K class.
//
//////////////////////////////////////////////////////////////////////

#include <winsock2.h>
#include <windows.h>
#include <winioctl.h>
#include "resource.h"
#include "LoadHex.h"
#include "XUtil.h"

#include "XAH2K.h"
#include "memory.h"
#include "DM642DevEx.h"

#define AH2K_MAX_WIDTH		1920
#define AH2K_MAX_HEIGHT		1080



static int	g_nAH2KDevCount  = 0;
static bool	g_bAH2kDevInited = false;


struct DEVChannel {
	int used;
	DEV_EX_TYPE		dev_type;
	CHANNEL_TYPE	channel_type;

	int				channel_index;       // 卡内的编号
	int dev;      // 
};

static struct DEVChannel m_chan[12];
static DM642DevEx g_aDM642DevEx[4];

bool GetEveryChannelHasAudio(int nDevID);

int FindDev(int chan)
{
	if (m_chan[chan].used)
		return m_chan[chan].dev;

	return -1;
}

int VIDEC_AH2K::Init(void)
{
	int nDevCount = DM642DevEx::GetDevExCount(&AH2K_DEVEX_CLASS_GUID);
	if (nDevCount <= 0)
		return -1;

	PBYTE pBufAH2K32 = NULL;
	DWORD dwLenAH2K32 = 0;

	{
		g_nAH2KDevCount = 0;
		if (LoadHex(g_hInstance, IDR_HEX_AH2K32, &pBufAH2K32, dwLenAH2K32))
		{
			for (int i = 0; i < nDevCount; i++)
			{
				DEV_EX_TYPE type;

				DM642DevEx::GetDevType(i, &AH2K_DEVEX_CLASS_GUID, type);
 
				XAH2K::BootLoad(i,pBufAH2K32,dwLenAH2K32,type);

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
				}
			}
		}
	}

	return 0;
}

void XAH2K::BootLoad(int nDevID)
{
	PBYTE pBufAH2K32 = NULL;
	DWORD dwLenAH2K32 = 0;

	if (LoadHex(g_hInstance, IDR_HEX_AH2K32, &pBufAH2K32, dwLenAH2K32))
	{
		DEV_EX_TYPE type;

		DM642DevEx::GetDevType(nDevID, &AH2K_DEVEX_CLASS_GUID, type);

		XAH2K::BootLoad(nDevID,pBufAH2K32,dwLenAH2K32,type);
	}
}

void XAH2K::BootLoad(int nDevID,PBYTE pData,DWORD dwLen,DEV_EX_TYPE nDevType)
{
	if (nDevType == DT_EX_AH2210L || 
		nDevType == DT_EX_AH2205L || 
		nDevType == DT_EX_AH4220E || 
		nDevType == DT_EX_AH3102SE || 
		nDevType == DT_EX_AH3102SEA || 
		nDevType == DT_EX_AH3101A || 
		nDevType == DT_EX_AH3101EA || 
		nDevType == DT_EX_AH3201EA || 
		nDevType == DT_EX_AH3111 || 
		nDevType == DT_EX_AH3111A || 
		nDevType == DT_EX_AH3111EA || 
		nDevType == DT_EX_AH3111E)
	{
		DM642DevEx::BootLoad(nDevID, &AH2K_DEVEX_CLASS_GUID, pData, dwLen,true);
	}
	else
	{
		DM642DevEx::BootLoad(nDevID, &AH2K_DEVEX_CLASS_GUID, pData, dwLen,false);
	}
}


void VIDEC_AH2K::Terminate(void)
{

}

int VIDEC_AH2K::GetDevCount(void)
{
	return g_nAH2KDevCount;
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
};

int VIDEC_AH2K::GetDevName(int nDevID,char*szName,int nMaxCount)
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

int VIDEC_AH2K::GetDevType(int nDevID,VIDEC_DEV_TYPE&nDevType)
{
	if (nDevID<0 || nDevID >= g_nAH2KDevCount)
		return -1;

	if (!m_chan[nDevID].used)
		return -1;

	int nRet=-1;
	switch (m_chan[nDevID].channel_type)
	{
	case CHANNEL_TYPE_SD:
		{
			nDevType=VIDEC_DEV_SD;
			nRet=0;
		}
		break;
	case CHANNEL_TYPE_VGA:
		{
			nDevType=VIDEC_DEV_VGA;
			nRet=0;
		}
		break;
	case CHANNEL_TYPE_HD:
		{
			nDevType=VIDEC_DEV_HD;
			nRet=0;
		}
		break;
	}

	return nRet;
}

bool GetEveryChannelHasAudio(int nDevID)
{
	if (m_chan[nDevID].dev_type==DT_EX_QAVP3400)
	{
		return true;
	}
	return false;
}

bool VIDEC_AH2K::HasAudioInput(int nDevID)
{
	if (m_chan[nDevID].dev_type==DT_EX_AH2KA ||
		m_chan[nDevID].dev_type==DT_EX_AH2205L ||
		m_chan[nDevID].dev_type==DT_EX_AH3102SE ||
		m_chan[nDevID].dev_type==DT_EX_AH3102SEA ||
		m_chan[nDevID].dev_type==DT_EX_AH3111 ||
		m_chan[nDevID].dev_type==DT_EX_AH3111A ||
		m_chan[nDevID].dev_type==DT_EX_AH3111E ||
		m_chan[nDevID].dev_type==DT_EX_AH3111EA ||
		m_chan[nDevID].dev_type==DT_EX_AH3110L ||
		m_chan[nDevID].dev_type==DT_EX_AH3101 ||
		m_chan[nDevID].dev_type==DT_EX_AH3101A ||
		m_chan[nDevID].dev_type==DT_EX_AH3101EA ||
		m_chan[nDevID].dev_type==DT_EX_AH3210 ||
		m_chan[nDevID].dev_type==DT_EX_AH3210A ||
		m_chan[nDevID].dev_type==DT_EX_AH3201EA ||
		m_chan[nDevID].dev_type==DT_EX_QAVP3210 ||
		m_chan[nDevID].dev_type==DT_EX_QAVP3400)
	{
		return true;
	}
	return false;
}

bool VIDEC_AH2K::SupportHardwareEncoding(int nDevID)
{
	if (m_chan[nDevID].dev_type==DT_EX_AH4220E)
	{
		return true;
	}
	return false;
}

VIDEC_AH2K*VIDEC_AH2K::Create(VIDEC_AH2KCallback&rCallback)
{
	return new XAH2K(rCallback);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XAH2K::XAH2K(VIDEC_AH2KCallback&rCallback)
:m_rCallback(rCallback)

,m_pXImage(NULL)
,m_pResize(NULL)

,m_pDev(NULL)

,m_nScaleType(0)

#ifdef __WWENCODER_H__
,m_pWWEncoderChannel(NULL)
#endif
{
	m_nActualWidth=0;
	m_nActualHeight=0;
	m_nVirtualWidth=0;
	m_nVirtualHeight=0;

	m_nImageSize=0;

	m_pTempBuf=NULL;
	m_nBufLen=0;
	m_usDstWidth=0;
	m_usDstHeight=0;

	m_nChannelIndex=0;
	m_nChannelType=CHANNEL_TYPE_HD;

	m_bStartStream=false;
	m_nBitrate=2048;
	m_nBitrateControlType=0;
	m_nQMin=15;
	m_nQMax=42;

	m_nInputWidth=0;
	m_nInputHeight=0;
	m_nInputFrameRate=0;
	m_nPortType=0;

	m_nLastFrameTimestamp=XGetTimestamp();
	m_nResetWidth=0;
	m_nResetHeight=0;
}

XAH2K::~XAH2K(void)
{

}

int XAH2K::Open(int nDevID, int nWidth, int nHeight, int nFrameRate)
{
	m_usDstWidth=0;
	m_usDstHeight=0;

	DM642DevEx *pDev;
	int nIndex = FindDev(nDevID);
	pDev = &g_aDM642DevEx[nIndex];
	m_nChannelIndex = m_chan[nDevID].channel_index;
	m_nChannelType = m_chan[nDevID].channel_type;

	if (nFrameRate <= 1)
		nFrameRate = 1;
	else if (nFrameRate > 30)
	{
		nFrameRate = 100;
	}

	if ((nWidth==1920 && nHeight==1080) ||
		(nWidth==0 && nHeight==0))
	{
		m_nScaleType=0;
	}
	else if ((nWidth<=960 || nHeight<=540) && nWidth!=1920 && nHeight!=1080)
	{
		m_nScaleType=2;
	}
	else
	{
		m_nScaleType=1;
	}

	if (m_nChannelType!=CHANNEL_TYPE_SD)
	{
		m_pResize=VIDEC_Resize::Create();
		if (m_pResize->Open()!=0)
		{
			m_pResize->Close();
			delete m_pResize;
			m_pResize=NULL;
		}

		if (m_pResize!=NULL)
		{
			if (nWidth!=0)
			{
				if (nWidth==1920 && nHeight==540)
				{
					nHeight=1080;
				}
				else
				{
					double dblRatio=(double)nHeight/(double)nWidth;
					m_usDstWidth=nWidth;
					m_usDstHeight=nHeight;

					nWidth=1080.00/dblRatio;
					nHeight=1080;
					if (nWidth%32)
					{
						nWidth+=(32-(nWidth%32));
					}
				}
			}
		}
	}

	if (nWidth>AH2K_MAX_WIDTH)
		nWidth=AH2K_MAX_WIDTH;
	if (nHeight>AH2K_MAX_HEIGHT)
		nHeight=AH2K_MAX_HEIGHT;

	m_nBufLen=AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT*3/2+AH2K_MAX_WIDTH*AH2K_MAX_HEIGHT;

	m_nFrameRate					= nFrameRate;
	m_nResetWidth=nWidth;
	m_nResetHeight=nHeight;

	XAH_PCI2DSP xah_pci2dsp;
	xah_pci2dsp.channel_index = m_nChannelIndex;
	xah_pci2dsp.width	= nWidth;
	xah_pci2dsp.height	= nHeight;

	xah_pci2dsp.framerate	= m_nFrameRate;
	xah_pci2dsp.enable		= 1;
	xah_pci2dsp.scale_type	= m_nScaleType;//0:F 1:I 2:Q

	if (0 != pDev->Open(nIndex, &AH2K_DEVEX_CLASS_GUID,GetEveryChannelHasAudio(nDevID)))
	{
		return -1;
	}

	m_nLastFrameTimestamp=XGetTimestamp();

	pDev->SetChan(m_nChannelIndex, this);
	m_pDev = pDev;
	
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

void XAH2K::Close(void)
{
	StopVideoStream();

	// 通知DSP停止发送视频数据
	if (m_pDev!=NULL)
	{
		XAH_PCI2DSP xah_pci2dsp;
		xah_pci2dsp.channel_index = m_nChannelIndex;
		xah_pci2dsp.width	= 0;
		xah_pci2dsp.height	= 0;

		xah_pci2dsp.framerate	= m_nFrameRate;
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

	if (m_pXImage != NULL)
	{
		m_pXImage->Release();
		m_pXImage = NULL;
	}

	if (m_pResize!=NULL)
	{
		m_pResize->Close();
		delete m_pResize;
		m_pResize=NULL;
	}

	if (m_pTempBuf)
	{
		free(m_pTempBuf);
		m_pTempBuf = NULL;
	}

}


int XAH2K::StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax)
{
#ifdef __WWENCODER_H__
	XAutoLock l(m_csWWEncoderChannel);
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;
	m_nQMin=qmin;
	m_nQMax=qmax;
	m_bStartStream=true;

	m_nInputWidth=0;
	m_nInputHeight=0;
#endif

	return 0;
}

int XAH2K::RequestKeyFrame(void)
{
#ifdef __WWENCODER_H__
	XAutoLock l(m_csWWEncoderChannel);
	if (m_pWWEncoderChannel!=NULL)
	{
		m_pWWEncoderChannel->RequestKeyFrame();
	}
#endif
	return 0;
}

int XAH2K::StopVideoStream(void)
{
	DoStopVideoStream();

#ifdef __WWENCODER_H__
	XAutoLock l(m_csWWEncoderChannel);
	m_bStartStream=false;
#endif
	return 0;
}


void XAH2K::DoStartVideoStream(void)
{
#ifdef __WWENCODER_H__
	XAutoLock l(m_csWWEncoderChannel);
	if (m_bStartStream && m_pWWEncoderChannel==NULL && m_pDev!=NULL && m_pDev->m_hWWDev!=NULL)
	{
		m_pWWEncoderChannel=WWEncoderChannel::Create(*this);
		if (m_pWWEncoderChannel!=NULL)
		{
			if (0!=m_pWWEncoderChannel->Open(m_pDev->m_hWWDev,m_nChannelIndex,m_nInputWidth,m_nInputHeight,m_nFrameRate,m_nInputFrameRate,m_nBitrate,m_nQMin,m_nQMax,m_nBitrateControlType,m_nPortType))
			{
				m_pWWEncoderChannel->Close();
				delete m_pWWEncoderChannel;
				m_pWWEncoderChannel=NULL;
			}
		}
	}
#endif
}

void XAH2K::DoStopVideoStream(void)
{
#ifdef __WWENCODER_H__
	XAutoLock l(m_csWWEncoderChannel);
	if (m_pWWEncoderChannel!=NULL)
	{
		m_pWWEncoderChannel->Close();
		delete m_pWWEncoderChannel;
		m_pWWEncoderChannel=NULL;
	}
#endif
}


void XAH2K::ReadDataFromDsp(PIOBUF pIOBuf)
{
	PPCI_VIDEO_FRAME pVidFrame = (PPCI_VIDEO_FRAME)pIOBuf->pBuf;
	DWORD dwHeaderLen = sizeof(PCI_VIDEO_FRAME);

	if (pVidFrame->base_frame.version != FRAME_VERSION)
		return;

	if (pVidFrame->base_frame.channel != m_nChannelIndex)
		return;

	unsigned int nWidth = pVidFrame->nOutputWidth;
	unsigned int nHeight = pVidFrame->nOutputHeight;

	unsigned short nInputWidth = pVidFrame->nInputWidth;
	unsigned short nInputHeight = pVidFrame->nInputHeight;
	unsigned char nInputFrameRate=pVidFrame->nInputFrameRate;

	unsigned char nPortType=pVidFrame->nPortType;

	unsigned char nFrameRate = pVidFrame->base_frame.framerate;

	unsigned int nActualWidth=m_usDstWidth;
	unsigned int nActualHeight=m_usDstHeight;
	unsigned int nVirtualWidth=m_usDstWidth;
	unsigned int nVirtualHeight=m_usDstHeight;

	bool bAlreadyFieldHeight = (nWidth > (nHeight << 1));
	bool bAlreadyFieldWidth = (nWidth < nHeight);

	if (nWidth > AH2K_MAX_WIDTH || nHeight > AH2K_MAX_HEIGHT ||
		nWidth < 160 || nHeight < 120)
		return ;

	unsigned long ulTimestamp = XGetTimestamp();
	m_nLastFrameTimestamp=ulTimestamp;

	PBYTE pBufY = pIOBuf->pBuf + dwHeaderLen;
	PBYTE pBufU = pBufY + nWidth * nHeight;
	PBYTE pBufV = pBufU + nWidth * nHeight / 4;

	unsigned char *pCapYUV = pBufY;
	bool bResize=false;

	if (m_usDstWidth>=nWidth && m_usDstHeight>=nHeight)
	{
		bResize=false;
	}
	else
	{
		bResize=true;
	}

	if (bResize && (((m_usDstWidth!=0 && m_usDstWidth!=(nWidth*(bAlreadyFieldWidth?2:1))) ||
		 (m_usDstHeight!=0 && m_usDstHeight!=(nHeight*(bAlreadyFieldHeight?2:1)))
		)))
	{
		int nTempDstHeight = (bAlreadyFieldHeight?(m_usDstHeight>>1):m_usDstHeight);
		int nTempDstWidth  = (bAlreadyFieldWidth?(m_usDstWidth>>1):m_usDstWidth);

		double dblRatioDst=(double)m_usDstWidth/(double)nTempDstHeight;
		double dblRatioOrg=(double)nWidth/(double)nHeight;
		double dblDelta=dblRatioDst-dblRatioOrg;
		{
			if (m_pTempBuf==NULL)
			{
				m_pTempBuf=(PBYTE)malloc(m_nBufLen);
				if (m_pTempBuf==NULL)
					return ;
			}

			if (nHeight==540 && bAlreadyFieldHeight)
			{
				nTempDstHeight<<=1;
			}
				
			if (nTempDstHeight>nHeight || (nHeight==1080 && nTempDstHeight>=720))
			{
				nActualHeight=nHeight;
				nTempDstHeight=nHeight;
			}
			else
			{
				nActualHeight=nTempDstHeight;
			}

			nActualWidth=nTempDstWidth;

			if (nWidth!=nTempDstWidth || nHeight!=nTempDstHeight)
			{
				unsigned int nSrcYBufferSize = nWidth * nHeight;
				unsigned int nSrcUVBufferSize = nSrcYBufferSize >> 2;

				unsigned char*src[3];
				src[0]= pCapYUV;
				src[1] = src[0] + nSrcYBufferSize;
				src[2] = src[1] + nSrcUVBufferSize;

				unsigned int nDstYBufferSize = nTempDstWidth*nTempDstHeight;
				unsigned int nDstUVBufferSize = nDstYBufferSize >> 2;

				unsigned char*dst[3];
				dst[0] = m_pTempBuf;
				dst[1] = dst[0] + nDstYBufferSize;
				dst[2] = dst[1] + nDstUVBufferSize;

				if (0==m_pResize->Resize(src,nWidth,nHeight,dst,nTempDstWidth,nTempDstHeight))
				{
					nWidth=nTempDstWidth;
					nHeight=nTempDstHeight;
					pCapYUV=m_pTempBuf;
				}
			}
		}
	}
	else
	{
		if (bAlreadyFieldHeight)
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;
			nVirtualWidth=nWidth;
			nVirtualHeight=nHeight<<1;
		}
		else if (bAlreadyFieldWidth)
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;

			if ((nWidth==480 && nHeight==540))
			{
				nVirtualWidth=m_usDstWidth;
				nVirtualHeight=m_usDstHeight;
			}
			else
			{
				if ((nWidth==960 && nHeight==1080) ||
					(nWidth==640 && nHeight==720))
				{
					nVirtualWidth=nWidth;
				}
				else
				{
					nVirtualWidth=nWidth<<1;
				}
				nVirtualHeight=nHeight;
			}
		}
		else
		{
			nActualWidth=nWidth;
			nActualHeight=nHeight;
			//nVirtualWidth=nWidth;
			//nVirtualHeight=nHeight;
		}
	}

	if (m_nActualWidth!=nActualWidth ||
		m_nActualHeight!=nActualHeight)
	{
		m_nActualWidth=nActualWidth;
		m_nActualHeight=nActualHeight;
		m_nImageSize=m_nActualWidth*m_nActualHeight*3/2;
	}

	if (m_nVirtualWidth!=nVirtualWidth ||
		m_nVirtualHeight!=nVirtualHeight)
	{
		m_nVirtualWidth=nVirtualWidth;
		m_nVirtualHeight=nVirtualHeight;
	}

	if (m_pXImage!=NULL)
	{
		m_pXImage->Release();
		m_pXImage=NULL;
	}

	if (pCapYUV==m_pTempBuf)
	{
		dwHeaderLen=0;
		m_pTempBuf=NULL;
	}
	else 
	{
		pIOBuf->pBuf=NULL;
	}

	XImage*pXImage=new XImage(pCapYUV,m_nImageSize,dwHeaderLen);
	pXImage->AddRef();

	pXImage->SetActualWidth(m_nActualWidth);
	pXImage->SetActualHeight(m_nActualHeight);
	pXImage->SetVirtualWidth(m_nVirtualWidth);
	pXImage->SetVirtualHeight(m_nVirtualHeight);
	pXImage->SetDoubleField(false);
	pXImage->SetEvenField(false);
	pXImage->SetOddField(false);
	pXImage->SetTimestamp(ulTimestamp);
	pXImage->SetHardware(true);

	if (nFrameRate>=50)
	{
		if (nWidth==1280 && nHeight==720)
		{
		}
		else
		{
			nFrameRate>>=1;
		}
	}
	pXImage->SetFrameRate(nFrameRate);

	m_rCallback.OnVIDEC_AH2KCallbackImage(pXImage);

	m_pXImage=pXImage;

#ifdef __WWENCODER_H__
	{
		m_csWWEncoderChannel.Lock();
		if (m_bStartStream && (m_nInputWidth!=nInputWidth || m_nInputHeight!=nInputHeight || m_nInputFrameRate!=nInputFrameRate || m_nPortType!=nPortType))
		{
			m_nInputWidth=nInputWidth;
			m_nInputHeight=nInputHeight;
			m_nInputFrameRate=nInputFrameRate;
			m_nFrameRate=nFrameRate;
			m_nPortType=nPortType;

			if (m_pWWEncoderChannel!=NULL)
			{
				m_pWWEncoderChannel->Close();
				delete m_pWWEncoderChannel;
				m_pWWEncoderChannel=NULL;
			}

			m_csWWEncoderChannel.UnLock();

			if ((nWidth==1280 && nHeight==720) ||
				(nWidth==1920 && nHeight==1080) ||
				(nWidth==1920 && nHeight==540))
			{
				m_pDev->CloseWWDev(this);
				m_pDev->OpenWWDev(this);

				m_nInputWidth=nWidth;
				m_nInputHeight=nHeight;

				DoStartVideoStream();
			}
		}
		else
		{
			m_csWWEncoderChannel.UnLock();
		}
	}
#endif
}


bool XAH2K::IsExpired(void)
{
	unsigned long nTimestamp = XGetTimestamp();
	unsigned long nDelta=nTimestamp-m_nLastFrameTimestamp;
	if (nTimestamp<m_nLastFrameTimestamp)
	{
		m_nLastFrameTimestamp=nTimestamp;
	}
	else if (nDelta>=6000)
	{
		//char szDebug[1024];
		//sprintf(szDebug,"%u nDelta=%u nTimestamp=%u m_nLastFrameTimestamp=%u\n",(unsigned long)this,nDelta,nTimestamp,m_nLastFrameTimestamp);
		//OutputDebugString(szDebug);
		
		m_nLastFrameTimestamp=nTimestamp;
		return true;
	}
	return false;
}

void XAH2K::Reset(void)
{
	{
		XAH_PCI2DSP xah_pci2dsp;
		xah_pci2dsp.channel_index = m_nChannelIndex;
		xah_pci2dsp.width	= 0;
		xah_pci2dsp.height	= 0;

		xah_pci2dsp.framerate	= m_nFrameRate;
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

		xah_pci2dsp.framerate	= m_nFrameRate;
		xah_pci2dsp.enable		= 1;
		xah_pci2dsp.scale_type	= m_nScaleType;//0:F 1:I 2:Q

		m_nLastFrameTimestamp=XGetTimestamp();
		
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


void XAH2K::OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate)
{
	m_rCallback.OnVIDEC_AH2KCallbackAudioData(pSamples,nSamples,nSampleRate);
}

void XAH2K::OnWWEncoderChannelCallbackPacketData(const void*pPacketData,int nPacketLen,int nWidth,int nHeight,int nHeaderLen)
{
	m_rCallback.OnVIDEC_AH2KCallbackVideoStreamData((unsigned char*)pPacketData,nPacketLen,nWidth,nHeight,nHeaderLen);
}
