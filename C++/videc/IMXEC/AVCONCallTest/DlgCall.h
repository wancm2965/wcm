#pragma once
#include "afxwin.h"


// DlgCall �Ի���

class DlgCall : public CDialog
{
	DECLARE_DYNAMIC(DlgCall)

public:
	DlgCall(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgCall();

// �Ի�������
	enum { IDD = IDD_DIALOG_CALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUserID;
	afx_msg void OnEnChangeEditUserID();

	CButton m_btnOK;
	virtual BOOL OnInitDialog();
	CEdit m_editUserID;
};
