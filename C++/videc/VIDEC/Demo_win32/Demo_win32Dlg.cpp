
// Demo_win32Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Demo_win32.h"
#include "Demo_win32Dlg.h"
#include "OpAH2K.h"
#include "OpVidecCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static CDemo_win32Dlg* g_pDemo_win32Dlg=NULL;
// CDemo_win32Dlg 对话框

CDemo_win32Dlg::CDemo_win32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDemo_win32Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_pDemo_win32Dlg=this;
}

CDemo_win32Dlg* CDemo_win32Dlg::Instance(void)
{
	return g_pDemo_win32Dlg;
}

void CDemo_win32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_AV, m_editAV);
	DDX_Control(pDX, IDC_BUTTON_RECV, m_btnRecv);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_STATIC_MCUIP, m_stacMCUIP);
	DDX_Control(pDX, IDC_EDIT_MCUIP, m_editMCUIP);
	DDX_Control(pDX, IDC_STATIC_RATE, m_stacRate);
	DDX_Control(pDX, IDC_EDIT_RATE, m_editRate);
	DDX_Control(pDX, IDC_EDIT_AV_CAPTURE, m_editCapture);
	DDX_Control(pDX, IDC_BUTTON_CAPTURE, m_btnCapture);
	DDX_Control(pDX, IDC_BUTTON_CAPTURE_STOP, m_bntCaptureStop);
	DDX_Control(pDX, IDC_STATIC_Bitrate, m_stacBitrate);
	DDX_Control(pDX, IDC_EDIT_Bitrate, m_editBitrate);
	DDX_Control(pDX, IDC_CHECK_SHOW, m_btnShow);
	DDX_Control(pDX, IDC_CHECK_HPAVC, m_btnEncode);
	DDX_Control(pDX, IDC_CBX_CHANNEL, m_cbxVideoChan);
	DDX_Control(pDX, IDC_CBX_SHOW, m_cbxVideoShow);
	DDX_Control(pDX, IDC_CHECK_SHOW3, m_btnShow2);
	DDX_Control(pDX, IDC_CHECK_PG_SDK, m_btnPG_SDK);
}

BEGIN_MESSAGE_MAP(CDemo_win32Dlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RECV, &CDemo_win32Dlg::OnBnClickedButtonRecv)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDemo_win32Dlg::OnBnClickedButtonStop)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(MSG_SET_FULLWND,&CDemo_win32Dlg::OnSetFullWnd)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CDemo_win32Dlg::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_STOP, &CDemo_win32Dlg::OnBnClickedButtonCaptureStop)
	ON_BN_CLICKED(IDC_CHECK_SHOW, &CDemo_win32Dlg::OnBnClickedCheckShow)
	ON_BN_CLICKED(IDC_CHECK_SHOW3, &CDemo_win32Dlg::OnBnClickedCheckShow)
END_MESSAGE_MAP()


// CDemo_win32Dlg 消息处理程序

BOOL CDemo_win32Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nVideoCount = 0;

	m_bStartCapture = false;

	NETEC_Core::Start();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ReSize();
	InitChans();

	m_editMCUIP.SetWindowText(_T("192.168.12.201"));

	m_editRate.SetWindowText(_T("60"));
	m_editBitrate.SetWindowText(_T("2048"));

	m_btnShow.SetCheck(TRUE);
	m_btnShow2.SetCheck(TRUE);
	m_btnEncode.SetCheck(TRUE);
	m_btnPG_SDK.SetCheck(FALSE);

//#ifdef _DEBUG
	m_editAV.SetWindowText(_T("33203,33204,0-5586E58-ED8F28A2@xf,MCU-001@xf,192.168.12.201"));
