// WBRreviewDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "whiteBoard.h"
#include "WBPreviewDlg.h"
#include "afxdialogex.h"
#include "whiteBoard.h"
#include "common/WBData.h"
#include "WBFunctionDlg.h"
// CWBRreviewDlg �Ի���

IMPLEMENT_DYNAMIC(CWBPreviewDlg, CDialogEx)

CWBPreviewDlg::CWBPreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBPreviewDlg::IDD, pParent)
	,m_pageCount(0)
	,m_pPreviewView(NULL)
	,m_LastPreView(NULL)
	,m_pViewList(NULL) 
	,m_bAlreadyOnInit(false)
	,m_uOldCourrtPageIndex(0)
	,m_ViewListWidht(VIEWWIDTH)
	,m_bReduceListWidth(false)
{
	m_pViewList = new CObList();
	m_RectTempPriview.SetRect(0,0,0,0);
	//m_viewPosRt.SetRect(5, 0,(VIEWWIDTH + 5),(int)(VIEWHIGHT));
	//m_NewButtonRect.SetRect((VIEWWIDTH + 5),5,(int)(VIEWHIGHT + 34),34);
	m_viewPosRt.SetRect(5, 0, (VIEWWIDTH + 5), (int)(VIEWHIGHT));
	m_NewButtonRect.SetRect((VIEWWIDTH + 5), 5, (int)(VIEWWIDTH + 34), 34);
	m_bNeedRefesh = false;

}

CWBPreviewDlg::CWBPreviewDlg(CRect rect,CWnd* pParent /*=NULL*/)
	: CDialogEx(CWBPreviewDlg::IDD, pParent)
	,m_pageCount(0)
	,m_pPreviewView(NULL)
	,m_LastPreView(NULL)
	,m_pViewList(NULL) 
	,m_bAlreadyOnInit(false)
	,m_uOldCourrtPageIndex(0)
	,m_rect(rect)
	,m_ViewListWidht(VIEWWIDTH)
	,m_bReduceListWidth(false)
{
	m_pViewList = new CObList();
	m_RectTempPriview.SetRect(0,0,0,0);
	
	//m_viewPosRt.SetRect(5,(int)(-VIEWHIGHT),(VIEWWIDTH + 5),0);
	m_viewPosRt.SetRect(5, 0, (VIEWWIDTH + 5), (int)(VIEWHIGHT));
	//m_NewButtonRect.SetRect((VIEWWIDTH + 5), 5, (int)(VIEWHIGHT + 34), 34);
	m_NewButtonRect.SetRect((VIEWWIDTH + 5), 5, (int)(VIEWWIDTH + 34), 34);
	m_bNeedRefesh = false;


}

CWBPreviewDlg::~CWBPreviewDlg()
{
	if (NULL != m_pViewList)
	{
		delete m_pViewList;
		m_pViewList = NULL;
	}	
}

void CWBPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWBPreviewDlg, CDialogEx)
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnPreviewBtnClicked)
	ON_MESSAGE(WM_WB_FUNCTIONVIEWSYN,&CWBPreviewDlg::OnNewViewSynchronizer)
	ON_COMMAND(IDC_NEWPAGE, &CWBPreviewDlg::OnWbNewpageb)
	ON_COMMAND(IDC_PRAGPAGE, &CWBPreviewDlg::OnWbForepageb)
	ON_COMMAND(IDC_NEXTPAGE, &CWBPreviewDlg::OnNextpage)
	ON_COMMAND(IDC_FIRSTPAGE, &CWBPreviewDlg::OnFirstpage)
	ON_COMMAND(IDC_LASTPAGE, &CWBPreviewDlg::OnWbLastpageb)
	ON_COMMAND(IDC_INSERTPAGEB, &CWBPreviewDlg::OnInsertpageb)
	ON_COMMAND(IDC_DELPAGE, &CWBPreviewDlg::OnDelpage)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_WB_FUNCTION_ADDPREVIEW, &CWBPreviewDlg::OnWbFunction_AddPreview)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CWBRreviewDlg ��Ϣ�������


BOOL CWBPreviewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DisableTabFeedback(GetSafeHwnd());
	CWBPage *WBPage;
	m_pageCount = g_pWBFile->GetPageCount();
	UINT uCurPageIndex = g_pWBFile->GetCurPageIndex();
	CDC *pdc = g_pWhiteBoardView->GetDC();
	if (pdc == NULL)
	{
		return FALSE;
	}
	for (UINT i = 0 ;i < m_pageCount ; i++)
	{
		m_pPage = g_pWBFile->GetPage(i+1);
		m_pPreviewView = new CWBPreviewView(m_hWnd,m_pPage);
		if (NULL == m_pPreviewView)
		{
			return FALSE;
		}	

		if (NULL == g_pWhiteBoardView)
		{
			return FALSE;
		}

		m_viewPosRt.top = m_viewPosRt.top + /*(LONG)VIEWHIGHT +*/ 10;
		m_viewPosRt.bottom = m_viewPosRt.top + (LONG)VIEWHIGHT;
		m_pPreviewView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW,m_viewPosRt,this,NULL);

		m_pViewList->AddTail(m_pPreviewView);
		m_pPreviewView->SetCurPageIndex(i+1);
		m_pPreviewView->Invalidate(TRUE);
		if ( !m_bAlreadyOnInit )
		{		//�����ļ�
			g_pWBFile->GotoPage(i+1);
			WBPage = g_pWBFile->GetCurPage();
			WBPage->Draw(pdc);	
		}	
	}

	g_pWhiteBoardView->ReleaseDC(pdc);
	ShowButtonPlace();
	//g_pWBFile->GotoPage(uCurPageIndex);
	g_pWhiteBoardView->GotoPage(uCurPageIndex);

	//����������Ϣ����
	m_si.cbSize = sizeof (SCROLLINFO);
	m_si.fMask = SIF_ALL;
	m_si.nMin = 0;
	m_si.nMax = m_viewPosRt.bottom -20;

	m_si.nPage = m_rect.Height();   //������һ��ҳ�ĸ߶�
	m_si.nPos = 0;
	SetScrollInfo(SB_VERT,&m_si,1);

	if (m_rect.bottom < m_viewPosRt.bottom) //����Ի���ĸ߶�С����������ͼ�ĵײ�����ʾ������
	{
		ShowScrollBar(SB_VERT,TRUE);
	}
	else
	{
		ShowScrollBar(SB_VERT,FALSE);
	}
	m_bLineDown = TRUE;
	m_bLineUp = TRUE;
	// ���ص����˵�ͼƬ
	NewPageBmp.LoadBitmap(IDB_PREVIEWDLG_NEWPAGE);//�½�ҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&NewPageBmp);
	PrePageBmp.LoadBitmap(IDB_PREVIEWDLG_PREPAGE);//��һҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&PrePageBmp);
	NextPageBmp.LoadBitmap(IDB_PREVIEWDLG_NEXTPAGE);//��һҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&NextPageBmp);
	FirstPageBmp.LoadBitmap(IDB_PREVIEWDLG_FIRSTPAGE);//��ҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&FirstPageBmp);
	LastPageBmp.LoadBitmap(IDB_PREVIEWDLG_LASTPAGE);//ĩҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&LastPageBmp);
	INSERTPageBmp.LoadBitmap(IDB_PREVIEWDLG_INSERTPAGE);//����ҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&INSERTPageBmp);
	DELETEPageBmp.LoadBitmap(IDB_PREVIEWDLG_DELETEPAGE);//ɾ��ҳ
	g_pWhiteBoardView->TranslateBitmapSizeToFitXP(&DELETEPageBmp);
	
	Invalidate(TRUE);
	UpdateWindow();
	return TRUE;  
}

void CWBPreviewDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO scrollinfo;
	GetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
	switch (nSBCode)  
	{  
	case SB_BOTTOM: 
		ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMax)); 
		scrollinfo.nPos = scrollinfo.nMax;  
		SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		break;  
	case SB_TOP:  
		ScrollWindow(0,(scrollinfo.nPos-scrollinfo.nMin)); 
		scrollinfo.nPos = scrollinfo.nMin; 
		SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
		break; 
	case SB_LINEUP:  
		{	
			m_bLineDown = TRUE;
			scrollinfo.nPos -= 20;
			if (scrollinfo.nPos<scrollinfo.nMin)
			{ 
				if (m_bLineUp)
				{
					ScrollWindow(0,(20 + scrollinfo.nPos));
					m_bLineUp = FALSE;
				}
				scrollinfo.nPos = scrollinfo.nMin;  
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
				break;
			} 
			else
			{
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
				ScrollWindow(0,20);
				break; 
			}
		}
	case SB_LINEDOWN:
		{
			m_bLineUp = TRUE;
			scrollinfo.nPos += 20; 
			if (scrollinfo.nPos > (int)(scrollinfo.nMax - scrollinfo.nPage))  
			{ 

				if (m_bLineDown)
				{	
					int a = scrollinfo.nPos - (scrollinfo.nMax - scrollinfo.nPage)-1;
					ScrollWindow(0,-(20 - a )); 
					m_bLineDown = FALSE;
				}
				//SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				//ScrollWindow(0,-(30 - (scrollinfo.nPos - (scrollinfo.nMax - scrollinfo.nPage)))); 
				scrollinfo.nPos = scrollinfo.nMax; 
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				break;  
			}
			else
			{
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				ScrollWindow(0,-20); 
				break;
			}
		}
	case SB_PAGEUP: 
		{	
			m_bLineDown = TRUE;
			UINT a = scrollinfo.nPos;
			scrollinfo.nPos -= scrollinfo.nPage;				
			if (scrollinfo.nPos<scrollinfo.nMin)
			{ 
				scrollinfo.nPos = scrollinfo.nMin; 
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL); 
				ScrollWindow(0,a); 
				break;
			} 
			else
			{
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				ScrollWindow(0,scrollinfo.nPage); 
				break; 
			}	  
		}
	case SB_PAGEDOWN: 			
		{	
			m_bLineUp = TRUE;
			UINT a = scrollinfo.nPos;
			scrollinfo.nPos += scrollinfo.nPage ;  
			if (scrollinfo.nPos>(int)(scrollinfo.nMax - scrollinfo.nPage)) 	
			{   
				int offset = 0;
				offset =  scrollinfo.nMax - scrollinfo.nPos ;
				scrollinfo.nPos = scrollinfo.nMax;
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				ScrollWindow(0,-offset); 
				break;  
			}
			else
			{
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);  
				ScrollWindow(0,-((int)scrollinfo.nPage)); 
				break; 
			}	
		}
	case SB_ENDSCROLL: 
		break; 
	case SB_THUMBPOSITION:  
		break;  
	case SB_THUMBTRACK:  
		{	
			m_bLineUp = TRUE;
			m_bLineDown = TRUE;
			//У׼
			int offset = (int)nPos -(int)scrollinfo.nPos;
			if (offset > 0)//�����ƶ�
			{
				if ( (nPos + scrollinfo.nPage) > scrollinfo.nMax )//Խ�����ֵ
				{
					offset = (scrollinfo.nMax -scrollinfo.nPage + 1) - scrollinfo.nPos;
				}
				ScrollWindow(0,-offset); 
				scrollinfo.nPos = scrollinfo.nPos + offset; 
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
			}
			else if (offset < 0)
			{
				if (nPos < scrollinfo.nMin)
				{
					offset = scrollinfo.nMin - scrollinfo.nPos;
				}
				ScrollWindow(0,-offset); 
				scrollinfo.nPos = scrollinfo.nPos + offset; 
				SetScrollInfo(SB_VERT,&scrollinfo,SIF_ALL);
			}			    
			break; 
		}
	}
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CWBPreviewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
}

