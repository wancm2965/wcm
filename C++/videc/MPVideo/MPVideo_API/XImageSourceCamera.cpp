#include "XImageSourceCamera.h"
#include "XCameraDevMgr.h"
#include "XUtil.h"

MPVIDEO_CameraDev*MPVIDEO_CameraDev::Create(void)
{
	return new XImageSourceCamera();
}

//取设备个数
int MPVIDEO_CameraDev::GetDevCount(void)
{
	return XCameraDevMgr::GetDevCount();
}

//取设备名称
int MPVIDEO_CameraDev::GetDevName(unsigned int nDevIndex,char*szName,int nMaxCount)
{
	return XCameraDevMgr::GetDevName(nDevIndex,szName,nMaxCount);
}


XImageSourceCamera::XImageSourceCamera(void)
:XImageSink(this)
,XImageSource(this)
,m_nDevID(-1)
{
#ifdef _DEMO
	m_ulFirstTimestamp=XGetTimestamp();
#endif
}

XImageSourceCamera::~XImageSourceCamera(void)
{

}


int XImageSourceCamera::Open(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat)
{
	if (0!=XCameraDevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate,bDefaultFormat))
		return -1;

	if (0!=XCameraDevMgr::AddImageSink(nDevID,this))
		return -1;

	m_nDevID=nDevID;

	return 0;
}

void XImageSourceCamera::Close(void)
{
	XImageSource::Close();
	if (m_nDevID>=0)
	{
		XCameraDevMgr::RemoveImageSink(m_nDevID,this);
		m_nDevID=-1;
	}
}

int XImageSourceCamera::Reset(int nDevID,int nWidth,int nHeight,int nFrameRate,bool bDefaultFormat)
{
	if (m_nDevID!=nDevID && m_nDevID>=0)
	{
		XCameraDevMgr::RemoveImageSink(m_nDevID,this);
	}

	if (0!=XCameraDevMgr::SetDevCap(nDevID,nWidth,nHeight,nFrameRate,bDefaultFormat))
		return -1;

	if (m_nDevID!=nDevID)
	{
		if (0!=XCameraDevMgr::AddImageSink(nDevID,this))
			return -1;

		m_nDevID=nDevID;
	}

	return 0;
}
 
 
int XImageSourceCamera::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::SetVideoParam(m_nDevID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
	return -1;
}

int XImageSourceCamera::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::SetVideoGammaRGB(m_nDevID,nGammaR,nGammaG,nGammaB);
	}
	return -1;
}

void XImageSourceCamera::ShowAdjustVideoWnd(HWND hWnd)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::ShowAdjustVideoWnd(m_nDevID,hWnd);
	}
}

void XImageSourceCamera::ShowVideoInputWnd(HWND hWnd)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::ShowVideoInputWnd(m_nDevID,hWnd);
	}
}

bool XImageSourceCamera::HasVideoInputWnd(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::HasVideoInputWnd(m_nDevID);
	}
	return false;
}

int XImageSourceCamera::TVTunerPutChannel(long lChannel)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::TVTunerPutChannel(m_nDevID,lChannel);
	}
	return -1;
}

void XImageSourceCamera::SetInvert(int nInvert)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::SetInvert(m_nDevID,nInvert);
	}
}

int XImageSourceCamera::GetInvert(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::GetInvert(m_nDevID);
	}
	return -1;
}

void XImageSourceCamera::SetDenoise(int nDenoise)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::SetDenoise(m_nDevID,nDenoise);
	}
}

int XImageSourceCamera::GetDenoise(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::GetDenoise(m_nDevID);
	}
	return -1;
}

void XImageSourceCamera::SetLevelFixer(int nLevelFixer)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::SetLevelFixer(m_nDevID,nLevelFixer);
	}
}

int XImageSourceCamera::GetLevelFixer(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::GetLevelFixer(m_nDevID);
	}
	return -1;
}
	
void XImageSourceCamera::EnableOSD(bool bEnable)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::EnableOSD(m_nDevID,bEnable);
	}
}

bool XImageSourceCamera::IsOSDEnable(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::IsOSDEnable(m_nDevID);
	}
	return false;
}

void XImageSourceCamera::SetOSDText(const char*cszText,int nAlign)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::SetOSDText(m_nDevID,cszText,nAlign);
	}
}

void XImageSourceCamera::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_nDevID>=0)
	{
		XCameraDevMgr::SetOSDFont(m_nDevID,fontOSD,crText,crBk);
	}

}


//设置遮掩并启动遮掩
int XImageSourceCamera::SetupMask(RECT *RectList, int iRectCount)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::SetupMask(m_nDevID,RectList,iRectCount);
	}
	return -1;

}

//停止遮掩
int XImageSourceCamera::StopMask(void)
{
	if (m_nDevID>=0)
	{
		return XCameraDevMgr::StopMask(m_nDevID);
	}
	return -1;

}

int XImageSourceCamera::AddImageSink(MPVIDEO_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void XImageSourceCamera::RemoveImageSink(MPVIDEO_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void XImageSourceCamera::OnAttachedToImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void XImageSourceCamera::OnDetachedFromImageSource(MPVIDEO_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void XImageSourceCamera::OnReceivedImage(MPVIDEO_Image*pImage)
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

