#include "StdAfx.h"
#include "WBPreviewSingleView.h"
#include "whiteBoard.h"
#include "WBEagelEye.h"

CWBPreviewSingleView::CWBPreviewSingleView( HWND ParentHwnd /*= NULL*/,CWBPage* pPage /*= NULL*/ )
{
	m_parentHwnd = ParentHwnd;
	m_pPage = pPage;
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
	m_rect = NULL;
	m_offpoint = NULL;
}


CWBPreviewSingleView::~CWBPreviewSingleView(void)
{
}
BEGIN_MESSAGE_MAP(CWBPreviewSingleView, CView)
//	ON_WM_MBUTTONDOWN()
	ON_BN_CLICKED(IDB_BTN_EAGLECLOSE, OnEagleEyeClose)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_EAGLEEYE_SYNCHRONIZER, &CWBPreviewSingleView::Synchronizer)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
void CWBPreviewSingleView::SetCurPage( CWBPage *pPage )
{
	m_pPage = pPage;
	SetIsRedrawView(true);
}

void CWBPreviewSingleView::SetRectInPreview()
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
			OffsetX = 1;
		}
		if (offsetY < 1)
		{
			offsetY = 1;
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
/*		rect = g_pWhiteBoardView->getWBViewRECT();//�װ�Ĵ�С*/
// 		m_RectInPreview.SetRect( (OffsetPage.x / m_fRatioX),(OffsetPage.y / m_fRatioY),
// 			(OffsetPage.x / m_fRatioX) + ((m_rectwbView.Width()) / m_fRatioX ),
// 			((OffsetPage.y /m_fRatioY) + ((m_rectwbView.Height()) / m_fRatioY ))
// 			);
	}
	m_bAccordPreViewScroll = true;	
	m_bViewIsRedraw = true;
}

void CWBPreviewSingleView::SetRatio()
{
	this->GetClientRect(&m_rect);
	m_rectwbView = g_pWhiteBoardView->getWBViewRECT();//�װ�Ĵ�С
    float WBrectX = (float)g_nWidth;
 	float WBrectY = (float)g_nHeight;
	if(g_reduce>0.00000000000000000)
	{
		WBrectX=WBrectX*g_reduce;
		WBrectY=WBrectY*g_reduce;

	}
	float rectX = (float)m_rect.Width();
	float rectY = (float)m_rect.Height();
	m_fRatioX = WBrectX/rectX;
	m_fRatioY = WBrectY/rectY;
}


void CWBPreviewSingleView::OnDraw(CDC* pDC)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (m_bViewIsRedraw)
	{
		m_bViewIsRedraw = false;
		CDocument* pDoc = GetDocument();
		//ASSERT_VALID(pDoc);
		// TODO: �ڴ���ӻ��ƴ���
		pDC->SetStretchBltMode(HALFTONE);   //��������   
		CRect rect;
		GetClientRect(&rect);
		::StretchBlt(pDC->m_hDC, 0, 0, rect.Width() , rect.Height(), m_pPage->GetMemHDC(), 0, 0, g_nWidth, g_nHeight, SRCCOPY);
		//�����ջ�ˢ������
		CBrush * pOldBrush = NULL;
		CBrush  *pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
		pOldBrush=pDC->SelectObject(pBrush);
		CPen* pen = new CPen(PS_SOLID,3,RGB(255,0,0));
		pDC->SelectObject(pen);
		pDC->SetBkMode(TRANSPARENT);
		//���ƴ������ƶ�ʱ���ο򳬳�����
		rect.NormalizeRect();
		m_RectInPreview.NormalizeRect();
		CRect UnRect;
		UnRect.UnionRect(rect,m_RectInPreview); //�������Ϳ�ϲ�
		if (UnRect != rect)
		{
			SetRectInPreview();
		}
		pDC->Rectangle(m_RectInPreview);
		m_RectTracker.m_rect= m_RectInPreview;
		CPen *blackpen = new CPen(PS_SOLID,3,RGB(0,0,0));
		pDC->SelectObject(blackpen);
		pDC->SetBkMode(TRANSPARENT);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldBrush);
		pBrush->DeleteObject();
		delete blackpen;
		delete pen;
	}
}