LRESULT  CWBPreviewDlg::OnNewViewSynchronizer(WPARAM wParam, LPARAM lParam)
{
	if ((1 == g_pWBFile->GetPageCount()) | (NULL == m_LastPreView ))  ////��ֻ��һҳʱ����һ�ε��Ǹ�Ҳ�͵�ǰҳ��һ����
	{
		SetLastPreview();
	}
	if (!::IsWindow(m_LastPreView->GetSafeHwnd()))
	{
		return FALSE;
	}
	m_LastPreView->Invalidate(TRUE);
	if (m_pViewList->GetCount() < (int)g_pWBFile->GetPageCount() )
	{
		return FALSE;
	}
	int PreScrollPos,NextScrollPos;     //��������λ��
	//CRect NewViewRt;   //�²�����view���ɵ�λ��
	BOOL bNewPreView;//�Ƿ�����µ�ҳ
	CWBPreviewView * PPreviewView;
	POSITION pos;
	CRect NewButtonRect,LastPreviewRect; //�²����İ�ť���ɵ�λ��
	bNewPreView = FALSE;

	UINT nCurPage_Num =	g_pWBFile->GetCurPageIndex();
	pos = m_pViewList->FindIndex(nCurPage_Num-1); //�����ļ��еĵ�ǰҳ���ҵ���ҳ��Ӧ��m_pViewList�����б��е�λ��
	if (NULL == pos)
	{
		return FALSE;
	}
	PPreviewView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
	if (NULL == PPreviewView)
	{
		return FALSE;
	}
	PPreviewView->SetCurPageIndex(g_pWBFile->GetCurPageIndex());
	m_LastPreView->GetWindowRect(&LastPreviewRect);//�����һ��ҳ���Priviewdlg������
	ScreenToClient(&LastPreviewRect);	

	if (bNewPreView)
	{
		g_pWhiteBoardView->setCurrentViewScroll(g_pWBFile->GetCurPage()->GetPageViewScroll());//���ðװ�Ĺ�������λ��
	}
	PPreviewView->SetRectInPreview(); //����Ԥ�����еľ��ο��λ��

	m_LastPreView = PPreviewView;

	PPreviewView->GetWindowRect(&NewButtonRect);//��õ�ǰҳ���Priviewdlg������
	ScreenToClient(&NewButtonRect);

	//////////////////////////////////////////////ҳ����ʱ���������ƶ�//////////////////////////////////////////
	if (((m_uOldCourrtPageIndex != 0)&&(m_uOldCourrtPageIndex != nCurPage_Num))||( !m_bAlreadyOnInit ))
	{
		m_bAlreadyOnInit = true;
		PreScrollPos = GetScrollPos(SB_VERT);  //��õ�ǰ��������λ��
		/*	GetScrollInfo(SB_VERT,&m_si,SIF_ALL); */
		if (m_rect.bottom < NewButtonRect.bottom )
		{
			m_si.nPos = PreScrollPos;
			m_si.nPos = m_si.nPos + NewButtonRect.bottom - m_rect.bottom;
			if (m_si.nPos >= m_si.nMax)
			{
				m_si.nPos = m_si.nMax;
			}		
			SetScrollInfo(SB_VERT,&m_si,1); 
			//		 ShowScrollBar(SB_VERT,TRUE);
			NextScrollPos = GetScrollPos(SB_VERT); //������ view֮��Ĺ�����λ��
			ScrollWindow(0,-(NextScrollPos - PreScrollPos));
		}
		if (m_rect.top > NewButtonRect.top )
		{
			m_si.nPos = PreScrollPos;
			if (NewButtonRect.top <=0)
			{
				m_si.nPos = m_si.nPos + NewButtonRect.top;
			}
			if ( m_si.nMax - m_si.nPos <= m_si.nPage)
			{
				m_si.nPos =m_si.nMax - m_si.nPage;
			}
			SetScrollInfo(SB_VERT,&m_si,1); 
			//		ShowScrollBar(SB_VERT,TRUE);
			NextScrollPos = GetScrollPos(SB_VERT); //������view֮��Ĺ�����λ��
			ScrollWindow(0,-(NextScrollPos - PreScrollPos));

		}
	}
	m_uOldCourrtPageIndex = nCurPage_Num;

	////////////////////////////////////���Ԥ�����ұߵİ�ť////////////////////////////////////////////////////
	PPreviewView->GetWindowRect(&NewButtonRect);
	ScreenToClient(&NewButtonRect);
	m_NewButtonRect.left = NewButtonRect.Width() + 4;
	m_NewButtonRect.top = NewButtonRect.top;
	m_NewButtonRect.right = NewButtonRect.Width() + 32;
	m_NewButtonRect.bottom = NewButtonRect.top+ 30;
	ShowButtonPlace();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	PPreviewView->Invalidate(TRUE);
	return 0;
}

void CWBPreviewDlg::SetLastPreview(void)
{
	POSITION pos;
	pos = m_pViewList->FindIndex(0); 
	if (pos == NULL)
	{
		return;
	}
	m_LastPreView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��	
}

void CWBPreviewDlg::ShowButtonPlace(void) //��ʾ�ұߵİ�ťλ��
{
	if (! m_BtnRightDown.m_hWnd)
	{
		m_BtnRightDown.Create(_T(" "),WS_VISIBLE |WS_CHILD | BS_OWNERDRAW ,m_NewButtonRect,this ,IDC_BTN_PREVIEW);
		m_BtnRightDown.ShowWindow(SW_SHOW);
	} 
	m_BtnRightDown.MoveWindow(m_NewButtonRect);
}

