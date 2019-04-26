// DlgMain.cpp : 实现文件
//

#include "stdafx.h"

#include "TestAH2K.h"
#include "DlgMain.h"
#include "DlgSetting.h"
#include "../VIDEC_FRE/VIDECFREAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL Is64bitSystem()
{
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		return TRUE;
	else
		return FALSE;
}

void OutputSystemProgramInfo()
{
	BOOL bIsWow64 = FALSE; //32bits 
	IsWow64Process(::GetCurrentProcess(), &bIsWow64);
	int nSize = sizeof(int *);

	int df = 0;
#ifdef _WIN64
	df = 64;
#else 
	df = 32;
#endif

	DebugOutput( logcallstack ,"The program sizeof(int) :%d,sizeof(long):%d,sizeof(long long):%d ,sizeof(__int64):%d,sizeof(__int32):%d \n", sizeof(int), sizeof(long), sizeof(long long), sizeof(__int64), sizeof(__int32));
	DebugOutput( logcallstack ,"IsWow64Process:%d,Is64bitSystem:%d,program bits:%d define:%d\n", bIsWow64, Is64bitSystem(), nSize * 8, df);
}



// DlgMain 对话框
DlgMain::DlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMain::IDD, pParent)
	, m_bEncode(FALSE)
	, m_bDecode(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDlgFullScreen=NULL;
	m_pDlgCapVideo=NULL;
	m_pDlgPlayVideo=NULL;
	m_pDlgVideo=NULL;
	m_pDlgBar=NULL;


	m_pCapChan=NULL;
#ifdef _TEST_DECODER
	m_pDecoder=NULL;
#else
#ifdef TEST_PLAYER
	m_pPlayer=NULL;
#else
	m_pPlayChan=NULL;
#endif
#endif

	m_nWidth=0;
	m_nHeight=0;
	m_nFrameRate=60;
	
	m_nFormat=0;

	m_nDevID=0;
	m_nQuality=11;
	m_CodecType=VIDEC_CODEC_H264_SVC;


#ifdef _AVIFILE
	m_pAVIWriter=NULL;
#else
	m_pASFWriter=NULL;
#endif

#ifdef TEST_DC
	m_pMemDC=NULL;
	m_pDC=NULL;
	m_pDCRecord=NULL;
#endif

#ifdef TEST_RAW_FILE
	m_pReader=NULL;
#else
	m_pDev=NULL;
#endif
}

void DlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DlgMain, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
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

	VIDEC_Init(VIDEC_INIT_FLAG_ALL|VIDEC_INIT_FALG_ONLY_USB_CAMERA);

#ifdef TEST_DVC
	VIDEC_DVCDev::SetIP("192.168.5.121",4222,"123");
#endif

#ifdef TEST_PLAYER
	VIDEC_Player::SetMaxDXVAPlayers(1);
#endif

	//if (VIDEC_GetSupportHardwareEncoder())
	//	VIDEC_SetHardwareEncoderMaxCount(1);
	VIDEC_SetHardwareEncoderMaxCount(0);

	CRect rcWnd;
	GetWindowRect(rcWnd);
	CRect rcClient;
	GetClientRect(rcClient);

	SetWindowPos(NULL,0,0,1440+(rcWnd.Width()-rcClient.Width()),540+(rcWnd.Height()-rcClient.Height())+24,0);
	CenterWindow(NULL);

	m_pDlgBar=new DlgBar(*this);
	m_pDlgBar->Create(DlgBar::IDD,this);
	m_pDlgCapVideo=new DlgCapVideo(*this);
	m_pDlgCapVideo->Create(DlgCapVideo::IDD,this);
	m_pDlgPlayVideo=new DlgPlayVideo(*this);
	m_pDlgPlayVideo->Create(DlgPlayVideo::IDD,this);

	m_pDlgVideo=m_pDlgCapVideo;

	ResizeControls();

	return TRUE;  // 除非设置了控件的焦点，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void DlgMain::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