//#endif // _DEBUG

	m_cbxVideoShow.ShowWindow(SW_HIDE);

	m_bWaitStop = false;
	StartThread();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDemo_win32Dlg::OnPaint()
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
HCURSOR CDemo_win32Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDemo_win32Dlg::OnBnClickedButtonRecv()
{
	if(m_nVideoCount >=36 )
	{
		return;
	}

	CString strText;
	m_editAV.GetWindowText(strText);
	//m_editAV.SetWindowText(_T(""));

	std::string strAV = CW2A(strText);

	if (strAV.length() < 10)
	{
		return;
	}
	UpdateData(TRUE);

	CVideoWnd*pVideoWnd = new CVideoWnd(this,false);
	pVideoWnd->Create(CVideoWnd::IDD, this);
	if (pVideoWnd)
	{
		pVideoWnd->Start(strAV,m_nVideoCount);
		//pVideoWnd->SetVideoShow(m_btnShow.GetCheck());
		m_mapWnd[m_nVideoCount++] = pVideoWnd;
	}

	ReSize();
}

void CDemo_win32Dlg::OnBnClickedButtonStop()
{
	StopAll();
}

BOOL CDemo_win32Dlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDemo_win32Dlg::DestroyWindow()
{
	m_bWaitStop = true;
	WaitForStop();

	OnBnClickedButtonCaptureStop();

	StopAll();

	//printf("Demo => NETEC_Node::Stop\n");
	NETEC_Node::Stop();
	//printf("Demo => NETEC_Core::Stop\n");
	NETEC_Core::Stop();	

	{
		XAutoLock l(m_csListPacketData);
		while (m_listPacketData.size())
		{
			XSurfaceBuffer*pXPacketBuffer=NULL;
			pXPacketBuffer=(XSurfaceBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();
			if (pXPacketBuffer)
			{			
				delete pXPacketBuffer;
				pXPacketBuffer=NULL;
			}
		}
	}

	return CDialog::DestroyWindow();
}

void CDemo_win32Dlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 330;
	lpMMI->ptMinTrackSize.y = 260;
	__super::OnGetMinMaxInfo(lpMMI);
}

void CDemo_win32Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


	ReSize();
}

LRESULT	CDemo_win32Dlg::OnSetFullWnd(WPARAM wParam, LPARAM lParam)
{
	int nIndex = wParam;
	VIDEO_WND_SIZE nSizeType = (VIDEO_WND_SIZE)(lParam % VWND_SIZE_COUNT);

	CVideoWnd* pVideoWnd = NULL;
	MAP_Wnd::iterator it = m_mapWnd.find(nIndex);
	if (it != m_mapWnd.end())
	{
		pVideoWnd = it->second;
		if (!pVideoWnd || !pVideoWnd->GetSafeHwnd())
		{
			return 0;
		}
	}

	switch (nSizeType)
	{
	case VWND_CHILD_NORMAL:
		{
			pVideoWnd->ModifyStyle(WS_CHILD,WS_CAPTION);
			pVideoWnd->ModifyStyleEx(WS_EX_TOPMOST,0);
			pVideoWnd->SetParent(this);
			ReSize(&nIndex);
		}
		break;
	//case VWND_CHILD_FULL:
	//	{
	//		CRect rect;
	//		GetClientRect(&rect);
	//		int nBtnW = 50;
	//		int nBtnH = 25;
	//		int nLeft = 10;
	//		int nTop = 10;
	//		int nBottom = 10;
	//		int nRight = 10;
	//		int nSpace = 5;

	//		int nWidth = rect.Width() - 2 * nLeft;
	//		//	int nHeight = rect.Height() - 2 * nTop - nBtnH - nBottom;
	//		int nHeight = rect.Height() - 2 * nTop - nBtnH - nBottom - nBtnH - nSpace;


	//		//pVideoWnd->ModifyStyle(WS_CAPTION,WS_CHILD);
	//		pVideoWnd->ModifyStyleEx(WS_EX_TOPMOST,0);
	//		pVideoWnd->SetParent(this);
	//		pVideoWnd->SetWindowPos(NULL, nLeft, nTop, nWidth, nHeight, SWP_SHOWWINDOW);
	//		pVideoWnd->Invalidate();
	//	}
	//	break;
	case VWND_NOMAL:
		{
			RECT rt;
			ReSize(&nIndex);
			pVideoWnd->GetWindowRect(&rt);
			pVideoWnd->ModifyStyle(WS_CHILD|WS_SYSMENU,WS_CAPTION|WS_DLGFRAME|WS_SIZEBOX);
			pVideoWnd->SetParent(NULL);
			pVideoWnd->ModifyStyleEx(0,WS_EX_TOPMOST);
			pVideoWnd->MoveWindow(&rt);
		}
		break;
	case VWND_NORMAL_FULL:
		{
			pVideoWnd->ModifyStyle(WS_CHILD|WS_CAPTION|WS_DLGFRAME|WS_SYSMENU,0);
			pVideoWnd->SetParent(NULL);
			pVideoWnd->ModifyStyleEx(0,WS_EX_TOPMOST);
			pVideoWnd->ShowWindow(SW_MAXIMIZE);
			pVideoWnd->Invalidate();
		}
		break;
	}
	
	return 0;
}

