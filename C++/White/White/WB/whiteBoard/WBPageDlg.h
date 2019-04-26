#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"
#include "WBToolbarDlg.h"
#include "WBAttributeStatic.h"
// CWBPageDlg �Ի���

class CWBPageDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBPageDlg)

public:
	CWBPageDlg(CWBToolbarDlg *pToolbar,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBPageDlg();

// �Ի�������
	enum { IDD = IDD_WB_PAGEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	//CTransparentButtonEx m_previewPageBtn;
	CTransparentButtonEx m_prePageBtn;
	CTransparentButtonEx m_nextPageBtn;
	CWBAttributeStatic m_pageNumberStatic; //��ʾ�װ�����ҳ���͵�ǰҳ
	CWBToolbarDlg * m_pToolbar;
public:
	static HWND g_WBPageDlgHWND;
	void ShowPageNumber();
	void SetPreAndNextBtnStatic();
	CString GetCaptureFromType(eType btnEmumType);
	afx_msg void OnBnClickedPreviewpagebtn();
	afx_msg void OnBnClickedPrepagebtn();
	afx_msg void OnBnClickedNextpagebtn();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnShowPageNumber(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
