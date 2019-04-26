#include "DVCDev.h"
#include "XUtil.h"
#include "../VIDEC_API/XImage.h"
#include <VIDEC/VIDEC_Header.h>

#ifdef SUPPORT_DVC

DVCDev::DVCDev(unsigned int nDevIndex)
:XImageSource(this)
,XStreamSource(this)
,m_nDevIndex(nDevIndex)

,m_nBitrate(1000)
,m_nBitrateControlType(0)
,m_nFrameRate(30)
,m_nResolution(6)


,m_pDVC(NULL)
{
}

DVCDev::~DVCDev(void)
{

}

int DVCDev::Open(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	m_nBitrate=nBitrate;
	m_nBitrateControlType=nBitrateControlType;

	m_nFrameRate=nFrameRate;
	m_nResolution=nResolution;

	m_pDVC=VIDEC_DVC::Create(*this);
	if (m_pDVC!=NULL)
	{
		if (0!=m_pDVC->Open(m_nDevIndex,m_nFrameRate,m_nBitrate,m_nBitrateControlType,m_nResolution))
		{
			m_pDVC->Close();
			delete m_pDVC;
			m_pDVC=NULL;
		}
	}

	if (m_pDVC==NULL)
	{
		return -1;
	}


	return 0;
}


void DVCDev::Close(void)
{
	XImageSource::Close();
	XStreamSource::Close();

	if (m_pDVC!=NULL)
	{
		m_pDVC->Close();
		delete m_pDVC;
		m_pDVC=NULL;
	}
}


	
int DVCDev::Reset(int nFrameRate,int nBitrate,int nBitrateControlType,int nResolution)
{
	if (m_nBitrate!=nBitrate ||
		m_nBitrateControlType!=nBitrateControlType ||
		m_nFrameRate!=nFrameRate ||
		m_nResolution!=nResolution)
	{
		m_nBitrate=nBitrate;
		m_nBitrateControlType=nBitrateControlType;
		m_nFrameRate=nFrameRate;
		m_nResolution=nResolution;

		if (m_pDVC!=NULL)
		{
			m_pDVC->SetParams(m_nFrameRate,m_nBitrate,m_nBitrateControlType,m_nResolution);
		}
		else
		{
			m_pDVC=VIDEC_DVC::Create(*this);
			if (m_pDVC!=NULL)
			{
				if (0!=m_pDVC->Open(m_nDevIndex,m_nFrameRate,m_nBitrate,m_nBitrateControlType,m_nResolution))
				{
					m_pDVC->Close();
					delete m_pDVC;
					m_pDVC=NULL;
				}
			}

			if (m_pDVC==NULL)
			{
				return -1;
			}		
		}

	}


	return 0;
}

 
int DVCDev::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{

	return 0;
}

int DVCDev::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{

	return 0;
}

int DVCDev::RequestKeyFrame(void)
{
	if (m_pDVC!=NULL)
	{
		return m_pDVC->RequestKeyFrame();
	}
	return -1;
}


bool DVCDev::CanBeClosed(void)
{
	return (XImageSource::GetImageSinkCount()<=0 && XStreamSource::GetStreamSinkCount()<=0);
}


void DVCDev::OnVIDEC_DVCCallbackImage(VIDEC_Image*pImage)
{
	SendImage(pImage);
}

void DVCDev::OnVIDEC_DVCCallbackVideoStreamData(unsigned char*pData,int nLen,int nKeyFrame,int nWidth,int nHeight)
{
	int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pData);
	SendStreamData(pData,nLen,nWidth,nHeight,nHeaderLen);
}

int DVCDev::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void DVCDev::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

int DVCDev::AddStreamSink(VIDEC_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void DVCDev::RemoveStreamSink(VIDEC_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void DVCDev::SetInvert(int nInvert)
{

}

int DVCDev::GetInvert(void)
{

	return -1;
}

void DVCDev::SetDenoise(int nDenoise)
{

}

int DVCDev::GetDenoise(void)
{
	return -1;
}

void DVCDev::SetLevelFixer(int nLevelFixer)
{

}

int DVCDev::GetLevelFixer(void)
{
	return -1;
}
	
void DVCDev::EnableOSD(bool bEnable)
{

}

bool DVCDev::IsOSDEnable(void)
{

	return false;
}

void DVCDev::SetOSDText(const char*cszText,int nAlign)
{

}

void DVCDev::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{

}

#endif