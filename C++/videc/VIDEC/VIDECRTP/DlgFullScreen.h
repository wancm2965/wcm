#pragma once

// DlgFullScreen �Ի���
class DlgMain;
class DlgFullScreen : public CDialog
{
	DECLARE_DYNAMIC(DlgFullScreen)

public:
	DlgFullScreen(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgFullScreen();
	
	CWnd*	m_pWndChild;

	void ResizeControls(void);
	void SetWndChild(CWnd*pWndChild);
// �Ի�������
	enum { IDD = IDD_DIALOG_FULL_SCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