HCURSOR DlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL DlgMain::DestroyWindow()
{
	KillTimer(2000);

	m_pDlgVideo=NULL;
	DoStop();

#ifdef TEST_COREAVC
	if (g_pCoreAVCVideoPlayer!=NULL)
	{
		g_pCoreAVCVideoPlayer->ReleaseConnections();
		delete g_pCoreAVCVideoPlayer;
		g_pCoreAVCVideoPlayer=NULL;
	}
#endif

#ifdef TEST_RAW_FILE
	if (m_pReader)
	{
		m_pReader->Close();
		delete m_pReader;
		m_pReader=NULL;
	}
#endif
	if (m_pDlgBar!=NULL)
	{
		m_pDlgBar->DestroyWindow();
		delete m_pDlgBar;
		m_pDlgBar=NULL;
	}

	if (m_pDlgCapVideo!=NULL)
	{
		m_pDlgCapVideo->DestroyWindow();
		delete m_pDlgCapVideo;
		m_pDlgCapVideo=NULL;
	}

	if (m_pDlgPlayVideo!=NULL)
	{
		m_pDlgPlayVideo->DestroyWindow();
		delete m_pDlgPlayVideo;
		m_pDlgPlayVideo=NULL;
	}

	if (m_pDlgFullScreen!=NULL)
	{
		m_pDlgFullScreen->DestroyWindow();
		delete m_pDlgFullScreen;
		m_pDlgFullScreen=NULL;
	}

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
	if (m_pDlgCapVideo!=NULL)
	{
		m_pDlgCapVideo->SetWindowPos(NULL,0,BAR_HEIGHT,rc.Width()/2,rc.Height()-BAR_HEIGHT,(m_pDlgVideo==m_pDlgCapVideo?SWP_SHOWWINDOW:SWP_SHOWWINDOW)|SWP_NOACTIVATE);
	}
	if (m_pDlgPlayVideo!=NULL)
	{
		m_pDlgPlayVideo->SetWindowPos(NULL,rc.Width()/2,BAR_HEIGHT,rc.Width()/2,rc.Height()-BAR_HEIGHT,(m_pDlgVideo==m_pDlgPlayVideo?SWP_SHOWWINDOW:SWP_SHOWWINDOW)|SWP_NOACTIVATE);
	}
}

void DlgMain::SetFullScreen(void)
{
	if (m_pDlgFullScreen!=NULL)
	{
		if (m_pDlgVideo!=NULL)
		{
			m_pDlgVideo->SetParent(this);
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

		m_pDlgFullScreen->SetWndChild(m_pDlgVideo);
	}
}

void DlgMain::DoStart(void)
{
	DebugOutput( logcallstack ,"DlgMain::DoStart\n");
	OutputSystemProgramInfo();

	m_pDlgVideo=m_pDlgCapVideo;

#ifdef TEST_RAW_FILE
	m_pPlayer=VIDEC_Player::Create(*this);
	if (m_pPlayer!=NULL)
	{
		if (0!=m_pPlayer->Open(m_pDlgPlayVideo->m_hWnd))
		{
			m_pPlayer->Close();
			delete m_pPlayer;
			m_pPlayer=NULL;
		}
		else
		{
			m_pDlgPlayVideo->m_pPlayer=m_pPlayer;
			m_pDlgPlayVideo->ResizeControls();
		}
	}


	if (m_pReader==NULL)
	{
		m_pReader=new Reader(*this);
		m_pReader->Open();
	}
#else

	//SetFullScreen();

	//m_nFrameRate=1;

	int nDevID=m_nDevID;
#ifdef TEST_AH2K
	if (nDevID<VIDEC_AH2KDev::GetDevCount())
	{
		m_pDev=VIDEC_AH2KDev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_AH2KDev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate,true))
#endif
#ifdef TEST_DVC
				if (nDevID<VIDEC_DVCDev::GetDevCount())
				{
					m_pDev=VIDEC_DVCDev::Create();
					if (m_pDev!=NULL)
					{
						int nQuality=m_nQuality;
						int nBitrate=1024;
						int nMinQP=11;
						int nMaxQP=51;
						VIDEC_CODEC_TYPE CodecType=VIDEC_CODEC_H264;

						GetCodecInfo(nQuality,nBitrate,nMinQP,nMaxQP,CodecType,false);

						if (0!=m_pDev->Open(nDevID,m_nFrameRate,nBitrate,0,m_nFormat))
#endif
#ifdef TEST_CAMERA
	if (nDevID<VIDEC_CameraDev::GetDevCount())
	{
		m_pDev=VIDEC_CameraDev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_CameraDev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth=0,m_nHeight=0,m_nFrameRate))
#endif
#ifdef TEST_AH410
	if (nDevID<VIDEC_AH410Dev::GetDevCount())
	{
		m_pDev=VIDEC_AH410Dev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_AH410Dev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate))
#endif
#ifdef TEST_AH210
	if (nDevID<VIDEC_AH210Dev::GetDevCount())
	{
		m_pDev=VIDEC_AH210Dev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_AH210Dev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate))
