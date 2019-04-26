// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "MyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMyEdit


CMyEdit::CMyEdit()
{
	m_bMove=TRUE;	
	m_crText = RGB(0,0,0);
	m_crBackGnd = RGB(250,250,250);
	m_brBackGnd.CreateSolidBrush(m_crBackGnd);
	m_ptWindowLT = CPoint(0,0);
	m_ScreenWidth = 0;
}

CMyEdit::~CMyEdit()
{
	if (m_brBackGnd.GetSafeHandle())
	{
		m_brBackGnd.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
	//ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	//ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers
// int CMyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// {
// 	if (CEdit::OnCreate(lpCreateStruct) == -1)
// 		return -1;
// 	
// 	// TODO: Add your specialized creation code here
// 	return 0;
// }

void CMyEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rect;
	GetWindowRect(&rect);

	//int xScreen = GetSystemMetrics(SM_CXSCREEN);
	//int ySCreen = GetSystemMetrics(SM_CYSCREEN);

	if(m_bMove)
	{
		//移动到左上角
		MoveWindow(m_ptWindowLT.x+10,10+m_ptWindowLT.y,rect.Width(),rect.Height());
		m_bMove=FALSE;
	}
	else
	{
		//移动到右上角
		MoveWindow(m_ptWindowLT.x+m_ScreenWidth-180,10+m_ptWindowLT.y,rect.Width(),rect.Height());
		m_bMove=TRUE;
	}
//**************************************************************************************
	CEdit::OnMouseMove(nFlags, point);
}

// void CMyEdit::OnSetFocus(CWnd* pOldWnd) 
// {
// 	CEdit::OnSetFocus(pOldWnd);
// //**********************************************************************************
//    //隐藏光标提示符,其实也用不着
// 	this->HideCaret();
// //*********************************************************************************
// 	// TODO: Add your message handler code here	
// }


HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_crText);
	pDC->SetBkColor(m_crBackGnd);

	//return the brush used for background this sets control background
	return m_brBackGnd;
}

BOOL CMyEdit::OnEraseBkgnd(CDC* pDC) 
{
	CEdit::OnEraseBkgnd(pDC);
	return TRUE;

	// TODO: Add your message handler code here and/or call default
//**************************************************************************************
	//设置位图背景
	CBitmap bitmap;
	BITMAP bmp;
	CDC dcCompatible;

	//取消文字复选
	this->SetSel(0,0);

	try{
		BOOL rValue = bitmap.LoadBitmap(IDB_BITMAP2);
		if (rValue == NULL)
		{
			CEdit::OnEraseBkgnd(pDC);
			return FALSE;
		}
		
		bitmap.GetBitmap(&bmp);
		dcCompatible.CreateCompatibleDC(pDC);
		dcCompatible.SelectObject(&bitmap);
	}
	catch(CException* e)
	{
		TCHAR errorMsg[512];
		e->GetErrorMessage(errorMsg, 512);
		OutputDebugString(errorMsg);
		CEdit::OnEraseBkgnd(pDC);
		return FALSE;
	}

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);
	return TRUE;
//*********************************************************************************
	//CEdit::OnEraseBkgnd(pDC);
}

void CMyEdit::SetTextColor(COLORREF rgb)
{
	m_crText = rgb;
	Invalidate(TRUE);
}

void CMyEdit::SetBackColor(COLORREF rgb)
{
	m_crBackGnd = rgb;

	//free brush
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(rgb);

	//redraw
	Invalidate(TRUE);
}

void CMyEdit::SetWindowLT( CPoint pt )
{
	m_ptWindowLT = pt;
}

void CMyEdit::SetScreenWidth( int width )
{
	m_ScreenWidth = width;
}


