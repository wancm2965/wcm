
// MFCtestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCtest.h"
#include "MFCtestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCtestDlg �Ի���



CMFCtestDlg::CMFCtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCtestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pNetSream = NULL;
	m_precord = NULL;
	m_pfilerecord = NULL;
}

void CMFCtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CMFCtestDlg::OnCompoundResut(int nResult)
{
	if (nResult == 0)
	{

	}

	if (m_pfilerecord)
	{
		delete m_pfilerecord;
		m_pfilerecord = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMFCtestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCtestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCtestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDOK, &CMFCtestDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMFCtestDlg ��Ϣ�������

BOOL CMFCtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	m_pNetSream = new CNetStream();
	if (m_pNetSream)
	{
		m_pNetSream->startNete();
	}
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCtestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCtestDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	 m_precord = IFFmpegrecord::Create();
	 if (m_precord)
	 {
	 	std::string strfilename = "d:\\ffmpegnet.mp4";
	 	m_precord->Open(strfilename.c_str());
	 }
	 if (m_pNetSream)
	 {
	 	m_pNetSream->SetReadStreamNotify(m_precord);
	 	m_pNetSream->openNete();
	 }
}

void CMFCtestDlg::OnBnClickedButton2()
{
	if (m_pNetSream)
	{
		m_pNetSream->SetReadStreamNotify(NULL);
	}
}

void CMFCtestDlg::OnBnClickedOk()
{
	if (m_pNetSream)
	{
		delete m_pNetSream;
		m_pNetSream = NULL;
	}
	if (m_precord)
	{
		delete m_precord;
		m_precord = NULL;
	}
	CDialogEx::OnOK();
}
