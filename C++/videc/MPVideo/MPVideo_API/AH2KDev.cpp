#include "AH2KDev.h"
#include "XUtil.h"
#include "../MPVIDEO_API/XImage.h"

#ifdef SUPPORT_AH2K

AH2KDev::AH2KDev(unsigned int nDevIndex)
:XImageSource(this)
,XStreamSource(this)
,m_nDevIndex(nDevIndex)

,m_nWidth(0)
,m_nHeight(0)
,m_nFrameRate(15)

,m_bDenoise(true)

,m_pAH2K(NULL)
,m_pIXPreProcess(NULL)

#ifdef AH2KDEV_USE_THREAD
,m_bRunning(false)
#endif
{
}

AH2KDev::~AH2KDev(void)
{

}

int AH2KDev::Open(int nWidth,int nHeight,int nFrameRate,bool bDenoise)
{
#ifdef SUPPORT_AH2K_ONLY_VGA
	if (nFrameRate>30)
	{
		nFrameRate=30;
	};
#endif

	m_nWidth=nWidth;
	m_nHeight=nHeight;

	m_nFrameRate=nFrameRate;
	m_bDenoise=bDenoise;

	m_pIXPreProcess=CreateIXPreProcess(*this);
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(m_bDenoise);
		m_pIXPreProcess->SetPictProp(MPVIDEO_GetDefaultBrightness(),MPVIDEO_GetDefaultContrast(),MPVIDEO_GetDefaultSaturation(),MPVIDEO_GetDefaultHue(),MPVIDEO_GetDefaultGamma(),MPVIDEO_GetDefaultSharpen());
	}

	m_pAH2K=MPVIDEO_AH2K::Create(*this);
	if (m_pAH2K!=NULL)
	{
		if (0!=m_pAH2K->Open(m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate))
		{
			m_pAH2K->Close();
			delete m_pAH2K;
			m_pAH2K=NULL;
		}
	}

	if (m_pAH2K==NULL)
	{
		return -1;
	}

#ifdef AH2KDEV_USE_THREAD
	m_bRunning=true;
	StartThread();
#endif

	return 0;
}


void AH2KDev::Close(void)
{
#ifdef AH2KDEV_USE_THREAD
	m_bRunning=false;
	WaitForStop();
#endif

	XImageSource::Close();
	XStreamSource::Close();

	if (m_pAH2K!=NULL)
	{
		m_pAH2K->Close();
		delete m_pAH2K;
		m_pAH2K=NULL;
	}

	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->ReleaseConnections();
		delete m_pIXPreProcess;
		m_pIXPreProcess=NULL;
	}

#ifdef AH2KDEV_USE_THREAD
	{
		XAutoLock l(m_csListImage);
		while (m_listImage.size())
		{
			MPVIDEO_Image*pImage=(MPVIDEO_Image*)m_listImage.front();
			m_listImage.pop_front();

			pImage->Release();
			pImage=NULL;
		}
	}
#endif
}


	
int AH2KDev::Reset(int nWidth,int nHeight,int nFrameRate,bool bDenoise)
{
#ifdef SUPPORT_AH2K_ONLY_VGA
	if (nFrameRate>30)
	{
		nFrameRate=30;
	};
#endif

	m_bDenoise=bDenoise;
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(m_bDenoise);
	}

	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_nFrameRate!=nFrameRate)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameRate=nFrameRate;

		if (m_pAH2K!=NULL)
		{
			m_pAH2K->Close();
			delete m_pAH2K;
			m_pAH2K=NULL;
		}

		m_pAH2K=MPVIDEO_AH2K::Create(*this);
		if (m_pAH2K!=NULL)
		{
			if (0!=m_pAH2K->Open(m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate))
			{
				m_pAH2K->Close();
				delete m_pAH2K;
				m_pAH2K=NULL;
			}
		}

		if (m_pAH2K==NULL)
		{
			return -1;
		}		
	}


	return 0;
}

 
int AH2KDev::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}

	return 0;
}

int AH2KDev::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}

	return 0;
}
  	
int AH2KDev::StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax)
{
	if (m_pAH2K!=NULL)
	{
		return m_pAH2K->StartVideoStream(nBitrate,nBitrateControlType,qmin,qmax);
	}
	return -1;
}

int AH2KDev::RequestKeyFrame(void)
{
	if (m_pAH2K!=NULL)
	{
		return m_pAH2K->RequestKeyFrame();
	}
	return -1;
}

