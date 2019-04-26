// DlgDecoder.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDecoder.h"
#include "DlgDecoder.h"


// DlgDecoder 对话框

IMPLEMENT_DYNAMIC(DlgDecoder, CDialog)

DlgDecoder::DlgDecoder(CWnd* pParent /*=NULL*/)
	: CDialog(DlgDecoder::IDD, pParent)
{
	for (int i=0;i<MAX_PLAYER;i++)
	{
		m_apDlgPlayVideo[i]=NULL;
	}

	m_pWndVideo=NULL;
	m_nRows=1;

	m_pDlgFullScreen=NULL;
}

DlgDecoder::~DlgDecoder()
{
}

void DlgDecoder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgDecoder, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// DlgDecoder 消息处理程序

void DlgDecoder::OnClose()
{
}

BOOL DlgDecoder::DestroyWindow()
{
	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (m_apDlgPlayVideo[i]!=NULL)
		{
			m_apDlgPlayVideo[i]->DestroyWindow();
			delete m_apDlgPlayVideo[i];
			m_apDlgPlayVideo[i]=NULL;
		}
	}

	if (m_pDlgFullScreen!=NULL)
	{
		m_pDlgFullScreen->DestroyWindow();
		delete m_pDlgFullScreen;
		m_pDlgFullScreen=NULL;
	}

	return CDialog::DestroyWindow();
}

BOOL DlgDecoder::OnInitDialog()
{
	CDialog::OnInitDialog();

	int nWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nHeight = ::GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL,0,0,nWidth,nHeight,SWP_SHOWWINDOW|SWP_NOACTIVATE);

	if (m_nDecoderCount>MAX_PLAYER)
	{
		m_nDecoderCount=MAX_PLAYER;
	}

	m_nRows=1;
	while (m_nRows*m_nRows<m_nDecoderCount){m_nRows++;};

	CRect rc;
	GetClientRect(rc);
	int x=0;
	int y=0;
	int cx=rc.Width()/m_nRows;
	int cy=rc.Height()/m_nRows;

	for (int i=0;i<m_nDecoderCount;i++)
	{
		DlgPlayVideo*pDlgPlayVideo=new DlgPlayVideo(*this);
		pDlgPlayVideo->Create(DlgPlayVideo::IDD,this);
		pDlgPlayVideo->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);

		m_apDlgPlayVideo[i]=pDlgPlayVideo;

		x+=cx;
		if (i%m_nRows==(m_nRows-1))
		{
			x=0;
			y+=cy;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void DlgDecoder::PlayFramePacket(const void*pPacketData,int nPacketLen)
{
	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (m_apDlgPlayVideo[i]!=NULL)
		{
			m_apDlgPlayVideo[i]->PlayFramePacket(pPacketData,nPacketLen);
		}
	}
}

void DlgDecoder::PlayRTPPacket(const void*pPacketData,int nPacketLen)
{
	for (int i=0;i<MAX_PLAYER;i++)
	{
		if (m_apDlgPlayVideo[i]!=NULL)
		{
			m_apDlgPlayVideo[i]->PlayRTPPacket(pPacketData,nPacketLen);
		}
	}
}

void DlgDecoder::SetFullScreen(CWnd*pWndVideo)
{
	if (m_pDlgFullScreen!=NULL)
	{
		if (m_pWndVideo!=NULL)
		{
			m_pWndVideo->SetParent(this);
			m_pWndVideo=NULL;
		}


		CRect rc;
		GetClientRect(rc);
		int x=0;
		int y=0;
		int cx=rc.Width()/m_nRows;
		int cy=rc.Height()/m_nRows;

		for (int i=0;i<m_nDecoderCount;i++)
		{
			DlgPlayVideo*pDlgPlayVideo=m_apDlgPlayVideo[i];

			if (pDlgPlayVideo!=NULL)
				pDlgPlayVideo->SetWindowPos(NULL,x,y,cx,cy,SWP_SHOWWINDOW|SWP_NOACTIVATE);

			x+=cx;
			if (i%m_nRows==(m_nRows-1))
			{
				x=0;
				y+=cy;
			}
		}

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

		m_pWndVideo=pWndVideo;
		m_pDlgFullScreen->SetWndChild(pWndVideo);
	}
}
