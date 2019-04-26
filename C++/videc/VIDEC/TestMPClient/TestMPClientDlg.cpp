
// TestMPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestMPClient.h"
#include "TestMPClientDlg.h"
#include <VIDEC/VIDEC_AVIWriter.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestMPClientDlg 对话框


static CTestMPClientDlg* g_pTestMPClientDlg = NULL;

CTestMPClientDlg::CTestMPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMPClientDlg::IDD, pParent)
	, m_nAudioCode(X_AUDIO_CODEC_G7221_14_24)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pChanDevCaptureAudio = NULL;
	m_pChanDevCapture = NULL;
	m_pIMXECPlayChan = NULL;
	m_pChanDevCapture2 = NULL;
	m_pIMXECPlayChan2 = NULL;

	m_pDlgVideoLocal=NULL;
	m_pDlgVideoRemote=NULL;
	m_pDlgVideoLocal2=NULL;
	m_pDlgVideoRemote2=NULL;
	g_pTestMPClientDlg= this;

	m_nStartSecondVideo=FALSE;
}

CTestMPClientDlg*CTestMPClientDlg::Instance()
{
	return g_pTestMPClientDlg;
}

void CTestMPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SERVER, m_editServer);
	DDX_Control(pDX, IDC_EDIT_LOCALIP, m_editLocalIP);
	DDX_Control(pDX, IDC_EDIT_PORT1, m_editPort1);
	DDX_Control(pDX, IDC_EDIT_PORT2, m_editPort2);
	DDX_Control(pDX, IDC_EDIT_SERVER2, m_editServer2);
	DDX_Control(pDX, IDC_EDIT_CLIENT_CAPTURE, m_editCapture);
	DDX_Control(pDX, IDC_EDIT_CLIENT_CAPTURE2, m_editCapture2);
	DDX_Control(pDX, IDC_EDIT_RECV2, m_editReceive);
	DDX_Control(pDX, IDC_EDIT_RECV3, m_editReceive2);
	DDX_Control(pDX, IDC_EDIT_DEVID, m_editDevID);
	DDX_Control(pDX, IDC_EDIT_DEVID1, m_editDevID1);
	DDX_Control(pDX, IDC_EDIT_DEVID3, m_editDevID2);
	DDX_Control(pDX, IDC_EDIT_DEVID4, m_editDevID4);
	DDX_Control(pDX, IDC_EDIT_DEVID2, m_editDevID3);
	//DDX_Control(pDX, IDC_EDIT_MSGTEXT, m_editMsg);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE, m_editFrameRate1);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE2, m_editFrameRate2);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE3, m_editFrameRate3);
	DDX_Control(pDX, IDC_EDIT_FRAMERATE4, m_editFrameRate4);
	DDX_Control(pDX, IDC_EDIT_BITRATE, m_editBitRate1);
	DDX_Control(pDX, IDC_EDIT_BITRATE2, m_editBitRate2);
	DDX_Control(pDX, IDC_EDIT_BITRATE3, m_editBitRate3);
	DDX_Control(pDX, IDC_EDIT_BITRATE4, m_editBitRate4);
	DDX_Control(pDX, IDC_CHECK_ENCODE, m_btnEncode);
	DDX_Control(pDX, IDC_CHECK_AUTOSIZE, m_btnAutoSize);
	DDX_Control(pDX, IDC_CHECK_DENOISE, m_btnDenoise);
	DDX_Control(pDX, IDC_COMBO_AUDIO_CODE, m_comboxAudioCode);
	DDX_Control(pDX, IDC_COMBO_NETTYPE1, m_comboxNetType1);
	DDX_Control(pDX, IDC_COMBO_NETTYPE2, m_comboxNetType2);
	DDX_Control(pDX, IDC_COMBO_H323NETTYPE1, m_comboxH323NetType1);
	DDX_Control(pDX, IDC_COMBO_H323NETTYPE2, m_comboxH323NetType2);
	DDX_Control(pDX, IDC_COMBO_H323SIZE1, m_comboxH323Size1);
	DDX_Control(pDX, IDC_COMBO_H323SIZE2, m_comboxH323Size2);
	DDX_Control(pDX, IDC_COMBO_FRAMETYPE2, m_comboxFrameType2);
	DDX_Control(pDX, IDC_COMBO_FRAMETYPE1, m_comboxFrameType1);
	DDX_Control(pDX, IDC_LIST1, m_listMsg);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth1);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight1);
	DDX_Control(pDX, IDC_EDIT_WIDTH2, m_editWidth2);
	DDX_Control(pDX, IDC_EDIT_HEIGHT2, m_editHeight2);
}

BEGIN_MESSAGE_MAP(CTestMPClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_NETWORK, &CTestMPClientDlg::OnBnClickedButtonNetwork)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE, &CTestMPClientDlg::OnBnClickedButtonClientCapture)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_STOP, &CTestMPClientDlg::OnBnClickedButtonClientCaptureStop)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE, &CTestMPClientDlg::OnBnClickedButtonClientReceive)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE_STOP, &CTestMPClientDlg::OnBnClickedButtonClientReceiveStop)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE_Record, &CTestMPClientDlg::OnBnClickedButtonClientReceiveRecord)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE_STOP_Record, &CTestMPClientDlg::OnBnClickedButtonClientReceiveStopRecord)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_SEND_AUDIO_SEND, &CTestMPClientDlg::OnBnClickedButtonClientCaptureSendAudioSend)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_SEND_AUDIO_CLOSE, &CTestMPClientDlg::OnBnClickedButtonClientCaptureSendAudioClose)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_SEND_VIDEO_START, &CTestMPClientDlg::OnBnClickedButtonClientCaptureSendVideoStart)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_SEND_VIDEO_CLOSE, &CTestMPClientDlg::OnBnClickedButtonClientCaptureSendVideoClose)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_CODE, &CTestMPClientDlg::OnCbnSelchangeComboAudioCode)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPEN_MIC, &CTestMPClientDlg::OnBnClickedButtonClientOpenMic)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CLOSE_MIC, &CTestMPClientDlg::OnBnClickedButtonClientCloseMic)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE2, &CTestMPClientDlg::OnBnClickedButtonClientCapture2)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_CAPTURE_STOP2, &CTestMPClientDlg::OnBnClickedButtonClientCaptureStop2)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE2, &CTestMPClientDlg::OnBnClickedButtonClientReceive2)
	ON_BN_CLICKED(IDC_BUTTON_CLIENT_RECEIVE_STOP2, &CTestMPClientDlg::OnBnClickedButtonClientReceiveStop2)
	ON_BN_CLICKED(IDC_BUTTON_H323CAPTURE, &CTestMPClientDlg::OnBnClickedButtonH323capture)
	ON_BN_CLICKED(IDC_BUTTON_H323STOP, &CTestMPClientDlg::OnBnClickedButtonH323stop)
	ON_BN_CLICKED(IDC_BUTTON_H323CALL, &CTestMPClientDlg::OnBnClickedButtonH323call)
	ON_BN_CLICKED(IDC_BUTTON_H323HANGUP, &CTestMPClientDlg::OnBnClickedButtonH323hangup)
	ON_BN_CLICKED(IDC_BUTTON_H323ACCEPT, &CTestMPClientDlg::OnBnClickedButtonH323accept)
	ON_BN_CLICKED(IDC_BUTTON_H323SECOND, &CTestMPClientDlg::OnBnClickedButtonH323second)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_BMP1, &CTestMPClientDlg::OnBnClickedButtonCaptureBmp1)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_BMP2, &CTestMPClientDlg::OnBnClickedButtonCaptureBmp2)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAMETYPE1, &CTestMPClientDlg::OnCbnSelchangeComboFrametype1)
	ON_CBN_SELCHANGE(IDC_COMBO_FRAMETYPE2, &CTestMPClientDlg::OnCbnSelchangeComboFrametype2)
	ON_BN_CLICKED(IDC_BUTTON_VIDEOPOLL1, &CTestMPClientDlg::OnBnClickedButtonVideopoll1)
	ON_BN_CLICKED(IDC_BUTTON_VIDEOPOLL2, &CTestMPClientDlg::OnBnClickedButtonVideopoll2)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_RECORD, &CTestMPClientDlg::OnBnClickedButtonRecvRecord)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_RECORD2, &CTestMPClientDlg::OnBnClickedButtonRecvRecord2)
