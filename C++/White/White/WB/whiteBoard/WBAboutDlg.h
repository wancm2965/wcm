#pragma once
#include "WBAttributeStatic.h"
#include "afxwin.h"
#include "common\WBData.h"

// CWBAboutDlg �Ի���

class CWBAboutDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBAboutDlg)

public:
	CWBAboutDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBAboutDlg();

// �Ի�������
	enum { IDD = IDD_WB_ABOUTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	Bitmap* m_pBitmap;
public:
	void SetSkinImage(CString strPath);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
