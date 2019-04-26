#include "AH400Dev.h"
#include "XUtil.h"
#include "MPVIDEO_Codec.h"
#include "MPVIDEO_Header.h"
#include "../MPVIDEO_API/XImage.h"


#ifdef SUPPORT_AH400

AH400Dev::AH400Dev(unsigned int nDevIndex)
:XImageSource(this)
,XStreamSource(this)
,m_nDevIndex(nDevIndex)

,m_nWidth(0)
,m_nHeight(0)
,m_nFrameRate(15)

,m_pAH400(NULL)
,m_pIXPreProcess(NULL)

{
	m_pStreamBuf=NULL;
	m_nStreamBufLen=0;

	m_usSeqNo=0;

	m_bEnableHikOSD=false;
}

AH400Dev::~AH400Dev(void)
{

}

int AH400Dev::Open(int nWidth,int nHeight,int nFrameRate)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;

	m_pIXPreProcess=CreateIXPreProcess(*this);

	m_pAH400=MPVIDEO_AH400::Create(*this,m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,true);
	if (m_pAH400==NULL)
	{
		return -1;
	}
	
	m_pAH400->SetVideoParam(MPVIDEO_GetDefaultBrightness(),MPVIDEO_GetDefaultContrast(),MPVIDEO_GetDefaultSaturation(),MPVIDEO_GetDefaultHue());

	return 0;
}
 

void AH400Dev::Close(void)
{
	XImageSource::Close();
	XStreamSource::Close();

	if (m_pAH400!=NULL)
	{
		m_pAH400->Close();
		delete m_pAH400;
		m_pAH400=NULL;
	}

	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->ReleaseConnections();
		delete m_pIXPreProcess;
		m_pIXPreProcess=NULL;
	}

	if (m_pStreamBuf!=NULL)
	{
		free(m_pStreamBuf);
		m_pStreamBuf=NULL;
	}
}

	
int AH400Dev::Reset(int nWidth,int nHeight,int nFrameRate)
{
	if (m_nWidth!=nWidth ||
		m_nHeight!=nHeight ||
		m_nFrameRate!=nFrameRate)
	{
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		m_nFrameRate=nFrameRate;

		if (m_pAH400!=NULL)
		{
			m_pAH400->Close();
			delete m_pAH400;
			m_pAH400=NULL;
		}

		m_pAH400=MPVIDEO_AH400::Create(*this,m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,true);
		if (m_pAH400==NULL)
		{
			return -1;
		}		
	}

	return 0;
}


int AH400Dev::StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax)
{
	if (m_pAH400!=NULL)
	{
		if (m_pAH400!=NULL)
		{
			m_pAH400->Close();
			delete m_pAH400;
			m_pAH400=NULL;
		}

		m_pAH400=MPVIDEO_AH400::Create(*this,m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,false);
		if (m_pAH400==NULL)
		{
			return -1;
		}		

		return m_pAH400->StartVideoStream(nBitrate,nQuant,qmin,qmax,nKeyFrameInterval,bEnableSub,nSubScaleType,nSubBitrate,nSubQuant,nSubQMin,nSubQMax);
	}
	return -1;
}

int AH400Dev::StopVideoStream(void)
{
	if (m_pAH400!=NULL)
	{
		if (m_pAH400!=NULL)
		{
			m_pAH400->Close();
			delete m_pAH400;
			m_pAH400=NULL;
		}

		m_pAH400=MPVIDEO_AH400::Create(*this,m_nDevIndex,m_nWidth,m_nHeight,m_nFrameRate,true);
		if (m_pAH400==NULL)
		{
			return -1;
		}	

		return m_pAH400->StopVideoStream();
	}
	return -1;
}

int AH400Dev::RequestKeyFrame(void)
{
	if (m_pAH400!=NULL)
	{
		return m_pAH400->RequestKeyFrame();
	}
	return -1;
}


int AH400Dev::RequestSubKeyFrame(void)
{
	if (m_pAH400!=NULL)
	{
		return m_pAH400->RequestSubKeyFrame();
	}
	return -1;
}

int AH400Dev::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_pAH400!=NULL)
	{
		return m_pAH400->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
	return -1;
}

int AH400Dev::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}

	return 0;
}

bool AH400Dev::CanBeClosed(void)
{
	return (XImageSource::GetImageSinkCount()<=0 && XStreamSource::GetStreamSinkCount()<=0);
}


