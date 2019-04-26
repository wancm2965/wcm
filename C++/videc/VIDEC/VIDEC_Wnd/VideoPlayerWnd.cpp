// VideoPlayerWnd.cpp: implementation of the VideoPlayerWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <WtsApi32.h>
#include <WinUser.h>
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "VideoPlayerWnd.h"
#include "XUtil.h"
#include "../VIDEC_API/XImage.h"
#include "../VIDEC_MP/MPScreen.h"
#include "Resource.h"
#include "IYUVDraw.h"
#include <emmintrin.h>
#include "VIDEC_CSP.h"

#define XUM_IMAGE_CHANGED	(WM_USER+100)

#pragma comment(lib,"WtsApi32.lib")

extern void InitMapScreenRect(void);
extern int GetActualScreenIndex(const RECT&rRect,int&nWidth,int&nHeight);
extern int GetScreenSameSize(int nIndex1,int nIndex2);


extern void YV12ToYV12(BYTE *pDst, BYTE *pSrc, DWORD width, DWORD height, DWORD pitch);


IMPLEMENT_DYNAMIC(VideoPlayerWnd, CWnd)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

VideoPlayerWnd::VideoPlayerWnd(YNCD3D_CB_DC cbdc,void*pUserData)
:m_hWndParent(NULL)

,m_pVideoDisplay(NULL)
,m_nActualWidth(0)
,m_nActualHeight(0)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_nFrameRate(30)
,m_bDoubleField(false)
,m_bEvenField(false)
,m_bOddField(false)

,m_bImageChanged(false)

,m_bShowWindow(false)
,m_bEnableD3D(true)

,m_bGotFirstVideo(false)
,m_bRunning(false)
,m_pIXPreProcess(NULL)
,m_cbdc(cbdc)
,m_pUserData(pUserData)
,m_nEnablePreview(0)
,m_nEnableVideo(1)
, m_nBgBmpWidth(96)
, m_nBgBmpHeight(96)
{
	m_nFrameCount=0;
	m_ulFrameCountTimestamp=XGetTimestamp();
	m_nLastWidth=0;

	m_nBrightness=VIDEC_GetDefaultBrightness();
	m_nContrast=VIDEC_GetDefaultContrast();
	m_nSaturation=VIDEC_GetDefaultSaturation();
	m_nHue=VIDEC_GetDefaultHue();
	m_nGamma=VIDEC_GetDefaultGamma();
	m_nSharpen=VIDEC_GetDefaultSharpen();
	m_nGammaR=VIDEC_GetDefaultGamma();
	m_nGammaG=VIDEC_GetDefaultGamma();
	m_nGammaB=VIDEC_GetDefaultGamma();

	m_nWndIndex=-1;
	m_nScreenIndex=-1;
	m_nFullScreen=0;

	m_bReset=true;
	m_nRedraw=0;
	m_nActualScreenIndex=-1;
	m_bActualScreenIndexChanged=false;

	m_pImage=NULL;

	m_pBitmapBG=NULL;
	m_nLastFrameRateTimestamp=0;
	m_nLastFrameRate=30;
	m_nLastFrameTimestamp=0;
	m_nLastFrameCount=0;
	m_nFrameIntervalCount=0;
	m_nLastSizingTimestamp=0;

	m_pICE=NULL;
	m_nLastGetImageTimestamp=0;

	m_pH264Player=(NULL);
	m_nWidth=1920;
	m_nHeight=1088;
	m_bTryingGPU=false;
	m_bUsingGPU=false;
	m_bH264PlayerFailed=false;
	m_bFirstDraw=false;
	m_nOldScreenIndex =-1;
	memset(m_sBgBmp, 0, sizeof(m_sBgBmp));
	m_nBgColorR = -1;
	m_nBgColorG = -1;
	m_nBgColorB = -1;
}

VideoPlayerWnd::~VideoPlayerWnd(void)
{
}

BEGIN_MESSAGE_MAP(VideoPlayerWnd, CWnd)
	//{{AFX_MSG_MAP(VideoPlayerWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(XUM_IMAGE_CHANGED,OnImageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VideoPlayerWnd message handlers



