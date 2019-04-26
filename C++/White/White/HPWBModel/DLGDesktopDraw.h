#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDLGDesktopDraw �Ի���

class CDLGDesktopDraw : public CDialog
{
	DECLARE_DYNAMIC(CDLGDesktopDraw)

public:
	CDLGDesktopDraw(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDLGDesktopDraw();

// �Ի�������
	enum { IDD = IDD_DLG_DESKTOPDRAW };
protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//������ʾ�ؼ�
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:	

	void SetParentWnd(CWnd* pWnd);
	void SetSkinImage(CString strPath);
	void FlashSkin();
	void Relayout();



protected:
	virtual void OnCancel();
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CTransparentButtonEx m_btnDDOpen;
	CTransparentButtonEx m_btnDDFullScreen;
	CTransparentButtonEx m_btnDDEffect;
	CTransparentButtonEx m_btnCtrl;
	CTransparentButtonEx m_btnDDClose;
	afx_msg void OnBnClickedBtnDdopen();
	afx_msg void OnBnClickedBtnDdfullscreen();
	afx_msg void OnBnClickedBtnDdeffect();
	afx_msg void OnBnClickedBtnDdctrl();
	afx_msg void OnBnClickedBtnDdclose();
};
