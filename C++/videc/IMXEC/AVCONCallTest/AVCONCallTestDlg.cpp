// AVCONCallTestDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AVCONCallTest.h"
#include "AVCONCallTestDlg.h"
#include "DlgCall.h"
#include "DlgAudioSetting.h"
#include "DlgVideoSetting.h"
#include "DlgLogin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define LEFT_OFFSET	12
#define TOP_OFFSET	12

// CAVCONCallTestDlg �Ի���

CAVCONCallTestDlg::CAVCONCallTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAVCONCallTestDlg::IDD, pParent)
	, m_bAutoAnswer(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strServerIP=_T("192.168.29.111");
	m_strUserID=_T("100");
	m_strPassword=_T("");

	m_strCurrentCallID="";
	m_pDlgVideoLocal=NULL;
	m_pDlgVideoRemote=NULL;
	
	m_nDevID=0;
	m_nFrameRate=15;
	m_nBitrate=1024;
	m_nVideoFormatType=IMXEC_AVCONCall::VF_4CIF;
	m_nCbrOrVbr=1;

	m_nMicDevID=-1;
	m_nSpkDevID=-1;
	m_nAEC=1;
	m_nVAD=1;
	m_nAGC=1;
	m_nNS=1;
}

void CAVCONCallTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MAKE_CALL, m_btnMakeCall);
	DDX_Control(pDX, IDC_BUTTON_HANGUP_CALL, m_btnHangupCall);
	DDX_Control(pDX, IDC_BUTTON_ACCEPT_CALL, m_btnAcceptCall);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_btnStatus);
	DDX_Check(pDX, IDC_CHECK_AUTO_ANSWER, m_bAutoAnswer);
	DDX_Control(pDX, IDC_CHECK_AUTO_ANSWER, m_btnAutoAnswer);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_SETTING, m_btnVideoSetting);
}

BEGIN_MESSAGE_MAP(CAVCONCallTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_MAKE_CALL, &CAVCONCallTestDlg::OnBnClickedButtonMakeCall)
	ON_BN_CLICKED(IDC_BUTTON_HANGUP_CALL, &CAVCONCallTestDlg::OnBnClickedButtonHangupCall)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT_CALL, &CAVCONCallTestDlg::OnBnClickedButtonAcceptCall)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_SETTING, &CAVCONCallTestDlg::OnBnClickedButtonAudioSetting)
	ON_BN_CLICKED(IDC_CHECK_AUTO_ANSWER, &CAVCONCallTestDlg::OnBnClickedCheckAutoAnswer)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_SETTING, &CAVCONCallTestDlg::OnBnClickedButtonVideoSetting)
END_MESSAGE_MAP()


// CAVCONCallTestDlg ��Ϣ�������

BOOL CAVCONCallTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_btnHangupCall.EnableWindow(FALSE);
	m_btnAcceptCall.EnableWindow(FALSE);
	m_btnMakeCall.EnableWindow(FALSE);

	m_pDlgVideoLocal=new DlgVideo(this);
	m_pDlgVideoLocal->Create(DlgVideo::IDD,this);
	m_pDlgVideoLocal->SetWindowPos(NULL,LEFT_OFFSET,TOP_OFFSET,352,288,SWP_SHOWWINDOW);

	m_pDlgVideoRemote=new DlgVideo(this);
	m_pDlgVideoRemote->Create(DlgVideo::IDD,this);
	m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,SWP_SHOWWINDOW);


	DlgLogin dlgLogin(this);
	dlgLogin.m_strServerIP=m_strServerIP;
	dlgLogin.m_strUserID=m_strUserID;
	dlgLogin.m_strPassword=m_strPassword;
	if (IDOK==dlgLogin.DoModal())
	{
		m_strServerIP=dlgLogin.m_strServerIP;
		m_strUserID=dlgLogin.m_strUserID;
		m_strPassword=dlgLogin.m_strPassword;

		IMXEC_AVCONCall::Login(m_strUserID,m_strPassword,m_strServerIP,5168);
	}


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAVCONCallTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAVCONCallTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CAVCONCallTestDlg::DestroyWindow()
{
	if (m_pDlgVideoLocal!=NULL)
	{
		m_pDlgVideoLocal->DestroyWindow();
		delete m_pDlgVideoLocal;
		m_pDlgVideoLocal=NULL;
	}

	if (m_pDlgVideoRemote!=NULL)
	{
		m_pDlgVideoRemote->DestroyWindow();
		delete m_pDlgVideoRemote;
		m_pDlgVideoRemote=NULL;
	}

	IMXEC_AVCONCall::Logout();

	return CDialog::DestroyWindow();
}


