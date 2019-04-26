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

//ȡ��Ƶ���
int VideoWnd::GetVideoWidth(void)
{
	return 320;
}

//ȡ��Ƶ�߶�
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

//���ֵ�ǰͼ��BMP�ļ�
bool VideoWnd::CaptureToBMPFile(const char*cszFile)
{
	return false;
}


double VideoWnd::GetAverageFPS(void)
{
	return 0.0;
}

//������Ƶ����
int VideoWnd::SetVideoParam(int nBrightness/*����ֵ0-255*/, int nContrast/*�Աȶ�0-127*/, int nSaturation/*���Ͷ�0-127*/, int nHue/*ɫ��0-255*/,int nGamma/*٤��1-400*/,int nSharpen/*��0-127*/)
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

//����ͼ����ʾģʽ

/******************************************************************************
* SetScreenWnd
*����������������Ļ����λ��
*���룺	nScreenIndex	-��Ļ��
		nWndIndex		-���ں�
		nFullScreen		-�Ƿ�ȫ����ʾ��0����ͨ��ʾ����0��ȫ����ʾ��
*�������
*����ֵ����
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