void CWBPreviewSingleView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((!g_bIsSupportMTTouch) && g_bNoSupportTouchInWB)//���û����Ȩ�Ͳ���Ӧ������
	{
		if((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)
		{
			return;
		}
	}
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_ButtonDownPoint = point;//�����ƶ�Ԥ�����ο��
	g_pWBPage->ClearSelectFrameList();
	g_pWhiteBoardView->Invalidate();
	g_pObj = NULL;
	ClipCursor(NULL);
	m_offpoint = point;
	int ret=m_RectTracker.HitTest(point);
	if(ret>=0)//�ж��Ƿ�Ҫ�������߿�
	{
		InvalidateRect(&m_RectTracker.m_rect);

		if(ret==CRectTracker::hitMiddle)//ֻ�е�������м䰴��,����������ƶ�����
		{
			if (g_bAvconLock) //��������аװ幦�ܱ�����
			{
				return;
			}
			POINT pos;
			GetCursorPos(&pos);
			ScreenToClient(&pos);
			CRect ClientRect;
			GetClientRect(&ClientRect);
			
			ClientRect.left+=pos.x-m_RectInPreview.left ;
			ClientRect.top+=pos.y-m_RectInPreview.top;
			ClientRect.right-=m_RectInPreview.right-pos.x -1;
			ClientRect.bottom-=m_RectInPreview.bottom-pos.y-1;
			
			ClientToScreen(&ClientRect);
			ClipCursor(&ClientRect);
		}
		SetIsRedrawView(true);
	}
	
	CRect rect=m_RectInPreview;
	InvalidateRect(&rect);
	//�˺��������ص������Ϣ,����Ҫ�������������д��굯��Ҫ������¼�.
	if(m_RectTracker.Track(this,point,false,NULL)) 
	{ 
		if (m_RectTracker.m_rect.Size() != m_RectInPreview.Size())
		{
			int width = m_RectInPreview.Width();
			int height = m_RectInPreview.Height();
			m_RectInPreview.left = m_RectTracker.m_rect.left;
			m_RectInPreview.top = m_RectTracker.m_rect.top;
			m_RectInPreview.right = m_RectTracker.m_rect.left + width;
			m_RectInPreview.bottom = m_RectTracker.m_rect.top + height;
			
			//m_RectTracker.m_rect = m_RectInPreview;
		}
		else
		{
			m_RectInPreview=m_RectTracker.m_rect;
		}
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
		ClipCursor(NULL);
	}
	else
	{	
		ClipCursor(NULL);
	}
	g_pWhiteBoardView->SetViewIsNeedDraw(FALSE);
	g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
	Invalidate();
}


BOOL CWBPreviewSingleView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}


afx_msg LRESULT CWBPreviewSingleView::Synchronizer(WPARAM wParam, LPARAM lParam)
{
	SetCurPage(g_pWBFile->GetCurPage());
	SetRectInPreview();
	Invalidate();
	bool bflag = false;
	bflag = g_pWhiteBoardView->GetColorPaletteDlg();
	if (bflag)
	{
		UpdateWindow();
		g_pWhiteBoardView->SetColorPaletteDlg(false);//�������ñ���ʱ��ʱʱ˧��
	}
	
	return 0;
}

void CWBPreviewSingleView::SetCloseBtn()
{
	//��ȡ�رհ�ť������
// 	CRect clientRect;
// 	GetClientRect(&clientRect);
// 	//CRect rtBitmapBtn(0, clientRect.bottom - 54, 54, clientRect.bottom);
// 	CRect RightRectBtn(clientRect.right-54, clientRect.top, clientRect.right, clientRect.top+54);
// 	//�����رհ�ť
// 	m_ptCloseBtn = new CWMBtn(this, BTN_EAGLEEYE_CLOSE);
// 	m_ptCloseBtn->Create(_T("�ر�"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON |BS_OWNERDRAW | WS_CLIPSIBLINGS, RightRectBtn, this, BTN_EAGLEEYE_CLOSE);
// 	//pos = TRUE;//��ʼ״̬��������
// 	m_ptCloseBtn->SetButtonImage(_T("SpotlighyClose.png"));
}

