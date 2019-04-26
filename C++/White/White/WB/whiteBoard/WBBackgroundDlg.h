#pragma once
#include "WBDesignFillPad.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TransparentButtonEx.h"
#include "WBColorPaletteDlg.h"

// CWBBackgroundDlg 对话框
class CWBFunctionDlg;
class CWBBackgroundDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBBackgroundDlg)

public:
	CWBBackgroundDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBBackgroundDlg();
	virtual BOOL OnInitDialog();
	// 对话框数据
	enum { IDD = IDD_SINWB_BACKGROUNDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedWbBgdlgRadioforclrpad();
	afx_msg void OnBnClickedWbBgdlgRadiofodesignpad();
	afx_msg void OnBnClickedWbBgdlgRadioforpicpad();
private:
  	CWBDesignFillPad	*m_pWBDesignFillPad;//图案填充面板
	//CButton m_Radio;//单选按钮
	CTransparentButtonEx *m_Radio;//单选按钮
	CWBColorPaletteDlg *m_pColorPallete;//调色板
	//CListCtrl *m_pClrList;//颜色列表框
	
	//COLORREF m_color;
public:
	afx_msg void OnLvnItemchangedSinwbBgdlgClrlist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	//void SetColor(COLORREF *color);
	//COLORREF *GetColor();
	CWBDesignFillPad	*GetDesignFillPad();
	void CloseColorPaletteDlg();         //关闭调色板
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CWBColorPaletteDlg * GetColorPaletteDlg() { return m_pColorPallete; }
};
