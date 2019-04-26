// BitmapTabCtr.cpp : 实现文件
//

#include "stdafx.h"
#include "whiteBoard.h"
#include "BitmapTabCtr.h"


// CBitmapTabCtr

IMPLEMENT_DYNAMIC(CBitmapTabCtr, CTabCtrl)

CBitmapTabCtr::CBitmapTabCtr()
{

	EnableAutomation();
}

CBitmapTabCtr::~CBitmapTabCtr()
{
}

void CBitmapTabCtr::OnFinalRelease()
{
	// 释放了对自动化对象的最后一个引用后，将调用
	// OnFinalRelease。基类将自动
	// 删除该对象。在调用该基类之前，请添加您的
	// 对象所需的附加清理代码。

	CTabCtrl::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CBitmapTabCtr, CTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CBitmapTabCtr, CTabCtrl)
END_DISPATCH_MAP()

// 注意: 我们添加 IID_IBitmapTabCtr 支持
//  以支持来自 VBA 的类型安全绑定。此 IID 必须同附加到 .IDL 文件中的
//  调度接口的 GUID 匹配。

// {9551FFFF-29EA-4074-97D1-04E9E4194033}
static const IID IID_IBitmapTabCtr =
{ 0x9551FFFF, 0x29EA, 0x4074, { 0x97, 0xD1, 0x4, 0xE9, 0xE4, 0x19, 0x40, 0x33 } };

BEGIN_INTERFACE_MAP(CBitmapTabCtr, CTabCtrl)
	INTERFACE_PART(CBitmapTabCtr, IID_IBitmapTabCtr, Dispatch)
END_INTERFACE_MAP()


// CBitmapTabCtr 消息处理程序




void CBitmapTabCtr::DrawItem(LPDRAWITEMSTRUCT lpDS /*lpDrawItemStruct*/)
{

	CDC dc; 
	dc.Attach( lpDS->hDC ); 	
	CString CurPicPath;	//获取当前程序执行的目录
	CImage img;
	if( lpDS->itemID == this->GetCurSel( ) ) 
	{ 
		CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\FocusTab.png");
		img.Load( CurPicPath );
		img.Draw( dc.m_hDC, lpDS->rcItem );
	}
	else
	{
		CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\NormalTab.png");
		img.Load( CurPicPath );
		img.Draw(dc.m_hDC, lpDS->rcItem);		
	}
	
	TCHAR buf[255]; 
	TCITEM item; 
	item.mask = TCIF_TEXT; 
	item.pszText = buf; 
	item.cchTextMax = sizeof( buf ); 
	this->GetItem( lpDS->itemID , &item ); 
	dc.SetBkMode( TRANSPARENT ); 
	//dc.SetTextAlign(TA_BASELINE);
	dc.SetTextColor( RGB( 0 , 0 , 0) ); 
 	CRect rcText( lpDS->rcItem ); 
	rcText.top += 5;
	dc.DrawText( buf , &rcText , DT_CENTER |DT_VCENTER); 
	dc.Detach(); 
}

void CBitmapTabCtr::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码

	DRAWITEMSTRUCT dis;
	dis.CtlType = ODT_TAB;
	dis.CtlID = GetDlgCtrlID();
	dis.hwndItem = GetSafeHwnd();
	dis.hDC = dc.GetSafeHdc();
	dis.itemAction = ODA_DRAWENTIRE;

	// draw the rest of the border
	CRect rPage;

	GetClientRect(&dis.rcItem);
	rPage = dis.rcItem;
	AdjustRect(FALSE, rPage);
	dis.rcItem.top = rPage.top - 2;

	//DrawMainBorder(&dis);

	// paint the tabs first and then the borders
	int nTab = GetItemCount();
	int nSel = GetCurSel();

	if (!nTab) // no pages added
		return;

	while (nTab--)
	{
		if (nTab != nSel)
		{
			dis.itemID = nTab;
			dis.itemState = 0;

			VERIFY(GetItemRect(nTab, &dis.rcItem));

			dis.rcItem.bottom -= 2;
			DrawItem(&dis);
			//DrawItemBorder(&dis);
		}
	}

	// now selected tab
	dis.itemID = nSel;
	dis.itemState = ODS_SELECTED;

	VERIFY(GetItemRect(nSel, &dis.rcItem));

	dis.rcItem.bottom += 2;
	dis.rcItem.top -= 2;
	DrawItem(&dis);
	//DrawItemBorder(&dis);

	// 不为绘图消息调用 CTabCtrl::OnPaint()
}


BOOL CBitmapTabCtr::OnEraseBkgnd(CDC* pDC)
{			
	CImage img;
	CRect clicentRc;
	GetClientRect(&clicentRc);

	CString CurPicPath = GetCurrentPath() + _T("\\SKIN\\WhiteBoard\\image\\ResourceBackground.png");

	img.Load( CurPicPath );
	img.Draw(pDC->m_hDC,clicentRc);

	return TRUE;

	//return CTabCtrl::OnEraseBkgnd(pDC);
}
