// ConferenceCallTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConferenceCallTest.h"
#include "ConferenceCallTestDlg.h"
#include "DlgCall.h"
#include "DlgNetworkSetting.h"
#include "DlgAudioSetting.h"
#include "DlgVideoSetting.h"
#include "DlgCallInfo.h"
#include "DlgDialPad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define HAS_SIP

#define LEFT_OFFSET	12
#define TOP_OFFSET	12

// CConferenceCallTestDlg 对话框
void CConferenceCallTestDlg::SetH235AuthFromINI()
{
	char senderId[256]={0};
	char generalId[256]={0};
	char password[64]={0};
	int nAnnexI=0;
	int nAnnexIA=0;
	int nAnnexII=0;
	int nAnnexIII=0;
	int nAnnexIV=0;
	int nDH512=0;
	int nDH1024=0;
	int nDH1536=0;
	int nCheckRecvMsg=0;
	int nUsePrime=0;
	int nH235MediaOption=0;
	int nUseAlogrithmId=0;
	int nH235AuthOption=0;

	CString strPath = _T("");
	GetModuleFileName(NULL,strPath.GetBufferSetLength(512 + 1),512);
	int n = strPath.ReverseFind('\\');
	strPath = strPath.Left(n);
	strPath += _T("\\H235Auth.ini");

	GetPrivateProfileString("AUTH", "SenderId", "",senderId,254, strPath );
	GetPrivateProfileString("AUTH", "GeneralId", "",generalId,254, strPath );
	GetPrivateProfileString("AUTH", "Password", "",password,254, strPath );



	nH235AuthOption=GetPrivateProfileInt("AUTH","H235AuthOption",0,strPath);
	nH235MediaOption=GetPrivateProfileInt("AUTH","H235MediaOption",0,strPath);
	nAnnexI=GetPrivateProfileInt("AUTH","AnnexI",0,strPath);
	nAnnexIA=GetPrivateProfileInt("AUTH","AnnexIA",0,strPath);
	nAnnexII=GetPrivateProfileInt("AUTH","AnnexII",0,strPath);
	nAnnexIII=GetPrivateProfileInt("AUTH","AnnexIII",0,strPath);
	nAnnexIV=GetPrivateProfileInt("AUTH","AnnexIV",0,strPath);
	nDH512=GetPrivateProfileInt("AUTH","DH512",0,strPath);
	nDH1024=GetPrivateProfileInt("AUTH","DH1024",0,strPath);
	nDH1536=GetPrivateProfileInt("AUTH","DH1536",0,strPath);
	nCheckRecvMsg=GetPrivateProfileInt("AUTH","CheckRecvMsg",0,strPath);
	nUsePrime=GetPrivateProfileInt("AUTH","UsePrime",0,strPath);
	nUseAlogrithmId=GetPrivateProfileInt("AUTH","UseAlogrithmId",0,strPath);

	int nH235MediaSpecified=0;
	char H235MediaEncPWD[64]={0};
	nH235MediaSpecified=GetPrivateProfileInt("AUTH","H235MediaSpecified",0,strPath);
	GetPrivateProfileString("AUTH", "H235MediaEncPWD", "",H235MediaEncPWD,254, strPath );

	IMXEC_H323::SetH235SecurityAuthOption(nH235AuthOption);
	IMXEC_H323::SetH235MediaSecurityOption(nH235MediaOption);
	IMXEC_H323::SetH235SecurityAuth(senderId,generalId,password);

	IMXEC_H323::SetH235SecurityAnnexIEnable(false);
	IMXEC_H323::SetH235SecurityAnnexIAEnable(false);
	IMXEC_H323::SetH235SecurityAnnexIIEnable(false);
	IMXEC_H323::SetH235SecurityAnnexIIIEnable(false);
	IMXEC_H323::SetH235SecurityMediaDH512Enable(false);
	IMXEC_H323::SetH235SecurityMediaDH1024Enable(false);
	IMXEC_H323::SetH235SecurityMediaDH1536Enable(false);
	IMXEC_H323::SetH235SecurityCheckIncomingMsgEnable(false);
	IMXEC_H323::SetH235SecurityUserPrimeEnable(false);
	IMXEC_H323::SetH235MediaSecurityEncryptionPWD(0,"");

	if(nAnnexI>0)
		IMXEC_H323::SetH235SecurityAnnexIEnable(true);
	if(nAnnexIA>0)
		IMXEC_H323::SetH235SecurityAnnexIAEnable(true);
	if(nAnnexII>0)
		IMXEC_H323::SetH235SecurityAnnexIIEnable(true);
	if(nAnnexIII>0)
		IMXEC_H323::SetH235SecurityAnnexIIIEnable(true);


	if(nDH512>0)
		IMXEC_H323::SetH235SecurityMediaDH512Enable(true);
	if(nDH1024>0)
		IMXEC_H323::SetH235SecurityMediaDH1024Enable(true);
	if(nDH1536>0)
		IMXEC_H323::SetH235SecurityMediaDH1536Enable(true);

	if(nCheckRecvMsg>0)
		IMXEC_H323::SetH235SecurityCheckIncomingMsgEnable(true);
	if(nUsePrime>0)
		IMXEC_H323::SetH235SecurityUserPrimeEnable(true);

	IMXEC_H323::SetH235MediaEncryptionAlogrithmFirst(nUseAlogrithmId);

	if(nH235MediaSpecified==1)
		IMXEC_H323::SetH235MediaSecurityEncryptionPWD(nH235MediaSpecified,H235MediaEncPWD);

}

