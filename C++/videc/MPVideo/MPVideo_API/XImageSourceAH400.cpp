#include "XImageSourceAH400.h"
#include "AH400DevMgr.h"
#include "XUtil.h"


#ifdef SUPPORT_AH400



XImageSourceAH400::XImageSourceAH400(void)
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

XImageSourceAH400::~XImageSourceAH400(void)
{

}


int XImageSourceAH400::Open(int nDevID,int nWidth,int nHeight,int nFrameRate)
{
	if (0!=AH400DevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate))
		return -1;

	if (0!=AH400DevMgr::AddImageSink(nDevID,this))
		return -1;
	if (0!=AH400DevMgr::AddStreamSink(nDevID,this))
		return -1;

	m_nDevID=nDevID;

	return 0;
}

void XImageSourceAH400::Close(void)
{
	XImageSource::Close();
	XImageSink::Close();
	XStreamSource::Close();
	XStreamSink::Close();
	if (m_nDevID>=0)
	{
		AH400DevMgr::RemoveImageSink(m_nDevID,this);
		AH400DevMgr::RemoveStreamSink(m_nDevID,this);
		m_nDevID=-1;
	}
}

int XImageSourceAH400::Reset(int nDevID,int nWidth,int nHeight,int nFrameRate)
{
	if (m_nDevID!=nDevID && m_nDevID>=0)
	{
		AH400DevMgr::RemoveImageSink(m_nDevID,this);
		AH400DevMgr::RemoveStreamSink(m_nDevID,this);
	}

	if (0!=AH400DevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate))
		return -1;

	if (m_nDevID!=nDevID)
	{
		if (0!=AH400DevMgr::AddImageSink(nDevID,this))
			return -1;
		if (0!=AH400DevMgr::AddStreamSink(nDevID,this))
			return -1;

		m_nDevID=nDevID;
	}

	return 0;
}

int XImageSourceAH400::StartVideoStream(int nBitrate,int nQuant,int qmin,int qmax,int nKeyFrameInterval,bool bEnableSub,int nSubScaleType,int nSubBitrate,int nSubQuant,int nSubQMin,int nSubQMax)
{
	int nRet=-1;
	if (m_nDevID>=0)
	{
		nRet=AH400DevMgr::StartVideoStream(m_nDevID,nBitrate,nQuant,qmin,qmax,nKeyFrameInterval,bEnableSub,nSubScaleType,nSubBitrate,nSubQuant,nSubQMin,nSubQMax);
	}
	return nRet;
}

int XImageSourceAH400::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::SetVideoGammaRGB(m_nDevID,nGammaR,nGammaG,nGammaB);
	}
	return -1;
}

int XImageSourceAH400::StopVideoStream(void)
{
	int nRet=-1;
	if (m_nDevID>=0)
	{
		nRet=AH400DevMgr::StopVideoStream(m_nDevID);
	}
	return nRet;
}

int XImageSourceAH400::RequestKeyFrame(void)
{
	int nRet=-1;
	if (m_nDevID>=0)
	{
		nRet=AH400DevMgr::RequestKeyFrame(m_nDevID);
	}
	return nRet;
}

int XImageSourceAH400::RequestSubKeyFrame(void)
{
	int nRet=-1;
	if (m_nDevID>=0)
	{
		nRet=AH400DevMgr::RequestSubKeyFrame(m_nDevID);
	}
	return nRet;
}

void XImageSourceAH400::SetInvert(int nInvert)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::SetInvert(m_nDevID,nInvert);
	}
}

int XImageSourceAH400::GetInvert(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::GetInvert(m_nDevID);
	}
	return -1;
}

void XImageSourceAH400::SetDenoise(int nDenoise)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::SetDenoise(m_nDevID,nDenoise);
	}
}

int XImageSourceAH400::GetDenoise(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::GetDenoise(m_nDevID);
	}
	return -1;
}

void XImageSourceAH400::SetLevelFixer(int nLevelFixer)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::SetLevelFixer(m_nDevID,nLevelFixer);
	}
}

int XImageSourceAH400::GetLevelFixer(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::GetLevelFixer(m_nDevID);
	}
	return -1;
}
	
void XImageSourceAH400::EnableOSD(bool bEnable)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::EnableOSD(m_nDevID,bEnable);
	}
}

bool XImageSourceAH400::IsOSDEnable(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::IsOSDEnable(m_nDevID);
	}
	return false;
}

