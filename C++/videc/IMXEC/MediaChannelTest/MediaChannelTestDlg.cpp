// MediaChannelTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MediaChannelTest.h"
#include "MediaChannelTestDlg.h"
#include "DlgAudioSetting.h"
#include "DlgVideoSetting.h"

#include <AUDEC/AUDEC_Engine.h>
#include <VIDEC/VIDEC_DVCDev.h>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_Player.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define LEFT_OFFSET	12
#define TOP_OFFSET	12

static int g_nJPGFileIndex=0;

// CMediaChannelTestDlg 对话框

CMediaChannelTestDlg::CMediaChannelTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMediaChannelTestDlg::IDD, pParent)
	, m_AudioCodecID(X_AUDIO_CODEC_AUDEC_24)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDlgVideoLocal=NULL;
	m_pDlgVideoRemote=NULL;

	m_nDevID=1;
	m_VideoCodecType=VIDEC_CODEC_H264_SVC;
	m_nFrameRate=30;
	m_nBitrate=1024;
	m_nWidth=1920;
	m_nHeight=1080;

	m_pCapChanDev=NULL;
	m_pPlayChan=NULL;

	m_pAudioMixerCapChan=NULL;

#ifdef TEST_BK_MUSIC
	m_pBackgroundMusic=NULL;
#endif
}

void CMediaChannelTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_btnStatus);
	DDX_Control(pDX, IDC_BUTTON_START, m_btnStart);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_CHECK_SUB_VIDEO, m_btnSubVideo);
	DDX_Control(pDX, IDC_CHECK_QSUB_VIDEO, m_btnQSub);
}

BEGIN_MESSAGE_MAP(CMediaChannelTestDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_AUDIO_SETTING, &CMediaChannelTestDlg::OnBnClickedButtonAudioSetting)
	ON_BN_CLICKED(IDC_BUTTON_VIDEO_SETTING, &CMediaChannelTestDlg::OnBnClickedButtonVideoSetting)
	ON_BN_CLICKED(IDC_BUTTON_START, &CMediaChannelTestDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CMediaChannelTestDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_CHECK_SUB_VIDEO, &CMediaChannelTestDlg::OnBnClickedCheckSubVideo)
	ON_BN_CLICKED(IDC_CHECK_QSUB_VIDEO, &CMediaChannelTestDlg::OnBnClickedCheckQsubVideo)
	ON_WM_TIMER()
	ON_BN_CLICKED(BTN_MEDIA, &CMediaChannelTestDlg::OnBnClickedMedia)
END_MESSAGE_MAP()


// CMediaChannelTestDlg 消息处理程序

BOOL CMediaChannelTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(FALSE);

	//VIDEC_DVCDev::SetIP("192.168.4.160",4222,"123");

	VIDEC_SetEnableDonotEncodeNoSignalVideo(1);
	VIDEC_SetEnableVideoDisplaySmoothly(0);
	VIDEC_SetHardwareEncoderMaxCount(2);
	VIDEC_Player::SetMaxDXVAPlayers(2);

	NETEC_Core::Start(0);
	//NETEC_Node::SetServerIP("180.168.50.93");
	//NETEC_Node::SetServerIP("192.168.2.121");
	NETEC_Node::SetServerIP("192.168.12.201");
	NETEC_Node::SetServerPort(4222);
	NETEC_Node::Start();

	//NETEC_Setting::SetAudioProtocolType(NETEC_Setting::PT_TCP);
	NETEC_Setting::SetAudioProtocolType(NETEC_Setting::PT_UDP);
	//NETEC_Setting::SetVideoProtocolType(NETEC_Setting::PT_TCP);
	//NETEC_Setting::SetVideoProtocolType(NETEC_Setting::PT_RTP);
	NETEC_Setting::SetVideoProtocolType(NETEC_Setting::PT_UDP);
	//NETEC_Setting::SetVideoProtocolType(NETEC_Setting::PT_AUDP);
	//NETEC_Setting::SetSessionProtocolType(NETEC_Setting::PT_AUDP);
	//NETEC_Setting::SetEnableMulticast(1);
	NETEC_Setting::SetEnableDynamicChannel(1);

	CWnd* pDeskTopWnd = GetDesktopWindow();//FromHandle(HWND_DESKTOP);
	m_pDlgVideoLocal=new DlgVideo(pDeskTopWnd/*this*/);
	m_pDlgVideoLocal->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoLocal->SetWindowPos(NULL,LEFT_OFFSET,TOP_OFFSET,352,288,SWP_SHOWWINDOW);

	m_pDlgVideoRemote=new DlgVideo(pDeskTopWnd/*this*/);
	m_pDlgVideoRemote->Create(DlgVideo::IDD,pDeskTopWnd/*this*/);
	m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,SWP_SHOWWINDOW);


	NETEC_Setting::SetMCUOnly(1);

	IMXEC_CapChanDev::SetVideoAutoFormat(1);

