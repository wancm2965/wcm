#include "MPScreen.h"
#include "XUtil.h"
#include "ImageInput.h"
#include "../MPVIDEO_API/XImage.h"

MPVideo_MPScreen*MPVideo_MPScreen::Create(void)
{
	return new MPScreen();
}

MPScreen::MPScreen(void)
:XImageSource(this)
,m_pResize(NULL)
,m_nDstBufLen(0)
,m_pDstBuf(NULL)
{
	m_nWidth=352;
	m_nHeight=288;
	m_nFrameRate=30;
	m_nScreenIndex=-1;
	m_nFrameRateCount=1;
}

MPScreen::~MPScreen(void)
{

}

int MPScreen::Open(int nWidth,int nHeight,int nFrameRate)
{
	//nWidth =nWidth >>1 <<1; nHeight =nHeight >>1 <<1;  //20160427 解决新的VDP合成库在奇数时会出现偏色 YUV分离的问题 

	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;
	if (m_nFrameRate<1)
	{
		m_nFrameRate=1;
	}
	else if (m_nFrameRate>30)
	{
		m_nFrameRate=30;
	}

	ImageInputMgr::Instance().AddMPScreen(this);

	return 0;
}

void MPScreen::Close(void)
{
	ImageInputMgr::Instance().RemoveMPScreen(this);

	XImageSource::Close();

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

int MPScreen::SetScreenIndex(int nScreenIndex)
{
	int nOldScreenIndex=m_nScreenIndex;
	m_nScreenIndex=nScreenIndex;
	ImageInputMgr::Instance().OnMPScreenScreenIndexChanged(this,nOldScreenIndex);
	return 0;
}

int MPScreen::GetScreenIndex(void)
{
	return m_nScreenIndex;
}

int MPScreen::GetWidth(void)
{
	return m_nWidth;
}

int MPScreen::GetHeight(void)
{
	return m_nHeight;
}

int MPScreen::GetFrameRate(void)
{
	return m_nFrameRate;
}


int MPScreen::AddImageSink(MPVideo_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void MPScreen::RemoveImageSink(MPVideo_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void MPScreen::OnCaptureImage(MPVideo_Image*pImage)
{
	if (m_nFrameRateCount*m_nFrameRate<30)
	{
		m_nFrameRateCount++;
		return ;
	}

	m_nFrameRateCount=1;
	if (pImage->GetWidth()==m_nWidth && pImage->GetHeight()==m_nHeight)
	{
		XImageSource::SendImage(pImage);

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
	else
	{
		XImage*pXImage=dynamic_cast<XImage*>(pImage);
		if (pXImage==NULL)
		{
			return ;
		}
		unsigned char*pData=(unsigned char*)pXImage->GetData();
		int nLen=pXImage->GetDataLen();

		if (m_pResize==NULL)
		{
			m_pResize=MPVideo_Resize::Create();
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
			m_nDstBufLen=m_nWidth*m_nHeight*3/2;
			m_pDstBuf=(unsigned char*)malloc(m_nDstBufLen+(m_nDstBufLen>>2));
			if (m_pDstBuf!=NULL)
			{
				int nDstYBufferSize = m_nWidth * m_nHeight;
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

		bool bDoubleField=pXImage->GetDoubleField();
		int nSrcYBufferSize = pXImage->GetActualWidth() * pXImage->GetActualHeight();
		int nSrcUVBufferSize = nSrcYBufferSize >> 2;
		unsigned char*src[3];
		src[0]=pData;
		src[1] = src[0] + nSrcYBufferSize;
		src[2] = src[1] + nSrcUVBufferSize;
		if (0!=m_pResize->Resize(src,pXImage->GetActualWidth(),pXImage->GetActualHeight()/(bDoubleField?2:1),m_dst,m_nWidth,m_nHeight))
		{
			return;
		}
		pData=m_pDstBuf;
		nLen=m_nDstBufLen;

		{
			m_pDstBuf=NULL;
			XImage*pXImageNew=new XImage(pData,nLen);
			pXImageNew->AddRef();

			pXImageNew->SetActualWidth(m_nWidth);
			pXImageNew->SetActualHeight(m_nHeight);
			pXImageNew->SetVirtualWidth(m_nWidth);
			pXImageNew->SetVirtualHeight(m_nHeight);
			pXImageNew->SetEvenFieldFirst(0);
			pXImageNew->SetDoubleField(0);
			pXImageNew->SetEvenField(0);
			pXImageNew->SetOddField(0);
			pXImageNew->SetTimestamp(pXImage->GetTimestamp());
			pXImageNew->SetTimestampOffset(pXImage->GetTimestampOffset());
			pXImageNew->SetFrameRate(pXImage->GetFrameRate());
			pXImageNew->SetHardware(pXImage->GetHardware());

			XImageSource::SendImage(pXImageNew);

			pXImageNew->Release();
			pXImageNew=NULL;
		}
	}
}
