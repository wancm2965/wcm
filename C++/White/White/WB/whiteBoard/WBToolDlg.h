#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"

// CWBToolDlg 对话框

class CWBToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBToolDlg)

public:
	CWBToolDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBToolDlg();

// 对话框数据
	enum { IDD = IDD_WB_TOOLDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CWnd * m_pParentDlg;
	Bitmap* m_pBitmap;

	CTransparentButtonEx *m_pParentBtn;

	CTransparentButtonEx m_MAQNBtn;
	CTransparentButtonEx m_SpotLightBtn;
	CTransparentButtonEx m_BafflePlateBtn;
	CTransparentButtonEx m_ProtractorBtn;
	CTransparentButtonEx m_CompassesBtn;
	CTransparentButtonEx m_RulerBtn;
	virtual void OnCancel();
	virtual void OnOK();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedMaqnbtn();
	afx_msg void OnBnClickedSpotlightbtn();
	afx_msg void OnBnClickedBaffleplatebtn();
	afx_msg void OnBnClickedScreenrecbtn();
	afx_msg void OnBnClickedProtractorbtn();
	afx_msg void OnBnClickedCompassesbtn();
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedRuler();
};
