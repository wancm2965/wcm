#include "StdAfx.h"
#include "WBToolTip.h"

/*-----------------------------------------------------------------------------------
Begin:  definition of the class of CWBToolTipTemplate
------------------------------------------------------------------------------------*/
CWBToolTipTemplate::CWBToolTipTemplate(void):m_bgColor(RGB(254, 255, 225))
{
	memset(&m_fontlog, 0, sizeof(LOGFONT));
	m_fontlog.lfHeight = 14;
	m_fontlog.lfCharSet = 134;
	_tcscpy_s(m_fontlog.lfFaceName, _T("宋体"));
}


CWBToolTipTemplate::~CWBToolTipTemplate(void)
{
}

void CWBToolTipTemplate::SetColor(COLORREF bgColor)
{
	m_bgColor = bgColor;
}

void CWBToolTipTemplate::SetText(CString str)
{
	m_strText = str;
}

BEGIN_MESSAGE_MAP(CWBToolTipTemplate, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CWBToolTipTemplate::OnPaint()
{
	CRect rc;
	GetClientRect(&rc);

	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	
	//绘制背景和边框
	CBrush brush;
	brush.CreateSolidBrush(m_bgColor);
	HBRUSH oldBrush = (HBRUSH)dc.SelectObject(brush);
	dc.Rectangle(rc.left, rc.top, rc.right, rc.bottom);
	dc.SelectObject(oldBrush);
	brush.DeleteObject();

	//绘制文字
	CFont font;
	font.CreateFontIndirect(&m_fontlog);
	HFONT oldfont = (HFONT)dc.SelectObject(font);
	CSize szText = dc.GetTextExtent(m_strText);
	rc.left = rc.left + (rc.Width() - szText.cx + 1)/2;
	rc.top = rc.top + (rc.Height() - szText.cy + 1) / 2;
	rc.bottom = rc.top + szText.cy;
	rc.right = rc.left + szText.cx;
	dc.DrawText(m_strText, rc, DT_CENTER);
	dc.SelectObject(oldfont);
	font.DeleteObject();
}

void CWBToolTipTemplate::OnMouseMove( UINT nFlags, CPoint point )
{
	ShowWindow(SW_HIDE);
}


/*-----------------------------------------------------------------------------------
End:  definition of the class of CWBToolTipTemplate
------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------
Begin:  definition of the class of CWBToolTip
------------------------------------------------------------------------------------*/
CWBToolTip* CWBToolTip::s_pWBToolTipIns = NULL;
CWBToolTip::CWBToolTip(): m_pToolTip(NULL)
	, m_position(0,0)
{
	
}


CWBToolTip::~CWBToolTip(void)
{
	if (m_pToolTip)
	{
		m_pToolTip->DestroyWindow();
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}

void CWBToolTip::SetColor(COLORREF toolTipBgColor)
{
	m_pToolTip->SetColor(toolTipBgColor);
	Restore();
}

void CWBToolTip::SetText(CString toolTipStr)
{
	if (m_pToolTip->GetText() == toolTipStr)
		return;

	m_pToolTip->SetText(toolTipStr);
	CSize sz;
	HDC hdc = GetWindowDC(NULL);
	GetTextExtentPoint(hdc, toolTipStr.GetBuffer(), toolTipStr.GetLength(), &sz);
	toolTipStr.ReleaseBuffer();
    SetSize(sz + CSize(0, 4));
	Restore();
	ReleaseDC(NULL,hdc);
}

void CWBToolTip::SetPosition(CPoint toolTipPos)
{
	//安排提示框的正确位置
	//在分屏的时候点坐标的位置
	POINT pt;
	::GetCursorPos(&pt);
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	int   nScreenWidth = rcWork.right; 
	int   nScreenHeight = rcWork.bottom;

	//如果提示框的右边超出屏幕
	if (toolTipPos.x + m_size.cx > nScreenWidth)
	{
		toolTipPos.x = nScreenWidth - m_size.cx;
	}
	//如果提示框的下边超出屏幕
	if (toolTipPos.y + m_size.cy > nScreenHeight)
	{
		toolTipPos.y = nScreenHeight - m_size.cy;
	}

	if (m_position == toolTipPos)
	{
		return;
	}

	m_position = toolTipPos;
	if (IsWindow(m_pToolTip->m_hWnd))
	{
		m_pToolTip->MoveWindow(toolTipPos.x, toolTipPos.y, (int)(m_size.cx+11), m_size.cy);
	}
	Restore();
	
}

void CWBToolTip::SetSize(CSize toolTipSize)
{
	m_size = toolTipSize;
	if (IsWindow(m_pToolTip->m_hWnd))
	{
		m_pToolTip->MoveWindow(m_position.x, m_position.y, (int)(toolTipSize.cx+11), toolTipSize.cy);
	}
	Restore();
}


void CWBToolTip::Display()
{
	if (IsWindow(m_pToolTip->m_hWnd))
	{
		::SetWindowPos(m_pToolTip->GetSafeHwnd(),HWND_TOPMOST,0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW | SWP_NOACTIVATE );
		m_pToolTip->ShowWindow(SW_SHOW);		
	}	
}

void CWBToolTip::Restore()
{
	if (IsWindow(m_pToolTip->m_hWnd))
	{
		m_pToolTip->ShowWindow(SW_HIDE);
	}
	
}

CWBToolTip * CWBToolTip::GetIns()
{
	return s_pWBToolTipIns;
}

bool CWBToolTip::CreateIns()
{
	if (s_pWBToolTipIns)
	{
		return false;
	}
	s_pWBToolTipIns = new CWBToolTip;	

	return true;
}

void CWBToolTip::DestroyIns()
{
	if (s_pWBToolTipIns)
	{
		delete s_pWBToolTipIns;
		s_pWBToolTipIns = NULL;
	}
}

void CWBToolTip::InitCToolTipTemplateDlg(CWnd* pParentWnd)
{
	m_pToolTip = new CWBToolTipTemplate;
	m_pToolTip->Create(IDD_WHITEBOARD_TOOLTIP, pParentWnd);
	
}

void CWBToolTip::SetTextString( CString toolTipStr,CSize size /*= 0*/ )
{
	if (m_pToolTip->GetText() == toolTipStr)
		return;

	m_pToolTip->SetText(toolTipStr);
	
	SetSize(size + CSize(0, 10));
	Restore();
}

/*-----------------------------------------------------------------------------------
End:  definition of the class of CWBToolTip
------------------------------------------------------------------------------------*/