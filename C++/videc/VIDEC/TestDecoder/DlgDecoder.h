#pragma once

#include "DlgPlayVideo.h"
#include "DlgFullScreen.h"

#define MAX_PLAYER	64

// DlgDecoder 对话框

class DlgDecoder : public CDialog
{
	DECLARE_DYNAMIC(DlgDecoder)

public:
	DlgDecoder(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgDecoder();
public:

	void SetFullScreen(CWnd*pWndVideo);
	void PlayFramePacket(const void*pPacketData,int nPacketLen);
	void PlayRTPPacket(const void*pPacketData,int nPacketLen);

	DWORD			m_nDecoderCount;
	int				m_nRows;

	DlgPlayVideo*	m_apDlgPlayVideo[MAX_PLAYER];

	CWnd*		m_pWndVideo;
	DlgFullScreen*	m_pDlgFullScreen;
public:
// 对话框数据
	enum { IDD = IDD_DIALOG_DECODER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	virtual BOOL DestroyWindow();
	virtual BOOL OnInitDialog();
};
