#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// DlgLogin �Ի���

class DlgLogin : public CDialog
{
	DECLARE_DYNAMIC(DlgLogin)

public:
	DlgLogin(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgLogin();

	CString		m_strServerIP;
	CString		m_strUserID;
	CString		m_strPassword;

// �Ի�������
	enum { IDD = IDD_DIALOG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	CIPAddressCtrl m_ctrServerIP;
	CEdit m_ctrUserID;
	CEdit m_ctrPassword;
	CButton m_btnOK;
	afx_msg void OnBnClickedOk();
};