//	NETEC_Setting::SetProxyHost("192.168.7.93");
//#if 0
//	NETEC_Setting::SetProxyPort(80);
//	NETEC_Setting::SetProxyType(NETEC_Setting::PT_HTTP_PROXY);
//#else
//	NETEC_Setting::SetProxyUser("a1");
//	NETEC_Setting::SetProxyPassword("123");
//	NETEC_Setting::SetProxyPort(1080);
//	NETEC_Setting::SetProxyType(NETEC_Setting::PT_SOCK5);
//#endif

	//IMXEC_PDFConvert::SetTempPath("E:\\");

	//IMXEC_PDFConvert*pPDFConvert=IMXEC_PDFConvert::Create();
	//pPDFConvert->Open("E:\\1.doc","E:\\1.pdf");

	//while (true)
	//{
	//	if (pPDFConvert->GetStatus()==IMXEC_PDFConvert::PDF_STATUS_CONVERT_OK ||
	//		pPDFConvert->GetStatus()==IMXEC_PDFConvert::PDF_STATUS_CONVERT_FAILED)
	//	{
	//		break;
	//	}
	//	else
	//	{
	//		Sleep(100);
	//	}
	//}

	//pPDFConvert->Close();
	//delete pPDFConvert;
	//pPDFConvert=NULL;

	AUDEC_GetEngine().AUDEC_SetVAD(0);

	//m_pAudioMixerCapChan=IMXEC_AudioMixerCapChan::Create();
	//m_pAudioMixerCapChan->Open();
	//m_pAudioMixerCapChan->Close();
	//delete m_pAudioMixerCapChan;
	//m_pAudioMixerCapChan=NULL;
#ifdef TEST_BK_MUSIC
	m_pBackgroundMusic=IMXEC_BackgroundMusic::Create(*this);
	if (m_pBackgroundMusic)
	{
		if (0!=m_pBackgroundMusic->Open("g:\\media_files\\mp3\\1.mp3",true))
		{
			m_pBackgroundMusic->Close();
			delete m_pBackgroundMusic;
			m_pBackgroundMusic=NULL;
		}
		else
		{
			m_pBackgroundMusic->SetVolume(2);
		}
	}
#endif

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMediaChannelTestDlg::OnPaint()
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
HCURSOR CMediaChannelTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CMediaChannelTestDlg::DestroyWindow()
{
#ifdef TEST_BK_MUSIC
	if (m_pBackgroundMusic)
	{
		m_pBackgroundMusic->Close();
		delete m_pBackgroundMusic;
		m_pBackgroundMusic=NULL;
	}
#endif

	if (m_pCapChanDev!=NULL)
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev=NULL;
	}

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

	NETEC_Node::Stop();
	NETEC_Core::Stop();

	return CDialog::DestroyWindow();
}

void CMediaChannelTestDlg::OnBnClickedButtonAudioSetting()
{
	DlgAudioSetting dlgAudioSetting(this);
	dlgAudioSetting.m_AudioCodecID=m_AudioCodecID;

	if (dlgAudioSetting.DoModal()==IDOK)
	{
		m_AudioCodecID=dlgAudioSetting.m_AudioCodecID;
		if (m_pCapChanDev!=NULL)
		{
			m_pCapChanDev->OpenAudio(-1,m_AudioCodecID);
		}
	}
}

