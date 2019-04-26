// LocalVideoWindow.cpp: implementation of the LocalVideoWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <WtsApi32.h>
#include <WinUser.h>
#include <streams.h>
#include <atlbase.h>
#include <uuids.h>
#include "LocalVideoWindow.h"
#include "XUtil.h"
#include "../VIDEC_API/XImage.h"
#include "../VIDEC_MP/MPScreen.h"
#include "Resource.h"
#include "IYUVDraw.h"
#include "VIDEC_CSP.h"

#define XUM_IMAGE_CHANGED	(WM_USER+100)


void InitMapScreenRect(void);
int GetActualScreenIndex(const RECT&rRect,int&nWidth,int&nHeight);
int GetScreenSameSize(int nIndex1,int nIndex2);

//pinganrenshou 20160601
bool g_bWindowsHideSendVideo =false;

VIDEC_Wnd*VIDEC_Wnd::CreateLocalVideo(VIDEC_WndCallback&rCallback)
{
	NEWRECORD3( LocalVideoWindow*,pXVideoWindow ,new LocalVideoWindow(rCallback) )
	//NULLReturnRet( pXVideoWindow , pXVideoWindow )  
	return pXVideoWindow;
}

VIDEC_API VIDEC_Wnd*VIDEC_Wnd::Create(VIDEC_WndCallback&rCallback)
{
	return CreateLocalVideo(rCallback);
}

IMPLEMENT_DYNAMIC(LocalVideoWindow, CWnd)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LocalVideoWindow::LocalVideoWindow(VIDEC_WndCallback&rCallback)
:XImageSink(this)
,m_hWndParent(NULL)

,m_pVideoDisplay(NULL)
,m_nActualWidth(0)
,m_nActualHeight(0)
,m_nVirtualWidth(0)
,m_nVirtualHeight(0)
,m_nFrameRate(30)
,m_bDoubleField(false)
,m_bEvenField(false)
,m_bOddField(false)
,m_rCallback(rCallback)
,m_bImageChanged(false)

,m_bShowWindow(false)


,m_bGotFirstVideo(false)

,m_nEnablePreview(0)
,m_nEnableVideo(1)
,m_pIXPreProcess(NULL)
, m_nBgBmpWidth(96)
, m_nBgBmpHeight(96)
{
	m_nFrameCount=0;
	m_ulFrameCountTimestamp=XGetTimestamp();
	m_bReSizeMax=false;
	m_bReSizeMin=false;
	m_nLastWidth=0;
	m_nWndIndex=-1;
	m_nScreenIndex=-1;
	m_nFullScreen=0;

	m_bReset=true;

	m_nActualScreenIndex=-1;
	m_bActualScreenIndexChanged=false;

	m_pImage=NULL;

	m_pBitmapBG=NULL;
	m_nLastSizingTimestamp=0;

	m_pICE=NULL;

	m_nNoPreviewWidth=320;
	m_nNoPreviewHeight=240;
	memset(m_sBgBmp, 0, sizeof(m_sBgBmp));
	m_nBgColorR = -1;
	m_nBgColorG = -1;
	m_nBgColorB = -1;
}

LocalVideoWindow::~LocalVideoWindow(void)
{
}

BEGIN_MESSAGE_MAP(LocalVideoWindow, CWnd)
	//{{AFX_MSG_MAP(LocalVideoWindow)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_MESSAGE(XUM_IMAGE_CHANGED,OnImageChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LocalVideoWindow message handlers
BOOL LocalVideoWindow::OnEraseBkgnd(CDC* pDC) 
{
	if (!m_bGotFirstVideo || m_pVideoDisplay==NULL)
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

		int x=(rc.Width()-m_nBgBmpWidth)/2;
		int y=(rc.Height()-m_nBgBmpHeight)/2;

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

	return FALSE;
}


void LocalVideoWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	ResizeControls();
}


void LocalVideoWindow::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	ResizeControls();
}

