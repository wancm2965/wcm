#include "StdAfx.h"
#include "SpotlightDlg.h"
#include "common\WBData.h"
#include "LayeredWindowHelperST.h"
#include "WMBtn.h"
#include "whiteBoard.h"
#include "WBResFunc.h"
using namespace WBRESWND;
// SpotlightDlg.cpp : 实现文件

#define RECTCHANGE 23   //圆偏移量
#define LEASTROUND  180 //圆最小值
#define MOUSE_TOP_LEFT 1
#define MOUSE_TOP_RIGHT 2
#define MOUSE_BOTTOM_LEFT 3
#define MOUSE_BOTTOM_RIGHT 4
INT CSpotlightDlg::m_nCount = 0;

CSpotlightDlg::CSpotlightDlg(CWBSpotlight *pWBSpotlight, CRect screenRect/* = NULL*/)
{
	m_pWBSpotlight = pWBSpotlight;
	m_transparent = 60;
	m_bgColor = RGB(162, 177, 238);
	m_pSpotlightClose = NULL;
	m_pClassName = _T("");
	m_bool  = FALSE;
	m_nMousePos = 0;

	m_ScreenRect = screenRect; //中心圆坐标
	m_ParenthWnd = NULL;	
	m_hCrossCursor = NULL;// 光标
	m_prePoint = NULL;
	m_rect = NULL;  //主窗体坐标
	m_nLargeWidth = 0;
	m_nLargeHight = 0;

	m_bIsPosChange = FALSE;
}
CSpotlightDlg::~CSpotlightDlg()
{
	delete m_pSpotlightClose;
	m_pSpotlightClose = NULL;
	DestroyWindow();
}
// CSpotlight的消息映射
BEGIN_MESSAGE_MAP(CSpotlightDlg, CWnd)	
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// 消息处理程序
// 创建主窗体
BOOL CSpotlightDlg::CreateMainWnd(CRect rect, HWND hwnd)
{
	m_nLargeWidth = rect.Width();
	m_nLargeHight = rect.Height();
	m_rect = rect;
	m_ParenthWnd = hwnd;
	int width = m_rect.right - m_rect.left;
	int height = m_rect.bottom - m_rect.top;
	m_pClassName = AfxGetThreadState()->m_szTempClassName;
	if (0 == m_nCount)
	{
		if(!CreateEx(WS_EX_TOOLWINDOW,
			::AfxRegisterWndClass(CS_VREDRAW,NULL,0,0),
			NULL,

			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,//子窗口使用窗口坐标系

			m_rect.left, m_rect.top, width, height,
			m_ParenthWnd,
			NULL))
		{
			return FALSE;
		}
		else
		{
			m_nCount++;
		}
	} 
	else
	{
		if(!CreateEx(WS_EX_TOOLWINDOW,
			m_pClassName,
			NULL,

			WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			m_rect.left, m_rect.top, width, height,
			m_ParenthWnd,
			NULL))
		{
			return FALSE;
		}
	}

	CRect clientRect;
	GetClientRect(&clientRect);
	CRect RightRectBtn(clientRect.right-54, clientRect.bottom-54, clientRect.right, clientRect.bottom);
	//创建关闭按钮

 	m_pSpotlightClose = new CWMBtn(this, BTN_CLOSE_ID);
 	m_pSpotlightClose->Create(_T("关闭"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS, RightRectBtn, this, BTN_CLOSE_ID);
 	//pos = TRUE;//初始状态表明在左
 	m_pSpotlightClose->SetButtonImage(_T("SpotlighyClose.png"));

	//透明处理
	CLayeredWindowHelperST cl;
	cl.AddLayeredStyle(GetSafeHwnd());
	cl.SetTransparentPercentage(GetSafeHwnd(), m_transparent);

	return TRUE;
}

// 创建窗体
int CSpotlightDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 加载光标资源
	m_hCrossCursor = AfxGetApp()->LoadCursor(IDC_ARROW);

	// 设置窗口尺寸
	int width  = m_rect.right - m_rect.left;
	int height = m_rect.bottom - m_rect.top;
	SetSpotlightPos(m_rect);

	// 初始化椭圆外框矩形大小
	if (NULL == m_ScreenRect)
	{
		if(g_reduce>0.00)
		{
			m_ScreenRect.top = g_reduce*height/4;
			m_ScreenRect.bottom = g_reduce*height*2/3;
			m_ScreenRect.left = g_reduce*width/3;
			m_ScreenRect.right = g_reduce*width*2/3;
		}
		else
		{
			m_ScreenRect.top = height/4;
			m_ScreenRect.bottom = height*2/3;
			m_ScreenRect.left = width/3;
			m_ScreenRect.right = width*2/3;
		}
		
	}
	
	UpdateWindowRegion();
	return 0;
}
//调整窗口大小，当属性窗口打开和关闭时，需要调整窗口大小
void CSpotlightDlg::AdjustSize(CRect& newRect,bool bSendNet)
{
	m_rect = newRect;//屏幕坐标系
	SetSpotlightPos(m_rect);
	ChangeCloseBtn(m_ScreenRect);
	UpdateWindowRegion();
	if (bSendNet)
	{
		g_pWhiteBoardView->SendSpotlight();
	}
}

