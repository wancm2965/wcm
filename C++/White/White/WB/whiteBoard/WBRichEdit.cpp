// WBRichEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBRichEdit.h"


// WBRichEdit

IMPLEMENT_DYNCREATE(CWBRichEdit, CRichEditView)

CWBRichEdit::CWBRichEdit()
{
	m_strClass = MSFTEDIT_CLASS;

	//初始化字体
// 	m_Font.CreateFont(30, 0, 0, 0, 0, 0, 0, 0, 1
// 		, OUT_DEFAULT_PRECIS
// 		, CLIP_DEFAULT_PRECIS
// 		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));

	m_lfCurFont.lfItalic = 0;
	m_lfCurFont.lfStrikeOut = 0;
	m_lfCurFont.lfUnderline = 0;
	m_lfCurFont.lfWeight = 400;
	m_lfCurFont.lfWidth = 0;
	m_lfCurFont.lfHeight = 36;
	m_lfCurFont.lfCharSet = 134;
	m_lfCurFont.lfEscapement = 0;
	m_lfCurFont.lfOrientation = 0;
	_tcscpy_s(m_lfCurFont.lfFaceName, _T("宋体"));
}

CWBRichEdit::~CWBRichEdit()
{
}

BEGIN_MESSAGE_MAP(CWBRichEdit, CRichEditView)
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


// WBRichEdit 诊断

#ifdef _DEBUG
void CWBRichEdit::AssertValid() const
{
	CRichEditView::AssertValid();
}

#ifndef _WIN32_WCE
void CWBRichEdit::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif
#endif //_DEBUG


// WBRichEdit 消息处理程序


void CWBRichEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	CRichEditView::OnMouseMove(nFlags, point);
}

void CWBRichEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == 27) //键盘Esc键消息
	{
		return;
	}
	if (IsCTRLPressed()) //按下CTRL键时就清空剪切板
	{
		if(OpenClipboard())//打开剪切板
		{
			EmptyClipboard();//清空剪切板
			CloseClipboard();//关闭
		}
		return;
	}
	CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CWBRichEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar == 27 ) //键盘Esc键消息
	{
		return;
	}
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);

	if (g_pWhiteBoardView->GetCurEdit() != NULL)
	{
		int iFontWeight;
		CFont Font;
		if (g_isBold)
		{
			iFontWeight = FW_BOLD;
		}
		else
			iFontWeight = FW_NORMAL;
		if (Font.GetSafeHandle())
		{
			Font.Detach();
		}
		Font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
			, OUT_DEFAULT_PRECIS
			, CLIP_DEFAULT_PRECIS
			, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
		g_pWhiteBoardView->GetCurEdit()->SetFont(&Font);
		if (nChar == 32)
		{
			
		}
		g_pWhiteBoardView->SetEditCharFormat(g_pWhiteBoardView->GetCurEdit());

		SetCurTextFont();
		Font.DeleteObject();
	}
}


void CWBRichEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg())
	{
		if (NULL != g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox())
		{
			CString str;
			g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox()->GetWindowText(str);
			if (_tcscmp(str, _T("")) == 0)
			{
				g_pWhiteBoardView->GetPromptFontSetDlg()->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
				g_pWhiteBoardView->GetPromptFontSetDlg()->UpdateData(FALSE);
			}
		}
	}
	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		if (_tcscmp(g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize,_T("")) == 0)
		{
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
			g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
		}
	}
	g_pWhiteBoardView->SendLocalMessage(CMD_FOCUS_GET,FALSE);
	CRichEditView::OnLButtonDown(nFlags, point);
}

HRESULT CWBRichEdit::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat, DWORD reco, BOOL bReally, HGLOBAL hMetaPict)
{
	return S_FALSE;
}

