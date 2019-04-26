#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDlgResource �Ի���

class CDlgResource : public CDialog
{
	DECLARE_DYNAMIC(CDlgResource)

public:
	CDlgResource(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgResource();
    virtual BOOL OnInitDialog();
	virtual void OnOK();
// �Ի�������
	enum { IDD = IDD_DLG_RESOURSE };

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
	
	CTransparentButtonEx m_btnPicture;
	CTransparentButtonEx m_btnViedo;
	CTransparentButtonEx m_btnFlash;
	CTransparentButtonEx m_btnDocument;

	afx_msg void OnBnClickedBtnPicture();
	afx_msg void OnBnClickedBtnViedo();
	afx_msg void OnBnClickedBtnFlash();
	afx_msg void OnBnClickedBtnDocument();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
