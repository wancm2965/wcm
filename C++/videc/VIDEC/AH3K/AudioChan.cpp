#include "stdafx.h"
#include "AudioChan.h"
#include "AH3KDev.h"


AudioChan::AudioChan(int nPhyicalDevID)
:m_hDevice(INVALID_HANDLE_VALUE)
,m_bRunning(false)
,m_nPhyicalDevID(nPhyicalDevID)
{
	ZeroMemory(&m_Overlapped,sizeof(m_Overlapped));
	m_Overlapped.hEvent = NULL;
}

AudioChan::~AudioChan(void)
{

}

int AudioChan::Open(const char*cszChanPath)
{
	m_hDevice=CreateFile(cszChanPath,
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

	return 0;
}

void AudioChan::Close(void)
{
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
}

void AudioChan::AddAH3KDev(AH3KDev*pAH3KDev)
{
	XAutoLock l(m_csMapAH3KDev);
	m_MapAH3KDev[pAH3KDev]=pAH3KDev;
}

void AudioChan::RemoveAH3KDev(AH3KDev*pAH3KDev)
{
	XAutoLock l(m_csMapAH3KDev);
	m_MapAH3KDev.erase(pAH3KDev);
}

int AudioChan::GetAH3KDevCount(void)
{
	XAutoLock l(m_csMapAH3KDev);
	return m_MapAH3KDev.size();
}

void AudioChan::ThreadProcMain(void)
{
	DWORD dwBufLen=1024*1024*4;
	unsigned char*pBuffer=(unsigned char*)malloc(dwBufLen);
	if (!pBuffer)
		return ;

	while (m_bRunning)
	{
		DWORD dwBytesRead=0;
		if (!ReadFile(m_hDevice,pBuffer,dwBufLen,&dwBytesRead,&m_Overlapped))
		{
			if (ERROR_IO_PENDING==GetLastError())
			{
				if (WAIT_OBJECT_0==WaitForSingleObject(m_Overlapped.hEvent,2000) )
				{
					if (!GetOverlappedResult(m_hDevice,&m_Overlapped,&dwBytesRead,TRUE))
					{
						dwBytesRead=0;
					}
				}
				else
				{
					CancelIo(m_hDevice);
				}
			}
		}

		if(dwBytesRead>sizeof(DAT_PACKET))
		{
			{
				DWORD dwHeaderLen=sizeof(DAT_PACKET);
				short*pSamples=(short*)(pBuffer+dwHeaderLen);
				int nSamples=(dwBytesRead-dwHeaderLen)>>1;
				int nSampleRate=16000;

				if (nSamples==1280)
				{
					XAutoLock l(m_csMapAH3KDev);
					MapAH3KDev::iterator iter;
					for (iter=m_MapAH3KDev.begin();iter!=m_MapAH3KDev.end();++iter)
					{
						AH3KDev*pAH3KDev=iter->second;
						pAH3KDev->OnCapturedAudio(pSamples,nSamples,nSampleRate);
					}
				}

			}
		}
		else
		{
			Sleep(1);
		}
	}

	free(pBuffer);
	pBuffer=NULL;
}