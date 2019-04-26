#pragma once
#include <IMXEC/IMXEC_MediaChannel.h>


// CPlayer �Ի���

class CPlayer : public CDialog
{
	DECLARE_DYNAMIC(CPlayer)

public:
	CPlayer(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlayer();
	void SetHwnd(IMXEC_PlayChan* play, IMXEC_CapChanDev* cap);

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	IMXEC_PlayChan* m_pPlay;
	IMXEC_CapChanDev* m_pCap;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
