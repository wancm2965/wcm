#include "XCameraDev.h"
#include "../VIDEC_API/XImage.h"
#include "XUtil.h"
#include <string>

extern bool GetOnlyUSBCamera(void);

XCameraDev::XCameraDev(unsigned int nDevIndex)
:XImageSource(this)
,m_nDevIndex(nDevIndex)

,m_nWidth(0)
,m_nHeight(0)
,m_nFrameRate(15)

,m_bDefaultFormat(true)

,m_pCamera(NULL)
,m_pIXPreProcess(NULL)

,m_isUSBCamera(false)
,m_bAS120(false)
{
	char szDevName[1024]="";
	VIDEC_Camera::GetDeviceName(m_nDevIndex,szDevName);

	char*p=szDevName;
	for( p = szDevName; p < szDevName + strlen( szDevName ); p++ )
	{
		if (*p>='A' && *p<='Z')
			*p=_tolower( *p );
	}

	if (strstr(szDevName,"logitech"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"camera"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"usb"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"quickcam"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"uvc"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"web"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"cam"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"smi"))
	{
		m_isUSBCamera=true;
	}
	else if (strstr(szDevName,"analog"))
	{
		m_isUSBCamera=true;
	}

	std::string strDevName=szDevName;
	if (strDevName=="usb media capture device")
	{
		m_isUSBCamera=false;
	}

	if (strDevName=="microsoft lifecam studio")
	{
		m_isUSBCamera=false;
		m_bLifeCamera=true;
	}
	else
	{
		m_bLifeCamera=false;
	}

	if (strDevName=="713x bda analog capture")
	{
		m_bAS120=true;
	}
	else
	{
		m_bAS120=false;
	}

	m_bRBSCamera =false;
	if (strDevName=="v-camera")
	{
		m_bRBSCamera=true;
	}
	else if (strDevName.find("v-camera"))
	{
		m_bRBSCamera=true;
	}
	else
	{
		m_bRBSCamera=false;
	}


	//m_isUSBCamera=false;
	m_nVirtualWidth=m_nWidth;
	m_nVirtualHeight=m_nHeight;

	m_pConvertUV =NULL;
	//m_pConvertUV=new char[1920*1200 /4];

}

XCameraDev::~XCameraDev(void)
{
	if(m_pConvertUV)
	{
	
		delete [] m_pConvertUV;
		m_pConvertUV =NULL;
	}
}

int XCameraDev::Open(int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat)
{
	int nTryMaxSize=0;

	if (m_bAS120)
	{
		if (nWidth==0 && nHeight==0)
		{
			nWidth=704;
			nHeight=576;
		}
		else if (nWidth>=640)
		{
			nWidth=704;
			nHeight=576;
		}
		else if (nWidth>=320)
		{
			nWidth=352;
			nHeight=288;
		}
		else
		{
			nWidth=176;
			nHeight=144;
		}
	}

	if (nWidth==0 && nHeight==0)
	{
		if (m_isUSBCamera)
		{
			nWidth=640;
			nHeight=480;
			nTryMaxSize=1;
		}
		else
		{
			nWidth=704;
			nHeight=576;
		}
	}

	if (GetOnlyUSBCamera())
	{
		nTryMaxSize=0;

		if (m_isUSBCamera || m_bAS120)
		{
			if (nWidth>704 || nHeight>576)
			{
				nWidth=704;
				nHeight=576;
			}
		}
		else
		{
			nWidth=176;
			nHeight=144;
		}
	}

	m_nVirtualWidth=nWidth;
	m_nVirtualHeight=nHeight;

	if (m_bLifeCamera)
	{
		if (nWidth==960)
		{
			m_nVirtualWidth=960;
			m_nVirtualHeight=540;

			nWidth=960;
			nHeight=544;
		}
	}

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;
	m_bDefaultFormat=bDefaultFormat;

	int nTempWidth=nWidth;
	int nTempHeight=nHeight;

	m_pIXPreProcess=CreateIXPreProcess(*this);
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),VIDEC_GetDefaultSharpen());
	}

	m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?0:1);
	if (m_pCamera!=NULL)
	{
		if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,nTryMaxSize))
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}
		else
		{
			if (m_nWidth>=704 && m_nHeight>=576)
			{
				if (nTempWidth<640 || nTempHeight<480)
				{
					if (m_bAS120)
					{
						nTempWidth=704;
						nTempHeight=576;
					}
					else
					{
						nTempWidth=640;
						nTempHeight=480;
					}

					m_pCamera->Close();
					delete m_pCamera;
					m_pCamera=NULL;

					m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?0:1);
					if (m_pCamera!=NULL)
					{
						if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
						{
							m_pCamera->Close();
							delete m_pCamera;
							m_pCamera=NULL;
						}
					}
				}
			}
		}
	}

	if (m_pCamera==NULL)
	{
		nTempWidth=nWidth;
		nTempHeight=nHeight;

		m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?1:0);
		if (m_pCamera!=NULL)
		{
			if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
			{
				m_pCamera->Close();
				delete m_pCamera;
				m_pCamera=NULL;
			}
			else
			{
				if (m_nWidth>=704 && m_nHeight>=576)
				{
					if (nTempWidth<640 || nTempHeight<480)
					{
						if (m_bAS120)
						{
							nTempWidth=704;
							nTempHeight=576;
						}
						else
						{
							nTempWidth=640;
							nTempHeight=480;
						}

						m_pCamera->Close();
						delete m_pCamera;
						m_pCamera=NULL;

						m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?1:0);
						if (m_pCamera!=NULL)
						{
							if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
							{
								m_pCamera->Close();
								delete m_pCamera;
								m_pCamera=NULL;
							}
						}
					}
				}
			}
		}

		if (m_pCamera==NULL)
		{
			return -1;
		}
	}

	return 0;
}


