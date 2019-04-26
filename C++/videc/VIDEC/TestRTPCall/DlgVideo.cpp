// DlgVideo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestRTPCall.h"
#include "DlgVideo.h"
#include "TestRTPCallDlg.h"


// DlgVideo �Ի���

IMPLEMENT_DYNAMIC(DlgVideo, CDialog)
DlgVideo::DlgVideo(CTestRTPCallDlg&rTestRTPCallDlg,bool bLocal)
	: CDialog(DlgVideo::IDD, &rTestRTPCallDlg)
	, m_rTestRTPCallDlg(rTestRTPCallDlg)
	, m_bLocal(bLocal)
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
END_MESSAGE_MAP()


// DlgVideo ��Ϣ�������

void DlgVideo::OnOK()
{
}

void DlgVideo::OnCancel()
{
}

BOOL DlgVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	ResizeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL DlgVideo::DestroyWindow()
{
	return CDialog::DestroyWindow();
}

void DlgVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rTestRTPCallDlg.SetFullScreen();

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgVideo::ResizeControls(void)
{
	if (m_rTestRTPCallDlg.m_pVideoRTPCall!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		if (m_bLocal)
		{
			m_rTestRTPCallDlg.m_pVideoRTPCall->SetLocalWndPos(0,0,rc.Width(),rc.Height());
		}
		else
		{
			m_rTestRTPCallDlg.m_pVideoRTPCall->SetPeerWndPos(0,0,rc.Width(),rc.Height());
		}
	}
}
