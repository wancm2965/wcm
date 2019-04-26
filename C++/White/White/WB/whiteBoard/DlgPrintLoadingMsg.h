#pragma once
#include "Resource.h"
#include "MyBitMap.h"
#include "WBAttributeStatic.h"
#include <afxwin.h>

// CDlgPrintLoadingMsg �Ի���

class CDlgPrintLoadingMsg : public CDialog
{
	DECLARE_DYNAMIC(CDlgPrintLoadingMsg)

public:
	CDlgPrintLoadingMsg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPrintLoadingMsg();

// �Ի�������
	enum { IDD = IDD_DIALOG_PRINTDOC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
