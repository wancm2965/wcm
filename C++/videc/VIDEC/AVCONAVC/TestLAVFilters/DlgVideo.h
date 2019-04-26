#pragma once

#include <AVCONAVC/AVCONAVC_VideoDraw.h>

// DlgVideo 对话框

class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgVideo();
public:
	AVCONAVC_VideoDraw*	m_pVideoDraw;
public:
	void ResizeControls(void);

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
