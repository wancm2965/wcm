#include "StdAfx.h"
#include "HorizontalLayoutUIEx.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

COptionUIEx::COptionUIEx( void )
{
}

LPVOID COptionUIEx::GetInterface( LPCTSTR pstrName )
{
	if (_tcscmp(pstrName, _T("OptionEx")) == 0)
	{
		return this;
	}
	return COptionUI::GetInterface(pstrName);
}

void COptionUIEx::DoEvent( TEventUI& event )
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pParent != NULL ) m_pParent->DoEvent(event);
		else CLabelUI::DoEvent(event);
		return;
	}

	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_KEYDOWN )
	{
		if (IsKeyboardEnabled()) {
			if( event.chKey == VK_SPACE || event.chKey == VK_RETURN ) {
				Activate();
				return;
			}
		}
	}
	if( event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK )
	{
		if( ::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled() ) {
			m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
			Invalidate();
		}
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) m_uButtonState |= UISTATE_PUSHED;
			else m_uButtonState &= ~UISTATE_PUSHED;
			Invalidate();
		}
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		if( (m_uButtonState & UISTATE_CAPTURED) != 0 ) {
			m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
			Invalidate();
			if (event.ptMouse.x == -1 && event.ptMouse.y == -1)
			{
				return;
			}
			if( ::PtInRect(&m_rcItem, event.ptMouse) ) Activate();
		}
		else if (event.ptMouse.x == -1 && event.ptMouse.y == -1)
		{
			return;
		}
	}
	if( event.Type == UIEVENT_CONTEXTMENU )
	{
		if( IsContextMenuUsed() ) 
		{
			m_pManager->SendNotify(this,0, event.wParam, event.lParam);
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() ) {
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( IsEnabled() ) {
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_SETCURSOR ) {
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
		return;
	}
	CLabelUI::DoEvent(event);
}

bool COptionUIEx::Activate()
{
	return CButtonUI::Activate();
}

CHorizontalLayoutUIEx::CHorizontalLayoutUIEx(void)
: m_pCurClick(NULL)
{
}

void CHorizontalLayoutUIEx::SetPos(RECT rc)
{
	CControlUI::SetPos(rc);
	rc = m_rcItem;

	// Adjust for inset
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;

	if( m_items.GetSize() == 0) {
		ProcessScrollBar(rc, 0, 0);
		return;
	}

	// 	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();

	// Determine the width of elements that are sizeable
	SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) 
		szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange();

	int nAdjustables = 0;
	int cxFixed = 0;
	int nEstimateNum = 0;
	for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) continue;
		SIZE sz = pControl->EstimateSize(szAvailable);
		if( sz.cx == 0 ) {
			nAdjustables++;
		}
		else {
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
		}
		cxFixed += sz.cx +  pControl->GetPadding().left + pControl->GetPadding().right;
		nEstimateNum++;
	}
	cxFixed += (nEstimateNum - 1) * m_iChildPadding;

	// Place elements
	int cyNeeded = 0;
	int cxExpand = 0;
	if( nAdjustables > 0 ) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
	// Position the elements
	SIZE szRemaining = szAvailable;
	int iPosX = rc.left;
	if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
		iPosX -= m_pHorizontalScrollBar->GetScrollPos();
	}
	int iAdjustable = 0;
	int cxFixedRemaining = cxFixed;
	for( int it2 = 0; it2 < m_items.GetSize(); it2++ ) {
		CControlUI* pControl = static_cast<CControlUI*>(m_items[it2]);
		if( !pControl->IsVisible() ) continue;
		if( pControl->IsFloat() ) {
			SetFloatPos(it2);
			continue;
		}
		RECT rcPadding = pControl->GetPadding();
		szRemaining.cx -= rcPadding.left;
		SIZE sz = pControl->EstimateSize(szRemaining);
		if( sz.cx == 0 ) {
			iAdjustable++;
			sz.cx = cxExpand;
			// Distribute remaining to last element (usually round-off left-overs)
			if( iAdjustable == nAdjustables ) {
				sz.cx = MAX(0, szRemaining.cx - rcPadding.right - cxFixedRemaining);
			}
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();
		}
		else {
			if( sz.cx < pControl->GetMinWidth() ) sz.cx = pControl->GetMinWidth();
			if( sz.cx > pControl->GetMaxWidth() ) sz.cx = pControl->GetMaxWidth();

			cxFixedRemaining -= sz.cx;
		}

		sz.cy = pControl->GetFixedHeight();
		if( sz.cy == 0 ) sz.cy = rc.bottom - rc.top - rcPadding.top - rcPadding.bottom;
		if( sz.cy < 0 ) sz.cy = 0;
		if( sz.cy < pControl->GetMinHeight() ) sz.cy = pControl->GetMinHeight();
		if( sz.cy > pControl->GetMaxHeight() ) sz.cy = pControl->GetMaxHeight();

		RECT rcCtrl = { iPosX + rcPadding.left, rc.top + rcPadding.top, iPosX + sz.cx + rcPadding.left + rcPadding.right, rc.top + rcPadding.top + sz.cy};
		pControl->SetPos(rcCtrl);
		iPosX += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
		cyNeeded += sz.cx + m_iChildPadding + rcPadding.left + rcPadding.right;
		szRemaining.cx -= sz.cx + m_iChildPadding + rcPadding.right;
	}

	if( m_pHorizontalScrollBar != NULL ) 
	{
		if( cyNeeded > rc.right - rc.left ) 
		{
			if( m_pHorizontalScrollBar->IsVisible() ) 
			{
				m_pHorizontalScrollBar->SetScrollRange(cyNeeded - (rc.right - rc.left));
				rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight();
			}
			else 
			{
				m_pHorizontalScrollBar->SetVisible(true);
				m_pHorizontalScrollBar->SetScrollRange(cyNeeded - (rc.right - rc.left));
				m_pHorizontalScrollBar->SetScrollPos(0);
			}
		}
		else 
		{
			if( m_pHorizontalScrollBar->IsVisible() ) 
			{
				m_pHorizontalScrollBar->SetVisible(false);
				m_pHorizontalScrollBar->SetScrollRange(0);
				m_pHorizontalScrollBar->SetScrollPos(0);
				rc.bottom += m_pHorizontalScrollBar->GetFixedHeight();
			}
		}
	}

	ProcessScrollBar(rc, iPosX, 0);
}