void CMediaChannelTestDlg::OnBnClickedButtonVideoSetting()
{
	DlgVideoSetting dlgVideoSetting(this);
	dlgVideoSetting.m_nDevID=m_nDevID;
	dlgVideoSetting.m_nFrameRate=m_nFrameRate;
	dlgVideoSetting.m_nBitrate=m_nBitrate;
	dlgVideoSetting.m_VideoCodecType=m_VideoCodecType;
	dlgVideoSetting.m_nWidth=m_nWidth;
	dlgVideoSetting.m_nHeight=m_nHeight;

	if (dlgVideoSetting.DoModal()==IDOK)
	{
		m_nDevID=dlgVideoSetting.m_nDevID;
		m_nFrameRate=dlgVideoSetting.m_nFrameRate;
		m_nBitrate=dlgVideoSetting.m_nBitrate;
		m_VideoCodecType=dlgVideoSetting.m_VideoCodecType;
		m_nWidth=dlgVideoSetting.m_nWidth;
		m_nHeight=dlgVideoSetting.m_nHeight;

		if (m_pCapChanDev!=NULL)
		{
#if 1
			m_pCapChanDev->OpenVideoDev(m_nDevID,m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,VIDEC_NETWORK_SATELLITE,m_VideoCodecType,0,0);
#else
			//m_pCapChanDev->OpenVideoDev("192.168.2.246","sanyo","admin","admin",m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,0,0,0);
			m_pCapChanDev->OpenVideoDev("192.168.11.201","alk","admin","admin",m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,0,1,1);
#endif
		}
	}
}

//音频数据回调函数
void CMediaChannelTestDlg::OnIMXEC_CapChanCallbackAudioData(unsigned char*pData,int nLen,bool&bLoop)
{
}

void CMediaChannelTestDlg::OnIMXEC_CapChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{
}

//视频数据回调函数
//nKeyFrame,0:非关键帧 1:关键帧
void CMediaChannelTestDlg::OnIMXEC_CapChanCallbackVideoData(unsigned char*pData,int nLen,int nKeyFrame)
{

}

void CMediaChannelTestDlg::OnIMXEC_CapChanCallbackMotionDetected(int*pResultList,int nResultCount)
{

}

void CMediaChannelTestDlg::OnIMXEC_CapChanCallbackAudioIDChanged(unsigned long nAudioID)
{
	char szDebug[1024]="";
	sprintf(szDebug,"OnIMXEC_CapChanCallbackAudioIDChanged nAudioID=%u\n",nAudioID);
	OutputDebugString(szDebug);

	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->SetAudioID(nAudioID);
	}
}

//音频数据回调函数
void CMediaChannelTestDlg::OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop)
{
	//X_AUDIO_CODEC_ID nCodecID=(X_AUDIO_CODEC_ID)AUDEC_HEADER_GET_CODEC_ID(pData);
	//char szDebug[1024];
	//sprintf(szDebug,"nCodecID=%d\n",nCodecID);
	//OutputDebugString(szDebug);
}

void CMediaChannelTestDlg::OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)
{

}

//视频数据回调函数
//nKeyFrame,0:非关键帧 1:关键帧
void CMediaChannelTestDlg::OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode)
{

}

void CMediaChannelTestDlg::OnIMXEC_PlayChanCallbackRemoteSnapJPG(const char*cszPathName)
{
	OutputDebugString("OnIMXEC_PlayChanCallbackRemoteSnapJPG\n");
}


void CMediaChannelTestDlg::OnBnClickedButtonStart()
{
	if (m_pDlgVideoLocal==NULL)
	{
		return ;
	}

	m_pCapChanDev=IMXEC_CapChanDev::Create(*this);
	if (m_pCapChanDev==NULL)
	{
		return ;
	}

	m_pCapChanDev->SetForceEncodeAudio(1);
	m_pCapChanDev->SetForceEncodeVideo(1);

	char szDebug[1024]="";
	sprintf(szDebug,"m_pCapChanDev->GetAudioID()=%u\n",m_pCapChanDev->GetAudioID());
	OutputDebugString(szDebug);

	if (0!=m_pCapChanDev->Open(m_pDlgVideoLocal->m_hWnd,0))
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev=NULL;
		return ;
	}
	m_pCapChanDev->SetScreenWnd(0,0,0);

	m_pCapChanDev->OpenAudio(-1,m_AudioCodecID);

	AUDEC_GetEngine().AUDEC_SetVAD(0);
#if 1
	//if (m_nDevID==0)
	//{
	//	m_pCapChanDev->OpenVideoDev(-1,m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,VIDEC_NETWORK_SATELLITE,m_VideoCodecType,1,1,"192.168.4.160",4222,"123");
	//}
	//else if (m_nDevID==1)
	//{
	//	m_pCapChanDev->OpenVideoDev(m_nDevID,m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,VIDEC_NETWORK_SATELLITE,m_VideoCodecType,1,1,"192.168.5.182",4222,"123");
	//}
	//else
	{
		m_pCapChanDev->OpenVideoDev(m_nDevID,m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,VIDEC_NETWORK_SATELLITE,m_VideoCodecType,1,1);
	}