void XCameraDev::Close(void)
{
	XImageSource::Close();

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

	
int XCameraDev::Reset(int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat)
{
	int nTryMaxSize=0;

	if (m_bAS120)
	{
		if (nWidth==0 && nHeight==0)
		{
			nWidth=704;
			nHeight=576;
		}
		else if (nWidth>=640)
		{
			nWidth=704;
			nHeight=576;
		}
		else if (nWidth>=320)
		{
			nWidth=352;
			nHeight=288;
		}
		else
		{
			nWidth=176;
			nHeight=144;
		}
	}

	if (nWidth==0 && nHeight==0)
	{
		if (m_isUSBCamera)
		{
			nWidth=640;
			nHeight=480;
			nTryMaxSize=1;
		}
		else
		{
			nWidth=704;
			nHeight=576;
		}
	}

	if (GetOnlyUSBCamera())
	{
		nTryMaxSize=0;
		if (m_isUSBCamera || m_bAS120)
		{
			if (nWidth>704 || nHeight>576)
			{
				nWidth=704;
				nHeight=576;
			}
		}
		else
		{
			nWidth=176;
			nHeight=144;
		}
	}

	m_nVirtualWidth=nWidth;
	m_nVirtualHeight=nHeight;

	if (m_bLifeCamera)
	{
		if (nWidth==960)
		{
			m_nVirtualWidth=960;
			m_nVirtualHeight=540;

			nWidth=960;
			nHeight=544;
		}
	}

	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_nFrameRate!=nFrameRate ||
		m_bDefaultFormat!=bDefaultFormat )
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameRate=nFrameRate;
		m_bDefaultFormat=bDefaultFormat;

		if (m_pCamera!=NULL)
		{
			m_pCamera->Close();
			delete m_pCamera;
			m_pCamera=NULL;
		}

		int nTempWidth=nWidth;
		int nTempHeight=nHeight;

		m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?0:1);
		if (m_pCamera!=NULL)
		{
			if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,nTryMaxSize))
			{
				m_pCamera->Close();
				delete m_pCamera;
				m_pCamera=NULL;
			}
			else
			{
				if (m_nWidth>=704 && m_nHeight>=576)
				{
					if (nTempWidth<640 || nTempHeight<480)
					{
						if (m_bAS120)
						{
							nTempWidth=704;
							nTempHeight=576;
						}
						else
						{
							nTempWidth=640;
							nTempHeight=480;
						}

						m_pCamera->Close();
						delete m_pCamera;
						m_pCamera=NULL;

						m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?0:1);
						if (m_pCamera!=NULL)
						{
							if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
							{
								m_pCamera->Close();
								delete m_pCamera;
								m_pCamera=NULL;
							}
						}
					}
				}
			}
		}

		if (m_pCamera==NULL)
		{
			nTempWidth=nWidth;
			nTempHeight=nHeight;

			m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?1:0);
			if (m_pCamera!=NULL)
			{
				if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
				{
					m_pCamera->Close();
					delete m_pCamera;
					m_pCamera=NULL;
				}
				else
				{
					if (m_nWidth>=704 && m_nHeight>=576)
					{
						if (nTempWidth<640 || nTempHeight<480)
						{
							if (m_bAS120)
							{
								nTempWidth=704;
								nTempHeight=576;
							}
							else
							{
								nTempWidth=640;
								nTempHeight=480;
							}

							m_pCamera->Close();
							delete m_pCamera;
							m_pCamera=NULL;

							m_pCamera=VIDEC_Camera::Create(*this,m_bAS120?1:0);
							if (m_pCamera!=NULL)
							{
								if (0!=m_pCamera->Open(m_nDevIndex,nTempWidth,nTempHeight,m_nFrameRate,0))
								{
									m_pCamera->Close();
									delete m_pCamera;
									m_pCamera=NULL;
								}
							}
						}
					}
				}
			}

			if (m_pCamera==NULL)
			{
				return -1;
			}
		}
	}

	return 0;
}

 
int XCameraDev::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}

	return 0;
}