void CHorizontalLayoutUIEx::DoPaint( HDC hDC, const RECT& rcPaint )
{
	RECT rcTemp = { 0 };
	if( !::IntersectRect(&rcTemp, &rcPaint, &m_rcItem) ) return;

	CRenderClip clip;
	CRenderClip::GenerateClip(hDC, rcTemp, clip);
	CControlUI::DoPaint(hDC, rcPaint);

	if( m_items.GetSize() > 0 ) {
		RECT rc = m_rcItem;
		rc.left += m_rcInset.left;
		rc.top += m_rcInset.top;
		rc.right -= m_rcInset.right;
		rc.bottom -= m_rcInset.bottom;
		if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();

		if( !::IntersectRect(&rcTemp, &rcPaint, &rc) ) {
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( !pControl->IsVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
				if( pControl ->IsFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
					pControl->DoPaint(hDC, rcPaint);
				}
			}
		}
		else {
			CRenderClip childClip;
			CRenderClip::GenerateClip(hDC, rcTemp, childClip);
			for( int it = 0; it < m_items.GetSize(); it++ ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_items[it]);
				if( !pControl->IsVisible() ) continue;
				if( !::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos()) ) continue;
				if( pControl ->IsFloat() ) {
					if( !::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos()) ) continue;
					CRenderClip::UseOldClipBegin(hDC, childClip);
					pControl->DoPaint(hDC, rcPaint);
					CRenderClip::UseOldClipEnd(hDC, childClip);
				}
				else {
					if( !::IntersectRect(&rcTemp, &rc, &pControl->GetPos()) ) continue;
					pControl->DoPaint(hDC, rcPaint);
				}
			}
		}
	}

	if( m_pVerticalScrollBar != NULL && m_pVerticalScrollBar->IsVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()) ) {
			m_pVerticalScrollBar->DoPaint(hDC, rcPaint);
		}
	}

	if( m_pHorizontalScrollBar != NULL && m_pHorizontalScrollBar->IsVisible() ) {
		if( ::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()) ) {
			m_pHorizontalScrollBar->DoPaint(hDC, rcPaint);
		}
	}
}