BOOL VideoPlayerWnd::OnEraseBkgnd(CDC* pDC) 
{
	if (!HasVideo())
	{
		bool bDrawNoVideoBG=false;
		CRect rc;
		GetClientRect(rc);

		if (m_sBgBmp[0] == 0 && (m_nBgColorR != -1 || m_nBgColorG != -1 || m_nBgColorB != -1))
		{
			pDC->FillRect(rc, &CBrush(RGB(m_nBgColorR, m_nBgColorG, m_nBgColorB)));
			return FALSE;
		}

		if (m_nBgColorR == -1 && m_nBgColorG == -1 && m_nBgColorB == -1)
		{
			m_nBgColorR = 0;
			m_nBgColorG = 0;
			m_nBgColorB = 0;
		}

		if (!m_pBitmapBG)
		{
			NEWRECORD( m_pBitmapBG,new CMyBitMap )
			NULLReturnRet( m_pBitmapBG , FALSE )  
			if (m_sBgBmp[0] == 0)
				m_pBitmapBG->SetBitmap(IDB_BITMAP_NO_VIDEO);
			else
				m_pBitmapBG->SetBitmap(m_sBgBmp, m_nBgBmpWidth, m_nBgBmpHeight);
		}

		int x = (rc.Width() - m_nBgBmpWidth) / 2;
		int y = (rc.Height() - m_nBgBmpHeight) / 2;

		if (m_pBitmapBG)
		{
			bDrawNoVideoBG=m_pBitmapBG->drawOnDcN( pDC->m_hDC,x,y);
		}

		if (bDrawNoVideoBG)
		{
			CRgn rgnBig;
			rgnBig.CreateRectRgnIndirect(rc);
			CRgn rgnSmall;
			rgnSmall.CreateRectRgn(x, y, x + m_nBgBmpWidth, y + m_nBgBmpHeight);
			rgnBig.CombineRgn(&rgnBig,&rgnSmall,RGN_DIFF);

			pDC->FillRgn(&rgnBig, &CBrush(RGB(m_nBgColorR, m_nBgColorG, m_nBgColorB)));
		}
		else
		{
			pDC->FillRect(rc, &CBrush(RGB(m_nBgColorR, m_nBgColorG, m_nBgColorB)));
		}
	}
	else
	{
		if (m_pBitmapBG)
		{
			delete m_pBitmapBG;
			m_pBitmapBG=NULL;
		}
	}
	m_nRedraw=1;

	return FALSE;
}

int VideoPlayerWnd::SetBgBmp(char* sFile)
{
	if (m_sBgBmp[0] == 0)
	{
		memcpy(m_sBgBmp, sFile, strlen(sFile) + 1);
		return 0;
	}
	return -1;
}

int VideoPlayerWnd::SetBgRgb(int r, int g, int b)
{
	if (m_nBgColorR == -1 && m_nBgColorG == -1 && m_nBgColorB == -1)
	{
		if (r > 255 || r < 0)
			m_nBgColorR = 0;
		else
			m_nBgColorR = r;

		if (g > 255 || g < 0)
			m_nBgColorG = 0;
		else
			m_nBgColorG = g;

		if (b > 255 || b < 0)
			m_nBgColorB = 0;
		else
			m_nBgColorB = b;

		return 0;
	}
	return -1;
}

void VideoPlayerWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	ResizeControls();

	//if (m_pH264Player)
	//{
	//	m_pH264Player->Resize();
	//}

	m_nRedraw=1;
}

void VideoPlayerWnd::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		{
			m_bImageChanged=false;
			m_nRedraw=0;

			{
				if (m_pH264Player)
				{
					m_pH264Player->Close();
					delete m_pH264Player;
					m_pH264Player=NULL;
				}
			}
			
			if (H264PlayerTry(NULL,m_nWidth,m_nHeight) == 0)
			{
				if (m_pVideoDisplay)
				{
					m_pVideoDisplay->Close();
					delete m_pVideoDisplay;
					m_pVideoDisplay=NULL;
				}
			}

			NEWRECORD( m_pH264Player,new H264Player() )
			NULLReturn( m_pH264Player )  
			if (0!=m_pH264Player->Open(m_cbdc,m_pUserData,m_hWnd,m_nWidth,m_nHeight))
			{
				m_pH264Player->Close();
				delete m_pH264Player;
				m_pH264Player=NULL;

				m_bH264PlayerFailed=true;
				m_bTryingGPU=false;
				m_bUsingGPU=false;
			}
			else
			{
				m_bH264PlayerFailed=false;
				m_bTryingGPU=false;
				m_bUsingGPU=true;
			}

			m_bFirstDraw=false;

			m_nActualWidth=m_nWidth;
			m_nActualHeight=m_nHeight;

			KillTimer(nIDEvent);			
			::PostMessage(m_hWndParent,WM_SIZE,0,0);
			SetTimer(1000,1000,NULL);
		}
		break;
	case 1000:
		{
			KillTimer(nIDEvent);
			::PostMessage(m_hWndParent,WM_SIZE,0,0);
		}
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