END_MESSAGE_MAP()


// CTestMPClientDlg 消息处理程序

BOOL CTestMPClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_bStartRecordToAVI1=false;
	m_bStartRecordToAVI2=false;

	m_bVideopoll1 = false;
	m_bVideopoll2 = false;
	m_nVideopoll1_Status=FRAME_TYPE_CONTROL_MODE_MAIN;
	m_nVideopoll2_Status=FRAME_TYPE_CONTROL_MODE_MAIN;

	CWnd* pDeskTopWnd = GetDesktopWindow();//FromHandle(HWND_DESKTOP);
	m_pDlgVideoLocal=new DlgVideo(0,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal->SetWindowPos(NULL,0,0,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoLocal->SetWindowText(_T("Capture1"));

	m_pDlgVideoRemote=new DlgVideo(1,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote->SetWindowPos(NULL,352,0,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoRemote->SetWindowText(_T("Player1"));

	m_pDlgVideoLocal2=new DlgVideo(2,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal2->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal2->SetWindowPos(NULL,0,288,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoLocal2->SetWindowText(_T("Capture2"));

	m_pDlgVideoRemote2=new DlgVideo(3,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote2->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote2->SetWindowPos(NULL,352,288,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoRemote2->SetWindowText(_T("Player2"));

	m_pDlgVideoLocal3=new DlgVideo(4,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal3->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal3->SetWindowPos(NULL,0,576,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoLocal3->SetWindowText(_T("H323 Local"));

	m_pDlgVideoRemote3=new DlgVideo(5,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote3->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote3->SetWindowPos(NULL,352,576,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoRemote3->SetWindowText(_T("H323 Remote"));

	m_pDlgVideoRemote4=new DlgVideo(5,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote4->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote4->SetWindowPos(NULL,704,576,352,288,SWP_SHOWWINDOW);
	m_pDlgVideoRemote4->SetWindowText(_T("H323 Second"));

	SetWindowPos(NULL,704,0,0,0,SWP_NOSIZE);
	NETEC_Core::Start();

	m_editWidth1.SetWindowText(_T("1920"));
	m_editHeight1.SetWindowText(_T("1080"));
	m_editWidth2.SetWindowText(_T("1920"));
	m_editHeight2.SetWindowText(_T("1080"));

	m_editServer.SetWindowText(_T("192.168.202.39"));//180.150.178.79  10.200.192.17//192.168.16.133//10.200.192.17
	m_editServer2.SetWindowText(_T("192.168.16.16"));

	//m_editReceive.SetWindowText(_T("545708371,545708371,0-5026226D-9BB90CF,MCU-001,192.168.11.11"));
	


	//m_editReceive.SetWindowText(_T("1504485726,1504485727,0-371E8A1F-A866BC0E@xf,MCU-001@xf,192.168.11.126"));

//	m_editServer.SetWindowText(_T("www.avms.com.cn"));

	//SetTimer(1,1000,NULL);

	int nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_GSM"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_GSM);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G719_32"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G719_32);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_OPUS_FWB_48"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_OPUS_FWB_48);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G7221_14_24"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G7221_14_24);
	m_comboxAudioCode.SetCurSel(nIndex);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G7221_14_48"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G7221_14_48);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G729"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G729);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G711U"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G711U);

	nIndex = m_comboxAudioCode.AddString(_T("X_AUDIO_CODEC_G711A"));
	m_comboxAudioCode.SetItemData(nIndex,(DWORD_PTR)X_AUDIO_CODEC_G711A);

	nIndex = m_comboxNetType1.AddString(_T("互联网"));
	m_comboxNetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_INTERNET);
	m_comboxNetType1.SetCurSel(nIndex);
	nIndex = m_comboxNetType1.AddString(_T("局域网"));
	m_comboxNetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_LAN);
	nIndex = m_comboxNetType1.AddString(_T("专网"));
	m_comboxNetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_PRIVATE);
	nIndex = m_comboxNetType1.AddString(_T("卫星网"));
	m_comboxNetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_SATELLITE);
	nIndex = m_comboxNetType1.AddString(_T("3/4G"));
	m_comboxNetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_3G);

	nIndex = m_comboxNetType2.AddString(_T("互联网"));
	m_comboxNetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_INTERNET);
	m_comboxNetType2.SetCurSel(nIndex);
	nIndex = m_comboxNetType2.AddString(_T("局域网"));
	m_comboxNetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_LAN);
	nIndex = m_comboxNetType2.AddString(_T("专网"));
	m_comboxNetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_PRIVATE);
	nIndex = m_comboxNetType2.AddString(_T("卫星网"));
	m_comboxNetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_SATELLITE);
	nIndex = m_comboxNetType2.AddString(_T("3/4G"));
	m_comboxNetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_3G);

	nIndex = m_comboxH323NetType1.AddString(_T("互联网"));
	m_comboxH323NetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_INTERNET);
	m_comboxH323NetType1.SetCurSel(nIndex);
	nIndex = m_comboxH323NetType1.AddString(_T("局域网"));
	m_comboxH323NetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_LAN);
	nIndex = m_comboxH323NetType1.AddString(_T("专网"));
	m_comboxH323NetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_PRIVATE);
	nIndex = m_comboxH323NetType1.AddString(_T("卫星网"));
	m_comboxH323NetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_SATELLITE);
	nIndex = m_comboxH323NetType1.AddString(_T("3/4G"));
	m_comboxH323NetType1.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_3G);
	
	nIndex = m_comboxH323NetType2.AddString(_T("互联网"));
	m_comboxH323NetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_INTERNET);
	m_comboxH323NetType2.SetCurSel(nIndex);
	nIndex = m_comboxH323NetType2.AddString(_T("局域网"));
	m_comboxH323NetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_LAN);
	nIndex = m_comboxH323NetType2.AddString(_T("专网"));
	m_comboxH323NetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_PRIVATE);
	nIndex = m_comboxH323NetType2.AddString(_T("卫星网"));
	m_comboxH323NetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_SATELLITE);
	nIndex = m_comboxH323NetType2.AddString(_T("3/4G"));
	m_comboxH323NetType2.SetItemData(nIndex,(DWORD_PTR)VIDEC_NETWORK_3G);	

	nIndex = m_comboxH323Size1.AddString(_T("QCIF"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_QCIF);
	nIndex = m_comboxH323Size1.AddString(_T("CIF"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_CIF);
	nIndex = m_comboxH323Size1.AddString(_T("4CIF"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_4CIF);
	nIndex = m_comboxH323Size1.AddString(_T("16CIF"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_16CIF);
	nIndex = m_comboxH323Size1.AddString(_T("720I"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_720I);
	nIndex = m_comboxH323Size1.AddString(_T("720P"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_720P);
	nIndex = m_comboxH323Size1.AddString(_T("1080I"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_1080I);
	nIndex = m_comboxH323Size1.AddString(_T("1080P"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_1080P);
	m_comboxH323Size1.SetCurSel(nIndex);
	nIndex = m_comboxH323Size1.AddString(_T("QVGA"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_QVGA);
	nIndex = m_comboxH323Size1.AddString(_T("VGA"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_VGA);
	nIndex = m_comboxH323Size1.AddString(_T("SVGA"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_SVGA);
	nIndex = m_comboxH323Size1.AddString(_T("XGA"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_XGA);
	nIndex = m_comboxH323Size1.AddString(_T("SXGA"));
	m_comboxH323Size1.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_SXGA);

	nIndex = m_comboxH323Size2.AddString(_T("QCIF"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_QCIF);
	nIndex = m_comboxH323Size2.AddString(_T("CIF"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_CIF);
	nIndex = m_comboxH323Size2.AddString(_T("4CIF"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_4CIF);
	nIndex = m_comboxH323Size2.AddString(_T("16CIF"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_16CIF);
	nIndex = m_comboxH323Size2.AddString(_T("720I"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_720I);
	nIndex = m_comboxH323Size2.AddString(_T("720P"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_720P);
	nIndex = m_comboxH323Size2.AddString(_T("1080I"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_1080I);
	nIndex = m_comboxH323Size2.AddString(_T("1080P"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_1080P);
	m_comboxH323Size2.SetCurSel(nIndex);
	nIndex = m_comboxH323Size2.AddString(_T("QVGA"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_QVGA);
	nIndex = m_comboxH323Size2.AddString(_T("VGA"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_VGA);
	nIndex = m_comboxH323Size2.AddString(_T("SVGA"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_SVGA);
	nIndex = m_comboxH323Size2.AddString(_T("XGA"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_XGA);
	nIndex = m_comboxH323Size2.AddString(_T("SXGA"));
	m_comboxH323Size2.SetItemData(nIndex,(DWORD_PTR)VIDEO_FORMAT_SXGA);

	nIndex = m_comboxFrameType1.AddString(_T("高码流"));
	m_comboxFrameType1.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_MAIN);
	m_comboxFrameType1.SetCurSel(nIndex);
	nIndex = m_comboxFrameType1.AddString(_T("中码流"));
	m_comboxFrameType1.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_SUB);
	nIndex = m_comboxFrameType1.AddString(_T("底码流"));
	m_comboxFrameType1.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_QSUB);

	nIndex = m_comboxFrameType2.AddString(_T("高码流"));
	m_comboxFrameType2.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_MAIN);
	m_comboxFrameType2.SetCurSel(nIndex);
	nIndex = m_comboxFrameType2.AddString(_T("中码流"));
	m_comboxFrameType2.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_SUB);
	nIndex = m_comboxFrameType2.AddString(_T("底码流"));
	m_comboxFrameType2.SetItemData(nIndex,(DWORD_PTR)FRAME_TYPE_CONTROL_MODE_QSUB);

	//CheckNew();

	m_btnEncode.SetCheck(FALSE);	
	m_btnDenoise.SetCheck(FALSE);	
	m_btnAutoSize.SetCheck(FALSE);	

	m_editDevID1.SetWindowText(_T("0"));
	m_editDevID2.SetWindowText(_T("1"));
	m_editBitRate1.SetWindowText(_T("2048"));
	m_editBitRate2.SetWindowText(_T("2048"));
	m_editFrameRate1.SetWindowText(_T("30"));
	m_editFrameRate2.SetWindowText(_T("30"));	
	SetTimer(1000,1200,NULL);

	m_editDevID3.SetWindowText(_T("2"));
	m_editDevID4.SetWindowText(_T("3"));
	m_editBitRate3.SetWindowText(_T("2048"));
	m_editFrameRate3.SetWindowText(_T("60"));
	m_editBitRate4.SetWindowText(_T("1024"));
	m_editFrameRate4.SetWindowText(_T("30"));
	m_editPort1.SetWindowText(_T("1720"));
	m_editPort2.SetWindowText(_T("12000"));

	SetWindowText(_T("1:SoftWare; 2:Intel;1:DShow;2:D3D9;3:D3D11"));

	IMXEC_H323::SetSupportH239(1);

	IMXEC_Call::SetCallType(IMXEC_PERSONAL_CALL);
	IMXEC_Call::Start();

	IMXEC_IMX::SetUserID("1001");
	IMXEC_IMX::SetUserName("1001");
	IMXEC_IMX::Start();

	OnBnClickedButtonH323capture();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestMPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestMPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestMPClientDlg::OnBnClickedButtonNetwork()
{
	UpdateData(FALSE);

	CString strText;
	m_editServer.GetWindowText(strText);
	std::string strIP = CW2A(strText);

	NETEC_Node::SetServerIP(strIP.c_str());
	NETEC_Node::SetServerPort(4222);
	NETEC_Node::Start();
	NETEC_Setting::SetMCUOnly(1);
}


bool CTestMPClientDlg::CheckNew(void)
{
	UpdateData(TRUE);

	if (m_csNet != NETEC_Session::CS_CONNECTED)
	{
		OnBnClickedButtonNetwork();

	//	AfxMessageBox(_T("正在连接网络...."));

		if (m_csNet != NETEC_Session::CS_CONNECTED)
		{
			return false;
		}
	}
	return true;
}

void CTestMPClientDlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	
	m_editLocalIP.SetWindowText(CA2W(NETEC_Node::GetLocalIP()));

	m_csNet = cs;

	TRACE("OnNETEC_NodeConnectStatusChanged\n");
}

//---------------------------------------------------------------------------------------
// 创建SQL语句
//std::string CreateMySQL(const char* cszSQL,...)
//{
//	char szSQL[65535]={0};
//	va_list args;
//	va_start(args,cszSQL);
//#ifdef WIN32
//	_vsnprintf(szSQL,sizeof(szSQL),cszSQL,args);
//#else
//	vsnprintf(szSQL,sizeof(szSQL),cszSQL,args);
//#endif
//	va_end(args);
//	return szSQL;
//}

void CTestMPClientDlg::OnBnClickedButtonClientCapture()
{
	UpdateData(TRUE);

	if (!CheckNew())
	{
		SetTimer(1001,1000,NULL);
		return;
	}

	OnBnClickedButtonClientCaptureStop();

	IMXEC_CapChanDev::SetVideoAutoFormat(m_btnAutoSize.GetCheck());

	if (m_pChanDevCapture == NULL)
	{
		m_pChanDevCapture = IMXEC_CapChanDev::Create(*this);
	}
	if (m_pChanDevCapture)
	{
		m_pChanDevCapture->Open(m_pDlgVideoLocal->GetSafeHwnd(),0);


		CString	Filter = _T("Media Files|*.dat;*.avi;*.mp3;*.mpeg;*.mpg;*.mov;*.vob;*.asf;*.wmv;*.wm;*.wma;*.rm;*.rmvb;*.flv;*.mp4;*.f4v;*.mts;*.mkv;*.avms||");

		//CFileDialog dlgFile(TRUE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST,Filter,this,0, FALSE);		
		//if(dlgFile.DoModal() == IDOK)
		{
			//CString strFilePath = dlgFile.GetPathName();

			CString strText;
			m_editDevID1.GetWindowText(strText);
			std::string strDevID = CW2A(strText);
			m_editFrameRate1.GetWindowText(strText);
			std::string strFrameRate = CW2A(strText);
			m_editBitRate1.GetWindowText(strText);
			std::string strBitRate = CW2A(strText);

			m_editWidth1.GetWindowText(strText);
			std::string strWidth = CW2A(strText);

			m_editHeight1.GetWindowText(strText);
			std::string strHeight = CW2A(strText);

			int nIndex = m_comboxNetType1.GetCurSel();
			int nNetType = m_comboxNetType1.GetItemData(nIndex);

			//int nResult = m_pChanDevCapture->OpenVideoDev(KW2A(strFilePath).c_str(), 1, 512,0, VIDEC_CODEC_H264_SVC, 1, 1);
			int nResult = m_pChanDevCapture->OpenVideoDev(atoi(strDevID.c_str()), atoi(strWidth.c_str()), atoi(strHeight.c_str()), atoi(strFrameRate.c_str()), atoi(strBitRate.c_str()), nNetType, VIDEC_CODEC_H264_SVC, 1, 1);

			m_pChanDevCapture->EnableOSD(true);
			m_pChanDevCapture->SetOSDText("内蒙古消防总队");
			//m_pChanDevCapture->SetWindowPos(NULL,0,0,1920,1080,SWP_SHOWWINDOW);
			CRect rc;
			m_pDlgVideoLocal->GetClientRect(rc);
			m_pChanDevCapture->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
			m_pChanDevCapture->SetForceEncodeVideo(m_btnEncode.GetCheck());
			m_pChanDevCapture->SetDenoise(m_btnDenoise.GetCheck());
			//m_pChanDevCapture->SetVolume(0);

			unsigned long ulAudioID = m_pChanDevCapture->GetAudioID();
			if (m_pChanDevCaptureAudio)
			{
				ulAudioID = m_pChanDevCaptureAudio->GetAudioID();
			}

			//CString strText;
			strText.Format(_T("%u,%u,%s,%s,%s"),ulAudioID,m_pChanDevCapture->GetVideoID(),KA2W(NETEC_Node::GetNodeID()),KA2W(NETEC_Node::GetMCUID()),KA2W(NETEC_Node::GetMCUIP()));

			m_editCapture.SetWindowText(strText);

			m_editReceive.SetWindowText(strText);


			//m_editDevID.EnableWindow(FALSE);
			//m_editDevID.GetWindowText(strText);
			//if (!strText.IsEmpty())
			//{
			//	std::string strDevID = CW2A(strText);

			//	m_editServer.GetWindowText(strText);
			//	std::string strIP = CW2A(strText);
			//	if (m_DBDriver.CheckDB())
			//	{
			//		m_DBDriver.Disconnect();
			//	}

			//	//连接数据库
			//	MYSQL_CONNECTION MyConn;
			//	MyConn.dbhost = strIP.c_str();
			//	MyConn.dbport = 3306;
			//	MyConn.dbname = "avmcdb";
			//	MyConn.dbuser = "avcon";
			//	MyConn.dbpass = "avcon*20130420*";
			//	if(m_DBDriver.Connect(MyConn) && m_DBDriver.CheckDB())
			//	{
			//		std::string sql;	//修改设备表中状态
			//		sql=CreateMySQL("update `mon_dev` set `svrid`='%s',`svraddr`='%s',`svrport`=4222,`nodeid`='%s',`status`=1,`logintime`=now() where `devid`='%s'",NETEC_Node::GetMCUID(),NETEC_Node::GetMCUIP(),NETEC_Node::GetNodeID(),strDevID.c_str());
			//		m_DBDriver.SQLExecute(sql);


			//		sql=CreateMySQL("update `mon_channel` set `status`=1,`videocid`=%u,`audiocid`=%u where `devid`='%s' and `channelno`=0",m_pChanDevCapture->GetVideoID(),m_pChanDevCapture->GetAudioID(),strDevID.c_str());
			//		m_DBDriver.SQLExecute(sql);
			//	}
			//}
		}
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureStop()
{
	KillTimer(1001);
	KillTimer(1002);
	if (m_pChanDevCapture)
	{
		m_pChanDevCapture->CloseVideoDev();
		m_pChanDevCapture->CloseAudio();
		m_pChanDevCapture->Close();
		delete m_pChanDevCapture;
		m_pChanDevCapture = NULL;
	}
	//m_editDevID.EnableWindow();

	//CString strText;
	//m_editDevID.GetWindowText(strText);
	//if (!strText.IsEmpty() && m_DBDriver.CheckDB())
	//{
	//	std::string strDevID = CW2A(strText);
	//	std::string sql;	
	//	sql=CreateMySQL("update `mon_dev` set `nodeid`='',`nataddr`='',`localaddr`='',`localport`=0,`termtype`=0,`termname`='',`status`=0,`mgsid`='',`logouttime`=now() where `devid`='%s'", strDevID.c_str());
	//	m_DBDriver.SQLExecute(sql);
	//	sql=CreateMySQL( "update `mon_channel` set `videocid`=0,`audiocid`=0,`status`=0 where `devid`='%s'",strDevID.c_str());
	//	m_DBDriver.SQLExecute(sql);
	//	m_DBDriver.Disconnect();
	//}
}

void CTestMPClientDlg::OnBnClickedButtonClientReceive()
{
	if (!CheckNew())
	{
		return;
	}

	OnBnClickedButtonClientReceiveStop();

	if (m_pIMXECPlayChan == NULL)
	{
		m_pIMXECPlayChan = IMXEC_PlayChan::Create(*this);
	}
	if (m_pIMXECPlayChan)
	{
		unsigned long nAudChanID=0;
		unsigned long nVidChanID=0;
		std::string strNoidID;
		std::string strMCUID;
		std::string strMCUIP;

		CString strText;
		m_editReceive.GetWindowText(strText);

		CString strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		nAudChanID = strtoul(KW2A(strTemp).c_str(),NULL,10);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		nVidChanID = strtoul(KW2A(strTemp).c_str(),NULL,10);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		strNoidID = KW2A(strTemp);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		strMCUID = KW2A(strTemp);

		strMCUIP = KW2A(strText);


		if (nAudChanID == 0 || nVidChanID == 0 || 0!=m_pIMXECPlayChan->Open(m_pDlgVideoRemote->GetSafeHwnd(),nAudChanID,nVidChanID,strNoidID.c_str(),"",0,"",0,strMCUID.c_str(),strMCUIP.c_str(),4222,1))
		{
			m_pIMXECPlayChan->Close();
			delete m_pIMXECPlayChan;
			m_pIMXECPlayChan=NULL;
		}
		
		if (m_pIMXECPlayChan)
		{
			if (m_bVideopoll1)
			{
				m_pIMXECPlayChan->SetVideoPolledMode();
			}

			m_pIMXECPlayChan->EnableAudio(1);
			m_pIMXECPlayChan->StartAudio();
			m_pIMXECPlayChan->EnableVideo(1);
			m_pIMXECPlayChan->StartVideo();	

			int nIndex = m_comboxFrameType1.GetCurSel();
			int nType = m_comboxFrameType1.GetItemData(nIndex);
			m_pIMXECPlayChan->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)nType);
			m_pIMXECPlayChan->RequestKeyFrame();
			//m_pIMXECPlayChan->SetVideoSkipFrame(1,1);
			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);
			m_pIMXECPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

			//m_pIMXECPlayChan->SetWindowPos(NULL,445,210,120,90,SWP_SHOWWINDOW);
		}

		//m_editReceive.SetWindowText(_T(""));
	}

}

void CTestMPClientDlg::OnBnClickedButtonClientReceiveStop()
{
	if (m_pIMXECPlayChan)
	{
		m_pIMXECPlayChan->Close();
		delete m_pIMXECPlayChan;
		m_pIMXECPlayChan = NULL;
	}

	m_bStartRecordToAVI1=false;

}

BOOL CTestMPClientDlg::DestroyWindow()
{	
	//KillTimer(1000);
	OnBnClickedButtonH323hangup();

	OnBnClickedButtonClientCloseMic();

	OnBnClickedButtonClientCaptureStop2();
	OnBnClickedButtonClientCaptureStop();
	OnBnClickedButtonClientReceiveStop();
	OnBnClickedButtonClientReceiveStop2();

	IMXEC_H323::Stop();
	IMXEC_IMX::Stop();
	IMXEC_Call::Stop();

	NETEC_Node::Stop();
	NETEC_Core::Stop();

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
	if (m_pDlgVideoLocal2!=NULL)
	{
		m_pDlgVideoLocal2->DestroyWindow();
		delete m_pDlgVideoLocal2;
		m_pDlgVideoLocal2=NULL;
	}

	if (m_pDlgVideoRemote2!=NULL)
	{
		m_pDlgVideoRemote2->DestroyWindow();
		delete m_pDlgVideoRemote2;
		m_pDlgVideoRemote2=NULL;
	}

	if (m_pDlgVideoLocal3!=NULL)
	{
		m_pDlgVideoLocal3->DestroyWindow();
		delete m_pDlgVideoLocal3;
		m_pDlgVideoLocal3=NULL;
	}

	if (m_pDlgVideoRemote3!=NULL)
	{
		m_pDlgVideoRemote3->DestroyWindow();
		delete m_pDlgVideoRemote3;
		m_pDlgVideoRemote3=NULL;
	}


	if (m_pDlgVideoRemote4!=NULL)
	{
		m_pDlgVideoRemote4->DestroyWindow();
		delete m_pDlgVideoRemote4;
		m_pDlgVideoRemote4=NULL;
	}

	return CDialog::DestroyWindow();
}

void CTestMPClientDlg::OnIMXEC_CapChanCallbackFilePositionChanged(unsigned long ulSeconds)
{
	printf("%u\n",ulSeconds);
}
void CTestMPClientDlg::OnBnClickedButtonClientReceiveRecord()
{
	if (m_pIMXECPlayChan)
	{
		m_pIMXECPlayChan->StartRecordToAVI("d:/new.mp4",2);
		m_pIMXECPlayChan->RequestKeyFrame();
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientReceiveStopRecord()
{
	if (m_pIMXECPlayChan)
	{
		m_pIMXECPlayChan->StopRecordToAVI();
	}
}

void CTestMPClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		KillTimer(1);
		OnBnClickedButtonNetwork();		
		//OnBnClickedButtonClientReceive();

		SetTimer(2,2000,NULL);
	}
	else if (nIDEvent == 1001)
	{
		KillTimer(1001);
		OnBnClickedButtonClientCapture();
	}
	else if (nIDEvent == 1002)
	{
		KillTimer(1002);
		OnBnClickedButtonClientCapture2();
	}
	else if (nIDEvent == 2)
	{
		KillTimer(2);
		//OnBnClickedButtonClientReceiveRecord();
	}
	else if (nIDEvent == 100)
	{
		KillTimer(100);
		ShellExecute(NULL,_T("open"),_T("d:/demo.bmp"),NULL,NULL,SW_SHOW);
	}
	else if (nIDEvent == 3000)
	{
		if (m_pIMXECPlayChan)
		{
			switch(m_nVideopoll1_Status)
			{
			case FRAME_TYPE_CONTROL_MODE_MAIN:
				m_nVideopoll1_Status = FRAME_TYPE_CONTROL_MODE_SUB;
				break;
			case FRAME_TYPE_CONTROL_MODE_SUB:
				m_nVideopoll1_Status = FRAME_TYPE_CONTROL_MODE_QSUB;
				break;
			case FRAME_TYPE_CONTROL_MODE_QSUB:
				m_nVideopoll1_Status = FRAME_TYPE_CONTROL_MODE_MAIN;
				break;
			};
			m_pIMXECPlayChan->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)m_nVideopoll1_Status);
			m_pIMXECPlayChan->RequestKeyFrame();
		}
	}
	else if (nIDEvent == 3001)
	{
		if (m_pIMXECPlayChan2)
		{
			switch(m_nVideopoll2_Status)
			{
			case FRAME_TYPE_CONTROL_MODE_MAIN:
				m_nVideopoll2_Status = FRAME_TYPE_CONTROL_MODE_SUB;
				break;
			case FRAME_TYPE_CONTROL_MODE_SUB:
				m_nVideopoll2_Status = FRAME_TYPE_CONTROL_MODE_QSUB;
				break;
			case FRAME_TYPE_CONTROL_MODE_QSUB:
				m_nVideopoll2_Status = FRAME_TYPE_CONTROL_MODE_MAIN;
				break;
			};
			m_pIMXECPlayChan2->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)m_nVideopoll2_Status);
			m_pIMXECPlayChan2->RequestKeyFrame();
		}
	}
	else if (nIDEvent == 1000)
	{
		if (m_pIMXECPlayChan)
		{
			//CString strText;
			unsigned long ulTotalPackets;unsigned long ulLostPackets;double AvgPacketLossRate;double CurPacketLossRate;int nBitrate;
			m_pIMXECPlayChan->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);

			//strText.Format(_T("Player1 Show=%d Recv=%d Decode=%d ShowType=%d %d X %d bps=%d"),m_pIMXECPlayChan->GetVideoShowFrameRate(),m_pIMXECPlayChan->GetVideoRecvFrameRate(),m_pIMXECPlayChan->GetDecoderType(),m_pIMXECPlayChan->GetVideoShowType(),m_pIMXECPlayChan->GetVideoWidth(),m_pIMXECPlayChan->GetVideoHeight(),nBitrate);
			//m_pDlgVideoRemote->SetWindowText(strText);
		}
		if (m_pChanDevCapture)
		{
			unsigned long ulTotalPackets;unsigned long ulLostPackets;double AvgPacketLossRate;double CurPacketLossRate;int nBitrate;
			m_pChanDevCapture->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
			//CString strText;
			//strText.Format(_T("Capture1 Show=%d Send=%d Encode=%d ShowType=%d %d X %d bps=%d"),m_pChanDevCapture->GetVideoShowFrameRate(),m_pChanDevCapture->GetVideoSendFrameRate(),m_pChanDevCapture->GetEncoderType(),m_pChanDevCapture->GetVideoShowType(),m_pChanDevCapture->GetVideoWidth(),m_pChanDevCapture->GetVideoHeight(),nBitrate);
			//m_pDlgVideoLocal->SetWindowText(strText);
		}

		if (m_pIMXECPlayChan2)
		{
			unsigned long ulTotalPackets;unsigned long ulLostPackets;double AvgPacketLossRate;double CurPacketLossRate;int nBitrate;
			m_pIMXECPlayChan2->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
			CString strText;
			strText.Format(_T("Player2 Show=%d Recv=%d Decode=%d ShowType=%d %d X %d bps=%d"),m_pIMXECPlayChan2->GetVideoShowFrameRate(),m_pIMXECPlayChan2->GetVideoRecvFrameRate(),m_pIMXECPlayChan2->GetDecoderType(),m_pIMXECPlayChan2->GetVideoShowType(),m_pIMXECPlayChan2->GetVideoWidth(),m_pIMXECPlayChan2->GetVideoHeight(),nBitrate);
			m_pDlgVideoRemote2->SetWindowText(strText);
		}
		if (m_pChanDevCapture2)
		{
			unsigned long ulTotalPackets;unsigned long ulLostPackets;double AvgPacketLossRate;double CurPacketLossRate;int nBitrate;
			m_pChanDevCapture2->GetVideoPacketStatistics(ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
			CString strText;
			strText.Format(_T("Capture2 Show=%d Send=%d Encode=%d ShowType=%d %d X %d bps=%d"),m_pChanDevCapture2->GetVideoShowFrameRate(),m_pChanDevCapture2->GetVideoSendFrameRate(),m_pChanDevCapture2->GetEncoderType(),m_pChanDevCapture2->GetVideoShowType(),m_pChanDevCapture2->GetVideoWidth(),m_pChanDevCapture2->GetVideoHeight(),nBitrate);
			m_pDlgVideoLocal2->SetWindowText(strText);
		}

		if (m_strCurrentCallID.size() > 0 && m_pDlgVideoRemote3)
		{
			int nWidth;int nHeight;VIDEC_CODEC_TYPE nVideoCodecType;HWND hVideoWnd;CallChannelInfo  chInfo;
			IMXEC_Call::GetRemoteMainVideoInfo(m_strCurrentCallID.c_str(), nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);

			unsigned long ulTotalPackets;unsigned long ulLostPackets;double AvgPacketLossRate;double CurPacketLossRate;int nBitrate;
			IMXEC_Call::GetRemoteMainVideoPacketStatistics(m_strCurrentCallID.c_str(),ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate ,nBitrate);
			CString strText;
			strText.Format(_T("H323 Remote %d X %d FrameRate=%d bps=%d"),nWidth,nHeight,chInfo.m_nFrame,nBitrate);
			m_pDlgVideoRemote3->SetWindowText(strText);

			IMXEC_Call::GetSecondVideoInfo(m_strCurrentCallID.c_str(), nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
			IMXEC_Call::GetSecondVideoPacketStatistics(m_strCurrentCallID.c_str(),ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate ,nBitrate);
			strText.Format(_T("H323 Second %d X %d FrameRate=%d bps=%d"),nWidth,nHeight,chInfo.m_nFrame,nBitrate);
			m_pDlgVideoRemote4->SetWindowText(strText);

			IMXEC_Call::GetLocalMainVideoInfo(m_strCurrentCallID.c_str(), nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
			IMXEC_Call::GetLocalMainVideoPacketStatistics(m_strCurrentCallID.c_str(),ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate ,nBitrate);
			strText.Format(_T("H323 Local Show=%d Send=%d Encode=%d ShowType=%d %d X %d bps=%d"),0,0,0,0,nWidth,nHeight,nBitrate);
			strText.Format(_T("H323 Local %d X %d FrameRate=%d bps=%d"),nWidth,nHeight,chInfo.m_nFrame,nBitrate);
			m_pDlgVideoLocal3->SetWindowText(strText);
		}

	}

	__super::OnTimer(nIDEvent);
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureSendAudioSend()
{
	IMXEC_CapChanDev::EnableAudioSend(true);
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureSendAudioClose()
{
	IMXEC_CapChanDev::EnableAudioSend(false);	
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureSendVideoStart()
{
	IMXEC_CapChanDev::EnableVideoSend(true);
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureSendVideoClose()
{
	IMXEC_CapChanDev::EnableVideoSend(false);
}

void CTestMPClientDlg::OnCbnSelchangeComboAudioCode()
{
	int nIndex = m_comboxAudioCode.GetCurSel();
	m_nAudioCode = m_comboxAudioCode.GetItemData(nIndex);
}

void CTestMPClientDlg::OnBnClickedButtonClientOpenMic()
{
	OnBnClickedButtonClientCloseMic();

 	AUDEC_Engine::AUDEC_Init();
 	int n = AUDEC_GetEngine().AUDEC_GetVAD();
 	n = AUDEC_GetEngine().AUDEC_SetVAD(0);
 	n = AUDEC_GetEngine().AUDEC_GetVAD();

	if (m_pChanDevCaptureAudio == NULL)
	{
		m_pChanDevCaptureAudio = IMXEC_CapChanDev::Create(*this);
	}

	if (m_pChanDevCaptureAudio)
	{
		m_pChanDevCaptureAudio->Open(GetSafeHwnd(),0);
		m_pChanDevCaptureAudio->OpenAudio(-1,(X_AUDIO_CODEC_ID)m_nAudioCode);
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientCloseMic()
{
	if (m_pChanDevCaptureAudio)
	{
		m_pChanDevCaptureAudio->CloseVideoDev();
		m_pChanDevCaptureAudio->CloseAudio();
		m_pChanDevCaptureAudio->Close();
		delete m_pChanDevCaptureAudio;
		m_pChanDevCaptureAudio = NULL;

		AUDEC_Engine::AUDEC_Terminate();
	}
}

void CTestMPClientDlg::OnIMXEC_CapChanCallbackDrawD3D9DC(void* hDC/*HDC*/)
{
	//SetTextColor((HDC)hDC, RGB(255,0,0));
	//SetBkMode((HDC)hDC, TRANSPARENT);

	//int nType = 0;
	//RECT rc;rc.left = 10;rc.top = 30;rc.bottom = 60;rc.right = 300;

	//if (m_pChanDevCapture)
	//{
	//	CString strText;
	//	strText.Format(_T("Capture_CallbackDrawD3D9DC fps=%d"),m_pChanDevCapture->GetVideoShowFrameRate());
	//	DrawText((HDC)hDC,strText,-1,&rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
	//}
}

void CTestMPClientDlg::OnIMXEC_PlayChanCallbackDrawD3D9DC(void* hDC/*HDC*/)
{
	//SetTextColor((HDC)hDC, RGB(0,0,255));
	//SetBkMode((HDC)hDC, TRANSPARENT);

	//int nType = 0;
	//RECT rc;rc.left = 10;rc.top = 30;rc.bottom = 60;rc.right = 300;

	///*CString strText;
	//switch(nType)
	//{
	//case 0:
	//	strText.Format(_T("FrameRate = %2d Unknown Decoder"),m_nFrameRateActual);
	//	break;
	//case 1:
	//	strText.Format(_T("FrameRate = %2d SoftWare Decoder"),m_nFrameRateActual);		
	//	break;
	//case 2:
	//	strText.Format(_T("FrameRate = %2d Intel HardWare Decoder"),m_nFrameRateActual);		
	//	break;
	//};*/

	//if (m_pIMXECPlayChan)
	//{
	//	CString strText;
	//	strText.Format(_T("Player_CallbackDrawD3D9DC fps=%d"),m_pIMXECPlayChan->GetVideoShowFrameRate());
	//	DrawText((HDC)hDC,strText,-1,&rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
	//}
}


void CTestMPClientDlg::ReSetVideoSize(int nLocal)
{
	if (nLocal == 0)
	{
		if (m_pChanDevCapture)
		{
			CRect rc;
			m_pDlgVideoLocal->GetClientRect(rc);
			m_pChanDevCapture->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	else if (nLocal == 1)
	{
		if (m_pIMXECPlayChan)
		{
			CRect rc;
			m_pDlgVideoRemote->GetClientRect(rc);
			m_pIMXECPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	else if (nLocal == 2)
	{
		if (m_pChanDevCapture2)
		{
			CRect rc;
			m_pDlgVideoLocal2->GetClientRect(rc);
			m_pChanDevCapture2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	else if (nLocal == 3)
	{
		if (m_pIMXECPlayChan2)
		{
			CRect rc;
			m_pDlgVideoRemote2->GetClientRect(rc);
			m_pIMXECPlayChan2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
	}
	else if (nLocal == 4)
	{
		if (m_pDlgVideoLocal3 && m_strCurrentCallID.size() > 0)
		{
			CRect rc;
			m_pDlgVideoLocal3->GetClientRect(rc);
			//m_pChanDevCapture2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
			IMXEC_Call::SetLocalMainVideoWndPos(m_strCurrentCallID.c_str(),0,0,rc.Width(),rc.Height());
		}
	}
	else if (nLocal == 5)
	{
		if (m_pDlgVideoRemote3 && m_strCurrentCallID.size() > 0)
		{
			CRect rc;
			m_pDlgVideoRemote3->GetClientRect(rc);
			//m_pChanDevCapture2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
			IMXEC_Call::SetRemoteMainVideoWndPos(m_strCurrentCallID.c_str(),0,0,rc.Width(),rc.Height());
		}
	}
}
void CTestMPClientDlg::OnBnClickedButtonClientCapture2()
{
	UpdateData(TRUE);

	if (!CheckNew())
	{
		SetTimer(1002,1000,NULL);
		return;
	}

	OnBnClickedButtonClientCaptureStop2();

	IMXEC_CapChanDev::SetVideoAutoFormat(m_btnAutoSize.GetCheck());

	if (m_pChanDevCapture2 == NULL)
	{
		m_pChanDevCapture2 = IMXEC_CapChanDev::Create(*this);
	}
	if (m_pChanDevCapture2)
	{
		m_pChanDevCapture2->Open(m_pDlgVideoLocal2->GetSafeHwnd(),0);

		CString	Filter = _T("Media Files|*.dat;*.avi;*.mp3;*.mpeg;*.mpg;*.mov;*.vob;*.asf;*.wmv;*.wm;*.wma;*.rm;*.rmvb;*.flv;*.mp4;*.f4v;*.mts;*.mkv;*.avms||");

		//CFileDialog dlgFile(TRUE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST,Filter,this,0, FALSE);		
		//if(dlgFile.DoModal() == IDOK)
		{
			//CString strFilePath = dlgFile.GetPathName();

			CString strText;
			m_editDevID2.GetWindowText(strText);
			std::string strDevID = CW2A(strText);
			m_editFrameRate2.GetWindowText(strText);
			std::string strFrameRate = CW2A(strText);
			m_editBitRate2.GetWindowText(strText);
			std::string strBitRate = CW2A(strText);


			m_editWidth2.GetWindowText(strText);
			std::string strWidth = CW2A(strText);

			m_editHeight2.GetWindowText(strText);
			std::string strHeight = CW2A(strText);

			int nIndex = m_comboxNetType2.GetCurSel();
			int nNetType = m_comboxNetType2.GetItemData(nIndex);

			//int nResult = m_pChanDevCapture->OpenVideoDev(KW2A(strFilePath).c_str(), 1, 512,0, VIDEC_CODEC_H264_SVC, 1, 1);
			int nResult = m_pChanDevCapture2->OpenVideoDev(atoi(strDevID.c_str()), atoi(strWidth.c_str()), atoi(strHeight.c_str()), atoi(strFrameRate.c_str()), atoi(strBitRate.c_str()), nNetType, VIDEC_CODEC_H264_SVC, 1, 1);

			//m_pChanDevCapture->SetWindowPos(NULL,0,0,1920,1080,SWP_SHOWWINDOW);
			CRect rc;
			m_pDlgVideoLocal2->GetClientRect(rc);
			m_pChanDevCapture2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

			m_pChanDevCapture2->SetForceEncodeVideo(m_btnEncode.GetCheck());
			m_pChanDevCapture2->SetDenoise(m_btnDenoise.GetCheck());

			//m_pChanDevCapture2->SetVolume(0);

			unsigned long ulAudioID = m_pChanDevCapture2->GetAudioID();
			if (m_pChanDevCaptureAudio)
			{
				ulAudioID = m_pChanDevCaptureAudio->GetAudioID();
			}

			//CString strText;
			strText.Format(_T("%u,%u,%s,%s,%s"),ulAudioID,m_pChanDevCapture2->GetVideoID(),KA2W(NETEC_Node::GetNodeID()),KA2W(NETEC_Node::GetMCUID()),KA2W(NETEC_Node::GetMCUIP()));

			m_editCapture2.SetWindowText(strText);

			m_editReceive2.SetWindowText(strText);
		}
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientCaptureStop2()
{
	KillTimer(1001);
	KillTimer(1002);

	if (m_pChanDevCapture2)
	{
		m_pChanDevCapture2->CloseVideoDev();
		m_pChanDevCapture2->CloseAudio();
		m_pChanDevCapture2->Close();
		delete m_pChanDevCapture2;
		m_pChanDevCapture2 = NULL;
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientReceive2()
{
	if (!CheckNew())
	{
		return;
	}

	OnBnClickedButtonClientReceiveStop2();

	if (m_pIMXECPlayChan2 == NULL)
	{
		m_pIMXECPlayChan2 = IMXEC_PlayChan::Create(*this);
	}
	if (m_pIMXECPlayChan2)
	{
		unsigned long nAudChanID=0;
		unsigned long nVidChanID=0;
		std::string strNoidID;
		std::string strMCUID;
		std::string strMCUIP;

		CString strText;
		m_editReceive2.GetWindowText(strText);

		CString strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		nAudChanID = strtoul(KW2A(strTemp).c_str(),NULL,10);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		nVidChanID = strtoul(KW2A(strTemp).c_str(),NULL,10);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		strNoidID = KW2A(strTemp);

		strTemp = strText.Left(strText.Find(_T(",")));
		strText = strText.Right(strText.GetLength() - strText.Find(_T(",")) - 1 );
		strMCUID = KW2A(strTemp);

		strMCUIP = KW2A(strText);


		if (nAudChanID == 0 || nVidChanID == 0 || 0!=m_pIMXECPlayChan2->Open(m_pDlgVideoRemote2->GetSafeHwnd(),nAudChanID,nVidChanID,strNoidID.c_str(),"",0,"",0,strMCUID.c_str(),strMCUIP.c_str(),4222,1))
		{
			m_pIMXECPlayChan2->Close();
			delete m_pIMXECPlayChan2;
			m_pIMXECPlayChan2=NULL;
		}

		if (m_pIMXECPlayChan2)
		{
			if (m_bVideopoll2)
			{
				//m_pIMXECPlayChan2->SetVideoPolledMode();
			}

			m_pIMXECPlayChan2->EnableAudio(1);
			m_pIMXECPlayChan2->StartAudio();
			m_pIMXECPlayChan2->EnableVideo(1);
			m_pIMXECPlayChan2->StartVideo();	


			int nIndex = m_comboxFrameType2.GetCurSel();
			int nType = m_comboxFrameType2.GetItemData(nIndex);
			m_pIMXECPlayChan2->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)nType);
			m_pIMXECPlayChan2->RequestKeyFrame();
			//m_pIMXECPlayChan->SetVideoSkipFrame(1,1);
			CRect rc;
			m_pDlgVideoRemote2->GetClientRect(rc);
			m_pIMXECPlayChan2->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

			//m_pIMXECPlayChan->SetWindowPos(NULL,445,210,120,90,SWP_SHOWWINDOW);
		}

		//m_editReceive.SetWindowText(_T(""));
	}
}

void CTestMPClientDlg::OnBnClickedButtonClientReceiveStop2()
{
	if (m_pIMXECPlayChan2)
	{
		m_pIMXECPlayChan2->Close();
		delete m_pIMXECPlayChan2;
		m_pIMXECPlayChan2 = NULL;
	}
	m_bStartRecordToAVI2=false;

}

void CTestMPClientDlg::OnBnClickedButtonH323capture()
{
	UpdateData(TRUE);

	CString strText;
	m_editPort1.GetWindowText(strText);
	std::string strTemp = CW2A(strText);
	IMXEC_H323::SetH323ListenPort(atoi(strTemp.c_str()));
	m_editPort2.GetWindowText(strText);
	strTemp = CW2A(strText);
	IMXEC_H323::SetRTPBasePort(atoi(strTemp.c_str()));
	m_editLocalIP.GetWindowText(strText);
	strTemp = CW2A(strText);
	IMXEC_H323::SetLocalIP(strTemp.c_str());
	IMXEC_H323::SetSupportH239(1);	
	IMXEC_H323::Stop();
	IMXEC_H323::SetUserName("100001");
	IMXEC_H323::Start();


	m_editDevID1.GetWindowText(strText);
	std::string strDevID = CW2A(strText);
	m_editFrameRate3.GetWindowText(strText);
	std::string strFrameRate = CW2A(strText);
	m_editBitRate3.GetWindowText(strText);
	std::string strBitRate = CW2A(strText);

	m_editDevID3.GetWindowText(strText);
	strDevID = CW2A(strText);

	int nIndex = m_comboxH323NetType1.GetCurSel();
	int nNetType = m_comboxH323NetType1.GetItemData(nIndex);
	nIndex = m_comboxH323Size1.GetCurSel();
	int nSize = m_comboxH323Size1.GetItemData(nIndex);
	IMXEC_VideoSetting::SetMainVideoDevParam(atoi(strDevID.c_str()),atoi(strFrameRate.c_str()),atoi(strBitRate.c_str()),VIDEC_CODEC_H264,(VIDEO_FORMAT_TYPE)nSize,nNetType);

	m_editDevID4.GetWindowText(strText);
	if (strText != _T("-1"))
	{
		strDevID = CW2A(strText);
		m_editFrameRate4.GetWindowText(strText);
		strFrameRate = CW2A(strText);
		m_editBitRate4.GetWindowText(strText);
		strBitRate = CW2A(strText);
		nIndex = m_comboxH323NetType2.GetCurSel();
		nNetType = m_comboxH323NetType2.GetItemData(nIndex);
		nIndex = m_comboxH323Size2.GetCurSel();
		nSize = m_comboxH323Size2.GetItemData(nIndex);
		IMXEC_VideoSetting::SetSecondVideoDevParam(atoi(strDevID.c_str()),atoi(strFrameRate.c_str()),atoi(strBitRate.c_str()),VIDEC_CODEC_H264,(VIDEO_FORMAT_TYPE)nSize,nNetType);
	}

}

void CTestMPClientDlg::OnBnClickedButtonH323stop()
{
	//if(m_strCurrentCallID.size()>0)
	
}

void CTestMPClientDlg::OnBnClickedButtonH323call()
{
	UpdateData(TRUE);
	CString strText;
	m_editServer2.GetWindowText(strText);
	std::string strTemp = CW2A(strText);
	m_strCurrentCallID=IMXEC_Call::MakeCall(strTemp.c_str(),"H323");
}

void CTestMPClientDlg::OnBnClickedButtonH323hangup()
{
	IMXEC_Call::HangupCall(m_strCurrentCallID.c_str());
}

void CTestMPClientDlg::OnBnClickedButtonH323accept()
{
	IMXEC_Call::AcceptCall(m_strCurrentCallID.c_str());
}

void CTestMPClientDlg::OnBnClickedButtonH323second()
{
	m_nStartSecondVideo = !m_nStartSecondVideo;
	if (m_nStartSecondVideo!=0)
	{
		IMXEC_Call::StartSecondVideo(m_strCurrentCallID.c_str());
	}
	else
	{
		IMXEC_Call::StopSecondVideo(m_strCurrentCallID.c_str());
	}
}


void CTestMPClientDlg::ShowMsgText(CString strText,const char*cszCallID)
{
	CString strData;
	time_t t;
	t=time(&t);
	struct tm* pTimeStruct = localtime(const_cast<time_t *>(&t));

	if(pTimeStruct)
	{
		// 99 or 02, not 1999 or 2002, Year 2050 problem...
		int nYear = pTimeStruct->tm_year;
		strData.Format(_T("%02d:%02d:%02d"),/*((nYear<50)?(nYear+2000):(nYear+1900)),(pTimeStruct->tm_mon + 1),(pTimeStruct->tm_mday),*/(pTimeStruct->tm_hour),(pTimeStruct->tm_min),(pTimeStruct->tm_sec));
	}
	std::string strCallID=cszCallID;
	CString strTemp = CA2W(strCallID.c_str());
	CString strMsg;
	strMsg.Format(_T("%s %s(CallID=%s)"),strData,strText,strTemp);

	m_listMsg.AddString(strMsg);
}


void CTestMPClientDlg::OnIMXEC_CallRingIn(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	ShowMsgText(_T("OnIMXEC_CallRingIn"),cszCallID);

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
		m_strCurrentCallID=cszCallID;
		IMXEC_Call::AcceptCall(cszCallID);
	}
}

void CTestMPClientDlg::OnIMXEC_CallRingOut(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	ShowMsgText(_T("OnIMXEC_CallRingOut"),cszCallID);
}

void CTestMPClientDlg::OnIMXEC_CallEstablished(const char*cszCallID,const char*cszUserID,const char*cszUserName)
{
	ShowMsgText(_T("OnIMXEC_CallEstablished"),cszCallID);
	if (m_strCurrentCallID==cszCallID)
	{
		if (m_pDlgVideoRemote3!=NULL)
		{
			IMXEC_Call::SetRemoteMainVideoWnd(cszCallID,m_pDlgVideoRemote3->GetSafeHwnd());

			CRect rc;
			m_pDlgVideoRemote3->GetClientRect(rc);

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

		IMXEC_Call::SetMainVideoScreenIndex(cszCallID,0);
		IMXEC_Call::SetRemoteVideoScreenWnd(cszCallID,0,0,0);

		int nTemp=IMXEC_Call::GetCallSupportSecondVideo(cszCallID);

	}
	/*else if (m_strCallID2==cszCallID)
	{
		if (m_pDlgVideoRemote4!=NULL)
		{
			IMXEC_Call::SetRemoteMainVideoWnd(cszCallID,m_pDlgVideoRemote4->GetSafeHwnd());

			CRect rc;
			m_pDlgVideoRemote4->GetClientRect(rc);

			IMXEC_Call::SetRemoteMainVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());

			m_pDlgVideoRemote4->SetWindowPos(NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_SHOWWINDOW);
		}
	}*/

	{
		const char * cszCallID = m_strCurrentCallID.c_str();

		CString strText;
		m_editDevID3.GetWindowText(strText);
		std::string strDevID = CW2A(strText);

		IMXEC_Call::SetLocalMainVideoSource(m_strCurrentCallID.c_str(),atoi(strDevID.c_str()));

		if (m_pDlgVideoLocal3!=NULL)
		{
			IMXEC_Call::SetLocalMainVideoWnd(cszCallID,m_pDlgVideoLocal3->GetSafeHwnd());

			CRect rc;
			m_pDlgVideoLocal3->GetClientRect(rc);

			IMXEC_Call::SetLocalMainVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());
		}

		m_editDevID4.GetWindowText(strText);
		if (strText != _T("-1"))
		{
			strDevID = CW2A(strText);
			IMXEC_Call::SetLocalSecondVideoSource(m_strCurrentCallID.c_str(),atoi(strDevID.c_str()));
		}
	}
}

void CTestMPClientDlg::OnIMXEC_CallDestroyed(const char*cszCallID,const char*cszUserID,const char*cszUserName,IMXEC_CALL_END_REASON nReason)
{
	ShowMsgText(_T("OnIMXEC_CallDestroyed"),cszCallID);

	if (m_strCurrentCallID==cszCallID)
	{
		m_strCurrentCallID="";
	}
	else if (m_strCallID2==cszCallID)
	{
		m_strCallID2="";
	}
}

void CTestMPClientDlg::OnIMXEC_CallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	ShowMsgText(_T("OnIMXEC_CallRecvdPTZ"),cszCallID);
}

void CTestMPClientDlg::OnIMXEC_CallRecvdDTMF(const char*cszCallID,char szDtmf)
{
	ShowMsgText(_T("OnIMXEC_CallRecvdDTMF"),cszCallID);
}

void CTestMPClientDlg::OnIMXEC_CallSecondVideoStart(const char*cszCallID)
{
	ShowMsgText(_T("OnIMXEC_CallSecondVideoStart"),cszCallID);

	if (m_pDlgVideoRemote4!=NULL)
	{
		IMXEC_Call::SetSecondVideoWnd(cszCallID,m_pDlgVideoRemote4->GetSafeHwnd());
		CRect rc;
		m_pDlgVideoRemote4->GetClientRect(rc);
		IMXEC_Call::SetSecondVideoWndPos(cszCallID,0,0,rc.Width(),rc.Height());
	}
}

void CTestMPClientDlg::OnIMXEC_CallSecondVideoStop(const char*cszCallID)
{
	ShowMsgText(_T("OnIMXEC_CallSecondVideoStop"),cszCallID);
}

void CTestMPClientDlg::OnBnClickedButtonCaptureBmp1()
{
	if (m_pIMXECPlayChan)
	{
		if (m_pIMXECPlayChan->CaptureToBMPFile("d:/demo.bmp"))
		{
			SetTimer(100,100,NULL);			
		}		
	}
}

void CTestMPClientDlg::OnBnClickedButtonCaptureBmp2()
{
	if (m_pIMXECPlayChan2)
	{
		if (m_pIMXECPlayChan2->CaptureToBMPFile("d:/demo.bmp"))
		{
			SetTimer(100,100,NULL);
		}		
	}	
}

void CTestMPClientDlg::OnCbnSelchangeComboFrametype1()
{
	UpdateData(TRUE);
	
	if (m_pIMXECPlayChan)
	{
		int nIndex = m_comboxFrameType1.GetCurSel();
		int nType = m_comboxFrameType1.GetItemData(nIndex);
		m_pIMXECPlayChan->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)nType);
		m_pIMXECPlayChan->RequestKeyFrame();
	}	
}

void CTestMPClientDlg::OnCbnSelchangeComboFrametype2()
{
	UpdateData(TRUE);

	if (m_pIMXECPlayChan2)
	{
		int nIndex = m_comboxFrameType2.GetCurSel();
		int nType = m_comboxFrameType2.GetItemData(nIndex);
		m_pIMXECPlayChan2->SetFrameTypeControlMode((FRAME_TYPE_CONTROL_MODE)nType);
		m_pIMXECPlayChan2->RequestKeyFrame();
	}
}

void CTestMPClientDlg::OnBnClickedButtonVideopoll1()
{
	m_bVideopoll1 = !m_bVideopoll1;
	if (m_bVideopoll1)
	{
		SetTimer(3000,5000,NULL);
	}
	else
	{
		KillTimer(3000);
	}

	OnBnClickedButtonClientReceive();

	m_bVideopoll2;

}

void CTestMPClientDlg::OnBnClickedButtonVideopoll2()
{
	m_bVideopoll2 = !m_bVideopoll2;
	if (m_bVideopoll2)
	{
		SetTimer(3001,5000,NULL);
	}
	else
	{
		KillTimer(3001);
	}
	OnBnClickedButtonClientReceive2();
}

void CTestMPClientDlg::OnBnClickedButtonRecvRecord()
{
	if (m_pIMXECPlayChan)
	{
		if (!m_bStartRecordToAVI1)
		{
			m_pIMXECPlayChan->StartRecordToAVI("d:\\Recv1.mp4",WRITER_STANDARD_FORMAT_MP4);
			m_bStartRecordToAVI1=true;
		}
		else
		{
			m_pIMXECPlayChan->StopRecordToAVI();
			m_bStartRecordToAVI1=false;
		}
	}
}

void CTestMPClientDlg::OnBnClickedButtonRecvRecord2()
{
	if (m_pIMXECPlayChan2)
	{
		if (!m_bStartRecordToAVI2)
		{
			m_pIMXECPlayChan2->StartRecordToAVI("d:\\Recv2.mp4",WRITER_STANDARD_FORMAT_MP4);
			m_bStartRecordToAVI2=true;
		}
		else
		{
			m_pIMXECPlayChan2->StopRecordToAVI();
			m_bStartRecordToAVI2=false;
		}
	}
}
