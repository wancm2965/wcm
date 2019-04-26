// VideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "VideoWnd.h"
#include "afxdialogex.h"
#include "Demo_win32Dlg.h"


bool		g_bStartNode = false;

// CVideoWnd 对话框

IMPLEMENT_DYNAMIC(CVideoWnd, CDialogEx)

CVideoWnd::CVideoWnd(CWnd* pParent /*=NULL*/,bool bUserHPAVC)
	: CDialogEx(CVideoWnd::IDD, pParent)
	,m_pIAVReceiver(NULL)
	,m_pParentWnd(pParent)
#if USE_HPAVC
	,m_pHPAVC_Player(NULL)
#else
	,m_pVIDEC_Player(NULL)
#endif
	,m_pFile(NULL)
	,m_nLBDbClickTimes(0)
{
#if USE_HPAVC
	m_pHPAVC_Player = HPAVC_Player::Create(this);
#else
	m_pVIDEC_Player = VIDEC_Player::Create(*this);
	//m_pVIDEC_Player->SetVideoSkipFrame(1,1);
#endif
}

CVideoWnd::~CVideoWnd()
{
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		delete m_pHPAVC_Player;
		m_pHPAVC_Player = NULL;
	}
#else
	if (m_pVIDEC_Player)
	{
		delete m_pVIDEC_Player;
		m_pVIDEC_Player = NULL;
	}
#endif

}

void CVideoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoWnd, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOVE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CVideoWnd 消息处理程序


BOOL CVideoWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_param.iBrightness		= 0;
	m_param.iContrast		= 1;
	m_param.iSaturation		= 1;
	m_param.iHue			= 0;
	m_param.iDetail			= 0;
	m_param.iDenoise		= 0;


	SetTimer(1000,1000,NULL);

	ModifyStyle(WS_CHILD|WS_SYSMENU,WS_CAPTION|WS_DLGFRAME|WS_SIZEBOX);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


BOOL CVideoWnd::DestroyWindow()
{
	KillTimer(1000);

	return CDialogEx::DestroyWindow();
}


BOOL CVideoWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CVideoWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;// CDialogEx::OnEraseBkgnd(pDC);
}


void CVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(&rect);
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		m_pHPAVC_Player->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
#else
	if (m_pVIDEC_Player)
	{
		m_pVIDEC_Player->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
#endif

}


void CVideoWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (1000 == nIDEvent)
	{
		CString strText;
		int nFps = 0;
		int nRecvFrameRate=0;
		int nShowType = 0;
		if (m_pVIDEC_Player)
		{
			nFps=m_pVIDEC_Player->GetVideoShowFrameRate();
			nRecvFrameRate=m_pVIDEC_Player->GetVideoRecvFrameRate();
			nShowType = m_pVIDEC_Player->GetVideoShowType();
		}
		CString strShowType;
		switch(nShowType)
		{
		case 0:
			strShowType = _T("Unknown");
			break;
		case 1:
			strShowType = _T("DirectShow");
			break;
		case 2:
			strShowType = _T("D3D9");
			break;
		case 3:
			strShowType = _T("D3D11");
			break;
		case 4:
			strShowType = _T("D3D12");
			break;
		};

		if (m_pVIDEC_Player)
		{
			int nType = m_pVIDEC_Player->GetDecoderType();
			switch(nType)
			{
			case 0:
				strText.Format(_T("Unknown Decoder Show=%d Recv=%d %s"),nFps,nRecvFrameRate,strShowType);
				break;
			case 1:
				strText.Format(_T("SoftWare Decoder Show=%d Recv=%d %s"),nFps,nRecvFrameRate,strShowType);
				break;
			case 2:
				strText.Format(_T("Intel HardWare Decoder Show=%d Recv=%d %s"),nFps,nRecvFrameRate,strShowType);
				break;
			};
		}
		SetWindowText(strText);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_pParentWnd)
	{
		m_pParentWnd->PostMessage(MSG_SET_FULLWND,m_nIndex,++m_nLBDbClickTimes);
	}

	CDialogEx::OnLButtonDblClk(nFlags, point);
}

