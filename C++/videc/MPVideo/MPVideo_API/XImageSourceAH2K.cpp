#include "XImageSourceAH2K.h"
#include "AH2KDevMgr.h"
#include "XUtil.h"

#ifdef SUPPORT_AH2K

XImageSourceAH2K::XImageSourceAH2K(void)
:XImageSink(this)
,XImageSource(this)
,XStreamSource(this)
,XStreamSink(this)
,m_nDevID(-1)
,m_nHasVideoSignal(1)
{
#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XImageSourceAH2K::~XImageSourceAH2K(void)
{

}


int XImageSourceAH2K::Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDenoise)
{
	if (0!=AH2KDevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate,bDenoise))
		return -1;

	if (0!=AH2KDevMgr::AddImageSink(nDevID,this))
		return -1;
	if (0!=AH2KDevMgr::AddStreamSink(nDevID,this))
		return -1;

	m_nDevID=nDevID;

	return 0;
}

void XImageSourceAH2K::Close(void)
{
	XImageSource::Close();
	XImageSink::Close();
	XStreamSource::Close();
	XStreamSink::Close();
	if (m_nDevID>=0)
	{
		AH2KDevMgr::RemoveImageSink(m_nDevID,this);
		AH2KDevMgr::RemoveStreamSink(m_nDevID,this);
		m_nDevID=-1;
	}
}

int XImageSourceAH2K::Reset(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDenoise)
{
	if (m_nDevID!=nDevID && m_nDevID>=0)
	{
		AH2KDevMgr::RemoveImageSink(m_nDevID,this);
		AH2KDevMgr::RemoveStreamSink(m_nDevID,this);
	}

	if (0!=AH2KDevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate,bDenoise))
		return -1;

	if (m_nDevID!=nDevID)
	{
		if (0!=AH2KDevMgr::AddImageSink(nDevID,this))
			return -1;
		if (0!=AH2KDevMgr::AddStreamSink(nDevID,this))
			return -1;

		m_nDevID=nDevID;
	}

	return 0;
}
  

int XImageSourceAH2K::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::SetVideoParam(m_nDevID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
	return -1;
}

int XImageSourceAH2K::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::SetVideoGammaRGB(m_nDevID,nGammaR,nGammaG,nGammaB);
	}
	return -1;
}

int XImageSourceAH2K::StartVideoStream(int nBitrate,int nBitrateControlType,int qmin,int qmax)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::StartVideoStream(m_nDevID,nBitrate,nBitrateControlType,qmin,qmax);
	}
	return -1;
}

int XImageSourceAH2K::RequestKeyFrame(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::RequestKeyFrame(m_nDevID);
	}
	return -1;
}

int XImageSourceAH2K::StopVideoStream(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::StopVideoStream(m_nDevID);
	}
	return -1;
}

void XImageSourceAH2K::SetInvert(int nInvert)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::SetInvert(m_nDevID,nInvert);
	}
}

int XImageSourceAH2K::GetInvert(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::GetInvert(m_nDevID);
	}
	return -1;
}

void XImageSourceAH2K::SetDenoise(int nDenoise)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::SetDenoise(m_nDevID,nDenoise);
	}
}

int XImageSourceAH2K::GetDenoise(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::GetDenoise(m_nDevID);
	}
	return -1;
}

void XImageSourceAH2K::SetLevelFixer(int nLevelFixer)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::SetLevelFixer(m_nDevID,nLevelFixer);
	}
}

int XImageSourceAH2K::GetLevelFixer(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::GetLevelFixer(m_nDevID);
	}
	return -1;
}

int XImageSourceAH2K::HasVideoSignal(void)
{
	return m_nHasVideoSignal;
}
	
void XImageSourceAH2K::EnableOSD(bool bEnable)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::EnableOSD(m_nDevID,bEnable);
	}
}

bool XImageSourceAH2K::IsOSDEnable(void)
{
	if (m_nDevID>=0)
	{
		return AH2KDevMgr::IsOSDEnable(m_nDevID);
	}
	return false;
}

void XImageSourceAH2K::SetOSDText(const char*cszText,int nAlign)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::SetOSDText(m_nDevID,cszText,nAlign);
	}
}

