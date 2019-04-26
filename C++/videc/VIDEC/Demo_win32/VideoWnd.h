#pragma once
#include "stdafx.h"

#include <NETEC_Node.h>
#include <NETEC_Core.h>
#include <NETEC_MediaReceiver.h>

#include <VIDEC.h>

#include <HPAVC_Player.h>
#include <VIDEC_Player.h>

#include "DlgVideoParam.h"

// CVideoWnd 对话框


class CVideoWnd
	: public CDialogEx
	, public NETEC_MediaReceiverCallback
	, public NETEC_Node
#if USE_HPAVC
	, public HPAVC_PlayerCallback
#else
	, public VIDEC_PlayerCallback
#endif
	, public VideoParamNotify
{
	DECLARE_DYNAMIC(CVideoWnd)

public:
	CVideoWnd(CWnd* pParent = NULL,bool bUserHPAVC = false);   // 标准构造函数
	virtual ~CVideoWnd();

// 对话框数据
	enum { IDD = IDD_DIALOG_VIDEOWND };

private:
	int						m_nIndex;
	bool					m_bUserHPAVC;
#if USE_HPAVC
	HPAVC_Player*			m_pHPAVC_Player;
#else
	VIDEC_Player*			m_pVIDEC_Player;
#endif

	NETEC_MediaReceiver*	m_pIAVReceiver;
	unsigned long			m_nAudChanID;
	unsigned long			m_nVidChanID;
	std::string				m_strNoidID;
	std::string				m_strMCUID;
	std::string				m_strMCUIP;

	FILE*					m_pFile;

	Video_Param				m_param;	
	int						m_nLBDbClickTimes;
	CWnd*					m_pParentWnd;

public:

	bool Start(std::string strAV,int nIndex);
	void Stop(void);
	void SetVideoShow(bool bShowVideo);

	int PlayData(void*pPacketData,int nPacketSize);
public:
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);
	virtual void OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen);
	virtual void OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen);

	virtual void OnHPAVC_PlayerCallbackDrawDC(void* hDC/*HDC*/);

	virtual void VideoParamNotifyNewParam(Video_Param param);

	virtual void OnVIDEC_PlayerCallbackDrawD3D9DC(void* hDC/*HDC*/);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL DestroyWindow();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
};
