#pragma once
#include "afxwin.h"


// CWBRollCallShowDlg 对话框

class CWBRollCallShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRollCallShowDlg)

public:
	CWBRollCallShowDlg(int left,int top,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBRollCallShowDlg();

// 对话框数据
	enum { IDD = IDC_WB_ROLLCALLSHOWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	CStatic m_nListStatic;
	CListBox m_RollCallList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_okBtn;
	CButton m_cancleBtn;
	void ShowagainDlg(int left,int top);
private:
	int m_left;
	int  m_top;
};
