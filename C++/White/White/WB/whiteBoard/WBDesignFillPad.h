#pragma once
#include "afxwin.h"
#include "WBFillPadBtn.h"
#include "whiteBoard.h"


// CWBDesignFillPad 对话框

class CWBDesignFillPad : public CDialogEx
{
	DECLARE_DYNAMIC(CWBDesignFillPad)

public:
	CWBDesignFillPad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CWBDesignFillPad();
	virtual BOOL OnInitDialog();

// 对话框数据
	enum { IDD = IDD_SINWB_BGDLG_DESIGNPAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
private:
	CWBFillPadBtn m_cClrFillBtnOne;
	CWBFillPadBtn m_cClrFillBtnTwo;
	CWBFillPadBtn m_cClrFillBtnThree;
	CWBFillPadBtn m_cClrFillBtnFour;
	CWBFillPadBtn m_cClrFillBtnFive;
	CWBFillPadBtn m_cClrFillBtnSix;
	CWBFillPadBtn m_cClrFillBtnSeven;
	CWBFillPadBtn m_cClrFillBtnEight;
	CWBFillPadBtn m_cClrFillBtnNine;
	CWBFillPadBtn m_cClrFillBtnTen;
	CWBFillPadBtn m_cClrFillBtnEleven;
	CWBFillPadBtn m_cClrFillBtnTwelve;
	CWBFillPadBtn m_cClrFillBtnThirteen;
	CWBFillPadBtn m_cClrFillBtnFourteen;
	CWBFillPadBtn m_cClrFillBtnFifteen;
public:
	afx_msg void OnBnClickedClrfillbtn1();
	afx_msg void OnBnClickedClrfillbtn2();
	afx_msg void OnBnClickedClrfillbtn3();
	afx_msg void OnBnClickedClrfillbtn4();
	afx_msg void OnBnClickedClrfillbtn5();
	afx_msg void OnBnClickedClrfillbtn6();
	afx_msg void OnBnClickedClrfillbtn7();
	afx_msg void OnBnClickedClrfillbtn8();
	afx_msg void OnBnClickedClrfillbtn9();
	afx_msg void OnBnClickedClrfillbtn10();
	afx_msg void OnBnClickedClrfillbtn11();
	afx_msg void OnBnClickedClrfillbtn12();
	afx_msg void OnBnClickedClrfillbtn13();
	afx_msg void OnBnClickedClrfillbtn14();
	afx_msg void OnBnClickedClrfillbtn15();
	void SetFillBtnState(int nBtnID);//设置面板中15个按钮的状态
	void SetBKGfalse();//删除背景
};
