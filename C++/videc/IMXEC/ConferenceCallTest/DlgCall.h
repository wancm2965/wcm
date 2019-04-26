#pragma once
#include "afxwin.h"


// DlgCall 对话框

class DlgCall : public CDialog
{
	DECLARE_DYNAMIC(DlgCall)

public:
	DlgCall(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgCall();

// 对话框数据
	enum { IDD = IDD_DIALOG_CALL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPeerIP;
	CComboBox m_cbxProto;
	afx_msg void OnEnChangeEditPeerIp();
	CString m_strProto;
	CButton m_btnOK;
	virtual BOOL OnInitDialog();
	CEdit m_editPeerIP;
};
