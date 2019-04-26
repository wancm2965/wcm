#pragma once
#include "WBPreviewSingleView.h"

// CWBEagelEye �Ի���

class CWBEagelEye : public CDialog
{
	DECLARE_DYNAMIC(CWBEagelEye)

public:
	CWBEagelEye(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBEagelEye();

// �Ի�������
	enum { IDD = IDD_EAGLEEYE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
/*	afx_msg BOOL OnEraseBkgnd(CDC* pDC);*/
	CWBPreviewSingleView* GetPreviewSingleView();
private:
	CWBPreviewSingleView* m_pWBPreviewSingleView;
	CRect m_rect;
public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
