// StyleButton5.cpp : 实现文件
//

#include "stdafx.h"
#include "StyleButton5.h"
#include ".\stylebutton5.h"

#include "atlimage.h"


CRgn g_rgnOriStyleButton;
int CStyleButton5::m_iWidthOri = 0;
int CStyleButton5::m_iHeightOri = 0;

// CStyleButton5

IMPLEMENT_DYNAMIC(CStyleButton5, CButton)
CStyleButton5::CStyleButton5()
{
	m_pImageBg = NULL;
	m_iWidth = 0;
	m_iHeight = 0;
	m_iFrameCount = 0;

	m_iSelect = 0;

	ColorMap defaultMap;
	defaultMap.oldColor = Color(255, 255, 0, 255);
	defaultMap.newColor = Color(0, 255, 0, 255);
	m_imAtt.SetRemapTable(1, &defaultMap, ColorAdjustTypeDefault);

	m_rgn.CreateRectRgn(0,0,0,0);
}

CStyleButton5::~CStyleButton5()
{
	Clear();
	m_rgn.DeleteObject();
}


BEGIN_MESSAGE_MAP(CStyleButton5, CButton)
	ON_WM_ERASEBKGND()
	ON_WM_MOVE()
END_MESSAGE_MAP()



// CStyleButton5 消息处理程序


void CStyleButton5::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	UINT state = lpDrawItemStruct->itemState;

	/*
	if((state & ODS_FOCUS)==ODS_FOCUS)
	{
		// 有输入焦点
		DrawFrame(0, lpDrawItemStruct->hDC);
	}
	else if((state & ODS_SELECTED)==ODS_SELECTED)
	{
		// 被选中
		DrawFrame(0, lpDrawItemStruct->hDC);
	}
	else if((state & ODS_DISABLED)==ODS_DISABLED)
	{
		// 不可用
		DrawFrame(0, lpDrawItemStruct->hDC);
	}
	else
	{
		// 其他
		DrawFrame(0, lpDrawItemStruct->hDC);
	}*/
	BOOL bFocus = ((state & ODS_FOCUS)==ODS_FOCUS);
	BOOL bSelected = /*(GetCheck()==BST_CHECKED)*/m_iSelect;
	BOOL bDisable = ((state & ODS_DISABLED)==ODS_DISABLED);
	int iPos = 0;
	if(bDisable)
	{
		iPos = 4;
	}
	else
	{
		if(bFocus)
		{
			if(bSelected)
			{
				iPos = 3;
			}
			else
			{
				iPos = 1;
			}
		}
		else
		{
			if(bSelected)
			{
				iPos = 2;
			}
			else
			{
				iPos = 0;
			}
		}
	}
	DrawFrame(iPos, lpDrawItemStruct->hDC);
}

void CStyleButton5::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	
	UINT iStyle = GetButtonStyle();
	if((iStyle&BS_OWNERDRAW) != BS_OWNERDRAW)
	{
		iStyle |= BS_OWNERDRAW;
		SetButtonStyle( iStyle );
	}
	

	CButton::PreSubclassWindow();
}

BOOL CStyleButton5::LoadButtonImage(CString sFile, int iWidth, int iHeight)
{
	Clear();

	if(iWidth <= 0 || iHeight <= 0)
	{
		return FALSE;
	}
	if (!PathFileExists(sFile))
	{
		SetWindowPos(NULL, 0, 0, m_iWidth, m_iHeight, SWP_NOMOVE);
		return FALSE;
	}

	/*TCHAR data[512];
	::GetModuleFileName(AfxGetInstanceHandle(), data, 512);
	TCHAR* p = _tcsrchr(data, '\\');
	if(p != NULL) *p=0;
	CString sFileTmp(data);
	sFileTmp += L"\\";
	sFileTmp += sFile;

	m_pImageBg = Bitmap::FromFile(sFileTmp, FALSE);*/
	m_pImageBg = Bitmap::FromFile(sFile, FALSE);
	if(m_pImageBg == NULL)
	{
		return FALSE;
	}
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	int iW = m_pImageBg->GetWidth()/m_iWidth;
	int iH = m_pImageBg->GetHeight()/m_iHeight;
	if(iW <= 0 || iH <= 0)
	{
		Clear();
		return FALSE;
	}
	m_iFrameCount = iW*iH;

	SetWindowPos(NULL, 0, 0, m_iWidth, m_iHeight, SWP_NOMOVE);

	UpdateRgn();

	return TRUE;
}

