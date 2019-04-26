//AH2KCapDev.cpp
#include "AH2KCapDev.h"
#include "XImage.h"
#include "libxpostproc.h"
#include "../VIDEC_API/AH2KDevMgr.h"

//��ʼ���豸��һ������ֻ��Ҫ����һ�Σ���ʹ���豸ǰ����
void VIDEC_AH2KCapDev::Init(void)
{
	::CoInitialize(NULL);

	VIDEC_PostProcInit();
#ifdef SUPPORT_AH2K
	AH2KDevMgr::Init();
#endif
}

//��ֹ�豸��һ������ֻ��Ҫ����һ�Σ���ʹ���豸������
void VIDEC_AH2KCapDev::Terminate(void)
{
#ifdef SUPPORT_AH2K
	AH2KDevMgr::Terminate();
#endif
	VIDEC_PostProcTerminate();

	::CoUninitialize();
}

//ȥ�豸����
int VIDEC_AH2KCapDev::GetDevCount(void)
{
#ifdef SUPPORT_AH2K
	return AH2KDevMgr::GetDevCount();
#endif
	return 0;
}

//�����豸IDȡ�豸����
int VIDEC_AH2KCapDev::GetDevName(int nDevID,char*szName,int nMaxCount)
{
#ifdef SUPPORT_AH2K
	return AH2KDevMgr::GetDevName(nDevID,szName,nMaxCount);
#endif
	return -1;
}

//����һ���豸ʵ��
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

//���豸����ʼ�ɼ���������ƵԴ��ʱ��Ż�ɼ���ͼ�񣬳ɹ�����0��ʧ�ܷ���-1
//nDevID���豸��ţ���0��ʼ
//nFrameRate��֡�ʣ�1-30
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

//�ر��豸
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

//����ͼ�����
//nBrightness:����
//nContrast:�Աȶ�
//nSaturation:���Ͷ�
//nHue:ɫ��
void AH2KCapDev::SetImageParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/)
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
