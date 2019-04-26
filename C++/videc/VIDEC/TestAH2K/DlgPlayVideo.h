#pragma once


#include "../../include/VIDEC/VIDEC.h"
#include "../../include/VIDEC/VIDEC_Player.h"

// DlgPlayVideo �Ի���
class DlgMain;
class DlgPlayVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgPlayVideo)

public:
	DlgPlayVideo(DlgMain&rDlgMain);   // ��׼���캯��
	virtual ~DlgPlayVideo();

#ifdef TEST_PLAYER
#ifdef TEST_DXVA
	AVCONAVC_Player*	m_pPlayer;
#else
	VIDEC_Player*	m_pPlayer;
#endif
#else
	VIDEC_Wnd*		m_pWnd;
#endif

	DlgMain&		m_rDlgMain;

	void ResizeControls(void);

// �Ի�������
	enum { IDD = IDD_DIALOG_PLAY_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