CConferenceCallTestDlg::CConferenceCallTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConferenceCallTestDlg::IDD, pParent)
	, m_AudioCodecID(X_AUDIO_CODEC_G711U)
	, m_bAutoAnswer(TRUE)
	, m_nStartSecondVideo(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_strCurrentCallID="";
	m_pDlgVideoLocal=NULL;
	m_pDlgVideoRemote=NULL;
	m_pDlgVideoRemote2=NULL;
	m_pDlgSecondVideo=NULL;
	
	m_nH323ListenPort=1720;
	m_nRTPBasePort=12000;
	m_strLocalIP="";

	m_strLastPeerIP="";

	m_nDevID=0;
	m_VideoCodecType=VIDEC_CODEC_H264;
	m_nFrameRate=15;
	m_nBitrate=384;
	m_VideoFormatType=VIDEO_FORMAT_CIF;
	m_nSecondDevID=0;
	m_SecondVideoCodecType=VIDEC_CODEC_H264;
	m_nSecondFrameRate=15;
	m_nSecondBitrate=384;
	m_SecondVideoFormatType=VIDEO_FORMAT_CIF;
    m_nMainVideoNetworkType=VIDEC_NETWORK_INTERNET;
    m_nSecondVideoNetworkType=VIDEC_NETWORK_INTERNET;

	m_nSupportH239=0;
	m_pPlayChan=NULL;

	m_nLineID=-1;
}

void CConferenceCallTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_MAKE_CALL, m_btnMakeCall);
	DDX_Control(pDX, IDC_BUTTON_HANGUP_CALL, m_btnHangupCall);
	DDX_Control(pDX, IDC_BUTTON_ACCEPT_CALL, m_btnAcceptCall);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_btnStatus);
	DDX_Control(pDX, IDC_BUTTON_NETWORK_SETTING, m_btnNetworkSetting);
	DDX_Check(pDX, IDC_CHECK_AUTO_ANSWER, m_bAutoAnswer);
	DDX_Control(pDX, IDC_CHECK_AUTO_ANSWER, m_btnAutoAnswer);
	DDX_Control(pDX, IDC_BUTTON_VIDEO_SETTING, m_btnVideoSetting);
	DDX_Control(pDX, IDC_BUTTON_CALL_INFO, m_btnCallInfo);
	DDX_Control(pDX, IDC_BUTTON_ZOOM_IN, m_btnZoomIn);
	DDX_Control(pDX, IDC_BUTTON_ZOOM_OUT, m_btnZoomOut);
	DDX_Control(pDX, IDC_CHECK_SECOND_VIDEO, m_btnSecondVideo);
	DDX_Check(pDX, IDC_CHECK_SECOND_VIDEO, m_nStartSecondVideo);
}

BEGIN_MESSAGE_MAP(CConferenceCallTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_MAKE_CALL, &CConferenceCallTestDlg::OnBnClickedButtonMakeCall)
	ON_BN_CLICKED(IDC_BUTTON_HANGUP_CALL, &CConferenceCallTestDlg::OnBnClickedButtonHangupCall)
	ON_BN_CLICKED(IDC_BUTTON_ACCEPT_CALL, &CConferenceCallTestDlg::OnBnClickedButtonAcceptCall)
	ON_BN_CLICKED(IDC_BUTTON_NETWORK_SETTING, &CConferenceCallTestDlg::OnBnClickedButtonNetworkSetting)
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_SETTING, &CConferenceCallTestDlg::OnBnClickedButtonAudioSetting)
	ON_BN_CLICKED(IDC_CHECK_AUTO_ANSWER, &CConferenceCallTestDlg::OnBnClickedCheckAutoAnswer)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_SETTING, &CConferenceCallTestDlg::OnBnClickedButtonVideoSetting)
	ON_BN_CLICKED(IDC_BUTTON_CALL_INFO, &CConferenceCallTestDlg::OnBnClickedButtonCallInfo)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CConferenceCallTestDlg::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CConferenceCallTestDlg::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CConferenceCallTestDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CConferenceCallTestDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_IN, &CConferenceCallTestDlg::OnBnClickedButtonZoomIn)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_OUT, &CConferenceCallTestDlg::OnBnClickedButtonZoomOut)
	ON_BN_CLICKED(IDC_CHECK_SECOND_VIDEO, &CConferenceCallTestDlg::OnBnClickedCheckSecondVideo)
	ON_BN_CLICKED(IDC_BUTTON_DIALPAD, &CConferenceCallTestDlg::OnBnClickedButtonDialpad)
END_MESSAGE_MAP()


//test add
void CConferenceCallTestDlg::ReadICESet()
{
	CString m_strIniPath = _T("");
	GetModuleFileName(NULL,m_strIniPath.GetBufferSetLength(512 + 1),512);
	int n = m_strIniPath.ReverseFind('\\');
	m_strIniPath = m_strIniPath.Left(n);
	m_strIniPath += _T("\\ICESET.ini");
	CString strStun="";
	CString strUserName="";
	CString strPassword="";
	CString strTurn="";
	CString strDNS1="";
	CString strDNS2="";
	bool bEnableICE = (1==GetPrivateProfileInt(("ICE"),("EnableICE"),0,m_strIniPath)) ? true : false;
	GetPrivateProfileString(("ICE"),("DNS1"),(""),strDNS1.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("ICE"),("DNS2"),(""),strDNS2.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("ICE"),("STUN"),(""),strStun.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("ICE"),("TURN"),(""),strTurn.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("ICE"),("UserName"),(""),strUserName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("ICE"),("Password"),(""),strPassword.GetBufferSetLength(256),256,m_strIniPath);
	int nCompCount = GetPrivateProfileInt(("ICE"), ("COMPCOUNT"), 1, m_strIniPath);
	int nTurnType = GetPrivateProfileInt(("ICE"), ("TURNTYPE"), 17, m_strIniPath);
	IMXEC_SIP::SetICEEnable(bEnableICE);
	IMXEC_SIP::SetICETransportCount(nCompCount);
	IMXEC_SIP::SetDNSServerIP(strDNS1.GetBuffer(strDNS1.GetLength()));
	IMXEC_SIP::SetDNSServerIP2(strDNS2.GetBuffer(strDNS2.GetLength()));
	IMXEC_SIP::SetSTUNServerIPPort(strStun.GetBuffer(strStun.GetLength()));
	IMXEC_SIP::SetTURNServerIPPort(strTurn.GetBuffer(strTurn.GetLength()));
	IMXEC_SIP::SetTURNLoginAuth(strUserName.GetBuffer(strUserName.GetLength()),strPassword.GetBuffer(strPassword.GetLength()));
	IMXEC_SIP::SetTurnTransportType(nTurnType);
}

