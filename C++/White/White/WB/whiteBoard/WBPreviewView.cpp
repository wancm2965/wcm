// WBPreviewView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBPreviewView.h"



// CWBPreviewView

IMPLEMENT_DYNCREATE(CWBPreviewView, CView)

	CWBPreviewView::CWBPreviewView(HWND ParentHwnd ,CWBPage *pPage )
{
	m_parentHwnd = ParentHwnd;
	m_pPage = pPage;
//	m_RectInPreview.SetRect(0,0,122,74);
	m_RectInPreview.SetRect(0,0,0,0);
	m_RectTracker.m_rect = m_RectInPreview;
	m_RectTracker.m_nStyle = CRectTracker::solidLine;
	m_ButtonDownPoint = (0,0);
	m_ButtonGap = (0,0);
    m_bAccordPreViewScroll = true;
	m_bViewIsRedraw = true;
	m_fRatioX = 1;
	m_fRatioY = 1;
	m_rectwbView = NULL;
	//m_bgcolor = RGB();
}

CWBPreviewView::~CWBPreviewView()
{
	// 	if(this->IsWindowVisible())
	// 		this->DestroyWindow();
}

BEGIN_MESSAGE_MAP(CWBPreviewView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CWBPreviewView ��ͼ

void CWBPreviewView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	// TODO: �ڴ���ӻ��ƴ���
/*	if(m_bViewIsRedraw)*/
	{
		m_bViewIsRedraw =false;
		pDC->SetStretchBltMode(HALFTONE);   //��������                      
		::StretchBlt(pDC->m_hDC, 0, 0, VIEWWIDTH , (int)VIEWHIGHT , m_pPage->GetMemHDC(), 0, 0, g_nWidth, g_nHeight, SRCCOPY);

		CClientDC dc(this);    //ҳ��
		CFont font;
		font.CreatePointFont(125,_T("�����п�"),NULL);
		CFont *pOldFont = dc.SelectObject(&font); 
		UINT CurPageIndex = GetCurPageIndex();
		//	UINT CurPageIndex = g_pWBFile->GetCurPageIndex();
		dc.SetBkMode(TRANSPARENT);
		CString csOut;
		csOut.Format(L"%d", CurPageIndex);
		dc.TextOut(120, 18 , csOut , csOut.GetLength());
		dc.SelectObject(pOldFont);
		font.DeleteObject();

		//�����ջ�ˢ������
		double nViewWidth=WB_VIEWHSCROLL*g_reduce;
		double nViewHight=WB_VIEWVSCROLL*g_reduce;
		CRect rc;
		g_whiteboardDlg->GetClientRect(&rc);

		double nWindowWidth=rc.right-(WB_FUNCTION_DLG_WIDTH + WB_OPTIONDLG_WIDTH);
		double nWindowHight=rc.bottom;
		
		if((nViewHight>nWindowHight)||(nViewWidth>nWindowWidth)||((g_reduce<0.00+0.00000000000000001)&&(g_reduce>0.00-0.00000000000000001)))//�ж��Ƿ��й��������й������ĲŻ�����
		//if(g_reduce>0.35||((g_reduce<0.00+0.00000000000000001)&&(g_reduce>0.00-0.00000000000000001)))//�й�������ʱ��Ŵ������ο�
		{
			if (m_CurPageIndex == g_pWBFile->GetCurPageIndex())
			{	
				CBrush * pOldBrush;
				CBrush  *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
				pOldBrush=pDC->SelectObject(pBrush);
				CPen* pen = new CPen(PS_SOLID,3,RGB(255,0,0));
				pDC->SelectObject(pen);
				pDC->SetBkMode(TRANSPARENT);
				CRect rect;
				GetClientRect(&rect);
				rect.NormalizeRect();
				m_RectInPreview.NormalizeRect();
				CRect UnRect;
				UnRect.UnionRect(rect,m_RectInPreview);
				if (UnRect != rect || m_RectInPreview.IsRectNull())
				{
					SetRectInPreview();
				}
				pDC->Rectangle(m_RectInPreview);
				m_RectTracker.m_rect= m_RectInPreview;
				delete pen;
				pDC->SelectObject(pOldBrush);
				pBrush->DeleteObject();
			}
		}
	}
}

void CWBPreviewView::SetCurPage(CWBPage *pPage)
{
	m_pPage = pPage;
}
// CWBPreviewView ���
#ifdef _DEBUG
void CWBPreviewView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CWBPreviewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