void CDemo_win32Dlg::ReSize(int* pVideoIndex/*=NULL*/)
{
	if (!m_btnRecv.GetSafeHwnd())
		return;

	CRect rect;
	GetClientRect(&rect);
	int nEditRate = 100;
	int nEditW = 100;
	int nBtnW = 50;
	int nBtnH = 25;
	int nLeft = 10;
	int nTop = 10;
	int nBottom = 10;
	int nRight = 10;
	int nSpace = 5;
	
	int x = rect.right - nRight - nBtnW;
	int y = rect.bottom - nBtnH - nBottom - nBtnH - nSpace;
	m_btnStop.MoveWindow(x, y, nBtnW, nBtnH);
	x = x - nBtnW - nSpace;
	m_btnRecv.MoveWindow(x, y, nBtnW, nBtnH);	

	x=nLeft;
	m_btnShow.MoveWindow(x, y, nBtnW+20, nBtnH);	

	x+=nBtnW + 20 + nSpace;
	m_btnShow2.MoveWindow(x, y, nBtnW+20, nBtnH);	

	x+=nBtnW + nSpace;
	m_stacMCUIP.MoveWindow(x, y+5, 30, nBtnH);	

	x+=30 + nSpace;
	m_editMCUIP.MoveWindow(x, y, nEditRate, nBtnH);

	x+=nEditRate + nSpace;
	nEditW = rect.Width() - x - (nBtnW + nSpace)*2 - nSpace*2;
	//nEditW = rect.Width() - (nBtnW + nSpace)*3 - nRight - nLeft - nEditRate - nSpace;
	m_editAV.MoveWindow(x, y,nEditW, nBtnH);

	x = rect.right - nRight - nBtnW;
	y = rect.bottom - nBtnH - nBottom;
	m_bntCaptureStop.MoveWindow(x, y, nBtnW, nBtnH);
	x = x - nBtnW - nSpace;
	m_btnCapture.MoveWindow(x, y, nBtnW, nBtnH);
	m_cbxVideoChan.MoveWindow(x-nBtnW*4,y,nBtnW*4-5,nBtnH);

	x=nLeft;
	m_btnPG_SDK.MoveWindow(x, y, nBtnW+15, nBtnH);	

	x+=nBtnW + nSpace + 18;
	m_btnEncode.MoveWindow(x, y, nBtnW+10, nBtnH);	

	x+=nBtnW + nSpace +10;
	m_stacRate.MoveWindow(x, y+5, nBtnW, nBtnH);	

	x+=nBtnW + nSpace;
	m_editRate.MoveWindow(x, y, nEditRate/2, nBtnH);

	x +=nEditRate/2;
	m_stacBitrate.MoveWindow(x, y+5, nBtnW, nBtnH);	

	x +=nBtnW + nSpace;
	m_editBitrate.MoveWindow(x, y, nEditRate/2, nBtnH);

	x +=nEditRate/2 + nSpace;
	nEditW = rect.Width() - x - (nBtnW + nSpace)*2 - nSpace*2;
	m_editCapture.MoveWindow(x, y,nEditW-4*nBtnW, nBtnH);


	int nWidth = rect.Width() - 2 * nLeft;
	int nHeight = rect.Height() - 2 * nTop - nBtnH - nBottom - nBtnH - nSpace;
	int nX[36];
	int nY[36];
	int nCX[36];
	int nCY[36];

	if (m_nVideoCount == 1)
	{
		nX[0] = nLeft; nY[0] = nTop; nCX[0] = nWidth; nCY[0] = nHeight;
	}
	else if (m_nVideoCount == 2)
	{
		int nTempCX = (nWidth - 10) / 2;
		nX[0] = nLeft; nY[0] = nTop; nCX[0] = nTempCX; nCY[0] = nHeight;
		nX[1] = nLeft * 2 + nTempCX; nY[1] = nTop; nCX[1] = nTempCX; nCY[1] = nHeight;
	}
	else if (m_nVideoCount == 3 || m_nVideoCount == 4)
	{
		int nTempCX = (nWidth - 10) / 2;
		int nTempCY = (nHeight - 10) / 2;
		nX[0] = nLeft; nY[0] = nTop; nCX[0] = nTempCX; nCY[0] = nTempCY;
		nX[1] = nLeft * 2 + nTempCX; nY[1] = nTop; nCX[1] = nTempCX; nCY[1] = nTempCY;
		nX[2] = nLeft; nY[2] = nTop * 2 + nTempCY; nCX[2] = nTempCX; nCY[2] = nTempCY;
		nX[3] = nLeft * 2 + nTempCX; nY[3] = nTop * 2 + nTempCY; nCX[3] = nTempCX; nCY[3] = nTempCY;
	}
	else if (m_nVideoCount <= 20)
	{
		int nTempCX = (nWidth - 30) / 4;
		int nTempCY = (nHeight - 40) / 5;
		int nIndex = 0;
		for (int j = 1; j <= 5; j++)
		{
			for (int i = 1; i <= 4; i++)
			{
				nX[nIndex] = nLeft*i + (i - 1) * nTempCX;
				nY[nIndex] = nTop*j + (j - 1)* nTempCY;
				nCX[nIndex] = nTempCX;
				nCY[nIndex] = nTempCY;
				nIndex++;
			}
		}
	}
	else if (m_nVideoCount <= 36)
	{
		int nTempCX = (nWidth - 50) / 6;
		int nTempCY = (nHeight - 50) / 6;
		int nIndex = 0;
		for (int j = 1; j <= 6; j++)
		{
			for (int i = 1; i <= 6; i++)
			{
				nX[nIndex] = nLeft*i + (i - 1) * nTempCX;
				nY[nIndex] = nTop*j + (j - 1)* nTempCY;
				nCX[nIndex] = nTempCX;
				nCY[nIndex] = nTempCY;
				nIndex++;
			}
		}
	}
	else
	{
		return;
	}

	int j = 0;
	MAP_Wnd::iterator it = m_mapWnd.begin();
	while (it != m_mapWnd.end())
	{
		if (pVideoIndex==NULL || *pVideoIndex==it->first)
		{
			CVideoWnd*pVideoWnd = it->second;
			if (pVideoWnd && pVideoWnd->GetSafeHwnd())
			{
				pVideoWnd->SetWindowPos(NULL, nX[j], nY[j], nCX[j], nCY[j], SWP_SHOWWINDOW);
			}
		}
		j++;
		it++;
	}

	OnBnClickedCheckShow();
}