#endif
#ifdef TEST_AH400
	if (nDevID<VIDEC_AH400Dev::GetDevCount())
	{
		m_pDev=VIDEC_AH400Dev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_AH400Dev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate))
#endif
#ifdef TEST_SCREEN
	{
		m_pDev=VIDEC_Screen::Create();
		if (m_pDev!=NULL)
		{
			if (0!=m_pDev->Open(0,0,704,576,5,16))
#endif
#ifdef TEST_DC
	{
		m_pDev=VIDEC_DC::Create();
		if (m_pDev!=NULL)
		{
			if (m_pMemDC!=NULL)
			{
				delete m_pMemDC;
				m_pMemDC=NULL;
			}
			if (m_pDC!=NULL)
			{
				ReleaseDC(m_pDC);
				m_pDC=NULL;
			}

			m_pDC=GetDC();
			CRect rc(0,0,704,576);
			m_pMemDC=new CMemDC(m_pDC,rc);

			m_pDCRecord=VIDEC_DCRecord::Create();
			if (m_pDCRecord!=NULL)
			{
				if (0!=m_pDCRecord->Open(m_pMemDC->m_hDC,0,0,704,576,"d:\\dcr.avi"))
				{
					m_pDCRecord->Close();
					delete m_pDCRecord;
					m_pDCRecord=NULL;
				}
			}

			if (0!=m_pDev->Open(m_pMemDC->m_hDC,0,0,704,576))
#endif
#ifdef TEST_MEDIAFILE
	{
		m_pDev=VIDEC_MediaFile::Create(*this);
		if (m_pDev!=NULL)
		{
			//const char*cszFileName="xms://192.168.7.217//opt/avcon/hprec/recfile//xf@xf/542@xf/av1@xf/0/NormalRecord/20120803/542@xf_0_201208030939363928179682.avms";
			//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.avi";
			//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.dat";
			const char*cszFileName="G:\\media_files\\AVMSVideo\\mpeg4-15s2.avms";

			if (0!=m_pDev->Open(m_pDlgCapVideo->m_hWnd,cszFileName,true))
#endif

			{
				m_pDev->Close();
				delete m_pDev;
				m_pDev=NULL;
			}
			else
			{
#ifdef TEST_MEDIAFILE
				//if (m_pDlgCapVideo)
				//{
				//	m_pDlgCapVideo->m_pMediaFile=m_pDev;
				//	m_pDlgCapVideo->ResizeControls();
				//}
				m_pDev->Seek(m_pDev->GetDuration()-30);
#endif
				
				//m_pDev->SetDenoise(0);
			}
#ifdef TEST_DC
			SetTimer(100,100,NULL);
#endif
		}
	}

	if (m_pDev!=NULL && 
#ifdef TEST_MEDIAFILE1
		1
#else
		m_pDlgCapVideo->m_pWnd!=NULL
#endif
		)
	{
		m_pDev->SetDenoise(1);
		//m_pDev->SetVideoParam(180);

		//m_pDev->SetVideoParam(128,64,64,0,100,5);

		//将采集到的数据送去显示
#ifdef TEST_MEDIAFILE1
#else
		m_pDev->AddImageSink(m_pDlgCapVideo->m_pWnd);
#endif
		//if (m_pDlgPlayVideo!=NULL && m_pDlgPlayVideo->m_pWnd!=NULL)
		//{
		//	m_pDev->AddImageSink(m_pDlgPlayVideo->m_pWnd);
		//}

		m_pDlgCapVideo->SetImageSource(m_pDev);

		m_pDev->EnableOSD(true);
		m_pDev->SetOSDText("北京天安门",VIDEC_OSD_ALIGN_TOP_CENTER_DOWN);

		//m_pDev->SetInvert(1);
		//int nValue=m_pDev->GetInvert();
#if 0
		if (m_pAVIFileWriter==NULL)
		{
			int nWidth=0;
			int nHeight=0;
			if (m_bEncode)
			{
				nWidth=640;
				nHeight=352;
			}
			m_pAVIFileWriter=new AVIFileWriter();
			if (!m_pAVIFileWriter->Open("d:\\first.avi",16000,nWidth,nHeight))
			{
				m_pAVIFileWriter->Close();
				delete m_pAVIFileWriter;
				m_pAVIFileWriter=NULL;
			}
		}
#endif

#ifdef TEST_MEDIAFILE1
		if (1)
#else
		if (0)
#endif
		{
#ifdef _AVIFILE
			m_pAVIWriter=VIDEC_AVIWriter::Create();
			if (0!=m_pAVIWriter->Open("d:\\first.avi",1))
			{
				m_pAVIWriter->Close();
				delete m_pAVIWriter;
				m_pAVIWriter=NULL;
			}
#else
			m_pASFWriter=AVCONEC_ASFWriter::Create();
			if (0!=m_pASFWriter->Open(L"d:\\videc_h264.asf",H264,m_nWidth,m_nHeight,m_nFrameRate,384000))
			{
				m_pASFWriter->Close();
				delete m_pASFWriter;
				m_pASFWriter=NULL;
			}
#endif
		}
	}


	if (m_bEncode)
	{
		if (m_bDecode)
		{
#ifdef _TEST_DECODER
			m_pDecoder=VIDEC_StreamDecoder::Create();
			if (m_pDecoder!=NULL)
			{
				if (0!=m_pDecoder->Open())
				{
					m_pDecoder->Close();
					delete m_pDecoder;
					m_pDecoder=NULL;
				}
			}
#else
#ifdef TEST_PLAYER
#ifdef TEST_DXVA
			m_pPlayer=AVCONAVC_Player::Create();
#else
			m_pPlayer=VIDEC_Player::Create(*this);
#endif
			if (m_pPlayer!=NULL)
			{
				if (0!=m_pPlayer->Open(m_pDlgPlayVideo->m_hWnd))
				{
					m_pPlayer->Close();
					delete m_pPlayer;
					m_pPlayer=NULL;
				}
				else
				{
					m_pDlgPlayVideo->m_pPlayer=m_pPlayer;
					m_pDlgPlayVideo->ResizeControls();
				}
			}
#else
			m_pPlayChan=VIDEC_PlayChan::Create();
			if (m_pPlayChan!=NULL)
			{
				if (0!=m_pPlayChan->Open())
				{
					m_pPlayChan->Close();
					delete m_pPlayChan;
					m_pPlayChan=NULL;
				}
				else
				{
					//将解码后的数据送去显示
					if (m_pDlgPlayVideo->m_pWnd!=NULL)
					{
						m_pPlayChan->AddImageSink(m_pDlgPlayVideo->m_pWnd);
					}
				}
			}
#endif
#endif
		}

		if (m_pDev!=NULL)
		{
#ifdef TEST_DVC
			//让压缩后的视频流回掉出来
			m_pDev->AddStreamSink(this);
#else
			m_pCapChan=VIDEC_CapChan::Create(*this);
			if (m_pCapChan!=NULL)
			{
				int nBitrateControlType=VIDEC_NETWORK_PRIVATE;
				if (0!=m_pCapChan->Open(m_nQuality,nBitrateControlType,m_CodecType,320,180))
				//if (0!=m_pCapChan->Open(m_nWidth,m_nHeight,m_nFrameRate,m_nQuality,m_CodecType,m_nFrameRate*8,0))
				{
					m_pCapChan->Close();
					delete m_pCapChan;
					m_pCapChan=NULL;
				}
				else
				{
					//将采集到的数据送去压缩
					m_pDev->AddImageSink(m_pCapChan);

					//让压缩后的视频流回掉出来
					m_pCapChan->AddStreamSink(this);
				}
			}
#endif
		}
	}

#endif



	ResizeControls();
	SetTimer(2000,2000,NULL);
}

void DlgMain::DoStop(void)
{
	DebugOutput( logcallstack ,"DlgMain::DoStop\n");
	KillTimer(2000);

#ifdef TEST_COREAVC
	if (g_pCoreAVCVideoPlayer!=NULL)
	{
		g_pCoreAVCVideoPlayer->ReleaseConnections();
		delete g_pCoreAVCVideoPlayer;
		g_pCoreAVCVideoPlayer=NULL;
	}
#endif

#ifdef TEST_RAW_FILE
	if (m_pReader)
	{
		m_pReader->Close();
		delete m_pReader;
		m_pReader=NULL;
	}
#endif

	if (m_pCapChan!=NULL)
	{
		DebugOutput( logcallstack ,"DlgMain CapChan Close 1\n");
		m_pCapChan->Close();
		delete m_pCapChan;
		DebugOutput( logcallstack ,"DlgMain CapChan Close 2\n");
		m_pCapChan=NULL;
	}

#ifdef _TEST_DECODER
	if (m_pDecoder!=NULL)
	{
		m_pDecoder->Close();
		delete m_pDecoder;
		m_pDecoder=NULL;
	}
#else
#ifdef TEST_PLAYER

	if (m_pDlgPlayVideo!=NULL)
	{
		m_pDlgPlayVideo->m_pPlayer=NULL;
	}

	if (m_pPlayer!=NULL)
	{
		DebugOutput( logcallstack ,"DlgMain Player Close 1\n");
		m_pPlayer->Close();
		delete m_pPlayer;
		DebugOutput( logcallstack ,"DlgMain Player Close 2\n");
		m_pPlayer=NULL;
	}
#else
	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->Close();
		delete m_pPlayChan;
		m_pPlayChan=NULL;
	}
#endif
#endif

#ifndef TEST_RAW_FILE
	if (m_pDev!=NULL)
	{
#ifdef TEST_MEDIAFILE1
		if (m_pDlgCapVideo)
		{
			m_pDlgCapVideo->m_pMediaFile=NULL;
		}
#endif

		DebugOutput( logcallstack ,"DlgMain Dev Close 1\n");
		m_pDev->Close();
		delete m_pDev;
		DebugOutput( logcallstack ,"DlgMain Dev Close 2\n");
		m_pDev=NULL;
	}
#endif


#ifdef _AVIFILE
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->Close();
		delete m_pAVIWriter;
		m_pAVIWriter=NULL;
	}
#else
	if (m_pASFWriter!=NULL)
	{
		m_pASFWriter->Close();
		delete m_pASFWriter;
		m_pASFWriter=NULL;
	}
#endif
#ifdef TEST_DC
	if (m_pDCRecord!=NULL)
	{
		m_pDCRecord->Close();
		delete m_pDCRecord;
		m_pDCRecord=NULL;
	}

	if (m_pMemDC!=NULL)
	{
		delete m_pMemDC;
		m_pMemDC=NULL;
	}
	if (m_pDC!=NULL)
	{
		ReleaseDC(m_pDC);
		m_pDC=NULL;
	}
#endif

	m_pDlgVideo=m_pDlgCapVideo;

	ResizeControls();
}

void DlgMain::DoSetting(void)
{
	DlgSetting dlgSetting(*this);
	dlgSetting.m_bEncode=m_bEncode;
	dlgSetting.m_bDecode=m_bDecode;
	if (IDOK==dlgSetting.DoModal())
	{

	}
}

void DlgMain::OnAttachedToStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void DlgMain::OnDetachedFromStreamSource(VIDEC_StreamSource*pStreamSource)
{

}

void DlgMain::OnReceivedStreamData(VIDEC_StreamSource*pStreamSource,unsigned char*pData,int nLen,bool bKeyFrame,int nWidth,int nHeight,int nHeaderLen)
{
	m_XBitrateVideo.Calculate(nLen);
	//static int nFrameCount=0;
	//nFrameCount++;
	//if (nFrameCount%500==499)
	//	return;
	//if (bKeyFrame)
	//{
	//	TRACE("KeyFrame %d\n",nFrameCount);
	//}
	//if (VIDEC_HEADER_EXT_GET_BFRAME(pData))
	//{
	//	DebugOutput("VIDEC_HEADER_EXT_GET_BFRAME\n");
	//	return;
	//}

#ifdef TEST_COREAVC
	if (!m_bDecode)
	{
		if (g_pCoreAVCVideoPlayer!=NULL)
		{
			g_pCoreAVCVideoPlayer->OnDeliver((char*)pData+nHeaderLen,nLen-nHeaderLen);
		}
	}
#endif


#ifdef _TEST_DECODER
	if (m_pDecoder!=NULL)
	{
		void*pYUVData=NULL;
		int nYUVWidth=0;
		int nYUVHeight=0;
		if (0==m_pDecoder->Decode(pData,nLen,&pYUVData,nYUVWidth,nYUVHeight))
		{
			if (m_pDlgPlayVideo->m_pWnd!=NULL)
			{
				m_pDlgPlayVideo->m_pWnd->OnReceivedImageData((unsigned char*)pYUVData,nYUVWidth*nYUVHeight*3/2,nYUVWidth,nYUVHeight,nYUVWidth,nYUVHeight,30,false);
			}
		}
	}
#else
#ifdef TEST_PLAYER
	if (m_pPlayer!=NULL)
	{
		m_pPlayer->PlayData(pData,nLen);
	}
#else
	if (m_pPlayChan!=NULL)
	{
		m_pPlayChan->OnReceivedStreamData(NULL,pData,nLen,bKeyFrame,nWidth,nHeight,nHeaderLen);
	}
#endif
#endif

#ifdef _AVIFILE
	if (m_pAVIWriter!=NULL)
	{
		//m_pAVIWriter->Write(pData+nHeaderLen,nLen-nHeaderLen,bKeyFrame);
		//m_pAVIWriter->WriteVideo(pData,nLen);
	}
#else
	if (m_pASFWriter!=NULL)
	{
		//m_pASFWriter->WriteVideo(pData,nLen,bKeyFrame,GetTickCount());
		m_pASFWriter->WriteVideo(pData+nHeaderLen,nLen-nHeaderLen,bKeyFrame,GetTickCount());
	}
#endif
}

#ifdef TEST_RAW_FILE
void DlgMain::OnReaderCallbackFrameData(const unsigned char*pFrameData,int nFrameLen)
{
#ifdef TEST_PLAYER
	if (m_pPlayer!=NULL)
	{
		m_pPlayer->PlayData((void*)pFrameData,nFrameLen);
	}
#endif
}
#endif

#ifdef TEST_MEDIAFILE
void DlgMain::OnVIDEC_MediaFileCallbackPCMData(VIDEC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,unsigned long ulTimestamp)
{
	//static unsigned long ulLastTimestamp=ulTimestamp;
	//if (ulTimestamp-ulLastTimestamp<5000)
	//	return ;

#ifdef _AVIFILE
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->WriteAudio(pPCMData,nLen,32000,ulTimestamp);
	}
#endif

}
#endif

