// DlgVideo.cpp : 实现文件
//

#include "stdafx.h"

#include "DlgVideo.h"

#include "TestMPClientDlg.h"

// DlgVideo 对话框

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)

DlgVideo::DlgVideo(int nLocal,CWnd* pParent /*=NULL*/)
	: CDialog(DlgVideo::IDD, pParent)
	, m_nLocal(nLocal)
{
	m_bShowMax=false;
}

DlgVideo::~DlgVideo()
{
}

void DlgVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgVideo, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DlgVideo 消息处理程序

BOOL DlgVideo::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->FillRect(rc,&CBrush(RGB(0,0,0)));

	return TRUE;
}

void DlgVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_bShowMax = !m_bShowMax;

	if (m_bShowMax)
	{
		ModifyStyle(WS_CAPTION,0);
		ShowWindow(SW_MAXIMIZE);
	}
	else
	{
		ModifyStyle(0,WS_CAPTION);
		ShowWindow(SW_RESTORE);
	}


	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CTestMPClientDlg::Instance()->ReSetVideoSize(m_nLocal);
}

BOOL DlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	//ModifyStyle(WS_CHILD|WS_SYSMENU,WS_CAPTION|WS_DLGFRAME|WS_SIZEBOX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
