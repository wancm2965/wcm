#pragma once
#include "WBDesignFillPad.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TransparentButtonEx.h"
#include "WBColorPaletteDlg.h"

// CWBBackgroundDlg �Ի���
class CWBFunctionDlg;
class CWBBackgroundDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBBackgroundDlg)

public:
	CWBBackgroundDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBBackgroundDlg();
	virtual BOOL OnInitDialog();
	// �Ի�������
	enum { IDD = IDD_SINWB_BACKGROUNDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOK();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedWbBgdlgRadioforclrpad();
	afx_msg void OnBnClickedWbBgdlgRadiofodesignpad();
	afx_msg void OnBnClickedWbBgdlgRadioforpicpad();
private:
  	CWBDesignFillPad	*m_pWBDesignFillPad;//ͼ��������
	//CButton m_Radio;//��ѡ��ť
	CTransparentButtonEx *m_Radio;//��ѡ��ť
	CWBColorPaletteDlg *m_pColorPallete;//��ɫ��
	//CListCtrl *m_pClrList;//��ɫ�б��
	
	//COLORREF m_color;
public:
	afx_msg void OnLvnItemchangedSinwbBgdlgClrlist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	//void SetColor(COLORREF *color);
	//COLORREF *GetColor();
	CWBDesignFillPad	*GetDesignFillPad();
	void CloseColorPaletteDlg();         //�رյ�ɫ��
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CWBColorPaletteDlg * GetColorPaletteDlg() { return m_pColorPallete; }
};
