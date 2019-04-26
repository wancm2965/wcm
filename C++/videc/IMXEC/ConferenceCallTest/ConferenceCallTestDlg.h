// ConferenceCallTestDlg.h : 头文件
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

// CConferenceCallTestDlg 对话框
class CConferenceCallTestDlg 
	: public CDialog
	, public IMXEC_Call
	, public IMXEC_PlayChanCallback
	, public NETEC_Node
{
// 构造
public:
	CConferenceCallTestDlg(CWnd* pParent = NULL);	// 标准构造函数
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
	*描述：呼入事件，有终端呼叫本机的时候系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallRingOut
	*描述：呼出事件，本机发起呼叫成功后系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallEstablished
	*描述：呼叫建立事件，当双方建立通话后【本机接受了远端的呼叫请求或远端接受了本机的呼叫请求】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName);

	/******************************************************************************
	* OnIMXEC_CallDestroyed
	*描述：呼叫销毁事件，当通话结束后【本机挂断或远端挂断】系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszUserID		-用户标识或用户IP
			cszUserName		-用户名称
			nReason			-结束原因
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason);

	/******************************************************************************
	* OnIMXEC_CallRecvdPTZ
	*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnIMXEC_CallRecvdDTMF
	*描述：接收到DTMF事件，当对方发送DTMF控制信令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			szDtmf			-DTMF信令
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf);
	//end [20140730]

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStart
	*描述：辅流开始事件，当辅流开始时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStart(const char*cszCallID);

	/******************************************************************************
	* OnIMXEC_CallSecondVideoStop
	*描述：辅流停止事件，当辅流停止时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_CallSecondVideoStop(const char*cszCallID);

	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp);
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

	/******************************************************************************
	* OnNETEC_NodeConnectStatusChanged
	*描述：网络连接状态的回调
	*输入：	cs					-网络连接状态
	*输出： 无
	*返回值：无
	*****************************************************************************/
	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	/******************************************************************************
	* OnNETEC_NodeGetUserInfo
	*描述：获取用户信息的回调
	*输入：	cszUserID				-用户标识
	*输出： szNodeID				-用户的网络节点标识
	*返回值：成功返回0；失败返回-1，表示该用户不在线
	*****************************************************************************/
	virtual int OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID);
public:
// 对话框数据
	enum { IDD = IDD_IMXECTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	*描述：H323注册成功事件
	*输入：cszGKHost 注册到的GK地址 port 注册到的GK端口
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationSuccess(const char*cszGKHost,int port);

	/******************************************************************************
	* OnIMXEC_H323RegistrationError
	*描述：H323注册失败事件
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_H323RegistrationError(void);

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationSuccess
	*描述：SIP注册成功事件
	*输入：	nLineID		-线路标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID);

	/******************************************************************************
	* OnIMXEC_SIPVirtualLineRegistrationError
	*描述：SIP注册失败事件
	*输入：	nLineID			-线路标识
			cszErrorCode	-错误码
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode);

};
