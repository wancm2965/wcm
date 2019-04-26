

#include "stdafx.h"

#include "CatchScreenDlg.h"
//#include "common/XYIniReader.h"
#include <atlimage.h>
#include "whiteBoard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCatchScreenDlg dialog

CCatchScreenDlg::CCatchScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCatchScreenDlg::IDD, pParent)
{
    //��ʼ����Ƥ����,������resizeMiddle ����
	m_rectTracker.m_nStyle=CMyTracker::resizeMiddle|CMyTracker::solidLine;  
	m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
	//���þ�����ɫ
	m_rectTracker.SetRectColor(RGB(10,100,130));
	//���þ��ε���ʱ���,Ĭ�ϵ�̫С��,Ū�˸�����
	m_rectTracker.SetResizeCursor(IDC_CURSOR6,IDC_CURSOR5,IDC_CURSOR2,IDC_CURSOR3,IDC_CURSOR4);

	//���
    m_hCursor=AfxGetApp()->LoadCursor(IDC_CURSOR1);  
    	
	m_bDraw=FALSE;
	m_bFirstDraw=FALSE;
	m_bQuit=FALSE;
	m_bShowMsg=FALSE;
    m_startPt=0;
    m_hIcon = NULL;
	m_xScreen = 0;
	m_yScreen = 0;
	m_nscreenLeft = 0;
	m_nscreenTop = 0;
	m_ptEditLT = CPoint(0,0);	
	//��ȡ��Ļ�ֱ���
// 	POINT pt;
// 	::GetCursorPos(&pt);
// 	MONITORINFO oMonitor = {}; 
// 	oMonitor.cbSize = sizeof(oMonitor);
// 	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
// 	CRect rcWork = oMonitor.rcWork;
	
	
	//m_xScreen = GetSystemMetrics(SM_CXSCREEN);
	//m_yScreen = GetSystemMetrics(SM_CYSCREEN);
// 	m_xScreen = rcWork.right;
// 	m_yScreen = rcWork.bottom;
	GetDisplayInfo();
	m_ptEditLT = CPoint(m_nscreenLeft,m_nscreenTop);
	//��ȡ��Ļ��λͼ��
	CRect rect(m_nscreenLeft, m_nscreenTop,m_xScreen,m_yScreen);
	m_pBitmap=CBitmap::FromHandle(CopyScreenToBitmap(&rect));
    
	//��ʼ��ˢ�´������� m_rgn
    m_rgn.CreateRectRgn(m_nscreenLeft,m_nscreenTop,50,50);	
}

CCatchScreenDlg::~CCatchScreenDlg()
{
	if(m_pBitmap)
	{
		m_pBitmap->DeleteObject();
	}
	m_rgn.DeleteObject();
}

void CCatchScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_tipEdit);
}

BEGIN_MESSAGE_MAP(CCatchScreenDlg, CDialog)
	//{{AFX_MSG_MAP(CCatchScreenDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_WM_CHAR()
	ON_STN_CLICKED(IDC_EDIT1, &CCatchScreenDlg::OnStnClickedEdit1)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCatchScreenDlg message handlers

BOOL CCatchScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//**************************************************************************
	//�ѶԻ������ó�ȫ�����㴰��
	SetWindowPos(&wndTopMost,m_nscreenLeft,m_nscreenTop,m_xScreen-m_nscreenLeft,m_yScreen-m_nscreenTop,SWP_SHOWWINDOW);
    
	//�ƶ�������ʾ����
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
	m_tipEdit.SetWindowLT(m_ptEditLT);
	int width = GetSystemMetrics(SM_CXSCREEN);
	m_tipEdit.SetScreenWidth(width);
    //m_tipEdit.MoveWindow(m_ptEditLT.x+10,m_ptEditLT.y+10,rect.Width(),rect.Height());

	//��ʾ������ʾ��������
	DrawTip();
	SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE)|WS_EX_TOOLWINDOW);
	//���񰴼���Ϣ����,���Ի���ľ�����ݵ�CCatchScreenApp��
//	((CCatchScreenApp *)AfxGetApp())->m_hwndDlg=m_hWnd;
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//this->SetFocus();
	if (this->m_hWnd)
	{
		if (g_hVedioHwnd)
		{
			g_hVedioHwnd = NULL;
		}
		g_hVedioHwnd = this->m_hWnd;
	}
	return TRUE;  
}


void CCatchScreenDlg::OnPaint() 
{
	if (IsIconic())//���細������С����
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
//**************************************************************************
		CPaintDC dc(this);
         
		//��ʾ��ȡ���δ�С��Ϣ
		if(m_bShowMsg&&m_bFirstDraw)
		{
			//�õ���ǰ���δ�С
			CRect rect;
			m_rectTracker.GetTrueRect(&rect);
			rect.left -= m_ptEditLT.x;
			rect.top -= m_ptEditLT.y;
			rect.right -= m_ptEditLT.x;
			rect.bottom -= m_ptEditLT.y;
			//����CPaintDC ��Ϊ�˲����˴����ϻ���Ϣ
			DrawMessage(rect,&dc);
		}

		//������Ƥ�����
		if(m_bFirstDraw)
		{
			m_rectTracker.Draw(&dc);
		}

//*************************************************************************
		CDialog::OnPaint();
	}
}

HCURSOR CCatchScreenDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CCatchScreenDlg::OnOK() 
{
	//�������״�ָ�Ϊ��ͷ
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
	CDialog::OnOK();
}

void CCatchScreenDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
//***************************************************************
	//�������״�ָ�Ϊ��ͷ
	SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
	if(m_bFirstDraw)
	{
		//ȡ���ѻ����α���
		m_bFirstDraw=FALSE;
		m_bDraw=FALSE;
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		PaintWindow();
	}
	else
	{
		CDialog::OnCancel();
	}
//*******************************************************************
}

void CCatchScreenDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	   if(m_bDraw)
	   {
			//��̬�������δ�С,��ˢ�»���
		    point = point - CPoint(18,9)/* + m_ptEditLT*/;
		    m_rectTracker.m_rect.SetRect(m_startPt.x+4,m_startPt.y+4,point.x,point.y);
			PaintWindow();
	   }
	   
	   //�ֲ�������С��λ��ʱ,���ղ���MouseMove��Ϣ
	   CRect rect;
	   m_tipEdit.GetWindowRect(&rect);
	   if(rect.PtInRect(point))
	   m_tipEdit.SendMessage(WM_MOUSEMOVE);
       
	   ChangeRGB();
	   CDialog::OnMouseMove(nFlags, point);
}

void CCatchScreenDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //�жϻ���λ��
	if(nHitTest<0)
	{
		if(!m_bFirstDraw)
		{
			//��һ�λ�����
			point = point - CPoint(18,9)/* + m_ptEditLT*/;
			m_startPt = point;
			m_bDraw=TRUE;
			m_bFirstDraw=TRUE;
			//���õ���갴��ʱ��С�ľ��δ�С
			m_rectTracker.m_rect.SetRect(point.x,point.y,point.x + 4,point.y + 4);	
			
			//��֤����굱��ʱ������ʾ��Ϣ
			m_bShowMsg=TRUE;		
			DrawTip();
			PaintWindow();
		}
	}
	else
	{
		//��֤����굱��ʱ������ʾ��Ϣ
		m_bShowMsg=TRUE;		
		PaintWindow();
		
		if(m_bFirstDraw)
		{
			//������Сʱ,Track���Զ��������δ�С,��Щ�ڼ�,��Ϣ��CRectTracker�ڲ�����
			m_rectTracker.Track(this,point + m_ptEditLT,TRUE);
			//SendMessage(WM_LBUTTONUP,NULL,NULL);
			PaintWindow();

		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CCatchScreenDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	
    
	m_bShowMsg=FALSE;
	m_bDraw=FALSE;
	DrawTip();
	PaintWindow();

	//����ѡ����Σ�ʹ��Ϊ�����ϵ����µ�����
	int nTemp;
	if (m_rectTracker.m_rect.left > m_rectTracker.m_rect.right)
	{
		nTemp = m_rectTracker.m_rect.left;
		m_rectTracker.m_rect.left = m_rectTracker.m_rect.right;
		m_rectTracker.m_rect.right = nTemp;
	}
	if (m_rectTracker.m_rect.bottom < m_rectTracker.m_rect.top)
	{
		nTemp = m_rectTracker.m_rect.top;
		m_rectTracker.m_rect.top = m_rectTracker.m_rect.bottom;
		m_rectTracker.m_rect.bottom = nTemp;
	}
	CRect rectDraw ;
	rectDraw = m_rectTracker.m_rect + m_ptEditLT;
	
	CopyScreenToBitmap(rectDraw,TRUE);
	OnOK();
	CDialog::OnLButtonUp(nFlags, point);
}

void CCatchScreenDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	int nHitTest;
	nHitTest=m_rectTracker.HitTest(point);

    //������Ǿ����ڲ�˫��
	if(nHitTest==8)      
	{
		//����λͼ��ճ������,bSave ΪTRUE,
		CopyScreenToBitmap(m_rectTracker.m_rect,TRUE);
	    OnOK();
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CCatchScreenDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{	
	if(m_bFirstDraw)
	{
		//����Ѿ���ȡ���������ȡ����
		m_bFirstDraw=FALSE;
		//������δ�С
		m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
		DrawTip();
		PaintWindow();
		SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR8));  
		EndDialog(100);//wangxiaobin�ȹر�ģ̬��
	}
	else
	{
	   OnCancel();
	   EndDialog(100);
	}

	CDialog::OnRButtonUp(nFlags, point);
}

HBRUSH CCatchScreenDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	//���ò�����ʾ�����ı���ɫ
	/*if(pWnd->GetDlgCtrlID()==IDC_EDIT1)
	{
	pDC->SetTextColor(RGB(255,255,255));
	}*/
	return hbr;
}

BOOL CCatchScreenDlg::OnEraseBkgnd(CDC* pDC) 
{
	//�������������ȫ���Ի��򱳾�
	BITMAP bmp;
	m_pBitmap->GetBitmap(&bmp);

	CDC dcCompatible;
	dcCompatible.CreateCompatibleDC(pDC);

	dcCompatible.SelectObject(m_pBitmap);

	CRect rect;
	GetClientRect(&rect);
	pDC->BitBlt(0,0,rect.Width(),rect.Height(),&dcCompatible,0,0,SRCCOPY);

	return TRUE;
}

BOOL CCatchScreenDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	
	//���øı��ȡ���δ�Сʱ���
	if (pWnd == this &&m_rectTracker.SetCursor(this, nHitTest)
		             &&!m_bDraw &&m_bFirstDraw) //�˴��жϱ���ȡʱ����ʼ��Ϊ��ɫ���
    {
		return TRUE; 
	}

	//���ò�ɫ���
	SetCursor(m_hCursor);
	return TRUE;
}

//*********************���ӵĺ���**********************************************************
//������Ļ,��δ������������� �Ǻ�
HBITMAP CCatchScreenDlg::CopyScreenToBitmap(LPRECT lpRect,BOOL bSave)
{                                                                          //lpRect ����ѡ������
	HDC       hScrDC, hMemDC;       // ��Ļ���ڴ��豸������
	
	HBITMAP    hBitmap, hOldBitmap;    // λͼ���
	
	int       nX, nY, nX2, nY2;       // ѡ����������
	
	int       nWidth, nHeight;
	
	// ȷ��ѡ������Ϊ�վ���
	if (IsRectEmpty(lpRect))
		return NULL;
	//Ϊ��Ļ�����豸������
	hScrDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);

	//Ϊ��Ļ�豸�����������ݵ��ڴ��豸������
	hMemDC = CreateCompatibleDC(hScrDC);
	// ���ѡ����������
	nX = lpRect->left;
	nY = lpRect->top ;
	nX2 = lpRect->right ;
	nY2 = lpRect->bottom ;

	//ȷ��ѡ�������ǿɼ���
// 	if (nX < 0)
// 		nX = 0;
// 	if (nY < 0)
// 		nY = 0;
	if (nX2 > m_xScreen)
		nX2 = m_xScreen;
	if (nY2 > m_yScreen)
		nY2 = m_yScreen;
	nWidth = nX2 - nX;
	nHeight = nY2 - nY;

	// ����һ������Ļ�豸��������ݵ�λͼ
	hBitmap = CreateCompatibleBitmap
		(hScrDC, nWidth, nHeight);

	// ����λͼѡ���ڴ��豸��������
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	// ����Ļ�豸�����������ڴ��豸��������
	if(bSave)
	{
		//����������DC,��bSaveΪ��ʱ�ѿ�ʼ�����ȫ��λͼ,����ȡ���δ�С����

		CDC dcCompatible;
		dcCompatible.CreateCompatibleDC(CDC::FromHandle(hMemDC));
		dcCompatible.SelectObject(m_pBitmap);


		CImage img;
		
		img.Create(nWidth,nHeight,32);

		HDC hdc = img.GetDC();

		SetStretchBltMode(hdc,HALFTONE);

		BitBlt(hdc, 0, 0, nWidth, nHeight,
			dcCompatible, nX -m_ptEditLT.x, nY - m_ptEditLT.y, SRCCOPY);				
				
		CTime time = CTime::GetCurrentTime();			

		m_filePath = g_strAvconTempPath + /*"��ͼ"*/_T("catch");	


		if(!PathIsDirectory(g_strAvconTempPath))	
		{		
			if(!CreateDirectory(g_strAvconTempPath, NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strInterimFolderCreateFail;
				strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
				if (strInterimFolderCreateFail.IsEmpty())
				{
					strInterimFolderCreateFail = _T("��ʱ�ļ��д���ʧ��");
				} 
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strInterimFolderCreateFail);
				}
				return NULL;	
			}		
		}	
		//�ж���Դ���ļ����Ƿ���ڣ�û���򴴽�
		if(!PathIsDirectory(m_filePath.Left(m_filePath.GetLength() - 3)))
		{
			if(!CreateDirectory(m_filePath.Left(m_filePath.GetLength() - 3), NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strInterimFolderCreateFail;
				strInterimFolderCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("InterimFolderCreateFail"));
				if (strInterimFolderCreateFail.IsEmpty())
				{
					strInterimFolderCreateFail = _T("��ʱ�ļ��д���ʧ��");
				} 
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strInterimFolderCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strInterimFolderCreateFail);
				}
				else
				{
					AfxMessageBox(strInterimFolderCreateFail);
				}
				return NULL;	
			}
		}
		
		//�жϽ�ͼ�ļ����Ƿ���ڣ�û���򴴽�
		if(!PathIsDirectory(m_filePath))	
		{		
			if(!CreateDirectory(m_filePath, NULL))
			{
				CString strPrompt;
				strPrompt = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("Prompt"));
				if (strPrompt.IsEmpty())
				{
					strPrompt = _T("��ʾ");
				}
				CString strScrshotCreateFail;
				strScrshotCreateFail = ReadConfigText(_T("WhiteBoardMessageBoxText"), _T("ScrshotCreateFail"));
				if (strScrshotCreateFail.IsEmpty())
				{
					strScrshotCreateFail = _T("��ͼ�ļ��д���ʧ��");
				}
				if(g_bSupportU7OpenFile && g_pHPWBDataNotify)
				{
					//g_pHPWBDataNotify->MessageBox(NULL,strPrompt, strScrshotCreateFail, MB_OK,true);
					g_pHPWBDataNotify->ShowTipWindow(strScrshotCreateFail);
				}
				else
				{
					AfxMessageBox(strScrshotCreateFail);
				}
				return NULL;

			}		
		}	
		if (g_pWhiteBoardView == NULL)
		{
			return NULL;
		}
		m_filePath +=  _T("\\")+ time.Format(_T("%Y%m%d %H-%M-%S")) + _T(".jpg");
		
		img.Save(m_filePath);
		img.ReleaseDC();	
	}
	else
	{
		BitBlt(hMemDC, 0, 0, nWidth, nHeight,
			hScrDC, nX, nY, SRCCOPY);
	}

	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//�õ���Ļλͼ�ľ��
	//��� 
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	
	if(bSave)
	{
		if (OpenClipboard()) 
		{
        //��ռ�����
        EmptyClipboard();
        //����Ļ����ճ������������,
        //hBitmap Ϊ�ղŵ���Ļλͼ���
        SetClipboardData(CF_BITMAP, hBitmap);
        //�رռ�����
		CloseClipboard();
		}
	}
	// ����λͼ���

	return hBitmap;
}

