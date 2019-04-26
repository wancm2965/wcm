// WBEdit.cpp : 实现文件
//
#include "StdAfx.h"
#include "WBEdit.h"
#include "whiteBoard.h"


// CWBEdit

IMPLEMENT_DYNAMIC(CWBEdit, CEdit)
CWBEdit::CWBEdit()
{
	m_clrBackground	= RGB(255, 255, 255);	//背景颜色
	m_clrFrame		= RGB(61, 61, 61);	//边框颜色
// 	m_clrText		= RGB(110, 90, 95);		//文字颜色

// 	m_clrDisableTxt	= RGB(185, 190, 170);	//禁用文字色
// 	m_clrDisableBkg	= RGB(230, 235, 210);	//禁用背景色

	m_brushBkg.CreateSolidBrush(m_clrBackground);//创建背景画刷
	m_bIsWritePageCount = false;
	m_bIsHave = false;
}

CWBEdit::~CWBEdit()
{
	m_bIsWritePageCount = false;
}


BEGIN_MESSAGE_MAP(CWBEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CWBEdit 消息处理程序
HBRUSH CWBEdit::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	return (HBRUSH)m_brushBkg.GetSafeHandle();
}

void CWBEdit::OnNcPaint()
{
	CRect	rectEdit, rectInser;
	CBrush	brushInser, brushOuter;
	CDC*	pDC= GetWindowDC();

	//设置编辑框的区域
	GetWindowRect(rectEdit);
	ScreenToClient(rectEdit);
	rectEdit.OffsetRect(CSize(2, 2));
	rectInser.SetRect(rectEdit.left+1, rectEdit.top+1, rectEdit.right-1, rectEdit.bottom-1);

	//创建画笔
	brushInser.CreateSolidBrush(m_clrBackground);
	brushOuter.CreateSolidBrush(m_clrFrame);

	//画边框
	pDC->FrameRect(rectEdit, &brushOuter);
	pDC->FrameRect(rectInser, &brushInser);

	ReleaseDC(pDC);
}

void CWBEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (g_bAvconLock)
	{
		return;
	}
	if (nChar == 8)
	{
		g_pWhiteBoardView->SetIsKeyDel(TRUE);
	} 
	else
	{
		g_pWhiteBoardView->SetIsKeyDel(FALSE);
	}
	if (m_bIsWritePageCount)
	{
		if ((nChar >= 48 && nChar <= 57) || nChar == 45 || nChar == 44 || nChar == 8)
		{
			;
		}
		else
		{
			CString strPrompt;
			strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
			if (strPrompt.IsEmpty())
			{
				strPrompt = _T("提示");
			}		
			CString strFolderCreateFail;
			strFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("SavePageToPicture"));
			if (strFolderCreateFail.IsEmpty())
			{
				strFolderCreateFail = _T("只能输入数字，‘-’‘，’字符");
			}
			if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
			{
				//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strFolderCreateFail, MB_OK,true);
				g_pHPWBDataNotify->ShowTipWindow(strFolderCreateFail);
			}
			else
			{
				AfxMessageBox(strFolderCreateFail);
			}
			return;
		}
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CWBEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认
	if (g_bAvconLock)
	{
		return;
	}
	/************************************************************************/
	//判定‘-’字符只能有一个
	if ( nChar != 45)
	{
		m_bIsHave = false;
	}
	if (m_bIsHave)
	{
		return;
	}
	if (nChar == 45)
	{
		m_bIsHave = true;
	}                                                                
	/************************************************************************/
	if (IsCTRLPressed()) //按下CTRL键时就清空剪切板
	{
		if(OpenClipboard())//打开剪切板
		{
			EmptyClipboard();//清空剪切板
			CloseClipboard();//关闭
		}
		return;
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
