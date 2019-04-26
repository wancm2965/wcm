// DlgFullScreen.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDecoder.h"
#include "DlgFullScreen.h"


// DlgFullScreen 对话框

IMPLEMENT_DYNAMIC(DlgFullScreen, CDialog)
DlgFullScreen::DlgFullScreen(CWnd* pParent)
	: CDialog(DlgFullScreen::IDD, pParent)
	, m_pWndChild(NULL)
{
}

DlgFullScreen::~DlgFullScreen()
{
}

void DlgFullScreen::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgFullScreen, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DlgFullScreen 消息处理程序

void DlgFullScreen::OnOK()
{
}

void DlgFullScreen::OnCancel()
{
}

BOOL DlgFullScreen::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgFullScreen::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgFullScreen::ResizeControls(void)
{
	if (m_pWndChild!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pWndChild->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

void DlgFullScreen::SetWndChild(CWnd*pWndChild)
{
	m_pWndChild=pWndChild;
	if (m_pWndChild)
	{
		m_pWndChild->SetParent(this);
	}

	ResizeControls();
}