LRESULT VideoPlayerWnd::OnImageChanged(WPARAM wParam, LPARAM lParam)
{
	if (wParam==102)
	{
		::PostMessage(m_hWndParent,WM_SIZE,0,0);
		return 0L;
	}


	if (!m_bImageChanged)
	{
		return 0L;
	}

	if (m_pVideoDisplay!=NULL)
	{
		if (!m_bActualScreenIndexChanged)
		{
			m_bImageChanged=false;

			::PostMessage(m_hWndParent,WM_SIZE,0,0);

			return 0L;
		}
	}

	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->Close();
		delete m_pVideoDisplay;
		m_pVideoDisplay=NULL;
	}

	if (m_nEnableVideo==0)
	{
		m_nActualWidth=0;
		m_nActualHeight=0;
		m_bImageChanged=false;
		return 0L;
	}


	m_pVideoDisplay=VideoDisplay::Create(m_cbdc,m_pUserData);

	if (m_pVideoDisplay)
	{
		if (0!=m_pVideoDisplay->Open(m_hWnd))
		{
			m_pVideoDisplay->Close();
			delete m_pVideoDisplay;
			m_pVideoDisplay=NULL;
		}
	}

	if (m_pVideoDisplay==NULL)
	{
		return 0;
	}
	{
		m_nRedraw=0;
		if (m_pH264Player)
		{
			m_pH264Player->Close();
			delete m_pH264Player;
			m_pH264Player=NULL;
		}
	}


	m_bActualScreenIndexChanged=false;
	m_bImageChanged=false;

	ResizeControls();

	::PostMessage(m_hWndParent,WM_SIZE,0,0);

	SetTimer(1000,1000,NULL);

	return 0;
}




void VideoPlayerWnd::ResizeControls(void)
{

	{
		int nWidth=1920;
		int nHeight=1080;
		CRect rc;
		GetWindowRect(rc);

		if (m_pVideoDisplay && m_pVideoDisplay->GetVideoShowType() == 3 && abs(rc.Width() - m_nLastWidth) > 100 && GetVideoWidth() >= 1280)
		{
			m_nLastWidth=rc.Width();
			m_bActualScreenIndexChanged=true;
		}

	//	int nActualScreenIndex=GetActualScreenIndex(rc,nWidth,nHeight);
	//	if (nActualScreenIndex!=m_nActualScreenIndex)
	//	{
	//		if (GetScreenSameSize(m_nActualScreenIndex,nActualScreenIndex)==0)
	//		{
	//			m_nActualScreenIndex=nActualScreenIndex;
	//			m_bActualScreenIndexChanged=true;
	//		}
	//		else
	//		{
	//			m_nActualScreenIndex=nActualScreenIndex;
	//			m_bActualScreenIndexChanged=true;
	//		}

	//		if (m_pH264Player==NULL)
	//		{
	//			m_bH264PlayerFailed=false;
	//		}
	//	}
	}
}

int VideoPlayerWnd::Open(HWND hwndParent)
{
	m_hWndParent=hwndParent;

	CRect rc;
	::GetClientRect(m_hWndParent,rc);
	if (!CreateEx(0,AfxRegisterWndClass(CS_CLASSDC), _T("VideoPlayerWnd"),WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_DISABLED ,0, 0,1920, 1080/*rc.left,rc.top,rc.Width(),rc.Height()*/,m_hWndParent,NULL))
	{
		return -1;
	}

	ImageInput::Open();

	m_pIXPreProcess=CreateIXPreProcess(*this);
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),VIDEC_GetDefaultSharpen());
	}

	WTSRegisterSessionNotification(m_hWnd,NOTIFY_FOR_THIS_SESSION);

	return 0;
}

