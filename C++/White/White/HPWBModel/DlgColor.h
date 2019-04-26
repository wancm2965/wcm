#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDlgColor 对话框

class CDlgColor : public CDialog
{
	DECLARE_DYNAMIC(CDlgColor)

public:
	CDlgColor(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgColor();

// 对话框数据
	enum { IDD = IDD_DLG_COLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();


protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//工具提示控件
public:	

	void SetParentWnd(CWnd* pWnd);
	void SetSkinImage(CString strPath);
	void FlashSkin();
	void Relayout();
	CTransparentButtonEx m_btnRed;
	CTransparentButtonEx m_btnBlack;
	CTransparentButtonEx m_btnYellow;
	CTransparentButtonEx m_btnOrange;
	CTransparentButtonEx m_btnblue;
	CTransparentButtonEx m_btnPurple;
	afx_msg void OnBnClickedBtnPurple();
	afx_msg void OnBnClickedBtnBlue();
	afx_msg void OnBnClickedBtnOrange();
	afx_msg void OnBnClickedBtnYellow();
	afx_msg void OnBnClickedBtnBlack();
	afx_msg void OnBnClickedBtnRed();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
