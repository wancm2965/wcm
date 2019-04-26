#pragma once
#include "afxwin.h"


// CWBRollCallShowDlg �Ի���

class CWBRollCallShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBRollCallShowDlg)

public:
	CWBRollCallShowDlg(int left,int top,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CWBRollCallShowDlg();

// �Ի�������
	enum { IDD = IDC_WB_ROLLCALLSHOWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	CStatic m_nListStatic;
	CListBox m_RollCallList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_okBtn;
	CButton m_cancleBtn;
	void ShowagainDlg(int left,int top);
private:
	int m_left;
	int  m_top;
};
