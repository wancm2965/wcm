#pragma once
#include "afxwin.h"
#include"Resource.h"
#include "TransparentButtonEx.h"
#include <GdiPlus.h>
#define WIDTH_BTN 54
#define HEIGHT_BTN 54

// CDlgFile �Ի���

class CDlgFile : public CDialog
{
	DECLARE_DYNAMIC(CDlgFile)

public:
	CDlgFile(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgFile();
	virtual void OnOK();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_DLG_FILE };

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


	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CTransparentButtonEx m_btnOpen;
	CTransparentButtonEx m_btnSave;
	CTransparentButtonEx m_btnResave;
	CTransparentButtonEx m_btnPrint;
	CTransparentButtonEx m_btnCloseFile;


	afx_msg void OnBnClickedBtnCloseflie();
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnPrint();
	afx_msg void OnBnClickedBtnResave();
	afx_msg void OnBnClickedBtnSave();
protected:
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