void XImageSourceAH400::SetOSDText(const char*cszText,int nAlign)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::SetOSDText(m_nDevID,cszText,nAlign);
	}
}

void XImageSourceAH400::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_nDevID>=0)
	{
		AH400DevMgr::SetOSDFont(m_nDevID,fontOSD,crText,crBk);
	}
}


//设置遮掩并启动遮掩
int XImageSourceAH400::SetupMask(RECT *RectList, int iRectCount)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::SetupMask(m_nDevID,RectList,iRectCount);
	}
	return -1;
}

//停止遮掩
int XImageSourceAH400::StopMask(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::StopMask(m_nDevID);
	}
	return -1;

}

int XImageSourceAH400::StartMotionDetect(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::StartMotionDetect(m_nDevID);
	}
	return -1;
}

int XImageSourceAH400::StopMotionDetect(void)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::StopMotionDetect(m_nDevID);
	}
	return -1;
}

int XImageSourceAH400::SetupMotionDetect(int iGrade/*灵敏度等级0-6*/,RECT *RectList, int iRectCount)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::SetupMotionDetect(m_nDevID,iGrade,RectList,iRectCount);
	}
	return -1;
}
 

int XImageSourceAH400::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_nDevID>=0)
	{
		return AH400DevMgr::SetVideoParam(m_nDevID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
	return -1;
}


int XImageSourceAH400::AddImageSink(MPVIDEO_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XImageSourceAH400::RemoveImageSink(MPVIDEO_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XImageSourceAH400::OnAttachedToImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XImageSourceAH400::OnDetachedFromImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XImageSourceAH400::OnReceivedImage(MPVIDEO_Image*pImage)
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

void XImageSourceAH400::OnReceivedMotionDetect(int*pResultList,int nResultCount)
{
	SendMotionDetect(pResultList,nResultCount);
}


int XImageSourceAH400::AddStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	return XStreamSource::AddStreamSink(pStreamSink);
}

void XImageSourceAH400::RemoveStreamSink(MPVIDEO_StreamSink*pStreamSink)
{
	XStreamSource::RemoveStreamSink(pStreamSink);
}

void XImageSourceAH400::OnAttachedToStreamSource(MPVIDEO_StreamSource*pStreamSource)
{
	XStreamSink::OnAttachedToStreamSource(pStreamSource);
}

void XImageSourceAH400::OnDetachedFromStreamSource(MPVIDEO_StreamSource*pStreamSource)
{
	XStreamSink::OnDetachedFromStreamSource(pStreamSource);
}

void XImageSourceAH400::OnReceivedStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
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

void XImageSourceAH400::OnReceivedSubStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
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

void XImageSourceAH400::OnReceivedAudioStreamData(MPVIDEO_StreamSource*pStreamSource,unsigned char*pData,int nLen)
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



MPVIDEO_AH400Dev*MPVIDEO_AH400Dev::Create(void)
{
	return new XImageSourceAH400();
}

//取设备个数
int MPVIDEO_AH400Dev::GetDevCount(void)
{
	return AH400DevMgr::GetDevCount();
}

//取设备名称
int MPVIDEO_AH400Dev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return AH400DevMgr::GetDevName(nDevIndex,szName,nMaxCount);
}

//列出可裁剪的格式的个数
int MPVIDEO_AH400Dev::GetFormatCount(void)
{
	return AH400DevMgr::GetFormatCount();
}
	
//取格式名称
int MPVIDEO_AH400Dev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return AH400DevMgr::GetFormatName(nIndex,szName,nMaxCount);
}

//取格式大小
int MPVIDEO_AH400Dev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return AH400DevMgr::GetFormatSize(nIndex,nWidth,nHeight);
}

#else



MPVIDEO_AH400Dev*MPVIDEO_AH400Dev::Create(void)
{
	return NULL;
}

//取设备个数
int MPVIDEO_AH400Dev::GetDevCount(void)
{
	return 0;
}

//取设备名称
int MPVIDEO_AH400Dev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return 0;
}

//列出可裁剪的格式的个数
int MPVIDEO_AH400Dev::GetFormatCount(void)
{
	return 0;
}
	
//取格式名称
int MPVIDEO_AH400Dev::GetFormatName(int nIndex,char*szName,int nMaxCount)
{
	return 0;
}

//取格式大小
int MPVIDEO_AH400Dev::GetFormatSize(int nIndex,int&nWidth,int&nHeight)
{
	return -1;
}

#endif