BOOL CWBRichEdit::CreateEdit( BOOL bVertical, const CRect &rct, CWnd *pParent )
{
	ASSERT(NULL != pParent);

	CFont font;
	int nVerticalStyle = (bVertical)?(ES_VERTICAL):(0);
	BOOL bCreate = Create(NULL, NULL, ES_MULTILINE
		| ES_WANTRETURN 
		| WS_CHILD 
		| WS_VISIBLE 
		| WS_TABSTOP 
		| WS_BORDER
		| nVerticalStyle
		| WS_VSCROLL
		| WS_HSCROLL
		| ES_AUTOVSCROLL
		| ES_AUTOHSCROLL, rct, pParent, 1);

	if (bCreate)
	{
		SetFocus();
		GetRichEditCtrl().LimitText(1024);
	}

	//设置文字录入时的字体样式caoyuqin2011-7-11
	int iFontWeight;
	if (g_isBold)
	{
		iFontWeight = FW_BOLD;
	}
	else
		iFontWeight = FW_NORMAL;
	
	font.CreateFont(g_nWBCurObjFontSize, 0, 0, 0, iFontWeight, g_isItalic, g_isUnderline, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,g_nWBCurObjFontFaceName);	
	m_clrCurTextColor = g_clrWBCurObjColor;
	SetFont(&font);

	//字体样式luowei
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	GetRichEditCtrl().GetSelectionCharFormat(cf);
	cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
	cf.dwEffects = 0;
	if (g_isBold)
	{
		cf.dwEffects += CFE_BOLD;
	}
	if (g_isItalic)
	{
		cf.dwEffects += CFE_ITALIC;
	}
	if (g_isUnderline)
	{
		cf.dwEffects += CFE_UNDERLINE;
	}
	cf.crTextColor = g_clrWBCurObjColor;
	long lLenght = GetTextLength();
	GetRichEditCtrl().SetSel(0, lLenght);
	GetRichEditCtrl().SetSelectionCharFormat(cf);
	GetRichEditCtrl().SetSel(-1, -1);//光标到末尾

	SetCurTextFont();
	font.DeleteObject();
	//m_myDropTarget.DragDropRegister(m_hWnd);
	return bCreate;
}

void CWBRichEdit::SetCurTextFont()
{
	m_lfCurFont.lfHeight = g_nWBCurObjFontSize;
	_tcscpy_s(m_lfCurFont.lfFaceName,g_nWBCurObjFontFaceName);
	if (g_isBold)
	{
		m_lfCurFont.lfWeight = FW_BOLD;
	} 
	else
	{
		m_lfCurFont.lfWeight = FW_NORMAL;
	}
	m_lfCurFont.lfItalic = g_isItalic;
	m_lfCurFont.lfUnderline = g_isUnderline;
}

LOGFONT CWBRichEdit::GetCurFont()
{
	return m_lfCurFont;
}

void CWBRichEdit::SetCurFont(const LOGFONT& lfFont)
{
	memcpy((char *)&m_lfCurFont, (char *)&lfFont, sizeof(LOGFONT));
}

COLORREF CWBRichEdit::GetFontColor()
{
	return m_clrCurTextColor;
}

void CWBRichEdit::SetFontColor(COLORREF clrText)
{
	m_clrCurTextColor = clrText;
}

void CWBRichEdit::OnKillFocus( CWnd* pNewWnd )
{
	g_pWhiteBoardView->SendLocalMessage(CMD_FOCUS_KILL,FALSE);
	return CRichEditView::OnKillFocus(pNewWnd);
}

void CWBRichEdit::OnSetFocus( CWnd* pNewWnd )
{
	g_pWhiteBoardView->SendLocalMessage(CMD_FOCUS_GET,FALSE);
	return CRichEditView::OnSetFocus(pNewWnd);
}

