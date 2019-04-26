// WBAttributeStatic.cpp : ʵ���ļ�
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



// CWBAttributeStatic ��Ϣ�������




BOOL CWBAttributeStatic::OnEraseBkgnd(CDC* pDC)
{
	CString CurPicPath = GetExePath();	//��ȡ��ǰ����ִ�е�Ŀ¼
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
		, DEFAULT_QUALITY,FF_DONTCARE,_T("����"));
	dc.SelectObject(pFont);
	if (TRUE == m_bText)
	{
		dc.DrawText(m_strCaption,&rect, DT_LEFT);//���ı�����Static�����
	} 
	else
	{
		dc.DrawText(m_strCaption,&rect, DT_CENTER);//���ı�����Static������
	}
	
	delete pFont;
	// ��Ϊ��ͼ��Ϣ���� CStatic::OnPaint()
}
void CWBAttributeStatic::SetTextPosition(BOOL bText)
{
	m_bText = bText;
}

