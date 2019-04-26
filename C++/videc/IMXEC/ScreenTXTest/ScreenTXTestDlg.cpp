// ScreenTXTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScreenTXTest.h"
#include "ScreenTXTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CScreenTXTestDlg �Ի���




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


// CScreenTXTestDlg ��Ϣ�������

BOOL CScreenTXTestDlg::OnInitDialog()
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


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CScreenTXTestDlg::OnPaint()
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