void VideoPlayerWnd::Close(void)
{
	WTSUnRegisterSessionNotification(m_hWnd);

	ImageInput::Close();

	{
		m_nRedraw=0;
		if (m_pH264Player)
		{
			m_pH264Player->Close();
			delete m_pH264Player;
			m_pH264Player=NULL;
		}
	}


	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->Close();
		delete m_pVideoDisplay;
		m_pVideoDisplay=NULL;
	}

	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->ReleaseConnections();
		delete m_pIXPreProcess;
		m_pIXPreProcess=NULL;
	}

	if (m_pBitmapBG)
	{
		delete m_pBitmapBG;
		m_pBitmapBG=NULL;
	}

	{
		XAutoLock l(m_csImage);
		if (m_pImage!=NULL)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
	}

	if (m_pICE)
	{
		m_pICE->Close();
		delete m_pICE;
		m_pICE=NULL;
	}

	DestroyWindow();
}

int VideoPlayerWnd::GetVideoWidth(void)
{
	return (m_nVirtualWidth!=0?m_nVirtualWidth:m_nActualWidth);
}

int VideoPlayerWnd::GetVideoHeight(void)
{
	int nWidth=(m_nVirtualWidth!=0?m_nVirtualWidth:m_nActualWidth);
	int nHeight=(m_nVirtualHeight!=0?m_nVirtualHeight:m_nActualHeight);
	if ((nWidth==352 && nHeight==144) || (nWidth==704 && nHeight==288)|| (nWidth==1920 && nHeight== 540) )
	{
		nHeight*=2;
	}
	return nHeight;
}


HWND VideoPlayerWnd::GetWnd(void)
{
	return m_hWnd;
}

bool VideoPlayerWnd::IsWindowVisible(void)
{
	return CWnd::IsWindowVisible()?true:false;
}

//char globeszDebug[1024];

bool VideoPlayerWnd::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
 	y-=2;
 	cy+=4;

	nFlags|=SWP_SHOWWINDOW;
	BOOL bRet=CWnd::SetWindowPos(CWnd::FromHandle(hWndInsertAfter),x,y,cx,cy,nFlags);

	if (!(nFlags&SWP_NOSIZE))
	{
		ResizeControls();
	}



	//sprintf(globeszDebug,"VideoPlayerWnd SetWindowPos th:%p, x= %u, y= %u, cx= %u, cy= %u,nFlags= %d, bRet= %d\n", this, x,  y,  cx, cy ,nFlags,bRet);
	//DebugOutput(globeszDebug);


	return bRet?true:false;
}


//设置视频属性
int VideoPlayerWnd::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	m_nBrightness=nBrightness;
	m_nContrast=nContrast;
	m_nSaturation=nSaturation;
	m_nHue=nHue;
	m_nGamma=nGamma;
	m_nSharpen=nSharpen;

	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}

	return 0;
}

int VideoPlayerWnd::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	m_nGammaR=nGammaR;
	m_nGammaG=nGammaG;
	m_nGammaB=nGammaB;
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetVideoGammaRGB(m_nGammaR,m_nGammaG,m_nGammaB);
	}

	return 0;
}


bool VideoPlayerWnd::CaptureToBMPFile(const char*cszFile)
{
	if (cszFile==NULL || strlen(cszFile)==0)
	{
		return false;
	}
	
	if (m_pH264Player)
	{
		return m_pH264Player->CaptureToBMPFile(cszFile);
	}

	bool bRet=false;
	VIDEC_Image*pImage=NULL;

	{
		XAutoLock l(m_csImage);

		if (m_pImage!=NULL)
		{
			m_pImage->AddRef();
			pImage=m_pImage;
		}
	}

	if (pImage!=NULL)
	{
		XImage*pXImage=dynamic_cast<XImage*>(pImage);
		if (pXImage!=NULL)
		{
			bRet=IYUVDraw::SaveToFileJPG(pXImage->GetActualWidth(),pXImage->GetActualHeight(),pXImage->GetVirtualWidth(),pXImage->GetVirtualHeight(),(unsigned char*)pXImage->GetData(),pXImage->GetDataLen(),cszFile);
		}

		pImage->Release();
		pImage=NULL;
	}

	return bRet;
}


