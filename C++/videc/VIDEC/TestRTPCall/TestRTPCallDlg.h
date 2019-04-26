
// TestRTPCallDlg.h : header file
//

#pragma once

#include <VIDEC/VIDEC_RTPCall.h>
#include "DlgVideo.h"


// CTestRTPCallDlg dialog
class CTestRTPCallDlg : public CDialog
{
// Construction
public:
	CTestRTPCallDlg(CWnd* pParent = NULL);	// standard constructor

	VIDEC_RTPCall*	m_pVideoRTPCall;
	DlgVideo*		m_pLocalDlgVideo;
	DlgVideo*		m_pPeerDlgVideo;

	void ResizeControls(void);
	void SetFullScreen(void);

// Dialog Data
	enum { IDD = IDD_TESTRTPCALL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDevModeChange(LPTSTR lpDeviceName);
};
