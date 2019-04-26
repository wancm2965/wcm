#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54


// CDlgRubber 对话框

class CDlgRubber : public CDialog
{
	DECLARE_DYNAMIC(CDlgRubber)

public:
	CDlgRubber(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgRubber();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_DLG_RUBBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//工具提示控件
	CTransparentButtonEx m_btnBigRubber;
	CTransparentButtonEx m_btnLitterRubber;
	CTransparentButtonEx m_btnClearObject;
	CTransparentButtonEx m_btnCurveRubber;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:	
	void SetParentWnd(CWnd* pWnd);
	void SetSkinImage(CString strPath);
	void FlashSkin();
	void Relayout();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedBtnBigrubber();
	afx_msg void OnBnClickedBtnClearobject();
	afx_msg void OnBnClickedBtnCurverubber();
	afx_msg void OnBnClickedBtnLitterrubber();

	
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
