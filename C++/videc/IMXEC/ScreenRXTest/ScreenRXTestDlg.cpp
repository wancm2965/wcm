// ScreenRXTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScreenRXTest.h"
#include "ScreenRXTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenRXTestDlg 对话框




CScreenRXTestDlg::CScreenRXTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScreenRXTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pScreenRX=NULL;
	
	m_strPeerNodeID="";
	m_nVNCServerID=0;
}

void CScreenRXTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PEER_NODEID, m_editPeerNodeID);
}

BEGIN_MESSAGE_MAP(CScreenRXTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CScreenRXTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CScreenRXTestDlg::OnBnClickedButtonDisconnect)
END_MESSAGE_MAP()


// CScreenRXTestDlg 消息处理程序

BOOL CScreenRXTestDlg::OnInitDialog()
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

	//m_pVNCServer=VNCEC_VNCServer::Create(*this);
	//if (m_pVNCServer!=NULL)
	//{
	//	//m_pVNCServer->Start();
	//}
	//m_pScreenRX=IMXEC_ScreenRX::Create(*this);
	//if (m_pScreenRX!=NULL)
	//{
	//	//m_pScreenRX->Start("XMC","123");
	//}


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CScreenRXTestDlg::OnPaint()
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
HCURSOR CScreenRXTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CScreenRXTestDlg::DestroyWindow()
{
	if (m_pScreenRX!=NULL)
	{
		m_pScreenRX->Stop();
		delete m_pScreenRX;
		m_pScreenRX=NULL;
	}

	NETEC_Node::Stop();
	NETEC_Core::Stop();

	return CDialog::DestroyWindow();
}

void CScreenRXTestDlg::OnBnClickedButtonConnect()
{
	CString strPeerNodeID="";
	m_editPeerNodeID.GetWindowText(strPeerNodeID);
	if (strPeerNodeID.GetLength())
	{
		if (m_pScreenRX!=NULL)
		{
			m_pScreenRX->Stop();
			delete m_pScreenRX;
			m_pScreenRX=NULL;
		}

		m_pScreenRX=IMXEC_ScreenRX::Create(*this);
		if (m_pScreenRX!=NULL)
		{
			m_pScreenRX->Start("熊模昌",strPeerNodeID,m_nVNCServerID,"MCU-001","220.231.143.229",4222,"P");
		}
	}
}

void CScreenRXTestDlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs==NETEC_Session::CS_CONNECTED)
	{
		TRACE("NodeID=%s\n",NETEC_Node::GetNodeID());
	}
}

void CScreenRXTestDlg::OnNETEC_NodeReceivedFromNode(const char*cszNodeID,unsigned int nFromAgentID,unsigned int nToAgentID,const char* pData,int nLen)
{
	m_strPeerNodeID=cszNodeID;
	if (pData!=NULL || nLen==4)
		m_nVNCServerID=*(unsigned long*)pData;
	m_editPeerNodeID.SetWindowText(cszNodeID);
}
void CScreenRXTestDlg::OnBnClickedButtonDisconnect()
{
	if (m_pScreenRX!=NULL)
	{
		m_pScreenRX->Stop();
		delete m_pScreenRX;
		m_pScreenRX=NULL;
	}
}
