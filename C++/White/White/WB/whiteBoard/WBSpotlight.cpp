#include "StdAfx.h"
#include "WBSpotlight.h"
#include "LayeredWindowHelperST.h"


// SpotlightDlg.cpp : 实现文件
INT g_count = 0;
#define RECTCHANGE 30

#define LEASTROUND  180

CWBSpotlight::CWBSpotlight()
{
	m_transparent = 30;
	m_preColor = RGB(20, 220, 50);
	m_pSpotlightClose = NULL;
	m_pClassName = NULL;
	m_bool  = FALSE;
}
CWBSpotlight::~CWBSpotlight()
{
	delete m_pSpotlightClose;
	m_pSpotlightClose = NULL;
	PostNcDestroy();
}
// CWBSpotlight的消息映射
BEGIN_MESSAGE_MAP(CWBSpotlight, CWnd)	
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// 消息处理程序
// 创建主窗体
BOOL CWBSpotlight::CreateMainWnd(CRect rect, HWND hwnd)
{
	m_rect = rect;
	m_ParenthWnd = hwnd;
	int width = m_rect.right - m_rect.left;
	int height = m_rect.bottom - m_rect.top;
	m_pClassName = AfxGetThreadState()->m_szTempClassName;
	if (0 == g_count)
	{
		if(!CreateEx(WS_EX_TOOLWINDOW,
			::AfxRegisterWndClass(CS_VREDRAW,NULL,0,0),
			NULL,
			WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
			m_rect.left, m_rect.top, width, height,
			m_ParenthWnd,
			NULL))
		{
			return FALSE;
		}
		else
		{
			g_count++;
		}
	} 
	else
	{
		if(!CreateEx(WS_EX_TOOLWINDOW,
			m_pClassName,
			NULL,
			WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS,
			m_rect.left, m_rect.top, width, height,
			m_ParenthWnd,
			NULL))
		{
			return FALSE;
		}
	}

	// 创建菜单按钮和关闭按钮
 	CRect rtBitmapBtn(20, 30, 100, 100);
 	m_pSpotlightClose = new CWMBtn(this, BTN_CLOSE_ID);
 	m_pSpotlightClose->Create(_T("关闭"), WS_VISIBLE|BS_PUSHBUTTON | WS_CLIPSIBLINGS, rtBitmapBtn, this, BTN_CLOSE_ID);
	//透明处理
 	CLayeredWindowHelperST cl;
 	cl.AddLayeredStyle(GetSafeHwnd());
 	cl.SetTransparentPercentage(GetSafeHwnd(), m_transparent);

	return TRUE;
}

// 创建窗体
int CWBSpotlight::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 加载光标资源
	m_hCrossCursor = AfxGetApp()->LoadCursor(IDC_ARROW);

	// 设置窗口尺寸
	CRect rect;
	GetClientRect(rect);
	int width  = m_rect.right - m_rect.left;
	int height = m_rect.bottom - m_rect.top;
	SetWindowPos(&wndTop, m_rect.left, m_rect.top, width, height, SWP_SHOWWINDOW);
	// 初始化椭圆外框矩形大小
	m_ScreenRect.top = height/4;
	m_ScreenRect.bottom = height*2/3;
	m_ScreenRect.left = width/3;
	m_ScreenRect.right = width*2/3;

	UpdateWindowRegion();
	return 0;
}