void AH400Dev::OnMPVIDEO_AH400CallbackImage(MPVIDEO_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
		return;

	if (m_pIXPreProcess!=NULL)
	{
		bool bDeinterlace=false;
		bool bDoubleFrameRate=false;
		if (m_nFrameRate>30)
		{
			bDoubleFrameRate=true;
		}

		{
			bDeinterlace=(pXImage->GetActualHeight()>288);
		}

		m_pIXPreProcess->EnableDeinterlaced(bDeinterlace,bDoubleFrameRate);
		m_pIXPreProcess->PreProcess(pImage);
	}
	else
	{
		OnIXPreProcessNotifyOutputData(pImage);
	}
}

void AH400Dev::OnMPVIDEO_AH400CallbackVideoStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen)
{
	SendStreamData(pData,nLen,nWidth,nHeight,nHeaderLen);
}

void AH400Dev::OnMPVIDEO_AH400CallbackVideoSubStreamData(unsigned char*pData,int nLen,int nWidth,int nHeight,int nHeaderLen)
{
	SendSubStreamData(pData,nLen,nWidth,nHeight,nHeaderLen);
}

void AH400Dev::OnMPVIDEO_AH400CallbackAudioStreamData(unsigned char*pData,int nLen)
{
	SendAudioStreamData(pData,nLen);
}

void AH400Dev::OnMPVIDEO_AH400CallbackMotionDetected(int*pResultList,int nResultCount)
{
	SendMotionDetect(pResultList,nResultCount);
}

void AH400Dev::OnIXPreProcessNotifyOutputData(MPVIDEO_Image*pImage)
{
	SendImage(pImage);
}

int AH400Dev::AddImageSink(MPVIDEO_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void AH400Dev::RemoveImageSink(MPVIDEO_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

int AH400Dev::AddStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void AH400Dev::RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void AH400Dev::SetInvert(int nInvert)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableInvert(nInvert!=0);
	}
}

int AH400Dev::GetInvert(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsInvertEnable()?1:0;
	}
	return -1;
}

void AH400Dev::SetDenoise(int nDenoise)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(nDenoise!=0);
	}
}

int AH400Dev::GetDenoise(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsDenoiseEnable()?1:0;
	}
	return -1;
}

void AH400Dev::SetLevelFixer(int nLevelFixer)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableLevelFixer(nLevelFixer!=0);
	}
}

int AH400Dev::GetLevelFixer(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsLevelFixerEnable()?1:0;
	}
	return -1;
}
	
void AH400Dev::EnableOSD(bool bEnable)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableOSD(bEnable);
	}

	if (m_pAH400!=NULL)
	{
		if (m_bEnableHikOSD)
		{
			m_pAH400->EnableOSD(bEnable);
		}
		else
		{
			m_pAH400->EnableOSD(false);
		}
	}
}

bool AH400Dev::IsOSDEnable(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsOSDEnable();
	}
	return false;
}

void AH400Dev::SetOSDText(const char*cszText,int nAlign)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDText(cszText,nAlign);
	}
	if (m_pAH400!=NULL)
	{
		if (cszText==NULL || strlen(cszText)==0)
		{
			m_bEnableHikOSD=true;

			m_pAH400->SetOSDText();
			m_pAH400->EnableOSD(IsOSDEnable());
		}
		else
		{
			m_bEnableHikOSD=false;
			m_pAH400->EnableOSD(false);
		}
	}
}

void AH400Dev::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDFont(fontOSD,crText,crBk);
	}
}

//设置遮掩并启动遮掩
int AH400Dev::SetupMask(RECT *RectList, int iRectCount)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetupMask(RectList,iRectCount);
	}

	if (m_pAH400!=NULL)
	{
		m_pAH400->SetupMask(RectList,iRectCount);
	}

	return 0;
}

//停止遮掩
int AH400Dev::StopMask(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->StopMask();
	}

	if (m_pAH400!=NULL)
	{
		m_pAH400->StopMask();
	}

	return 0;
}

int AH400Dev::StartMotionDetect(void)
{
	if (m_pAH400!=NULL)
	{
		m_pAH400->StartMotionDetect();
	}

	return 0;
}

int AH400Dev::StopMotionDetect(void)
{
	if (m_pAH400!=NULL)
	{
		m_pAH400->StopMotionDetect();
	}

	return 0;
}

int AH400Dev::SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount)
{
	if (m_pAH400!=NULL)
	{
		m_pAH400->SetupMotionDetect(iGrade,RectList,iRectCount);
	}

	return 0;
}


#endif
