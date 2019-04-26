#pragma once


// CWBResDlg 对话框

class CWBResDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBResDlg)

public:
	CWBResDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBResDlg();

	// 对话框数据
	enum { IDD = IDD_WB_RESDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
};