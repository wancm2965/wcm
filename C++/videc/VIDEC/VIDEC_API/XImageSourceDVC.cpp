#include "XImageSourceDVC.h"
#include "DVCDevMgr.h"
#include "XUtil.h"
#include "../VIDEC_DVC/DVCWrapper.h"

#ifdef SUPPORT_DVC

XImageSourceDVC::XImageSourceDVC(void)
:XImageSink(this)
,XImageSource(this)
,XStreamSource(this)
,XStreamSink(this)
,m_nDevID(-1)
{
#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XImageSourceDVC::~XImageSourceDVC(void)
{

}


int XImageSourceDVC::Open(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolusion)
{
	if (0!=DVCDevMgr::SetDevCap(nDevID,nFrameRate,nBitrate,nBitrateControlType,nResolusion))
		return -1;

	if (0!=DVCDevMgr::AddImageSink(nDevID,this))
		return -1;
	if (0!=DVCDevMgr::AddStreamSink(nDevID,this))
		return -1;

	m_nDevID=nDevID;

	return 0;
}

void XImageSourceDVC::Close(void)
{
	XImageSource::Close();
	XImageSink::Close();
	XStreamSource::Close();
	XStreamSink::Close();
	if (m_nDevID>=0)
	{
		DVCDevMgr::RemoveImageSink(m_nDevID,this);
		DVCDevMgr::RemoveStreamSink(m_nDevID,this);
		m_nDevID=-1;
	}
}

int XImageSourceDVC::Reset(int nDevID,int nFrameRate,int nBitrate,int nBitrateControlType,int nResolusion)
{
	if (m_nDevID!=nDevID && m_nDevID>=0)
	{
		DVCDevMgr::RemoveImageSink(m_nDevID,this);
		DVCDevMgr::RemoveStreamSink(m_nDevID,this);
	}

	if (0!=DVCDevMgr::SetDevCap(nDevID,nFrameRate,nBitrate,nBitrateControlType,nResolusion))
		return -1;

	if (m_nDevID!=nDevID)
	{
		if (0!=DVCDevMgr::AddImageSink(nDevID,this))
			return -1;
		if (0!=DVCDevMgr::AddStreamSink(nDevID,this))
			return -1;

		m_nDevID=nDevID;
	}

	return 0;
}
  

int XImageSourceDVC::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::SetVideoParam(m_nDevID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
	return -1;
}

int XImageSourceDVC::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::SetVideoGammaRGB(m_nDevID,nGammaR,nGammaG,nGammaB);
	}
	return -1;
}

int XImageSourceDVC::RequestKeyFrame(void)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::RequestKeyFrame(m_nDevID);
	}
	return -1;
}

int XImageSourceDVC::SendData(const char*pData,int nLen)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::SendData(m_nDevID,pData,nLen);
	}
	return -1;
}

void XImageSourceDVC::SetInvert(int nInvert)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::SetInvert(m_nDevID,nInvert);
	}
}

int XImageSourceDVC::GetInvert(void)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::GetInvert(m_nDevID);
	}
	return -1;
}

void XImageSourceDVC::SetDenoise(int nDenoise)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::SetDenoise(m_nDevID,nDenoise);
	}
}

int XImageSourceDVC::GetDenoise(void)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::GetDenoise(m_nDevID);
	}
	return -1;
}

void XImageSourceDVC::SetLevelFixer(int nLevelFixer)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::SetLevelFixer(m_nDevID,nLevelFixer);
	}
}

int XImageSourceDVC::GetLevelFixer(void)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::GetLevelFixer(m_nDevID);
	}
	return -1;
}
	
void XImageSourceDVC::EnableOSD(bool bEnable)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::EnableOSD(m_nDevID,bEnable);
	}
}

bool XImageSourceDVC::IsOSDEnable(void)
{
	if (m_nDevID>=0)
	{
		return DVCDevMgr::IsOSDEnable(m_nDevID);
	}
	return false;
}

void XImageSourceDVC::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::SetOSDText(m_nDevID,cszText,nAlign,nTop,nLeft);
	}
}

void XImageSourceDVC::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_nDevID>=0)
	{
		DVCDevMgr::SetOSDFont(m_nDevID,fontOSD,crText,crBk);
	}
}


int XImageSourceDVC::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XImageSourceDVC::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XImageSourceDVC::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XImageSourceDVC::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}
void XImageSourceDVC::OnReceivedPreSend(int& nSend)
{
	nSend = PreSendImage();
}
void XImageSourceDVC::OnReceivedImage(VIDEC_Image*pImage)
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

	SendImage(pImage);
}

void XImageSourceDVC::OnReceivedAudioSamples(short*pSamples,int nSamples,int nSampleRate)
{
	SendAudioSamples(pSamples,nSamples,nSampleRate);
}

int XImageSourceDVC::AddStreamSink(VIDEC_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XImageSourceDVC::RemoveStreamSink(VIDEC_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void XImageSourceDVC::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XStreamSink::OnAttachedToStreamSource(pStreamSource);
}

void XImageSourceDVC::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{
	XStreamSink::OnDetachedFromStreamSource(pStreamSource);
}

void XImageSourceDVC::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
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

void XImageSourceDVC::OnReceivedSubStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
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

void XImageSourceDVC::OnReceivedAudioStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen)
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


VIDEC_DVCDev*VIDEC_DVCDev::Create(void)
{
	//return new XImageSourceDVC();
	NEWRECORDret3(  VIDEC_DVCDev*, p , new XImageSourceDVC( )  )   
}

//取设备个数
int VIDEC_DVCDev::GetDevCount(void)
{
	return DVCDevMgr::GetDevCount();
}

//取设备名称
int VIDEC_DVCDev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return DVCDevMgr::GetDevName(nDevIndex,szName,nMaxCount);
}

int VIDEC_DVCDev::GetDevType(unsigned int nDevIndex,VIDEC_DEV_TYPE&nDevType)
{
	return DVCDevMgr::GetDevType(nDevIndex,nDevType);
}

bool VIDEC_DVCDev::HasAudioInput(unsigned int nDevIndex)
{
	return DVCDevMgr::HasAudioInput(nDevIndex);
}

bool VIDEC_DVCDev::SupportHardwareEncoding(unsigned int nDevIndex)
{
	return DVCDevMgr::SupportHardwareEncoding(nDevIndex);
}

//列出可裁剪的格式的个数
int VIDEC_DVCDev::GetFormatCount(void)
{
	return DVCDevMgr::GetFormatCount();
}

//取格式名称
int VIDEC_DVCDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return DVCDevMgr::GetFormatName(nIndex,szName,nMaxCount);
}

//取格式大小
int VIDEC_DVCDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return DVCDevMgr::GetFormatSize(nIndex,nWidth,nHeight);
}


void VIDEC_DVCDev::SetIP(const char*cszIP,unsigned short nPort,const char*cszPassword)
{
	DVCSetIP(cszIP,nPort,cszPassword);
}

#else

VIDEC_DVCDev*VIDEC_DVCDev::Create(void)
{
	return NULL;
}

//取设备个数
int VIDEC_DVCDev::GetDevCount(void)
{
	return 0;
}

//取设备名称
int VIDEC_DVCDev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return -1;
}

int VIDEC_DVCDev::GetDevType(unsigned int nDevIndex,VIDEC_DEV_TYPE&nDevType)
{
	return -1;
}

//列出可裁剪的格式的个数
int VIDEC_DVCDev::GetFormatCount(void)
{
	return 0;
}
	
//取格式名称
int VIDEC_DVCDev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return -1;
}

//取格式大小
int VIDEC_DVCDev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return -1;
}
#endif