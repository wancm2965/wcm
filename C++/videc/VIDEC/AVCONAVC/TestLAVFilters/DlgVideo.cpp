// DlgVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "TestLAVFilters.h"
#include "DlgVideo.h"
#include "afxdialogex.h"


// DlgVideo 对话框

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)

DlgVideo::DlgVideo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgVideo::IDD, pParent)
{
	m_pVideoDraw=NULL;
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
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// DlgVideo 消息处理程序


BOOL DlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


BOOL DlgVideo::DestroyWindow()
{
	if (m_pVideoDraw)
	{
		m_pVideoDraw->Close();
		delete m_pVideoDraw;
		m_pVideoDraw=NULL;
	}

	return CDialog::DestroyWindow();
}


void DlgVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}


void DlgVideo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if (m_pVideoDraw)
	{
		m_pVideoDraw->Repaint();
	}
}


BOOL DlgVideo::OnEraseBkgnd(CDC* pDC)
{

	return FALSE;
}

void DlgVideo::ResizeControls(void)
{
	CRect rc;
	GetClientRect(rc);
	if (m_pVideoDraw)
	{
		m_pVideoDraw->SetWndPos(0,0,rc.Width(),rc.Height());
	}
}