void LocalVideoWindow::OnTimer(UINT_PTR nIDEvent)
{
	KillTimer(nIDEvent);

	if (nIDEvent==10000)
	{
		ResizeControls();
	}
	else if (nIDEvent==1000)
	{
		KillTimer(1000);
		if (m_hWndParent)
		{
			::PostMessage(m_hWndParent,WM_SIZE,0,0);
		}
	}

	CWnd::OnTimer(nIDEvent);
}


void LocalVideoWindow_CallbackDrawD3D9DC(const void*pUserData,void* hDC)
{
	LocalVideoWindow*pLocalVideoWindow=(LocalVideoWindow*)pUserData;
	if (pLocalVideoWindow)
	{
		pLocalVideoWindow->OnCallbackDrawD3D9DC(hDC);
	}
}

void LocalVideoWindow::OnCallbackDrawD3D9DC(void* hDC)
{
	m_rCallback.OnVIDEC_WndCallbackDrawD3D9DC(hDC);
}

LRESULT LocalVideoWindow::OnImageChanged(WPARAM wParam, LPARAM lParam)
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

	if (m_pVideoDisplay && m_bActualScreenIndexChanged)
	{
		m_pVideoDisplay->Close();
		delete m_pVideoDisplay;
		m_pVideoDisplay=NULL;
	}


	if (m_nEnableVideo==0)
	{
		//m_nActualWidth=0;
		//m_nActualHeight=0;
		m_bImageChanged=false;

		return 0L;
	}

	if (m_pVideoDisplay==NULL)
	{
		m_pVideoDisplay=VideoDisplay::Create(LocalVideoWindow_CallbackDrawD3D9DC,this);
		if (m_pVideoDisplay)
		{
			if (0!=m_pVideoDisplay->Open(m_hWnd))
			{
				m_pVideoDisplay->Close();
				delete m_pVideoDisplay;
				m_pVideoDisplay=NULL;

				return 0;
			}
		}
	}

	m_bActualScreenIndexChanged=false;
	m_bImageChanged=false;

	ResizeControls();

	::PostMessage(m_hWndParent,WM_SIZE,0,0);

	SetTimer(1000,1000,NULL);

	SetTimer(10000,10000,NULL);

	return 0;
}


void LocalVideoWindow::ResizeControls(void)
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

		int nActualScreenIndex=GetActualScreenIndex(rc,nWidth,nHeight);
		if (nActualScreenIndex!=m_nActualScreenIndex)
		{
			if (GetScreenSameSize(m_nActualScreenIndex,nActualScreenIndex)==0)
			{
				m_nActualScreenIndex=nActualScreenIndex;
			}
			else
			{
				m_nActualScreenIndex=nActualScreenIndex;
				//m_bActualScreenIndexChanged=true;
			}
			//if (m_nActualScreenIndex==-1 || !IsWindowVisible())
			//{
			//	m_nActualWidth=0;
			//	m_nActualHeight=0;
			//}
		}
	}
}

int LocalVideoWindow::Open(HWND hwndParent, bool bLocal, unsigned long*lpnVidFrameTimestamp, unsigned long*lpnAudFrameTimestamp, char* pBgBmp, int bgcolr, int bgcolg, int bgcolb)
{
	m_hWndParent=hwndParent;

	CRect rc;
	::GetClientRect(m_hWndParent,rc);
	if (!CreateEx(0,AfxRegisterWndClass(CS_CLASSDC), _T("LocalVideoWindow"),WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_DISABLED ,0,0,1920,1080/*rc.left,rc.top,rc.Width(),rc.Height()*/,m_hWndParent,NULL))
	{
		return -1;
	}

	ImageInput::Open();


	WTSRegisterSessionNotification(m_hWnd,NOTIFY_FOR_THIS_SESSION);

	if (pBgBmp)
		SetBgBmp(pBgBmp);

	if (bgcolr != -1 || bgcolg != -1 || bgcolb != -1)
		SetBgRgb(bgcolr, bgcolg, bgcolb);

	return 0;
}

