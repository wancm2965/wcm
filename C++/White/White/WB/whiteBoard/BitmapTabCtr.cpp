// BitmapTabCtr.cpp : ʵ���ļ�
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
	// �ͷ��˶��Զ�����������һ�����ú󣬽�����
	// OnFinalRelease�����ཫ�Զ�
	// ɾ���ö����ڵ��øû���֮ǰ�����������
	// ��������ĸ���������롣

	CTabCtrl::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CBitmapTabCtr, CTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CBitmapTabCtr, CTabCtrl)
END_DISPATCH_MAP()

// ע��: ������� IID_IBitmapTabCtr ֧��
//  ��֧������ VBA �����Ͱ�ȫ�󶨡��� IID ����ͬ���ӵ� .IDL �ļ��е�
//  ���Ƚӿڵ� GUID ƥ�䡣

// {9551FFFF-29EA-4074-97D1-04E9E4194033}
static const IID IID_IBitmapTabCtr =
{ 0x9551FFFF, 0x29EA, 0x4074, { 0x97, 0xD1, 0x4, 0xE9, 0xE4, 0x19, 0x40, 0x33 } };

BEGIN_INTERFACE_MAP(CBitmapTabCtr, CTabCtrl)
	INTERFACE_PART(CBitmapTabCtr, IID_IBitmapTabCtr, Dispatch)
END_INTERFACE_MAP()


// CBitmapTabCtr ��Ϣ�������




void CBitmapTabCtr::DrawItem(LPDRAWITEMSTRUCT lpDS /*lpDrawItemStruct*/)
{

	CDC dc; 
	dc.Attach( lpDS->hDC ); 	
	CString CurPicPath;	//��ȡ��ǰ����ִ�е�Ŀ¼
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
	// TODO: �ڴ˴������Ϣ����������

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

	// ��Ϊ��ͼ��Ϣ���� CTabCtrl::OnPaint()
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
