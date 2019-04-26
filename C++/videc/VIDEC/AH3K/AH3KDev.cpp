#include "stdafx.h"
#include "AH3KDev.h"
#include "AH3KAPI.h"
#include <mmsystem.h>
#include <string>

#include "AH3KDev.h"
#include "AudioChan.h"

#pragma comment(lib,"winmm.lib")

#define AH3K_MAX_WIDTH		1920
#define AH3K_MAX_HEIGHT		1080


typedef struct tagDEV_CHAN
{
	int				nDevID;
	int				nChanID;
	int				nChanType;
	std::string		strDevName;
	std::string		strDevPath;
	std::string		strChanName;
	std::string		strChanPath;
}DEV_CHAN;

static DEV_CHAN			g_aDevVideoChan[32];
static int				g_nDevVideoChanCount=0;
static bool				g_bAH3KInitialied=false;
static const GUID		g_guidAH3K=GUID_DEVICE_AH3K;

static DEV_CHAN			g_aDevAudioChan[32];
static int				g_nDevAudioChanCount=0;

typedef std::map<void*,AudioChan*>	MapAudioChan;

static MapAudioChan		g_MapAudioChan;


void AddAH3KDev(int nPhyicalDevID,AH3KDev*pAH3KDev)
{
	AudioChan*pAudioChan=NULL;
	MapAudioChan::iterator iter;
	for (iter=g_MapAudioChan.begin();iter!=g_MapAudioChan.end();++iter)
	{
		pAudioChan=iter->second;
		if (pAudioChan->GetPhyicalDevID()==nPhyicalDevID)
		{
			break;
		}
		else
		{
			pAudioChan=NULL;
		}
	}

	if (pAudioChan==NULL)
	{
		std::string strChanPath="";
		for (int i=0;i<g_nDevAudioChanCount;++i)
		{
			if (g_aDevAudioChan[i].nDevID==nPhyicalDevID)
			{
				strChanPath=g_aDevAudioChan[i].strChanPath;
				break;
			}
		}

		if (strChanPath.size())
		{
			pAudioChan=new AudioChan(nPhyicalDevID);
			if (0!=pAudioChan->Open(strChanPath.c_str()))
			{
				pAudioChan->Close();
				delete pAudioChan;
				pAudioChan=NULL;
			}
			else
			{
				g_MapAudioChan[pAudioChan]=pAudioChan;
			}
		}
	}

	if (pAudioChan)
	{
		pAudioChan->AddAH3KDev(pAH3KDev);
	}
}

