#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBToolbarDlg.h"


// CWBRedoAndUndoDlg �Ի���

class CWBRedoAndUndoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRedoAndUndoDlg)

public:
	CWBRedoAndUndoDlg(CWBToolbarDlg *pToolbar,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBRedoAndUndoDlg();

// �Ի�������
	enum { IDD = IDD_WB_REDODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CTransparentButtonEx m_imagectrl;
	CTransparentButtonEx m_undoBtn;
	CTransparentButtonEx m_redoBtn;
	CWBToolbarDlg * m_pToolbar;
public:
	CString GetCaptureFromType(eType btnEmumType);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedUndobtn();
	afx_msg void OnBnClickedRedobtn();
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
