// DlgMain.cpp : 实现文件
//

#include "stdafx.h"

#include "TestDecoder.h"
#include "DlgMain.h"
#include "DlgSetting.h"
#include "XUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// DlgMain 对话框
DlgMain::DlgMain(CWnd* pParent /*=NULL*/)
	: CDialog(DlgMain::IDD, pParent)
	, m_bEncode(FALSE)
	, m_bDecode(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pDlgFullScreen=NULL;
	m_pDlgCapVideo=NULL;
	m_pDlgDecoder=NULL;
	m_pDlgBar=NULL;


	m_pCapChan=NULL;


	m_nWidth=0;
	m_nHeight=0;
	m_nFrameRate=15;
	
	m_nFormat=0;

	m_nDevID=0;
	m_nQuality=2;
	m_CodecType=VIDEC_CODEC_H264_SVC;

	m_nDecoderCount=1;

	m_nLastSkipTimestamp=0;

#ifdef TEST_IPCAMERA
	m_pIPCamera=NULL;
#else
	m_pDev=NULL;
#endif

#ifdef TEST_DC
	m_pMemDC=NULL;
	m_pDC=NULL;
	m_pDCRecord=NULL;
#endif

#ifdef TEST_MJPG
	m_pMJPGEncoder=NULL;
	m_pMJPGDecoder=NULL;
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

	VIDEC_Init();

	CRect rcWnd;
	GetWindowRect(rcWnd);
	CRect rcClient;
	GetClientRect(rcClient);

	SetWindowPos(NULL,0,0,960+(rcWnd.Width()-rcClient.Width()),720+(rcWnd.Height()-rcClient.Height()),0);
	CenterWindow(NULL);

	m_pDlgBar=new DlgBar(*this);
	m_pDlgBar->Create(DlgBar::IDD,this);
	m_pDlgCapVideo=new DlgCapVideo(*this);
	m_pDlgCapVideo->Create(DlgCapVideo::IDD,this);

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

	DoStop();

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

	if (m_pDlgDecoder!=NULL)
	{
		m_pDlgDecoder->DestroyWindow();
		delete m_pDlgDecoder;
		m_pDlgDecoder=NULL;
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
		m_pDlgCapVideo->SetWindowPos(NULL,0,BAR_HEIGHT,rc.Width(),rc.Height()-BAR_HEIGHT,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}

void DlgMain::SetFullScreen(void)
{
	if (m_pDlgFullScreen!=NULL)
	{
		if (m_pDlgCapVideo!=NULL)
		{
			m_pDlgCapVideo->SetParent(this);
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
		m_pDlgFullScreen->SetWindowPos(NULL,0,0,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);

		m_pDlgFullScreen->SetWndChild(m_pDlgCapVideo);
	}
}

void DlgMain::DoStart(void)
{
	int nDevID=m_nDevID;

#ifdef TEST_IPCAMERA
	m_pIPCamera=VIDEC_IPCamera::Create(*this);
	if (m_pIPCamera!=NULL)
	{
		int nBitrate=768;
		int nBitrateControlType=VIDEC_BITRATE_CONTROL_CBR;
		VIDEC_CODEC_TYPE nCodecType=VIDEC_CODEC_H264;
		int nQMin=2;
		int nQMax=51;
		GetCodecInfo(m_nQuality,nBitrateControlType,nBitrate,nQMin,nQMax,nCodecType,false);

		if (m_pIPCamera->Open("192.168.5.246",m_nWidth=1280,m_nHeight=720,m_nFrameRate,nBitrate,0,0,"sanyo","admin","admin")!=0)
		//if (m_pIPCamera->Open("192.168.2.71",m_nWidth=1280,m_nHeight=720,m_nFrameRate,nBitrate,0,0,"alk","admin","admin")!=0)
		{
			m_pIPCamera->Close();
			delete m_pIPCamera;
			m_pIPCamera=NULL;
		}
	}
#else

#ifdef TEST_AH2K
	if (nDevID<VIDEC_AH2KDev::GetDevCount())
	{
		m_pDev=VIDEC_AH2KDev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_AH2KDev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
			if (0!=m_pDev->Open(nDevID,m_nWidth,m_nHeight,m_nFrameRate,true))
#endif
#ifdef TEST_CAMERA
	if (nDevID<VIDEC_CameraDev::GetDevCount())
	{
		m_pDev=VIDEC_CameraDev::Create();
		if (m_pDev!=NULL)
		{
			VIDEC_CameraDev::GetFormatSize(m_nFormat,m_nWidth,m_nHeight);
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
			if (0!=m_pDev->Open("F:\\media_files\\again英国版.wmv",true))
#endif

			{
				m_pDev->Close();
				delete m_pDev;
				m_pDev=NULL;
			}
			else
			{
#ifdef TEST_MEDIAFILE
				m_pDev->Seek(m_pDev->GetDuration()*4/5);
#endif
				
				//m_pDev->SetDenoise(0);
			}
#ifdef TEST_DC
			SetTimer(100,100,NULL);
#endif
		}
	}

	if (m_pDev!=NULL && m_pDlgCapVideo->m_pWnd!=NULL)
	{
#ifdef TEST_MJPG
		m_pMJPGEncoder=VIDEC_MJPGEncoder::Create(*this);
		if (m_pMJPGEncoder!=NULL)
		{
			if (0!=m_pMJPGEncoder->Open())
			{
				m_pMJPGEncoder->Close();
				delete m_pMJPGEncoder;
				m_pMJPGEncoder=NULL;
			}
			else
			{
				m_pMJPGDecoder=VIDEC_MJPGDecoder::Create(*this);
				if (m_pMJPGDecoder!=NULL)
				{
					if (0!=m_pMJPGDecoder->Open())
					{
						m_pMJPGDecoder->Close();
						delete m_pMJPGDecoder;
						m_pMJPGDecoder=NULL;
					}
					else
					{
					}
				}

				m_pDev->AddImageSink(m_pMJPGEncoder);
			}
		}

		if (m_pMJPGDecoder!=NULL)
		{

		}
		else
		{
			//将采集到的数据送去显示
			m_pDev->AddImageSink(m_pDlgCapVideo->m_pWnd);
		}
#else
		//将采集到的数据送去显示
		m_pDev->AddImageSink(m_pDlgCapVideo->m_pWnd);
#endif
	}
#endif


	if (m_bEncode)
	{
		if (m_bDecode)
		{
			m_pDlgDecoder=new DlgDecoder(this);
			m_pDlgDecoder->m_nDecoderCount=m_nDecoderCount;
			m_pDlgDecoder->Create(DlgDecoder::IDD,this);

			int cx = ::GetSystemMetrics(SM_CXSCREEN);
			int cy = ::GetSystemMetrics(SM_CYSCREEN);
			m_pDlgDecoder->SetWindowPos(NULL,0,0,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}

#ifndef TEST_IPCAMERA
		if (m_pDev!=NULL)
		{
#ifdef TEST_AH2K
			if (VIDEC_AH2KDev::SupportHardwareEncoding(nDevID))
			{
				int nBitrate=768;
				int nBitrateControlType=VIDEC_BITRATE_CONTROL_CBR;
				VIDEC_CODEC_TYPE nCodecType=VIDEC_CODEC_H264;
				int nQMin=2;
				int nQMax=51;
				GetCodecInfo(m_nQuality,nBitrateControlType,nBitrate,nQMin,nQMax,nCodecType,false);

				m_pDev->AddStreamSink(this);
				m_pDev->StartVideoStream(nBitrate,nBitrateControlType,nQMin,nQMax);
			}
			else
			{
				m_pCapChan=VIDEC_CapChan::Create(*this);
				if (m_pCapChan!=NULL)
				{
					int nBitrateControlType=VIDEC_BITRATE_CONTROL_CBR;

					if (0!=m_pCapChan->Open(2048,nBitrateControlType,m_CodecType,0))
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

						m_pDev->SetVideoParam(128,64,72,0,100,12);

						//让压缩后的视频流回掉出来
						m_pCapChan->AddStreamSink(this);
					}
				}
			}
#else
			m_pCapChan=VIDEC_CapChan::Create(*this);
			if (m_pCapChan!=NULL)
			{
				int nBitrateControlType=VIDEC_BITRATE_CONTROL_CBR;
				if (0!=m_pCapChan->Open(2048,nBitrateControlType,m_CodecType,0))
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

					m_pDev->SetVideoParam(128,64,72,0,100,12);

					//让压缩后的视频流回掉出来
					m_pCapChan->AddStreamSink(this);
				}
			}
#endif

		}
#endif
	}


#ifndef TEST_IPCAMERA
	if (m_pDev!=NULL)
	{
		//将采集到的数据送去显示
		m_pDev->AddImageSink(this);
	}
#endif

	ResizeControls();
	SetTimer(2000,2000,NULL);
}

void DlgMain::DoStop(void)
{
	KillTimer(2000);


	if (m_pCapChan!=NULL)
	{
		m_pCapChan->Close();
		delete m_pCapChan;
		m_pCapChan=NULL;
	}

#ifdef TEST_MJPG
	if (m_pMJPGEncoder!=NULL)
	{
		m_pMJPGEncoder->Close();
		delete m_pMJPGEncoder;
		m_pMJPGEncoder=NULL;
	}
	if (m_pMJPGDecoder!=NULL)
	{
		m_pMJPGDecoder->Close();
		delete m_pMJPGDecoder;
		m_pMJPGDecoder=NULL;
	}
#endif

	if (m_pDlgDecoder!=NULL)
	{
		m_pDlgDecoder->DestroyWindow();
		delete m_pDlgDecoder;
		m_pDlgDecoder=NULL;
	}


#ifdef TEST_IPCAMERA
	if (m_pIPCamera!=NULL)
	{
		m_pIPCamera->Close();
		delete m_pIPCamera;
		m_pIPCamera=NULL;
	}
#else

	if (m_pDev!=NULL)
	{
		m_pDev->Close();
		delete m_pDev;
		m_pDev=NULL;
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


	if (m_pDlgDecoder!=NULL)
	{
		m_pDlgDecoder->PlayFramePacket(pData,nLen);
	}
}

#ifdef TEST_MEDIAFILE
void DlgMain::OnVIDEC_MediaFileCallbackPCMData(VIDEC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,unsigned long ulTimestamp)
{
	static unsigned long ulLastTimestamp=ulTimestamp;
	if (ulTimestamp-ulLastTimestamp<5000)
		return ;
#ifdef _AVIFILE
	if (m_pAVIWriter!=NULL)
	{
		m_pAVIWriter->WriteAudio(pPCMData,nLen,16000,ulTimestamp);
	}
#endif



	if (m_pAVIFileWriter!=NULL)
	{
		m_pAVIFileWriter->WriteAudio(pPCMData,nLen,ulTimestamp);
	}
}
#endif

void DlgMain::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==2000)
	{
		int nBitrate=0;
		double dblFPS=0.00;
		int nWidth=0;
		int nHeight=0;

		nBitrate=m_XBitrateVideo.GetBitrate();

		{
			if (m_pDlgCapVideo!=NULL)
			{
				if (m_pDlgCapVideo->m_pWnd!=NULL)
				{
					nWidth=m_pDlgCapVideo->m_pWnd->GetVideoWidth();
					nHeight=m_pDlgCapVideo->m_pWnd->GetVideoHeight();
					dblFPS=m_pDlgCapVideo->m_pWnd->GetAverageFPS();
				}
			}
		}


		CString strText;
		strText.Format("TestDecoder [%d X %d] [%.2f fps] [%d kbps]",nWidth,nHeight,dblFPS,nBitrate);

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
void DlgMain::OnReceivedPreSend(int& nSend)
{
}
void DlgMain::OnReceivedImage(VIDEC_Image*pImage)
{
}

void DlgMain::OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag)
{
	nSkipFlag=0;
	//unsigned long nTimestamp=XGetTimestamp();
	//if (nTimestamp-m_nLastSkipTimestamp>=1000)
	//{
	//	nSkipFlag=0;
	//	m_nLastSkipTimestamp=nTimestamp;
	//}
	//else
	//{
	//	nSkipFlag=1;
	//}
}

#ifdef TEST_IPCAMERA
void DlgMain::OnVIDEC_IPCameraCallbackVideoFramePacketMain(unsigned char*pPacketData,int nPacketLen)
{
#ifndef TEST_IPCAMERA_RTP
	if (m_pDlgDecoder!=NULL)
	{
		m_pDlgDecoder->PlayFramePacket(pPacketData,nPacketLen);
	}
#endif
}

void DlgMain::OnVIDEC_IPCameraCallbackVideoFramePacketSub(unsigned char*pPacketData,int nPacketLen)
{

}

void DlgMain::OnVIDEC_IPCameraCallbackVideoFramePacketQSub(unsigned char*pPacketData,int nPacketLen)
{

}
void DlgMain::OnVIDEC_IPCameraCallbackVideoRTPPacketMain(unsigned char*pPacketData,int nPacketLen)
{
#ifdef TEST_IPCAMERA_RTP
	if (m_pDlgDecoder!=NULL)
	{
		m_pDlgDecoder->PlayRTPPacket(pPacketData,nPacketLen);
	}
#endif
}

void DlgMain::OnVIDEC_IPCameraCallbackVideoRTPPacketSub(unsigned char*pPacketData,int nPacketLen)
{

}

void DlgMain::OnVIDEC_IPCameraCallbackVideoRTPPacketQSub(unsigned char*pPacketData,int nPacketLen)
{

}
#endif

#ifdef TEST_MJPG
void DlgMain::OnVIDEC_MJPGEncoderCallbackStreamPacket(VIDEC_MJPGEncoder*pMJPGEncoder,unsigned char*pPacketData,int nPacketLen)
{
	if (m_pMJPGDecoder!=NULL)
	{
		m_pMJPGDecoder->InputPacketData(pPacketData,nPacketLen);
	}
}

void DlgMain::OnVIDEC_MJPGEncoderCallbackPreEncode(VIDEC_MJPGEncoder*pMJPGEncoder,int&nSkip)
{
	nSkip=0;
}

void DlgMain::OnVIDEC_MJPGDecoderCallbackImage(VIDEC_MJPGDecoder*pMJPGEncoder,VIDEC_Image*pImage)
{
	if (m_pDlgCapVideo!=NULL && m_pDlgCapVideo->m_pWnd!=NULL)
	{
		m_pDlgCapVideo->m_pWnd->OnReceivedImage(pImage);
	}
}
#endif