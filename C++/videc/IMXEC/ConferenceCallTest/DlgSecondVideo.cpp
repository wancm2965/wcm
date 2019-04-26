// DlgSecondVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "ConferenceCallTest.h"
#include "DlgSecondVideo.h"


// DlgSecondVideo 对话框

IMPLEMENT_DYNAMIC(DlgSecondVideo, CDialog)

DlgSecondVideo::DlgSecondVideo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgSecondVideo::IDD, pParent)
{
	m_strCallID="";
}

DlgSecondVideo::~DlgSecondVideo()
{
}

void DlgSecondVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgSecondVideo, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// DlgSecondVideo 消息处理程序

BOOL DlgSecondVideo::OnInitDialog()
{
	CDialog::OnInitDialog();


	IMXEC_Call::SetSecondVideoWnd(m_strCallID.c_str(),m_hWnd);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgSecondVideo::DestroyWindow()
{
	IMXEC_Call::SetSecondVideoWnd(m_strCallID.c_str(),NULL);

	return CDialog::DestroyWindow();
}

void DlgSecondVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);
	IMXEC_Call::SetSecondVideoWndPos(m_strCallID.c_str(),0,0,rc.Width(),rc.Height());
}
