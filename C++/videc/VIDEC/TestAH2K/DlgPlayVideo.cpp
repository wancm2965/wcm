// DlgPlayVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAH2K.h"
#include "DlgPlayVideo.h"
#include "DlgMain.h"

// DlgPlayVideo 对话框

IMPLEMENT_DYNAMIC(DlgPlayVideo, CDialog)
DlgPlayVideo::DlgPlayVideo(DlgMain&rDlgMain)
	: CDialog(DlgPlayVideo::IDD, &rDlgMain)
#ifdef TEST_PLAYER
	, m_pPlayer(NULL)
#else
	, m_pWnd(NULL)
#endif
	, m_rDlgMain(rDlgMain)
{
}

DlgPlayVideo::~DlgPlayVideo()
{
}

void DlgPlayVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgPlayVideo, CDialog)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// DlgPlayVideo 消息处理程序

void DlgPlayVideo::OnOK()
{
}

void DlgPlayVideo::OnCancel()
{
}

BOOL DlgPlayVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifndef TEST_PLAYER
	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		if (0!=m_pWnd->Open(m_hWnd))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
	}
#endif

	ResizeControls();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgPlayVideo::DestroyWindow()
{
#ifndef TEST_PLAYER
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}
#endif

	return CDialog::DestroyWindow();
}

void DlgPlayVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgPlayVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rDlgMain.SetFullScreen();

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgPlayVideo::ResizeControls(void)
{
#ifdef TEST_PLAYER
	if (m_pPlayer!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pPlayer->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
#else
	if (m_pWnd!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
#endif
}