void CWBPreviewSingleView::ShowButtonPlace()
{
	CRect rect;
	rect.SetRect(m_rect.right-30,m_rect.top,m_rect.right,m_rect.top+30);
	if(!m_CloseBtn.m_hWnd)
	{
	/*	m_CloseBtn = new CWBFunctionBtn;*/
		m_CloseBtn.Create(_T("�ر� "),WS_VISIBLE |WS_CHILD | BS_OWNERDRAW ,rect,this ,IDB_BTN_EAGLECLOSE);
		m_CloseBtn.ShowWindow(SW_SHOW);
	} 
	m_CloseBtn.MoveWindow(rect);
}

void CWBPreviewSingleView::OnEagleEyeClose()
{
	this->GetParent()->PostMessage(WM_CLOSE);
	SetRatio();
	
	g_pWhiteBoardView->SetViewIsNeedDraw(FALSE);
	g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
}

void CWBPreviewSingleView::SetBtnState(int BtnId,WB_MOUSE_STATE MouseState)
{
	switch (BtnId)
	{
	case IDB_BTN_EAGLECLOSE:
		m_CloseBtn.SetMouseState(MouseState);
		break;
	default:
		break;
	}
}

void CWBPreviewSingleView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (bTransmitMessageToWBView())
	{
		ClientToScreen(&point);
		g_pWhiteBoardView->ScreenToClient(&point);
		::SendMessage(g_pWhiteBoardView->m_hWnd, WM_MOUSEMOVE,nFlags,MAKELPARAM( point.x,   point.y));    // ��������ƶ�����Ϣ���װ�view
		return;
	}
	if (MK_LBUTTON ==( nFlags&&MK_LBUTTON)) 
	{
		int ret=m_RectTracker.HitTest(point);
		if(ret==CRectTracker::hitNothing)//
		{
			g_pWhiteBoardView->SetViewIsNeedDraw(FALSE);
			g_pWhiteBoardView->SetPageIsNeedDraw(FALSE);
			if ( DRAW_NORMAL == g_nDrawStatus)  //������汾��ֻ��ѡ��״̬�������ƶ�ӥ�� huanglongquan �޸�
			{
				this->GetParent()->PostMessage(WM_NCLBUTTONDOWN,   HTCAPTION,   MAKELPARAM(  point.x,   point.y)); 
			}
		}
	}
	//CView::OnMouseMove(nFlags, point);
}


void CWBPreviewSingleView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (bTransmitMessageToWBView())
	{
		ClientToScreen(&point);
		g_pWhiteBoardView->ScreenToClient(&point);
		::SendMessage(g_pWhiteBoardView->m_hWnd, WM_LBUTTONUP,nFlags,MAKELPARAM( point.x,   point.y));    // ����������������Ϣ
	}
	//	CView::OnLButtonUp(nFlags, point);
}

void CWBPreviewSingleView::SetIsRedrawView( bool bRedraw )
{
	m_bViewIsRedraw = bRedraw;
}
bool CWBPreviewSingleView::bTransmitMessageToWBView()
{
	if (DRAWING_POLYLINE == g_nDrawStatus
		|| DRAWING_LINE == g_nDrawStatus
		|| DRAWING_TEXT == g_nDrawStatus
		|| DRAWING_RECT == g_nDrawStatus
		|| DRAWING_POLYGON == g_nDrawStatus
		|| DRAWING_ELLIPSE == g_nDrawStatus
		|| DRAWING_POLYGON == g_nDrawStatus
		|| DRAWING_ELLIPSE == g_nDrawStatus
		|| DRAWING_TRIANGLE == g_nDrawStatus
		|| DRAWING_RHOMBUS == g_nDrawStatus
		|| DRAWING_PENTAGON == g_nDrawStatus
		|| DRAWING_HEXAGON == g_nDrawStatus
		|| DRAWING_FIVEPOINTSTART == g_nDrawStatus
		|| DRAWING_TRAPEZIUM == g_nDrawStatus
		|| DRAW_RGNDELETING == g_nDrawStatus
		|| DRAW_ERASING == g_nDrawStatus
		|| DRAW_CLEANUPING == g_nDrawStatus //���������ʲô״̬
		)
	{
		return true;
	}
	return false;
}