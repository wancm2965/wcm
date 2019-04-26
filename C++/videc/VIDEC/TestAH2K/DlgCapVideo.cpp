// DlgCapVideo.cpp : 实现文件
//

#include "stdafx.h"
#include "TestAH2K.h"
#include "DlgCapVideo.h"
#include "DlgMain.h"


// DlgCapVideo 对话框

IMPLEMENT_DYNAMIC(DlgCapVideo, CDialog)
DlgCapVideo::DlgCapVideo(DlgMain&rDlgMain)
	: CDialog(DlgCapVideo::IDD, &rDlgMain)
#ifdef TEST_MEDIAFILE1
	,m_pMediaFile(NULL)
#else
	, m_pWnd(NULL)
#endif
	, m_rDlgMain(rDlgMain)
{
}

DlgCapVideo::~DlgCapVideo()
{
}

void DlgCapVideo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgCapVideo, CDialog)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// DlgCapVideo 消息处理程序

void DlgCapVideo::OnOK()
{
}

void DlgCapVideo::OnCancel()
{
}

BOOL DlgCapVideo::OnInitDialog()
{
	CDialog::OnInitDialog();

#ifdef TEST_MEDIAFILE1

#else
	m_pWnd=VIDEC_Wnd::CreateLocalVideo( *this );
	if (m_pWnd!=NULL)
	{
		if (0!=m_pWnd->Open(m_hWnd,true))
		{
			m_pWnd->Close();
			delete m_pWnd;
			m_pWnd=NULL;
		}
	}

	for (int i=0;i<0;i++)
	{
		VIDEC_Wnd*pWnd=VIDEC_Wnd::Create( *this );
		if (pWnd!=NULL)
		{
			if (0!=pWnd->Open(m_hWnd,true))
			{
				pWnd->Close();
				delete pWnd;
				pWnd=NULL;
			}
			else
			{
				m_ListWnd.push_back(pWnd);
			}
		}

	}
#endif

	ResizeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL DlgCapVideo::DestroyWindow()
{
#ifdef TEST_MEDIAFILE1

#else
	if (m_pWnd!=NULL)
	{
		m_pWnd->Close();
		delete m_pWnd;
		m_pWnd=NULL;
	}

	while (m_ListWnd.size()>0)
	{
		VIDEC_Wnd*pWnd=m_ListWnd.front();
		m_ListWnd.pop_front();
		pWnd->Close();
		delete pWnd;
		pWnd=NULL;
	}
#endif


	return CDialog::DestroyWindow();
}

void DlgCapVideo::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	ResizeControls();
}

void DlgCapVideo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	m_rDlgMain.SetFullScreen();

	CDialog::OnLButtonDblClk(nFlags, point);
}

void DlgCapVideo::ResizeControls(void)
{
	int nOffset=0;
	CRect rc;
	GetClientRect(rc);

#ifdef TEST_MEDIAFILE1
	if (m_pMediaFile!=NULL)
	{
		m_pMediaFile->SetWindowPos(NULL,0,0,rc.Width(),rc.Height(),SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}
#else
	if (m_pWnd!=NULL)
	{
		m_pWnd->SetWindowPos(NULL,-0,-nOffset,rc.Width()+0,rc.Height()+nOffset*2,SWP_SHOWWINDOW|SWP_NOACTIVATE);
		//m_pWnd->SetWindowPos(NULL,0,0,960,720,SWP_SHOWWINDOW|SWP_NOACTIVATE);
	}

	ListWnd::iterator iter=m_ListWnd.begin();
	while (iter!=m_ListWnd.end())
	{
		VIDEC_Wnd*pWnd=*iter;
		pWnd->SetWindowPos(NULL,-0,-nOffset,rc.Width()+0,rc.Height()+nOffset*2,SWP_SHOWWINDOW|SWP_NOACTIVATE);

		++iter;
	}
#endif
}

void DlgCapVideo::SetImageSource(VIDEC_ImageSource*pImageSource)
{
#ifdef TEST_MEDIAFILE1

#else
	ListWnd::iterator iter=m_ListWnd.begin();
	while (iter!=m_ListWnd.end())
	{
		VIDEC_Wnd*pWnd=*iter;

		pImageSource->AddImageSink(pWnd);

		++iter;
	}
#endif
}