void CWBPreviewView::SetCurPageIndex(UINT PageIndex)
{
	m_CurPageIndex = PageIndex;
}
UINT CWBPreviewView::GetCurPageIndex()
{
	return m_CurPageIndex;
}

// ����Ԥ���еľ��ο�
void CWBPreviewView::SetRectInPreview()
{
	if (m_bAccordPreViewScroll)
	{
		CWBPage *pCurrentPage;
		CPoint OffsetPage;//��ǰҳ��ƫ����
		POINT Pagepoint;
		pCurrentPage = g_pWBFile->GetCurPage();
		Pagepoint.x = g_pWhiteBoardView->getXnPos();
		Pagepoint.y = g_pWhiteBoardView->getYnPos();
		
		pCurrentPage->SetPageViewScroll(Pagepoint);
		OffsetPage = pCurrentPage->GetPageViewScroll();

		SetRatio();			

		float OffsetX = (float)OffsetPage.x;
		float offsetY = (float)OffsetPage.y;
		float fwidth = (float)m_rectwbView.Width();
		float fheight = (float)m_rectwbView.Height();
		fwidth = (OffsetX + fwidth)/m_fRatioX;
		fheight = (offsetY + fheight)/m_fRatioY;
		OffsetX = OffsetX/m_fRatioX;
		offsetY = offsetY/m_fRatioY;
		if (OffsetX <1)
		{
			OffsetX = 0;
		}
		if (offsetY < 1)
		{
			offsetY = 0;
		}
		m_RectInPreview.SetRect((int)OffsetX,(int)offsetY,(int)fwidth,(int)fheight);
		if (m_RectInPreview.bottom > (m_rect.bottom-2) )
		{
			m_RectInPreview.bottom = m_rect.bottom- 2;
		}
		if (m_RectInPreview.right > (m_rect.right - 2))
		{
			m_RectInPreview.right = m_rect.right -2;
		}
		m_bViewIsRedraw = true;
// 		rect = g_pWhiteBoardView->getWBViewRECT();//�װ�Ĵ�С
// 		m_RectInPreview.SetRect( (OffsetPage.x / VIEWRATIO),(OffsetPage.y / VIEWRATIO),
// 			(OffsetPage.x / VIEWRATIO) + ((rect.Width()) / VIEWRATIO ),
// 			((OffsetPage.y /VIEWRATIO) + ((rect.Height()) / VIEWRATIO ))
// 			);
// 		if (m_RectInPreview.bottom > (m_rect.bottom-1) )
// 		{
// 			m_RectInPreview.bottom = m_rect.bottom- 1;
// 		}
// 		if (m_RectInPreview.right > (m_rect.right - 1))
// 		{
// 			m_RectInPreview.right = m_rect.right -1;
// 		}

	}
	else
	{
			m_bAccordPreViewScroll = true;	
	}
}

void CWBPreviewView::SetRatio()
{
	this->GetClientRect(&m_rect);
	m_rectwbView = g_pWhiteBoardView->getWBViewRECT();//�װ�Ĵ�С
	float WBrectX = (float)g_nWidth;
	float WBrectY = (float)g_nHeight;
	float rectX = (float)m_rect.Width();
	float rectY = (float)m_rect.Height();
	m_fRatioX = WBrectX/rectX;
	m_fRatioY = WBrectY/rectY;
}

//CWBPreviewView ��Ϣ�������

void CWBPreviewView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((!g_bIsSupportMTTouch) && g_bNoSupportTouchInWB)//���û����Ȩ�Ͳ���Ӧ������
	{
		if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
		{
			return;
		}
	}
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_CurPageIndex != g_pWBFile->GetCurPageIndex())
	{
	    g_pWhiteBoardView->GotoPage(m_CurPageIndex);//�ƶ�����ǰҳ
	}
