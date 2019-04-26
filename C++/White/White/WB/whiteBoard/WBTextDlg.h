#pragma once
#include "TransparentButtonEx.h"
#include "afxwin.h"

// CWBTextDlg 对话框

class CWBTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBTextDlg)

public:
	CWBTextDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBTextDlg();

// 对话框数据
	enum { IDD = IDD_WB_TEXTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	void Relayout();
	void SetSkinImage(CString strPath);
private:
	CWnd* m_pParentDlg;
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;
	CTransparentButtonEx m_HTextBtn;
	CTransparentButtonEx m_VTextBtn;
public:
	afx_msg void OnBnClickedHTextbtn();
	afx_msg void OnBnClickedVTextbtn();
	afx_msg void OnDestroy();
};
