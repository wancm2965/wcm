// LocalVideoWnd.cpp: implementation of the LocalVideoWnd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <WtsApi32.h>
#include <WinUser.h>
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "LocalVideoWnd.h"
#include "XUtil.h"
#include "../VIDEC_API/XImage.h"
#include "../VIDEC_MP/MPScreen.h"
#include "Resource.h"
#include "IYUVDraw.h"

#define XUM_IMAGE_CHANGED	(WM_USER+100)


extern void InitMapScreenRect(void);
extern int GetActualScreenIndex(const RECT&rRect,int&nWidth,int&nHeight);
extern int GetScreenSameSize(int nIndex1,int nIndex2);

extern bool GetTryingEVR(void);
extern void SetEVRFailed(void);


VIDEC_Wnd*VIDEC_Wnd::CreateLocalVideo(void)
{
	LocalVideoWnd*pXVideoWindow=new LocalVideoWnd();
	return pXVideoWindow;
}

VIDEC_API VIDEC_Wnd*VIDEC_Wnd::Create(void)
{
	return CreateLocalVideo();
}

IMPLEMENT_DYNAMIC(LocalVideoWnd, CWnd)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LocalVideoWnd::LocalVideoWnd(void)
:XImageSink(this)
,m_hWndParent(NULL)

,m_pVideoDraw(NULL)
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


,m_bGotFirstVideo(false)

,m_nEnablePreview(0)
{
	m_nFrameCount=0;
	m_ulFrameCountTimestamp=XGetTimestamp();

	m_nWndIndex=-1;
	m_nScreenIndex=-1;
	m_nFullScreen=0;

	m_bReset=true;

	m_nActualScreenIndex=-1;
	m_bActualScreenIndexChanged=false;
	m_nActualScreenWidth=0;
	m_nActualScreenHeight=0;

	m_pImage=NULL;

	m_bDrawFailed=false;
	m_bForceDestroy=false;
	m_bWTLocked=false;
	m_nWTUnlockedDrawFailedCount=0;
	m_nLastWTUnLockedTimestamp=0;
	m_nFirstDrawFailedTimestamp=0;

	m_pBitmapBG=NULL;
	m_nLastSizingTimestamp=0;

	m_pICE=NULL;
}

LocalVideoWnd::~LocalVideoWnd(void)
{
}

BEGIN_MESSAGE_MAP(LocalVideoWnd, CWnd)
	//{{AFX_MSG_MAP(LocalVideoWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_MESSAGE(XUM_IMAGE_CHANGED,OnImageChanged)
	ON_MESSAGE(WM_WTSSESSION_CHANGE,OnWTSessionChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LocalVideoWnd message handlers
LRESULT LocalVideoWnd::OnWTSessionChanged(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case WTS_SESSION_LOCK:
		m_nWTUnlockedDrawFailedCount=0;
		m_bWTLocked=true;
		break;
	case WTS_SESSION_UNLOCK:
		m_nWTUnlockedDrawFailedCount=0;
		m_bWTLocked=false;
		m_nLastWTUnLockedTimestamp=XGetTimestamp();
		break;
	}

	return 0;
}

BOOL LocalVideoWnd::OnEraseBkgnd(CDC* pDC) 
{
#define NO_VIDEO_BITMAP_WIDTH 96
#define NO_VIDEO_BITMAP_HEIGHT 96

	if (!m_bGotFirstVideo || m_pVideoDraw==NULL)
	{
		bool bDrawNoVideoBG=false;
		CRect rc;
		GetClientRect(rc);

		if (!m_pBitmapBG)
		{
			m_pBitmapBG=new CMyBitMap;
			m_pBitmapBG->SetBitmap(IDB_BITMAP_NO_VIDEO);
		}

		int x=(rc.Width()-NO_VIDEO_BITMAP_WIDTH)/2;
		int y=(rc.Height()-NO_VIDEO_BITMAP_HEIGHT)/2;

		if (m_pBitmapBG)
		{
			bDrawNoVideoBG=m_pBitmapBG->drawOnDcN( pDC->m_hDC,x,y);
		}

		if (bDrawNoVideoBG)
		{
			CRgn rgnBig;
			rgnBig.CreateRectRgnIndirect(rc);
			CRgn rgnSmall;
			rgnSmall.CreateRectRgn(x,y,x+NO_VIDEO_BITMAP_WIDTH,y+NO_VIDEO_BITMAP_HEIGHT);
			rgnBig.CombineRgn(&rgnBig,&rgnSmall,RGN_DIFF);

			pDC->FillRgn(&rgnBig,&CBrush(RGB(0,0,0)));
		}
		else
		{
			pDC->FillRect(rc,&CBrush(RGB(0,0,0)));
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

	return FALSE;
}


void LocalVideoWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (m_pVideoDraw!=NULL)
	{
		m_pVideoDraw->Repaint();
	}
}

void LocalVideoWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	ResizeControls();
}