void CWBPreviewDlg::OnPreviewBtnClicked() //����ұߵİ�ť ���������˵�
{
	if (!IsWindow( m_BtnRightDown.m_hWnd))
	{
		return;
	}
	CMenu menu,*pSubMenu;
	
	menu.LoadMenu(IDR_PREVIEWMENU);//װ���Զ�����Ҽ��˵�  
	pSubMenu=menu.GetSubMenu(0);
	//���������ļ���ȡ�����ò˵��ϵ����԰汾
	CString MenuText;
	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_PrePage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("��һҳ");
	}
	pSubMenu->ModifyMenu(IDC_PRAGPAGE,MF_BYCOMMAND,IDC_PRAGPAGE,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_NextPage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("��һҳ");
	}
	pSubMenu->ModifyMenu(IDC_NEXTPAGE,MF_BYCOMMAND,IDC_NEXTPAGE,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_FirstPage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("��ҳ");
	}
	pSubMenu->ModifyMenu(IDC_FIRSTPAGE,MF_BYCOMMAND,IDC_FIRSTPAGE,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_LastPage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("ĩҳ");
	}
	pSubMenu->ModifyMenu(IDC_LASTPAGE,MF_BYCOMMAND,IDC_LASTPAGE,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_DeletePage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("ɾ��ҳ");
	}
	pSubMenu->ModifyMenu(IDC_DELPAGE,MF_BYCOMMAND,IDC_DELPAGE,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_InsertPage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("����ҳ");
	}
	pSubMenu->ModifyMenu(IDC_INSERTPAGEB,MF_BYCOMMAND,IDC_INSERTPAGEB,MenuText);

	MenuText = ReadConfigText(_T("WhiteBoardMenuListText"), _T("Preview_NewPage"));
	if (MenuText.IsEmpty())
	{
		MenuText = _T("�½�ҳ");
	}
	pSubMenu->ModifyMenu(IDC_NEWPAGE,MF_BYCOMMAND,IDC_NEWPAGE,MenuText);


	UINT nCurPage_Num =	g_pWBFile->GetCurPageIndex();
	UINT nPage_Counts = m_pViewList->GetCount(); 
	if(nPage_Counts == 1)
	{
		if (MF_GRAYED != pSubMenu->GetMenuState(IDC_PRAGPAGE,MF_BYCOMMAND)
			// 			|| MF_GRAYED != pSubMenu->GetMenuState(IDC_FIRSTPAGE,MF_BYCOMMAND)
			// 			|| MF_GRAYED != pSubMenu->GetMenuState(IDC_LASTPAGE,MF_BYCOMMAND)
			// 			|| MF_GRAYED != pSubMenu->GetMenuState(IDC_NEXTPAGE,MF_BYCOMMAND)
			)
		{
			pSubMenu->EnableMenuItem(IDC_PRAGPAGE,MF_GRAYED);
			pSubMenu->EnableMenuItem(IDC_FIRSTPAGE,MF_GRAYED);
			pSubMenu->EnableMenuItem(IDC_LASTPAGE,MF_GRAYED);
			pSubMenu->EnableMenuItem(IDC_NEXTPAGE,MF_GRAYED);
		}
	}
	else
	{
		if (nCurPage_Num == 1)
		{
			pSubMenu->EnableMenuItem(IDC_PRAGPAGE,MF_GRAYED);
			pSubMenu->EnableMenuItem(IDC_FIRSTPAGE,MF_GRAYED);
		}
		else
		{
			if (nCurPage_Num == nPage_Counts)
			{
				pSubMenu->EnableMenuItem(IDC_LASTPAGE,MF_GRAYED);
				pSubMenu->EnableMenuItem(IDC_NEXTPAGE,MF_GRAYED);
			}
			else
			{
				if (MF_GRAYED == pSubMenu->GetMenuState(IDC_PRAGPAGE,MF_BYCOMMAND))
				{
					pSubMenu->EnableMenuItem(IDC_PRAGPAGE,MF_ENABLED);
					pSubMenu->EnableMenuItem(IDC_FIRSTPAGE,MF_ENABLED);
					pSubMenu->EnableMenuItem(IDC_LASTPAGE,MF_ENABLED);
					pSubMenu->EnableMenuItem(IDC_NEXTPAGE,MF_ENABLED);
				}
			}
		}
	}	
	CPoint oPoint;//����һ������ȷ�����λ�õ�λ��  
	GetCursorPos(&oPoint);//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����  

	pSubMenu->SetMenuItemBitmaps(IDC_NEWPAGE, MF_BYCOMMAND, &NewPageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_PRAGPAGE, MF_BYCOMMAND, &PrePageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_NEXTPAGE, MF_BYCOMMAND, &NextPageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_FIRSTPAGE, MF_BYCOMMAND, &FirstPageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_LASTPAGE, MF_BYCOMMAND, &LastPageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_INSERTPAGEB, MF_BYCOMMAND, &INSERTPageBmp,NULL);	
	pSubMenu->SetMenuItemBitmaps(IDC_DELPAGE, MF_BYCOMMAND, &DELETEPageBmp,NULL);

	
	//����δ�����ʱ����
	//menu.GetSubMenu(0)->DeleteMenu(6,MF_BYPOSITION);     //�½�ҳ
	menu.GetSubMenu(0)->DeleteMenu(5,MF_BYPOSITION);     //����ҳ
	menu.GetSubMenu(0)->DeleteMenu(4,MF_BYPOSITION);     //ɾ��ҳ
	//menu.GetSubMenu(0)->DeleteMenu(3,MF_BYPOSITION);     //���ҳ
	//menu.GetSubMenu(0)->DeleteMenu(2,MF_BYPOSITION);     //��һҳ
	//menu.GetSubMenu(0)->DeleteMenu(1,MF_BYPOSITION);       //��һҳ
	//menu.GetSubMenu(0)->DeleteMenu(0,MF_BYPOSITION);     //��һҳ
	

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN,oPoint.x,oPoint.y,this);   //��ָ��λ����ʾ�����˵�
	pSubMenu->Detach();
	menu.Detach();
} 
///////////////////�ұ߰�ť�����˵���Ӧ����///////////////////////////////////////////
void CWBPreviewDlg::OnWbNewpageb()   //�����˵��е��½�һҳ
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL ==CurentPagePreview)
	{
		return ;
	}
	//WB_SetDrawStatus( DRAW_NORMAL);
	g_pWBPage->ClearSelectFrameList();
	WB_AddNewPage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnWbForepageb()
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL == CurentPagePreview)
	{
		return ;
	}	
	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_PrePage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnNextpage()
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL == CurentPagePreview)
	{
		return ;
	}
	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_NextPage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnFirstpage()
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL == CurentPagePreview)
	{
		return ;
	}
	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_FirstPage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnWbLastpageb()
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL == CurentPagePreview)
	{
		return ;
	}
	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_LastPage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnInsertpageb()
{
	CWBPreviewView * CurentPagePreview = NULL;
	CurentPagePreview = GetIndexPreviewPage(g_pWBFile->GetCurPageIndex());
	if (NULL == CurentPagePreview)
	{
		return;
	}

	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_InsertNewPage();
	CurentPagePreview->Invalidate();
}

void CWBPreviewDlg::OnDelpage()
{
	// TODO: �ڴ���������������
	//WB_SetDrawStatus( DRAW_NORMAL);
	WB_DeletePage();
	/*	RefurbishPreview();*/
}

