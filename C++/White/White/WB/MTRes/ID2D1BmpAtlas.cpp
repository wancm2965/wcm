#include "stdafx.h"
#include "ID2D1BmpAtlas.h"
#define ATLASSIZE 10

#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "windowscodecs.lib" )

ID2D1BmpAtlas* ID2D1BmpAtlas::CreateBmpAtlas( UINT cx, UINT cy, ID2D1RenderTarget *pRenderTarget )
{
	ID2D1BmpAtlas *pD2DBmpAtlas = new ID2D1BmpAtlas(cx,cy,pRenderTarget);
	if(pD2DBmpAtlas->AddPage()) return pD2DBmpAtlas;
	delete pD2DBmpAtlas;
	return NULL;
}

void ID2D1BmpAtlas::Release()
{
	delete this;
}

D2D_RECT_U ID2D1BmpAtlas::GetBitmap( HID2DBMP hD2dbmp )
{
	if((hD2dbmp != 0) && (hD2dbmp <= m_uCurPos))
	{
		return GetBmpRect(hD2dbmp);
	}
	return D2D1::RectU();
}
HID2DBMP ID2D1BmpAtlas::AddBitmap( ID2D1Bitmap* pBitmap )
{
	if(pBitmap == NULL) return 0;
	int page = GetPage(m_uCurPos+1);
	D2D_RECT_U destRt = GetBmpRect(m_uCurPos+1);
	D2D_POINT_2U destPoint = {destRt.left,destRt.top};
	D2D1_SIZE_F size = pBitmap->GetSize();
	m_pBmpSize[page-1][m_uCurPos%m_uAlbumVolume].width = size.width;
	m_pBmpSize[page-1][m_uCurPos%m_uAlbumVolume].height = size.height;
	m_pBitmap[page-1]->CopyFromBitmap(&destPoint,pBitmap,&D2D1::RectU(0,0,size.width,size.height));
	return ++m_uCurPos;
}

HID2DBMP ID2D1BmpAtlas::AddBitmap( UINT cx, UINT cy, BYTE* pDate, UINT WidthBytes /*= 0*/ )
{
	if(pDate == NULL) return 0;
	int page = GetPage(m_uCurPos+1);
	D2D_RECT_U destRt = GetBmpRect(m_uCurPos+1);
	destRt.right = destRt.left + cx;
	destRt.bottom = destRt.top + cy;
	m_pBmpSize[page-1][m_uCurPos%m_uAlbumVolume].width = cx;
	m_pBmpSize[page-1][m_uCurPos%m_uAlbumVolume].height = cy;
	if(WidthBytes == 0) WidthBytes = cx*4;
	m_pBitmap[page-1]->CopyFromMemory(&destRt,(void *)pDate,WidthBytes);
	return ++m_uCurPos;
	
}

HID2DBMP ID2D1BmpAtlas::AddBitmap( LPCTSTR *fileStr )
{
	return 0;
}

D2D_SIZE_U ID2D1BmpAtlas::GetBmpSize( HID2DBMP hD2dbmp )
{
	if(hD2dbmp == 0) return m_BmpSize;

	int page = 1 + (hD2dbmp-1)/100;//求出图像所在页数
	if(hD2dbmp <= m_uCurPos) return m_pBmpSize[page-1][(hD2dbmp-1)%m_uAlbumVolume];
	D2D_SIZE_U si = {0};
	return si;
}

void ID2D1BmpAtlas::ClearAll( bool bClearMem /*= false */ )
{
	if (bClearMem)
	{
		for (int i = 0; i < m_uAlbumPages; ++i)
		{
			SafeRelease(&m_pBitmap[i]);
			delete[] (m_pBmpSize[i]);
		}
		delete[] m_pBitmap;
		delete[] m_pBmpSize;
		m_pBitmap = NULL;
		m_pBmpSize = NULL;
		m_uAlbumPages = 0;
		m_uCurPos = 0;
		AddPage();
	}
	else
	{
		if(m_uCurPos > 0)
		{
			D2D_RECT_U clearRt = {0,0,m_AlbumSize.width,m_AlbumSize.height};
			BYTE* pData = new BYTE[clearRt.right*4*(clearRt.bottom)];
			ZeroMemory(pData,clearRt.right*4*(clearRt.bottom));
			for(int i = 0; i < m_uAlbumPages; ++i) m_pBitmap[i]->CopyFromMemory(&clearRt,pData,clearRt.right*4);
			delete pData;
			m_uCurPos = 0;
		}
	}
}
ID2D1BmpAtlas::ID2D1BmpAtlas( UINT cx, UINT cy, ID2D1RenderTarget* pRenderTarget )
	:m_pBitmap(NULL)
	,m_uCurPos(0)
	,m_pBmpSize(NULL)
	,m_pRenderTarget(pRenderTarget)
	,m_uAlbumPages(0)
{
	m_BmpSize.width = cx;
	m_BmpSize.height = cy;
	m_AlbumSize.width = ATLASSIZE*cx;
	m_AlbumSize.height = ATLASSIZE*cy;
	m_uAlbumVolume = ATLASSIZE*ATLASSIZE;
}

