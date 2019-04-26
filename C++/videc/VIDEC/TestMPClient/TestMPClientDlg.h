
// TestMPClientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include <NETEC/NETEC_Node.h>
#include <NETEC/NETEC_Core.h>
#include "NETEC/NETEC_Setting.h"
#include <IMXEC/IMXEC_MediaChannel.h>
#include <IMXEC/IMXEC_Call.h>
#include <IMXEC/IMXEC_IMX.h>
#include <IMXEC/IMXEC_H323.h>
#include "DlgVideo.h"
#include <AUDEC/AUDEC_Engine.h>

//#include "KBASE.h"
//#include "KDBAPI.h"

//#pragma comment(lib,"./KBASE.lib")
//#pragma comment(lib,"./KDBAPI.lib")


// CTestMPClientDlg 对话框
class CTestMPClientDlg
	: public CDialog
	, public NETEC_Node
	, public IMXEC_PlayChanCallback
	, public IMXEC_CapChanCallback
	, public IMXEC_Call
{
// 构造
public:
	CTestMPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TESTMPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	NETEC_Session::CONNECT_STATUS	m_csNet;

	IMXEC_CapChanDev*				m_pChanDevCaptureAudio;
	IMXEC_CapChanDev*				m_pChanDevCapture;
	IMXEC_PlayChan*					m_pIMXECPlayChan;
	IMXEC_CapChanDev*				m_pChanDevCapture2;
	IMXEC_PlayChan*					m_pIMXECPlayChan2;

	DlgVideo*			m_pDlgVideoLocal;
	DlgVideo*			m_pDlgVideoRemote;

	DlgVideo*			m_pDlgVideoLocal2;
	DlgVideo*			m_pDlgVideoRemote2;
	DlgVideo*			m_pDlgVideoLocal3;
	DlgVideo*			m_pDlgVideoRemote3;
	DlgVideo*			m_pDlgVideoRemote4;

	bool				m_bStartRecordToAVI1;
	bool				m_bStartRecordToAVI2;

public:

	static CTestMPClientDlg* Instance();
	void ReSetVideoSize(int nLocal);

	void ShowMsgText(CString strText,const char*cszCallID);
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


	virtual void OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs);

	virtual void OnIMXEC_CapChanCallbackFilePositionChanged(unsigned long ulSeconds);
	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop){};
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode){};

	virtual void OnIMXEC_PlayChanCallbackDrawD3D9DC(void* hDC/*HDC*/);
	virtual void OnIMXEC_CapChanCallbackDrawD3D9DC(void* hDC/*HDC*/);


	bool CheckNew(void);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonNetwork();
	afx_msg void OnBnClickedButtonClientCapture();
	afx_msg void OnBnClickedButtonClientCaptureStop();
	afx_msg void OnBnClickedButtonClientReceive();
	afx_msg void OnBnClickedButtonClientReceiveStop();
	CEdit m_editServer;
	CEdit m_editLocalIP;
	CEdit m_editPort1;
	CEdit m_editPort2;
	CEdit m_editServer2;
	CEdit m_editCapture;
	CEdit m_editCapture2;
	CEdit m_editReceive;
	CEdit m_editReceive2;
	virtual BOOL DestroyWindow();
	CEdit m_editDevID;
	CEdit m_editDevID2;
	CEdit m_editDevID3;
	CEdit m_editFrameRate1;
	CEdit m_editFrameRate2;
	CEdit m_editFrameRate3;
	CEdit m_editFrameRate4;
	CEdit m_editDevID1;
	CEdit m_editDevID4;
	CEdit m_editMsg;
	CEdit m_editBitRate1;
	CEdit m_editBitRate2;
	CEdit m_editBitRate3;
	CEdit m_editBitRate4;
	CButton m_btnEncode;
	CButton m_btnAutoSize;
	CButton m_btnDenoise;
	//MySQLDB			m_DBDriver;
	CEdit m_editWidth1;
	CEdit m_editWidth2;
	CEdit m_editHeight1;
	CEdit m_editHeight2;

	std::string			m_strCurrentCallID;
	std::string			m_strCallID2;
	BOOL				m_nStartSecondVideo;

	bool				m_bVideopoll1;
	int					m_nVideopoll1_Status;
	bool				m_bVideopoll2;
	int					m_nVideopoll2_Status;

	DWORD_PTR		m_nAudioCode;
	afx_msg void OnBnClickedButtonClientReceiveRecord();
	afx_msg void OnBnClickedButtonClientReceiveStopRecord();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonClientCaptureSendAudioSend();
	afx_msg void OnBnClickedButtonClientCaptureSendAudioClose();
	afx_msg void OnBnClickedButtonClientCaptureSendVideoStart();
	afx_msg void OnBnClickedButtonClientCaptureSendVideoClose();
	CComboBox m_comboxAudioCode;
	CComboBox m_comboxNetType1;
	CComboBox m_comboxNetType2;
	CComboBox m_comboxH323NetType1;
	CComboBox m_comboxH323NetType2;
	CComboBox m_comboxH323Size1;
	CComboBox m_comboxH323Size2;
	CComboBox m_comboxFrameType1;
	CComboBox m_comboxFrameType2;
	afx_msg void OnCbnSelchangeComboAudioCode();
	afx_msg void OnBnClickedButtonClientOpenMic();
	afx_msg void OnBnClickedButtonClientCloseMic();
	afx_msg void OnBnClickedButtonClientCapture2();
	afx_msg void OnBnClickedButtonClientCaptureStop2();
	afx_msg void OnBnClickedButtonClientReceive2();
	afx_msg void OnBnClickedButtonClientReceiveStop2();
	afx_msg void OnBnClickedButtonH323capture();
	afx_msg void OnBnClickedButtonH323stop();
	afx_msg void OnBnClickedButtonH323call();
	afx_msg void OnBnClickedButtonH323hangup();
	afx_msg void OnBnClickedButtonH323accept();
	afx_msg void OnBnClickedButtonH323second();
	CListBox m_listMsg;
	afx_msg void OnBnClickedButtonCaptureBmp1();
	afx_msg void OnBnClickedButtonCaptureBmp2();
	afx_msg void OnCbnSelchangeComboFrametype1();
	afx_msg void OnCbnSelchangeComboFrametype2();
	afx_msg void OnBnClickedButtonVideopoll1();
	afx_msg void OnBnClickedButtonVideopoll2();
	afx_msg void OnBnClickedButtonRecvRecord();
	afx_msg void OnBnClickedButtonRecvRecord2();
};
