#pragma once

// DlgVideo 对话框
class CTestRTPCallDlg;
class DlgVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgVideo)

public:
	DlgVideo(CTestRTPCallDlg&rTestRTPCallDlg,bool bLocal);   // 标准构造函数
	virtual ~DlgVideo();

	bool					m_bLocal;
	CTestRTPCallDlg&		m_rTestRTPCallDlg;

	void ResizeControls(void);

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