void CDemo_win32Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnTimer(nIDEvent);
}

int CDemo_win32Dlg::PlayData(void*pPacketData,int nPacketSize)
{
	XSurfaceBuffer*pXPacketBuffer=new XSurfaceBuffer;
	if (pXPacketBuffer)
	{
		pXPacketBuffer->pData=malloc(nPacketSize);
		if (pXPacketBuffer->pData)
		{
			memcpy((char*)pXPacketBuffer->pData,(char*)pPacketData,nPacketSize);
			pXPacketBuffer->nDatalen=nPacketSize;
		}
		else
		{
			delete pXPacketBuffer;
			pXPacketBuffer=NULL;
		}
	}
	if (pXPacketBuffer)
	{
		XAutoLock l(m_csListPacketData);
		m_listPacketData.push_back(pXPacketBuffer);
	}

	return 0;
}


void CDemo_win32Dlg::ThreadProcMain(void)
{
	while(!m_bWaitStop)
	{
		XSurfaceBuffer*pXSurfaceBuffer=NULL;
		{
			XAutoLock l(m_csListPacketData);
			int nSize = m_listPacketData.size();
			if (nSize > 10)
			{
				do
				{
					XSurfaceBuffer*pXPacketBuffer=(XSurfaceBuffer*)m_listPacketData.front();
					m_listPacketData.pop_front();
					if (pXPacketBuffer)
					{			
						delete pXPacketBuffer;
						pXPacketBuffer=NULL;
					}
				}while (m_listPacketData.size() > 5);
			}
			pXSurfaceBuffer=(XSurfaceBuffer*)m_listPacketData.front();
			m_listPacketData.pop_front();		
		}
		if (pXSurfaceBuffer!=NULL)
		{			
			MAP_Wnd::iterator it = m_mapWnd.begin();
			while (it!=m_mapWnd.end())
			{
				CVideoWnd*pVideoWnd = it->second;
				if (pVideoWnd)
				{
					pVideoWnd->PlayData(pXSurfaceBuffer->pData,pXSurfaceBuffer->nDatalen);
				}
				it++;
			}
			delete pXSurfaceBuffer;
			pXSurfaceBuffer=NULL;
		}
		else
		{
			Sleep(1);
		}
	}
}