double VideoPlayerWnd::GetAverageFPS(void)
{
	double fFPS=0.0;
	unsigned long ulTimestamp=XGetTimestamp();
	unsigned long ulDelta=ulTimestamp-m_ulFrameCountTimestamp;
	if (ulDelta)
	{
		fFPS=(double)m_nFrameCount*1000.00/(double)ulDelta;
	}
	m_ulFrameCountTimestamp=ulTimestamp;
	m_nFrameCount=0;
	return fFPS;
}

bool VideoPlayerWnd::DisplayImage(XImage*pXImage)
{
	{
		int nMaxCount=2;
		if (pXImage->GetTimestampOffset())
		{
			nMaxCount=6;
		}


		m_nLastFrameCount++;

		unsigned long nFrameTimestamp=pXImage->GetTimestamp();
		if (m_nLastFrameTimestamp!=0)
		{
			unsigned long nInterval=nFrameTimestamp-m_nLastFrameTimestamp;
			int nValue=0;
			MapFrameInterval::iterator iter=m_MapFrameInterval.find(nInterval);
			if (iter!=m_MapFrameInterval.end())
			{
				nValue=iter->second;
			}
			m_MapFrameInterval[nInterval]=(nValue+1);
			m_nFrameIntervalCount+=1;
		}
		m_nLastFrameTimestamp=nFrameTimestamp;

		unsigned long nTimestamp=XGetTimestamp();
		if (m_nLastFrameRateTimestamp==0)
		{
			m_nLastFrameRateTimestamp=nTimestamp;
		}
		unsigned long nDuration=nTimestamp-m_nLastFrameRateTimestamp;
		if (nDuration>=10000 && m_nFrameIntervalCount>=MAX_FRAME_INTERVAL_COUNT)
		{
			if (m_nFrameIntervalCount>=6)
			{
				int nSkipCount=m_nFrameIntervalCount/3;
				int nFrameCount=0;
				int nIndex=0;
				int nTotalValue=0;
				unsigned long nFrameInterval=0;
				MapFrameInterval::iterator iter=m_MapFrameInterval.begin();
				while (iter!=m_MapFrameInterval.end())
				{
					nIndex+=1;
					int nValue=iter->second;
					if ((nValue+nTotalValue)>nSkipCount && ((nValue+nTotalValue)<=(m_nFrameIntervalCount-nSkipCount)))
					{
						nFrameInterval+=(iter->first*nValue);
						nFrameCount+=nValue;
					}
					nTotalValue+=nValue;

					++iter;
				}

				if (nFrameCount)
				{
					nFrameInterval=nFrameInterval/nFrameCount;
					if (nFrameInterval)
					{
						int nFrameRate=1000/nFrameInterval;

						//char szDebug[1024];
						//sprintf(szDebug,"nFrameRate=%d nFrameRate=%d nFrameRate=%d\n",nFrameRate,nFrameRate,nFrameRate);
						//DebugOutput(szDebug);

						if (nFrameRate>=55)
							nFrameRate=60;
						else if (nFrameRate>=40)
							nFrameRate=50;
						else if (nFrameRate>=20)
							nFrameRate=30;
						else if (nFrameRate>=12)
							nFrameRate=15;
						else if (nFrameRate>=7)
							nFrameRate=10;
						else if (nFrameRate>=3)
							nFrameRate=5;
						else 
							nFrameRate=1;
						m_nLastFrameRate=nFrameRate;
					}
				}
			}

			m_nLastFrameCount=0;
			m_nLastFrameRateTimestamp=nTimestamp;
			m_nFrameIntervalCount=0;
			m_MapFrameInterval.clear();
		}
	}

	if (m_nEnableVideo==0)
	{
		m_nActualWidth=0;
		m_nActualHeight=0;
		m_bImageChanged=false;
		return true;
	}

	{
		if (m_pVideoDisplay==NULL)
		{
			if (!m_bImageChanged)
			{
				m_nActualWidth=pXImage->GetActualWidth();
				m_nActualHeight=pXImage->GetActualHeight();
				m_nVirtualWidth=pXImage->GetVirtualWidth();
				m_nVirtualHeight=pXImage->GetVirtualHeight();

				if (m_nActualWidth!=pXImage->GetActualWidth() ||
					m_nActualHeight!=pXImage->GetActualHeight())
				{
					m_nLastFrameRateTimestamp=0;
					m_nLastFrameRate=30;
					m_nLastFrameTimestamp=0;
					m_nLastFrameCount=0;						
					m_nFrameIntervalCount=0;
					m_MapFrameInterval.clear();
				}

				m_nFrameRate=m_nLastFrameRate;

				m_bImageChanged=true;

				PostMessage(XUM_IMAGE_CHANGED);

				return true;
			}
		}

		if (m_nVirtualWidth!=pXImage->GetVirtualWidth() ||
			m_nVirtualHeight!=pXImage->GetVirtualHeight())
		{
			m_nVirtualWidth=pXImage->GetVirtualWidth();
			m_nVirtualHeight=pXImage->GetVirtualHeight();

			unsigned long nTimestamp=XGetTimestamp();
			if (nTimestamp-m_nLastSizingTimestamp>1000)
			{
				::PostMessage(m_hWndParent,WM_SIZE,0,0);
				m_nLastSizingTimestamp=nTimestamp;
			}
		}
	}

	{
		if (m_bImageChanged || m_bActualScreenIndexChanged)
		{
			m_bImageChanged=true;
			PostMessage(XUM_IMAGE_CHANGED);

			return true;
		}
	}

	if (m_pVideoDisplay)
	{
		m_nActualWidth=pXImage->GetActualWidth();
		m_nActualHeight=pXImage->GetActualHeight();
		int nVirtualWidth=pXImage->GetVirtualWidth();
		int nVirtualHeight=pXImage->GetVirtualHeight();

		if (m_nVirtualWidth!=nVirtualWidth ||
			m_nVirtualHeight!=nVirtualHeight)
		{
			m_nVirtualWidth=nVirtualWidth;
			m_nVirtualHeight=nVirtualHeight;

			::PostMessage(m_hWndParent,WM_SIZE,0,0);
		}

		m_pVideoDisplay->DisplayImage(pXImage);
		VIDEC_Image*pTempImage=NULL;
		pXImage->AddRef();
		{
			XAutoLock l(m_csImage);
			pTempImage=m_pImage;
			m_pImage=pXImage;
		}
		if (pTempImage)
		{
			pTempImage->Release();
			pTempImage=NULL;
		}	
	}

	return true;
}
int VideoPlayerWnd::GetVideoShowFrameRate(void)
{
	if (m_pVideoDisplay)
	{
		return m_pVideoDisplay->GetVideoShowFrameRate();
	}
	else if (m_pH264Player)
	{
		return m_pH264Player->GetFrameRate();
	}
	return 0;
}
int VideoPlayerWnd::GetVideoShowType(void)
{
	if (m_pVideoDisplay)
	{
		return m_pVideoDisplay->GetVideoShowType();
	}
	else if (m_pH264Player)
	{
		return m_pH264Player->GetShowType();
	}
	return 0;
}

