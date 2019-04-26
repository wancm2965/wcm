// WBToolbarCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBToolbarCtrlDlg.h"
#include "afxdialogex.h"
#include "WhiteBoardDlg.h"
#include "../WB/whiteBoard/whiteBoard.h"
#include "../WB/whiteBoard/common/WBMedia/WBGlobalMediaCtrl.h"



// CWBToolbarCtrlDlg 对话框

IMPLEMENT_DYNAMIC(CWBToolbarCtrlDlg, CDialog)
CWBToolbarCtrlDlg::CWBToolbarCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWBToolbarCtrlDlg::IDD, pParent)
{
	m_isShowToolbar = 1;
}

CWBToolbarCtrlDlg::~CWBToolbarCtrlDlg()
{
}

void CWBToolbarCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TOOLBARBTN, m_toolbarBtn);
}


BEGIN_MESSAGE_MAP(CWBToolbarCtrlDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_TOOLBARBTN, &CWBToolbarCtrlDlg::OnClickedToolbarbtn)
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()


// CWBToolbarCtrlDlg 消息处理程序


BOOL CWBToolbarCtrlDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CDialog::OnEraseBkgnd(pDC);
}


BOOL CWBToolbarCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_toolbarBtn.SetType(TYPE_TOOLBAR_SHOW);
	CString strToolBtn;
	strToolBtn=ReadConfigText(_T("WhiteBoardControlText"), _T("TYPE_TOOLBTN"));
	if (strToolBtn.IsEmpty())
	{
		strToolBtn=_T("工具条");
	}
	m_toolbarBtn.SetWindowText(strToolBtn);
	m_toolbarBtn.MoveWindow(0,0,WB_CTRLBTNDLG_WIDTH,WB_CTRLBTNDLG_HEIGHT);	
	//m_toolbarBtn.MoveWindow(0,0,50,50);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CWBToolbarCtrlDlg::OnClickedToolbarbtn()
{
	m_isShowToolbar *= -1;
	if (0 < m_isShowToolbar)
	{
		m_toolbarBtn.SetType(TYPE_TOOLBAR_SHOW);
		m_toolbarBtn.Invalidate();
	}
	else
	{
		m_toolbarBtn.SetType(TYPE_TOOLBAR_HIDE);
		m_toolbarBtn.Invalidate();
	}
	((CWhiteBoardDlg*)GetParent())->HideShowToolbar();
	//将是否显示工具栏传给新视频控制对话框
// 	if (g_pWhiteBoardView->GetMediaCtrl() != NULL)
// 	{
// 		g_pWhiteBoardView->GetMediaCtrl()->SetToolBarHideOrNot(m_isShowToolbar);
// 	}
}

void CWBToolbarCtrlDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}


void CWBToolbarCtrlDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnCancel();
}


void CWBToolbarCtrlDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWBAboutDlg m_WBAboutDlg;
	m_WBAboutDlg.DoModal();
	CDialog::OnRButtonDblClk(nFlags, point);
}