int XCameraDev::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}

	return 0;
}

bool XCameraDev::CanBeClosed(void)
{
	return (XImageSource::GetImageSinkCount()<=0);
}


void XCameraDev::OnVIDEC_CameraCallbackImageData(VIDEC_Image*pImage)
{
	if (PreSendImage())
	{
		//if(m_bRBSCamera && m_pConvertUV)
		//{
		//	char * pTemp=(char *)pImage->GetData();
		//	int nY=pImage->GetWidth()*pImage->GetHeight();
		//	int nUV=nY/4;
		//	//unsigned char*pYUV[3];
		//	//pYUV[0]=(unsigned char*)pFrameData;
		//	//pYUV[2]=(unsigned char*)pFrameData+nY;
		//	//pYUV[1]=pYUV[2]+nUV;
		//	memcpy(m_pConvertUV,pTemp+nY,nUV);
		//	memcpy(pTemp+nY,pTemp+nY+nUV,nUV);
		//	memcpy(pTemp+nY+nUV,m_pConvertUV,nUV);
		//}

		XImage*pXImage=dynamic_cast<XImage*>(pImage);
		if (pXImage==NULL)
		{
			return ;
		}

		if (m_bLifeCamera && pXImage->GetActualWidth()>=960)
		{
			pXImage->SetVirtualWidth(m_nVirtualWidth);
			pXImage->SetVirtualHeight(m_nVirtualHeight);
		}

		if (m_pIXPreProcess!=NULL)
		{
			bool bDeinterlace=false;
			bool bDoubleFrameRate=false;
			//if (m_nFrameRate>30)
			//{
			//	bDoubleFrameRate=true;
			//}

			if (!m_isUSBCamera)
			{
				bDeinterlace=((pXImage->GetActualHeight()>=480 && pXImage->GetActualHeight()<=576) || pXImage->GetActualHeight()>=1056);
			}

			m_pIXPreProcess->EnableDeinterlaced(bDeinterlace,bDoubleFrameRate);

			m_pIXPreProcess->PreProcess(pImage);
		}
		else
		{
			OnIXPreProcessNotifyOutputData(pImage);
		}
	}	
}



void XCameraDev::OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage)
{
	SendImage(pImage);
}

int XCameraDev::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XCameraDev::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}


void XCameraDev::ShowAdjustVideoWnd(HWND hWnd)
{
	if (m_pCamera!=NULL)
	{
		m_pCamera->ShowAdjustVideoWnd(hWnd);
	}
}

void XCameraDev::ShowVideoInputWnd(HWND hWnd)
{
	if (m_pCamera!=NULL)
	{
		m_pCamera->ShowVideoInputWnd(hWnd);
	}
}

bool XCameraDev::HasVideoInputWnd(void)
{
	if (m_pCamera!=NULL)
	{
		return m_pCamera->HasVideoInputWnd();
	}
	return false;
}

int XCameraDev::TVTunerPutChannel(long lChannel)
{
	if (m_pCamera!=NULL)
	{
		if (m_pCamera->TVTunerPutChannel(lChannel))
		{
			return 0;
		}
	}
	return -1;
}


void XCameraDev::SetInvert(int nInvert)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableInvert(nInvert!=0);
	}
}

int XCameraDev::GetInvert(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsInvertEnable()?1:0;
	}
	return -1;
}

void XCameraDev::SetDenoise(int nDenoise)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(nDenoise!=0);
	}
}

int XCameraDev::GetDenoise(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsDenoiseEnable()?1:0;
	}
	return -1;
}

void XCameraDev::SetLevelFixer(int nLevelFixer)
{
}

int XCameraDev::GetLevelFixer(void)
{
	return -1;
}	

void XCameraDev::EnableOSD(bool bEnable)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableOSD(bEnable);
	}
}

bool XCameraDev::IsOSDEnable(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsOSDEnable();
	}
	return false;
}

void XCameraDev::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDText(cszText,nAlign,nTop,nLeft);
	}
}

void XCameraDev::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDFont(fontOSD,crText,crBk);
	}
}

//设置遮掩并启动遮掩
int XCameraDev::SetupMask(RECT *RectList, int iRectCount)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetupMask(RectList,iRectCount);
	}

	return 0;
}

//停止遮掩
int XCameraDev::StopMask(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->StopMask();
	}

	return 0;
}