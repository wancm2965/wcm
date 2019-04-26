// VIDECRTPDlg.h : ͷ�ļ�
//

#pragma once
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include "XBitrate.h"


#include "DlgFullScreen.h"
#include "DlgVideo.h"
#include "DlgBar.h"
#include <NETEC/NETEC_RTPSession.h>

// DlgMain �Ի���
class DlgMain 
	: public CDialog
	, public VIDEC_CapChanRTPCallback
	, public NETEC_RTPSessionCallback
{
// ����
public:
	DlgMain(CWnd* pParent = NULL);	// ��׼���캯��


	void ResizeControls(void);
	void SetFullScreen(DlgVideo*pDlgVideo);
	void DoStart(void);
	void DoStop(void);
	void DoSetting(void);

	void OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen);
	void OnNETEC_RTPSessionCallbackRecvdRTPPacket(NETEC_RTPSession*pRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen);

	DlgFullScreen*		m_pDlgFullScreen;
	DlgVideo*			m_pDlgVideoPlayer;
	DlgVideo*			m_pDlgVideoLocal;
	DlgBar*				m_pDlgBar;

	VIDEC_CameraDev*	m_pCamera;

	VIDEC_CapChanRTP*	m_pCapChanRTP;
	VIDEC_PlayChanRTP*	m_pPlayChanRTP;

	NETEC_RTPSession*	m_pRTPSession;

	int					m_nWidth;
	int					m_nHeight;
	int					m_nDevID;
	int					m_nFormat;
	int					m_nFrameRate;
	int					m_nQuality;
	VIDEC_CODEC_TYPE	m_CodecType;


	XBitrate			m_XBitrate;
	UINT				m_nLocalPort;
	UINT				m_nRemotePort;
	CString				m_strRemoteIP;

// �Ի�������
	enum { IDD = IDD_DIALOG_MAIN };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
};