unsigned long STR2ULONG(const std::string& value)
{
	return strtoul(value.c_str(),NULL,10);
}

bool CVideoWnd::Start(std::string strAV,int nIndex)
{	
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		m_pHPAVC_Player->Open(GetSafeHwnd());
	}
#else
	if (m_pVIDEC_Player)
	{
		m_pVIDEC_Player->Open(GetSafeHwnd());

		CRect rect;
		GetClientRect(&rect);
#if USE_HPAVC
		if (m_pHPAVC_Player)
		{
			m_pHPAVC_Player->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
#else
		if (m_pVIDEC_Player)
		{
			m_pVIDEC_Player->SetWindowPos(NULL,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
		}
#endif

	}
#endif

	m_nIndex = nIndex;

	int nPos = strAV.find(',',0);
	std::string strTmp = strAV.substr(0,nPos);
	std::string strNewAV = strAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_nAudChanID = STR2ULONG(strTmp);

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_nVidChanID = STR2ULONG(strTmp);

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strNoidID = strTmp;

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strMCUID = strTmp;

	nPos = strNewAV.find(',',0);
	strTmp = strNewAV.substr(0,nPos);
	strNewAV = strNewAV.substr(nPos + 1,strAV.length() - nPos - 1);
	m_strMCUIP = strTmp;

	if (m_nAudChanID == 0 || m_nVidChanID == 0 || m_strNoidID.size() == 0 || m_strMCUID.size() == 0 ||  m_strMCUIP.size() == 0)
	{
		printf("Demo => AV parameters ERROR exit!\n");
		return 0;
	}

	if (NETEC_Node::IsStarted() == 0)
	{
		//远程服务器端口
		NETEC_Node::SetServerIP(m_strMCUIP.c_str());
		NETEC_Node::SetServerPort(4222);
		//启动NODE
		NETEC_Node::Start();

		//unsigned long ulTime = XGetTimestamp();
		//unsigned long ulTime2 = ulTime;
		//while(ulTime2 - ulTime < 2*1000)
		//{
		//	ulTime2 = XGetTimestamp();
		//	XSleep(1);
		//}
	}

	if (m_pIAVReceiver==NULL && GetLocalVideo()==0)
	{
		m_pIAVReceiver = NETEC_MediaReceiver::Create(*this);
		if (m_pIAVReceiver && 0==m_pIAVReceiver->Open(m_strNoidID.c_str(), "",0, "",0, m_strMCUID.c_str(), m_strMCUIP.c_str(), 4222))
		{
			m_pIAVReceiver->SetAudioID(m_nAudChanID);
			m_pIAVReceiver->SetVideoID(m_nVidChanID);
			m_pIAVReceiver->StartAudio();
			m_pIAVReceiver->StartVideo();
			m_pIAVReceiver->RequestKeyFrame();
		}
	}
	//m_pFile = fopen("d:/12.h264","wb");
	return true;
}

//---------------------------------------------------------------------------------------
// 停止服务
void CVideoWnd::Stop(void)
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		m_pHPAVC_Player->Close();
	}
#else
	if (m_pVIDEC_Player)
	{
		m_pVIDEC_Player->Close();
	}
#endif

	printf("Demo => player::StopReceiver\n");

	NETEC_MediaReceiver* pIAVReceiver = m_pIAVReceiver;
	m_pIAVReceiver = NULL;

	printf("Demo => AVReceive::Stop\n");
	if (pIAVReceiver)
	{
		pIAVReceiver->EnableAudio(0);
		pIAVReceiver->EnableVideo(0);
		printf("Demo => AVReceive::StopAudio\n");
		pIAVReceiver->StopAudio();
		printf("Demo => AVReceive::StopVideo\n");
		pIAVReceiver->StopVideo();
		printf("Demo => AVReceive::Close\n");
#ifdef _WIN32
		pIAVReceiver->Close();
#endif // _WIN32
		printf("Demo => AVReceive::delete pIAVReceiver\n");
		delete pIAVReceiver;
		pIAVReceiver = NULL;
	}

	printf("Demo => player::Stop END\n");
}