BOOL CWBPreviewDlg::RefurbishPreview(void)  //��������Ԥ����
{
	if (NULL == m_pViewList)
	{
		return FALSE;
	}
	CWBPreviewView *PView;
	//CRect LastViewRect;
	int NextScrollPos;

	m_viewPosRt.SetRect(5,(int)(-VIEWHIGHT),(m_ViewListWidht + 5),0);

	for(int viewCount = 0; viewCount < m_pViewList->GetCount();viewCount++)
	{
		POSITION pos = m_pViewList->FindIndex(viewCount);
		if (NULL == pos)
		{
			continue;
		}
		PView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
		if(NULL == PView)
			continue;
		if (NULL != PView)
		{
			PView ->DestroyWindow();
			//	delete PView;
		}
	}
	m_pViewList->RemoveAll(); 
	CWBPage *WBPage = NULL;
	CDC *pdc = g_pWhiteBoardView->GetDC();
	UINT uCurPageIndex = g_pWBFile->GetCurPageIndex();
	m_pageCount = g_pWBFile->GetPageCount();
	for (UINT i = 0 ;i < m_pageCount ; i++)
	{
		//m_pPreviewView = (CWBPreviewView*) RUNTIME_CLASS(CWBPreviewView)->CreateObject();
		m_pPage = g_pWBFile->GetPage(i+1);
		m_pPreviewView = new CWBPreviewView(m_hWnd,m_pPage);
		if (NULL == m_pPreviewView)
		{
			return FALSE;
		}		
		m_viewPosRt.top = m_viewPosRt.top + (LONG)VIEWHIGHT + 10;
		m_viewPosRt.bottom = m_viewPosRt.top + (LONG)VIEWHIGHT;
		m_pPreviewView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW,m_viewPosRt,this,NULL);
		m_pViewList->AddTail(m_pPreviewView);
		m_pPreviewView->SetCurPageIndex(i+1);
		if (m_bNeedRefesh)
		{		//�����ļ�
			g_pWBFile->GotoPage(i+1);
			WBPage = g_pWBFile->GetCurPage();
			WBPage->Draw(pdc);	
		}	
		//m_pPreviewView->Invalidate();		
	}
	g_pWhiteBoardView->ReleaseDC(pdc);
	ShowButtonPlace();      //���ð�ť��λ��
	g_pWhiteBoardView->GotoPage(uCurPageIndex);	
	//����������Ϣ����
	m_si.cbSize = sizeof(SCROLLINFO);
	m_si.fMask = SIF_ALL;
	m_si.nMin = 0;
	m_si.nMax = m_viewPosRt.bottom -20;
	m_si.nPage = m_rect.Height();
	m_si.nPos = 0 ; 
	//����������Ϣ����
	SetScrollInfo(SB_VERT,&m_si,1);
	int CurPageIndex = g_pWBFile->GetCurPageIndex();
	CRect rect;
	POSITION pos;
	CWBPreviewView *PPreviewView;
	pos = m_pViewList->FindIndex(CurPageIndex-1); //�����ļ��еĵ�ǰҳ���ҵ���ҳ��Ӧ��m_pViewList�����б��е�λ��
	if (pos == NULL)
	{
		return FALSE;
	}
	PPreviewView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
	if (PPreviewView == NULL)
	{
		return FALSE;
	}
	PPreviewView->SetCurPage(g_pWBFile->GetCurPage());
	PPreviewView->GetWindowRect(&rect);//��õ�ǰҳ���Priviewdlg������
	ScreenToClient(&rect);
	m_si.nPos = rect.top; 
	if (m_si.nPos > (int)(m_si.nMax - m_si.nPage))  
	{ 
		m_si.nPos = m_si.nMax; 	
	}

	
	NextScrollPos = GetScrollPos(SB_VERT);

	if (m_rect.bottom < m_viewPosRt.bottom) //����Ի���ĸ߶�С����������ͼ�ĵײ�����ʾ������
	{
		ShowScrollBar(SB_VERT,TRUE);
		//ScrollWindow(0,-NextScrollPos );//������ʵ���ƶ���λ�ÿ���ͨ��NextScrollPos���	
	}
	else
	{
		ShowScrollBar(SB_VERT,FALSE);
	}
//	SetViewListWidth();
	m_bLineDown = TRUE;
	m_bLineUp = TRUE;
	// Invalidate(TRUE);
	if (m_bNeedRefesh)
	{
		SetLastPreview();
	}
	return TRUE; 
}

UINT CWBPreviewDlg::InsertPreview(void)  //����Ԥ����
{

	UINT CurPageIndex =	g_pWBFile->GetCurPageIndex();
	POSITION pos = m_pViewList->FindIndex(CurPageIndex - 1);
	if (pos == NULL)
	{
		return 0;
	}

	m_pPreviewView = new CWBPreviewView(m_hWnd,m_pPage);
	if (NULL == m_pPreviewView)
	{
		return 0;
	}
	CWBPreviewView *pView = (CWBPreviewView *)m_pViewList->GetAt(pos);
	if (NULL == pView)
	{
		return 0;
	}
	pView->GetClientRect(&m_RectTempPriview);
	ScreenToClient(&m_RectTempPriview);
	m_pPreviewView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW,m_RectTempPriview,this,NULL);
	m_pViewList->InsertAfter(pos, m_pPreviewView);
	m_pPreviewView->SetCurPageIndex(CurPageIndex+1);
	m_viewPosRt.bottom = m_viewPosRt.top + (LONG)VIEWHIGHT;
	m_viewPosRt.top += (LONG)VIEWHIGHT + 10;
	RefurbishInserPreview();
	m_pageCount ++;
	CurPageIndex ++;
	return   CurPageIndex;
}

BOOL CWBPreviewDlg::RefurbishInserPreview()
{
	UINT uCurPageIndex = g_pWBFile->GetCurPageIndex();
	if (uCurPageIndex != m_pageCount)
	{
		return false;
	}
	//if ( NULL == m_pViewList)
	//{
	//	return FALSE;
	//}
	POSITION pos = NULL;
	if (m_pViewList)
	{
		 pos = m_pViewList->FindIndex(uCurPageIndex-1);
	}
	
	if (NULL == pos || NULL == m_pViewList)
	{
		return FALSE;
	}
	while(pos != m_pViewList->GetTailPosition())
	{
		CRect RectTemp = m_RectTempPriview;
		m_RectTempPriview.bottom = m_RectTempPriview.top + (LONG)VIEWHIGHT +3;	
		m_RectTempPriview.top += (LONG)VIEWHIGHT + 8;	
		m_pViewList->GetNext(pos);
		CWBPreviewView *pViewTemp = (CWBPreviewView *)m_pViewList->GetAt(pos);
		if (NULL == pViewTemp)
		{
			continue;
		}
		pViewTemp->GetClientRect(&m_RectTempPriview);
		ScreenToClient(&m_RectTempPriview);
		pViewTemp->MoveWindow(RectTemp);
	}
	CWBPreviewView *pView = (CWBPreviewView *)m_pViewList->GetTail();
	if (NULL == pView)
	{
		return FALSE;
	}
	pView->MoveWindow(m_viewPosRt);
	// 	int NextScrollPos = GetScrollPos(SB_VERT); //������view֮��Ĺ�����λ��	
	// 	if (m_rect.bottom < m_RectTempPriview.bottom) //����Ի���ĸ߶�С����������ͼ�ĵײ�����ʾ������
	// 	{
	// 		ShowScrollBar(SB_VERT,TRUE);
	// 		ScrollWindow(0,-(NextScrollPos - PreScrollPos));//������ʵ���ƶ���λ�ÿ���ͨ����NextScrollPos - PreScrollPos�����
	// 		POSITION pos;
	// 		CWBPreviewView *pView = NULL;
	// 		CRect m_ViewRt;
	// 		m_ViewRt.top = 6 - NextScrollPos;
	// 		for(int i = 0;i < m_pViewList->GetCount();i ++,m_ViewRt.top += VIEWHIGHT + 6)
	// 		{
	// 			pos = m_pViewList->FindIndex(i);
	// 			if (pos == NULL)
	// 			{
	// 				continue;
	// 			}
	// 			pView = (CWBPreviewView*)(m_pViewList->GetAt(pos));
	// 			pView->MoveWindow(6,m_ViewRt.top,VIEWWIDTH - 10,VIEWHIGHT);
	// 		}
	// 	}
	// 	else
	// 	{
	// 		ShowScrollBar(SB_VERT,FALSE);
	// 	}
	return TRUE;
}