void CDemo_win32Dlg::StopAll(void)
{
	m_nVideoCount = 0;

	MAP_Wnd::iterator it = m_mapWnd.begin();
	while (it!=m_mapWnd.end())
	{
		CVideoWnd*pVideoWnd = it->second;
		if (pVideoWnd)
		{
			pVideoWnd->Stop();
			pVideoWnd->DestroyWindow();
			delete pVideoWnd;
			pVideoWnd = NULL;
		}
		it++;
	}
	m_mapWnd.clear();

	ReSize();
}

void CDemo_win32Dlg::OnBnClickedButtonCapture()
{
	UpdateData(TRUE);

	if (NETEC_Node::IsStarted() == 0)
	{
		CString strMCUIP;
		m_editMCUIP.GetWindowText(strMCUIP);
		std::string strT = CW2A(strMCUIP);

		if (strT.length() > 0)
		{
			NETEC_Node::SetServerIP(strT.c_str());
			NETEC_Node::SetServerPort(4222);
			NETEC_Node::Start();
		}
	}

	USES_CONVERSION;
	CString strRate;
	m_editRate.GetWindowText(strRate);
	int nFrameRate = _ttoi(strRate);

	CString strBitrate;
	m_editBitrate.GetWindowText(strBitrate);
	int nBitrate = _ttoi(strBitrate);		

	int nLocalDev = -1;
	if (m_cbxVideoChan.GetCurSel() > 0)
	{
		nLocalDev = m_cbxVideoChan.GetItemData(m_cbxVideoChan.GetCurSel());
	}
	if (nLocalDev >= 0)
	{
		m_bStartCapture = true;
		CaptureLocalDev(nLocalDev,1920,1080,nFrameRate,nBitrate);
	}
	else
	{
		std::string strFile = BrowseFile();
		if (strFile != "")
		{
			m_bStartCapture = true;
			CaptureFile(strFile,nFrameRate,nBitrate);
		}
	}

	CString strPar;
	CString strNodeID = CA2W(NETEC_Node::GetNodeID());
	CString strMCUID = CA2W(NETEC_Node::GetMCUID());
	CString strMCUIP = CA2W(NETEC_Node::GetMCUIP());
	

	if (!strNodeID.IsEmpty())
	{
		strPar.Format(_T("%u,%u,%s,%s,%s"),m_ulAudioID,m_ulVideoID,strNodeID,strMCUID,strMCUIP);
		m_editCapture.SetWindowText(strPar);
		m_editAV.SetWindowText(strPar);
	}
}

