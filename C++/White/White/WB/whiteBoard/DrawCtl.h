#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDrawCtl 对话框

class CDrawCtl : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawCtl)

public:
	CDrawCtl(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDrawCtl();

	// 对话框数据
	enum { IDD = IDD_DRAWCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_transCtl;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedFull();
	afx_msg void OnBnClickedEffective();
	afx_msg void OnBnClickedDestory();
	afx_msg void OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
};