void VideoPlayerWnd::PausePlay(bool bPause,bool bClear)
{
	if (m_pVideoDisplay)
	{
		m_pVideoDisplay->PausePlay(bPause,bClear);
	}
}

void VideoPlayerWnd::SetEnableD3D(bool bEnabled)
{
	if (m_bEnableD3D != bEnabled)
	{
		if (m_pVideoDisplay)
		{
			m_bEnableD3D=bEnabled;
			m_pVideoDisplay->SetEnableD3D(bEnabled);
		}
	}
}
void VideoPlayerWnd::OnReceivedPreSend(int& nSend)
{
	if (!IsWindowVisible())
	{
		return;
	}

	RECT rect;
	GetClientRect(&rect);
	if (IsRectEmpty(&rect))
	{
		return;
	}
	nSend=1;
}
void VideoPlayerWnd::OnReceivedImage(VIDEC_Image*pImage)
{
	m_bGotFirstVideo=true;

	if (m_bReset)
	{
		m_bReset=false;
	}
	else
	{
		if (GetSupportPreview()==0)
		{
			return ;
		}
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
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (pXImage->GetDoubleField())
	{
		DoubleFrameRate::Process(pXImage);
	}
	else
	{
		OnFrameData(pXImage);
	}
}

void VideoPlayerWnd::OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (VIDEC_GetEnableImageColorEnhance() && pImage->GetNV12()==false)
	{
		if (m_pICE==NULL)
		{
			NEWRECORD( m_pICE,new ImageColorEnhance() )
			NULLReturn( m_pICE )  
			if (0!=m_pICE->Open())
			{
				m_pICE->Close();
				delete m_pICE;
				m_pICE=NULL;
			}
		}
	}
	else
	{
		if (0!=m_pICE)
		{
			m_pICE->Close();
			delete m_pICE;
			m_pICE=NULL;
		}
	}

	if (m_pICE)
	{
		int nWidth=pXImage->GetActualWidth();
		int nHeight=pXImage->GetActualHeight();

		unsigned char*pY=(unsigned char*)pXImage->GetData();
		unsigned char*pU=pY+nWidth*nHeight;
		unsigned char*pV=pU+nWidth*nHeight/4;

		m_pICE->Process(pY,pU,pV,nWidth,nHeight);
	}

	DisplayImage(pXImage);
}