void CStyleButton5::DrawFrame(int iFrame, HDC hDC)
{
	if(m_iFrameCount <= 0) return;
	if(m_iWidth <= 0) return;
	if(m_iHeight <= 0) return;
	if(m_pImageBg == NULL) return;
	if(hDC == NULL) return;

	if(iFrame >= m_iFrameCount)
	{
		iFrame = 0;
	}

	int iW = m_pImageBg->GetWidth()/m_iWidth;
	int iY = 0;
	int iX = iFrame;
	while(iX >= iW)
	{
		iX -= iW;
		iY++;
	}

	int i1 = m_pImageBg->GetWidth();
	Graphics graph(hDC);
//	graph.DrawImage(m_pImageBg, Rect(0, 0, m_iWidth, m_iHeight),iX*m_iWidth, iY*m_iHeight, m_iWidth, m_iHeight, UnitPixel, &m_imAtt);
	graph.DrawImage(m_pImageBg, Rect(0, 0, m_iWidth, m_iHeight),iX*m_iWidth, iY*m_iHeight, m_iWidth, m_iHeight, UnitPixel, NULL);
	graph.Flush();
	graph.ReleaseHDC(hDC);
}

void CStyleButton5::Clear()
{
	if(m_pImageBg)
	{
		delete m_pImageBg;
		m_pImageBg = NULL;
	}
	m_iWidth = 0;
	m_iHeight = 0;
	m_iFrameCount = 0;
}

BOOL CStyleButton5::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
}

void CStyleButton5::UpdateRgn(int iFrame /* = 0 */)
{
	if(m_pImageBg == NULL) return;
	if(m_iWidth == m_iWidthOri && m_iHeight == m_iHeightOri)
	{
		if(g_rgnOriStyleButton.m_hObject)
		{
			m_rgn.SetRectRgn(0, 0, 0, 0);
			m_rgn.CopyRgn(&g_rgnOriStyleButton);
			int x = 0, y = 0;
			CRect rect;
			GetWindowRect(rect);
			CRect rcParent(0, 0, 0, 0);
			CWnd* pPerant = GetParent();
			if(pPerant && pPerant->GetSafeHwnd())
			{
				pPerant->GetWindowRect(rcParent);
			}
			x=rect.left-rcParent.left;
			y=rect.top-rcParent.top;
			m_rgn.OffsetRgn(x, y);
			return;
		}
	}
	int iW = m_pImageBg->GetWidth()/m_iWidth;
	int iY = 0;
	int iX = iFrame;
	while(iX >= iW)
	{
		iX -= iW;
		iY++;
	}
	CRgn r, rr;
	r.CreateRectRgn(0, 0, 0, 0);
	rr.CreateRectRgn(0, 0, 0, 0);
	m_rgn.SetRectRgn(0, 0, 0, 0);
	if(g_rgnOriStyleButton.m_hObject)
	{
		g_rgnOriStyleButton.SetRectRgn(0, 0, 0, 0);
	}
	else
	{
		g_rgnOriStyleButton.CreateRectRgn(0, 0, 0, 0);
	}

	CRect rect;
	int x = 0, y = 0;
	CRect rcParent(0, 0, 0, 0);
	CWnd* pPerant = GetParent();
	if(pPerant && pPerant->GetSafeHwnd())
	{
		pPerant->GetWindowRect(rcParent);
	}
	GetWindowRect(rect);
	BitmapData bitmapData;
	Rect rt(iX*m_iWidth, iY*m_iHeight, iX*m_iWidth+m_iWidth, iY*m_iHeight+m_iHeight);

	m_pImageBg->LockBits(
		&rt,
		ImageLockModeRead,
		PixelFormat32bppARGB,
		&bitmapData);
	UINT* pixels = (UINT*)bitmapData.Scan0;
	for(int row = 0; row < bitmapData.Height; ++row)
	{
		for(int col = 0; col < bitmapData.Width; ++col)
		{
			if( ( *(pixels+row * bitmapData.Stride / 4 + col) & 0xFF000000) == 0xFF000000)
			{
				rr.SetRectRgn(x+col, y+row, x+col+1, y+row+1);
				g_rgnOriStyleButton.CombineRgn(&g_rgnOriStyleButton, &rr, RGN_XOR);
			}
		}
	}
	m_pImageBg->UnlockBits(&bitmapData);

	m_rgn.CopyRgn(&g_rgnOriStyleButton);
	m_rgn.OffsetRgn(rect.left-rcParent.left, rect.top-rcParent.top);
	m_iWidthOri = m_iWidth;
	m_iHeightOri = m_iHeight;
	return;
}

CRgn* CStyleButton5::GetRgn()
{
	return &m_rgn;
}

void CStyleButton5::OnMove(int x, int y)
{
	CButton::OnMove(x, y);

	UpdateRgn();
}

void CStyleButton5::SetSelect()
{
	if(m_iSelect == 0)
	{
		m_iSelect = 1;
	}
	else
	{
		m_iSelect = 0;
	}
	Invalidate();
}
int CStyleButton5::GetSelect()
{
	return m_iSelect;
}
