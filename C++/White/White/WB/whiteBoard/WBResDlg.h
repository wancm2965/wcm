#pragma once


// CWBResDlg �Ի���

class CWBResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBResDlg)

public:
	CWBResDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBResDlg();

	// �Ի�������
	enum { IDD = IDD_WB_RESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
};