typedef struct _SIP_SET
{
	IMXEC_SIP::TRANS_TYPE	nChannelType;		//通道类型
	bool					bEnableSIP;			//启用SIP
	CString					strDisplayName;		//显示名
	CString					strUserName;		//用户名
	CString					strPassword;		//密码
	CString					strDomain;			//域名
	CString					strAuthorUserName;	//认证
	CString					strProxyServer;		//代理服务器
	int						iSipLineId;
}TSipSet;

TSipSet g_tSipSet;

void CConferenceCallTestDlg::ReadSipSet()
{
	TSipSet & tSipSet=g_tSipSet;
	CString m_strIniPath = _T("");
	GetModuleFileName(NULL,m_strIniPath.GetBufferSetLength(512 + 1),512);
	int n = m_strIniPath.ReverseFind('\\');
	m_strIniPath = m_strIniPath.Left(n);
	m_strIniPath += _T("\\SIPSET.ini");

	tSipSet.bEnableSIP = (1==GetPrivateProfileInt(("SIP"),("EnableSIP"),0,m_strIniPath)) ? true : false;
	GetPrivateProfileString(("SIP"),("DisplayName"),(""),tSipSet.strDisplayName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("SIP"),("UserName"),(""),tSipSet.strUserName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("SIP"),("Password"),(""),tSipSet.strPassword.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("SIP"),("Domain"),(""),tSipSet.strDomain.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("SIP"),("AuthorName"),(""),tSipSet.strAuthorUserName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("SIP"),("Proxy"),(""),tSipSet.strProxyServer.GetBufferSetLength(256),256,m_strIniPath);
	tSipSet.nChannelType = (IMXEC_SIP::TRANS_TYPE)GetPrivateProfileInt(("SIP"), ("ChanelType"), IMXEC_SIP::TRANS_UDP, m_strIniPath);
}

void CConferenceCallTestDlg::AddSIPAccount()
{
	TSipSet & tSipSet=g_tSipSet;
	if(tSipSet.bEnableSIP)
		IMXEC_SIP::AddVirtualLine((g_tSipSet.strUserName),(g_tSipSet.strDisplayName),
		(g_tSipSet.strPassword),(g_tSipSet.strProxyServer),(g_tSipSet.strAuthorUserName),(g_tSipSet.strDomain));
}

TSipSet g_tH323Set;//notice borrow use sip struct
void CConferenceCallTestDlg::ReadH323RegSet()
{
	TSipSet & tSipSet=g_tH323Set;
	CString m_strIniPath = _T("");
	GetModuleFileName(NULL,m_strIniPath.GetBufferSetLength(512 + 1),512);
	int n = m_strIniPath.ReverseFind('\\');
	m_strIniPath = m_strIniPath.Left(n);
	m_strIniPath += _T("\\H323RegSET.ini");
	tSipSet.bEnableSIP = (1==GetPrivateProfileInt(("H323"),("EnableReg"),0,m_strIniPath)) ? true : false;
	GetPrivateProfileString(("H323"),("DisplayName"),(""),tSipSet.strDisplayName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("H323"),("UserName"),(""),tSipSet.strUserName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("H323"),("PhoneNum"),(""),tSipSet.strPassword.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("H323"),("AuthorName"),(""),tSipSet.strAuthorUserName.GetBufferSetLength(256),256,m_strIniPath);
	GetPrivateProfileString(("H323"),("GKAddr"),(""),tSipSet.strProxyServer.GetBufferSetLength(256),256,m_strIniPath);
	tSipSet.nChannelType = (IMXEC_SIP::TRANS_TYPE)GetPrivateProfileInt(("H323"), ("RegType"), IMXEC_SIP::TRANS_UDP, m_strIniPath);
	tSipSet.iSipLineId = GetPrivateProfileInt(("H323"),("H460"),0,m_strIniPath);
}


BOOL CConferenceCallTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    IMXEC_Call::SetNetecTransEnable(NULL,1);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	m_btnHangupCall.EnableWindow(FALSE);
	m_btnAcceptCall.EnableWindow(FALSE);
	m_btnCallInfo.EnableWindow(FALSE);
	m_btnMakeCall.EnableWindow(TRUE);
	m_btnNetworkSetting.EnableWindow(TRUE);
	m_btnSecondVideo.EnableWindow(FALSE);

	NETEC_Core::Start(0);
	//NETEC_Node::SetServerIP("192.168.7.40");
	//NETEC_Node::SetServerPort(4222);
	//NETEC_Node::Start();

	m_pDlgVideoLocal=new DlgVideo(this);
	m_pDlgVideoLocal->Create(DlgVideo::IDD,this);
	m_pDlgVideoLocal->SetWindowPos(NULL,LEFT_OFFSET,TOP_OFFSET,352,288,SWP_SHOWWINDOW);

	m_pDlgVideoRemote=new DlgVideo(this);
	m_pDlgVideoRemote->Create(DlgVideo::IDD,this);
	m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoRemote2=new DlgVideo(this);
	m_pDlgVideoRemote2->Create(DlgVideo::IDD,this);
	m_pDlgVideoRemote2->SetWindowPos(NULL,LEFT_OFFSET+352+176,TOP_OFFSET,176,288,SWP_HIDEWINDOW);

#ifdef HAS_H323 //#endif
	IMXEC_H323::SetH323ListenPort(m_nH323ListenPort);
	IMXEC_H323::SetRTPBasePort(m_nRTPBasePort);
	IMXEC_H323::SetLocalIP(m_strLocalIP);
	IMXEC_H323::SetSupportH239(m_nSupportH239);
