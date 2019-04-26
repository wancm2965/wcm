#include "SDDevCamera.h"
#include "XUtil.h"


SDDevCamera::SDDevCamera(unsigned int nDevIndex)
:SDDev(nDevIndex)

,m_pCamera(NULL)
,m_pIXPreProcess(NULL)

,m_ulLastImageTimestamp(0)
{

}

SDDevCamera::~SDDevCamera(void)
{

}

int SDDevCamera::Open(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;
	m_bAutoFrameRate=bAutoFrameRate;

	m_pIXPreProcess=CreateIXPreProcess(*this);

	m_pCamera=VIDEC_Camera::Create(*this);
	if (m_pCamera!=NULL)
	{
		if (0!=m_pCamera->Open(m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,m_bAutoFrameRate))
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}
	}

	if (m_pCamera==NULL)
	{
		return -1;
	}

	return 0;
}


void SDDevCamera::Close(void)
{
	if (m_pCamera!=NULL)
	{
		m_pCamera->Close();
		delete m_pCamera;
		m_pCamera=NULL;
	}

	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->ReleaseConnections();
		delete m_pIXPreProcess;
		m_pIXPreProcess=NULL;
	}
}

	
int SDDevCamera::SetDevCap(int nWidth,int nHeight,int nFrameRate,bool bAutoFrameRate)
{
	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_nFrameRate!=nFrameRate ||
		m_bAutoFrameRate!=bAutoFrameRate)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameRate=nFrameRate;
		m_bAutoFrameRate=bAutoFrameRate;

		if (m_pCamera!=NULL)
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}

		m_pCamera=VIDEC_Camera::Create(*this);
		if (m_pCamera!=NULL)
		{
			if (0!=m_pCamera->Open(m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,m_bAutoFrameRate))
			{
				m_pCamera->Close();
				delete m_pCamera;
				m_pCamera=NULL;
			}
		}

		if (m_pCamera==NULL)
		{
			return -1;
		}		
	}

	return 0;
}


void SDDevCamera::OnVIDEC_CameraCallbackImageData(unsigned char*pData,int nLen,int nWidth,int nHeight)
{
	m_ulLastImageTimestamp=XGetTimestamp();

	if (m_pIXPreProcess!=NULL)
	{
		//m_pIXPreProcess->EnableDeinterlace(bDeinterlaced);
		m_pIXPreProcess->PreProcess(pData,nLen,nWidth,nHeight);
	}
	else
	{
		OnIXPreProcessNotifyOutputData(pData,nLen,nWidth,nHeight);
	}
}


void SDDevCamera::OnIXPreProcessNotifyOutputData(unsigned char*pData,int nLen,int nWidth,int nHeight)
{
	XOutputImageMgr::OutputImage(pData,nLen,nWidth,nHeight);
	XOutputStreamMgr::OutputImage(pData,nLen,nWidth,nHeight);
}