void RemoveAH3KDev(int nPhyicalDevID,AH3KDev*pAH3KDev)
{
	AudioChan*pAudioChan=NULL;
	MapAudioChan::iterator iter;
	for (iter=g_MapAudioChan.begin();iter!=g_MapAudioChan.end();++iter)
	{
		pAudioChan=iter->second;
		if (pAudioChan->GetPhyicalDevID()==nPhyicalDevID)
		{
			break;
		}
		else
		{
			pAudioChan=NULL;
		}
	}

	if (pAudioChan)
	{
		pAudioChan->RemoveAH3KDev(pAH3KDev);
		if (pAudioChan->GetAH3KDevCount()==0)
		{
			g_MapAudioChan.erase(pAudioChan);

			pAudioChan->Close();
			delete pAudioChan;
			pAudioChan=NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AH3KDev::AH3KDev(void)
:m_nScaleType(0)
,m_hDevice(INVALID_HANDLE_VALUE)
,m_bRunning(false)
,m_nDevID(0)
,m_nChanID(0)
,m_nPhyicalDevID(0)
{
	m_nBufLen=0;

	m_nChannelIndex=0;
	m_nChannelType=0;


	m_video_cb=NULL;
	m_audio_cb=NULL;
	m_pUserData=NULL;

	ZeroMemory(&m_Overlapped,sizeof(m_Overlapped));
	m_Overlapped.hEvent = NULL;

	m_pBuffer=NULL;
}

AH3KDev::~AH3KDev(void)
{

}

int AH3KDev::Open(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData)
{
	if (nDevID<0 || nDevID>=g_nDevVideoChanCount)
	{
		return -1;
	}

	m_nDevID=nDevID;
	m_nChanID=g_aDevVideoChan[nDevID].nChanID;
	m_nPhyicalDevID=g_aDevVideoChan[nDevID].nDevID;

	std::string strChanPath=g_aDevVideoChan[nDevID].strChanPath;

	m_video_cb=video_cb;
	m_audio_cb=audio_cb;
	m_pUserData=pUserData;

	m_nBufLen=AH3K_MAX_WIDTH*AH3K_MAX_HEIGHT*3/2+AH3K_MAX_WIDTH*AH3K_MAX_HEIGHT;

	m_hDevice=CreateFile(strChanPath.c_str(),
		GENERIC_READ | GENERIC_WRITE, 
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(INVALID_HANDLE_VALUE==m_hDevice)
	{					
		return -1;
	}	
	
	m_Overlapped.hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

	m_bRunning=true;
	StartThread();

	if (AH3K_HasAudio(nDevID))
	{
		AddAH3KDev(m_nPhyicalDevID,this);
	}

	return 0;
}

int AH3KDev::SetParams(int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	if (nScaleType<0)
		nScaleType=0;
	else if (nScaleType>2)
		nScaleType=2;

	m_nScaleType=nScaleType;

	if (nWidth>AH3K_MAX_WIDTH)
		nWidth=AH3K_MAX_WIDTH;
	if (nHeight>AH3K_MAX_HEIGHT)
		nHeight=AH3K_MAX_HEIGHT;

	if (nFrameRate <= 1)
	{
		nFrameRate = 1;
	}
	else if (nFrameRate > 30)
	{
		nFrameRate = 100;
	}


	HANDLE hDevice=CreateFile(g_aDevVideoChan[m_nDevID].strDevPath.c_str(),
		GENERIC_READ | GENERIC_WRITE, 
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(INVALID_HANDLE_VALUE!=hDevice)
	{	
		OVERLAPPED tOverlapped;
		ZeroMemory(&tOverlapped,sizeof(tOverlapped));
		tOverlapped.hEvent=CreateEvent(0,0,0,0);


		MSG_PACKET packet;
		ZeroMemory(&packet,sizeof(packet));			

		packet.channel_index=m_nChanID;
		packet.enable=1;
		packet.scale_type=nScaleType;
		packet.framerate=nFrameRate;


		DWORD dwBytesRead;
		DWORD dwStatus[128];

		//IOCTL_AH3K_DSP_CMD 是可能被挂起的，如果这种情况出现一般认为DSP死机了
		if(!DeviceIoControl(hDevice,IOCTL_AH3K_DSP_CMD,&packet,sizeof(packet),dwStatus,sizeof(dwStatus),&dwBytesRead,&tOverlapped))
		{
			if(ERROR_IO_PENDING==GetLastError())
			{
				if(WAIT_OBJECT_0==WaitForSingleObject(tOverlapped.hEvent,2000) )
				{
					GetOverlappedResult(hDevice,&tOverlapped,&dwBytesRead,FALSE);
				}
				else
				{
					CancelIo(hDevice);
				}
			}
		}

		CloseHandle(tOverlapped.hEvent);
		CloseHandle(hDevice);
	}


	return 0;
}


void AH3KDev::Close(void)
{
	RemoveAH3KDev(m_nPhyicalDevID,this);

	m_bRunning=false;
	WaitForStop();

	if(m_hDevice!=INVALID_HANDLE_VALUE)
	{
		CancelIo(m_hDevice);
		CloseHandle(m_hDevice);
		m_hDevice=INVALID_HANDLE_VALUE;
	}
	
	if (m_Overlapped.hEvent != NULL)
	{
		CloseHandle(m_Overlapped.hEvent);
		m_Overlapped.hEvent = NULL;
	}

	if (m_pBuffer)
	{
		free(m_pBuffer);
		m_pBuffer=NULL;
	}
}

void AH3KDev::OnCapturedAudio(short*pSamples,int nSamples,int nSampleRate)
{
	if (m_audio_cb)
	{
		m_audio_cb(m_pUserData,pSamples,nSamples,nSampleRate,timeGetTime());
	}
}


void AH3KDev::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		DWORD dwBytesRead=0;
		if (m_pBuffer==NULL)
		{
			m_pBuffer=(unsigned char*)malloc(m_nBufLen);
			if (m_pBuffer==NULL)
			{
				return;
			}
		}
		if (!ReadFile(m_hDevice,m_pBuffer,m_nBufLen,&dwBytesRead,&m_Overlapped))
		{
			if (ERROR_IO_PENDING==GetLastError())
			{
				if (WAIT_OBJECT_0==WaitForSingleObject(m_Overlapped.hEvent,2000) )
				{
					GetOverlappedResult(m_hDevice,&m_Overlapped,&dwBytesRead,FALSE);
				}
				else
				{
					CancelIo(m_hDevice);
				}
			}
		}

		if(dwBytesRead>=sizeof(DAT_PACKET ) )
		{
			PDAT_PACKET pHead=(PDAT_PACKET)m_pBuffer;

			if(pHead->Base.byte.frame_type==FRAME_NULL) //无视频信号
			{
				
			}
			else if(pHead->Base.byte.frame_type==FRAME_VIDEO)
			{
				int nWidth=pHead->nOutputWidth;		
				int nHeight=pHead->nOutputHeight;
				int nFrameRate=pHead->Base.byte.framerate;
				DWORD dwHeaderLen=sizeof(DAT_PACKET);
				unsigned char*pFrameData=m_pBuffer+dwHeaderLen;
				int nFrameLen=dwBytesRead-dwHeaderLen;

				if (m_video_cb)
				{
					if (NULL==m_video_cb(m_pUserData,pFrameData,nFrameLen,nWidth,nHeight,nFrameRate,timeGetTime(),dwHeaderLen))
					{
						m_pBuffer=NULL;
					}
				}
			}
		}
		else
		{
			Sleep(1);
		}

	}
}


DWORD AH3K_GetPhyiscalDeviceCount(void)
{
	DWORD dwCount=0;
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&g_guidAH3K, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
		return 0;
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	DWORD nDevIndex=0;
	for (nDevIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &g_guidAH3K, nDevIndex, &ifdata); ++nDevIndex)
	{
		dwCount++;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return dwCount;
}

BOOL AH3K_GetPhyicalDeviceName(DWORD nDevID,std::string&strDevName,std::string&strDevPath)
{
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&g_guidAH3K, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
		return FALSE;
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	BOOL bRet=SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &g_guidAH3K, nDevID, &ifdata);
	if(bRet)
	{		
		DWORD nNeeded=0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifdata, NULL, 0, &nNeeded, NULL);	
		PSP_DEVICE_INTERFACE_DETAIL_DATA pDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(nNeeded);
		pDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);

		SP_DEVINFO_DATA cInfoData;
		cInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifdata, pDetailData, nNeeded, NULL, &cInfoData);

		TCHAR szDevName[256];									
		if (!SetupDiGetDeviceRegistryProperty(hDevInfo,
			&cInfoData,
			SPDRP_FRIENDLYNAME,
			NULL,
			(PBYTE) szDevName,
			sizeof(szDevName),
			NULL)
			&& !SetupDiGetDeviceRegistryProperty(hDevInfo,
			&cInfoData,
			SPDRP_DEVICEDESC,
			NULL,
			(PBYTE) szDevName,
			sizeof(szDevName),
			NULL)
			)
		{
			bRet=FALSE;
		}
		else
		{
			strDevName=szDevName;
			strDevPath=pDetailData->DevicePath;
		}

		free(pDetailData);
		pDetailData=NULL;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}