#endif

	IMXEC_AudioSetting::SetAudioDevParam(m_AudioCodecID);
	//IMXEC_VideoSetting::SetMainVideoDevParam(m_nDevID,m_nFrameRate,m_nBitrate,m_VideoCodecType,m_VideoFormatType);
    IMXEC_VideoSetting::SetMainVideoDevParam(m_nDevID,m_nFrameRate,m_nBitrate,m_VideoCodecType,m_VideoFormatType,m_nMainVideoNetworkType);
	//if(m_nSecondDevID ==0) m_nSecondDevID = -1;
	//IMXEC_VideoSetting::SetSecondVideoDevParam(m_nSecondDevID,m_nSecondFrameRate,m_nSecondBitrate,m_SecondVideoCodecType,m_SecondVideoFormatType);
    IMXEC_VideoSetting::SetSecondVideoDevParam(m_nSecondDevID,m_nSecondFrameRate,m_nSecondBitrate,m_SecondVideoCodecType,m_SecondVideoFormatType,m_nSecondVideoNetworkType);
    
	IMXEC_Call::SetCallType(IMXEC_PERSONAL_CALL);
	//IMXEC_Call::SetCallType(IMXEC_MCU_CALL);
	//IMXEC_Call::SetCallType(IMXEC_CONFERENCE_CALL);
	IMXEC_Call::Start();

#ifdef HAS_H323 //#endif
	ReadH323RegSet();
	TSipSet & tSipSet=g_tH323Set;
	SetH235AuthFromINI();
	//user name
	if(tSipSet.strDisplayName !="")
		IMXEC_H323::SetUserName(tSipSet.strDisplayName);
	if(tSipSet.bEnableSIP)
	{
		//reg host
		if(tSipSet.strProxyServer !="")
			IMXEC_H323::SetRegHost(tSipSet.strProxyServer);
		//h323 id
		if(tSipSet.strUserName !="")
			IMXEC_H323::SetH323ID(tSipSet.strUserName);
		//phone num
		if(tSipSet.strPassword !="")
			IMXEC_H323::SetTelNumber(tSipSet.strPassword);
		//strPassword
		if(tSipSet.strAuthorUserName !="")
			IMXEC_H323::SetRegPassword(tSipSet.strAuthorUserName);

		IMXEC_H323::SetH460NATFwOption(tSipSet.iSipLineId);

		//reg type
		IMXEC_H323::Register(tSipSet.nChannelType);
	}
	IMXEC_H323::Start();
#endif

#ifdef HAS_SIP
	//test add
	ReadSipSet();
	ReadICESet();
	IMXEC_SIP::SetSIPTransportType(g_tSipSet.nChannelType);

	IMXEC_SIP::SetSIPListenPort(5060);
	IMXEC_SIP::Start();
	AddSIPAccount();
	IMXEC_SIP::SetRTPBasePort(10000);
#endif

	IMXEC_IMX::SetUserID("1001");
	IMXEC_IMX::SetUserName("1001");
	IMXEC_IMX::Start();

	VIDEC_SetScreenPos(0,0,0,1440,900);
	VIDEC_SetScreenWndCount(0,4);
	VIDEC_SetScreenWndPos(0,0,0,0,720,450);
	VIDEC_SetScreenWndPos(0,1,720,0,720,450);
	VIDEC_SetScreenWndPos(0,2,0,450,720,450);
	VIDEC_SetScreenWndPos(0,3,720,450,720,450);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