//	g_pWhiteBoardView->setCurrentViewScroll(g_pWBFile->GetCurPage()->GetPageViewScroll());//���ðװ�Ĺ�������λ��
	m_ButtonDownPoint = point;//�����ƶ�Ԥ�����ο��
	//������������ҳ��
	// 	CWBPreviewDlg *PreviewDlg;
	// 	PreviewDlg = (CWBPreviewDlg*)FromHandle(m_parentHwnd);//��ø����ھ��
	// // 	HCURSOR hcursor = AfxGetApp()->LoadCursor(IDC_CURSOR9);//����Ϊ���ι��
	//  //	SetSystemCursor(hcursor,32512);   
	// 	PreviewDlg->ExchangePageA(this,m_CurPageIndex );

	//	CView::OnLButtonDown(nFlags, point);
	g_pWBPage->ClearSelectFrameList();
	//g_IsSelectObj = NULL;
	g_pObj = NULL;


	int ret=m_RectTracker.HitTest(point);
	if(ret>=0)//�ж��Ƿ�Ҫ�������߿�
	{
		InvalidateRect(&m_RectTracker.m_rect);

		if(ret==CRectTracker::hitMiddle)//ֻ�е�������м䰴��,����������ƶ�����
		{
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);
			CRect ClientRect;
			GetClientRect(&ClientRect);
			ClientRect.left+=pos.x-m_RectInPreview.left;
			ClientRect.top+=pos.y-m_RectInPreview.top;
			ClientRect.right-=m_RectInPreview.right-pos.x-1;
			ClientRect.bottom-=m_RectInPreview.bottom-pos.y-1;
			ClientToScreen(&ClientRect);
			ClipCursor(&ClientRect);
		}
	}
	else
	{
		CRect rect=m_RectTracker.m_rect;
		rect.left-=4;
		rect.right+=4;
		rect.top-=4;
		rect.bottom+=4;
		InvalidateRect(&rect);
	}
	   CRect rect=m_RectInPreview;
	   InvalidateRect(&rect);
	//�˺��������ص������Ϣ,����Ҫ�������������д��굯��Ҫ������¼�.
	if(m_RectTracker.Track(this,point,false,NULL)) 
	{ 
		if (m_RectTracker.m_rect.Size() != m_RectInPreview.Size())
		{
			m_RectTracker.m_rect = m_RectInPreview;
		}
		else
		{
			m_RectInPreview=m_RectTracker.m_rect;
		}
		m_bViewIsRedraw = true;
		CRect rect=m_RectInPreview;
		ClientToScreen(&rect);
		POINT ptCurrent;
		CRect WBrect;
		g_pWhiteBoardView->GetClientRect(&WBrect);
		CRect Prirect;
		this->GetClientRect(&Prirect);
		ClientToScreen(&Prirect);
		ptCurrent.x = (m_RectInPreview.left)* (LONG)m_fRatioX;// VIEWRATIOΪ�װ���Ԥ���ı����y
		ptCurrent.y = (m_RectInPreview.top) * (LONG)m_fRatioY;
		m_bAccordPreViewScroll = false;
		if (Prirect.bottom <=rect.bottom)
		{
			ptCurrent.y = g_nHeight - WBrect.Height();
		}
		if (Prirect.right <= rect.right)
		{
			ptCurrent.x = g_nWidth- WBrect.Width();
		}
		g_pWBFile->GetCurPage()->SetPageViewScroll(ptCurrent);
		g_pWhiteBoardView->setCurrentViewScroll(g_pWBFile->GetCurPage()->GetPageViewScroll());//���ðװ�Ĺ�������λ��
		g_pWhiteBoardView->SendScrollDraw();
// 		ptCurrent.x = (m_RectInPreview.left)* VIEWRATIO;// VIEWRATIOΪ�װ���Ԥ���ı����y
// 		ptCurrent.y = (m_RectInPreview.top) * VIEWRATIO;
// 		m_bAccordPreViewScroll = false;
// 		if (Prirect.bottom <=rect.bottom)
// 		{
// 			ptCurrent.y = WB_VIEWVSCROLL - WBrect.Height();
// 		}
// 		if (Prirect.right <= rect.right)
// 		{
// 			ptCurrent.x = WB_VIEWHSCROLL- WBrect.Width();
// 		}
// 		g_pWBFile->GetCurPage()->SetPageViewScroll(ptCurrent);
// 		g_pWhiteBoardView->setCurrentViewScroll(g_pWBFile->GetCurPage()->GetPageViewScroll());//���ðװ�Ĺ�������λ��
// 		g_pWhiteBoardView->SendChangePage();
		ClipCursor(NULL);
	}
	else
	{	
		ClipCursor(NULL);
	}
}

void CWBPreviewView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// 	HCURSOR hcursor = AfxGetApp()->LoadCursor(IDC_CURSOR8); //��ԭ�����״
	// 	SetSystemCursor(hcursor,32512);

	//������������ҳ��
	//	CWBPreviewDlg *PreviewDlg;
	// 	PreviewDlg = (CWBPreviewDlg*)FromHandle(m_parentHwnd);//��ø����ھ��
	// 	PreviewDlg->ExchangePageB(this,m_CurPageIndex );
	//PreviewDlg->MovePage();

	// 	if (m_CurPageIndex != g_pWBFile->GetCurPageIndex())
	// 	{
	// 		g_pWhiteBoardView->GotoPage(m_CurPageIndex);//�ƶ�����ǰҳ
	// 	}
}