//��ʾ������ʾ��Ϣ
void CCatchScreenDlg::DrawTip()
{
    //�õ�ǰ��������,
	CPoint pt;
	GetCursorPos(&pt);
	pt  = pt - m_ptEditLT;
	//������ǰR,G,B,������ֵ
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	CSize size;
	CFont *pFont = NULL;
	pFont = m_tipEdit.GetFont();
	dc.SelectObject(pFont);
	
	//����ʽ�ŷ��ַ���
	CString string;
	CString strTemp;
	CString strCurrentPixel;
	strCurrentPixel = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("CurrentPixel"));
	if (strCurrentPixel.IsEmpty())
	{
		strCurrentPixel = _T("��ǰ����RGB");
	}
	string.Format(_T("\r\n\r\n\r\n��")+strCurrentPixel+"(%d,%d,%d)\r\n",rValue,gValue,bValue);
   	
	if(!m_bDraw&&!m_bFirstDraw)
	{
		CString strLbuttonDown;
		strLbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonDown"));
		if (strLbuttonDown.IsEmpty())
		{
			strLbuttonDown = _T(" ������������ȷ����ȡ��Χ");
		}
		CString strLbuttonDownNext = _T("");
		strLbuttonDownNext = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonDownNext"));
		if (strLbuttonDownNext.IsEmpty())
		{
			strLbuttonDownNext = _T("");
		}
		CString strRbuttonDown;
		strRbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RbuttonDown"));
		if (strRbuttonDown.IsEmpty())
		{
			strRbuttonDown = _T("����������Ҽ��˳�");
		}
		strTemp.Format(_T("\r\n")+ strLbuttonDown+"\r\n\r\n"+strRbuttonDown);

		size=dc.GetTextExtent(strLbuttonDown,_tcslen(strLbuttonDown));
		
	}
	else if(m_bDraw&&m_bFirstDraw)
	{
		CString strLbuttonUp;
		strLbuttonUp = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonUp"));
		if (strLbuttonUp.IsEmpty())
		{
			strLbuttonUp = _T(" ���ɿ�������ȷ����ȡ��Χ");
		}
		CString strLbuttonUpNext = _T("");
		strLbuttonUpNext = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("LbuttonUpNext"));
		if (strLbuttonUpNext.IsEmpty())
		{
			strLbuttonUpNext = _T("");
		}
		
		CString strRbuttonDown;
		strRbuttonDown = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RbuttonDown"));
		if (strRbuttonDown.IsEmpty())
		{
			strRbuttonDown = _T("����������Ҽ��˳�");
		}
		//strTemp="\r\n ���ɿ�������ȷ����ȡ��Χ\r\n\r\n ����ESC���˳�";
		strTemp.Format(_T("\r\n")+ strLbuttonUp+"\r\n\r\n"+strRbuttonDown);
		size=dc.GetTextExtent(strLbuttonUp,_tcslen(strLbuttonUp));
		
	}
	else if(m_bFirstDraw)
	{
		//strTemp="\r\n ����������������ȡ��Χ��\r\n ��С��λ��\r\n\r\n ����ȡ��Χ��˫����������\r\n ��ͼ�񣬽�������\r\n\r\n ���������Ҽ�����ѡ��";
		strTemp=_T("\r\n ����������������ȡ��Χ��\r\n λ��\r\n\r\n ����ȡ��Χ��˫����������\r\n ��ͼ�񣬽�������\r\n\r\n ���������Ҽ�����ѡ��");
	}
	string+=strTemp;
	//��ʾ���༩����,������ʾ����
	CRect rect;
	m_tipEdit.GetWindowRect(&rect);
	//m_tipEdit.MoveWindow(m_ptEditLT.x+10,m_ptEditLT.y+10,size.cx + 28,rect.Height());
	m_tipEdit.SetWindowText(string);
	//m_tipEdit.UpdateWindow();
}