/******************************************************************************
* OnIMXEC_H323RegistrationSuccess
*描述：H323注册成功事件
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_H323RegistrationSuccess(const char*cszGKHost,int port)
{
	CString strStatus="";
	strStatus.Format("H323 Registration Success");
	m_btnStatus.SetWindowTextA(strStatus);
}

/******************************************************************************
* OnIMXEC_H323RegistrationError
*描述：H323注册失败事件
*输入：无
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_H323RegistrationError()
{
	CString strStatus="";
	strStatus.Format("H323 Registration Error");
	m_btnStatus.SetWindowTextA(strStatus);
}


/******************************************************************************
* OnIMXEC_SIPVirtualLineRegistrationSuccess
*描述：SIP注册成功事件
*输入：	nLineID		-线路标识
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_SIPVirtualLineRegistrationSuccess(int nLineID)
{
	CString strStatus="";
	strStatus.Format("SIP Registration Success");
	m_btnStatus.SetWindowTextA(strStatus);
}

/******************************************************************************
* OnIMXEC_SIPVirtualLineRegistrationError
*描述：SIP注册失败事件
*输入：	nLineID			-线路标识
cszErrorCode	-错误码
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_SIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode)
{
	CString strStatus="";
	strStatus.Format("SIP Registration Error:%s",cszErrorCode);
	m_btnStatus.SetWindowTextA(strStatus);
}

void CConferenceCallTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CConferenceCallTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CConferenceCallTestDlg::DestroyWindow()
{

#ifdef HAS_H323 //#endif
	IMXEC_H323::Stop();
#endif

#ifdef HAS_SIP
	IMXEC_SIP::Stop();
#endif
	IMXEC_IMX::Stop();

	IMXEC_Call::Stop();

	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan=NULL;
	}

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

	if (m_pDlgVideoRemote2!=NULL)
	{
		m_pDlgVideoRemote2->DestroyWindow();
		delete m_pDlgVideoRemote2;
		m_pDlgVideoRemote2=NULL;
	}

	if (m_pDlgSecondVideo!=NULL)
	{
		m_pDlgSecondVideo->DestroyWindow();
		delete m_pDlgSecondVideo;
		m_pDlgSecondVideo=NULL;
	}

	NETEC_Node::Stop();
	NETEC_Core::Stop();

	return CDialog::DestroyWindow();
}

void CConferenceCallTestDlg::OnBnClickedButtonMakeCall()
{
	DlgCall dlgCall(this);
	dlgCall.m_strPeerIP=m_strLastPeerIP;
	if (dlgCall.DoModal()==IDOK)
	{
		m_strLastPeerIP=dlgCall.m_strPeerIP;
		CString strProto=dlgCall.m_strProto;

		m_strCurrentCallID=IMXEC_Call::MakeCall(m_strLastPeerIP,strProto);

		if (m_strCurrentCallID.size())
		{
			m_btnHangupCall.EnableWindow(TRUE);
			m_btnAcceptCall.EnableWindow(FALSE);

#ifdef HAS_H323 //#endif
			m_btnMakeCall.EnableWindow(FALSE);
#endif

			CString strStatus="";
			strStatus.Format("正在呼出:%s %s %s",m_strLastPeerIP,m_strLastPeerIP,strProto);
			m_btnStatus.SetWindowTextA(strStatus);
		}
		else
		{
			CString strStatus="";
			strStatus.Format("呼叫失败:%s %s %s",m_strLastPeerIP,m_strLastPeerIP,strProto);
			m_btnStatus.SetWindowTextA(strStatus);
		}
	}
}

void CConferenceCallTestDlg::OnBnClickedButtonHangupCall()
{
	IMXEC_Call::HangupCall(m_strCurrentCallID.c_str());
}

void CConferenceCallTestDlg::OnBnClickedButtonAcceptCall()
{
	IMXEC_Call::AcceptCall(m_strCurrentCallID.c_str());
}

void CConferenceCallTestDlg::OnBnClickedButtonNetworkSetting()
{

	DlgNetworkSetting dlgNetworkSetting(this);
	dlgNetworkSetting.m_nH323ListenPort=m_nH323ListenPort;
	dlgNetworkSetting.m_nRTPBasePort=m_nRTPBasePort;
	dlgNetworkSetting.m_strLocalIP=m_strLocalIP;
	dlgNetworkSetting.m_nSupportH239=m_nSupportH239;

	if (dlgNetworkSetting.DoModal()==IDOK)
	{
		m_nH323ListenPort=dlgNetworkSetting.m_nH323ListenPort;
		m_nRTPBasePort=dlgNetworkSetting.m_nRTPBasePort;
		m_strLocalIP=dlgNetworkSetting.m_strLocalIP;
		m_nSupportH239=dlgNetworkSetting.m_nSupportH239;

#ifdef HAS_H323 //#endif
		IMXEC_H323::SetH323ListenPort(m_nH323ListenPort);
		IMXEC_H323::SetRTPBasePort(m_nRTPBasePort);
		IMXEC_H323::SetLocalIP(m_strLocalIP);
		IMXEC_H323::SetSupportH239(m_nSupportH239);
		IMXEC_H323::Stop();

		ReadH323RegSet();
		TSipSet & tSipSet=g_tH323Set;

		//user name
		if(tSipSet.strDisplayName !="")
			IMXEC_H323::SetUserName(tSipSet.strDisplayName);
		if(tSipSet.bEnableSIP)
		{
			//reg host
			if(tSipSet.strProxyServer !="")
				IMXEC_H323::SetRegHost(tSipSet.strProxyServer);
			//h323 id
			if(tSipSet.strUserName !="")
				IMXEC_H323::SetH323ID(tSipSet.strUserName);
			//phone num
			if(tSipSet.strPassword !="")
				IMXEC_H323::SetTelNumber(tSipSet.strPassword);
			//strPassword
			if(tSipSet.strAuthorUserName !="")
				IMXEC_H323::SetRegPassword(tSipSet.strAuthorUserName);
			//reg type
			IMXEC_H323::Register(tSipSet.nChannelType);
		}

		IMXEC_H323::Start();
#endif

#ifdef HAS_SIP
		IMXEC_SIP::SetLocalIP(m_strLocalIP);
		IMXEC_SIP::SetSupportH239(m_nSupportH239);
		IMXEC_SIP::Stop();
		//test add
		ReadSipSet();
		ReadICESet();
		IMXEC_SIP::SetSIPTransportType(g_tSipSet.nChannelType);
		IMXEC_SIP::SetSIPListenPort(5060);
		IMXEC_SIP::Start();
		AddSIPAccount();
		IMXEC_SIP::SetRTPBasePort(10000);

#endif
	}
}


void CConferenceCallTestDlg::OnBnClickedButtonAudioSetting()
{
	DlgAudioSetting dlgAudioSetting(this);
	dlgAudioSetting.m_AudioCodecID=m_AudioCodecID;

	if (dlgAudioSetting.DoModal()==IDOK)
	{
		m_AudioCodecID=dlgAudioSetting.m_AudioCodecID;

		IMXEC_AudioSetting::SetAudioDevParam(m_AudioCodecID);
	}
}

void CConferenceCallTestDlg::OnBnClickedButtonVideoSetting()
{
	DlgVideoSetting dlgVideoSetting(this);
	dlgVideoSetting.m_nDevID=m_nDevID;
	dlgVideoSetting.m_nFrameRate=m_nFrameRate;
	dlgVideoSetting.m_nBitrate=m_nBitrate;
	dlgVideoSetting.m_VideoCodecType=m_VideoCodecType;
	dlgVideoSetting.m_VideoFormatType=m_VideoFormatType;
	dlgVideoSetting.m_nSecondDevID=m_nSecondDevID;
	dlgVideoSetting.m_nSecondFrameRate=m_nSecondFrameRate;
	dlgVideoSetting.m_nSecondBitrate=m_nSecondBitrate;
	dlgVideoSetting.m_SecondVideoCodecType=m_SecondVideoCodecType;
	dlgVideoSetting.m_SecondVideoFormatType=m_SecondVideoFormatType;

	if (dlgVideoSetting.DoModal()==IDOK)
	{
		m_nDevID=dlgVideoSetting.m_nDevID;
		m_nFrameRate=dlgVideoSetting.m_nFrameRate;
		m_nBitrate=dlgVideoSetting.m_nBitrate;
		m_VideoCodecType=dlgVideoSetting.m_VideoCodecType;
		m_VideoFormatType=dlgVideoSetting.m_VideoFormatType;
		m_nSecondDevID=dlgVideoSetting.m_nSecondDevID;
		m_nSecondFrameRate=dlgVideoSetting.m_nSecondFrameRate;
		m_nSecondBitrate=dlgVideoSetting.m_nSecondBitrate;
		m_SecondVideoCodecType=dlgVideoSetting.m_SecondVideoCodecType;
		m_SecondVideoFormatType=dlgVideoSetting.m_SecondVideoFormatType;

        //IMXEC_VideoSetting::SetMainVideoDevParam(m_nDevID,m_nFrameRate,m_nBitrate,m_VideoCodecType,m_VideoFormatType);
        IMXEC_VideoSetting::SetMainVideoDevParam(m_nDevID,m_nFrameRate,m_nBitrate,m_VideoCodecType,m_VideoFormatType,m_nMainVideoNetworkType);
		//if(m_nSecondDevID ==0) m_nSecondDevID = -1;
        //IMXEC_VideoSetting::SetSecondVideoDevParam(m_nSecondDevID,m_nSecondFrameRate,m_nSecondBitrate,m_SecondVideoCodecType,m_SecondVideoFormatType);
        IMXEC_VideoSetting::SetSecondVideoDevParam(m_nSecondDevID,m_nSecondFrameRate,m_nSecondBitrate,m_SecondVideoCodecType,m_SecondVideoFormatType,m_nSecondVideoNetworkType);
		if(m_strCurrentCallID.size()>0)
		{
			const char * cszCallID = m_strCurrentCallID.c_str();
			/*是否启动OSD*/
			//IMXEC_Call::SetLocalMainVideoEnableOSD( cszCallID, TRUE );
			//HFONT hSysFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
			//if ( NULL != hSysFont )
			//{
			//	LOGFONTA lf;
			//	memset(&lf,0,sizeof(LOGFONTA));
			//	::GetObjectA(hSysFont,sizeof(LOGFONTA),&lf); 
			//	IMXEC_Call::SetLocalMainVideoOSDFont( cszCallID, lf );
			//}
			//IMXEC_Call::SetLocalMainVideoOSDText( cszCallID, "testosd2", VIDEC_OSD_ALIGN_TOP_LEFT );


			IMXEC_Call::SetLocalMainVideoSource(m_strCurrentCallID.c_str(),m_nDevID);
			IMXEC_Call::SetLocalSecondVideoSource(m_strCurrentCallID.c_str(),m_nDevID);
		}
	}

}

