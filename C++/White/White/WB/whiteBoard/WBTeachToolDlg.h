#pragma once

#include "WBFillPadBtn.h"
#include "whiteBoard.h"
#include "WBAttributeStatic.h"

// CWBTeachToolDlg �Ի���

class CWBTeachToolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBTeachToolDlg)

public:
	CWBTeachToolDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBTeachToolDlg();
	virtual BOOL OnInitDialog();

// �Ի�������
	enum { IDD = IDD_WB_TEACHTOOLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
private:
	CWBAttributeStatic m_cTTStatic;
	CWBFillPadBtn m_cTeachToolBtnOne;
	CWBFillPadBtn m_cTeachToolBtnTwo;
	CWBFillPadBtn m_cTeachToolBtnThree;
	CWBFillPadBtn m_cTeachToolBtnFour;
 	CWBFillPadBtn m_cTeachToolBtnFive;
// 	CWBFillPadBtn m_cTeachToolBtnSix;
// 	CWBFillPadBtn m_cTeachToolBtnSeven;
// 	CWBFillPadBtn m_cTeachToolBtnEight;
public:
	afx_msg void OnBnDoubleclickedWbTtbtn1();
	afx_msg void OnBnDoubleclickedWbTtbtn2();
	afx_msg void OnBnDoubleclickedWbTtbtn3();
	afx_msg void OnBnDoubleclickedWbTtbtn4();
	afx_msg void OnBnDoubleclickedWbTtbtn5();

	void SetTTBtnState(int nBtnID);
	void ShowFiveBtnVisable(bool bshow);
};
