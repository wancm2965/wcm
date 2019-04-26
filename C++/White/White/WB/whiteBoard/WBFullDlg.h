#pragma once

#include "WhiteBoardView.h"

// CWBFullDlg 对话框

class CWBFullDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFullDlg)

public:
	CWBFullDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual BOOL OnInitDialog();
	virtual ~CWBFullDlg();
	void showWin(BOOL sh);

// 对话框数据
	enum { IDD = IDD_WB_FULLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	CRect m_fullrect;
public:
	CWhiteBoardView* m_whiteboard_fullview;
};
