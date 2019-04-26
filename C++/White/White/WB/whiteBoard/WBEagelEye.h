#pragma once
#include "WBPreviewSingleView.h"

// CWBEagelEye 对话框

class CWBEagelEye : public CDialog
{
	DECLARE_DYNAMIC(CWBEagelEye)

public:
	CWBEagelEye(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBEagelEye();

// 对话框数据
	enum { IDD = IDD_EAGLEEYE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
/*	afx_msg BOOL OnEraseBkgnd(CDC* pDC);*/
	CWBPreviewSingleView* GetPreviewSingleView();
private:
	CWBPreviewSingleView* m_pWBPreviewSingleView;
	CRect m_rect;
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
