#pragma once
#include "afxwin.h"


// CWBRollCallDialog �Ի���

class CWBRollCallDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRollCallDialog)

public:
	CWBRollCallDialog(int left ,int top,CWnd* pParent = NULL );   // ��׼���캯��
	virtual ~CWBRollCallDialog();

// �Ի�������
	enum { IDD = IDD_WB_ROLLCALLDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	CStatic m_nStringStatic;
	CButton m_nOKBtn;
	CButton m_nCancleBtn;
	afx_msg void OnBnClickedRollcallcanclebtn();
	afx_msg void OnBnClickedRollcallokbtn();
	virtual void OnClose();
	void KillRollCallTimer();
	void SetRollCallShow(int left,int top);
private:
	int  m_nTimer;
	int m_left;
	int m_top;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