int AH2KDev::StopVideoStream(void)
{
	if (m_pAH2K!=NULL)
	{
		return m_pAH2K->StopVideoStream();
	}
	return -1;
}

bool AH2KDev::CanBeClosed(void)
{
	return (XImageSource::GetImageSinkCount()<=0 && XStreamSource::GetStreamSinkCount()<=0);
}


void AH2KDev::OnMPVIDEO_AH2KCallbackImage(MPVIDEO_Image*pImage)
{
#ifdef AH2KDEV_USE_THREAD
	{
		XAutoLock l(m_csListImage);
		pImage->AddRef();
		m_listImage.push_back(pImage);

		while (m_listImage.size()>3)
		{
			pImage=(MPVIDEO_Image*)m_listImage.front();
			m_listImage.pop_front();

			pImage->Release();
			pImage=NULL;
		}
	}
#else
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->PreProcess(pImage);;
	}
	else
	{
		OnIXPreProcessNotifyOutputData(pImage);
	}
#endif
}

void AH2KDev::OnMPVIDEO_AH2KCallbackAudioData(short*pSamples,int nSamples,int nSampleRate)
{
	SendAudioSamples(pSamples,nSamples,nSampleRate);

	//char szDebug[1024]="";
	//sprintf(szDebug,"OnMPVIDEO_AH2KCallbackAudioData nSamples=%u\n",nSamples);
	//OutputDebugString(szDebug);
}

void AH2KDev::OnIXPreProcessNotifyOutputData(MPVIDEO_Image*pImage)
{
	SendImage(pImage);
}

int AH2KDev::AddImageSink(MPVIDEO_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void AH2KDev::RemoveImageSink(MPVIDEO_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

int AH2KDev::AddStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void AH2KDev::RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void AH2KDev::SetInvert(int nInvert)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableInvert(nInvert!=0);
	}
}

int AH2KDev::GetInvert(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsInvertEnable()?1:0;
	}
	return -1;
}

void AH2KDev::SetDenoise(int nDenoise)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(nDenoise!=0);
	}
}

int AH2KDev::GetDenoise(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsDenoiseEnable()?1:0;
	}
	return -1;
}

void AH2KDev::SetLevelFixer(int nLevelFixer)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableLevelFixer(nLevelFixer!=0);
	}
}

int AH2KDev::GetLevelFixer(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsLevelFixerEnable()?1:0;
	}
	return -1;
}
	
void AH2KDev::EnableOSD(bool bEnable)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableOSD(bEnable);
	}
}

bool AH2KDev::IsOSDEnable(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsOSDEnable();
	}
	return false;
}

void AH2KDev::SetOSDText(const char*cszText,int nAlign)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDText(cszText,nAlign);
	}
}

void AH2KDev::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDFont(fontOSD,crText,crBk);
	}
}


#ifdef AH2KDEV_USE_THREAD
void AH2KDev::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		MPVIDEO_Image*pImage=NULL;
		{
			XAutoLock l(m_csListImage);
			if (m_listImage.size())
			{
				pImage=(MPVIDEO_Image*)m_listImage.front();
				m_listImage.pop_front();
			}
		}

		if (pImage!=NULL)
		{
			if (m_pIXPreProcess!=NULL)
			{
				XImage*pXImage=dynamic_cast<XImage*>(pImage);

				if ( pXImage!=NULL &&
					 (pXImage->GetActualWidth()==704 && 
					(pXImage->GetActualHeight()==480 || 
					 pXImage->GetActualHeight()==576)) ||
					 (pXImage->GetActualWidth()==720 && 
					(pXImage->GetActualHeight()==480 || 
					 pXImage->GetActualHeight()==576)))
				{
					bool bDeinterlace=false;
					bool bDoubleFrameRate=false;

					if (m_nFrameRate>30)
					{
						bDoubleFrameRate=true;
						pXImage->SetHardware(false);
						pXImage->SetEvenFieldFirst(false);
						pXImage->SetVirtualWidth(pXImage->GetActualWidth());
						pXImage->SetVirtualHeight(pXImage->GetActualHeight());
					}

					{
						bDeinterlace=(pXImage->GetActualHeight()>288);
					}

					m_pIXPreProcess->EnableDeinterlaced(bDeinterlace,bDoubleFrameRate);
				}

				m_pIXPreProcess->PreProcess(pImage);;
			}
			else
			{
				OnIXPreProcessNotifyOutputData(pImage);
			}

			pImage->Release();
			pImage=NULL;
		}
		else
		{
			XSleep(1);
		}
	}
}
#endif

#endif