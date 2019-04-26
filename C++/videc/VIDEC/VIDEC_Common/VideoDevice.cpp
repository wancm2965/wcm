#include "VideoDevice.h"
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include "../VIDEC_API/XImage.h"

VideoDevice::VideoDevice(VIDEC_DeviceCallback&rCallback)
:m_rCallback(rCallback)
,m_pCapDev(NULL)
{
	m_nDevID=-1;
	m_nWidth=704;
	m_nHeight=576;
	m_nFrameRate=30;
	m_nFrameType=VIDEO_FRAME_TYPE_YUV420;
}

VideoDevice::~VideoDevice(void)
{

}


int VideoDevice::Open(int nDevID,int nWidth,int nHeight,int nFrameRate,int nFrameType)
{
	if (nDevID<0)
	{
		Close();
		return -1;
	}

	if (m_nDevID!=nDevID ||
		m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_nFrameRate!=nFrameRate ||
		m_nFrameType!=nFrameType ||
		m_pCapDev==NULL)
	{
		m_nDevID=nDevID;
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameRate=nFrameRate;
		m_nFrameType=nFrameType;
	}
	else
	{
		return 0;
	}

	VIDEC_CapDev*pOldCapDev=m_pCapDev;

	int nAH2KDevID=-1;
	while (true)
	{
		int nDevCount=0;
		nDevCount=VIDEC_AH2KDev::GetDevCount();
		if (nDevID<nDevCount)
		{
			nAH2KDevID=nDevID;

			VIDEC_AH2KDev*pCapDev=NULL;
			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_AH2KDev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_AH2KDev::Create();
				if (pCapDev==NULL)
				{
					Close();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					Close();
					return -1;
				}
				m_pCapDev=pCapDev;
				break;
			}

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				Close();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		nDevCount=VIDEC_CameraDev::GetDevCount();
		if (nDevID<nDevCount)
		{
			VIDEC_CameraDev*pCapDev=NULL;
			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_CameraDev::Create();
				if (pCapDev==NULL)
				{
					Close();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					Close();
					return -1;
				}
				m_pCapDev=pCapDev;
				break;
			}

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				Close();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		nDevCount=VIDEC_AH400Dev::GetDevCount();
		if (nDevID<nDevCount)
		{
			VIDEC_AH400Dev*pCapDev=NULL;

			if (m_pCapDev!=NULL)
			{
				pCapDev=dynamic_cast<VIDEC_AH400Dev*>(m_pCapDev);
				if (pCapDev==NULL)
				{
					m_pCapDev->Close();
					delete m_pCapDev;
					m_pCapDev=NULL;

					pOldCapDev=NULL;
				}
			}

			if (m_pCapDev==NULL)
			{
				pCapDev=VIDEC_AH400Dev::Create();
				if (pCapDev==NULL)
				{
					Close();
					return -1;
				}
				if (0!=pCapDev->Open(nDevID,nWidth,nHeight,nFrameRate))
				{
					pCapDev->Close();
					delete pCapDev;
					pCapDev=NULL;

					Close();
					return -1;
				}

				m_pCapDev=pCapDev;
				break;
			}

			if (0!=pCapDev->Reset(nDevID,nWidth,nHeight,nFrameRate))
			{
				Close();
				return -1;
			}
			break;
		}
		nDevID-=nDevCount;

		Close();
		return -1;
	}

	if (m_pCapDev==NULL)
	{
		Close();
		return -1;
	}

	if (m_pCapDev!=pOldCapDev)
	{
		m_pCapDev->AddImageSink(this);
	}

	m_pCapDev->SetDenoise(0);

	return 0;
}

void VideoDevice::Close(void)
{
	m_nDevID=-1;

	if (m_pCapDev!=NULL)
	{
		m_pCapDev->Close();
		delete m_pCapDev;
		m_pCapDev=NULL;
	}
}

void VideoDevice::ShowInputWnd(HWND hParentWnd)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCapDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCapDev)
		{
			pCapDev->ShowVideoInputWnd(hParentWnd);
		}
	}
}

void VideoDevice::ShowPropertyWnd(HWND hParentWnd)
{
	if (m_pCapDev)
	{
		VIDEC_CameraDev*pCapDev=dynamic_cast<VIDEC_CameraDev*>(m_pCapDev);
		if (pCapDev)
		{
			pCapDev->ShowAdjustVideoWnd(hParentWnd);
		}
	}
}
void VideoDevice::OnReceivedPreSend(int& nSend)
{
	nSend=1;
}
void VideoDevice::OnReceivedImage(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	m_rCallback.OnVIDEC_DeviceCallbackFrameData(pXImage->GetData(),pXImage->GetDataLen(),pXImage->GetActualWidth(),pXImage->GetActualHeight(),16,VIDEO_FRAME_TYPE_YUV420);
}

int VIDEC_Device::Initialize(void)
{
	return VIDEC_Init();
}

void VIDEC_Device::UnInitialize(void)
{
	VIDEC_Terminate();
}

int VIDEC_Device::GetDevCount(void)
{
	int nDevCount=0;
	nDevCount+=VIDEC_AH2KDev::GetDevCount();
	nDevCount+=VIDEC_CameraDev::GetDevCount();
	nDevCount+=VIDEC_AH400Dev::GetDevCount();
	return nDevCount;
}

int VIDEC_Device::GetDevName(int nDevID,char*szName,int nMaxCount)
{
	if (nDevID<0)
		return -1;

	int nDevCount=0;
	nDevCount=VIDEC_AH2KDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_AH2KDev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_CameraDev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_CameraDev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	nDevCount=VIDEC_AH400Dev::GetDevCount();
	if (nDevID<nDevCount)
	{
		return VIDEC_AH400Dev::GetDevName(nDevID,szName,nMaxCount);
	}
	nDevID-=nDevCount;

	return -1;
}

VIDEC_Device*VIDEC_Device::Create(VIDEC_DeviceCallback&rCallback)
{
	//return new VideoDevice(rCallback);
	NEWRECORDret3(  VIDEC_Device* ,p , new VideoDevice(rCallback)  )  
}