void VideoPlayerWnd::OnFrameData(XImage*pXImage)
{
	m_nFrameCount++;


	int nWidth=pXImage->GetActualWidth();
	int nHeight=pXImage->GetActualHeight();
	if (nWidth==352 && (nHeight==480 || nHeight==576))
	{
		pXImage->SetVirtualWidth(nWidth<<1);
		pXImage->SetVirtualHeight(nHeight);
	}

	if (m_pIXPreProcess!=NULL && pXImage->GetNV12()==false)
	{
		m_pIXPreProcess->PreProcess(pXImage);
	}
	else
	{
		OnIXPreProcessNotifyOutputData(pXImage);
	}
}


bool VideoPlayerWnd::HasVideo(void)
{
	//bool bHasVideo=((m_pVideoDisplay!=NULL) && m_bGotFirstVideo);

	if (!m_bGotFirstVideo)
	{
		int nShowFrameRate=0;
		if (m_pVideoDisplay)
		{
			nShowFrameRate = m_pVideoDisplay->GetVideoShowFrameRate();
		}
		else if (m_pH264Player)
		{
			nShowFrameRate = m_pH264Player->GetFrameRate();
		}
		m_bGotFirstVideo = (nShowFrameRate > 0);
	}
	return m_bGotFirstVideo;
}

void VideoPlayerWnd::SetEnableVideo(int nEnable)
{
	m_nEnableVideo=nEnable;
	if (m_nEnableVideo==0)
	{
		if (m_pVideoDisplay)
		{
			m_pVideoDisplay->Close();
			delete m_pVideoDisplay;
			m_pVideoDisplay=NULL;

			m_nActualWidth=0;
			m_nActualHeight=0;
			m_bImageChanged=false;

			Invalidate(TRUE);
		}
	}
}

int VideoPlayerWnd::GetEnableVideo(void)
{
	return m_nEnableVideo;
}