void LocalVideoWindow::Close(void)
{
	WTSUnRegisterSessionNotification(m_hWnd);
	ImageInput::Close();


	XImageSink::Close();


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

int LocalVideoWindow::GetVideoWidth(void)
{
	//if (GetSupportPreview()==0)
	//{
	//	return m_nNoPreviewWidth;
	//}
	return (m_nVirtualWidth!=0?m_nVirtualWidth:m_nActualWidth);
}

int LocalVideoWindow::GetVideoHeight(void)
{
	//if (GetSupportPreview()==0)
	//{
	//	return m_nNoPreviewHeight;
	//}

	int nWidth=(m_nVirtualWidth!=0?m_nVirtualWidth:m_nActualWidth);
	int nHeight=(m_nVirtualHeight!=0?m_nVirtualHeight:m_nActualHeight);
	if ((nWidth==352 && nHeight==144) || (nWidth==704 && nHeight==288)|| (nWidth==1920 && nHeight== 540) )
	{
		nHeight*=2;
	}
	return nHeight;
	//return (m_nVirtualHeight!=0?m_nVirtualHeight:m_nActualHeight);
}


HWND LocalVideoWindow::GetWnd(void)
{
	return m_hWnd;
}

bool LocalVideoWindow::IsWindowVisible(void)
{
	//return CWnd::IsWindowVisible()?true:false;

	//test ??
	bool bEnable =false;
	// pinganrenshou 20160601
	if(g_bWindowsHideSendVideo ) return true;

	bEnable = CWnd::IsWindowVisible()?true:false;
	//if(bEnable )  DebugOutput("VideoPlayerWnd::IsWindowVisible    true  \n");
	//else  DebugOutput("VideoPlayerWnd::IsWindowVisible                      false    \n");
	return bEnable;
}

int LocalVideoWindow::SetBgBmp(char* sFile)
{
	if (m_sBgBmp[0] == 0)
	{
		memcpy(m_sBgBmp, sFile, strlen(sFile) + 1);
		return 0;
	}
	return -1;
}

int LocalVideoWindow::SetBgRgb(int r, int g, int b)
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

bool LocalVideoWindow::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	y-=2;
	cy+=4;

	nFlags|=SWP_SHOWWINDOW;
	BOOL bRet=CWnd::SetWindowPos(CWnd::FromHandle(hWndInsertAfter),x,y,cx,cy,nFlags);

	if (!(nFlags&SWP_NOSIZE))
	{
		ResizeControls();
	}

	return bRet?true:false;
}

void LocalVideoWindow::SetLevelFixer(int nLevelFixer)
{
}

int LocalVideoWindow::GetLevelFixer(void)
{
	return -1;
}

void LocalVideoWindow::SetSynFlag(int nSynFlag)
{
}

