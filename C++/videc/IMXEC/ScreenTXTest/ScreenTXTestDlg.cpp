// ScreenTXTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenTXTest.h"
#include "ScreenTXTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenTXTestDlg 对话框




CScreenTXTestDlg::CScreenTXTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenTXTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pScreenTX=NULL;
}

void CScreenTXTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PEER_NODEID, m_editPeerNodeID);
	DDX_Control(pDX, IDC_CHECK_VIEW_ONLY, m_btnViewOnly);
}

BEGIN_MESSAGE_MAP(CScreenTXTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CScreenTXTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_CHECK_VIEW_ONLY, &CScreenTXTestDlg::OnBnClickedCheckViewOnly)
END_MESSAGE_MAP()


// CScreenTXTestDlg 消息处理程序

BOOL CScreenTXTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	NETEC_Core::Start(0);
	NETEC_Node::SetServerIP("220.231.143.229");
	NETEC_Node::SetServerPort(4222);
	NETEC_Node::Start();

	//m_pScreenTX=IMXEC_ScreenTX::Create(*this);
	//if (m_pScreenTX!=NULL)
	//{
	//	//m_pScreenTX->Start();
	//}
	//m_pVNCClient=VNCEC_VNCClient::Create(*this);
	//if (m_pVNCClient!=NULL)
	//{
	//	//m_pVNCClient->Start("XMC","123");
	//}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CScreenTXTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CScreenTXTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CScreenTXTestDlg::DestroyWindow()
{
	if (m_pScreenTX!=NULL)
	{
		m_pScreenTX->Stop();
		delete m_pScreenTX;
		m_pScreenTX=NULL;
	}

	NETEC_Node::Stop();
	NETEC_Core::Stop();

	return CDialog::DestroyWindow();
}

void CScreenTXTestDlg::OnBnClickedButtonConnect()
{
	CString strPeerNodeID="";
	m_editPeerNodeID.GetWindowText(strPeerNodeID);
	if (strPeerNodeID.GetLength())
	{
		if (m_pScreenTX!=NULL)
		{
			m_pScreenTX->Stop();
			delete m_pScreenTX;
			m_pScreenTX=NULL;
		}

		m_pScreenTX=IMXEC_ScreenTX::Create(*this);
		if (m_pScreenTX!=NULL)
		{
			m_pScreenTX->Start(strPeerNodeID);
			unsigned long nScreenID=m_pScreenTX->GetScreenID();
			NETEC_Node::SendDataToNode(0,0,strPeerNodeID,(char*)&nScreenID,4);
		}
	}
}

void CScreenTXTestDlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs==NETEC_Session::CS_CONNECTED)
	{
		TRACE("NodeID=%s\n",NETEC_Node::GetNodeID());
	}
}

void CScreenTXTestDlg::OnBnClickedCheckViewOnly()
{
	if (m_pScreenTX!=NULL)
	{
		if (m_btnViewOnly.GetCheck())
		{
			m_pScreenTX->SetRemoteViewOnly(1);
		}
		else
		{
			m_pScreenTX->SetRemoteViewOnly(0);
		}
	}
}
