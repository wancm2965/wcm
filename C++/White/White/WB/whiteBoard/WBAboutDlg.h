#pragma once
#include "WBAttributeStatic.h"
#include "afxwin.h"
#include "common\WBData.h"

// CWBAboutDlg 对话框

class CWBAboutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBAboutDlg)

public:
	CWBAboutDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBAboutDlg();

// 对话框数据
	enum { IDD = IDD_WB_ABOUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	Bitmap* m_pBitmap;
public:
	void SetSkinImage(CString strPath);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
