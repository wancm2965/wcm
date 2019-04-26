#pragma once
#include "afxwin.h"


// DlgCallInfo 对话框

class DlgCallInfo : public CDialog
{
	DECLARE_DYNAMIC(DlgCallInfo)

public:
	DlgCallInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgCallInfo();
public:
	CString		m_strCallID;
	UINT32		m_nRefreshTimerID;

// 对话框数据
	enum { IDD = IDD_DIALOG_CALL_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual void ShowInfo(void);
	CStatic m_btnLocalVideoInfo;
	CStatic m_btnRemoteVideoInfo;
	CStatic m_btnSecondVideoInfo;
};