//---------------------------------------------------------------------------------------
void CVideoWnd::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs == NETEC_Session::CS_CONNECTED || cs == NETEC_Session::CS_RECONNECTED)
	{
		printf("Demo => OnNETEC_NodeConnectStatusChanged::CS_CONNECTED\n");
	}
	else
	{
		printf("Demo => OnNETEC_NodeConnectStatusChanged::%d\n",cs);
	}
}

void CVideoWnd::OnNETEC_MediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen)
{


}

int CVideoWnd::PlayData(void*pPacketData,int nPacketSize)
{
	OnNETEC_MediaReceiverCallbackVideoPacket((unsigned char*)pPacketData,nPacketSize);

	return 0;
}

void CVideoWnd::OnNETEC_MediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen)
{
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		nW = m_pHPAVC_Player->GetVideoWidth();
		nH = m_pHPAVC_Player->GetVideoHeight();
		m_pHPAVC_Player->PlayData(pData,nLen);
	}
#else
	if (m_pVIDEC_Player)
	{
		m_pVIDEC_Player->PlayData(pData,nLen);
	}
#endif
}

void CVideoWnd::OnHPAVC_PlayerCallbackDrawDC(void* hDC/*HDC*/)
{
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		RECT rc;rc.left = 10;rc.top = 10;rc.bottom = 30;rc.right = 300;
		TCHAR strFps[64]={0};
		_stprintf_s(strFps, 64, _T("w=%d h=%d"), m_pHPAVC_Player->GetVideoWidth(),m_pHPAVC_Player->GetVideoHeight());
		DrawText((HDC)hDC,strFps,-1,&rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
	}
#endif
}

void CVideoWnd::VideoParamNotifyNewParam(Video_Param param)
{
#if USE_HPAVC
	if (m_pHPAVC_Player)
	{
		m_param = param;
		m_pHPAVC_Player->SetVideoParam(param.iBrightness,param.iContrast,param.iSaturation,param.iHue,param.iDetail,param.iDenoise);
	}
#else
	if (m_pVIDEC_Player)
	{
		m_param = param;
		m_pVIDEC_Player->SetVideoParam(param.iBrightness,param.iContrast,param.iSaturation,param.iHue,param.iDetail,param.iDenoise);
	}
#endif

}
void CVideoWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDlgVideoParam dlg(*this,m_param);
	dlg.DoModal();

	__super::OnRButtonDown(nFlags, point);
}

void CVideoWnd::SetVideoShow(bool bShowVideo)
{

}

void CVideoWnd::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	__super::OnRButtonDblClk(nFlags, point);
}

void CVideoWnd::OnMove(int x, int y)
{
	__super::OnMove(x, y);

}

void CVideoWnd::OnVIDEC_PlayerCallbackDrawD3D9DC(void* hDC/*HDC*/)
{
	SetTextColor((HDC)hDC, RGB(0,0,255));
	SetBkMode((HDC)hDC, TRANSPARENT);

	int nType = 0;
	RECT rc;rc.left = 10;rc.top = 30;rc.bottom = 60;rc.right = 300;
	if (m_pVIDEC_Player)
		nType = m_pVIDEC_Player->GetDecoderType();

	CString strText;
	switch(nType)
	{
	case 0:
		strText.Format(_T("Unknown Decoder"));
		break;
	case 1:
		strText.Format(_T("SoftWare Decoder"));		
		break;
	case 2:
		strText.Format(_T("Intel HardWare Decoder"));		
		break;
	};

	DrawText((HDC)hDC,strText,-1,&rc, DT_LEFT|DT_TOP|DT_SINGLELINE);
}
void CVideoWnd::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	__super::OnClose();
}