void CConferenceCallTestDlg::OnBnClickedButtonCallInfo()
{
	DlgCallInfo dlgCallInfo(this);
	dlgCallInfo.m_strCallID=m_strCurrentCallID.c_str();
	dlgCallInfo.DoModal();
}


void CConferenceCallTestDlg::OnBnClickedButtonLeft()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_TILT_LEFT,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_TILT_LEFT,0);

}

void CConferenceCallTestDlg::OnBnClickedButtonRight()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_TILT_RIGHT,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_TILT_RIGHT,0);
}

void CConferenceCallTestDlg::OnBnClickedButtonUp()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_PAN_UP,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_PAN_UP,0);
}

void CConferenceCallTestDlg::OnBnClickedButtonDown()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_PAN_DOWN,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_PAN_DOWN,0);
}


void CConferenceCallTestDlg::OnBnClickedButtonZoomIn()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_ZOOM_IN,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_ZOOM_IN,0);
}

void CConferenceCallTestDlg::OnBnClickedButtonZoomOut()
{
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_START_ZOOM_OUT,0);
	IMXEC_Call::SendPTZ(m_strCurrentCallID.c_str(),PTZ_STOP_ZOOM_OUT,0);
}

void CConferenceCallTestDlg::OnBnClickedCheckSecondVideo()
{
	m_nStartSecondVideo=m_btnSecondVideo.GetCheck();
	if (m_nStartSecondVideo!=0)
	{
		IMXEC_Call::StartSecondVideo(m_strCurrentCallID.c_str());
	}
	else
	{
		IMXEC_Call::StopSecondVideo(m_strCurrentCallID.c_str());
	}
}


void CConferenceCallTestDlg::OnBnClickedCheckAutoAnswer()
{
	m_bAutoAnswer=m_btnAutoAnswer.GetCheck();
}

