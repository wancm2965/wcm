#pragma once

#include "Resource.h"

// DlgVideo �Ի���

class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(int nLocal,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgVideo();

// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	bool	m_bShowMax;
	int		m_nLocal;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
};
