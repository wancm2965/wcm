// WBColorPatetteBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBColorPatetteBtn.h"


// CWBColorPatetteBtn

IMPLEMENT_DYNAMIC(CWBColorPatetteBtn, CButton)

CWBColorPatetteBtn::CWBColorPatetteBtn(COLORREF bkColor)
 : m_strText(_T(""))
{
	m_bkColor = bkColor;
}

CWBColorPatetteBtn::~CWBColorPatetteBtn()
{
}


BEGIN_MESSAGE_MAP(CWBColorPatetteBtn, CButton)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CWBColorPatetteBtn 消息处理程序




void CWBColorPatetteBtn::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct/*lpDrawItemStruct*/)
{

	CRect rect;
	GetClientRect(&rect);
	Graphics graph(lpDrawItemStruct->hDC);
	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	SolidBrush brush(Color(255, GetRValue(m_bkColor), GetGValue(m_bkColor), GetBValue(m_bkColor)));
	graph.FillRectangle(&brush,rect.left,rect.top,rect.right,rect.bottom);
	//在按钮上写文本
	if (m_strText.GetLength()>0)
	{
		LOGFONT lf = {0};
		lf.lfHeight = 15;	
		_tcscpy_s(lf.lfFaceName,_T("宋体"));
		CFont font;
		font.CreateFontIndirect(&lf);	
		CFont *pOldFont = pDc->SelectObject(&font);	

		CSize sz = pDc->GetTextExtent(m_strText);
		int x = (rect.Width()-sz.cx)/2;
		int y = rect.Height()/2 + (rect.Height()/2-sz.cy)/2;
		if (TRUE)
		{
			x = (rect.Width()-15-sz.cx)/2 + 5;
			y = (rect.Height()-sz.cy)/2;
			pDc->SetTextColor(RGB(0,0,0));
		}
		pDc->SetBkMode(TRANSPARENT);
		pDc->TextOut(x,y,m_strText);
		font.DeleteObject();
	}
}


BOOL CWBColorPatetteBtn::OnEraseBkgnd(CDC* pDC)
{
// 	CRect rect;
// 	GetClientRect(&rect);
// 	Graphics graph(pDC->m_hDC);
// 	SolidBrush brush(Color(255, GetRValue(m_bkColor), GetGValue(m_bkColor), GetBValue(m_bkColor)));
// 	graph.FillRectangle(&brush,rect.left,rect.top,rect.right,rect.bottom);
	return TRUE;
	return CButton::OnEraseBkgnd(pDC);
}
void  CWBColorPatetteBtn::SetBackgroundColor(COLORREF bkColor)
{
	m_bkColor = bkColor;
}
COLORREF CWBColorPatetteBtn::GetBackgroundColor()
{
	return m_bkColor;
}
BOOL CWBColorPatetteBtn::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	m_btnID = nID;
	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}
void CWBColorPatetteBtn::SetShowText(CString strText)
{
	m_strText = strText;
	if (g_pWhiteBoardView == NULL)
	{
		return;
	}
	Invalidate();
}


void CWBColorPatetteBtn::SetBtnStyle()
{
	UINT style=this->GetButtonStyle();
	if(!(style&WS_CLIPSIBLINGS))
	{
		style=style|WS_CLIPSIBLINGS;
		this->SetButtonStyle(style);

	}


}