void LocalVideoWnd::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	ResizeControls();
}

void LocalVideoWnd::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	ResizeControls();

	CWnd::OnTimer(nIDEvent);
}

LRESULT LocalVideoWnd::OnImageChanged(WPARAM wParam, LPARAM lParam)
{
	if (!m_bImageChanged)
	{
		return 0L;
	}

	{
		int nWidth=1920;
		int nHeight=1080;
		CRect rc;
		GetWindowRect(rc);
		int nActualScreenIndex=GetActualScreenIndex(rc,nWidth,nHeight);
		if (nActualScreenIndex<0 || !IsWindowVisible())
		{
			return 0L;
		}
	}

	if (m_pVideoDraw!=NULL && !m_bForceDestroy)
	{
		if (m_pVideoDraw->GetWidth()==m_nActualWidth &&
			m_pVideoDraw->GetHeight()==m_nActualHeight && 
			!m_bActualScreenIndexChanged)
		{
			m_bImageChanged=false;

			::PostMessage(m_hWndParent,WM_SIZE,0,0);

			return 0L;
		}
	}

	if (m_pVideoDraw)
	{
		m_pVideoDraw->Close();
		delete m_pVideoDraw;
		m_pVideoDraw=NULL;
	}

	m_bForceDestroy=false;
	m_nWTUnlockedDrawFailedCount=0;

	bool bUsedEVR=GetTryingEVR();
	m_pVideoDraw=AVCONAVC_VideoDraw::Create();

	if (m_pVideoDraw)
	{
		if (0!=m_pVideoDraw->Open(m_hWnd,m_nActualWidth,m_nActualHeight,false,bUsedEVR))
		{
			m_pVideoDraw->Close();
			delete m_pVideoDraw;
			m_pVideoDraw=NULL;

			SetEVRFailed();
		}
	}

	if (m_pVideoDraw==NULL)
	{
		return 0;
	}

	if (!bUsedEVR)
	{
		SetEVRFailed();
	}

	m_bActualScreenIndexChanged=false;
	m_bImageChanged=false;

	ResizeControls();

	::PostMessage(m_hWndParent,WM_SIZE,0,0);

	SetTimer(10000,10000,NULL);

	return 0;
}


void LocalVideoWnd::ResizeControls(void)
{
	{
		CRect rc;
		GetClientRect(rc);
		int x=0;
		int y=0;
		int cx=rc.Width();
		int cy=rc.Height();

		if (m_pVideoDraw)
		{
			m_pVideoDraw->SetWndPos(x,y,cx,cy);
		}
	}

	{
		int nWidth=1920;
		int nHeight=1080;
		CRect rc;
		GetWindowRect(rc);
		int nActualScreenIndex=GetActualScreenIndex(rc,nWidth,nHeight);
		if (nActualScreenIndex!=m_nActualScreenIndex)
		{
			m_nActualScreenIndex=nActualScreenIndex;
			m_bActualScreenIndexChanged=true;
			if (m_nActualScreenIndex==-1 || !IsWindowVisible())
			{
				m_nActualWidth=0;
				m_nActualHeight=0;
			}
		}
	}
}

int LocalVideoWnd::Open(HWND hwndParent,bool bLocal,unsigned long*lpnVidFrameTimestamp,unsigned long*lpnAudFrameTimestamp)
{
	m_hWndParent=hwndParent;

	CRect rc;
	::GetClientRect(m_hWndParent,rc);
	if (!CreateEx(0,AfxRegisterWndClass(CS_CLASSDC), _T("LocalVideoWnd"),WS_CHILD|WS_CLIPCHILDREN|WS_DISABLED ,rc.left,rc.top,rc.Width(),rc.Height(),m_hWndParent,NULL))
	{
		return -1;
	}

	ImageInput::Open();


	WTSRegisterSessionNotification(m_hWnd,NOTIFY_FOR_THIS_SESSION);

	return 0;
}

void LocalVideoWnd::Close(void)
{
	WTSUnRegisterSessionNotification(m_hWnd);
	ImageInput::Close();


	XImageSink::Close();

	if (m_pVideoDraw)
	{
		m_pVideoDraw->Close();
		delete m_pVideoDraw;
		m_pVideoDraw=NULL;
	}

	{
		XAutoLock l(m_csImage);
		if (m_pImage!=NULL)
		{
			m_pImage->Release();
			m_pImage=NULL;
		}
	}

	if (m_pBitmapBG)
	{
		delete m_pBitmapBG;
		m_pBitmapBG=NULL;
	}


	DestroyWindow();
}

int LocalVideoWnd::GetVideoWidth(void)
{
	return (m_nVirtualWidth!=0?m_nVirtualWidth:m_nActualWidth);
}

int LocalVideoWnd::GetVideoHeight(void)
{
	return (m_nVirtualHeight!=0?m_nVirtualHeight:m_nActualHeight);
}