void CWBPreviewView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//if (MK_LBUTTON ==( nFlags&&MK_LBUTTON)) 
// 	if (MK_LBUTTON == nFlags)
// 	{
// 		CWBPage *pCurrentPage;
// 		CPoint OffsetPage;//��ǰҳ��ƫ����
// 		CPoint RectTopLeft;
// 		pCurrentPage = g_pWBFile->GetCurPage();
// 		OffsetPage = pCurrentPage->GetPageViewScroll();	
// 		m_ButtonGap =point - m_ButtonDownPoint ;
// 		m_ButtonDownPoint = point;
// 		m_RectInPreview.NormalizeRect();
// 		if (m_RectInPreview.PtInRect(point))
// 		{
// 			HCURSOR hcursor = AfxGetApp()->LoadCursor(IDC_CURSOR9);//����Ϊ���ι��
// 			//			SetCapture();
// 			SetCursor(hcursor);
// 			CPoint ptCurrent;
// 			m_RectInPreview.OffsetRect(m_ButtonGap);
// // 			if(PointTopLeft.x < 0 ||PointTopLeft.y < 0 || PointBottomRight.x >m_rect.right || PointBottomRight.y > m_rect.bottom)
// // 			{
// // 				m_RectInPreview = oldRect;
// // 			}
// 			if(m_RectInPreview.left < 0)
// 			{
// 
// 				m_RectInPreview.SetRect(  1,(OffsetPage.x / VIEWRATIO)+1,
// 					m_RectInPreview.Width()+1,
// 					(OffsetPage.y / VIEWRATIO) + m_RectInPreview.Height()+1
// 					);
// 			}
// 
// 			if(m_RectInPreview.top < 0)
// 			{
// 
// 				m_RectInPreview.SetRect(  (OffsetPage.x / VIEWRATIO)+1,1,
// 					(OffsetPage.x / VIEWRATIO) + m_RectInPreview.Width()+1,
// 					m_RectInPreview.Height()+1
// 					);
// 			} 
// 			if(m_RectInPreview.bottom > m_rect.bottom)
// 			{
// 
// 				m_RectInPreview.SetRect(  (OffsetPage.x / VIEWRATIO)-1,m_rect.bottom - m_RectInPreview.Height()-1,
// 					(OffsetPage.x / VIEWRATIO) + m_RectInPreview.Width()-1,
// 					m_rect.bottom -1
// 					);
// 
// 			}
// 			if(m_RectInPreview.right > m_rect.right)
// 			{
// 
// 				m_RectInPreview.SetRect(m_rect.right - m_RectInPreview.Width()-1 ,(OffsetPage.y / VIEWRATIO)-1,
// 					m_rect.right -1,
// 					((OffsetPage.y / VIEWRATIO) + m_RectInPreview.Height()-1)
// 					);
// 
// 			}
// 			ptCurrent.x = m_RectInPreview.left* (VIEWRATIO);// VIEWRATIOΪ�װ���Ԥ���ı����y
// 			ptCurrent.y = m_RectInPreview.top * (VIEWRATIO);
// //			g_pWBFile->GetCurPage()->SetPageViewScroll(ptCurrent);
// //			g_pWhiteBoardView->setCurrentViewScroll(g_pWBFile->GetCurPage()->GetPageViewScroll());//���ðװ�Ĺ�������λ��
// 			m_bPreviewRefresh =false;
// 			SendMessage(WM_NOTICE_PRAVIEW_REFRESH,(WPARAM)m_bPreviewRefresh,0);
// 		}
// 		else
// 		{ 
// 			if (m_bPreviewRefresh == false)
// 			{ 
// 				m_bPreviewRefresh = true;
// 				SendMessage(WM_NOTICE_PRAVIEW_REFRESH,(WPARAM)m_bPreviewRefresh,0);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (m_bPreviewRefresh == false)
// 		{ 
// 			m_bPreviewRefresh = true;
// 			SendMessage(WM_NOTICE_PRAVIEW_REFRESH,(WPARAM)m_bPreviewRefresh,0);
// 		}
// 		
// 	}
//	Invalidate();
//	CView::OnMouseMove(nFlags, point);
}

BOOL CWBPreviewView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
     return TRUE;
//	 return CView::OnEraseBkgnd(pDC);
}


void CWBPreviewView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	SetRatio();
	GetClientRect(&m_rect);
}


void CWBPreviewView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CView::OnLButtonDblClk(nFlags, point);
}


