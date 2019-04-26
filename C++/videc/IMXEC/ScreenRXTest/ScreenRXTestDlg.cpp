// ScreenRXTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenRXTest.h"
#include "ScreenRXTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenRXTestDlg �Ի���




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


// CScreenRXTestDlg ��Ϣ�������

BOOL CScreenRXTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

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


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CScreenRXTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
			m_pScreenRX->Start("��ģ��",strPeerNodeID,m_nVNCServerID,"MCU-001","220.231.143.229",4222,"P");
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
