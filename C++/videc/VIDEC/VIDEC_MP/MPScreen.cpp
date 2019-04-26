#include "MPScreen.h"
#include "XUtil.h"

VIDEC_MPScreen*VIDEC_MPScreen::Create(void)
{
	//return new MPScreen();
	NEWRECORDret3(  VIDEC_MPScreen* ,p , new MPScreen( )  )   
}

MPScreen::MPScreen(void)
:XImageSource(this)
{
	m_bRunning=false;
	m_nWidth=352;
	m_nHeight=288;
	m_nFrameRate=30;
	m_nScreenIndex=0;

	m_pScreenInput=NULL;
}

MPScreen::~MPScreen(void)
{

}

int MPScreen::Open(int nWidth,int nHeight,int nFrameRate)
{
	m_nWidth=nWidth;
	m_nHeight=nHeight;
	m_nFrameRate=nFrameRate;
	if (m_nFrameRate<1)
	{
		m_nFrameRate=1;
	}
	else if (m_nFrameRate>60)
	{
		m_nFrameRate=60;
	}

	m_bRunning=true;
	if (!StartThread())
	{
		return -1;
	}

	MPScreenMgr::Instance().AddMPScreen(this);

	return 0;
}

void MPScreen::Close(void)
{
	MPScreenMgr::Instance().RemoveMPScreen(this);

	m_bRunning=false;
	WaitForStop();

	XImageSource::Close();
}

int MPScreen::SetScreenIndex(int nScreenIndex)
{
	m_nScreenIndex=nScreenIndex;
	m_pScreenInput=ImageInputMgr::Instance().GetScreenInput(m_nScreenIndex);
	if (m_pScreenInput!=NULL)
	{
		m_pScreenInput->SetImageSize(m_nWidth,m_nHeight);
		return 0;
	}
	return -1;
}

int MPScreen::GetScreenIndex(void)
{
	return m_nScreenIndex;
}


int MPScreen::AddImageSink(VIDEC_ImageSink*pImageSink)
{
	return XImageSource::AddImageSink(pImageSink);
}

void MPScreen::RemoveImageSink(VIDEC_ImageSink*pImageSink)
{
	XImageSource::RemoveImageSink(pImageSink);
}

void MPScreen::ThreadProcMain(void)
{
	unsigned long ulFrameInterval=1000/m_nFrameRate;
	VIDEC_Image*pImage=NULL;
	ScreenInput*pScreenInput=NULL;
	int nFaileCount=0;
	while (m_bRunning)
	{
		pScreenInput=m_pScreenInput;
		if (pScreenInput!=NULL)
		{
			pImage=pScreenInput->GetImage((nFaileCount>=m_nFrameRate?1:0),m_nFrameRate);
			if (pImage!=NULL)
			{
				nFaileCount=0;

				XImageSource::SendImage(pImage);

				pImage->Release();
				pImage=NULL;
			}
			else
			{
				nFaileCount+=2;
			}
		}

		XSleep(ulFrameInterval);
	}
}

static MPScreenMgr g_MPScreenMgr;
MPScreenMgr::MPScreenMgr(void)
{

}

MPScreenMgr::~MPScreenMgr(void)
{

}

MPScreenMgr&MPScreenMgr::Instance(void)
{
	return g_MPScreenMgr;
}

void MPScreenMgr::AddMPScreen(MPScreen*pMPScreen)
{
	m_MapMPScreen[pMPScreen]=pMPScreen;
}

void MPScreenMgr::RemoveMPScreen(MPScreen*pMPScreen)
{
	m_MapMPScreen.erase(pMPScreen);
}

bool MPScreenMgr::HasMPScreen(int nScreenIndex)
{
	MapMPScreen::iterator iter=m_MapMPScreen.begin();
	while (iter!=m_MapMPScreen.end())
	{
		if (iter->second->GetScreenIndex()==nScreenIndex)
			return true;

		++iter;
	}

	return false;
}
