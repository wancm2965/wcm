// WBAttributeStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBAttributeStatic.h"

#define  TIME_MOUSE_DOWN   1
// CWBAttributeStatic

IMPLEMENT_DYNAMIC(CWBAttributeStatic, CStatic)

CWBAttributeStatic::CWBAttributeStatic()
	:m_bText(FALSE)
	,m_bRrow(true)
	,m_nFontSize(14)
	,m_size(0)
	,m_pFont(NULL)
	,m_bIsTextShowTip(true)
{	
	m_pFont = new CFont;
	if (m_pFont)
	{
		m_pFont->CreateFont(m_nFontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
	}
	m_strCaption = _T("");
}

CWBAttributeStatic::~CWBAttributeStatic()
{
	if (m_pFont)
	{
		m_pFont->DeleteObject();
		delete m_pFont;
		m_pFont = NULL;
	}
}

BEGIN_MESSAGE_MAP(CWBAttributeStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEHOVER()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOVE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()



// CWBAttributeStatic 消息处理程序




BOOL CWBAttributeStatic::OnEraseBkgnd(CDC* pDC)
{
	if ( !m_bRrow )
	{
		return TRUE;
	}
	CString CurPicPath;	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	int  BtnId;
	BtnId = GetDlgCtrlID();
	GetClientRect(&clicentRc);
	switch (BtnId)
	{
	//case IDC_ATTRIBUTE_TRANSPERSTATIC:
	//	CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\StaticBG.png";
	//	break;
	//case IDC_TRANSPARSTATIC:
	//	CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\NoFrameStaticBG.png";
	//	break;
	case IDC_WB_FONTSTATIC:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\image\\functionDlg\\StaticBG.png";
		break;
	case IDC_PAGENUMBERSTATIC:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\image\\ChildBtnBG.png";	
		break;
	case IDC_TOOLBOXSTATIC:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\image\\ChildBtnBG.png";
		break;
	case IDC_FRAMERATE:
	case IDC_PATHNAME:
	case IDC_FRAME:
	case IDC_FILENAME:
	case IDC_STATICVEDIOSTOP:
	case IDC_STATIC_PATH:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\ScreenRecord.png";
		break;
	case IDC_STATICPLACE:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\ScreenRecord.png";
		break;
	//case IDC_WB_FUNCHIDESTATIC:
	//	CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\m_cFuncDlgHideStaticBG.png";
	//	break;
	default:
		CurPicPath = GetCurrentPath() + L"\\SKIN\\WhiteBoard\\image\\functionDlg\\NoFrameStaticBG.png";
		break;
	}
	//CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\StaticBG.png";
	//OutputDebugString(CurPicPath + _T("\n"));
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}
	return TRUE;
}

void CWBAttributeStatic::SetCaption( CString strCaption )
{
	m_strCaption = strCaption;

	CDC *pdc = GetDC();
	pdc->SelectObject(m_pFont);
	CSize size;
	size = pdc->GetTextExtent(m_strCaption);
	size.cx += 10;
	m_size = size;
	ReleaseDC(pdc);
	Invalidate();
}
void CWBAttributeStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect( &rect );	
	dc.SetBkMode( TRANSPARENT );
	CFont *pFont = new CFont;
	pFont->CreateFont(m_nFontSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
	dc.SelectObject(pFont);
	CString strDraw = m_strCaption;
	if (strDraw.GetLength() > 64)
	{
		strDraw = strDraw.Left(50);
		strDraw += _T("...");
	}
	if (TRUE == m_bText)
	{
		dc.DrawText(strDraw,&rect, DT_LEFT|DT_WORDBREAK);//将文本画在Static的中央
	} 
	else
	{
		dc.DrawText(strDraw,&rect, DT_CENTER|DT_WORDBREAK);//将文本画在Static的中央
	}
	pFont->DeleteObject();
	delete pFont;
	// 不为绘图消息调用 CStatic::OnPaint()
}
void CWBAttributeStatic::SetTextPosition(BOOL bText)
{
	m_bText = bText;
}

void CWBAttributeStatic::SetRrowBK( bool bRrow /*= true*/ )
{
	m_bRrow = bRrow;
}

void CWBAttributeStatic::SetFontSize( int nFontSize /*= 13*/ )
{
	if (nFontSize < 1)
	{
		m_nFontSize = 1;
	}
	else
		if (nFontSize > 40)
		{
			m_nFontSize = 40;
		}
		else
		{
			m_nFontSize = nFontSize;
		}
}

CSize CWBAttributeStatic::GetStringLength()
{
	return m_size;
}

void CWBAttributeStatic::MoveStaticPos( int x,int y )
{
	CSize size = m_size;

	MoveWindow(x,y,size.cx,size.cy);
}


BOOL CWBAttributeStatic::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	return CStatic::OnMouseWheel(nFlags, zDelta, pt);
}


void CWBAttributeStatic::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bIsTextShowTip)
	{
		CWBToolTip::GetIns()->SetTextString(m_strCaption,m_size);
		RECT rcDlg = { 0 };
		::GetWindowRect(this->GetSafeHwnd(), &rcDlg);
		CWBToolTip::GetIns()->SetPosition(CPoint(rcDlg.right, rcDlg.bottom));
		CWBToolTip::GetIns()->Display();
		return;
	}
	if (m_strCaption.IsEmpty())
	{
		CString staticText = _T("");
		GetWindowText(staticText);
		m_strCaption = staticText;
	}
	CWBToolTip::GetIns()->SetTextString(m_strCaption,m_size);
	RECT rcDlg = { 0 };
	::GetWindowRect(this->GetSafeHwnd(), &rcDlg);
	CWBToolTip::GetIns()->SetPosition(CPoint(rcDlg.right, rcDlg.bottom));
	CWBToolTip::GetIns()->Display();
		
	CStatic::OnMouseHover(nFlags, point);
}


void CWBAttributeStatic::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return;
	CStatic::OnLButtonUp(nFlags, point);
}


void CWBAttributeStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return;
	CStatic::OnLButtonDown(nFlags, point);
}

void CWBAttributeStatic::PreSubclassWindow()
{
  	DWORD dwStyle = GetStyle();
  	::SetWindowLong(GetSafeHwnd(), GWL_STYLE, dwStyle | SS_NOTIFY);
 	CStatic::PreSubclassWindow();
}

void CWBAttributeStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwFlags = TME_LEAVE | TME_HOVER;//要触发的消息
	tme.hwndTrack = this->m_hWnd;
	tme.dwHoverTime = 10;// 若不设此参数，则无法触发mouseHover
	::_TrackMouseEvent(&tme); //MOUSELEAVE|MOUSEHOVER消息由此函数触发
	
	CStatic::OnMouseMove(nFlags, point);
}


void CWBAttributeStatic::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	if (CWBToolTip::GetIns())
	{
		CWBToolTip::GetIns()->Restore();
	}
	CStatic::OnMouseLeave();
}