bool LocalVideoWindow::CaptureToBMPFile(const char*cszFile)
{
	if (cszFile==NULL || strlen(cszFile)==0)
	{
		return false;
	}

	if (!m_bGotFirstVideo || m_pVideoDisplay==NULL)
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


double LocalVideoWindow::GetAverageFPS(void)
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

bool LocalVideoWindow::DisplayImage(XImage*pXImage)
{
	if (m_nEnableVideo==0)
	{
		//m_nActualWidth=0;
		//m_nActualHeight=0;
		m_bImageChanged=false;
		return true;
	}

	if (m_pVideoDisplay==NULL)
	{
		m_bImageChanged=true;

		m_nActualWidth=pXImage->GetActualWidth();
		m_nActualHeight=pXImage->GetActualHeight();
		m_nVirtualWidth=pXImage->GetVirtualWidth();
		m_nVirtualHeight=pXImage->GetVirtualHeight();


		PostMessage(XUM_IMAGE_CHANGED);

		return true;
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


void LocalVideoWindow::OnAttachedToImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnAttachedToImageSource(pImageSource);
}

void LocalVideoWindow::OnDetachedFromImageSource(VIDEC_ImageSource*pImageSource)
{
	XImageSink::OnDetachedFromImageSource(pImageSource);
}

void LocalVideoWindow::OnReceivedPreSend(int& nSend)
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

void LocalVideoWindow::OnReceivedImage(VIDEC_Image*pImage)
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
			if (pImage)
			{
				int nWidth=pImage->GetWidth();
				int nHeight=pImage->GetHeight();
				m_nNoPreviewWidth=nWidth?nWidth:320;
				m_nNoPreviewHeight=nHeight?nHeight:240;
			}

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

void LocalVideoWindow::OnIXPreProcessNotifyOutputData(VIDEC_Image*pImage)
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

void LocalVideoWindow::OnFrameData(XImage*pXImage)
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


bool LocalVideoWindow::HasVideo(void)
{
	bool bHasVideo=(m_pVideoDisplay!=NULL && m_bGotFirstVideo);

	return bHasVideo;
}

void LocalVideoWindow::SetEnableVideo(int nEnable)
{
	m_nEnableVideo=nEnable;
	if (m_nEnableVideo==0)
	{
		if (m_pVideoDisplay)
		{
			m_pVideoDisplay->Close();
			delete m_pVideoDisplay;
			m_pVideoDisplay=NULL;
		}

		//m_nActualWidth=0;
		//m_nActualHeight=0;
		m_bImageChanged=false;

		Invalidate(TRUE);
	}
}

int LocalVideoWindow::GetEnableVideo(void)
{
	return m_nEnableVideo;
}

int LocalVideoWindow::SetImageDisplayMode(int nMode)
{
	return 0;
}

int LocalVideoWindow::GetImageDisplayMode(void)
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
void LocalVideoWindow::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	m_nScreenIndex=nScreenIndex;
	m_nWndIndex=nWndIndex;
	m_nFullScreen=nFullScreen;

	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}

void LocalVideoWindow::EnablePreview(int nPreview)
{
	m_nEnablePreview=nPreview;
}

int LocalVideoWindow::GetSupportPreview(void)
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

void LocalVideoWindow::Reset(void)
{
	m_bReset=true;
}

VIDEC_Image*LocalVideoWindow::GetImage(void)
{
	XAutoLock l(m_csImage);
	if (m_pImage!=NULL)
	{
		m_pImage->AddRef();
	}

	return m_pImage;
}

typedef std::map<int,RECT>	MapScreenRect;
static MapScreenRect		g_MapScreenRect;
static bool					m_bMapScreenRectInitialized=false;

int GetScreenSameSize(int nIndex1,int nIndex2)
{
	RECT rc1;
	RECT rc2;
	MapScreenRect::iterator iter;
	iter=g_MapScreenRect.find(nIndex1);
	if (iter!=g_MapScreenRect.end())
	{
		rc1=iter->second;
		iter=g_MapScreenRect.find(nIndex2);
		if (iter!=g_MapScreenRect.end())
		{
			rc2=iter->second;
			if ((rc1.right-rc1.left)==(rc2.right-rc2.left) &&
				(rc1.bottom-rc1.top)==(rc2.bottom-rc2.top))
			{
				return 1;
			}
		}
	}
	return 0;
}

int GetActualScreenIndex(const RECT&rRect,int&nWidth,int&nHeight)
{
	InitMapScreenRect();

	MapScreenRect::iterator iter=g_MapScreenRect.begin();
	while (iter!=g_MapScreenRect.end())
	{
		RECT rcScreen=iter->second;

		int nCenterX=(rRect.left+rRect.right)>>1;
		int nCenterY=(rRect.top+rRect.bottom)>>1;
		if (rcScreen.left<nCenterX && 
			rcScreen.right>nCenterX &&
			rcScreen.top<nCenterY && 
			rcScreen.bottom>nCenterY)
		{
			nWidth=abs(rcScreen.right-rcScreen.left);
			nHeight=abs(rcScreen.bottom-rcScreen.top);
			return iter->first;
		}
		++iter;
	}
	return -1;
}

void InitMapScreenRect(void)
{
	if (m_bMapScreenRectInitialized)
		return;
	m_bMapScreenRectInitialized=true;

	g_MapScreenRect.clear();

	int nActualScreenCount=0;

	int nIndex = 0;
	DEVMODE devmode;
	DEVMODE priDevMode;
	priDevMode.dmPosition.x = 0;
	priDevMode.dmPosition.y = 0;
	priDevMode.dmPelsWidth = 0;
	priDevMode.dmPelsHeight = 0;
	DISPLAY_DEVICE dev = { sizeof(DISPLAY_DEVICE) };
	BOOL bNext = FALSE;

	while (true)
	{
		bNext = EnumDisplayDevices(NULL, nIndex, &dev, 0);
		if (bNext == FALSE)
		{
			break;
		}

		//
		EnumDisplaySettings(dev.DeviceName, ENUM_CURRENT_SETTINGS, &devmode);

		if (dev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ||  //在显示设置中启用选项: "将 Windows 桌面扩展到该显示器"
			dev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			if (dev.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
			{
				priDevMode = devmode;
			}
			else if (devmode.dmPosition.x == priDevMode.dmPosition.x && devmode.dmPosition.y == priDevMode.dmPosition.y
				&& devmode.dmPelsWidth == priDevMode.dmPelsWidth && devmode.dmPelsHeight == priDevMode.dmPelsHeight)
			{
				nIndex+=1;
				continue;
			}

			RECT rc;
			rc.left=devmode.dmPosition.x;
			rc.top=devmode.dmPosition.y;
			rc.right=rc.left+devmode.dmPelsWidth;
			rc.bottom=rc.top+devmode.dmPelsHeight;

			g_MapScreenRect[nActualScreenCount]=rc;
			nActualScreenCount++;
		}
		nIndex++;
	}
}

void LocalVideoWindow::SupportPreProcess(void)
{
	if (m_pIXPreProcess==NULL)
	{
		m_pIXPreProcess=CreateIXPreProcess(*this);
		if (m_pIXPreProcess!=NULL)
		{
			m_pIXPreProcess->SetPictProp(VIDEC_GetDefaultBrightness(),VIDEC_GetDefaultContrast(),VIDEC_GetDefaultSaturation(),VIDEC_GetDefaultHue(),VIDEC_GetDefaultGamma(),VIDEC_GetDefaultSharpen());
		}
	}
}

void LocalVideoWindow::EnableOSD(bool bEnable)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableOSD(bEnable);
	}
}

