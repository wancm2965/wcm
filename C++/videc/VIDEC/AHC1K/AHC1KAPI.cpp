// AHC1KAPI.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include <windows.h>
#include <tchar.h>

#include "AHC1KAPI.h"
#include "LoadHex.h"
#include "resource.h"

#include <winioctl.h>

#include <setupapi.h>
#pragma comment(lib,"setupapi.lib")
#include "ah4k.h"



typedef struct tagAHC1K_DEV
{
	int				nDevID;
	AHC1K_VIDEO_CB	video_cb;
	AHC1K_STREAM_CB stream_cb;

	const void*		pUserData;


	HANDLE			hThread;
	HANDLE			hEvtExit;
	DWORD			dwThreadId;

}AHC1K_DEV,*PAHC1K_DEV;



typedef struct _FRAME_HEAD
{
	DWORD Version;
	DWORD Resolution;
	DWORD TypeRateInterlaceInterface;
	DWORD Reserved;
}FRAME_HEAD,*PFRAME_HEAD;


AHC1K_MALLOC AHC1K_Malloc=malloc;
AHC1K_FREE	AHC1K_Free=free;

void AHC1K_SetMemory(AHC1K_MALLOC Malloc,AHC1K_FREE Free)
{
	AHC1K_Malloc=Malloc;
	AHC1K_Free=Free;
}

static bool g_bAHC1K_Initialized=false;
static int g_nAHC1K_DevCount=0;

extern HINSTANCE g_hInstance;


int AHC1K_BootLoad(int nDevID,unsigned char* pFileData,unsigned long nSize);
////////////////////////////////////////////////////////////////////
//interface.
//
void AHC1K_Initialize(void)
{
	if (!g_bAHC1K_Initialized)
	{
		g_bAHC1K_Initialized=true;

		PBYTE pBufAHC1K = NULL;
		DWORD nBufLenAHC1K= 0;

		if (LoadHex(g_hInstance, IDR_HEX, &pBufAHC1K, nBufLenAHC1K))
		{
			int nDevID=0;
			do
			{
				if (0!=AHC1K_BootLoad(nDevID,pBufAHC1K,nBufLenAHC1K))
				{
					break;
				}
				nDevID++;
			}while(true);

			g_nAHC1K_DevCount=nDevID;
		}
	}
}

int AHC1K_GetDevCount(void)
{
	AHC1K_Initialize();
	return g_nAHC1K_DevCount;
}

int AHC1K_GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0 || nDevID >= g_nAHC1K_DevCount)
		return -1;

	if (nMaxCount<=0)
		return -1;

	char szTemp[128];
	size_t size;
	sprintf(szTemp, "AHC1K Dev %d",nDevID);
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



static BOOL AHC1K_GetPath(DWORD nDevID,LPTSTR lpszPath,int nMaxCount)
{
	HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVICE_AH4K, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	SP_INTERFACE_DEVICE_DATA ifdata;
	ifdata.cbSize = sizeof(ifdata);
	BOOL bRet=SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVICE_AH4K, nDevID, &ifdata);
	if(bRet)
	{		
		DWORD nNeeded=0;
		SetupDiGetDeviceInterfaceDetail(hDevInfo, &ifdata, NULL, 0, &nNeeded, NULL);	
		PSP_DEVICE_INTERFACE_DETAIL_DATA pDetailData=(PSP_DEVICE_INTERFACE_DETAIL_DATA)new BYTE[nNeeded];
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
			lstrcpy(lpszPath,pDetailData->DevicePath);
		}

		delete pDetailData;
		pDetailData=NULL;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}

