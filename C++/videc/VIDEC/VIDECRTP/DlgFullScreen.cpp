// DlgFullScreen.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VIDECRTP.h"
#include "DlgFullScreen.h"
#include ".\dlgfullscreen.h"


// DlgFullScreen �Ի���

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
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DlgFullScreen ��Ϣ�������

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
	// �쳣: OCX ����ҳӦ���� FALSE
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


BOOL DlgFullScreen::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