void CAVCONCallTestDlg::OnBnClickedButtonMakeCall()
{
	DlgCall dlgCall(this);
	dlgCall.m_strUserID=m_strLastUserID;
	if (dlgCall.DoModal()==IDOK)
	{
		m_strLastUserID=dlgCall.m_strUserID;

		int nRet=IMXEC_AVCONCall::MakeCall(m_strLastUserID);
		m_strCurrentCallID=m_strLastUserID;

		if (nRet==0)
		{
			m_btnHangupCall.EnableWindow(TRUE);
			m_btnAcceptCall.EnableWindow(FALSE);
			m_btnMakeCall.EnableWindow(FALSE);

			CString strStatus="";
			strStatus.Format("���ں���:%s %s",m_strLastUserID,m_strLastUserID);
			m_btnStatus.SetWindowTextA(strStatus);
		}
		else
		{
			CString strStatus="";
			strStatus.Format("����ʧ��:%s %s",m_strLastUserID,m_strLastUserID);
			m_btnStatus.SetWindowTextA(strStatus);
		}
	}
}

void CAVCONCallTestDlg::OnBnClickedButtonHangupCall()
{
	IMXEC_AVCONCall::EndCall(m_strCurrentCallID.c_str());
}

void CAVCONCallTestDlg::OnBnClickedButtonAcceptCall()
{
	IMXEC_AVCONCall::AcceptCall(m_strCurrentCallID.c_str());
}



void CAVCONCallTestDlg::OnBnClickedButtonAudioSetting()
{
	DlgAudioSetting dlgAudioSetting(this);
	dlgAudioSetting.m_nMicDevID=m_nMicDevID;
	dlgAudioSetting.m_nSpkDevID=m_nSpkDevID;
	dlgAudioSetting.m_nAEC=m_nAEC;
	dlgAudioSetting.m_nVAD=m_nVAD;
	dlgAudioSetting.m_nAGC=m_nAGC;
	dlgAudioSetting.m_nNS=m_nNS;

	if (dlgAudioSetting.DoModal()==IDOK)
	{
		m_nMicDevID=dlgAudioSetting.m_nMicDevID;
		m_nSpkDevID=dlgAudioSetting.m_nSpkDevID;
		m_nAEC=dlgAudioSetting.m_nAEC;
		m_nVAD=dlgAudioSetting.m_nVAD;
		m_nAGC=dlgAudioSetting.m_nAGC;
		m_nNS=dlgAudioSetting.m_nNS;

		IMXEC_AVCONCall::SetAudioDev(m_nMicDevID,m_nSpkDevID,m_nAEC,m_nVAD,m_nAGC,m_nNS);
	}
}

void CAVCONCallTestDlg::OnBnClickedButtonVideoSetting()
{
	DlgVideoSetting dlgVideoSetting(this);
	dlgVideoSetting.m_nDevID=m_nDevID;
	dlgVideoSetting.m_nFrameRate=m_nFrameRate;
	dlgVideoSetting.m_nBitrate=m_nBitrate;
	dlgVideoSetting.m_nCbrOrVbr=m_nCbrOrVbr;
	dlgVideoSetting.m_nVideoFormatType=m_nVideoFormatType;

	if (dlgVideoSetting.DoModal()==IDOK)
	{
		m_nDevID=dlgVideoSetting.m_nDevID;
		m_nFrameRate=dlgVideoSetting.m_nFrameRate;
		m_nBitrate=dlgVideoSetting.m_nBitrate;
		m_nVideoFormatType=dlgVideoSetting.m_nVideoFormatType;
		m_nCbrOrVbr=dlgVideoSetting.m_nCbrOrVbr;

		IMXEC_AVCONCall::SetVideoDev(m_nDevID,m_nVideoFormatType,m_nBitrate,0,m_nFrameRate);
	}
}