static DWORD WINAPI ThreadProc(PVOID pArg)
{
	PAHC1K_DEV pAH1KDev=(PAHC1K_DEV)pArg;

	TCHAR szPath[2][MAX_PATH]={_T(""),_T("")};

	HANDLE hDevice[2]={INVALID_HANDLE_VALUE,INVALID_HANDLE_VALUE};
	HANDLE hEvent[2]={NULL,NULL};
	OVERLAPPED tOverlapped[2];
	BYTE* pBuffer[2]={NULL,NULL};
	DWORD dwBufSize[2]={0x400000,0x100000};//4M,1M


	__try
	{

		pBuffer[0]=new BYTE[dwBufSize[0]];
		if(pBuffer[0]==NULL)
			__leave;
		pBuffer[1]=new BYTE[dwBufSize[1]];
		if(pBuffer[1]==NULL)
			__leave;

		if(!AHC1K_GetPath(pAH1KDev->nDevID,szPath[0],MAX_PATH-1))	
			__leave;
		if(!AHC1K_GetPath(pAH1KDev->nDevID,szPath[1],MAX_PATH-1))
			__leave;


		hEvent[0]=CreateEvent(0,0,0,0);
		if(hEvent[0]==NULL)
			__leave;		
		hEvent[1]=CreateEvent(0,0,0,0);
		if(hEvent[1]==NULL)		
			__leave;
		

		
		lstrcat(szPath[0],_T("\\00"));
		hDevice[0]=CreateFile(szPath[0],
				GENERIC_READ | GENERIC_WRITE, 
				0,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,
				NULL);
		if(INVALID_HANDLE_VALUE==hDevice[0])
		{
			__leave;			
		}	
		lstrcat(szPath[1],_T("\\01"));
		hDevice[1]=CreateFile(szPath[1],
				GENERIC_READ | GENERIC_WRITE, 
				0,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_OVERLAPPED,
				NULL);
		if(INVALID_HANDLE_VALUE==hDevice[1])
		{			
			__leave;
		}


		ZeroMemory(tOverlapped,sizeof(tOverlapped) );

		tOverlapped[0].hEvent=hEvent[0];
		tOverlapped[1].hEvent=hEvent[1];

		ReadFile(hDevice[0],pBuffer[0],dwBufSize[0],NULL,&tOverlapped[0]);
		ReadFile(hDevice[1],pBuffer[1],dwBufSize[1],NULL,&tOverlapped[1]);



		HANDLE hEventArr[3]={pAH1KDev->hEvtExit,hEvent[0],hEvent[1]};
		DWORD dwError;
		DWORD dwWaitRet;
		BOOL bResult[2];
		DWORD nBytes[2];
		int ix;
		while(1)
		{
			
			
			dwError = GetLastError();

			//dwError=ERROR_IO_PENDING;

			if(ERROR_IO_PENDING==dwError)
			{
				dwWaitRet=WaitForMultipleObjects(3,hEventArr,FALSE,INFINITE);

				switch(dwWaitRet)
				{
				case WAIT_OBJECT_0:	//Exit event
					CancelIo(hDevice[0]);
					CancelIo(hDevice[1]);

					__leave;

					break;
				case WAIT_OBJECT_0+1:
				case WAIT_OBJECT_0+2:

					ix=dwWaitRet-1;

					bResult[ix] = GetOverlappedResult(hDevice[ix],
													  &tOverlapped[ix],
													  &nBytes[ix],
													  TRUE);

					if(bResult[ix])
					{
						
						if(ix==0 && pAH1KDev->video_cb)
						{
							PFRAME_HEAD pHead=(PFRAME_HEAD)(pBuffer[ix]+256-sizeof(FRAME_HEAD));
							//¼ì²éversion
							if(pHead->Version!=0xaa550001)
							{
								break;
							}


							WORD cx,cy;

							cx=LOWORD(pHead->Resolution);
							cy=HIWORD(pHead->Resolution);

							OutputDebugStringA("video_cb\n");							
							//pAH1KDev->video_cb(pAH1KDev->pUserData,pBuffer[ix]+256,nBytes[ix],cx,cy,30,0,0);
						}
						else if(ix==1 && pAH1KDev->stream_cb)
						{
							OutputDebugStringA("stream_cb\n");
							//pAH1KDev->stream_cb(pAH1KDev->pUserData,pBuffer[ix],nBytes[ix],1920,1080,30,0);
						}
						else
						{
							//ASSERT(0);
						}

						ReadFile(hDevice[ix],pBuffer[ix],dwBufSize[ix],NULL,&tOverlapped[ix]);
					}
					else
					{
						OutputDebugStringA("GetOverlappedResult failure\n");
			
					}


					break;
				default:
					//ASSERT(0)
					break;
				}
			}/*ERROR_IO_PENDING==dwError*/

		}
			
		
	}
	__finally
	{
		if(hDevice[0]!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(hDevice[0]);
		}

		if(hDevice[1]!=INVALID_HANDLE_VALUE)
		{
			CloseHandle(hDevice[1]);
		}

		if(hEvent[0]!=NULL)
		{
			CloseHandle(hEvent[0]);			
		}
		if(hEvent[1]!=NULL)
		{
			CloseHandle(hEvent[1]);			
		}

		delete pBuffer[0];
		delete pBuffer[1];

	}
	return 0;
}

void*AHC1K_Open(int nDevID,AHC1K_VIDEO_CB video_cb,AHC1K_STREAM_CB stream_cb,const void*pUserData)
{
	PAHC1K_DEV pAH1KDev=new AHC1K_DEV;
	if(pAH1KDev)
	{
		
		pAH1KDev->nDevID=nDevID;
		pAH1KDev->video_cb=video_cb;
		pAH1KDev->stream_cb=stream_cb;
		pAH1KDev->pUserData=pUserData;

		pAH1KDev->hEvtExit=CreateEvent(0,0,0,0);
		if(pAH1KDev->hEvtExit==NULL)
		{
			return NULL;
		}
		pAH1KDev->hThread=CreateThread(NULL,0,ThreadProc,pAH1KDev,0,&pAH1KDev->dwThreadId);			
             
		if(pAH1KDev->hThread==NULL)
		{
			CloseHandle(pAH1KDev->hEvtExit);
			return NULL;
		}
		
	}

	return pAH1KDev;
}