DWORD AH3K_GetDevChanCount(DWORD nDevID,const std::string&strDevName,const std::string&strDevPath)
{
	DWORD dwCount=0;
	HANDLE hDevice=CreateFile(strDevPath.c_str(),
		GENERIC_READ | GENERIC_WRITE, 
		0,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if(INVALID_HANDLE_VALUE!=hDevice)
	{
		TCHAR szChanPath[MAX_PATH];
		DWORD nChanID=0;

		while(1)
		{
			wsprintf(szChanPath,_T("%s\\%.2u"),strDevPath.c_str(),nChanID);

			HANDLE hChannel=CreateFile(szChanPath,
				GENERIC_READ | GENERIC_WRITE, 
				0,
				NULL,
				OPEN_EXISTING,
				NULL,
				NULL);
			if(INVALID_HANDLE_VALUE!=hChannel)
			{
				DWORD dwType=0;
				DWORD dwTemp=0;;
				DeviceIoControl(hChannel,IOCTL_AH3K_PORTTYPE_GET,
					&nChanID,sizeof(nChanID),
					&dwType, sizeof(dwType), &dwTemp, NULL);

				if (TYPE_AUDIO==dwType)
				{
					g_aDevAudioChan[g_nDevAudioChanCount].nDevID=nDevID;
					g_aDevAudioChan[g_nDevAudioChanCount].strDevName=strDevName;
					g_aDevAudioChan[g_nDevAudioChanCount].strDevPath=strDevPath;
					g_aDevAudioChan[g_nDevAudioChanCount].nChanID=nChanID;
					g_aDevVideoChan[g_nDevVideoChanCount].nChanType=dwType;
					g_aDevAudioChan[g_nDevAudioChanCount].strChanName=strDevName;
					g_aDevAudioChan[g_nDevAudioChanCount].strChanPath=szChanPath;
					++g_nDevAudioChanCount;
				}
				else
				{
					std::string strChanName=strDevName;
					if (TYPE_VIDEO_CVBS==dwType)
					{
						strChanName+=" SD";
					}
					else if (TYPE_VIDEO_DSUB==dwType)
					{
						strChanName+=" VGA";
					}
					else
					{
						strChanName+=" HD";
					}
					g_aDevVideoChan[g_nDevVideoChanCount].nDevID=nDevID;
					g_aDevVideoChan[g_nDevVideoChanCount].strDevName=strDevName;
					g_aDevVideoChan[g_nDevVideoChanCount].strDevPath=strDevPath;
					g_aDevVideoChan[g_nDevVideoChanCount].nChanID=nChanID;
					g_aDevVideoChan[g_nDevVideoChanCount].nChanType=dwType;
					g_aDevVideoChan[g_nDevVideoChanCount].strChanName=strChanName;
					g_aDevVideoChan[g_nDevVideoChanCount].strChanPath=szChanPath;
					++g_nDevVideoChanCount;
				}

				nChanID++;
				CloseHandle(hChannel);
			}
			else
			{
				dwCount=nChanID;
				break;
			}
		}

		CloseHandle(hDevice);
	}

	return dwCount;
}

void AH3K_Initialize(void)
{
	if (g_bAH3KInitialied)
		return;
	g_bAH3KInitialied=true;

	DWORD nDevCount=AH3K_GetPhyiscalDeviceCount();
	for (DWORD nDevID=0;nDevID<nDevCount;++nDevID)
	{
		std::string strDevName;
		std::string strDevPath;
		if (!AH3K_GetPhyicalDeviceName(nDevID,strDevName,strDevPath))
		{
			break;
		}

		DWORD nChanCount=AH3K_GetDevChanCount(nDevID,strDevName,strDevPath);
	}
}

int AH3K_GetDevCount(void)
{
	AH3K_Initialize();
	return g_nDevVideoChanCount;
}

int AH3K_GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0 || nDevID>=g_nDevVideoChanCount)
	{
		return -1;
	}

	std::string strName=g_aDevVideoChan[nDevID].strChanName;
	int size = strlen(strName.c_str()) + 1;

	if (szName != NULL)
	{
		if (nMaxCount>=size)
		{
			strcpy(szName,strName.c_str());
		}
		else
		{
			strncpy(szName,strName.c_str(),nMaxCount);
		}
	}

	return size;
}