ID2D1BmpAtlas::~ID2D1BmpAtlas()
{
	for (int i = 0; i < m_uAlbumPages; ++i)
	{
		SafeRelease(&(m_pBitmap[i]));
		delete[] (m_pBmpSize[i]);
	}
	delete[] m_pBitmap;
	delete[] m_pBmpSize;
}

void ID2D1BmpAtlas::FreeBmpsFromBack( UINT Num /*= 1*/ )
{
	if(m_uCurPos > Num) m_uCurPos -= Num;
	else m_uCurPos = 0;
}

D2D_RECT_U ID2D1BmpAtlas::GetBmpRect( HID2DBMP hD2dbmp )
{
	int page = 1 + (hD2dbmp-1)/m_uAlbumVolume;//求出图像所在页数
	hD2dbmp = 1 + (hD2dbmp-1)%m_uAlbumVolume;//转换为页内ID
	D2D_RECT_U rt;
	if(hD2dbmp != 0)
	{
		rt.left = ((hD2dbmp-1)%ATLASSIZE)*m_BmpSize.width;
		rt.top = ((hD2dbmp-1)/ATLASSIZE)*m_BmpSize.height;
		if (hD2dbmp > m_uCurPos)
		{
			rt.right = rt.left + m_BmpSize.width;
			rt.bottom = rt.top + m_BmpSize.height;
		}
		else
		{
			rt.right = rt.left + m_pBmpSize[page-1][hD2dbmp-1].width;
			rt.bottom = rt.top + m_pBmpSize[page-1][hD2dbmp-1].height;
		}
	}
	return rt;
}

void ID2D1BmpAtlas::DrawBitmap( ID2D1RenderTarget *pRenderTarget,HID2DBMP hD2dbmp,const D2D1_RECT_F &destRt,FLOAT opacity /*= 1.0f*/
	, D2D1_BITMAP_INTERPOLATION_MODE interpolationMode /*= D2D1_BITMAP_INTERPOLATION_MODE_LINEAR*/,const D2D1_RECT_F *sourceRectangle /*= NULL */ )
{
	if((hD2dbmp==NULL)||(pRenderTarget==NULL)||(hD2dbmp>m_uCurPos)) return;
	D2D1_RECT_U srcRt = GetBmpRect(hD2dbmp);
	D2D1_RECT_F sourceRt;
	sourceRt.left = srcRt.left;
	sourceRt.top = srcRt.top;
	if(sourceRectangle == NULL) 
	{
		sourceRt.right = srcRt.right;
		sourceRt.bottom = srcRt.bottom;
	}
	else
	{
		sourceRt.left += sourceRectangle->left;
		sourceRt.top += sourceRectangle->top;
		sourceRt.right = sourceRt.left + (sourceRectangle->right - sourceRectangle->left);
		sourceRt.bottom = sourceRt.top + (sourceRectangle->bottom - sourceRectangle->top);
	}
	int page = 1 + (hD2dbmp-1)/100;//求出图像所在页数
	pRenderTarget->DrawBitmap(m_pBitmap[page-1],destRt,opacity,interpolationMode,&sourceRt);
}

D2D_SIZE_U ID2D1BmpAtlas::GetAlbumSize()
{
	return m_AlbumSize;
}

UINT ID2D1BmpAtlas::GetPage( HID2DBMP hD2dbmp )
{
	int page = 1 + (hD2dbmp-1)/100;//求出图像所在页数
	if (page > m_uAlbumPages)
	{
		if( !AddPage() ) return 0;
	}
	return page;
}

bool ID2D1BmpAtlas::AddPage()
{
	if (m_uAlbumPages%10 == 0)
	{
		D2D_SIZE_U** pSize = new D2D_SIZE_U*[m_uAlbumPages+10];
		ID2D1Bitmap** pBmp = new ID2D1Bitmap*[m_uAlbumPages+10];
		ZeroMemory(pSize,sizeof(D2D_SIZE_U*)*(m_uAlbumPages+10));
		ZeroMemory(pBmp,sizeof(ID2D1Bitmap*)*(m_uAlbumPages+10));
		if (m_uAlbumPages != 0 )
		{
			memcpy_s(pSize,m_uAlbumPages,m_pBmpSize,m_uAlbumPages);
			memcpy_s(pBmp,m_uAlbumPages,m_pBitmap,m_uAlbumPages);
			delete[] m_pBmpSize;
			delete[] m_pBitmap;
		}
		m_pBmpSize = pSize;
		m_pBitmap = pBmp;
	}
	m_pRenderTarget->CreateBitmap(m_AlbumSize,D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),&(m_pBitmap[m_uAlbumPages]));
	m_pBmpSize[m_uAlbumPages++] = new D2D_SIZE_U[ATLASSIZE*ATLASSIZE];
	return true;
}