bool LocalVideoWindow::IsOSDEnable(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsOSDEnable();
	}
	return false;
}

void LocalVideoWindow::SetOSDText(const char*cszText,int nAlign,int nTop,int nLeft)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDText(cszText,nAlign,nTop, nLeft);
	}
}

void LocalVideoWindow::SetOSDFont(LOGFONTA fontOSD,COLORREF crText,COLORREF crBk)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetOSDFont(fontOSD,crText,crBk);
	}
}

int LocalVideoWindow::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetPictProp(nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
	}

	return 0;
}

int LocalVideoWindow::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->SetVideoGammaRGB(nGammaR,nGammaG,nGammaB);
	}

	return 0;
}

void LocalVideoWindow::SetDenoise(int nDenoise)
{
	if (m_pIXPreProcess!=NULL)
	{
		m_pIXPreProcess->EnableDenoise(nDenoise!=0);
	}
}

int LocalVideoWindow::GetDenoise(void)
{
	if (m_pIXPreProcess!=NULL)
	{
		return m_pIXPreProcess->IsDenoiseEnable()?1:0;
	}
	return -1;
}

int LocalVideoWindow::GetVideoShowFrameRate(void)
{
	if (m_pVideoDisplay)
	{
		return m_pVideoDisplay->GetVideoShowFrameRate();
	}
	return 0;
}
int LocalVideoWindow::GetVideoShowType(void)
{
	if (m_pVideoDisplay)
	{
		return m_pVideoDisplay->GetVideoShowType();
	}
	return 0;
}