int AH3K_HasAudio(int nDevID)
{
	if (nDevID<0 || nDevID>=g_nDevVideoChanCount)
	{
		return 0;
	}

	nDevID=g_aDevVideoChan[nDevID].nDevID;
	for (int i=0;i<g_nDevAudioChanCount;++i)
	{
		if (g_aDevAudioChan[i].nDevID==nDevID)
		{
			return 1;
		}
	}
	return 0;
}

int AH3K_IsHD(int nDevID)
{
	if (nDevID<0 || nDevID>=g_nDevVideoChanCount)
	{
		return 0;
	}

	if (g_aDevVideoChan[nDevID].nChanType!=TYPE_VIDEO_CVBS)
	{
		return 1;
	}

	return 0;
}


void*AH3K_Open(int nDevID,AH3K_VIDEO_CB video_cb,AH3K_AUDIO_CB audio_cb,const void*pUserData)
{
	AH3KDev*pAH3KDev=new AH3KDev();
	if (0!=pAH3KDev->Open(nDevID,video_cb,audio_cb,pUserData))
	{
		pAH3KDev->Close();
		delete pAH3KDev;
		pAH3KDev=NULL;
	}
	return pAH3KDev;
}

void AH3K_Close(void*pHandle)
{
	AH3KDev*pAH3KDev=(AH3KDev*)pHandle;
	if (pAH3KDev)
	{
		pAH3KDev->Close();
		delete pAH3KDev;
		pAH3KDev=NULL;
	}
}

int AH3K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate)
{
	int nRet=-1;
	AH3KDev*pAH3KDev=(AH3KDev*)pHandle;
	if (pAH3KDev)
	{
		nRet=pAH3KDev->SetParams(nScaleType,nWidth,nHeight,nFrameRate);
	}

	return nRet;
}
