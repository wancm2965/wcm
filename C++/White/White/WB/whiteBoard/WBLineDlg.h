#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "resource.h"


// CWBLineDlg 对话框

class CWBLineDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBLineDlg)

public:
	CWBLineDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBLineDlg();

// 对话框数据
	enum { IDD = IDD_WB_LINEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLinebtn();
	afx_msg void OnBnClickedCurvvebtn();
	afx_msg void OnBnClickedIcoCurvebtn();
private:
	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_lineBtn;//直线按钮
	CTransparentButtonEx m_curveBtn;//曲线按钮
	CTransparentButtonEx m_icocurveBtn;//图标曲线按钮
	CTransparentButtonEx m_magcurveBtn;//魔术线按钮
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedMagcurvebtn();
};
