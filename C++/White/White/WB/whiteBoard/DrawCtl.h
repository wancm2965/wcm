#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CDrawCtl �Ի���

class CDrawCtl : public CDialogEx
{
	DECLARE_DYNAMIC(CDrawCtl)

public:
	CDrawCtl(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDrawCtl();

	// �Ի�������
	enum { IDD = IDD_DRAWCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
