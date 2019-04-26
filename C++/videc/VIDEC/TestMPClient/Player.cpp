// Player.cpp : 实现文件
//

#include "stdafx.h"
#include "TestMPClient.h"
#include "Player.h"


// CPlayer 对话框

IMPLEMENT_DYNAMIC(CPlayer, CDialog)

CPlayer::CPlayer(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayer::IDD, pParent)
{
	m_pCap = NULL;
	m_pPlay = NULL;
}

CPlayer::~CPlayer()
{
}

void CPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayer, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPlayer 消息处理程序

void CPlayer::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (NULL != m_pPlay)
	{
		m_pPlay->SetWindowPos(NULL, 0, 0, cx, cy, SWP_SHOWWINDOW);
	}
	else if (NULL != m_pCap)
	{
		m_pCap->SetWindowPos(NULL, 0, 0, cx, cy, SWP_SHOWWINDOW);
	}
}

void CPlayer::SetHwnd(IMXEC_PlayChan* play, IMXEC_CapChanDev* cap)
{
	m_pCap = cap; 
	m_pPlay = play;
	CString strTitle;
	if (NULL != m_pPlay)
	{
		strTitle = _T("Player");
	}
	else if (NULL != m_pCap)
	{
		strTitle = _T("Capture");
	}
	SetWindowText(strTitle);
}
