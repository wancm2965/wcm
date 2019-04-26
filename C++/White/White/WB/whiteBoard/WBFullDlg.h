#pragma once

#include "WhiteBoardView.h"

// CWBFullDlg �Ի���

class CWBFullDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWBFullDlg)

public:
	CWBFullDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual BOOL OnInitDialog();
	virtual ~CWBFullDlg();
	void showWin(BOOL sh);

// �Ի�������
	enum { IDD = IDD_WB_FULLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	CRect m_fullrect;
public:
	CWhiteBoardView* m_whiteboard_fullview;
};