//---------------------------------------------------------------------------------------
void CDemo_win32Dlg::OnNETEC_NodeConnectStatusChanged(NETEC_Session::CONNECT_STATUS cs)
{
	if (cs == NETEC_Session::CS_CONNECTED || cs == NETEC_Session::CS_RECONNECTED)
	{
		CString strPar;
		CString strNodeID = CA2W(NETEC_Node::GetNodeID());
		CString strMCUID = CA2W(NETEC_Node::GetMCUID());
		CString strMCUIP = CA2W(NETEC_Node::GetMCUIP());

		if (!strNodeID.IsEmpty() && m_bStartCapture)
		{
			strPar.Format(_T("%u,%u,%s,%s,%s"),m_ulAudioID,m_ulVideoID,strNodeID,strMCUID,strMCUIP);
			m_editCapture.SetWindowText(strPar);
			m_editAV.SetWindowText(strPar);
		}

		printf("Demo => OnNETEC_NodeConnectStatusChanged::CS_CONNECTED\n");
	}
	else
	{
		printf("Demo => OnNETEC_NodeConnectStatusChanged::%d\n",cs);
	}
}

void CDemo_win32Dlg::OnBnClickedButtonCaptureStop()
{
	m_bStartCapture = false;

	MAP_Capture::iterator it = m_mapCapture.begin();
	while (it!=m_mapCapture.end())
	{
		CCaptureWnd*pVideoWnd = it->second;
		if (pVideoWnd)
		{
			pVideoWnd->Stop();
			pVideoWnd->DestroyWindow();
			delete pVideoWnd;
			pVideoWnd = NULL;
		}
		it++;
	}
	m_mapCapture.clear();
}

void CDemo_win32Dlg::OnBnClickedCheckShow()
{
	UpdateData(TRUE);

	int n=0;
	MAP_Capture::iterator it = m_mapCapture.begin();
	while (it!=m_mapCapture.end())
	{
		CCaptureWnd*pVideoWnd = it->second;
		if (pVideoWnd)
		{
			if (n == 0)
			{
				n=1;
				pVideoWnd->SetVideoShow(m_btnShow.GetCheck());
			}
			else
			{
				pVideoWnd->SetVideoShow(m_btnShow2.GetCheck());
			}
		}
		it++;
	}
	/*{
		MAP_Wnd::iterator it = m_mapWnd.begin();
		while (it!=m_mapWnd.end())
		{
			CVideoWnd*pVideoWnd = it->second;
			if (pVideoWnd)
			{
				pVideoWnd->SetVideoShow(m_btnShow.GetCheck());
			}
			it++;
		}
	}*/

}

