// DlgCapVideo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TestDecoder.h"
#include "DlgCapVideo.h"
#include "DlgMain.h"


// DlgCapVideo �Ի���

IMPLEMENT_DYNAMIC(DlgCapVideo, CDialog)
DlgCapVideo::DlgCapVideo(DlgMain&rDlgMain)
	: CDialog(DlgCapVideo::IDD, &rDlgMain)
	, m_pWnd(NULL)
	, m_rDlgMain(rDlgMain)
{
}

DlgCapVideo::~DlgCapVideo()
{
}

void DlgCapVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgCapVideo, CDialog)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// DlgCapVideo ��Ϣ�������

void DlgCapVideo::OnOK()
{
}

void DlgCapVideo::OnCancel()
{
}

BOOL DlgCapVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pWnd=VIDEC_Wnd::Create();
	if (m_pWnd!=NULL)
	{
		if (0!=m_pWnd->Open(m_hWnd,true))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
	}

	ResizeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL DlgCapVideo::DestroyWindow()
{
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}

	return CDialog::DestroyWindow();
}

void DlgCapVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgCapVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rDlgMain.SetFullScreen();

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgCapVideo::ResizeControls(void)
{
	if (m_pWnd!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pWnd->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}
