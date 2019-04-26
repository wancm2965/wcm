#pragma once


// DlgDialPad 对话框

class DlgDialPad : public CDialog
{
	DECLARE_DYNAMIC(DlgDialPad)

public:
	DlgDialPad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgDialPad();
public:
	CString		m_strCallID;

// 对话框数据
	enum { IDD = IDD_DIALOG_DIALPAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButtonStar();
	afx_msg void OnBnClickedButton0();
	afx_msg void OnBnClickedButton12();
};
