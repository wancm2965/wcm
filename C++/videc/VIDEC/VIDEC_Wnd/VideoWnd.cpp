#include "stdafx.h"
#include "VideoWnd.h"
#include "Resource.h"

IMPLEMENT_DYNAMIC(VideoWnd, CWnd)

VideoWnd::VideoWnd(void)
{
	m_pBitmapBG=NULL;

	ImageInput::Open();
}

VideoWnd::~VideoWnd(void)
{
	if (m_pBitmapBG)
	{
		delete m_pBitmapBG;
		m_pBitmapBG=NULL;
	}
}

BEGIN_MESSAGE_MAP(VideoWnd, CWnd)
	//{{AFX_MSG_MAP(VideoWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL VideoWnd::OnEraseBkgnd(CDC* pDC) 
{
#define NO_VIDEO_BITMAP_WIDTH 96
#define NO_VIDEO_BITMAP_HEIGHT 96

	if (!HasVideo())
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



void VideoWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	ResizeControls();
}

BOOL VideoWnd::DestroyWindow()
{
	ImageInput::Close();

	return CWnd::DestroyWindow();
}

//取视频宽度
int VideoWnd::GetVideoWidth(void)
{
	return 320;
}

//取视频高度
int VideoWnd::GetVideoHeight(void)
{
	return 240;
}


HWND VideoWnd::GetWnd(void)
{
	return m_hWnd;
}

bool VideoWnd::IsWindowVisible(void)
{
	return CWnd::IsWindowVisible()?true:false;
}

bool VideoWnd::SetWindowPos(HWND hWndInsertAfter,int x,int y,int cx,int cy,UINT nFlags)
{
	nFlags|=SWP_SHOWWINDOW;
	BOOL bRet=CWnd::SetWindowPos(CWnd::FromHandle(hWndInsertAfter),x,y,cx,cy,nFlags);

	if (!(nFlags&SWP_NOSIZE))
	{
		ResizeControls();
	}

	return bRet?true:false;
}

//保持当前图象到BMP文件
bool VideoWnd::CaptureToBMPFile(const char*cszFile)
{
	return false;
}


double VideoWnd::GetAverageFPS(void)
{
	return 0.0;
}

//设置视频属性
int VideoWnd::SetVideoParam(int nBrightness/*亮度值0-255*/, int nContrast/*对比度0-127*/, int nSaturation/*饱和度0-127*/, int nHue/*色调0-255*/,int nGamma/*伽马1-400*/,int nSharpen/*锐化0-127*/)
{
	return 0;
}

//int nGammaR[1-400 100]
//int nGammaG[1-400 100]
//int nGammaB[1-400 100]
int VideoWnd::SetVideoGammaRGB(int nGammaR,int nGammaG,int nGammaB)
{
	return 0;
}

//设置图像显示模式

/******************************************************************************
* SetScreenWnd
*描述：设置所在屏幕窗口位置
*输入：	nScreenIndex	-屏幕号
		nWndIndex		-窗口号
		nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
*输出：无
*返回值：无
*****************************************************************************/
void VideoWnd::SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)
{
	ImageInput::SetScreenIndex(nScreenIndex);
	ImageInput::SetWndIndex(nWndIndex);
	ImageInput::SetFullScreen(nFullScreen);
}

void VideoWnd::EnablePreview(int nPreview)
{

}
