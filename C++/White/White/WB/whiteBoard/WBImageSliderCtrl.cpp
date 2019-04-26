// WBImageSliderCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "WBImageSliderCtrl.h"
#include "WBFunctionAttributeDlg.h"


// CWBImageSliderCtrl

IMPLEMENT_DYNAMIC(CWBImageSliderCtrl, CSliderCtrl)

CWBImageSliderCtrl::CWBImageSliderCtrl()
{
	 SetPrimaryColor( RGB(255, 255, 0) ); 
}

CWBImageSliderCtrl::CWBImageSliderCtrl(COLORREF cr)
{
	SetPrimaryColor( cr) ; 
}
CWBImageSliderCtrl::~CWBImageSliderCtrl()
{
}


BEGIN_MESSAGE_MAP(CWBImageSliderCtrl, CSliderCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CWBImageSliderCtrl::OnNMCustomdraw)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



// CWBImageSliderCtrl 消息处理程序
BOOL CWBImageSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	return CSliderCtrl::OnEraseBkgnd(pDC);
}
void CWBImageSliderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNMHDR;

	UINT drawStage = nmcd.dwDrawStage;
	UINT itemSpec = nmcd.dwItemSpec;

	switch ( drawStage )
	{
	case CDDS_PREPAINT:     
		*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT ;
		break;

	case CDDS_PREERASE:     // Before the erase cycle begins
	case CDDS_POSTERASE:    // After the erase cycle is complete
	case CDDS_ITEMPREERASE: // Before an item is erased
	case CDDS_ITEMPOSTERASE:    // After an item has been erased
		*pResult = CDRF_DODEFAULT;
		break;

	case CDDS_ITEMPREPAINT: // Before an item is drawn

		switch ( itemSpec )
		{
		case TBCD_CHANNEL:  // 画中间的通道之前画底图
			{
			    CDC* pDC = CDC::FromHandle( nmcd.hdc );
				//CString CurPicPath;	//获取当前程序执行的目录
				CImage img;
				CRect clicentRc;
				GetClientRect(&clicentRc);
				CString CurPicPath = GetCurrentPath()+ "\\SKIN\\WhiteBoard\\image\\functionDlg\\Transparent.png";
				img.Load( CurPicPath );
				if (!img.IsNull())
				{
					img.Draw(pDC->m_hDC,clicentRc);
				}	
				*pResult = CDRF_DODEFAULT| CDRF_NOTIFYPOSTPAINT;
			}
			break;

		case TBCD_TICS:    
			*pResult = CDRF_DODEFAULT;
			break;

		case TBCD_THUMB:    // trackbar control's thumb marker. This is the portion of the control that the user moves

			{
				CDC* pDC = CDC::FromHandle( nmcd.hdc );
				int iSaveDC = pDC->SaveDC();

				CBrush* pB = &m_normalBrush;
				CPen pen( PS_SOLID, 1, m_crShadow );

				// 中心滑块选中状态的填充中心
				if ( nmcd.uItemState && CDIS_FOCUS )
				{
					pB = &m_focusBrush;
					pDC->SetBrushOrg( nmcd.rc.right%8, nmcd.rc.top%8 );
					pDC->SetBkColor( m_crPrimary );
					pDC->SetTextColor( m_crHilite );             
				}

				pDC->SelectObject( pB );
				pDC->SelectObject( &pen );

#if 1   // draw an ellipse
				//画中心的滑块
				pDC->Rectangle( &(nmcd.rc) );//画滑动块的矩形

#else   // draw a diamond

				int xx, yy, dx, dy, cx, cy;
				xx = nmcd.rc.left;
				yy = nmcd.rc.top;
				dx = 2;
				dy = 2;
				cx = nmcd.rc.right - xx - 1;
				cy = nmcd.rc.bottom - yy - 1;
				POINT pts[8] = { {xx+dx, yy}, {xx, yy+dy}, {xx, yy+cy-dy}, {xx+dx, yy+cy},
				{xx+cx-dx, yy+cy}, {xx+cx, yy+cy-dy}, {xx+cx, yy+dy}, {xx+cx-dx, yy} };

				pDC->Polygon( pts, 8 );

#endif  // which shape to draw

				pDC->RestoreDC( iSaveDC );
				pDC->Detach();
			}
			*pResult = CDRF_SKIPDEFAULT;    // don't let control draw itself, or it will un-do our work
			break;

		default:
			ASSERT( FALSE );    // all of a slider's items have been listed, so we shouldn't get here
		};

		break;

	case CDDS_ITEMPOSTPAINT:    // After an item has been drawn

		switch ( itemSpec )
		{
		case TBCD_CHANNEL:  // channel that the trackbar control's thumb marker slides along
			{
				CDC* pDC = CDC::FromHandle( nmcd.hdc );

				RECT rrc = {nmcd.rc.left+1, nmcd.rc.top+1, nmcd.rc.right-1, nmcd.rc.bottom-1};
	     		pDC->Draw3dRect( &rrc, m_crMidShadow, m_crHilite );
				pDC->Detach();
			}
			*pResult = CDRF_SKIPDEFAULT;
			break;

		case TBCD_TICS:     // the increment tick marks that appear along the edge of the trackbar control
			// currently, there is no special post-item-paint drawing of the tics

			*pResult = CDRF_DODEFAULT;
			break;

		case TBCD_THUMB:    // trackbar control's thumb marker. This is the portion of the control that the user moves
			// currently, there is no special post-item-paint drawing for the thumb

			*pResult = CDRF_DODEFAULT ; // don't let control draw itself, or it will un-do our work
			break;

		default:
			ASSERT( FALSE );    // all of a slider's items have been listed, so we shouldn't get here
		};

		break;

	case CDDS_POSTPAINT:      //在固定位置上画图形  
		{
			//CDC* pDC = CDC::FromHandle( nmcd.hdc );

			//CBrush bWhite( RGB(255, 255, 255) );    // white brush
			//CBrush bDark( m_crDarkerShadow );       // dark but still colored brush
			//CPen p(PS_SOLID, 1, m_crPrimary);

			//CRect rClient;
			//GetClientRect( &rClient );
			//DWORD dwStyle = GetStyle();


			//int cx = 8;
			//CRect rrcFirst;( 1, 1, cx, cx );
			//CRect rrcLast;
			//if ( dwStyle & TBS_VERT )
			//{
			//	if ( dwStyle & TBS_LEFT )
			//	{
			//		rrcFirst = CRect( rClient.right-cx, 1, rClient.right-1, cx ); 
			//		rrcLast = CRect( rClient.right-cx, rClient.bottom-cx, rClient.right-1, rClient.bottom-1 );
			//	}
			//	else
			//	{
			//		rrcFirst = CRect( 1, 1, cx, cx ); 
			//		rrcLast = CRect( 1, rClient.bottom-cx, cx, rClient.bottom-1 );
			//	}
			//}
			//else
			//{
			//	if ( dwStyle & TBS_TOP )
			//	{
			//		rrcFirst = CRect( 1, rClient.bottom-cx, cx, rClient.bottom-1 ); 
			//		rrcLast = CRect( rClient.right-cx, rClient.bottom-cx, rClient.right-1, rClient.bottom-1 );
			//	}
			//	else
			//	{
			//		rrcFirst = CRect( 1, 1, cx, cx ); 
			//		rrcLast = CRect( rClient.right-cx, 1, rClient.right-1, cx );
			//	}
			//}

			//int iSave = pDC->SaveDC();
			//pDC->SelectObject( &bWhite );
			//pDC->SelectObject( &p );
			//pDC->Ellipse( &rrcFirst );
			//pDC->SelectObject( &bDark );
			//pDC->Ellipse( &rrcLast );
			//pDC->RestoreDC( iSave );
			//pDC->Detach();
		} 
		*pResult = CDRF_SKIPDEFAULT;
		break;

	default:            
		ASSERT( FALSE );    // all drawing stages are listed, so we shouldn't get here          
	};

}
void CWBImageSliderCtrl::SetPrimaryColor(COLORREF cr)
{
      m_crPrimary = cr; 
      m_crHilite      = ::ColorAdjustLuma( cr, 500, TRUE );   // increase by 50%
      m_crMidShadow   = ::ColorAdjustLuma( cr, -210, TRUE );  // decrease by 21.0%
      m_crShadow      = ::ColorAdjustLuma( cr, -333, TRUE );  // decrease by 33.3%
      m_crDarkerShadow = ::ColorAdjustLuma( cr, -500, TRUE ); // decrease by 50.0%  
      // create normal (solid) brush 
      if ( m_normalBrush.m_hObject )
          m_normalBrush.DeleteObject();
       m_normalBrush.CreateSolidBrush( cr );      
      WORD bitsBrush1[8] = { 0x0055,0x00aa,0x0055,0x00aa,0x0055,0x00aa,0x0055,0x00aa };
      
      CBitmap bm;
      bm.CreateBitmap( 8, 8, 1, 1, bitsBrush1);
      
      LOGBRUSH logBrush;
      logBrush.lbStyle = BS_PATTERN;
      logBrush.lbHatch = (int) bm.GetSafeHandle();
      logBrush.lbColor = 0;       // ignored anyway; must set DC background and text colors
      if ( m_focusBrush.m_hObject )
          m_focusBrush.DeleteObject();
  
      m_focusBrush.CreateBrushIndirect(&logBrush);
}  
COLORREF CWBImageSliderCtrl::GetPrimaryColor() const
{
      return m_crPrimary;
}  
COLORREF CWBImageSliderCtrl::GetHiliteColor() const
{
      return m_crHilite;
}  
COLORREF CWBImageSliderCtrl::GetShadowColor() const
{
     return m_crShadow;
}


void CWBImageSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	((CWBFunctionAttributeDlg *)GetParent())->SetSliderCtrlPos();
	((CWBFunctionAttributeDlg *)GetParent())->SetFocus();
	CSliderCtrl::OnLButtonUp(nFlags, point);
}