//��ʾ��ȡ������Ϣ
void CCatchScreenDlg::DrawMessage(CRect &inRect,CDC * pDC)
{
	//��ȡ���δ�С��Ϣ�������
	const int space=3;
    
	//����������ɫ��С
	
	CPoint pt;
	
	//CPen pen(PS_SOLID,1,RGB(147,147,147));

	//dc.SetTextColor(RGB(147,147,147));
	CFont font;
	CFont * pOldFont;
	CString strFont;
	strFont = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("Font"));
	if (strFont.IsEmpty())
	{
		strFont = _T("����");
	}
	CString strTopPosition;
	strTopPosition = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("TopPosition"));
	if (strTopPosition.IsEmpty())
	{
		strTopPosition = _T("����λ��");
	}
	CString strRectSize;
	strRectSize = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("RectSize"));
	if (strRectSize.IsEmpty())
	{
		strRectSize = _T("���δ�С");
	}
	CString strCoordinate;
	strCoordinate = ReadConfigText(_T("WhiteBoardCatchScreenText"), _T("Coordinate"));
	if (strCoordinate.IsEmpty())
	{
		strCoordinate = _T("�������");
	}
	font.CreatePointFont(90,strFont);
	pOldFont=pDC->SelectObject(&font);

	//�õ������Ⱥ͸߶�
	GetCursorPos(&pt);
	pt -= m_ptEditLT;
	int OldBkMode;
	OldBkMode=pDC->SetBkMode(TRANSPARENT);

	TEXTMETRIC tm;
	int charHeight;
	CSize size;
	int	lineLength;
	pDC->GetTextMetrics(&tm);
	charHeight = tm.tmHeight+tm.tmExternalLeading;
	size=pDC->GetTextExtent(strCoordinate,_tcslen(strCoordinate));
	lineLength=size.cx + 10;
    
	//��ʼ������, �Ա�֤д����������
	CRect rect(pt.x+space,pt.y-charHeight*6-space,pt.x+lineLength+space,pt.y-space);
    
    //������ʱ����
    CRect rectTemp;
	//�����ε��������Եʱ��������ʹ�С
	CPoint ptTempt;
	GetCursorPos(&ptTempt);
	if((ptTempt.x+rect.Width())>=m_xScreen)
	{
		//�����Ϸ���ʾ���¾���
		rectTemp=rect;
		rectTemp.left=rect.left-rect.Width()-space*2;
		rectTemp.right=rect.right-rect.Width()-space*2;;
		rect=rectTemp;
	}

	if((ptTempt.y-rect.Height())<=0)
	{
		//�����ҷ���ʾ���¾���
		rectTemp=rect;
		rectTemp.top=rect.top+rect.Height()+space*2;;
		rectTemp.bottom=rect.bottom+rect.Height()+space*2;;
		rect=rectTemp;
		
	}

	//�����ջ�ˢ������
	CBrush * pOldBrush;
    pOldBrush=pDC->SelectObject(CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH)));
	
	pDC->Rectangle(rect);
   	rect.top+=2;
	//�ھ�������ʾ����
	CRect outRect(rect.left,rect.top,rect.left+lineLength,rect.top+charHeight);
	CString string(strTopPosition);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	outRect.SetRect(rect.left,rect.top+charHeight,rect.left+lineLength,charHeight+rect.top+charHeight);
	string.Format(L"(%d,%d)",inRect.left,inRect.top);
	pDC->DrawText(string,outRect,DT_CENTER);
	

	outRect.SetRect(rect.left,rect.top+charHeight*2,rect.left+lineLength,charHeight+rect.top+charHeight*2);
	string=strRectSize;
	pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*3,rect.left+lineLength,charHeight+rect.top+charHeight*3);
	string.Format(L"(%d,%d)",inRect.Width(),inRect.Height());
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*4,rect.left+lineLength,charHeight+rect.top+charHeight*4);
	string=strCoordinate;
    pDC->DrawText(string,outRect,DT_CENTER);

	outRect.SetRect(rect.left,rect.top+charHeight*5,rect.left+lineLength,charHeight+rect.top+charHeight*5);
	string.Format(L"(%d,%d)",pt.x,pt.y);
	pDC->DrawText(string,outRect,DT_CENTER);
    
	pDC->SetBkMode(OldBkMode);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldBrush);

	font.DeleteObject();
	
}
//�ػ�����
void CCatchScreenDlg::PaintWindow()
{
	//��ȡ��ȫ���Ի��򴰿ڴ�С
	CRect rect1;
	//GetWindowRect(rect1);
	GetClientRect(&rect1);

	//��ȡ�༭�򴰿ڴ�С
	CRect rect2;
	//m_tipEdit.GetWindowRect(rect2);
	m_tipEdit.GetClientRect(&rect2);

	CRgn rgn1,rgn2;
	rgn1.CreateRectRgnIndirect(rect1);
	rgn2.CreateRectRgnIndirect(rect2);

	//��ȡ��������,���ǳ��˱༭�򴰿ڲ�����
	m_rgn.CombineRgn(&rgn1,&rgn2,RGN_DIFF);
	
	InvalidateRgn(&m_rgn);

	rgn1.DeleteObject();
	rgn2.DeleteObject();
}
//�ı������ʾ���ڵ�RGBֵ
void CCatchScreenDlg::ChangeRGB()
{
	//����ɵ�RGBֵ�ַ���
	static CString strOld("");

	CPoint pt;
	GetCursorPos(&pt);
	pt += m_ptEditLT;
	//������ǰR,G,B,������ֵ
	COLORREF color;
	CClientDC dc(this);
	color=dc.GetPixel(pt);
	BYTE rValue,gValue,bValue;
	rValue=GetRValue(color);
	gValue=GetGValue(color);
	bValue=GetGValue(color);
	
	//����ʽ�ŷ��ַ���
	CString string;
	string.Format(L"(%d,%d,%d)",rValue,gValue,bValue);
	//�����ǰ��ɫû����ˢ��RGBֵ,���ⴰ���и�����˸
    if(strOld!=string)
	{
	    //�õ�RGB�ı���һ�е��ı�����
		int LineLength=m_tipEdit.LineLength(6);
		//��ѡRGBֵ�ı�,Ҳ����(255,255,255)��ʽ
	    m_tipEdit.SetSel(20,LineLength+6);
        
		//�滻RGB����
		m_tipEdit.ReplaceSel(string);
    }
	
	strOld=string;

}

void CCatchScreenDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//MessageBox(_T("����ONCHAR"));
	if (nChar == VK_ESCAPE)
	{
		if(m_bFirstDraw)
		{
			//����Ѿ���ȡ���������ȡ����
			m_bFirstDraw=FALSE;
			//������δ�С
			m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
			DrawTip();
			PaintWindow();
		}
		else
		{
			OnCancel();
		}
	}
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CCatchScreenDlg::OnStnClickedEdit1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


//BOOL CCatchScreenDlg::PreTranslateMessage(MSG* pMsg)
//{
//	// TODO: �ڴ����ר�ô����/����û���
//	if   (pMsg->message == WM_KEYDOWN) 
//	{ 
//		switch(pMsg->wParam)   
//		{ 
//		case   VK_ESCAPE: 
//			{
//				if(m_bFirstDraw)
//				{
//					//����Ѿ���ȡ���������ȡ����
//					m_bFirstDraw=FALSE;
//					//������δ�С
//					m_rectTracker.m_rect.SetRect(-1,-1,-1,-1);
//					DrawTip();
//					PaintWindow();
//				}
//				else
//				{
//					OnCancel();
//				}
//			}
//			return   true; 
//		default: 
//			return   true; 
//		} 
//	} 
//
//	return CDialog::PreTranslateMessage(pMsg);
//}
void CCatchScreenDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnRButtonDown(nFlags, point);
}

