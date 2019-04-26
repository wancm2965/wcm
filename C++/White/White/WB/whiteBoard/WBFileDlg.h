#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBFileDlg 对话框

class CWBFileDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBFileDlg)

public:
	CWBFileDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBFileDlg();

// 对话框数据
	enum { IDD = IDD_WB_FILEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

private:

	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	//对话框中按钮
	CTransparentButtonEx m_openBtn;
	CTransparentButtonEx m_combineBtn;
	CTransparentButtonEx m_saveBtn;
	CTransparentButtonEx m_resaveBtn;
	CTransparentButtonEx m_printBtn;
	CTransparentButtonEx m_closeBtn;
	CTransparentButtonEx m_BtnSavePic;
public:

	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOpenFilebtn();
	afx_msg void OnBnClickedCombineFilebtn();
	afx_msg void OnBnClickedSaveFilebtn();
	afx_msg void OnBnClickedResaveFilebtn();
	afx_msg void OnBnClickedPrintFilebtn();
	afx_msg void OnBnClickedCloseFilebtn();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClickedSavepicFilebtn();
};
