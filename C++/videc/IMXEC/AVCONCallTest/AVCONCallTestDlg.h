// AVCONCallTestDlg.h : 头文件
//

#pragma once

#include "afxwin.h"
#include "DlgVideo.h"
#include <IMXEC/IMXEC_AVCONCall.h>
#include <string>


// CAVCONCallTestDlg 对话框
class CAVCONCallTestDlg 
	: public CDialog
	, public IMXEC_AVCONCall
{
// 构造
public:
	CAVCONCallTestDlg(CWnd* pParent = NULL);	// 标准构造函数
public:
	CString				m_strServerIP;
	CString				m_strUserID;
	CString				m_strPassword;


	std::string			m_strCurrentCallID;
	DlgVideo*			m_pDlgVideoLocal;
	DlgVideo*			m_pDlgVideoRemote;

	CString				m_strLastUserID;

	int					m_nMicDevID;
	int					m_nSpkDevID;
	int					m_nAEC;
	int					m_nVAD;
	int					m_nAGC;
	int					m_nNS;

	int					m_nDevID;
	int					m_nFrameRate;
	int					m_nBitrate;
	int					m_nCbrOrVbr;

	IMXEC_AVCONCall::VIDEO_FORMAT	m_nVideoFormatType;
public:
	virtual void OnIMXEC_AVCONCallLoginOK(const char*cszUserID);
	virtual void OnIMXEC_AVCONCallLoginFailed(const char*cszUserID,int nReason);

	virtual void OnIMXEC_AVCONCallTrying(const char*cszUserID,const char*cszUserName);
	virtual void OnIMXEC_AVCONCallRingOut(const char*cszUserID,const char*cszUserName);
	virtual void OnIMXEC_AVCONCallRingIn(const char*cszUserID,const char*cszUserName);
	virtual void OnIMXEC_AVCONCallEstablished(const char*cszUserID,const char*cszUserName);
	virtual void OnIMXEC_AVCONCallDestroyed(const char*cszUserID,const char*cszUserName,IMXEC_AVCONCall::CALL_DESTROYED_REASON nReason);
public:
// 对话框数据
	enum { IDD = IDD_AVCONCALL_DIALOG };

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

	afx_msg void OnBnClickedButtonAudioSetting();
	CButton m_btnAutoAnswer;
	afx_msg void OnBnClickedCheckAutoAnswer();
	CButton m_btnVideoSetting;
	afx_msg void OnBnClickedButtonVideoSetting();
};