// 鼠标移动
void CWBSpotlight::OnMouseMove(UINT nFlags,CPoint point)
{
	
	if (nFlags&MK_LBUTTON)
	{
		CRect rect(m_ScreenRect);
		rect.InflateRect(RECTCHANGE, RECTCHANGE, RECTCHANGE, RECTCHANGE);
		if(m_bool)
		{
			CPoint movepoint = m_prePoint - point;
			m_prePoint = point;
			CPoint centerpoint = rect.CenterPoint();
			CRect  topleftrect(rect.TopLeft(), centerpoint);
			CRect  bottomrightrect(centerpoint, rect.BottomRight());
			CRect  bottomleftrect(rect.left, centerpoint.y, centerpoint.x, rect.bottom);
			CRect  toprightrect(centerpoint.x, rect.top, rect.right, centerpoint.y);
			CPoint toppoint();
			SetCursor(::LoadCursor(NULL, IDC_CROSS));
			if (PtInRect(&topleftrect, point))
			{
				if (m_ScreenRect.Width()<LEASTROUND)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (m_ScreenRect.Height()<LEASTROUND)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (m_ScreenRect.Width()>=LEASTROUND && m_ScreenRect.Height()>=180)
				{
					m_ScreenRect.InflateRect(movepoint.x/2, movepoint.y/2);
				} 
				
			}
			if (PtInRect(&bottomrightrect, point))
			{
				if (m_ScreenRect.Width()<LEASTROUND)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (m_ScreenRect.Height()<LEASTROUND)
				{
					if (movepoint.y<0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (m_ScreenRect.Width()>=LEASTROUND && m_ScreenRect.Height()>=180)
				{
					m_ScreenRect.InflateRect(-movepoint.x/2, -movepoint.y/2);
				} 
			}
			if (PtInRect(&bottomleftrect, point))
			{
				if (m_ScreenRect.Width()<LEASTROUND)
				{
					if (movepoint.x>0)
					{
						m_ScreenRect.InflateRect(movepoint.x/2, 0);
					}
				}
				if (m_ScreenRect.Height()<LEASTROUND)
				{
					if (movepoint.y<0)
					{
						m_ScreenRect.InflateRect(0, -movepoint.y/2);
					}
				}
				if (m_ScreenRect.Width()>=LEASTROUND && m_ScreenRect.Height()>=180)
				{
					m_ScreenRect.InflateRect(movepoint.x/2, -movepoint.y/2);
				} 
			}
			if (PtInRect(&toprightrect, point))
			{
				if (m_ScreenRect.Width()<LEASTROUND)
				{
					if (movepoint.x<0)
					{
						m_ScreenRect.InflateRect(-movepoint.x/2, 0);
					}
				}
				if (m_ScreenRect.Height()<LEASTROUND)
				{
					if (movepoint.y>0)
					{
						m_ScreenRect.InflateRect(0, movepoint.y/2);
					}
				}
				if (m_ScreenRect.Width()>=LEASTROUND && m_ScreenRect.Height()>=180)
				{
					m_ScreenRect.InflateRect(-movepoint.x/2, movepoint.y/2);
				} 
			}
		}
		else
		{
			SetCursor(::LoadCursor(NULL, IDC_HAND));
			int cx_scr = ::GetSystemMetrics(SM_CXSCREEN);
			int cy_scr = ::GetSystemMetrics(SM_CYSCREEN);		

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
		UpdateWindowRegion();
		Invalidate();
		Sleep(65);
	}
	CWnd::OnMouseMove(nFlags, point);
} 

// 左键按下
void CWBSpotlight::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	CRect rect(m_ScreenRect);
	rect.InflateRect(RECTCHANGE, RECTCHANGE, RECTCHANGE, RECTCHANGE);
	CRgn cRgn;
	cRgn.CreateEllipticRgn(rect.left, rect.top, rect.right, rect.bottom);

	if(cRgn.PtInRegion(point))
	{
		SetCursor(::LoadCursor(NULL, IDC_CROSS));
		m_bool = TRUE;
	}
	else
	{
		SetCursor(::LoadCursor(NULL, IDC_HAND));
	}
	m_prePoint = point;
}


//左键抬起
void CWBSpotlight::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_bool = FALSE;
	SetCursor(::LoadCursor(NULL, IDC_ARROW));
	Invalidate();
	CWnd::OnLButtonUp(nFlags, point);
}



// 绘制背景
BOOL CWBSpotlight::OnEraseBkgnd(CDC* pDC)
{
	CRect rtClient;
	CRgn rectRgn;
	CBrush brush;

	GetClientRect(&rtClient);
	rectRgn.CreateRectRgnIndirect(&rtClient);

	// 绘制背景色
	brush.CreateSolidBrush(m_preColor);	
	pDC->FillRgn(&rectRgn, &brush);
	ReleaseDC(pDC);
	MyGraph();

	return TRUE;
}

// 设置光标
BOOL CWBSpotlight::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	return TRUE;
}

// 绘制图形
void CWBSpotlight::MyGraph()
{
	CClientDC dc(this);
	CPen MyPen;

	COLORREF color = RGB(0 , 0, 255);
	MyPen.CreatePen(PS_SOLID, 45, color);


	CPen* pOriginalPen;
	pOriginalPen=dc.SelectObject(&MyPen);


	dc.Ellipse(m_ScreenRect);

	dc.SelectObject(pOriginalPen);
}

// 更新窗口区域
void CWBSpotlight::UpdateWindowRegion()
{
	CRect rtClient;
	GetClientRect(&rtClient);

	CRgn rtClientRgn;
	rtClientRgn.CreateRectRgnIndirect(&rtClient);

	CRgn roundRgn;
	roundRgn.CreateEllipticRgnIndirect(&m_ScreenRect);

	rtClientRgn.CombineRgn(&rtClientRgn, &roundRgn, RGN_XOR);

	SetWindowRgn((HRGN)rtClientRgn, TRUE);

	// 即时修改背景色
	OnEraseBkgnd(GetDC());


}	
// 形状菜单:圆形
void CWBSpotlight::OnRound()
{
	CRect rtClient;
	GetClientRect(&rtClient);

	CRgn rtClientRgn;
	rtClientRgn.CreateRectRgnIndirect(&rtClient);

	CRgn roundRgn;
	roundRgn.CreateEllipticRgnIndirect(&m_ScreenRect);

	rtClientRgn.CombineRgn(&rtClientRgn, &roundRgn, RGN_XOR);

	SetWindowRgn((HRGN)rtClientRgn, TRUE);

	// 即时修改背景色
	OnEraseBkgnd(GetDC());
}









