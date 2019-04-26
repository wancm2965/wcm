#pragma once

#include <string>
#include <IMXEC/IMXEC_Call.h>

// DlgSecondVideo 对话框

class DlgSecondVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgSecondVideo)

public:
	DlgSecondVideo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgSecondVideo();
public:
	std::string		m_strCallID;

// 对话框数据
	enum { IDD = IDD_DIALOG_SECOND_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
