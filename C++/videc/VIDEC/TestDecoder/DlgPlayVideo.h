#pragma once


#include "../../include/VIDEC/VIDEC.h"
#include "../../include/VIDEC/VIDEC_Player.h"

// DlgPlayVideo �Ի���
class DlgDecoder;
class DlgPlayVideo : public CDialog
{
	DECLARE_DYNAMIC(DlgPlayVideo)

public:
	DlgPlayVideo(DlgDecoder&rDlgDecoder);   // ��׼���캯��
	virtual ~DlgPlayVideo();

	VIDEC_Player*	m_pPlayer;

	DlgDecoder&		m_rDlgDecoder;

	void ResizeControls(void);
	void PlayFramePacket(const void*pPacketData,int nPacketLen);
	void PlayRTPPacket(const void*pPacketData,int nPacketLen);

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