void XImageSourceAH2K::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_nDevID>=0)
	{
		AH2KDevMgr::SetOSDFont(m_nDevID,fontOSD,crText,crBk);
	}
}


int XImageSourceAH2K::AddImageSink(MPVIDEO_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XImageSourceAH2K::RemoveImageSink(MPVIDEO_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XImageSourceAH2K::OnAttachedToImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XImageSourceAH2K::OnDetachedFromImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XImageSourceAH2K::OnReceivedImage(MPVIDEO_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage)
	{
		m_nHasVideoSignal=pXImage->GetVideoSignal();
	}
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return ;
	}
#endif

	SendImage(pImage);
}

void XImageSourceAH2K::OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate)
{
	SendAudioSamples(pSamples,nSamples,nSampleRate);
}

int XImageSourceAH2K::AddStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XImageSourceAH2K::RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void XImageSourceAH2K::OnAttachedToStreamSource(MPVIDEO_StreamSource*pStreamSource)
{
	XStreamSink::OnAttachedToStreamSource(pStreamSource);
}

void XImageSourceAH2K::OnDetachedFromStreamSource(MPVIDEO_StreamSource*pStreamSource)
{
	XStreamSink::OnDetachedFromStreamSource(pStreamSource);
}

void XImageSourceAH2K::OnReceivedStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return ;
	}
#endif

	SendStreamData(pData,nLen,nWidth,nHeight,nHeaderLen);
}

void XImageSourceAH2K::OnReceivedSubStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return ;
	}
#endif

	SendSubStreamData(pData,nLen,nWidth,nHeight,nHeaderLen);
}

void XImageSourceAH2K::OnReceivedAudioStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen)
{
#ifdef _DEMO
	unsigned long ulTimestamp=XGetTimestamp();
	if (ulTimestamp<m_ulFirstTimestamp)
	{
		m_ulFirstTimestamp=ulTimestamp;
	}
	else if (ulTimestamp-m_ulFirstTimestamp>60000*20)
	{
		return ;
	}
#endif

	SendAudioStreamData(pData,nLen);
}


MPVIDEO_AH2KDev*MPVIDEO_AH2KDev::Create(void)
{
	return new XImageSourceAH2K();
}

//取设备个数
int MPVIDEO_AH2KDev::GetDevCount(void)
{
	return AH2KDevMgr::GetDevCount();
}

//取设备名称
int MPVIDEO_AH2KDev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return AH2KDevMgr::GetDevName(nDevIndex,szName,nMaxCount);
}

int MPVIDEO_AH2KDev::GetDevType(unsigned int nDevIndex,MPVIDEO_DEV_TYPE&nDevType)
{
	return AH2KDevMgr::GetDevType(nDevIndex,nDevType);
}

bool MPVIDEO_AH2KDev::HasAudioInput(unsigned int nDevIndex)
{
	return AH2KDevMgr::HasAudioInput(nDevIndex);
}

bool MPVIDEO_AH2KDev::SupportHardwareEncoding(unsigned int nDevIndex)
{
	return AH2KDevMgr::SupportHardwareEncoding(nDevIndex);
}

//列出可裁剪的格式的个数
int MPVIDEO_AH2KDev::GetFormatCount(void)
{
	return AH2KDevMgr::GetFormatCount();
}
	
//取格式名称
int MPVIDEO_AH2KDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return AH2KDevMgr::GetFormatName(nIndex,szName,nMaxCount);
}

//取格式大小
int MPVIDEO_AH2KDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return AH2KDevMgr::GetFormatSize(nIndex,nWidth,nHeight);
}

#else

MPVIDEO_AH2KDev*MPVIDEO_AH2KDev::Create(void)
{
	return NULL;
}

//取设备个数
int MPVIDEO_AH2KDev::GetDevCount(void)
{
	return 0;
}

//取设备名称
int MPVIDEO_AH2KDev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return -1;
}

int MPVIDEO_AH2KDev::GetDevType(unsigned int nDevIndex,MPVIDEO_DEV_TYPE&nDevType)
{
	return -1;
}

//列出可裁剪的格式的个数
int MPVIDEO_AH2KDev::GetFormatCount(void)
{
	return 0;
}
	
//取格式名称
int MPVIDEO_AH2KDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return -1;
}

//取格式大小
int MPVIDEO_AH2KDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return -1;
}
#endif