// DlgMain.cpp : 实现文件
//

#include "stdafx.h"
#include "VIDECRTP.h"
#include "DlgMain.h"
#include "DlgSetting.h"
#include <NETEC/NETEC_Core.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DlgMain 对话框

#define MP_WIDTH	704
#define MP_HEIGHT	288



DlgMain::DlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMain::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDlgFullScreen=NULL;
	m_pDlgVideoPlayer=NULL;
	m_pDlgVideoLocal=NULL;
	m_pDlgBar=NULL;

	m_pCamera=NULL;
	m_pCapChanRTP=NULL;
	m_pPlayChanRTP=NULL;
	m_pRTPSession=NULL;
	m_pRTPSession=NULL;

	m_nWidth=352;
	m_nHeight=288;
	m_nFrameRate=15;
	
	m_nFormat=2;

	m_nDevID=0;
	m_nQuality=5;
	m_CodecType=VIDEC_CODEC_H264;

	m_strRemoteIP="127.0.0.1";
	m_nLocalPort=(8100);
	m_nRemotePort=(8100);
}

void DlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DlgMain, CDialog)
	//ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_WM_ERASEBKGND()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgMain 消息处理程序

BOOL DlgMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	
	VIDEC_Init();
	NETEC_Core::Start();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	CRect rcClient;
	GetClientRect(rcClient);

	SetWindowPos(NULL,0,0,MP_WIDTH+(rcWnd.Width()-rcClient.Width()),MP_HEIGHT+(rcWnd.Height()-rcClient.Height()),0);
	CenterWindow(NULL);

	m_pDlgBar=new DlgBar(*this);
	m_pDlgBar->Create(DlgBar::IDD,this);
	m_pDlgVideoPlayer=new DlgVideo(*this);
	m_pDlgVideoPlayer->Create(DlgVideo::IDD,this);
	m_pDlgVideoLocal=new DlgVideo(*this);
	m_pDlgVideoLocal->Create(DlgVideo::IDD,this);

	ResizeControls();

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
//
//void DlgMain::OnPaint() 
//{
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		CDialog::OnPaint();
//	}
//}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR DlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL DlgMain::DestroyWindow()
{
	KillTimer(2000);

	if (m_pDlgBar!=NULL)
	{
		m_pDlgBar->DestroyWindow();
		delete m_pDlgBar;
		m_pDlgBar=NULL;
	}

	if (m_pDlgVideoPlayer!=NULL)
	{
		m_pDlgVideoPlayer->DestroyWindow();
		delete m_pDlgVideoPlayer;
		m_pDlgVideoPlayer=NULL;
	}
	if (m_pDlgVideoLocal!=NULL)
	{
		m_pDlgVideoLocal->DestroyWindow();
		delete m_pDlgVideoLocal;
		m_pDlgVideoLocal=NULL;
	}

	if (m_pDlgFullScreen!=NULL)
	{
		m_pDlgFullScreen->DestroyWindow();
		delete m_pDlgFullScreen;
		m_pDlgFullScreen=NULL;
	}


	DoStop();

	NETEC_Core::Stop();
	VIDEC_Terminate();

	return CDialog::DestroyWindow();
}

