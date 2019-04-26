#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBAboutDlg.h"
// CWBToolbarCtrlDlg 对话框

class CWBToolbarCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBToolbarCtrlDlg)

public:
	CWBToolbarCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBToolbarCtrlDlg();

// 对话框数据
	enum { IDD = IDD_WB_CTRLBTNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
private:
	CTransparentButtonEx m_toolbarBtn;
	int m_isShowToolbar;
	
public:
	afx_msg void OnClickedToolbarbtn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
