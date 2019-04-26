#pragma once
#include "afxwin.h"
#include "TransparentButtonEx.h"

// CWBDesktopDrawDlg 对话框

class CWBDesktopDrawDlg : public CDialog
{
	DECLARE_DYNAMIC(CWBDesktopDrawDlg)

public:
	CWBDesktopDrawDlg(CTransparentButtonEx *pParentBtn,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBDesktopDrawDlg();

// 对话框数据
	enum { IDD = IDD_WB_SCREENDRAWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParentDlg; //主工具条窗口
	Bitmap* m_pBitmap;
	CTransparentButtonEx *m_pParentBtn;

	CTransparentButtonEx m_DDOpenBtn;
	CTransparentButtonEx m_DDEffectBtn;
//	CTransparentButtonEx m_DDFullScreenBtn;
//	CTransparentButtonEx m_DDCtrlBtn;
	CTransparentButtonEx m_DDCloseBtn;
	virtual void OnOK();
	virtual void OnCancel();
public:
	void Relayout();
	void SetSkinImage(CString strPath);
	CString GetCaptureFromType(eType btnEmumType);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedDdopenbtn();
	afx_msg void OnBnClickedDdeffectbtn();
	afx_msg void OnBnClickedDdfullscreenbtn();
	afx_msg void OnBnClickedDdctrlbtn();
	afx_msg void OnBnClickedDdclosebtn();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