void CHorizontalLayoutUIEx::DoEvent( TEventUI& event )
{
	CHorizontalLayoutUI::DoEvent(event);
}

CControlUI* CHorizontalLayoutUIEx::FindControl( FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags )
{
	// Check if this guy is valid
	if( (uFlags & UIFIND_VISIBLE) != 0 && !IsVisible() ) return NULL;
	if( (uFlags & UIFIND_ENABLED) != 0 && !IsEnabled() ) return NULL;
	if( (uFlags & UIFIND_HITTEST) != 0 ) {
		if( !::PtInRect(&m_rcItem, *(static_cast<LPPOINT>(pData))) ) return NULL;
		if( !m_bMouseChildEnabled ) {
			CControlUI* pResult = NULL;
			if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
			if( pResult == NULL && m_pHorizontalScrollBar != NULL ) pResult = m_pHorizontalScrollBar->FindControl(Proc, pData, uFlags);
			if( pResult == NULL ) pResult = CControlUI::FindControl(Proc, pData, uFlags);
			m_pCurClick = NULL;
			return pResult;
		}
	}

	CControlUI* pResult = NULL;
	m_pCurClick = NULL;
	if( m_pVerticalScrollBar != NULL ) pResult = m_pVerticalScrollBar->FindControl(Proc, pData, uFlags);
	if( pResult != NULL ) return pResult;

	if( (uFlags & UIFIND_ME_FIRST) != 0 ) {
		CControlUI* pControl = CControlUI::FindControl(Proc, pData, uFlags);
		if( pControl != NULL ) 
		{
			m_pCurClick = NULL;
			return pControl;
		}
	}
	RECT rc = m_rcItem;
	rc.left += m_rcInset.left;
	rc.top += m_rcInset.top;
	rc.right -= m_rcInset.right;
	rc.bottom -= m_rcInset.bottom;
	if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) rc.right -= m_pVerticalScrollBar->GetFixedWidth();
	if( (uFlags & UIFIND_TOP_FIRST) != 0 ) {
		for( int it = m_items.GetSize() - 1; it >= 0; it-- ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
				{
					m_pCurClick = pControl;
					return pControl;
				}            
			}
		}
	}
	else {
		for( int it = 0; it < m_items.GetSize(); it++ ) {
			CControlUI* pControl = static_cast<CControlUI*>(m_items[it])->FindControl(Proc, pData, uFlags);
			if( pControl != NULL ) {
				if( (uFlags & UIFIND_HITTEST) != 0 && !pControl->IsFloat() && !::PtInRect(&rc, *(static_cast<LPPOINT>(pData))) )
					continue;
				else 
				{
					m_pCurClick = pControl;
					return pControl;
				}  
			} 
		}
	}

	if( pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0 )
	{
		pResult = CControlUI::FindControl(Proc, pData, uFlags);
	}
	return pResult;
}

void CHorizontalLayoutUIEx::EnableScrollBar( bool bEnableVertical /*= true*/, bool bEnableHorizontal /*= false*/ )
{
	CHorizontalLayoutUI::EnableScrollBar(bEnableVertical, bEnableHorizontal);
	if (bEnableHorizontal && m_pHorizontalScrollBar != NULL)
	{
		m_pHorizontalScrollBar->SetFixedHeight(4);
		m_pHorizontalScrollBar->SetMouseEnabled(false);
		m_pHorizontalScrollBar->SetShowButton1(false);
		m_pHorizontalScrollBar->SetShowButton2(false);
	}
}