#pragma once

#include <string>
#include <IMXEC/IMXEC_Call.h>

// DlgSecondVideo �Ի���

class DlgSecondVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgSecondVideo)

public:
	DlgSecondVideo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgSecondVideo();
public:
	std::string		m_strCallID;

// �Ի�������
	enum { IDD = IDD_DIALOG_SECOND_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