/* OnIMXEC_CallRingIn
*描述：呼入事件，有终端呼叫本机的时候系统会调用此事件
*输入：	cszCallID		-呼叫标识
		cszUserID		-用户标识或用户IP
		cszUserName		-用户名称
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	if (m_strCurrentCallID.size())
	{
		if (m_strCallID2.size())
		{
			IMXEC_Call::HangupCall(cszCallID);
		}
		else
		{
			IMXEC_Call::AcceptCall(cszCallID);
			m_strCallID2=cszCallID;
		}
	}
	else
	{
		m_strLastPeerIP=cszUserID;

		m_strCurrentCallID=cszCallID;
		m_btnHangupCall.EnableWindow(TRUE);
		m_btnAcceptCall.EnableWindow(TRUE);
#ifdef HAS_H323 //#endif
		m_btnMakeCall.EnableWindow(FALSE);
#endif


		CString strStatus="";
		strStatus.Format("CallRingIn:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowTextA(strStatus);

		if (m_bAutoAnswer)
		{
			IMXEC_Call::AcceptCall(cszCallID);
		}
	}
}

/******************************************************************************
* OnIMXEC_CallRingOut
*描述：呼出事件，本机发起呼叫成功后系统会调用此事件
*输入：	cszCallID		-呼叫标识
		cszUserID		-用户标识或用户IP
		cszUserName		-用户名称
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	CString strStatus="";
	strStatus.Format("CallRingOut:%s %s",cszUserID,cszUserName);
	m_btnStatus.SetWindowTextA(strStatus);
}

/******************************************************************************
* OnIMXEC_CallEstablished
*描述：呼叫建立事件，当双方建立通话后【本机接受了远端的呼叫请求或远端接受了本机的呼叫请求】系统会调用此事件
*输入：	cszCallID		-呼叫标识
		cszUserID		-用户标识或用户IP
		cszUserName		-用户名称
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	if (m_strCurrentCallID==cszCallID)
	{
		m_btnHangupCall.EnableWindow(TRUE);
		m_btnCallInfo.EnableWindow(TRUE);
		m_btnAcceptCall.EnableWindow(FALSE);
#ifdef HAS_H323 //#endif
		m_btnMakeCall.EnableWindow(FALSE);
#endif


		m_btnSecondVideo.EnableWindow(IMXEC_Call::GetCallSupportSecondVideo(cszCallID));
		//m_btnSecondVideo.EnableWindow(TRUE);


		/*是否启动OSD*/
		IMXEC_Call::SetLocalMainVideoEnableOSD( cszCallID, TRUE );
		IMXEC_Call::SetSecondVideoEnableOSD( cszCallID, TRUE );

		HFONT hSysFont = (HFONT) ::GetStockObject(SYSTEM_FONT);
		if ( NULL != hSysFont )
		{
			LOGFONTA lf;
			memset(&lf,0,sizeof(LOGFONTA));
			::GetObjectA(hSysFont,sizeof(LOGFONTA),&lf);
			COLORREF crText=RGB(0,255,0);
			COLORREF crBk=RGB(0,0,255);
			IMXEC_Call::SetLocalMainVideoOSDFont( cszCallID, lf,crText,crBk );
			IMXEC_Call::SetSecondVideoOSDFont( cszCallID, lf ,crText,crBk );
		}
		IMXEC_Call::SetLocalMainVideoOSDText( cszCallID, "主流", VIDEC_OSD_ALIGN_TOP_LEFT );
		IMXEC_Call::SetSecondVideoOSDText( cszCallID, "辅流", VIDEC_OSD_ALIGN_TOP_LEFT );


		IMXEC_CALL_TYPE nCallType=IMXEC_PERSONAL_CALL;
		IMXEC_Call::GetCallType(nCallType);
		if (nCallType==IMXEC_PERSONAL_CALL)
		{
			if (m_pDlgVideoLocal!=NULL)
			{
				IMXEC_Call::SetLocalMainVideoWnd(cszCallID,m_pDlgVideoLocal->m_hWnd);

				CRect rc;
				m_pDlgVideoLocal->GetClientRect(rc);

				IMXEC_Call::SetLocalMainVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());
			}
		}

		if (m_pDlgVideoRemote!=NULL)
		{
			IMXEC_Call::SetRemoteMainVideoWnd(cszCallID,m_pDlgVideoRemote->m_hWnd);

			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);

			IMXEC_Call::SetRemoteMainVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());
		}

		//IMXEC_Call::StartRecordLocalMainToAVI(cszCallID,"d:\\localmain.avi",1);
		//IMXEC_Call::StartRecordRemoteMainToAVI(cszCallID,"d:\\remotemain.avi",1);
		//IMXEC_Call::StartRecordSecondToAVI(cszCallID,"d:\\second.avi",1);
	
		NETEC_Setting::SetMCUOnly(0);

		unsigned long nAudioID=0;
		IMXEC_Call::GetMainAudioID(cszCallID,nAudioID);
		unsigned long nMainVideoID=0;
		IMXEC_Call::GetMainVideoID(cszCallID,nMainVideoID);
		unsigned long nSecondVideoID=0;
		IMXEC_Call::GetSecondVideoID(cszCallID,nSecondVideoID);

		if (nCallType!=IMXEC_PERSONAL_CALL)
		{
			if (m_pDlgVideoLocal!=NULL && m_pPlayChan==NULL)
			{
				m_pPlayChan=IMXEC_PlayChan::Create(*this);
				if (m_pPlayChan!=NULL)
				{
					if (0!=m_pPlayChan->Open(m_pDlgVideoLocal->m_hWnd,nAudioID,nMainVideoID,NETEC_Node::GetNodeID(),NETEC_Node::GetNATIP(),NETEC_Node::GetNATPort(),NETEC_Node::GetLocalIP(),NETEC_Node::GetLocalPort(),NETEC_Node::GetMCUID(),NETEC_Node::GetMCUIP(),NETEC_Node::GetServerPort()))
					{
						m_pPlayChan->Close();
						delete m_pPlayChan;
						m_pPlayChan=NULL;
					}
					else
					{
						m_pPlayChan->StartAudio();
						m_pPlayChan->StartVideo();

						CRect rc;
						m_pDlgVideoLocal->GetClientRect(rc);
						m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
						m_pPlayChan->SetScreenWnd(0,1,0);
					}
				}
			}
		}

		IMXEC_Call::SetMainVideoScreenIndex(cszCallID,0);
		IMXEC_Call::SetRemoteVideoScreenWnd(cszCallID,0,0,0);

		int nTemp=IMXEC_Call::GetCallSupportSecondVideo(cszCallID);

		CString strStatus="";
		strStatus.Format("CallEstablished:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowTextA(strStatus);
	}
	else if (m_strCallID2==cszCallID)
	{
		if (m_pDlgVideoRemote2!=NULL)
		{
			IMXEC_Call::SetRemoteMainVideoWnd(cszCallID,m_pDlgVideoRemote2->m_hWnd);

			CRect rc;
			m_pDlgVideoRemote2->GetClientRect(rc);

			IMXEC_Call::SetRemoteMainVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());

			m_pDlgVideoRemote2->SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}

	if (m_pDlgVideoRemote!=NULL)
	{
		if (m_strCallID2.size())
		{
			m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,176,288,SWP_SHOWWINDOW);
		}
		else
		{
			m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,SWP_SHOWWINDOW);
		}
		if (m_strCurrentCallID.size())
		{
			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);
			IMXEC_Call::SetRemoteMainVideoWndPos(m_strCurrentCallID.c_str(),0,0,rc.Width(),rc.Height());
		}
	}
}

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
void CConferenceCallTestDlg::OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)
{
	if (m_strCurrentCallID==cszCallID)
	{
		m_strCurrentCallID="";
		m_btnHangupCall.EnableWindow(FALSE);
		m_btnAcceptCall.EnableWindow(FALSE);
		m_btnCallInfo.EnableWindow(FALSE);
		m_btnSecondVideo.EnableWindow(FALSE);
		m_btnMakeCall.EnableWindow(TRUE);

		if (m_pPlayChan!=NULL)
		{
			m_pPlayChan->Close();
			delete m_pPlayChan;
			m_pPlayChan=NULL;
		}

		CString strStatus="";
		strStatus.Format("CallDestroyed:%s %s",cszUserID,cszUserName);
		m_btnStatus.SetWindowTextA(strStatus);
	}
	else if (m_strCallID2==cszCallID)
	{
		m_strCallID2="";
		m_pDlgVideoRemote2->SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_HIDEWINDOW);
	}

	if (m_pDlgVideoRemote!=NULL)
	{
		if (m_strCallID2.size())
		{
			m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,176,288,SWP_SHOWWINDOW);
		}
		else
		{
			m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,SWP_SHOWWINDOW);
		}
		if (m_strCurrentCallID.size())
		{
			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);
			IMXEC_Call::SetRemoteMainVideoWndPos(m_strCurrentCallID.c_str(),0,0,rc.Width(),rc.Height());
		}
	}
}