void CDemo_win32Dlg::InitChans( void )
{
	m_cbxVideoShow.ResetContent();
	m_cbxVideoShow.AddString(_T("D3D"));
// 	m_cbxVideoShow.AddString(_T("D3D_9"));
// 	m_cbxVideoShow.AddString(_T("D3D_11"));
// 	m_cbxVideoShow.AddString(_T("DRSHOW"));
// 	m_cbxVideoShow.AddString(_T("XIS_Lib"));
	m_cbxVideoShow.SetCurSel(0);
	//////////////////////////////////////////////////////////////////////////

	m_cbxVideoChan.ResetContent();

	m_cbxVideoChan.AddString(_T("NULL"));
	m_cbxVideoChan.SetItemData(0,-1);
	m_cbxVideoChan.SetCurSel(0);

	int nCount = VIDEC_AH2K::GetDevCount();

	USES_CONVERSION;
	map<string,string> mapDev;
	for (int n=0; n<nCount; ++n)
	{
		const int nSize = 256;
		char szDevName[nSize] = {0};
		VIDEC_AH2K::GetDevName(n,szDevName,nSize);
		m_cbxVideoChan.AddString(A2T(szDevName));
		m_cbxVideoChan.SetItemData(m_cbxVideoChan.GetCount()-1,AH2K_DEV_BEGIN+n);
		mapDev.insert(std::make_pair(szDevName,szDevName));

		m_cbxVideoChan.SetCurSel(2);
	}
	
	nCount = VIDEC_Camera::GetDeviceCount();
	for (int n=0; n<nCount; ++n)
	{
		const int nSize = 256;
		char szDevName[nSize] = {0};
		VIDEC_Camera::GetDeviceName(n,szDevName);
		//if (mapDev.find(szDevName) == mapDev.end())
		{
			m_cbxVideoChan.AddString(A2T(szDevName));
			m_cbxVideoChan.SetItemData(m_cbxVideoChan.GetCount()-1,n);
		}
	}
}

void CDemo_win32Dlg::CaptureLocalDev(int nDev,int nWidth,int nHeight,int nFrameRate,int nBitrate)
{
	if (nDev >= 0)
	{
		int nShowType = m_cbxVideoShow.GetCurSel();

		bool bEncode = m_btnEncode.GetCheck();
		bool bPG_SDK = m_btnPG_SDK.GetCheck();
		CCaptureWnd*pVideoWnd = new CCaptureWnd(nShowType,bEncode);
		pVideoWnd->Create(CCaptureWnd::IDD, this);
		if (pVideoWnd)
		{
			pVideoWnd->ShowWindow(SW_SHOW);
			pVideoWnd->StartCapture(nDev,nWidth,nHeight,nFrameRate,nBitrate);
			if (bPG_SDK)
			{
				pVideoWnd->StartPGSDK();
			}
			m_mapCapture[pVideoWnd] = pVideoWnd;
			pVideoWnd->GetAVParameters(m_ulAudioID,m_ulVideoID);

			//pVideoWnd->SetVideoShow(m_btnShow.GetCheck());
		}
	}
}

void CDemo_win32Dlg::CaptureFile(const std::string& strFile,int nFrameRate,int nBitrate)
{
	bool bEncode = m_btnEncode.GetCheck();
	int nShowType = m_cbxVideoShow.GetCurSel();
	CCaptureWnd*pVideoWnd = new CCaptureWnd(nShowType,bEncode);
	pVideoWnd->Create(CCaptureWnd::IDD, this);
	if (pVideoWnd)
	{
		pVideoWnd->ShowWindow(SW_SHOW);
		pVideoWnd->Start(strFile,nFrameRate,nBitrate);
		m_mapCapture[pVideoWnd] = pVideoWnd;
		pVideoWnd->GetAVParameters(m_ulAudioID,m_ulVideoID);

		//pVideoWnd->SetVideoShow(m_btnShow.GetCheck());
	}
}

std::string CDemo_win32Dlg::BrowseFile()
{
	std::string strFile = "";
#ifdef _DEBUG
	strFile = "E:/WestWindEasy_8bit_YV12_1920x1080.yuv";//"d:/1080i2997_mobcal_ter.yuv";
#else
	CString	Filter = _T("YUV Files|*.yuv||");

	CFileDialog dlgFile(TRUE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST,Filter,this,0, FALSE);		
	if(dlgFile.DoModal() == IDOK)
	{
		CString strFilePath = dlgFile.GetPathName();
		strFile = CW2A(strFilePath);
	}
#endif // _DEBUG

	return strFile;
}
