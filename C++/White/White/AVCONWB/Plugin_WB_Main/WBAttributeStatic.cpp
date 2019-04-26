// WBAttributeStatic.cpp : 实现文件
//

#include "stdafx.h"
#include "WBAttributeStatic.h"

//using namespace AVCONPlugin;
// CWBAttributeStatic

IMPLEMENT_DYNAMIC(CWBAttributeStatic, CStatic)

CWBAttributeStatic::CWBAttributeStatic()
{
	m_bText =  FALSE;
}

CWBAttributeStatic::~CWBAttributeStatic()
{
}


BEGIN_MESSAGE_MAP(CWBAttributeStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CWBAttributeStatic 消息处理程序




BOOL CWBAttributeStatic::OnEraseBkgnd(CDC* pDC)
{
	CString CurPicPath = GetExePath();	//获取当前程序执行的目录
	CImage img;
	CRect clicentRc;
	int  BtnId;
	BtnId = GetDlgCtrlID();
	GetClientRect(&clicentRc);
	switch (BtnId)
	{
	case IDC_SHOWLANDMESSAGE_STATIC:
		CurPicPath += _T("SKIN\\LoginImage\\ShowLandMessageStaticBG.png");
		break;
	
	}
	//CurPicPath += _T("\\SKIN\\WhiteBoard\\image\\functionDlg\\StaticBG.png");
	img.Load( CurPicPath );
	if (!img.IsNull())
	{
		img.Draw(pDC->m_hDC,clicentRc);
	}	

	return TRUE;
}

void CWBAttributeStatic::SetCaption( CString strCaption )
{
	m_strCaption = strCaption;	
	Invalidate();
}
void CWBAttributeStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect( &rect );
	dc.SetBkMode( TRANSPARENT );
	CFont *pFont = new CFont;
	pFont->CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, 1
		, OUT_DEFAULT_PRECIS
		, CLIP_DEFAULT_PRECIS
		, DEFAULT_QUALITY,FF_DONTCARE,_T("宋体"));
	dc.SelectObject(pFont);
	if (TRUE == m_bText)
	{
		dc.DrawText(m_strCaption,&rect, DT_LEFT);//将文本画在Static的左边
	} 
	else
	{
		dc.DrawText(m_strCaption,&rect, DT_CENTER);//将文本画在Static的中央
	}
	
	delete pFont;
	// 不为绘图消息调用 CStatic::OnPaint()
}
void CWBAttributeStatic::SetTextPosition(BOOL bText)
{
	m_bText = bText;
}