void CAVCONCallTestDlg::OnBnClickedCheckAutoAnswer()
{
	m_bAutoAnswer=m_btnAutoAnswer.GetCheck();
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallLoginOK(const char*cszUserID)
{
	m_btnMakeCall.EnableWindow(TRUE);
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallLoginFailed(const char*cszUserID,int nReason)
{
	OutputDebugString("OnIMXEC_AVCONCallLoginFailed\n");
	if (nReason==AVCON_CALL_ERROR_USERID)
	{
		OutputDebugString("AVCON_CALL_ERROR_USERID\n");
	}
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallTrying(const char*cszUserID,const char*cszUserName)
{
	CString strStatus="";
	strStatus.Format("CallTrying:%s %s",cszUserID,cszUserName);
	m_btnStatus.SetWindowText(strStatus);
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallRingOut(const char*cszUserID,const char*cszUserName)
{
	CString strStatus="";
	strStatus.Format("CallRingOut:%s %s",cszUserID,cszUserName);
	m_btnStatus.SetWindowText(strStatus);
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallRingIn(const char*cszUserID,const char*cszUserName)
{
	if (m_strCurrentCallID.size())
	{
		IMXEC_AVCONCall::EndCall(cszUserID);
	}
	else
	{
		m_strCurrentCallID=cszUserID;
		m_btnHangupCall.EnableWindow(TRUE);
		m_btnAcceptCall.EnableWindow(TRUE);
		m_btnMakeCall.EnableWindow(FALSE);

		CString strStatus="";
		strStatus.Format("CallRingIn:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowText(strStatus);

		if (m_bAutoAnswer)
		{
			IMXEC_AVCONCall::AcceptCall(cszUserID);
		}
	}
}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallEstablished(const char*cszUserID,const char*cszUserName)
{
	if (m_strCurrentCallID==cszUserID)
	{
		m_btnHangupCall.EnableWindow(TRUE);
		m_btnAcceptCall.EnableWindow(FALSE);
		m_btnMakeCall.EnableWindow(FALSE);

		if (m_pDlgVideoLocal!=NULL)
		{
			IMXEC_AVCONCall::SetCallLocalWnd(cszUserID,m_pDlgVideoLocal->m_hWnd);

			CRect rc;
			m_pDlgVideoLocal->GetClientRect(rc);

			IMXEC_AVCONCall::SetCallLocalWndPos(cszUserID,0,0,rc.Width(),rc.Height());
		}

		if (m_pDlgVideoRemote!=NULL)
		{
			IMXEC_AVCONCall::SetCallRemoteWnd(cszUserID,m_pDlgVideoRemote->m_hWnd);

			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);

			IMXEC_AVCONCall::SetCallRemoteWndPos(cszUserID,0,0,rc.Width(),rc.Height());
		}

		CString strStatus="";
		strStatus.Format("CallEstablished:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowTextA(strStatus);
	}

}

void CAVCONCallTestDlg::OnIMXEC_AVCONCallDestroyed(const char*cszUserID,const char*cszUserName,IMXEC_AVCONCall::CALL_DESTROYED_REASON nReason)
{
	if (m_strCurrentCallID==cszUserID)
	{
		m_strCurrentCallID="";
		m_btnHangupCall.EnableWindow(FALSE);
		m_btnAcceptCall.EnableWindow(FALSE);
		m_btnMakeCall.EnableWindow(TRUE);

		CString strStatus="";
		strStatus.Format("CallDestroyed:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowText(strStatus);
	}
}