void CWBPreviewDlg::ExchangePageA(CWBPreviewView* ExchangeviewA, int ExchangeA)
{
	m_pPreViewA = ExchangeviewA;
	m_ExchangePageA = ExchangeA;
}

void CWBPreviewDlg::ExchangePageB(CWBPreviewView* ExchangeviewB, int ExchangeB)
{
	m_pPreViewB = ExchangeviewB;
	m_ExchangePageB = ExchangeB;
	//	InterchangView(m_pPreViewA,m_pPreViewB);
}

void CWBPreviewDlg::InterchangPage(void)
{
	if(m_ExchangePageA != m_ExchangePageB)
	{
		g_pWhiteBoardView->InterchangePage(m_ExchangePageA,m_ExchangePageB);
		POSITION posformer = m_pViewList->FindIndex(m_ExchangePageA - 1);
		POSITION postarget = m_pViewList->FindIndex(m_ExchangePageB - 1);

		CWBPreviewView *pformerPage = (CWBPreviewView *)m_pViewList->GetAt(posformer);
		CWBPreviewView *ptargetPage = (CWBPreviewView *)m_pViewList->GetAt(postarget);

		m_pViewList->SetAt(posformer, ptargetPage);
		m_pViewList->SetAt(postarget, pformerPage);

		pformerPage->GetClientRect(&m_ExchangePageARect);
		pformerPage->ClientToScreen(&m_ExchangePageARect);
		ScreenToClient(&m_ExchangePageARect);
		ptargetPage->GetClientRect(&m_ExchangePageBRect);
		ptargetPage->ClientToScreen(&m_ExchangePageBRect);
		ScreenToClient(&m_ExchangePageBRect);

		m_pPreViewB->MoveWindow(m_ExchangePageARect);
		m_pPreViewA->MoveWindow(m_ExchangePageBRect);

		g_pWhiteBoardView->GotoPage(m_ExchangePageA);
		//Invalidate(TRUE);
	}
}
// �ƶ�ҳ
BOOL CWBPreviewDlg::MovePage(void)
{
	if(m_ExchangePageA != m_ExchangePageB)
	{
		/*		int nChoose;
		nChoose = AfxMessageBox("�Ƿ��ƶ���ҳ��",MB_OKCANCEL);
		if (nChoose == IDOK)
		{
		*/	g_pWhiteBoardView->MovePage(m_ExchangePageA,m_ExchangePageB);

	POSITION posformer,postarget;
	posformer = m_pViewList->FindIndex(m_ExchangePageA - 1);
	if (posformer == NULL)
	{
		return FALSE;
	}

	CWBPreviewView *pformerPage = (CWBPreviewView *)m_pViewList->GetAt(posformer);

	m_pViewList->RemoveAt(posformer);
	if (m_ExchangePageA < m_ExchangePageB)
	{
		postarget = m_pViewList->FindIndex(m_ExchangePageB - 2);
		m_pViewList->InsertAfter(postarget, pformerPage);
	}
	else
	{
		postarget = m_pViewList->FindIndex(m_ExchangePageB -1);
		m_pViewList->InsertBefore(postarget, pformerPage);
	}
	RefurbishPreview();
	//}
	//else if (nChoose == IDCANCEL)
	//{
	//	return FALSE;
	//}

	}
	return TRUE;
}

