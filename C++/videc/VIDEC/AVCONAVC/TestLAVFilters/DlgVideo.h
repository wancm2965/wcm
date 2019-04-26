#pragma once

#include <AVCONAVC/AVCONAVC_VideoDraw.h>

// DlgVideo �Ի���

class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgVideo();
public:
	AVCONAVC_VideoDraw*	m_pVideoDraw;
public:
	void ResizeControls(void);

// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