void AHC1K_Close(void*pHandle)
{
	PAHC1K_DEV pAH1KDev=(PAHC1K_DEV)pHandle;
	if(pAH1KDev)
	{
		SetEvent(pAH1KDev->hEvtExit);

		if(WAIT_OBJECT_0!=::WaitForSingleObject(pAH1KDev->hThread,5*1000) )
		{
			TerminateThread(pAH1KDev->hThread,7);		
		}

		//
		CloseHandle(pAH1KDev->hEvtExit);
		CloseHandle(pAH1KDev->hThread);

		delete pAH1KDev;
		pAH1KDev=NULL;
	}
}


int AHC1K_SetParams(void*pHandle,int nScaleType,int nWidth,int nHeight,int nFrameRate,
							  int nBitrate,int nRateControlType)
{


	PAHC1K_DEV pAH1KDev=(PAHC1K_DEV)pHandle;

	AH4K_MSG msg;


	msg.id=0xAA000000; 
	msg.data[0]=MAKELONG(0,0);	
	msg.data[1]=MAKELONG(MAKEWORD(0,0),MAKEWORD(0,0));		
	msg.data[2]=MAKELONG(nWidth,nHeight);	
	msg.data[3]=MAKELONG(MAKEWORD(0,0),MAKEWORD(0,nFrameRate));	
	msg.data[4]=MAKELONG(nWidth,nHeight);	
	msg.data[5]=MAKELONG(MAKEWORD(0,0),MAKEWORD(0,1));	
	msg.data[6]=MAKELONG(MAKEWORD(1,25),MAKEWORD(2,nRateControlType));
	msg.data[7]=MAKELONG(nBitrate,30);	


	TCHAR szPath[MAX_PATH];
	HANDLE hDevice;
	if(AHC1K_GetPath(pAH1KDev->nDevID,szPath,MAX_PATH-1))	
	{
		hDevice=CreateFile(szPath,
						GENERIC_READ | GENERIC_WRITE, 
						0,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED,
						NULL);
		if(INVALID_HANDLE_VALUE==hDevice)
		{
			return -2;
		}	

		OVERLAPPED tOverlapped;
		ZeroMemory(&tOverlapped,sizeof(tOverlapped));
		tOverlapped.hEvent=CreateEvent(0,0,0,0);

		DWORD nBytes=0;

		if(DeviceIoControl(hDevice,IOCTL_AH4K_MSG,&msg,sizeof(msg),&msg,sizeof(msg),&nBytes,&tOverlapped) )
		{
			CloseHandle(tOverlapped.hEvent);
			CloseHandle(hDevice);
			return 0;
		}
		else
		{
			if (ERROR_IO_PENDING==GetLastError())
			{			
				if (WAIT_OBJECT_0==WaitForSingleObject(tOverlapped.hEvent,500) )
				{
					if (GetOverlappedResult(hDevice,&tOverlapped,&nBytes,TRUE))
					{
						CloseHandle(tOverlapped.hEvent);
						CloseHandle(hDevice);
						return 0;//success
					}
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

	return -1;
}


int AHC1K_BootLoad(int nDevID,unsigned char* pFileData,unsigned long nSize)
{
	TCHAR szPath[MAX_PATH];
	HANDLE hDevice;
	if(AHC1K_GetPath(nDevID,szPath,MAX_PATH-1))	
	{
		hDevice=CreateFile(szPath,
						GENERIC_READ | GENERIC_WRITE, 
						0,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED,
						NULL);
		if(INVALID_HANDLE_VALUE==hDevice)
		{
			return -2;
		}	

		OVERLAPPED tOverlapped;
		ZeroMemory(&tOverlapped,sizeof(tOverlapped));
		tOverlapped.hEvent=CreateEvent(0,0,0,0);

		DWORD nBytes;

		if(DeviceIoControl(hDevice,IOCTL_AH4K_LOAD,pFileData,nSize,0,0,&nBytes,&tOverlapped) )
		{
			CloseHandle(tOverlapped.hEvent);
			CloseHandle(hDevice);
			return 0;
		}
		else
		{
			if(ERROR_IO_PENDING==GetLastError())
			{			
				if(WAIT_OBJECT_0==WaitForSingleObject(tOverlapped.hEvent,500) )
				{
					if(GetOverlappedResult(hDevice,&tOverlapped,&nBytes,TRUE)  )
					{
						CloseHandle(tOverlapped.hEvent);
						CloseHandle(hDevice);
						return 0;//success
					}
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

	return -1;
}