void CWBPreviewDlg::InterchangView(UINT nFormerPage,UINT nTargetPage)
{
	if(nFormerPage != nTargetPage)
	{
		POSITION posformer = m_pViewList->FindIndex(nFormerPage - 1);
		POSITION postarget = m_pViewList->FindIndex(nTargetPage - 1);
		CWBPreviewView *pformerPage = (CWBPreviewView *)m_pViewList->GetAt(posformer);
		CWBPreviewView *ptargetPage = (CWBPreviewView *)m_pViewList->GetAt(postarget);

		m_pViewList->SetAt(posformer, ptargetPage);
		m_pViewList->SetAt(postarget, pformerPage);
		pformerPage->GetClientRect(&m_ExchangePageARect);
		ScreenToClient(&m_ExchangePageARect);
		ptargetPage->GetClientRect(&m_ExchangePageBRect);
		ScreenToClient(&m_ExchangePageBRect);

		m_pPreViewA->ShowWindow(SW_HIDE);
		m_pPreViewB->MoveWindow(m_ExchangePageARect);
		m_pPreViewA->MoveWindow(m_ExchangePageBRect);
		m_pPreViewA->ShowWindow(SW_SHOW);

		g_pWhiteBoardView->GotoPage(m_ExchangePageA);
	}
}
// �ڱ����д򿪹��ܶԻ�������
BOOL CWBPreviewDlg::OpenPreviewRefubish(void)
{
	if (NULL == m_pViewList)
	{
		return FALSE;
	}
	CWBPreviewView *PView;
	//CRect LastViewRect;
	CWBPage *WBPage;
	m_viewPosRt.SetRect(5,(int)(-VIEWHIGHT),(m_ViewListWidht + 5),0);
	int NextScrollPos;
	for(int viewCount = 0; viewCount < m_pViewList->GetCount();viewCount++)
	{
		POSITION pos = m_pViewList->FindIndex(viewCount);
		if (NULL == pos)
		{
			continue;
		}
		PView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��	
		if (NULL == PView)
		{
			continue;
		}
		if (NULL != PView)
		{
			PView ->DestroyWindow();
		}
	}
	m_LastPreView =NULL;
	m_pViewList->RemoveAll(); 
	m_pageCount = g_pWBFile->GetPageCount();
	CDC *pdc = g_pWhiteBoardView->GetDC();
	if (pdc == NULL)
	{
		return FALSE;
	}
	for (UINT i = 0 ;i < m_pageCount ; i++)
	{
		m_pPage = g_pWBFile->GetPage(i+1);
		m_pPreviewView = new CWBPreviewView(m_hWnd,m_pPage);
		if (NULL == m_pPreviewView)
		{
			return FALSE;
		}		
		m_viewPosRt.top = m_viewPosRt.top + (LONG)VIEWHIGHT + 10;
		m_viewPosRt.bottom = m_viewPosRt.top + (LONG)VIEWHIGHT;
		m_pPreviewView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW,m_viewPosRt,this,NULL);
		m_pViewList->AddTail(m_pPreviewView);
		m_pPreviewView->SetCurPageIndex(i+1);
		//�����ļ�
		g_pWBFile->GotoPage(i+1);
		WBPage = g_pWBFile->GetCurPage();
		WBPage->Draw(pdc);

	//	UpdateWindow();
		g_pWhiteBoardView->GotoPage(1);
		/*		m_pPreviewView->Invalidate();*/

	}
	g_pWhiteBoardView->ReleaseDC(pdc);
	UpdateWindow();

	ShowButtonPlace();      //���ð�ť��λ��
	g_pWhiteBoardView->GotoPage(g_pWBFile->GetCurPageIndex());
	//����������Ϣ����

	m_si.nMax = m_viewPosRt.bottom -20;
	/*		m_si.nMax = m_viewPosRt.bottom + 44;*/
	m_si.nPos = 0 ;   
	SetScrollInfo(SB_VERT,&m_si,1);
	NextScrollPos = GetScrollPos(SB_VERT);

	if (m_rect.bottom < m_viewPosRt.bottom) //����Ի���ĸ߶�С����������ͼ�ĵײ�����ʾ������
	{
		ShowScrollBar(SB_VERT,TRUE);
		ScrollWindow(0,-NextScrollPos );//������ʵ���ƶ���λ�ÿ���ͨ����NextScrollPos - PreScrollPos�����	

	}
	else
	{
		ShowScrollBar(SB_VERT,FALSE);
	}
	m_bLineDown = TRUE;
	m_bLineUp = TRUE;
	return TRUE; 

}
CWBPreviewView * CWBPreviewDlg::GetIndexPreviewPage(UINT CurPageIndex)
{
	POSITION pos;
	pos = m_pViewList->FindIndex(CurPageIndex-1); //�����ļ��еĵ�ǰҳ���ҵ���ҳ��Ӧ��m_pViewList�����б��е�λ��
	if (NULL == pos)
	{
		return 0;
	}
	return (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
}

BOOL CWBPreviewDlg::OnEraseBkgnd(CDC* pDC)
{
	//�Ի��򱳾�	
	//CString CurPicPath = GetCurrentPath();	//��ȡ��ǰ����ִ�е�Ŀ¼
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);
	//if (g_WBType == WB_SINGLERVER)
	//{
		CString CurPicPath = GetCurrentPath() + "\\SKIN\\WhiteBoard\\image\\functionDlg\\DlgBG.png";
// 	}
// 	else
// 	{
// 		CurPicPath += "\\SKIN\\WhiteBoard\\image\\functionDlg\\PreviewDlgBG.png";
// 	}
	
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}

void CWBPreviewDlg::SetBtnState(int BtnId,WB_MOUSE_STATE MouseState)
{
	switch (BtnId)
	{
	case IDC_BTN_PREVIEW:
		m_BtnRightDown.SetMouseState(MouseState);
		break;
	default:
		break;
	}
}

void CWBPreviewDlg::SetPreviewRect( CRect rect )
{
	m_rect = rect;
}

void CWBPreviewDlg::SetViewListWidth()//���ڴ�ֱ������ʱ,view�б���խ,������խ-caoyuqin2011/11/22
{	
	CRect NewButtonRect;
	if (m_rect.bottom < m_viewPosRt.bottom)//���ִ�ֱ������ʱ
	{
		if (m_ViewListWidht < VIEWWIDTH)
		{
			;
		} 
		else
		{
			POSITION pos;
			CWBPreviewView *pView = NULL;
			CRect m_ViewRt;		
			m_ViewRt.top = 10 - GetScrollPos(SB_VERT);			
			for(int i = 0;i < m_pViewList->GetCount();i ++)
			{
				pos = m_pViewList->FindIndex(i);
				if (pos == NULL)
				{
					continue;
				}
				pView = (CWBPreviewView*)(m_pViewList->GetAt(pos));
				if (NULL == pView)
				{
					continue;
				}
				pView->MoveWindow(5,m_ViewRt.top,VIEWWIDTH - 16,(int)VIEWHIGHT);
				m_ViewRt.top = m_ViewRt.top + 10 + (LONG)VIEWHIGHT;
			}
			m_ViewListWidht = VIEWWIDTH - 16;
			m_bReduceListWidth = true;
			pView->GetWindowRect(&NewButtonRect);//��õ�ǰҳ���Priviewdlg������
			ScreenToClient(&NewButtonRect);
			pView->GetWindowRect(&NewButtonRect);
			ScreenToClient(&NewButtonRect);
			m_NewButtonRect.left = NewButtonRect.Width() + 4;
			m_NewButtonRect.top = NewButtonRect.top;
			m_NewButtonRect.right = NewButtonRect.Width() + 32;
			m_NewButtonRect.bottom = NewButtonRect.top+ 30;
			ShowButtonPlace();
			if (g_pWhiteBoardView == NULL)
			{
				return;
			}
			Invalidate();
		}
	} 
	else//��������ʧʱ
	{
		//		if (m_ViewListWidht < VIEWWIDTH)
		if(m_ViewListWidht < 150)
		{
			POSITION pos;
			CWBPreviewView *pView = NULL;
			CRect m_ViewRt;			
			m_ViewRt.top = 10 - GetScrollPos(SB_VERT);
			for(int i = 0;i < m_pViewList->GetCount();i ++)
			{
				pos = m_pViewList->FindIndex(i);
				if (pos == NULL)
				{
					continue;
				}
				pView = (CWBPreviewView*)(m_pViewList->GetAt(pos));
				if (NULL == pView)
				{
					continue;
				}
				pView->MoveWindow(5,m_ViewRt.top,VIEWWIDTH,(int)VIEWHIGHT);
				pView->UpdateWindow();
				m_ViewRt.top = m_ViewRt.top + 10 + (LONG)VIEWHIGHT;
			}
			m_ViewListWidht = VIEWWIDTH;
			m_bReduceListWidth = false;
			pView->GetWindowRect(&NewButtonRect);//��õ�ǰҳ���Priviewdlg������
			ScreenToClient(&NewButtonRect);
			pView->GetWindowRect(&NewButtonRect);
			ScreenToClient(&NewButtonRect);
			m_NewButtonRect.left = NewButtonRect.Width() + 4;
			m_NewButtonRect.top = NewButtonRect.top;
			m_NewButtonRect.right = NewButtonRect.Width() + 32;
			m_NewButtonRect.bottom = NewButtonRect.top+ 30;
			ShowButtonPlace();
		}
	}
}