BOOL CWBRichEditMgr::ShowWBRichEdit( const unsigned long &nTouchID, BOOL bVertical, 
	CWnd *pParent, BOOL bShowPrompt/* = true*/ )
{
	XAutoLock lock(m_csMapOperation);
	CRect rct;
	if (!GetRect(nTouchID,rct))
	{
		return false;
	}

	if (g_bIsVertical)
	{
		CPoint pt;
		pt.x = rct.right;
		pt.y = rct.top;
		g_pWhiteBoardView->SetEditTopLeft(pt);
	} 
	else
	{
		CPoint pt = rct.TopLeft();
		
		g_pWhiteBoardView->SetEditTopLeft(pt);
	}

	//添加滚动条偏移量
	if (!g_bIsDrawScreen)
	{
		rct.left -= g_pWhiteBoardView->getXnPos();
		rct.right -= g_pWhiteBoardView->getXnPos();
		rct.top -= g_pWhiteBoardView->getYnPos();
		rct.bottom -= g_pWhiteBoardView->getYnPos();
	}

	DestroyRichWindow();
	ASSERT(NULL != pParent);

	//编辑框最小 100 x 100
	int nWidth = rct.right - rct.left;
	nWidth = nWidth > 450 ? nWidth : 450;
	int nHeight = rct.bottom - rct.top;
	nHeight = nHeight > 200 ? nHeight : 200;
	if (nWidth >450)
	{
		nWidth = rct.right - rct.left;
		nWidth = nWidth < 600 ? nWidth : 600;
		
	}
	if (nHeight >200)
	{
		nHeight = rct.bottom - rct.top;
		nHeight = nHeight < 600 ? nHeight : 600;
	}


	rct.right = rct.left + nWidth;
	rct.bottom = rct.top + nHeight;

	if (g_reduce>0.00)
	{
		rct.left =rct.left*g_reduce;
		rct.right =rct.right*g_reduce;
		rct.top =rct.top*g_reduce;
		rct.bottom =rct.bottom*g_reduce;
	}

	CWBRichEdit *pTempRichEdit = new CWBRichEdit;
	BOOL bCreateOK = pTempRichEdit->CreateEdit(bVertical, rct, pParent);
	if (bCreateOK)
	{
		if (bShowPrompt)
		{
			CWBPromptFontSetDlg *pPromptFontSetDlg = new CWBPromptFontSetDlg;
			int i = g_nWBCurObjFontSize;//保存重新创建字体对话框前的字体大小
			bCreateOK = pPromptFontSetDlg->Create(IDD_PROMPTFONTSETDIALOG, pParent);
			g_nWBCurObjFontSize = i; //把保存的字体大小重新赋值给它
			if (bCreateOK)
			{
				//矫正位置及显示
				CRect rctPrompt(rct);
				rctPrompt.bottom = rctPrompt.top;
				rctPrompt.top -= 32;
				if ((rctPrompt.Width()) < 426)
				{
					rctPrompt.right = rctPrompt.left + 426;
				}
				pPromptFontSetDlg->MoveWindow(&rctPrompt);
				pPromptFontSetDlg->ShowWindow(SW_SHOW);
				m_pPromptFontSetDlg = pPromptFontSetDlg;
				if (NULL != m_pPromptFontSetDlg)
				{
					m_pPromptFontSetDlg->OnWM_TEXTARRIBUTE(0,0);
					/*m_pPromptFontSetDlg->GetFontBoldBtn()->SetIsSelect(g_isBold);
					m_pPromptFontSetDlg->GetFontBoldBtn()->Invalidate();
					m_pPromptFontSetDlg->GetFontItalicBtn()->SetIsSelect(g_isItalic);
					m_pPromptFontSetDlg->GetFontItalicBtn()->Invalidate();
					m_pPromptFontSetDlg->GetFontUnderlineBtn()->SetIsSelect(g_isUnderline);
					m_pPromptFontSetDlg->GetFontUnderlineBtn()->Invalidate();
					m_pPromptFontSetDlg->GetFontVerticalBtn()->SetIsSelect(g_bIsVertical);
					m_pPromptFontSetDlg->GetFontVerticalBtn()->Invalidate();*/
				}
			}
			else
			{
				delete pPromptFontSetDlg;
			}
		}
		pTempRichEdit->MoveWindow(&rct);
		pTempRichEdit->ShowWindow(SW_SHOW);
		pTempRichEdit->SetFocus();
		(pTempRichEdit->GetRichEditCtrl()).LimitText(1024);

		//字体样式luowei
		CHARFORMAT cf;
		ZeroMemory(&cf, sizeof(CHARFORMAT));
		(pTempRichEdit->GetRichEditCtrl()).GetSelectionCharFormat(cf);
		cf.dwMask = CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_SIZE | CFM_UNDERLINE;
		cf.dwEffects = 0;
		if (g_isBold)
		{
			cf.dwEffects += CFE_BOLD;
		}
		if (g_isItalic)
		{
			cf.dwEffects += CFE_ITALIC;
		}
		if (g_isUnderline)
		{
			cf.dwEffects += CFE_UNDERLINE;
		}
		cf.crTextColor = g_clrWBCurObjColor;
		long lLenght = pTempRichEdit->GetTextLength();
		(pTempRichEdit->GetRichEditCtrl()).SetSel(0, lLenght);
		(pTempRichEdit->GetRichEditCtrl()).SetSelectionCharFormat(cf);
		(pTempRichEdit->GetRichEditCtrl()).SetSel(-1, -1);//光标到末尾

		m_pRichEdit = pTempRichEdit;
		m_bRichEditShow = true;
		g_pWhiteBoardView->SetRichEditRect(rct);
		g_pWhiteBoardView->SetEditRect(rct);
	}
	else
	{
		delete pTempRichEdit;
	}
	//编辑文本时自定义字体大小，再次编辑时字体大小控件不显示字体大小
	CWBPromptFontSetDlg* pTempFontdlg = g_pWhiteBoardView->GetPromptFontSetDlg();
	if (NULL != pTempFontdlg && NULL != pTempFontdlg->GetFontSizeComBox())
	{
		pTempFontdlg->GetFontSizeComBox()->SetDlgItemInt(IDC_PROMPTFONTSIZECOMBOX,g_nWBCurObjFontSize,FALSE);
		pTempFontdlg->UpdateData(FALSE);
	}
	pTempFontdlg = NULL;

	if (NULL != g_pWhiteBoardView->GetWBFunctionDlg() && NULL != g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg())
	{
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->m_strFontSize.Format(_T("%d"),g_nWBCurObjFontSize);
		g_pWhiteBoardView->GetWBFunctionDlg()->GetAttributeDlg()->UpdateData(FALSE);
	}

	return bCreateOK;
}

