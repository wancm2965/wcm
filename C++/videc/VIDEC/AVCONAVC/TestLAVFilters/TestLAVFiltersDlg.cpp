
// TestLAVFiltersDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TestLAVFilters.h"
#include "TestLAVFiltersDlg.h"
#include <MMSystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestLAVFiltersDlg 对话框

class XSocketInitializer  
{
public:
	XSocketInitializer(void)
	{
		WORD wVersion; 
		WSADATA wsaData;
		wVersion = MAKEWORD(2,2);
		WSAStartup(wVersion,&wsaData);
	}
	virtual ~XSocketInitializer(void)
	{
		WSACleanup(); 
	}
};


XSocketInitializer g_XSocketInitializer;


CTestLAVFiltersDlg::CTestLAVFiltersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestLAVFiltersDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pMediaFile=NULL;
	m_pDlgVideo=NULL;
	m_bTryEVR=true;
#ifdef __AVPULLPLAYER_H__
	m_pIAVPullPlayer=NULL;
#endif
}

void CTestLAVFiltersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestLAVFiltersDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CTestLAVFiltersDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTestLAVFiltersDlg 消息处理程序

BOOL CTestLAVFiltersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//const char*cszFileName="E:\\again.wmv";
	//const char*cszFileName="G:\\media_files\\胡锦涛讲话.rmvb";
	//const char*cszFileName="G:\\media_files\\100MB.avi";
	//const char*cszFileName="G:\\media_files\\MVI_4033.avi";
	//const char*cszFileName="G:\\media_files\\录像17.avi";
	//const char*cszFileName="G:\\media_files\\The_Magic_of_Flight_1080.wmv";
	//const char*cszFileName="G:\\media_files\\huapin.mpg";
	//const char*cszFileName="G:\\media_files\\wmv\\1.wmv";
	//const char*cszFileName="G:\\media_files\\wmv\\again-yingwenban.wmv";
	//const char*cszFileName="G:\\media_files\\rm\\小mm辣舞.rm";
	//const char*cszFileName="G:\\media_files\\Kung.Fu.Panda.2.2011.BluRay.1080p.DTS.x264-CHD.mkv";
	//const char*cszFileName="G:\\media_files\\M4V\\Justin.Bieber.-.[Baby].MV.(DVDRip).m4v";
	//const char*cszFileName="G:\\tong_shi_files\\maozongwu\\pad_720p_stream.3gp";
	const char*cszFileName="G:\\media_files\\avi\\WBrecord(长).avi";

	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\Kung.Fu.Panda.2.2011.BluRay.1080p.DTS.x264-CHD.mkv";
	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\wmv\\1.wmv";
	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\vob\\黑客帝国.VOB";
	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\vob\\人寿dvd片头.vob";
	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\rm\\小mm辣舞.rm";
	//const char*cszFileName="Xms://192.168.2.111/G:\\media_files\\胡锦涛讲话.rmvb";

	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.avi";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mkv";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.dat";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/again.wmv";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mp3";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.asf";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.flv";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.m4v";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mov";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mp4";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.mpg";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.rm";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.ts";
	//const char*cszFileName="xms://192.168.7.217//opt/avcon/media_files/1.vob";

#ifdef __AVPULLPLAYER_H__
	m_pIAVPullPlayer=CreateAVPullPlayer(m_hWnd,0,cszFileName);
	if (m_pIAVPullPlayer)
	{
		CRect rc;
		GetClientRect(rc);
		m_pIAVPullPlayer->SetWindowPos(0,0,rc.Width(),rc.Height());
		//m_pIAVPullPlayer->Seek(m_pIAVPullPlayer->GetDuration()*0.9);
	}
