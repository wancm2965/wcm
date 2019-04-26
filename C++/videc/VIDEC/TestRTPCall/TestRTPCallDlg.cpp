
// TestRTPCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestRTPCall.h"
#include "TestRTPCallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestRTPCallDlg dialog




CTestRTPCallDlg::CTestRTPCallDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestRTPCallDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pVideoRTPCall=NULL;
	m_pLocalDlgVideo=NULL;
	m_pPeerDlgVideo=NULL;
}

void CTestRTPCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestRTPCallDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DEVMODECHANGE()
END_MESSAGE_MAP()


// CTestRTPCallDlg message handlers

BOOL CTestRTPCallDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowPos(NULL,0,0,1280,720,NULL);

	VIDEC_Init();

	m_pLocalDlgVideo=new DlgVideo(*this,true);
	m_pLocalDlgVideo->Create(DlgVideo::IDD,this);
	m_pPeerDlgVideo=new DlgVideo(*this,false);
	m_pPeerDlgVideo->Create(DlgVideo::IDD,this);

	m_pVideoRTPCall=VIDEC_RTPCall::Create();
	if (m_pVideoRTPCall!=NULL)
	{
		int nWidth=320;
		int nHeight=240;
		int nFrameRate=30;
		int nBitrate=512;
		if (0!=m_pVideoRTPCall->Open(m_pLocalDlgVideo->m_hWnd,0,nWidth,nHeight,nFrameRate,nBitrate,VIDEC_CODEC_H264,97,"192.168.2.122",2000,m_pPeerDlgVideo->m_hWnd,"192.168.2.122",2000))
		//if (0!=m_pVideoRTPCall->Open(m_pLocalDlgVideo->m_hWnd,0,nWidth,nHeight,nFrameRate,nBitrate,VIDEC_CODEC_H264_SVC,97,"192.168.2.122",21000,m_pPeerDlgVideo->m_hWnd,"191.8.1.21",21000))
		{
			m_pVideoRTPCall->Close();
			delete m_pVideoRTPCall;
			m_pVideoRTPCall=NULL;
		}
	}

	ResizeControls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestRTPCallDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestRTPCallDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CTestRTPCallDlg::DestroyWindow()
{
	if (m_pVideoRTPCall!=NULL)
	{
		m_pVideoRTPCall->Close();
		delete m_pVideoRTPCall;
		m_pVideoRTPCall=NULL;
	}

	if (m_pLocalDlgVideo!=NULL)
	{
		m_pLocalDlgVideo->DestroyWindow();
		delete m_pLocalDlgVideo;
		m_pLocalDlgVideo=NULL;
	}

	if (m_pPeerDlgVideo!=NULL)
	{
		m_pPeerDlgVideo->DestroyWindow();
		delete m_pPeerDlgVideo;
		m_pPeerDlgVideo=NULL;
	}

	VIDEC_Terminate();

	return CDialog::DestroyWindow();
}

void CTestRTPCallDlg::ResizeControls(void)
{
	CRect rc;
	GetClientRect(rc);

	if (m_pLocalDlgVideo!=NULL)
	{
		m_pLocalDlgVideo->SetWindowPos(NULL,0,0,rc.Width()/2,rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	if (m_pPeerDlgVideo!=NULL)
	{
		m_pPeerDlgVideo->SetWindowPos(NULL,rc.Width()/2,0,rc.Width()/2,rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

void CTestRTPCallDlg::SetFullScreen(void)
{

}

LRESULT CTestRTPCallDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message==WM_DISPLAYCHANGE)
	{
		message=message;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CTestRTPCallDlg::OnDevModeChange(LPTSTR lpDeviceName)
{
	CDialog::OnDevModeChange(lpDeviceName);

	// TODO: 在此处添加消息处理程序代码
}
