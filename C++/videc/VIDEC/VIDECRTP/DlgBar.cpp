// DlgBar.cpp : 实现文件
//

#include "stdafx.h"
#include "VIDECRTP.h"
#include "DlgBar.h"
#include "DlgMain.h"
#include ".\dlgbar.h"


// DlgBar 对话框

IMPLEMENT_DYNAMIC(DlgBar, CDialog)
DlgBar::DlgBar(DlgMain&rDlgMain)
	: CDialog(DlgBar::IDD, &rDlgMain)
	, m_rDlgMain(rDlgMain)
{
}

DlgBar::~DlgBar()
{
}

void DlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_SETTING, m_btnSetting);
	DDX_Control(pDX, IDC_BUTTON_CLOSE, m_btnClose);
	DDX_Control(pDX, IDC_BUTTON_KEY_FRAME, m_btnKeyFrame);
}


BEGIN_MESSAGE_MAP(DlgBar, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnBnClickedButtonClose)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_KEY_FRAME, OnBnClickedButtonKeyFrame)
END_MESSAGE_MAP()


// DlgBar 消息处理程序

void DlgBar::OnOK()
{
}

void DlgBar::OnCancel()
{
}

void DlgBar::OnBnClickedButtonStart()
{
	m_rDlgMain.DoStart();

	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);
	m_btnKeyFrame.EnableWindow(TRUE);
	m_btnSetting.EnableWindow(FALSE);
}

void DlgBar::OnBnClickedButtonStop()
{
	m_rDlgMain.DoStop();

	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
	m_btnKeyFrame.EnableWindow(FALSE);
	m_btnSetting.EnableWindow(TRUE);
}

void DlgBar::OnBnClickedButtonSetting()
{
	m_rDlgMain.DoSetting();
}

void DlgBar::OnBnClickedButtonClose()
{
	m_rDlgMain.PostMessage(WM_CLOSE);
}

void DlgBar::ResizeControls(void)
{
	CRect rc;
	if (m_btnStart.GetSafeHwnd())
	{
		int nOffset=0;
		m_btnStart.GetWindowRect(rc);
	
		m_btnStart.SetWindowPos(NULL,nOffset,0,rc.Width(),rc.Height(),SWP_NOACTIVATE);
		nOffset+=1;
		nOffset+=rc.Width();
	
		m_btnStop.SetWindowPos(NULL,nOffset,0,rc.Width(),rc.Height(),SWP_NOACTIVATE);
		nOffset+=1;
		nOffset+=rc.Width();

		m_btnKeyFrame.SetWindowPos(NULL,nOffset,0,rc.Width(),rc.Height(),SWP_NOACTIVATE);
		nOffset+=1;
		nOffset+=rc.Width();
	
		m_btnSetting.SetWindowPos(NULL,nOffset,0,rc.Width(),rc.Height(),SWP_NOACTIVATE);

		CRect rcClient;
		GetClientRect(rcClient);
		nOffset=rcClient.Width()-rc.Width();
		m_btnClose.SetWindowPos(NULL,nOffset,0,rc.Width(),rc.Height(),SWP_NOACTIVATE);
	}
}


void DlgBar::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

BOOL DlgBar::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);
	m_btnKeyFrame.EnableWindow(FALSE);
	m_btnSetting.EnableWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgBar::OnBnClickedButtonKeyFrame()
{
	if (m_rDlgMain.m_pCapChanRTP!=NULL)
	{
		m_rDlgMain.m_pCapChanRTP->RequestKeyFrame();
	}
}