#else
	//m_pCapChanDev->OpenVideoDev("192.168.2.246","sanyo","admin","admin",m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,0,0,0);
	//m_pCapChanDev->OpenVideoDev("192.168.11.201","alk","admin","admin",m_nWidth,m_nHeight,m_nFrameRate,m_nBitrate,0,1,1);
#endif

	//const char*cszFileName="xms://192.168.7.217//opt/avcon/hprec/recfile//xf@xf/542@xf/av1@xf/0/NormalRecord/20120803/542@xf_0_201208030939363928179682.avms";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.avi";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.asf";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mkv";
	//const char*cszFileName="G:\\media_files\\AVMSVideo\\zdzhzx3@zj.xf_01_20120724152705.avms";
	//const char*cszFileName="G:\\media_files\\avi\\WBrecord(长).avi";
	//const char*cszFileName="G:\\media_files\\avi\\2012_9_17_9_6_48.avi";

	//m_pCapChanDev->OpenVideoDev(0,0,640,480,24,m_nFrameRate,m_nBitrate,0,m_VideoCodecType,1,1);
	//m_pCapChanDev->OpenVideoDev("G:\\media_files\\Kung.Fu.Panda.2.2011.BluRay.1080p.DTS.x264-CHD.mkv",1,m_nBitrate,0,m_VideoCodecType,1,1);
	//m_pCapChanDev->OpenVideoDev("G:\\media_files\\WMV\\1.wmv",1,m_nBitrate,0,m_VideoCodecType,1,1);
	//m_pCapChanDev->OpenVideoDev("E:\\again.wmv",1,m_nBitrate,0,m_VideoCodecType,1,1);
	//m_pCapChanDev->OpenVideoDev(cszFileName,1,m_nBitrate,0,m_VideoCodecType,1,1);

		m_pCapChanDev->EnableOSD(true);
		m_pCapChanDev->SetOSDText("国北京天安门",0);

		//m_pCapChanDev->SetMute(true);

	CRect rc;
	m_pDlgVideoLocal->GetClientRect(rc);
	m_pCapChanDev->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);

	//m_pCapChanDev->StartRecordToAVI("g:\\tmp\\2.mkv",2);

	unsigned long nAudioID=m_pCapChanDev->GetAudioID();
	unsigned long nVideoID=m_pCapChanDev->GetVideoID();

	//char szDebug[1024]="";
	sprintf(szDebug,"nAudioID=%u\n",nAudioID);
	OutputDebugString(szDebug);

	sprintf(szDebug,"nVideoID=%u\n",nVideoID);
	OutputDebugString(szDebug);

	if (m_pDlgVideoRemote!=NULL)
	{
		m_pPlayChan=IMXEC_PlayChan::Create(*this);
		if (m_pPlayChan!=NULL)
		{
			if (0!=m_pPlayChan->Open(m_pDlgVideoRemote->m_hWnd,nAudioID,nVideoID,NETEC_Node::GetNodeID(),NETEC_Node::GetNATIP(),NETEC_Node::GetNATPort(),NETEC_Node::GetLocalIP(),NETEC_Node::GetLocalPort(),NETEC_Node::GetMCUID(),NETEC_Node::GetMCUIP(),NETEC_Node::GetServerPort(),0))
			{
				m_pPlayChan->Close();
				delete m_pPlayChan;
				m_pPlayChan=NULL;
			}
			else
			{
				//m_pPlayChan->SetVideoSkipFrame(1,1);

				m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);

				m_pPlayChan->StartAudio();
				m_pPlayChan->EnableAudio(1);

				//m_pPlayChan->SetAudioCast(true);
				//m_pPlayChan->SetVideoCast(true);

				m_pPlayChan->EnableVideo(1);
				m_pPlayChan->StartVideo();
				//m_pPlayChan->StartMJPG(FRAME_TYPE_CONTROL_MODE_MAIN);

				//m_pPlayChan->StartRecordToAVI("g:\\tmp\\1.mp3",3);

				m_pDlgVideoRemote->GetClientRect(rc);
				m_pPlayChan->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
			}
		}
	}


	m_btnStart.EnableWindow(FALSE);
	m_btnStop.EnableWindow(TRUE);

	SetTimer(2000,2000,NULL);
}

