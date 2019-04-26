#pragma once
#include "afxwin.h"
#include "WBFunctionBtn.h"

// CWBOptionDlg 对话框

class CWBOptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBOptionDlg)

public:
	CWBOptionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBOptionDlg();

// 对话框数据
	enum { IDD = IDD_WB_OPTIONDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	CWBFunctionBtn m_cPreviewButton;
	CWBFunctionBtn m_cAttributeButton;
	CWBFunctionBtn m_cResButton;
	CWBFunctionBtn m_cBGButton;
	CWBFunctionBtn m_cTTButton;
	CWnd* m_pParent;
	CRect m_rect;
	CWBOptionDlg * m_WBOptionDlg;

	bool m_IsSet;  //用于Mousemove有限次数释放 mouseleave和mousehover操作
	bool  m_bIsSetTimer;  //是否可以设置定时器
public:
	//设置或获得是否可以使用定时器
	void SetTime(bool bl) { m_bIsSetTimer= bl;}

	afx_msg void OnBnClickedWbPreviewbtn();
	afx_msg void OnBnClickedWbResbtn();
	afx_msg void OnBnClickedWbAttributebtn();
	afx_msg void OnBnClickedWbBackgroundbtn();
	afx_msg void OnBnClickedWbTeachtool();

	void OnOptionBtnClicked(int BtnId);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnWM_SHOWOPTIONBTN(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
