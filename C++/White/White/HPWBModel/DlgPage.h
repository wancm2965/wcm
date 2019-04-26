#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDlgPage �Ի���

class CDlgPage : public CDialog
{
	DECLARE_DYNAMIC(CDlgPage)

public:
	CDlgPage(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgPage();

// �Ի�������
	enum { IDD = IDD_DLG_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	CWnd* m_pWndParent;
	Bitmap* m_pBitmap;
	CToolTipCtrl m_ToolTip;//������ʾ�ؼ�

public:	
	void SetParentWnd(CWnd* pWnd);
	void SetSkinImage(CString strPath);
	void FlashSkin();
	void Relayout();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:
	CTransparentButtonEx m_btnNewPage;
	CTransparentButtonEx m_btnPrePage;
	CTransparentButtonEx m_btnNextPage;
public:
	afx_msg void OnBnClickedBtnNewpage();
	afx_msg void OnBnClickedBtnPrepage();
	afx_msg void OnBnClickedBtnNextpage();
};
