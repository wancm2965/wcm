#pragma once
#include "afxwin.h"


// DlgBar 对话框

class DlgMain;
class DlgBar : public CDialog
{
	DECLARE_DYNAMIC(DlgBar)

public:
	DlgBar(DlgMain&rDlgMain);   // 标准构造函数
	virtual ~DlgBar();

	void ResizeControls(void);

	DlgMain&		m_rDlgMain;
// 对话框数据
	enum { IDD = IDD_DIALOG_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonSetting();
	afx_msg void OnBnClickedButtonClose();
	CButton m_btnStart;
	CButton m_btnStop;
	CButton m_btnSetting;
	CButton m_btnClose;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CButton m_btnKeyFrame;
	afx_msg void OnBnClickedButtonKeyFrame();
};
