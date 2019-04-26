#pragma once


#include <VIDEC/VIDEC.h>

// DlgVideo �Ի���
class DlgMain;
class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(DlgMain&rDlgMain);   // ��׼���캯��
	virtual ~DlgVideo();

	VIDEC_Wnd*		m_pWnd;
	DlgMain&		m_rDlgMain;

	void ResizeControls(void);

// �Ի�������
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
