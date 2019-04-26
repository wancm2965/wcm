#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// DlgNetworkSetting �Ի���

class DlgNetworkSetting : public CDialog
{
	DECLARE_DYNAMIC(DlgNetworkSetting)

public:
	DlgNetworkSetting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgNetworkSetting();

// �Ի�������
	enum { IDD = IDD_DIALOG_NETWORK_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	DWORD			m_nH323ListenPort;
	DWORD			m_nRTPBasePort;
	CString			m_strLocalIP;
	CIPAddressCtrl	m_ipctrl_LocalIP;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CButton m_btnSupportH239;
	BOOL m_nSupportH239;
	afx_msg void OnBnClickedCheckSupportH239();
};