/******************************************************************************
* SetScreenWnd
*描述：设置所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void VideoPlayerWnd::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	//add use xiaofang ,20160923
//#define  EnableDrawReopen 1 

#ifdef EnableDrawReopen  
	if(m_nScreenIndex >=0 && m_nOldScreenIndex !=nScreenIndex && m_pVideoDisplay)
	{
		m_pVideoDisplay->ScreenChangeReopenDraw();
		m_nOldScreenIndex = nScreenIndex;
	}
#endif

	m_nScreenIndex=nScreenIndex;
	m_nWndIndex=nWndIndex;
	m_nFullScreen=nFullScreen;



	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}

void VideoPlayerWnd::EnablePreview(int nPreview)
{
	m_nEnablePreview=nPreview;
}

int VideoPlayerWnd::GetSupportPreview(void)
{
	if (m_nEnablePreview)
	{
		return 1;
	}

	if (GetSafeHwnd() && IsWindowVisible()==false)
	{
		if (MPScreenMgr::Instance().HasMPScreen(m_nScreenIndex)==false)
		{
			return 0;
		}
	}

	return 1;
}

void VideoPlayerWnd::Reset(void)
{
	m_bReset=true;
}

void VideoPlayerWnd::ResetImage(VIDEC_Image*pImage)
{
	m_bGotFirstVideo=true;
	m_bReset=true;
	int	nEnableVideo=m_nEnableVideo;
	m_nEnableVideo=1;

	m_nActualWidth=0;
	m_nActualHeight=0;

	OnReceivedImage(pImage);

	OnImageChanged(0,0);

	m_nEnableVideo=nEnableVideo;
}

VIDEC_Image*VideoPlayerWnd::GetImage(void)
{
	XAutoLock l(m_csImage);

	unsigned long nTimestamp=XGetTimestamp();
	m_nLastGetImageTimestamp=nTimestamp;

	if (m_pImage!=NULL)
	{
		m_pImage->AddRef();
		return m_pImage;
	}

	return NULL;
}

void VideoPlayerWnd::TurnOnGPU(int nWidth,int nHeight)
{
	if ((!m_bUsingGPU && !m_bTryingGPU && !m_pH264Player) ||
		m_nWidth!=nWidth || m_nHeight!=nHeight)
	{
		m_bTryingGPU=true;
		m_nWidth=nWidth;
		m_nHeight=nHeight;
		SetTimer(1,1,NULL);
	}
}

void VideoPlayerWnd::TurnOffGPU(void)
{
	if (m_bUsingGPU || m_bTryingGPU)
	{
		m_bTryingGPU=false;
		m_bUsingGPU=false;
	}
}

bool VideoPlayerWnd::CanUseGPU(int nWidth,int nHeight,int nVirtualWidth,int nVirtualHeight)
{
	if (m_bH264PlayerFailed || (nWidth==0 || nHeight==0))
	{
		return false;
	}

	if ((m_nVirtualWidth!=nVirtualWidth) || (m_nVirtualHeight!=nVirtualHeight))
	{
		m_nVirtualWidth=nVirtualWidth;
		m_nVirtualHeight=nVirtualHeight;

		::PostMessage(m_hWndParent,WM_SIZE,0,0);
	}

	if (m_pH264Player)
	{
		if (m_nWidth!=nWidth || m_nHeight!=nHeight)
		{
			return (H264PlayerTry(m_pH264Player->GetPlayerHandle(),nWidth,nHeight)==0);
		}
	}
	else
	{
		return (H264PlayerTry(NULL,nWidth,nHeight)==0);
	}

	return true;
}

bool VideoPlayerWnd::IsTryingGPU(void)
{
	return m_bTryingGPU;
}

bool VideoPlayerWnd::IsUsingGPU(void)
{
	return m_bUsingGPU;
}

int VideoPlayerWnd::PlayFrame(const unsigned char *pFrameData, int nFrameLen,int nKeyFrame,int nFrameRate)
{
	if (m_pH264Player)
	{
		m_nRedraw=0;
		int nFlagReset=nKeyFrame;
		m_pH264Player->PlayFrame(pFrameData,nFrameLen,nFrameRate,nFlagReset);
		if (nFlagReset && nKeyFrame)
		{
			m_nWidth=m_pH264Player->GetWidth();
			m_nHeight=m_pH264Player->GetHeight();

			m_bH264PlayerFailed=true;
			m_bTryingGPU=true;
			SetTimer(1,1,NULL);
		}
		else
		{
			if (!m_bFirstDraw)
			{
				m_bFirstDraw=true;
				::PostMessage(m_hWndParent,WM_SIZE,0,0);
			}
		}
	}

	return 0;
}

void VideoPlayerWnd::SkipOneFrame(void)
{
	if (m_pH264Player)
	{
		m_pH264Player->SkipDisplay(0x10000000);
	}
}

void VideoPlayerWnd::RepeatFrame(void)
{
	if (m_nRedraw> 0)
	{
		if (m_nRedraw++ > 4)
		{
			m_nRedraw=0;
			if (m_pH264Player)
			{
				m_pH264Player->SkipDisplay(-1);
			}		
		}
	}
}
