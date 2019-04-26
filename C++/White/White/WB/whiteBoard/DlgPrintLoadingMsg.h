#pragma once
#include "Resource.h"
#include "MyBitMap.h"
#include "WBAttributeStatic.h"
#include <afxwin.h>

// CDlgPrintLoadingMsg 对话框

class CDlgPrintLoadingMsg : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrintLoadingMsg)

public:
	CDlgPrintLoadingMsg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgPrintLoadingMsg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PRINTDOC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CMyBitMap					m_bitmapLoadingBK;
	CWnd*							m_pParentWnd;
	float								m_fPercent;
	CProgressCtrl				m_ProgLoading;		
public:
	virtual BOOL OnInitDialog();
	HRGN BitmapToRegion(HBITMAP hBmp, COLORREF cTransparentColor);
	void SetMsg(CString strMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
protected:
	afx_msg LRESULT OnRedrowPrintLoadingMsg(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
};