#else
	m_pDlgVideo=new DlgVideo();
	m_pDlgVideo->Create(DlgVideo::IDD);
	m_pDlgVideo->ShowWindow(SW_SHOW);

	m_pMediaFile=AVCONAVC_MediaFile::Create(*this);
	if (m_pMediaFile)
	{
		if (0!=m_pMediaFile->Open(m_hWnd,cszFileName,m_bTryEVR))
		//if (0!=m_pMediaFile->Open(m_hWnd,"G:\\media_files\\scene.passby.mkv",true,false,true))
		//if (0!=m_pMediaFile->Open(m_hWnd,"G:\\media_files\\Kung.Fu.Panda.2.2011.BluRay.1080p.DTS.x264-CHD.mkv",true))
		//if (0!=m_pMediaFile->Open(m_hWnd,"G:\\media_files\\wmv\\again-yingwenban.wmv",true,false,true))
		{
			m_pMediaFile->Close();
			delete m_pMediaFile;
			m_pMediaFile=NULL;
		}
		else
		{
			CRect rc;
			GetClientRect(rc);
			m_pMediaFile->SetWndPos(0,0,rc.Width(),rc.Height());

			//m_pMediaFile->Seek(m_pMediaFile->GetDuration()/2);
			//m_pMediaFile->Play();
		}
	}
#endif

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestLAVFiltersDlg::OnPaint()
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
HCURSOR CTestLAVFiltersDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestLAVFiltersDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_pMediaFile)
	{
		CRect rc;
		GetClientRect(rc);
		m_pMediaFile->SetWndPos(0,0,rc.Width(),rc.Height());
	}
#ifdef __AVPULLPLAYER_H__
	if (m_pIAVPullPlayer)
	{
		CRect rc;
		GetClientRect(rc);
		m_pIAVPullPlayer->SetWindowPos(0,0,rc.Width(),rc.Height());
	}
#endif
}

BOOL CTestLAVFiltersDlg::DestroyWindow()
{
#ifdef __AVPULLPLAYER_H__
	if (m_pIAVPullPlayer)
	{
		m_pIAVPullPlayer->ReleaseConnections();
		delete m_pIAVPullPlayer;
		m_pIAVPullPlayer=NULL;
	}
#endif

	if (m_pMediaFile)
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;
	}

	if( m_pDlgVideo )
	{
		m_pDlgVideo->DestroyWindow();
		delete m_pDlgVideo;
		m_pDlgVideo=NULL;
	}

	return CDialog::DestroyWindow();
}

void CTestLAVFiltersDlg::OnAVCONAVC_MediaFileCallbackPCMData(AVCONAVC_MediaFile*pMediaFile,unsigned char*pPCMData,int nLen,int nSampleRate,unsigned long nTimestamp)
{

}

void CTestLAVFiltersDlg::OnAVCONAVC_MediaFileCallbackYV12Buffer(unsigned char**ppBuffer,int nBufferLen)
{
	*ppBuffer=(unsigned char*)_aligned_malloc(nBufferLen+nBufferLen/4,128);
}

void CTestLAVFiltersDlg::OnAVCONAVC_MediaFileCallbackYV12Data(AVCONAVC_MediaFile*pMediaFile,unsigned char*pData,int nLen,int nWidth,int nHeight,int nFrameRate,unsigned long nTimestamp)
{
	if (m_pDlgVideo)
	{
		if (m_pDlgVideo->m_pVideoDraw==NULL)
		{
			m_pDlgVideo->m_pVideoDraw=AVCONAVC_VideoDraw::Create();
			if (m_pDlgVideo->m_pVideoDraw)
			{
				if (m_pDlgVideo->m_pVideoDraw->Open(m_pDlgVideo->m_hWnd,nWidth,nHeight,false,m_bTryEVR)!=0)
				{
					m_pDlgVideo->m_pVideoDraw->Close();
					delete m_pDlgVideo->m_pVideoDraw;
					m_pDlgVideo->m_pVideoDraw=NULL;
				}
			}
		}

		if( m_pDlgVideo->m_pVideoDraw )
		{
			m_pDlgVideo->m_pVideoDraw->Draw(pData,nWidth);
		}
	}
	_aligned_free(pData);
}


void CTestLAVFiltersDlg::OnBnClickedButton1()
{
#ifdef __AVPULLPLAYER_H__
	if (m_pIAVPullPlayer)
	{
		m_pIAVPullPlayer->ReleaseConnections();
		delete m_pIAVPullPlayer;
		m_pIAVPullPlayer=NULL;
	}
#endif

	if (m_pMediaFile)
	{
		m_pMediaFile->Close();
		delete m_pMediaFile;
		m_pMediaFile=NULL;
	}

	if( m_pDlgVideo )
	{
		m_pDlgVideo->DestroyWindow();
		delete m_pDlgVideo;
		m_pDlgVideo=NULL;
	}
}
