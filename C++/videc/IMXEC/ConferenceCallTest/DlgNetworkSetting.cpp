// DlgNetworkSetting.cpp : 实现文件
//

#include "stdafx.h"
#include "ConferenceCallTest.h"
#include "DlgNetworkSetting.h"


// DlgNetworkSetting 对话框

IMPLEMENT_DYNAMIC(DlgNetworkSetting, CDialog)

DlgNetworkSetting::DlgNetworkSetting(CWnd* pParent /*=NULL*/)
	: CDialog(DlgNetworkSetting::IDD, pParent)
	, m_nH323ListenPort(0)
	, m_nRTPBasePort(0)
	, m_strLocalIP(_T(""))
	, m_nSupportH239(FALSE)
{

}

DlgNetworkSetting::~DlgNetworkSetting()
{
}

void DlgNetworkSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_H323_LISTEN_PORT, m_nH323ListenPort);
	DDV_MinMaxUInt(pDX, m_nH323ListenPort, 1000, 60000);
	DDX_Text(pDX, IDC_EDIT_RTP_BASE_PORT, m_nRTPBasePort);
	DDV_MinMaxUInt(pDX, m_nRTPBasePort, 1000, 60000);
	DDX_Control(pDX, IDC_LOCAL_IPADDRESS, m_ipctrl_LocalIP);
	DDX_Control(pDX, IDC_CHECK_SUPPORT_H239, m_btnSupportH239);
	DDX_Check(pDX, IDC_CHECK_SUPPORT_H239, m_nSupportH239);
}


BEGIN_MESSAGE_MAP(DlgNetworkSetting, CDialog)
	ON_BN_CLICKED(IDOK, &DlgNetworkSetting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_SUPPORT_H239, &DlgNetworkSetting::OnBnClickedCheckSupportH239)
END_MESSAGE_MAP()


// DlgNetworkSetting 消息处理程序

void DlgNetworkSetting::OnBnClickedOk()
{
	m_ipctrl_LocalIP.GetWindowText(m_strLocalIP);

	OnOK();
}

BOOL DlgNetworkSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ipctrl_LocalIP.SetWindowText(m_strLocalIP);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void DlgNetworkSetting::OnBnClickedCheckSupportH239()
{
	m_nSupportH239=m_btnSupportH239.GetCheck();
}
