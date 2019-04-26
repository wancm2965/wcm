#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54


// CDlgRubber �Ի���

class CDlgRubber : public CDialog
{
	DECLARE_DYNAMIC(CDlgRubber)

public:
	CDlgRubber(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgRubber();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DLG_RUBBER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//������ʾ�ؼ�
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
