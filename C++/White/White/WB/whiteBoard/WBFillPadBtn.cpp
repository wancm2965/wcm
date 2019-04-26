// WBFillPadBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBFillPadBtn.h"


// CWBFillPadBtn

IMPLEMENT_DYNAMIC(CWBFillPadBtn, CButton)

CWBFillPadBtn::CWBFillPadBtn()
{
	m_isMouseIn = FALSE;
	m_mouseState = MOUSE_NORMAL;
	m_isSelect = FALSE;
}

CWBFillPadBtn::~CWBFillPadBtn()
{
}


BEGIN_MESSAGE_MAP(CWBFillPadBtn, CButton)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CWBFillPadBtn 消息处理程序

void CWBFillPadBtn::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_mouseState = MOUSE_HOVER;
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
	CButton::OnMouseHover(nFlags, point);
}


void CWBFillPadBtn::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_isMouseIn = FALSE;
	m_mouseState = MOUSE_LEAVE;
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
	CButton::OnMouseLeave();
}


void CWBFillPadBtn::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(!m_isMouseIn)
	{
		TRACKMOUSEEVENT tme = {0};
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE;
		tme.dwHoverTime = 1;
		tme.hwndTrack = m_hWnd;
		m_isMouseIn = TrackMouseEvent(&tme);
	}
	CButton::OnMouseMove(nFlags, point);
}


void CWBFillPadBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	m_mouseState = MOUSE_LBTNDOWN;
	CButton::OnLButtonDown(nFlags, point);
}


void CWBFillPadBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (g_bAvconLock) //如果会议中白板功能被锁定
	{
		return;
	}
	m_mouseState = MOUSE_LBTNUP;
	CButton::OnLButtonUp(nFlags, point);
}

void CWBFillPadBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  添加您的代码以绘制指定项
	CDC dc;
	CImage img;
	int  iBtnId;
	dc.Attach( lpDrawItemStruct->hDC ); 
	CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录	
	iBtnId = GetDlgCtrlID();
	switch (iBtnId)
	{
	case IDC_SINWBCLRFILLBTN1:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\1sel.png");
				break;
			}
			switch (m_mouseState)//鼠标当前的状态
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\1.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\1sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\1.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN2:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\2sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\2.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\2sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\2.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN3:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\3sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\3.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\3sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\3.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN4:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\4sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\4.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\4sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\4.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN5:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\5sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\5.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\5sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\5.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN6:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\6sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\6.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\6sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\6.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN7:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\7sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\7.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\7sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\7.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN8:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\8sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\8.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\8sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\8.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN9:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\9sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\9.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\9sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\9.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN10:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\10sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\10.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\10sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\10.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN11:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\11sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\11.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\11sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\11.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN12:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\12sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\12.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\12sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\12.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN13:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\13sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\13.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\13sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\13.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN14:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\14sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\14.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\14sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\14.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_SINWBCLRFILLBTN15:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\15sel.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\15.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\15sel.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\15.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_WB_TTBTN1:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\zc2.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\zc.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\zc2.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\zc.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_WB_TTBTN2:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\yg2.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\yg.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\yg2.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\yg.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_WB_TTBTN3:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\sjc2.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\sjc.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\sjc2.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\sjc.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_WB_TTBTN4:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\ljq2.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\ljq.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\ljq2.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\ljq.png");
				break;
			default:
				break;
			}
		}
		break;
	case IDC_WB_TTBTN5:
		{
			if (m_isSelect)
			{
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\dm2.png");
				break;
			}
			switch (m_mouseState)
			{
			case MOUSE_NORMAL:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\dm.png");
				break;
			case MOUSE_HOVER:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\dm2.png");
				break;
			case MOUSE_LEAVE:
				CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\FillPad\\dm.png");
				break;
			default:
				break;
			}
		}
		break;
	}
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw( dc.m_hDC, lpDrawItemStruct->rcItem );
	}
	dc.Detach(); 
}
		
BOOL CWBFillPadBtn::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}

void CWBFillPadBtn::SetIsSelectBtn( BOOL isSelect )
{
	m_isSelect = isSelect;
}


