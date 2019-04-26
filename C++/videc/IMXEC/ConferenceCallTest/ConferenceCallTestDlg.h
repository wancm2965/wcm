// ConferenceCallTestDlg.h : ͷ�ļ�
//

#pragma once

#include <AUDEC/AUDEC_Engine.h>
#include <VIDEC/VIDEC.h>
#include <IMXEC/IMXEC_MediaChannel.h>
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Node.h>
#include <NETEC/NETEC_Setting.h>
#include <IMXEC/IMXEC_H323.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <IMXEC/IMXEC_AudioSetting.h>
#include <IMXEC/IMXEC_SIP.h>
#include <IMXEC/IMXEC_IMX.h>
#include <IMXEC/IMXEC_Call.h>
#include "afxwin.h"
#include "DlgVideo.h"
#include "DlgSecondVideo.h"
#include <string>

#define MAX_CALL_COUNT	2

// CConferenceCallTestDlg �Ի���
class CConferenceCallTestDlg 
	: public CDialog
	, public IMXEC_Call
	, public IMXEC_PlayChanCallback
	, public NETEC_Node
{
// ����
public:
	CConferenceCallTestDlg(CWnd* pParent = NULL);	// ��׼���캯��
public:
	std::string			m_strCurrentCallID;
	DlgVideo*			m_pDlgVideoLocal;
	DlgVideo*			m_pDlgVideoRemote;
	DlgVideo*			m_pDlgVideoRemote2;
	std::string			m_strCallID2;

void SetH235AuthFromINI();
	DlgSecondVideo*		m_pDlgSecondVideo;

	DWORD				m_nH323ListenPort;
	DWORD				m_nRTPBasePort;
	CString				m_strLocalIP;
	CString				m_strLastPeerIP;
	X_AUDIO_CODEC_ID	m_AudioCodecID;

	int					m_nDevID;
	VIDEC_CODEC_TYPE	m_VideoCodecType;
	int					m_nFrameRate;
	int					m_nBitrate;
	VIDEO_FORMAT_TYPE	m_VideoFormatType;
	int					m_nSecondDevID;
	VIDEC_CODEC_TYPE	m_SecondVideoCodecType;
	int					m_nSecondFrameRate;
	int					m_nSecondBitrate;
	VIDEO_FORMAT_TYPE	m_SecondVideoFormatType;
    int                 m_nMainVideoNetworkType;
    int                 m_nSecondVideoNetworkType;

	int					m_nSupportH239;
	IMXEC_PlayChan*		m_pPlayChan;

	int					m_nLineID;
public:
	/* OnIMXEC_CallRingIn
	*�����������¼������ն˺��б�����ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*�����������¼�������������гɹ���ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*���������н����¼�����˫������ͨ���󡾱���������Զ�˵ĺ��������Զ�˽����˱����ĺ�������ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*���������������¼�����ͨ�������󡾱����Ҷϻ�Զ�˹Ҷϡ�ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			cszUserID		-�û���ʶ���û�IP
			cszUserName		-�û�����
			nReason			-����ԭ��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason);

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*���������յ�PTZ�¼������Է�������̨��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			nPTZAction		-��̨����
			nSpeed			-�������ٶ�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnIMXEC_CallRecvdDTMF
	*���������յ�DTMF�¼������Է�����DTMF��������ʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
			szDtmf			-DTMF����
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf);
	//end [20140730]

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*������������ʼ�¼�����������ʼʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID);

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*����������ֹͣ�¼���������ֹͣʱ��ϵͳ����ô��¼�
	*���룺	cszCallID		-���б�ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID);

	//��Ƶ���ݻص�����
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	//��Ƶ���ݻص�����
	//nKeyFrame,0:�ǹؼ�֡ 1:�ؼ�֡
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*��������������״̬�Ļص�
	*���룺	cs					-��������״̬
	*����� ��
	*����ֵ����
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeGetUserInfo
	*��������ȡ�û���Ϣ�Ļص�
	*���룺	cszUserID				-�û���ʶ
	*����� szNodeID				-�û�������ڵ��ʶ
	*����ֵ���ɹ�����0��ʧ�ܷ���-1����ʾ���û�������
	*****************************************************************************/
	virtual int OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID);
public:
// �Ի�������
	enum { IDD = IDD_IMXECTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL DestroyWindow();
	CButton m_btnMakeCall;
	CButton m_btnHangupCall;
	CButton m_btnAcceptCall;
	CStatic m_btnStatus;
	CButton m_btnNetworkSetting;
	BOOL m_bAutoAnswer;

	afx_msg void OnBnClickedButtonMakeCall();
	afx_msg void OnBnClickedButtonHangupCall();
	afx_msg void OnBnClickedButtonAcceptCall();
	afx_msg void OnBnClickedButtonNetworkSetting();
	afx_msg void OnBnClickedButtonAudioSetting();
	CButton m_btnAutoAnswer;
	afx_msg void OnBnClickedCheckAutoAnswer();
	CButton m_btnVideoSetting;
	afx_msg void OnBnClickedButtonVideoSetting();
	afx_msg void OnBnClickedButtonCallInfo();
	CButton m_btnCallInfo;
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	CButton m_btnZoomIn;
	CButton m_btnZoomOut;
	afx_msg void OnBnClickedButtonZoomIn();
	afx_msg void OnBnClickedButtonZoomOut();
	CButton m_btnSecondVideo;
	BOOL m_nStartSecondVideo;
	afx_msg void OnBnClickedCheckSecondVideo();
	afx_msg void OnBnClickedButtonDialpad();

	void AddSIPAccount();
	//test add
	void ReadSipSet();
	void ReadICESet();
	//notice borrow sip struct
	void ReadH323RegSet();
	/******************************************************************************
	* OnIMXEC_H323RegistrationSuccess
	*������H323ע��ɹ��¼�
	*���룺cszGKHost ע�ᵽ��GK��ַ port ע�ᵽ��GK�˿�
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationSuccess(const char*cszGKHost,int port);

	/******************************************************************************
	* OnIMXEC_H323RegistrationError
	*������H323ע��ʧ���¼�
	*���룺��
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationError(void);

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationSuccess
	*������SIPע��ɹ��¼�
	*���룺	nLineID		-��·��ʶ
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID);

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationError
	*������SIPע��ʧ���¼�
	*���룺	nLineID			-��·��ʶ
			cszErrorCode	-������
	*�������
	*����ֵ����
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode);

};