void CCatchScreenDlg::GetDisplayInfo()
{
	int nDispDivIndex = 0;
	int index = 0;
	DEVMODE devmode;
	DISPLAY_DEVICE dev = {sizeof(DISPLAY_DEVICE)};
	BOOL bNext = FALSE;

	while (true)
	{
		bNext = EnumDisplayDevices(NULL,index,&dev,0);
		if (bNext == FALSE)
		{
			break;
		}

		BOOL BTRUE =::EnumDisplaySettings(dev.DeviceName,ENUM_CURRENT_SETTINGS, &devmode);
		/*if(BTRUE )
		{
			CString stroutput2;
		
			stroutput2.Format(_T("ssssssssssssssssss\n"));		
			OutputDebugString(stroutput2);
		}
		else
		{
			CString stroutput2;
		
			stroutput2.Format(_T("ppppppppppppppppppppppppp\n"));		
			OutputDebugString(stroutput2);
		}*/
		int x = devmode.dmPosition.x;
		int y = devmode.dmPosition.y;
		DWORD cx = devmode.dmPelsWidth +x;
		DWORD cy = devmode.dmPelsHeight +y;
		/*CString stroutput;
		
		stroutput.Format(_T("%d\n"),y);		
		OutputDebugString(stroutput);
		stroutput.Format(_T("%d\n"),cx);		
		OutputDebugString(stroutput);
		stroutput.Format(_T("%d\n"),cy);		
		OutputDebugString(stroutput);*/
		if (m_nscreenLeft >= x && x > -10000)
		{
			m_nscreenLeft = x;
			/*stroutput.Format(_T("%d\n"),m_nscreenLeft);		
			OutputDebugString(stroutput);*/
		}
		if (m_nscreenTop >= y && y> -10000)
		{
			m_nscreenTop = y;
			/*stroutput.Format(_T("%d\n"),m_nscreenTop);		
			OutputDebugString(stroutput);*/
		}
// 		m_xScreen += cx;
// 		m_yScreen += cy;
		if (m_xScreen <= cx && cx > 0 && cx < 10000)
		{
			m_xScreen =cx;
			/*stroutput.Format(_T("%d\n"),m_xScreen);		
			OutputDebugString(stroutput);*/
		}
		if (m_yScreen <= cy && cy >0 && cy < 10000)
		{
			m_yScreen = cy;
			/*stroutput.Format(_T("%d\n"),m_yScreen);		
			OutputDebugString(stroutput);*/
		}
		index++;

	}
	/*CString stroutputw;
	stroutputw.Format(_T("%d\n"),m_nscreenLeft);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_nscreenTop);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_xScreen);		
	OutputDebugString(stroutputw);
	stroutputw.Format(_T("%d\n"),m_yScreen);		
	OutputDebugString(stroutputw);*/
}