UINT CWBPreviewDlg::GetViewListWidth()
{
	return m_ViewListWidht ;
}

bool CWBPreviewDlg::GetNeedRedueListWidth()
{
	return m_bReduceListWidth;
}

void CWBPreviewDlg::RflashViewList()
{
	if (g_pWhiteBoardView->m_bLocalFileOperate)
	{
		OpenPreviewRefubish();
		g_pWhiteBoardView->m_bLocalFileOperate = false;
	}
}

void CWBPreviewDlg::SetCourrtPrvRatio()
{
	CWBPreviewView * PPreviewView;
	POSITION pos;
	UINT CurPageIndex =	g_pWBFile->GetCurPageIndex();
	pos = m_pViewList->FindIndex(CurPageIndex-1); //�����ļ��еĵ�ǰҳ���ҵ���ҳ��Ӧ��m_pViewList�����б��е�λ��
	if (pos == NULL)
	{
		return;
	}
	PPreviewView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
	if (NULL == PPreviewView)
	{
		return ;
	}
	PPreviewView->SetRatio();
}

afx_msg LRESULT CWBPreviewDlg::OnWbFunction_AddPreview(WPARAM wParam, LPARAM lParam)
{
	CWBPage *pWbpage = (CWBPage *)wParam;
	if (pWbpage == NULL)
	{
		return FALSE;
	}
	int PreScrollPos,NextScrollPos;     //��������λ��
	CRect NewViewRt;   //�²�����view���ɵ�λ��
	CWBPreviewView * pPreTemView = NULL;
	int nCurPage_Num =	g_pWBFile->GetCurPageIndex();
	if (m_pViewList->GetCount() < (int)g_pWBFile->GetPageCount() )  //���������ӵ�ҳ����Ԥ��������µ���ͼ��ͬʱ�޸Ĺ�������Ϣ
	{
		pPreTemView = new CWBPreviewView(m_hWnd,pWbpage);
		if (NULL == pPreTemView)
		{
			return FALSE;
		}
		PreScrollPos = GetScrollPos(SB_VERT);  //��õ�ǰ��������λ��

		//m_viewPosRt.bottom = m_viewPosRt.top + 2*(LONG)VIEWHIGHT +5 ;//+3 ;
		m_viewPosRt.bottom += (LONG)VIEWHIGHT + 5;
		m_viewPosRt.top += (LONG)VIEWHIGHT + 5; 
		NewViewRt.left = m_viewPosRt.left;  //�����ɵ�viewӦ�ø��ݹ�������λ������
		NewViewRt.right = m_viewPosRt.right;
		NewViewRt.top = m_viewPosRt.top - PreScrollPos;
		NewViewRt.bottom = m_viewPosRt.bottom- PreScrollPos ;
		if (! pPreTemView->Create(NULL,NULL,WS_VISIBLE | AFX_WS_DEFAULT_VIEW,NewViewRt,this,NULL) )
		{
			delete pPreTemView;
			pPreTemView = NULL;
			return FALSE;
		}
		m_pViewList->AddTail(pPreTemView);	
		pPreTemView->SetCurPageIndex(nCurPage_Num);
		m_si.nMax = m_viewPosRt.bottom -20;
		m_si.nPos = m_si.nMax ;   //��������view��ʱ���ù������������λ��,����ʵ����nPos�ƶ���λ��û�����ֵ��ô��
		SetScrollInfo(SB_VERT,&m_si,1); 	
		NextScrollPos = GetScrollPos(SB_VERT); //������view֮��Ĺ�����λ��	
		if (m_rect.bottom < m_viewPosRt.bottom) //����Ի���ĸ߶�С����������ͼ�ĵײ�����ʾ������
		{
			ShowScrollBar(SB_VERT,TRUE);
			ScrollWindow(0,-(NextScrollPos - PreScrollPos));//������ʵ���ƶ���λ�ÿ���ͨ����NextScrollPos - PreScrollPos�����	
		}
		else
		{
			ShowScrollBar(SB_VERT,FALSE);
		}
	}
	//m_LastPreView->Invalidate();
	if (pPreTemView)
	{
		pPreTemView->Invalidate(TRUE);
	}
	
	return 0;
}

bool CWBPreviewDlg::bLoadPreviews()
{
	
	if (m_pViewList->GetCount() == g_pWBFile->GetPageCount())
	{
		return false;
	}
	return true;
}


void CWBPreviewDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: �ڴ˴������Ϣ����������
	//���ڽ�ȡԤ���Ϲ��������Ҽ������˵���Ϣ
}

CWBPreviewView* CWBPreviewDlg::GetPreviewView()
{
	return m_pPreviewView;
}

void CWBPreviewDlg::ClearAllView()
{ 
	m_viewPosRt.SetRect(5,(int)(-VIEWHIGHT),(m_ViewListWidht + 5),0);
	if (m_pViewList != NULL)
	{
		CWBPreviewView *PView;
		for(int viewCount = 0; viewCount < m_pViewList->GetCount();viewCount++)
		{
			POSITION pos = m_pViewList->FindIndex(viewCount);
			if (NULL == pos)
			{
				continue;
			}
			PView = (CWBPreviewView *)m_pViewList->GetAt(pos); //����posλ�����õ�CWBPreviewViewָ��Ԫ��
			if(NULL == PView)
				continue;
			if (NULL != PView)
			{
				PView ->DestroyWindow();
				//	delete PView;
			}
		}
		m_pViewList->RemoveAll(); 
		m_LastPreView = NULL;
		m_bNeedRefesh = true;

	}
}

bool CWBPreviewDlg::GetIsNeedRefessh()
{
	return m_bNeedRefesh;
}

void CWBPreviewDlg::SetIsNeedRefesh( bool isneed )
{
	m_bNeedRefesh = isneed;
}
