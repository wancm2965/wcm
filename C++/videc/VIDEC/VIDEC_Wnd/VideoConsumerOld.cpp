#include "VideoConsumerOld.h"
#include "XUtil.h"

VideoConsumerOld::VideoConsumerOld(VideoConsumerCallback&rCallback)
:m_rCallback(rCallback)
{
	m_hVideoWnd=NULL;
	m_nWidth=320;
	m_nHeight=240;
	m_nFrameRate=30;
	m_pDraw=NULL;
	m_nLastDrawTimestamp=0;
	m_nNextFrameInterval=1000/m_nFrameRate;

	m_bNV12=false;

	m_bRunning=false;
}

VideoConsumerOld::~VideoConsumerOld(void)
{
}


int VideoConsumerOld::Open(long nHandle,int nWidth,int nHeight,int nFrameRate,VC_CSP_TYPE nCSPType)
{
	m_hVideoWnd=(HWND)nHandle;
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;

	m_bNV12=(nCSPType==VC_CSP_NV12);

	m_pDraw=IYUVDraw::Create(m_hVideoWnd,m_nWidth,m_nHeight,m_nWidth,m_nHeight,m_bNV12);
	if (m_pDraw==NULL)
	{
		return -1;
	}

	m_bRunning=true;
	if (!StartThread())
	{
		return -1;
	}

	return 0;
}

void VideoConsumerOld::Close(void)
{
	m_bRunning=false;
	WaitForStop();

	if (m_pDraw)
	{
		m_pDraw->Close();
		delete m_pDraw;
		m_pDraw=NULL;
	}
}

int VideoConsumerOld::GetWidth(void)
{
	return m_nWidth;
}

int VideoConsumerOld::GetHeight(void)
{
	return m_nHeight;
}

int VideoConsumerOld::GetFrameRate(void)
{
	return m_nFrameRate;
}

void VideoConsumerOld::SetWndPos(int x,int y,int cx,int cy)
{
	if (m_pDraw)
	{
		m_pDraw->SetWindowPos(x,y,cx,cy);
	}
}

void VideoConsumerOld::Repaint(void)
{

}


bool VideoConsumerOld::IsPending(void)
{
	if (::IsWindowVisible(m_hVideoWnd)==FALSE)
	{
		return false;
	}

	unsigned long nTimestamp=XGetTimestamp();
	if (nTimestamp-m_nLastDrawTimestamp>=m_nNextFrameInterval)
	{
		return true;
	}
	return false;
}

void VideoConsumerOld::ThreadProcMain(void)
{
	while (m_bRunning)
	{
		if (IsPending())
		{
			Running();
		}
		else
		{
			XSleep(10);
		}
	}
}

bool VideoConsumerOld::Running(void)
{
	unsigned char*pFrameData=NULL;
	int nFrameLen=0;
	int nWidth=0;
	int nHeight=0;
	int nFrameRate=0;
	int nStride=0;
	unsigned long nTimestamp=XGetTimestamp();

	VIDEC_Image*pImage=NULL;

	m_rCallback.OnVideoConsumerCallbackPull(&pFrameData,nWidth,nHeight,nStride,nFrameRate,&pImage);
	if (pFrameData==NULL)
	{
		m_nLastDrawTimestamp=nTimestamp+10;
		if (pImage)
		{
			pImage->Release();
			pImage=NULL;
		}
		return false;
	}

	if (m_pDraw)
	{
		if (!m_pDraw->Draw(pFrameData,nFrameLen,nStride))
		{
			m_rCallback.OnVideoConsumerCallbackDrawFailed();
			m_nLastDrawTimestamp=nTimestamp+10;

			if (pImage)
			{
				pImage->Release();
				pImage=NULL;
			}
			return false;
		}
	}

	if (pImage)
	{
		pImage->Release();
		pImage=NULL;
	}

	unsigned long nCurrentTimestamp=XGetTimestamp();
	unsigned long nDelta=nCurrentTimestamp-nTimestamp;
	if (nFrameRate<1)
		nFrameRate=1;
	m_nFrameRate=nFrameRate;

	unsigned long nFrameInterval=1000/nFrameRate;

	if (nFrameInterval>=60)
	{
		nFrameInterval=60;
	}
	else if (nFrameInterval>=38)
	{
		nFrameInterval=38;
	}
	else if (nFrameInterval>=30)
	{
		nFrameInterval=30;
	}
	else if (nFrameInterval>=18)
	{
		nFrameInterval=18;
	}
	else if (nFrameInterval>=14)
	{
		nFrameInterval=14;
	}
	else
	{
		nFrameInterval=10;
	}

	if (nFrameInterval>nDelta)
	{
		nFrameInterval-=nDelta;

		//char szDebug[1024];
		//sprintf(szDebug,"%u\n",nCurrentTimestamp-m_nLastDrawTimestamp);
		//OutputDebugString(szDebug);

		m_nLastDrawTimestamp=nCurrentTimestamp;
		m_nNextFrameInterval=nFrameInterval;
	}

	return true;
}
