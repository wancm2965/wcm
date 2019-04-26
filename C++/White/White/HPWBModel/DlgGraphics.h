#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDlgGraphics �Ի���

class CDlgGraphics : public CDialog
{
	DECLARE_DYNAMIC(CDlgGraphics)

public:
	CDlgGraphics(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgGraphics();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
// �Ի�������
	enum { IDD = IDD_DLG_GRAPHICS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//������ʾ�ؼ�		
public:	

	void SetParentWnd(CWnd* pWnd);
	void SetSkinImage(CString strPath);
	void FlashSkin();
	void Relayout();

	CTransparentButtonEx m_btmRect;
	CTransparentButtonEx m_btmRhombus;
	CTransparentButtonEx m_btmEllipse;
	CTransparentButtonEx m_btmTriangle;
	CTransparentButtonEx m_btmPolygon;
	afx_msg void OnBnClickedBtnEllipse();
	afx_msg void OnBnClickedBtnPolygon();
	afx_msg void OnBnClickedBtnRect();
	afx_msg void OnBnClickedBtnRhombus();
	afx_msg void OnBnClickedBtnTriangle();
protected:
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