/******************************************************************************
* OnIMXEC_CallRecvdPTZ
*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	TRACE("OnIMXEC_CallRecvdPTZ cszCallID=%s nPTZAction=%d nSpeed=%d\n",cszCallID,nPTZAction,nSpeed);
}

//add by Jan for DTMF [20140730]
/******************************************************************************
* OnIMXEC_CallRecvdDTMF
*描述：接收到DTMF事件，当对方发送DTMF控制信令时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
		szDtmf			-DTMF信令
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf)
{
	TRACE("CConferenceCallTestDlg::OnIMXEC_CallRecvdDTMF(%x)\n ", szDtmf);
}
//end [20140730]

/******************************************************************************
* OnIMXEC_CallSecondVideoStart
*描述：辅流开始事件，当辅流开始时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallSecondVideoStart(const char*cszCallID)
{
	IMXEC_CALL_DIRECTION nDir;
	IMXEC_Call::GetSecondVideoDirection(cszCallID,nDir);

	m_btnSecondVideo.SetCheck(nDir==IMXEC_CALL_DIR_OUT?1:0);

	if (m_pDlgSecondVideo==NULL)
	{
		m_pDlgSecondVideo=new DlgSecondVideo(this);
		m_pDlgSecondVideo->m_strCallID=cszCallID;
		m_pDlgSecondVideo->Create(DlgSecondVideo::IDD,this);
		m_pDlgSecondVideo->SetWindowPos(NULL,0,0,704,576,SWP_SHOWWINDOW|SWP_NOACTIVATE);


		if (m_pPlayChan!=NULL)
		{
			if (m_nStartSecondVideo)
			{
				m_pPlayChan->StartVideo();
			}
			else
			{
				m_pPlayChan->StartVideo();
			}
		}
	}
}

/******************************************************************************
* OnIMXEC_CallSecondVideoStop
*描述：辅流停止事件，当辅流停止时，系统会调用此事件
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnIMXEC_CallSecondVideoStop(const char*cszCallID)
{
	if (m_pDlgSecondVideo!=NULL)
	{
		m_pDlgSecondVideo->DestroyWindow();
		delete m_pDlgSecondVideo;
		m_pDlgSecondVideo=NULL;
	}

	m_nStartSecondVideo=0;
	m_btnSecondVideo.SetCheck(0);
	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->StartVideo();
	}
}


/******************************************************************************
* OnNETEC_NodeConnectStatusChanged
*描述：网络连接状态的回调
*输入：	cs					-网络连接状态
*输出： 无
*返回值：无
*****************************************************************************/
void CConferenceCallTestDlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs==NETEC_Session::CS_CONNECTED ||
		cs==NETEC_Session::CS_RESTARTED ||
		cs==NETEC_Session::CS_RECONNECTED)
	{
		//m_btnMakeCall.EnableWindow(TRUE);
		SetWindowText(GetNodeID());

		OutputDebugString(GetNodeID());
		OutputDebugString("\n");
	}
	else
	{
		//m_btnMakeCall.EnableWindow(FALSE);
	}

}

//音频数据回调函数
void CConferenceCallTestDlg::OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop)
{

}

void CConferenceCallTestDlg::OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{

}

//视频数据回调函数
//nKeyFrame,0:非关键帧 1:关键帧
void CConferenceCallTestDlg::OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode)
{

}

/******************************************************************************
* OnNETEC_NodeGetUserInfo
*描述：获取用户信息的回调
*输入：	cszUserID				-用户标识
*输出： szNodeID				-用户的网络节点标识
*返回值：成功返回0；失败返回-1，表示该用户不在线
*****************************************************************************/
int CConferenceCallTestDlg::OnNETEC_NodeGetUserInfo(const char*cszUserID,char*szNodeID)
{
	GetPrivateProfileString("USER", cszUserID , _T(""),szNodeID,128, "IMX.ini"); 	
	return 0;
}

void CConferenceCallTestDlg::OnBnClickedButtonDialpad()
{
	DlgDialPad dlgDialPad(this);
	dlgDialPad.m_strCallID=m_strCurrentCallID.c_str();
	dlgDialPad.DoModal();
}