void CMediaChannelTestDlg::OnBnClickedButtonStop()
{
	KillTimer(2000);

	if (m_pPlayChan)
	{
		m_pPlayChan->StopRecordToAVI();
	}

	m_btnStart.EnableWindow(TRUE);
	m_btnStop.EnableWindow(FALSE);

	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan=NULL;
	}

	if (m_pCapChanDev!=NULL)
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev=NULL;
	}
}

/******************************************************************************
* OnNETEC_NodeConnectStatusChanged
*描述：网络连接状态的回调
*输入：	cs					-网络连接状态
*输出： 无
*返回值：无
*****************************************************************************/
void CMediaChannelTestDlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs==NETEC_Session::CS_CONNECTED ||
		cs==NETEC_Session::CS_RESTARTED ||
		cs==NETEC_Session::CS_RECONNECTED)
	{
		m_btnStart.EnableWindow(TRUE);
	}
	else
	{
		//m_btnStart.EnableWindow(FALSE);
	}
}

void CMediaChannelTestDlg::OnBnClickedCheckSubVideo()
{
	VIDEC_Image*pImage=NULL;
	if (m_pCapChanDev)
	{
		pImage=m_pCapChanDev->GetImage();
	}

	if(pImage)
	{
		if (m_pPlayChan)
		{
			m_pPlayChan->SetImage(pImage);
		}
		pImage->Release();
		pImage=NULL;
	}

	if (m_pPlayChan!=NULL)
	{
		//m_pPlayChan->SetAudioCast(true);
		//m_pPlayChan->SetVideoCast(true);

		if (m_btnSubVideo.GetCheck())
		{
			//m_pPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_HALF);
			m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_SUB);
		}
		else
		{
			//m_pPlayChan->SetFrameRateControlMode(FRAME_RATE_CONTROL_MODE_FULL);
			m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_MAIN);
		}
	}

	m_btnQSub.SetCheck(0);
}

void CMediaChannelTestDlg::OnBnClickedCheckQsubVideo()
{
	if (m_pPlayChan!=NULL)
	{
		//m_pPlayChan->SetAudioCast(false);
		//m_pPlayChan->SetVideoCast(false);

		if (m_btnQSub.GetCheck())
		{
			m_pPlayChan->SetFrameTypeControlMode(FRAME_TYPE_CONTROL_MODE_QSUB);
		}
		else
		{
			OnBnClickedCheckSubVideo();
		}
	}
}

void CMediaChannelTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	//char szDebug[1024]="";
	//sprintf(szDebug,"U[%u %d] D[%u %d]\n",IMXEC_CapChanDev::GetTotalBytes(),IMXEC_CapChanDev::GetBitrate(),IMXEC_PlayChan::GetTotalBytes(),IMXEC_PlayChan::GetBitrate());
	//OutputDebugString(szDebug);

	//static int nCount=0;
	//if (nCount++>5)
	//{
	//	nCount=0;

	//	if (m_pDlgVideoRemote!=NULL)
	//	{
	//		m_pDlgVideoRemote->SetWindowPos(NULL,LEFT_OFFSET+352,TOP_OFFSET,352,288,(m_pDlgVideoRemote->IsWindowVisible()?SWP_HIDEWINDOW:SWP_SHOWWINDOW));
	//	}
	//}

	__super::OnTimer(nIDEvent);
}

void CMediaChannelTestDlg::OnBnClickedMedia()
{
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() != 1)
		return;

	if (m_pCapChanDev)
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev=NULL;
	}

	m_pCapChanDev = IMXEC_CapChanDev::Create(*this);
	if (m_pCapChanDev==NULL)
		return ;

	if (0!=m_pCapChanDev->Open(m_pDlgVideoLocal->m_hWnd,0))
	{
		m_pCapChanDev->Close();
		delete m_pCapChanDev;
		m_pCapChanDev=NULL;
		return ;
	}

	CString strPathName = dlg.GetPathName();
	char* czPathName = strPathName.GetBuffer();
	m_pCapChanDev->OpenVideoDev(czPathName, 1, 1024, 0, VIDEC_CODEC_H264_SVC);

	CRect rc;
	m_pDlgVideoLocal->GetClientRect(rc);
	m_pCapChanDev->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
}
