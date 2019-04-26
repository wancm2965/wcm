#pragma once
#include <IMXEC/IMXEC_MediaChannel.h>


// CPlayer 对话框

class CPlayer : public CDialog
{
	DECLARE_DYNAMIC(CPlayer)

public:
	CPlayer(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayer();
	void SetHwnd(IMXEC_PlayChan* play, IMXEC_CapChanDev* cap);

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	IMXEC_PlayChan* m_pPlay;
	IMXEC_CapChanDev* m_pCap;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
