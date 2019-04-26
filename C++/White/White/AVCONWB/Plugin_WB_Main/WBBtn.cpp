// WBFunctionBtn.cpp : 实现文件
//

#include "stdafx.h"
#include "WBBtn.h"
#include <atlimage.h>

//using namespace AVCONPlugin;

// CWBFunctionBtn

IMPLEMENT_DYNAMIC(CWBBtn, CBitmapButton)

CWBBtn::CWBBtn()
{
	m_bMouseIn = FALSE;
	m_isSelect = FALSE;
}

CWBBtn::~CWBBtn()
{
}


BEGIN_MESSAGE_MAP(CWBBtn, CBitmapButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
 	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
 	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
 	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CWBFunctionBtn 消息处理程序



BOOL CWBBtn::OnEraseBkgnd(CDC* pDC)
{
	
 	return TRUE;
	//return CBitmapButton::OnEraseBkgnd(pDC);
}

CString CWBBtn::GetCurrentPath()
{
	TCHAR szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH); 

	CString strPath = szPath;
	int nPos; 
	nPos = strPath.ReverseFind('\\'); 
	strPath = strPath.Left(nPos); 

	return strPath;
}

void CWBBtn::DrawItem(LPDRAWITEMSTRUCT lpDS/*lpDrawItemStruct*/)
{
	CDC dc;
	CImage img;
	CRect ImageRc;
	dc.Attach( lpDS->hDC );
	if (m_isSelect)
	{
		ImageRc = CRect(lpDS->rcItem.right*2,lpDS->rcItem.top,lpDS->rcItem.right*3,lpDS->rcItem.bottom);
	} 
	else
	{
		ImageRc = CRect(lpDS->rcItem.right*0,lpDS->rcItem.top,lpDS->rcItem.right*1,lpDS->rcItem.bottom);
	}

	SetImageFilePath();
	img.Load( m_ImageFilePath );
	if (!img.IsNull())
	{
		int  BtnId;	
		BtnId = GetDlgCtrlID();
		if (BtnId == IDC_CHECKMTFUNCTION)
		{
			img.Draw( dc.m_hDC, lpDS->rcItem );
		}
		else
		{
			img.Draw( dc.m_hDC, lpDS->rcItem, ImageRc);
		}
	}
}


void CWBBtn::OnLButtonDown(UINT nFlags, CPoint point)
{
	CBitmapButton::OnLButtonDown(nFlags, point);
}

void CWBBtn::OnLButtonUp(UINT nFlags, CPoint point)
{
	int  BtnId;	
	BtnId = GetDlgCtrlID();
	if (BtnId != IDC_CHECKMTFUNCTION)
	{
		m_isSelect = TRUE;
	}
	
	CBitmapButton::OnLButtonUp(nFlags, point);
}

 LRESULT CWBBtn::OnMouseHover(WPARAM wParam, LPARAM lParam)
 {
	 CClientDC dc(this);
	 CImage img;
     CRect rc;
	 GetClientRect(&rc);
	 CRect ImageRc(rc.right*1,rc.top,rc.right*2,rc.bottom);
	 SetImageFilePath();
	 img.Load( m_ImageFilePath );
	 if (!img.IsNull())
	 {
		 int  BtnId;	
		 BtnId = GetDlgCtrlID();
		 if (BtnId == IDC_CHECKMTFUNCTION)
		 {
			 img.Draw( dc.GetSafeHdc(), rc );
		 }
		 else
		 {
			 img.Draw( dc.GetSafeHdc(), rc, ImageRc);
		 }
	 }
 	return 0;
 }


 LRESULT CWBBtn::OnMouseLeave(WPARAM wParam, LPARAM lParam)
 {
	 CClientDC dc(this);
	 CImage img;
	 CRect rc;
	 GetClientRect(&rc);
	 CRect ImageRc(rc.right*0,rc.top,rc.right*1,rc.bottom);
	 SetImageFilePath();
	 img.Load( m_ImageFilePath );
	 if (!img.IsNull())
	 {
		 int  BtnId;	
		 BtnId = GetDlgCtrlID();
		 if (BtnId == IDC_CHECKMTFUNCTION)
		 {
			 img.Draw( dc.GetSafeHdc(), rc );
		 }
		 else
		 {
			 img.Draw( dc.GetSafeHdc(), rc, ImageRc);
		 }
	 }
 	m_bMouseIn = FALSE;
 	return 0;
 }
 
 
 void CWBBtn::OnMouseMove(UINT nFlags, CPoint point)
 {
 	if(!m_bMouseIn)
 	{
 		TRACKMOUSEEVENT tme = {0};
 		tme.cbSize = sizeof(TRACKMOUSEEVENT);
 		tme.dwFlags = TME_HOVER | TME_LEAVE;
 		tme.dwHoverTime = 1;
 		tme.hwndTrack = m_hWnd;
 		m_bMouseIn = TrackMouseEvent(&tme);
 	}
 	CBitmapButton::OnMouseMove(nFlags, point);
 }

void CWBBtn::SetImageFilePath()
 {
	 CString CurPicPath = GetCurrentPath();	//获取当前程序执行的目录	
	 int  BtnId;	
	 BtnId = GetDlgCtrlID();
	 switch(BtnId)
	 {
	 case IDC_ONLINE_BTN:
		 {
			
			m_ImageFilePath = CurPicPath +_T("\\SKIN\\LoginImage\\OnlineLandBtn.png");
			 
		 }
		 break;
	 case IDC_SINGLELAND_BTN:
		 {
			
			m_ImageFilePath = CurPicPath + _T("\\SKIN\\LoginImage\\SingleLandBtn.png");
			 
		 }
		 break;
	 case IDC_CLOSELOGINDLG_BTN:
		 {
			
			m_ImageFilePath = CurPicPath + _T("\\SKIN\\LoginImage\\LonginDlgCloseBtn.png");
			 
		 }
		 break;
	 case IDC_CANCEL_BTN:
		 {
			m_ImageFilePath = CurPicPath + _T("\\SKIN\\LoginImage\\CancelLandBtn.png");
		 }
		 break;
	 case IDC_CHECKMTFUNCTION:
		 {
			 if (m_isSelect)
			 {
				  m_ImageFilePath = CurPicPath + _T("\\SKIN\\LoginImage\\CheckMTFunctionBtn_check.png");
			 } 
			 else
			 {
				  m_ImageFilePath = CurPicPath + _T("\\SKIN\\LoginImage\\CheckMTFunctionBtn_normal.png");
			 }
		 }
		 break;
	 }
 }

void CWBBtn::SetIsSelect(BOOL bIsSelect)
{
	m_isSelect = bIsSelect;
}

BOOL CWBBtn::GetIsSelect()
{
	return m_isSelect;
}