void CSpotlightDlg::OnMouseMove(UINT nFlags,CPoint point)
{
	if (g_bAvconLock)
	{
		return;
	}
	if (nFlags&MK_LBUTTON)
	{
		CRect WBDlgRect;
		::GetWindowRect(g_pWhiteBoardView->GetSafeHwnd(),&WBDlgRect);
		int x,y;
		x = ::GetSystemMetrics(SM_CXVSCROLL);
		y = ::GetSystemMetrics(SM_CYHSCROLL);
		WBDlgRect.left = WBDlgRect.left + 2;
		WBDlgRect.top = WBDlgRect.top + 2;
		WBDlgRect.right = WBDlgRect.right - x - 2;
		WBDlgRect.bottom = WBDlgRect.bottom - y - 2;		
		CPoint ptcursor;
		GetCursorPos(&ptcursor);
		if (WBDlgRect.PtInRect(ptcursor))
		{
			ClipCursor(&WBDlgRect);	
		}
		else
		{
			ClipCursor(NULL);
		}

		
		CRect rect(m_ScreenRect);
		CRect preEllipse(m_ScreenRect);//当前中心园所在的坐标

		if(m_bool)
		{
			CPoint movepoint = m_prePoint - point;
			m_prePoint = point;
			SetCursor(::LoadCursor(NULL, IDC_CROSS));
			if (m_nMousePos == MOUSE_TOP_LEFT)
			{
				rect.InflateRect(movepoint.x/2, movepoint.y/2);
				if (rect.Width()<LEASTROUND)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (rect.Height()<LEASTROUND)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (rect.Width()>m_nLargeWidth)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (rect.Height()>m_nLargeHight)
				{
					if (movepoint.y < 0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (rect.Width()>=LEASTROUND && rect.Width()<=m_nLargeWidth)
				{
					m_ScreenRect.InflateRect(movepoint.x/2, 0);
				} 
				if (rect.Height()<=m_nLargeHight && rect.Height()>=LEASTROUND)
				{
					m_ScreenRect.InflateRect(0, movepoint.y/2);
				} 

			}
			if (m_nMousePos == MOUSE_BOTTOM_RIGHT)
			{
				rect.InflateRect(-movepoint.x/2, -movepoint.y/2);
				if (rect.Width()<LEASTROUND)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (rect.Height()<LEASTROUND)
				{
					if (movepoint.y<0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (rect.Width()>m_nLargeWidth)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (rect.Height()>m_nLargeHight)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (rect.Width()>=LEASTROUND && rect.Width()<=m_nLargeWidth)
				{
					m_ScreenRect.InflateRect(-movepoint.x/2, 0);
				} 
				if (rect.Height()<=m_nLargeHight && rect.Height()>=LEASTROUND)
				{
					m_ScreenRect.InflateRect(0, -movepoint.y/2);
				} 
			}
			if (m_nMousePos == MOUSE_BOTTOM_LEFT)
			{
				rect.InflateRect(movepoint.x/2, -movepoint.y/2);
				if (rect.Width()<LEASTROUND)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (rect.Height()<LEASTROUND)
				{
					if (movepoint.y<0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (rect.Width()>m_nLargeWidth)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (rect.Height()>m_nLargeHight)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (rect.Width()>=LEASTROUND && rect.Width()<=m_nLargeWidth)
				{
					m_ScreenRect.InflateRect(movepoint.x/2, 0);
				} 
				if (rect.Height()<=m_nLargeHight && rect.Height()>=LEASTROUND)
				{
					m_ScreenRect.InflateRect(0, -movepoint.y/2);
				} 
			}
			if (m_nMousePos == MOUSE_TOP_RIGHT)
			{
				rect.InflateRect(-movepoint.x/2, movepoint.y/2);
				if (rect.Width()<LEASTROUND)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (rect.Height()<LEASTROUND)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (rect.Width()>m_nLargeWidth)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (rect.Height()>m_nLargeHight)
				{
					if (movepoint.y<0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (rect.Height()<=m_nLargeHight && rect.Height()>=LEASTROUND)
				{
					m_ScreenRect.InflateRect(0, movepoint.y/2);
				} 
				if (rect.Width()>=LEASTROUND && rect.Width()<=m_nLargeWidth)
				{
					m_ScreenRect.InflateRect(-movepoint.x/2, 0);
				} 
			}
			

		}
		else
		{
			SetCursor(::LoadCursor(NULL, IDC_HAND));
			int cx_scr = m_rect.Width();
			int cy_scr = m_rect.Height();		

			// 计算位移
			m_ScreenRect.OffsetRect(point - m_prePoint);		
			CPoint centerPoint = m_ScreenRect.CenterPoint();
			if (centerPoint.y < 0)
			{
				m_ScreenRect.OffsetRect(CSize(0, -centerPoint.y));
			}
			if (centerPoint.x < 0)
			{
				m_ScreenRect.OffsetRect(CSize(-centerPoint.x, 0));
			}
			if (centerPoint.y > cy_scr)
			{
				m_ScreenRect.OffsetRect(CSize(0, cy_scr-centerPoint.y));
			}
			if (centerPoint.x > cx_scr)
			{
				m_ScreenRect.OffsetRect(CSize(cx_scr-centerPoint.x, 0));
			}
			m_prePoint = point;
		}
		
		//此处刷新时必须判断，矩形时放大还是缩小了，最好是在原来的基础上放大几个像素，就可以取到很好的效果。
		CRgn ellipseRgn;
		preEllipse.InflateRect(1,1);
		ellipseRgn.CreateEllipticRgnIndirect(&preEllipse);
		this->InvalidateRgn(&ellipseRgn);

		g_pWhiteBoardView->UpdateWindow();
		m_bIsPosChange = TRUE;
		ellipseRgn.DeleteObject();
		
	}
	CWnd::OnMouseMove(nFlags, point);	
} 

void CSpotlightDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_bAvconLock)
	{
		return;
	}
	this->SetCapture();
	CRgn cRgn;
	GetScreenRgn(cRgn, m_ScreenRect);
	if(cRgn.PtInRegion(point))
	{
		SetCursor(::LoadCursor(NULL, IDC_CROSS));
		m_bool = TRUE;
	}
	else
	{
		SetCursor(::LoadCursor(NULL, IDC_HAND));
	}
	CRect rect(m_ScreenRect);
	rect.InflateRect(RECTCHANGE, RECTCHANGE, RECTCHANGE, RECTCHANGE);
	CPoint centerpoint = rect.CenterPoint();
	CRect  topleftrect(rect.TopLeft(), centerpoint);
	CRect  bottomrightrect(centerpoint, rect.BottomRight());
	CRect  bottomleftrect(rect.left, centerpoint.y, centerpoint.x, rect.bottom);
	CRect  toprightrect(centerpoint.x, rect.top, rect.right, centerpoint.y);
	if (PtInRect(&topleftrect, point))
	{
		m_nMousePos = MOUSE_TOP_LEFT;
	}
	if (PtInRect(&bottomrightrect, point))
	{
		m_nMousePos = MOUSE_BOTTOM_RIGHT;
	}
	if (PtInRect(&bottomleftrect, point))
	{
		m_nMousePos = MOUSE_BOTTOM_LEFT;
	}
	if (PtInRect(&toprightrect, point))
	{
		m_nMousePos = MOUSE_TOP_RIGHT;
	}
	m_prePoint = point;
}

//左键抬起
void CSpotlightDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_bAvconLock)
	{
		return;
	}
	ClipCursor(NULL);
	m_nMousePos = 0;
	::ReleaseCapture();
	m_bool = FALSE;
	SetCursor(::LoadCursor(NULL, IDC_ARROW));
	ChangeCloseBtn(m_ScreenRect);

	if (TRUE == m_bIsPosChange)
	{
		m_bIsPosChange = FALSE;
		g_pWhiteBoardView->SendSpotlight();
	}
	
	CWnd::OnLButtonUp(nFlags, point);
}

// 绘制背景
BOOL CSpotlightDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

// 设置光标
BOOL CSpotlightDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}

// 更新窗口区域
void CSpotlightDlg::UpdateWindowRegion()
{
	CRect rtClient;
	GetClientRect(&rtClient);
	CRgn rtClientRgn;
	rtClientRgn.CreateRectRgnIndirect(&rtClient);

	CRgn roundRgn;
	roundRgn.CreateEllipticRgnIndirect(&m_ScreenRect);

	rtClientRgn.CombineRgn(&rtClientRgn, &roundRgn, RGN_XOR);//将两个区域进行异或，去掉重叠的部分
	SetWindowRgn((HRGN)rtClientRgn, TRUE);//设置窗口的有效区域

	//填充窗口背景
	//OnEraseBkgnd(GetDC());
	CRgn rectRgn;
	CBrush brush;
	CDC * pDC = GetDC();
	//创建于pDC兼容的DC和位图
	//GetClientRect(&rtClient);
	CDC dcMem;                                 //用于缓冲作图的内存DC
	CBitmap bmp;                             //内存中承载临时图象的位图
	dcMem.CreateCompatibleDC(pDC);              //依附窗口DC创建兼容内存DC
	if (!bmp.CreateCompatibleBitmap(pDC,rtClient.Width(),rtClient.Height()))//创建兼容位图
	{
		return ;
	}
	//将位图选进兼容DC中
	dcMem.SelectObject(&bmp);

	//绘制背景色
	rectRgn.CreateRectRgnIndirect(&rtClient);
	brush.CreateSolidBrush(m_bgColor);	
	dcMem.FillRgn(&rectRgn, &brush);

	//绘制椭圆
	CPen MyPen;
	COLORREF color = RGB(0 , 85, 35);
	MyPen.CreatePen(PS_SOLID, 45, color);
	CPen* pOriginalPen;
	pOriginalPen=dcMem.SelectObject(&MyPen);
	dcMem.Ellipse(m_ScreenRect);

	//将创建的背景位图复制到pDC中
	::BitBlt(pDC->m_hDC, 0, 0, rtClient.Width(), rtClient.Height(), dcMem.m_hDC, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOriginalPen);
	dcMem.DeleteDC();
	bmp.DeleteObject();
	MyPen.DeleteObject();
	rtClientRgn.DeleteObject();
	roundRgn.DeleteObject();
	rectRgn.DeleteObject();
	brush.DeleteObject();
	ReleaseDC(pDC);
}

BOOL CSpotlightDlg::GetScreenRect(CRect &rect)
{
	rect = m_ScreenRect;
	if (NULL == rect)
	{
		return FALSE;
	} 
	else
	{
		return TRUE;
	}
}

void CSpotlightDlg::SetScreenRect(CRect &rect)
{
	m_ScreenRect = rect;
	UpdateWindowRegion();
	ChangeCloseBtn(m_ScreenRect);
}

void CSpotlightDlg::GetScreenRgn(CRgn &rgn, CRect rt)
{
	CRect rect(rt);
	int bigLen = RECTCHANGE+10;
	rect.InflateRect(bigLen, bigLen, bigLen, bigLen);
	rgn.CreateEllipticRgn(rect.left, rect.top, rect.right, rect.bottom);
}

void CSpotlightDlg::ChangeCloseBtn(CRect rt)
{
	CRect clientRect;
	GetClientRect(clientRect);
	//TRUE:左，FALSE：右
	CRect RightRectBtn(clientRect.right-54, clientRect.bottom-54, clientRect.right, clientRect.bottom);
	if (m_pSpotlightClose)
	{
		m_pSpotlightClose->MoveWindow(RightRectBtn,TRUE);
	}
		
}

void CSpotlightDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialog::OnPaint()
	UpdateWindowRegion();
		
	CDialog::OnPaint();
}

// 设置聚光灯窗口的位置，参照CWhiteBoardDlg类中的OnSize函数
void CSpotlightDlg::SetSpotlightPos(CRect windowRect)
{
	SetWindowPos(&wndTop,windowRect.left, windowRect.top, windowRect.Width(), windowRect.Height(), SWP_SHOWWINDOW);

	CWBRedoAndUndoDlg * pUndoAndRedoDlg = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetRedoAndUndoDlg();
	::SetWindowPos(pUndoAndRedoDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);

	CWBPageDlg * pPageDlg = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetPageDlg();
	::SetWindowPos(pPageDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);

	//调整资源对话框为上层窗口
	if (IsGalleryWndShow())
	{
		WBRESWND::MoveGalleryWndEx(HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
	}
	
	//如下为调整其它窗口为相对于资源窗口的上层窗口
	if (g_pWhiteBoardView)
	{
		CWBFunctionDlg *pDlgFunction =  g_pWhiteBoardView->GetWBFunctionDlg();
		if (pDlgFunction)
		{
			if (pDlgFunction->IsWindowVisible())
			{
				::SetWindowPos(pDlgFunction->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			}
			else
			{
				::SetWindowPos(pDlgFunction->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);
			}
		}
	}
	CWBToolbarCtrlDlg* ptoolbarCtrlDlg = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarCtrDlg();
	CWBToolbarDlg* ptoolbarDlg = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetToolbarDlg();
	if (ptoolbarCtrlDlg)
	{
		::SetWindowPos(ptoolbarCtrlDlg->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		if (ptoolbarDlg )
		{
			if (ptoolbarDlg->IsWindowVisible())
			{
				::SetWindowPos(ptoolbarDlg->GetSafeHwnd(), ptoolbarCtrlDlg->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
			}
			else
			{
				::SetWindowPos(ptoolbarDlg->GetSafeHwnd(), ptoolbarCtrlDlg->GetSafeHwnd(), 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_HIDEWINDOW);
			}
			
		}
	}
	CWBEagelEye* pWBEagelEye = ((CWhiteBoardDlg*)g_whiteboardDlg)->GetWBEagleEyeDlg();
	if (pWBEagelEye != NULL)
	{
		if (pWBEagelEye->IsWindowVisible())
		{
			::SetWindowPos(pWBEagelEye->GetSafeHwnd(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW);
		}
	}
}
