
// MFCtestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCtest.h"
#include "MFCtestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCtestDlg 对话框



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


// CMFCtestDlg 消息处理程序

BOOL CMFCtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_pNetSream = new CNetStream();
	if (m_pNetSream)
	{
		m_pNetSream->startNete();
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCtestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCtestDlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
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