void DlgMain::ResizeControls(void)
{
#define BAR_HEIGHT	24
	CRect rc;
	GetClientRect(rc);
	if (m_pDlgBar!=NULL)
	{
		m_pDlgBar->SetWindowPos(NULL,0,0,rc.Width(),BAR_HEIGHT,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	if (m_pDlgVideoLocal!=NULL)
	{
		m_pDlgVideoLocal->SetWindowPos(NULL,0,BAR_HEIGHT,rc.Width()>>1,rc.Height()-BAR_HEIGHT,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
	if (m_pDlgVideoPlayer!=NULL)
	{
		m_pDlgVideoPlayer->SetWindowPos(NULL,rc.Width()>>1,BAR_HEIGHT,rc.Width()>>1,rc.Height()-BAR_HEIGHT,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

void DlgMain::SetFullScreen(DlgVideo*pDlgVideo)
{
	if (m_pDlgFullScreen!=NULL)
	{
		if (pDlgVideo==m_pDlgVideoPlayer)
		{
			if (m_pDlgVideoPlayer!=NULL)
			{
				m_pDlgVideoPlayer->SetParent(this);
			}
		}
		else
		{
			if (m_pDlgVideoLocal!=NULL)
			{
				m_pDlgVideoLocal->SetParent(this);
			}
		}

		ResizeControls();

		m_pDlgFullScreen->DestroyWindow();
		delete m_pDlgFullScreen;
		m_pDlgFullScreen=NULL;
	}
	else
	{
		m_pDlgFullScreen=new DlgFullScreen(this);
		m_pDlgFullScreen->Create(DlgFullScreen::IDD,this);
		int cx = ::GetSystemMetrics(SM_CXSCREEN);
		int cy = ::GetSystemMetrics(SM_CYSCREEN);
		//m_pDlgFullScreen->SetWindowPos(&wndTopMost,0,0,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		m_pDlgFullScreen->SetWindowPos(NULL,0,0,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);

		if (pDlgVideo==m_pDlgVideoPlayer)
		{
			m_pDlgFullScreen->SetWndChild(m_pDlgVideoPlayer);
		}
		else
		{
			m_pDlgFullScreen->SetWndChild(m_pDlgVideoLocal);
		}
	}
}

void DlgMain::DoStart(void)
{
	if (m_pPlayChanRTP==NULL)
	{
		m_pPlayChanRTP=VIDEC_PlayChanRTP::Create();
		if (m_pPlayChanRTP!=NULL)
		{
			int nPayloadType=97;
			switch (m_CodecType)
			{
			case VIDEC_CODEC_H261:
				nPayloadType=31;
				break;
			case VIDEC_CODEC_H263:
			case VIDEC_CODEC_H263P:
			case VIDEC_CODEC_H263PP:
				nPayloadType=34;
				break;
			default:
				nPayloadType=97;
				break;
			};
			if (0!=m_pPlayChanRTP->Open(nPayloadType,m_CodecType))
			{
				m_pPlayChanRTP->Close();
				delete m_pPlayChanRTP;
				m_pPlayChanRTP=NULL;
			}
			else if (m_pDlgVideoPlayer!=NULL)
			{
				m_pPlayChanRTP->AddImageSink(m_pDlgVideoPlayer->m_pWnd);
			}
		}
	}

	m_pRTPSession=NETEC_RTPSession::Create(*this);
	unsigned short usLocalPort=m_nLocalPort;
	if (0!=m_pRTPSession->Open(NULL,usLocalPort,m_strRemoteIP,m_nRemotePort))
	{
		m_pRTPSession->Close();
		delete m_pRTPSession;
		m_pRTPSession=NULL;
	}

	int nDevID=m_nDevID;
	if (nDevID<VIDEC_CameraDev::GetDevCount())
	{
		m_pCamera=VIDEC_CameraDev::Create();
		if (m_pCamera!=NULL)
		{
			VIDEC_CameraDev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);

			if (0!=m_pCamera->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate,false))
			{
				m_pCamera->Close();
				delete m_pCamera;
				m_pCamera=NULL;
			}
		}
	}

	if (m_pCamera!=NULL && m_pDlgVideoLocal!=NULL && m_pDlgVideoLocal->m_pWnd!=NULL)
	{
		m_pCamera->AddImageSink(m_pDlgVideoLocal->m_pWnd);
	}

	if (m_pCamera!=NULL)
	{
		m_pCapChanRTP=VIDEC_CapChanRTP::Create(*this);
		if (m_pCapChanRTP!=NULL)
		{
			int nPayloadType=97;
			switch (m_CodecType)
			{
			case VIDEC_CODEC_H261:
				nPayloadType=31;
				break;
			case VIDEC_CODEC_H263:
			case VIDEC_CODEC_H263P:
			case VIDEC_CODEC_H263PP:
				nPayloadType=34;
				break;
			default:
				nPayloadType=97;
				break;
			};
			if (0!=m_pCapChanRTP->Open(nPayloadType,m_nWidth,m_nHeight,m_nFrameRate,m_nQuality,m_CodecType))
			{
				m_pCapChanRTP->Close();
				delete m_pCapChanRTP;
				m_pCapChanRTP=NULL;
			}
			else
			{
				m_pCamera->AddImageSink(m_pCapChanRTP);
			}
		}
	}

	SetTimer(2000,2000,NULL);
}

void DlgMain::DoStop(void)
{
	KillTimer(2000);

	if (m_pCapChanRTP!=NULL)
	{
		m_pCapChanRTP->Close();
		delete m_pCapChanRTP;
		m_pCapChanRTP=NULL;
	}

	if (m_pRTPSession!=NULL)
	{
		m_pRTPSession->Close();
		delete m_pRTPSession;
		m_pRTPSession=NULL;
	}

	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->Close();
		delete m_pPlayChanRTP;
		m_pPlayChanRTP=NULL;
	}


	if (m_pCamera!=NULL)
	{
		m_pCamera->Close();
		delete m_pCamera;
		m_pCamera=NULL;
	}
}

void DlgMain::DoSetting(void)
{
	DlgSetting dlgSetting(*this);
	if (IDOK==dlgSetting.DoModal())
	{

	}
}
//
//BOOL DlgMain::OnEraseBkgnd(CDC* pDC)
//{
//	return FALSE;
//}

void DlgMain::OnVIDEC_CapChanRTPCallbackPacketData(VIDEC_CapChanRTP*pCapChanRTP,const void*pPacketData,int nPacketLen)
{
	m_XBitrate.Calculate(nPacketLen);
	if (m_pRTPSession!=NULL)
	{
		m_pRTPSession->Send((unsigned char*)pPacketData,nPacketLen);
	}
}

void DlgMain::OnNETEC_RTPSessionCallbackRecvdRTPPacket(NETEC_RTPSession*pRTPSession,unsigned char*pRTPPacketData,int nRTPPacketLen)
{
	if (m_pPlayChanRTP!=NULL)
	{
		m_pPlayChanRTP->OnRecvdRTPPacket((unsigned char*)pRTPPacketData,nRTPPacketLen);
	}
}


void DlgMain::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==2000)
	{
		CString strText;
		strText.Format("%d kbps",m_XBitrate.GetBitrate());
		SetWindowText(strText);
	}

	__super::OnTimer(nIDEvent);
}
