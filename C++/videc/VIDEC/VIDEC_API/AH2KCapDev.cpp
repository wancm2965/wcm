//AH2KCapDev.cpp
#include "AH2KCapDev.h"
#include "XImage.h"
#include "libxpostproc.h"
#include "../VIDEC_API/AH2KDevMgr.h"

//初始化设备，一个进程只需要调用一次，在使用设备前调用
void VIDEC_AH2KCapDev::Init(void)
{
	::CoInitialize(NULL);

	VIDEC_PostProcInit();
#ifdef SUPPORT_AH2K
	AH2KDevMgr::Init();
#endif
}

//终止设备，一个进程只需要调用一次，在使用设备完后调用
void VIDEC_AH2KCapDev::Terminate(void)
{
#ifdef SUPPORT_AH2K
	AH2KDevMgr::Terminate();
#endif
	VIDEC_PostProcTerminate();

	::CoUninitialize();
}

//去设备个数
int VIDEC_AH2KCapDev::GetDevCount(void)
{
#ifdef SUPPORT_AH2K
	return AH2KDevMgr::GetDevCount();
#endif
	return 0;
}

//根据设备ID取设备名称
int VIDEC_AH2KCapDev::GetDevName(int nDevID,char*szName,int nMaxCount)
{
#ifdef SUPPORT_AH2K
	return AH2KDevMgr::GetDevName(nDevID,szName,nMaxCount);
#endif
	return -1;
}

//创建一个设备实例
VIDEC_AH2KCapDev*VIDEC_AH2KCapDev::Create(VIDEC_AH2KCapDevCallback&rCallback)
{
	return new AH2KCapDev(rCallback);
}


AH2KCapDev::AH2KCapDev(VIDEC_AH2KCapDevCallback&rCallback)
:m_rCallback(rCallback)
,m_pDev(NULL)
,m_pResize(NULL)
,m_nDstBufLen(0)
,m_pDstBuf(NULL)
{

}

AH2KCapDev::~AH2KCapDev(void)
{

}

//打开设备，开始采集，当有视频源的时候才会采集到图像，成功返回0，失败返回-1
//nDevID：设备序号，从0开始
//nFrameRate：帧率，1-30
int AH2KCapDev::Open(int nDevID,int nFrameRate)
{
	m_pDev=VIDEC_AH2KDev::Create();
	if (m_pDev==NULL)
	{
		return -1;
	}

	if (nFrameRate<1)
		nFrameRate=1;
	else if (nFrameRate>30)
		nFrameRate=30;

	if (0!=m_pDev->Open(nDevID,0,0,nFrameRate,false))
	{
		m_pDev->Close();
		delete m_pDev;
		m_pDev=NULL;
		return -1;
	}

	return 0;
}

//关闭设备
void AH2KCapDev::Close(void)
{
	if (m_pDev!=NULL)
	{
		m_pDev->Close();
		delete m_pDev;
		m_pDev=NULL;
	}
	if (m_pResize!=NULL)
	{
		m_pResize->Close();
		delete m_pResize;
		m_pResize=NULL;
	}
	
	if (m_pDstBuf!=NULL)
	{
		free(m_pDstBuf);
		m_pDstBuf=NULL;
	}
}

//设置图像参数
//nBrightness:亮度
//nContrast:对比度
//nSaturation:饱和度
//nHue:色调
void AH2KCapDev::SetImageParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_pDev!=NULL)
	{
		m_pDev->SetVideoParam(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}
}
 
void AH2KCapDev::OnReceivedImage(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	unsigned char*pData=(unsigned char*)pXImage->GetData();
	int nLen=pXImage->GetDataLen();

	int nWidth=pXImage->GetVirtualWidth();
	int nHeight=pXImage->GetActualHeight();

	if (nWidth!=pXImage->GetActualWidth() ||
		nHeight!=pXImage->GetActualHeight())
	{
		if (m_pResize==NULL)
		{
			m_pResize=VIDEC_Resize::Create();
			if (m_pResize!=NULL)
			{
				if (0!=m_pResize->Open())
				{
					m_pResize->Close();
					delete m_pResize;
					m_pResize=NULL;
				}
			}
		}

		if (m_pDstBuf==NULL)
		{
			m_nDstBufLen=nWidth*nHeight*3/2;
			m_pDstBuf=(unsigned char*)malloc(m_nDstBufLen);
			if (m_pDstBuf!=NULL)
			{
				int nDstYBufferSize = nWidth * nHeight;
				int nDstUVBufferSize = nDstYBufferSize >> 2;
				m_dst[0] = m_pDstBuf;
				m_dst[1] = m_dst[0] + nDstYBufferSize;
				m_dst[2] = m_dst[1] + nDstUVBufferSize;
			}
		}

		if (m_pResize==NULL || m_pDstBuf==NULL)
		{
			return;
		}

		int nSrcYBufferSize = pXImage->GetActualWidth() * pXImage->GetActualHeight();
		int nSrcUVBufferSize = nSrcYBufferSize >> 2;
		unsigned char*src[3];
		src[0]=pData;
		src[1] = src[0] + nSrcYBufferSize;
		src[2] = src[1] + nSrcUVBufferSize;
		if (0!=m_pResize->Resize(src,pXImage->GetActualWidth(),pXImage->GetActualHeight(),m_dst,nWidth,nHeight))
		{
			return;
		}
		pData=m_pDstBuf;
		nLen=m_nDstBufLen;
	}
	else
	{
		if (m_pResize!=NULL)
		{
			m_pResize->Close();
			delete m_pResize;
			m_pResize=NULL;
		}
		if (m_pDstBuf!=NULL)
		{
			free(m_pDstBuf);
			m_pDstBuf=NULL;
		}
	}


	m_rCallback.OnVIDEC_AH2KCapDevCallbackImageData(pData,nLen,nWidth,nHeight);
}
