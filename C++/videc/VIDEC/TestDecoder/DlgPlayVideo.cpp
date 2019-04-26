// DlgPlayVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "TestDecoder.h"
#include "DlgPlayVideo.h"
#include "DlgDecoder.h"


// DlgPlayVideo 对话框

IMPLEMENT_DYNAMIC(DlgPlayVideo, CDialog)
DlgPlayVideo::DlgPlayVideo(DlgDecoder&rDlgDecoder)
	: CDialog(DlgPlayVideo::IDD, &rDlgDecoder)
	, m_pPlayer(NULL)
	, m_rDlgDecoder(rDlgDecoder)
{
}

DlgPlayVideo::~DlgPlayVideo()
{
}

void DlgPlayVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgPlayVideo, CDialog)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// DlgPlayVideo 消息处理程序

void DlgPlayVideo::OnOK()
{
}

void DlgPlayVideo::OnCancel()
{
}

BOOL DlgPlayVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pPlayer=VIDEC_Player::Create();
	if (m_pPlayer!=NULL)
	{
		if (0!=m_pPlayer->Open(m_hWnd))
		{
			m_pPlayer->Close();
			delete m_pPlayer;
			m_pPlayer=NULL;
		}
	}

	ResizeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgPlayVideo::DestroyWindow()
{
	if (NULL!=m_pPlayer)
	{
		m_pPlayer->Close();
		delete m_pPlayer;
		m_pPlayer=NULL;
	}

	return CDialog::DestroyWindow();
}

void DlgPlayVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgPlayVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rDlgDecoder.SetFullScreen(this);

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgPlayVideo::ResizeControls(void)
{
	if (m_pPlayer!=NULL)
	{
		CRect rc;
		GetClientRect(rc);
		m_pPlayer->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
}


void DlgPlayVideo::PlayFramePacket(const void*pPacketData,int nPacketLen)
{
	if (m_pPlayer!=NULL)
	{
		m_pPlayer->PlayData((void*)pPacketData,nPacketLen);
	}
}

void DlgPlayVideo::PlayRTPPacket(const void*pPacketData,int nPacketLen)
{
	//if (m_pPlayChanRTP!=NULL)
	//{
	//	m_pPlayChanRTP->OnRecvdRTPPacket((unsigned char*)pPacketData,nPacketLen);
	//}
}

