#pragma once


// DlgDialPad �Ի���

class DlgDialPad : public CDialog
{
	DECLARE_DYNAMIC(DlgDialPad)

public:
	DlgDialPad(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgDialPad();
public:
	CString		m_strCallID;

// �Ի�������
	enum { IDD = IDD_DIALOG_DIALPAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