void CWBRichEditMgr::DestroyRichWindow()
{
	XAutoLock lock(m_csMapOperation);
	if (m_bRichEditShow)
	{
		if (m_bIsSaveText)
		{
			g_pWhiteBoardView->DestroyRichWindow();
		}

		if (m_pPromptFontSetDlg)
		{
			if (::IsWindow(m_pPromptFontSetDlg->GetSafeHwnd()))
			{
				m_pPromptFontSetDlg->DestroyWindow();
			}
			delete m_pPromptFontSetDlg;
			m_pPromptFontSetDlg = NULL;
		}

		if (m_pRichEdit)
		{
			if (::IsWindow(m_pRichEdit->GetSafeHwnd()))
			{
				m_pRichEdit->DestroyWindow();
			}
			//delete m_pRichEdit;
			m_pRichEdit = NULL;
		}
		
		m_bRichEditShow = false;
	}

	for (std::map<unsigned long, CRect*>::iterator it = m_mapRect.begin(); m_mapRect.end() != it; ++it)
	{
		delete it->second;
	}
	m_mapRect.clear();

	return;
}

CWBRichEditMgr * CWBRichEditMgr::GetInst_()
{
	return &m_inst;
}

CWBRichEditMgr::CWBRichEditMgr()
	: m_pPromptFontSetDlg(NULL)
	, m_bRichEditShow(false)
	, m_bIsSaveText(true)
{

}

CWBRichEditMgr::~CWBRichEditMgr()
{
	//ASSERT(m_map.empty());
}

void CWBRichEditMgr::SetEndPoint( const unsigned int &nX, const unsigned int &nY, const unsigned long nTouchID /*= MOUSE_ID*/ )
{
	XAutoLock lock(m_csMapOperation);
	std::map<unsigned long, CRect*>::iterator it = m_mapRect.find(nTouchID);
	if (m_mapRect.end() != it)
	{
		CRect *rct = it->second;

		//矫正矩形上下左右位置
		if (rct->left < nX)
		{
			rct->right = nX;
		}
		else
		{
			rct->right = rct->left;
			rct->left = nX;
		}

		if (rct->top < nY)
		{
			rct->bottom = nY;
		}
		else
		{
			rct->bottom = rct->top;
			rct->top = nY;
		}
		
	}
}

void CWBRichEditMgr::SetBeginPoint( const unsigned int &nX, const unsigned int &nY, const unsigned long nTouchID /*= MOUSE_ID*/ )
{
	XAutoLock lock(m_csMapOperation);
	if (!m_bRichEditShow)
	{
		std::map<unsigned long, CRect*>::iterator it = m_mapRect.find(nTouchID);
		if (m_mapRect.end() != it)
		{
			delete it->second;
			m_mapRect.erase(it);
		}
		else
		{
			CRect *rctRichEdit = new CRect(nX, nY, nX, nY);
			m_mapRect.insert(std::pair<unsigned long, CRect*>(nTouchID, rctRichEdit));
		}
	}
}

bool CWBRichEditMgr::GetRect( const unsigned long &nID, CRect &rct )
{
	XAutoLock lock(m_csMapOperation);
	bool bGetRect = false;
	std::map<unsigned long, CRect*>::iterator it = m_mapRect.find(nID);
	if (m_mapRect.end() != it)
	{
		rct = CRect(*(it->second));
		bGetRect = true;
	}
	return bGetRect;
}

void CWBRichEditMgr::SetRichEditText(CString str)
{
	m_pRichEdit->SetWindowText(str);
}

void CWBRichEditMgr::SetRichEdit(CWBRichEdit *pEdit)
{
	m_pRichEdit = pEdit;
}

void CWBRichEditMgr::SetRichEditShow(bool bRichEditShow)
{
	m_bRichEditShow = bRichEditShow;
}

void CWBRichEditMgr::SetIsSaveText(bool bIsSaveText)
{
	m_bIsSaveText = bIsSaveText;
}

CWBRichEditMgr CWBRichEditMgr::m_inst;
