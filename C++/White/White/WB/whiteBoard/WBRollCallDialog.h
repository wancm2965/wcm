#pragma once
#include "afxwin.h"


// CWBRollCallDialog 对话框

class CWBRollCallDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRollCallDialog)

public:
	CWBRollCallDialog(int left ,int top,CWnd* pParent = NULL );   // 标准构造函数
	virtual ~CWBRollCallDialog();

// 对话框数据
	enum { IDD = IDD_WB_ROLLCALLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	CStatic m_nStringStatic;
	CButton m_nOKBtn;
	CButton m_nCancleBtn;
	afx_msg void OnBnClickedRollcallcanclebtn();
	afx_msg void OnBnClickedRollcallokbtn();
	virtual void OnClose();
	void KillRollCallTimer();
	void SetRollCallShow(int left,int top);
private:
	int  m_nTimer;
	int m_left;
	int m_top;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