HWND LocalVideoWnd::GetWnd(void)
{
	return m_hWnd;
}

bool LocalVideoWnd::IsWindowVisible(void)
{
	return CWnd::IsWindowVisible()?true:false;
}

bool LocalVideoWnd::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	nFlags|=SWP_SHOWWINDOW;
	BOOL bRet=CWnd::SetWindowPos(CWnd::FromHandle(hWndInsertAfter),x,y,cx,cy,nFlags);

	if (!(nFlags&SWP_NOSIZE))
	{
		ResizeControls();
	}

	return bRet?true:false;
}


//设置视频属性
int LocalVideoWnd::SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	return 0;
}

int LocalVideoWnd::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	return 0;
}

void LocalVideoWnd::SetLevelFixer(int nLevelFixer)
{
}

int LocalVideoWnd::GetLevelFixer(void)
{
	return -1;
}

void LocalVideoWnd::SetSynFlag(int nSynFlag)
{
}

bool LocalVideoWnd::CaptureToBMPFile(const char*cszFile)
{
	if (cszFile==NULL || strlen(cszFile)==0)
	{
		return false;
	}

	bool bRet=false;
	VIDEC_Image*pImage=GetImage();
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


double LocalVideoWnd::GetAverageFPS(void)
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

bool LocalVideoWnd::DisplayImage(XImage*pXImage)
{
	{
		XAutoLock l(m_csImage);
		if (pXImage)
		{
			if (m_pImage)
			{
				m_pImage->Release();
			}
			m_pImage=pXImage;
			m_pImage->AddRef();
		}

	}

	if (m_bDrawFailed)
	{
		m_nFirstDrawFailedTimestamp=0;
		m_nWTUnlockedDrawFailedCount=0;
		m_bDrawFailed=false;
		m_bForceDestroy=true;

		m_bImageChanged=true;
		PostMessage(XUM_IMAGE_CHANGED);

		return true;
	}


	{
		if (m_nActualWidth!=pXImage->GetActualWidth() ||
			m_nActualHeight!=pXImage->GetActualHeight())
		{
			if (!m_bImageChanged)
			{
				m_nActualWidth=pXImage->GetActualWidth();
				m_nActualHeight=pXImage->GetActualHeight();
				m_nVirtualWidth=pXImage->GetVirtualWidth();
				m_nVirtualHeight=pXImage->GetVirtualHeight();
				m_nFrameRate=pXImage->GetFrameRate();

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
		}
	}

	if (m_bImageChanged==false)
	{
		if (m_pVideoDraw)
		{
			m_pVideoDraw->Draw((unsigned char*)pXImage->GetData(),pXImage->GetActualWidth());
		}
	}

	return true;
}


void LocalVideoWnd::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void LocalVideoWnd::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void LocalVideoWnd::OnReceivedImage(VIDEC_Image*pImage)
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

void LocalVideoWnd::OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage)
{
	XImage*pXImage=dynamic_cast<XImage*>(pImage);
	if (pXImage==NULL)
	{
		return ;
	}

	if (VIDEC_GetEnableImageColorEnhance())
	{
		if (m_pICE==NULL)
		{
			m_pICE=new ImageColorEnhance();
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

void LocalVideoWnd::OnFrameData(XImage*pXImage)
{
	m_nFrameCount++;

	int nWidth=pXImage->GetActualWidth();
	int nHeight=pXImage->GetActualHeight();
	if (nWidth==352 && (nHeight==480 || nHeight==576))
	{
		pXImage->SetVirtualWidth(nWidth<<1);
		pXImage->SetVirtualHeight(nHeight);
	}
	OnIXPreProcessNotifyOutputData(pXImage);
}


bool LocalVideoWnd::HasVideo(void)
{
	bool bHasVideo=(m_pVideoDraw!=NULL && m_bGotFirstVideo);

	return bHasVideo;
}

int LocalVideoWnd::SetImageDisplayMode(int nMode)
{
	return 0;
}

int LocalVideoWnd::GetImageDisplayMode(void)
{
	return 0;
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
void LocalVideoWnd::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	m_nScreenIndex=nScreenIndex;
	m_nWndIndex=nWndIndex;
	m_nFullScreen=nFullScreen;

	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}

void LocalVideoWnd::EnablePreview(int nPreview)
{
	m_nEnablePreview=nPreview;
}

int LocalVideoWnd::GetSupportPreview(void)
{
	if (m_nEnablePreview)
	{
		return 1;
	}

	if (IsWindowVisible()==false)
	{
		if (MPScreenMgr::Instance().HasMPScreen(m_nScreenIndex)==false)
		{
			return 0;
		}
	}

	return 1;
}

void LocalVideoWnd::Reset(void)
{
	m_bReset=true;
}

VIDEC_Image*LocalVideoWnd::GetImage(void)
{
	XAutoLock l(m_csImage);
	if (m_pImage!=NULL)
	{
		m_pImage->AddRef();
	}

	return m_pImage;
}
