// DlgLogin.cpp : 实现文件
//

#include "stdafx.h"
#include "AVCONCallTest.h"
#include "DlgLogin.h"


// DlgLogin 对话框

IMPLEMENT_DYNAMIC(DlgLogin, CDialog)

DlgLogin::DlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(DlgLogin::IDD, pParent)
{
	m_strServerIP=_T("");
	m_strUserID=_T("");
	m_strPassword=_T("");
}

DlgLogin::~DlgLogin()
{
}

void DlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_IP, m_ctrServerIP);
	DDX_Control(pDX, IDC_EDIT_USERID, m_ctrUserID);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_ctrPassword);
	DDX_Control(pDX, IDOK, m_btnOK);
}


BEGIN_MESSAGE_MAP(DlgLogin, CDialog)
	ON_BN_CLICKED(IDOK, &DlgLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgLogin 消息处理程序

BOOL DlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrServerIP.SetWindowText(m_strServerIP);
	m_ctrUserID.SetWindowText(m_strUserID);
	m_ctrPassword.SetWindowText(m_strPassword);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void DlgLogin::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialog::OnOK();
}

void DlgLogin::OnBnClickedOk()
{
	m_ctrServerIP.GetWindowText(m_strServerIP);
	m_ctrUserID.GetWindowText(m_strUserID);
	m_ctrPassword.GetWindowText(m_strPassword);

	OnOK();
}