void DlgMain::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent==2000)
	{
		int nBitrate=0;
		double dblFPS=0.00;
		int nWidth=0;
		int nHeight=0;

		nBitrate=m_XBitrateVideo.GetBitrate();

		//static int nGamma=1;
		//if (m_pDev!=NULL)
		//{
		//	m_pDev->SetVideoParam(128,64,64,0,nGamma);
		//	nGamma+=10;
		//	if (nGamma>400)
		//		nGamma=1;
		//}

#ifndef _TEST_DECODER
#ifndef TEST_PLAYER
		if (m_pPlayChan!=NULL)
		{
			nBitrate=m_pPlayChan->GetAverageKBPS();
		}
#endif
#endif

#ifndef TEST_PLAYER
		if (m_pDlgPlayVideo!=NULL && m_pPlayChan!=NULL)
		{
			if (m_pDlgPlayVideo!=NULL)
			{
				if (m_pDlgPlayVideo->m_pWnd!=NULL)
				{
					nWidth=m_pDlgPlayVideo->m_pWnd->GetVideoWidth();
					nHeight=m_pDlgPlayVideo->m_pWnd->GetVideoHeight();
					dblFPS=m_pDlgPlayVideo->m_pWnd->GetAverageFPS();
				}
			}
		}
		else
#endif
		{
			if (m_pDlgCapVideo!=NULL)
			{
#ifdef TEST_MEDIAFILE1

#else
				if (m_pDlgCapVideo->m_pWnd!=NULL)
				{
					nWidth=m_pDlgCapVideo->m_pWnd->GetVideoWidth();
					nHeight=m_pDlgCapVideo->m_pWnd->GetVideoHeight();
					dblFPS=m_pDlgCapVideo->m_pWnd->GetAverageFPS();
				}
#endif
			}
		}


		CString strText;
		strText.Format("TestAH2K [%d X %d] [%.2f fps] [%d kbps]",nWidth,nHeight,dblFPS,nBitrate);

#ifdef TEST_SCREEN
		static int nCount=0;
		if (nCount++%10==0)
		{
			if (nCount>100)
				nCount=0;
			if (m_pDev!=NULL)
			{
				int nBitCount=(((nCount/10 +1)%3)+1)*8;
				if (0!=m_pDev->Reset(nCount,0,704,576,5,nBitCount))
				{
				}
			}
		}
#endif

		SetWindowText(strText);

		//DoStop();
		//DoStart();
	}

#ifdef TEST_DC
	else if (nIDEvent==100)
	{
		if (m_pDev && m_pMemDC)
		{
			static int nRGBIndex=0;
			CRect rc(0,0,704,576);
			m_pMemDC->FillRect(rc,&CBrush(RGB((nRGBIndex%3==0?255:0),0(nRGBIndex%1==0?255:0),(nRGBIndex%3==2?255:0))));
			nRGBIndex++;
			m_pDev->RequestFrame();

			if (m_pDCRecord!=NULL)
			{
				m_pDCRecord->CaptureFrame();
			}
		}
	}
#endif

	CDialog::OnTimer(nIDEvent);
}
