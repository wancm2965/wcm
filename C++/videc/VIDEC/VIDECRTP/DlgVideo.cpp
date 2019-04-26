// DlgVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "VIDECRTP.h"
#include "DlgVideo.h"
#include "DlgMain.h"
#include ".\dlgvideo.h"


// DlgVideo 对话框

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)
DlgVideo::DlgVideo(DlgMain&rDlgMain)
	: CDialog(DlgVideo::IDD, &rDlgMain)
	, m_pWnd(NULL)
	, m_rDlgMain(rDlgMain)
{
}

DlgVideo::~DlgVideo()
{
}

void DlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgVideo, CDialog)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DlgVideo 消息处理程序

void DlgVideo::OnOK()
{
}

void DlgVideo::OnCancel()
{
}

BOOL DlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

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

	ResizeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgVideo::DestroyWindow()
{
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}

	return CDialog::DestroyWindow();
}

void DlgVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rDlgMain.SetFullScreen(this);

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgVideo::ResizeControls(void)
{
	if (m_pWnd!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}
//
//BOOL DlgVideo::OnEraseBkgnd(CDC* pDC)
//{
//	return FALSE;
//}
