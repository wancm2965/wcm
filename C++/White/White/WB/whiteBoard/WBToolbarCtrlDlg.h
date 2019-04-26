#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBAboutDlg.h"
// CWBToolbarCtrlDlg �Ի���

class CWBToolbarCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBToolbarCtrlDlg)

public:
	CWBToolbarCtrlDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBToolbarCtrlDlg();

// �Ի�������
	enum { IDD = IDD_WB_CTRLBTNDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
private:
	CTransparentButtonEx m_toolbarBtn;
	int m_isShowToolbar;
	
public:
	afx_msg void OnClickedToolbarbtn();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};
