// HeaderCtrlEx1.cpp : implementation file
//

#include "stdafx.h"
#include "HeaderCtrlExt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx

CHeaderCtrlEx::CHeaderCtrlEx()
{
}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	//{{AFX_MSG_MAP(CHeaderCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeaderCtrlEx message handlers



void CHeaderCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

   ASSERT(lpDrawItemStruct->CtlType == ODT_HEADER);

   HDITEM hdi;
   TCHAR  lpBuffer[256];

   hdi.mask = HDI_TEXT;
   hdi.pszText = lpBuffer;
   hdi.cchTextMax = 256;
   RECT txtrect;
   RECT backrect;
   RECT framerect;

   GetItem(lpDrawItemStruct->itemID, &hdi);

	
	CDC* pDC;
	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	framerect.top = lpDrawItemStruct->rcItem.top +2;
	framerect.bottom = lpDrawItemStruct->rcItem.bottom +2;
	framerect.left = lpDrawItemStruct->rcItem.left+2;
	framerect.right = lpDrawItemStruct->rcItem.right+2;

	

	//THIS FONT IS ONLY FOR DRAWING AS LONG AS WE DON'T DO A SetFont(...)
	pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
   // Draw the button frame.
   ::DrawFrameControl(lpDrawItemStruct->hDC, 
      &framerect, DFC_BUTTON, DFCS_BUTTONPUSH);


	UINT uFormat = DT_CENTER;
	//DRAW THE TEXT
	CBrush backBrush(RGB(118,168,194));

	backrect.top = lpDrawItemStruct->rcItem.top -2;
	backrect.bottom = lpDrawItemStruct->rcItem.bottom -2;
	backrect.left = lpDrawItemStruct->rcItem.left-2;
	backrect.right = lpDrawItemStruct->rcItem.right-2;

	::FillRect(lpDrawItemStruct->hDC,&backrect,backBrush);
	::SetBkColor(lpDrawItemStruct->hDC,RGB(118,168,194));
    txtrect.top = lpDrawItemStruct->rcItem.top + 8;
	txtrect.bottom = lpDrawItemStruct->rcItem.bottom - 8;
	txtrect.left = lpDrawItemStruct->rcItem.left;
	txtrect.right = lpDrawItemStruct->rcItem.right;
	::DrawText(lpDrawItemStruct->hDC, lpBuffer, wcslen(lpBuffer), 
      &txtrect, uFormat);


   pDC->SelectStockObject(SYSTEM_FONT);

}
