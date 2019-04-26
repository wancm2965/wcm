#include "stdafx.h"
#include "DrapEffect.h"
#define UWM_PAINT (WM_USER + WM_PAINT)//自定义绘制消息
void CRichWnd::Invalidate()
{
	::PostMessage(m_hWnd,UWM_PAINT,0xFF,0xFF);
}

CRichWnd::CRichWnd(HWND hParent /*= NULL*/,RECT *rt /*= NULL*/)
:m_hParent(hParent)
{
	RECT defRt = {0,0,10,10};
	if (rt == NULL)
	{
		rt = &defRt;
	}
	m_wPos = *rt;
	rt->right -= rt->left;
	rt->bottom -= rt->top;
	WNDCLASS wc = {0};
	HINSTANCE hIn = (HINSTANCE)GetModuleHandle(NULL);
	wc.lpfnWndProc		= &CRichWnd::WndProc;
	wc.hInstance		= hIn;
	wc.lpszClassName	= _T("RichWnd");
	UnregisterClass(wc.lpszClassName,hIn);
	RegisterClass(&wc);
	m_hWnd = CreateWindow(wc.lpszClassName, NULL, WS_POPUP,rt->left, 
		rt->top, rt->right, rt->bottom, m_hParent, NULL, NULL, NULL);
	::SetWindowLong(m_hWnd,GWL_EXSTYLE,GetWindowLong(m_hWnd, GWL_EXSTYLE)|WS_EX_TOOLWINDOW);
	::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);
	assert(IsWindow(m_hWnd));
}

BOOL CRichWnd::RenderRichWnd( HWND hWnd,HDC hDCdes,POINT *ptDes,SIZE *size,HDC hDCsrc,POINT *ptSrc,COLORREF color,BLENDFUNCTION *blen,DWORD flag )
{
	static HINSTANCE hInst = LoadLibrary(L"User32.DLL");
	typedef BOOL (WINAPI * MYFUNC)(HWND,HDC,POINT *,SIZE *,HDC,POINT *,COLORREF,BLENDFUNCTION *,DWORD);
	if (hInst != NULL)
	{
		//取得SetLayeredWindowAttributes函数指针 
		static MYFUNC fun = (MYFUNC)GetProcAddress(hInst,"UpdateLayeredWindow"); 
		if(fun != NULL) 
		{
			POINT pt = {0,0};
			if (ptSrc == NULL)
			{
				ptSrc = &pt;
			}
			fun(hWnd,hDCdes,ptDes,size,hDCsrc,ptSrc,color,blen,flag);
			return TRUE;
		}
		else
		{
			fun = (MYFUNC)GetProcAddress(hInst,"UpdateLayeredWindow"); 
		}
	}
	else
	{
		hInst = LoadLibrary(L"User32.DLL");
	}
	return FALSE;
}

LRESULT CRichWnd::UserWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	BOOL bUsePaint = FALSE;
	switch(message)
	{
	case UWM_PAINT:
		if (wParam != 0xFF || lParam != 0xFF)
		{
			break;
		}
		bUsePaint = TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			if (bUsePaint)
			{
				hdc = GetDC(hWnd);
			}
			else
			{
				::SetWindowLong(hWnd,GWL_EXSTYLE,::GetWindowLong(hWnd,GWL_EXSTYLE)|WS_EX_LAYERED|WS_EX_TRANSPARENT);
				hdc = BeginPaint(hWnd, &ps);
			}

			HDC hMemDC = ::CreateCompatibleDC(hdc);
			BITMAPINFO bmi;
			::ZeroMemory(&bmi, sizeof(BITMAPINFO));
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = m_wPos.right - m_wPos.left;
			bmi.bmiHeader.biHeight = m_wPos.top - m_wPos.bottom;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = (m_wPos.right - m_wPos.left) * (m_wPos.bottom - m_wPos.top) * 4;
			LPBYTE pDest = NULL;
			HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void**)&pDest, NULL, 0);
			HBITMAP hbmp = (HBITMAP)::SelectObject(hMemDC,hBitmap);
			OnPaint(hMemDC);
			BLENDFUNCTION blendPixelFunction= { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
			POINT ptDes = {m_wPos.left,m_wPos.top};
			SIZE size = {m_wPos.right - m_wPos.left,m_wPos.bottom - m_wPos.top};
			RenderRichWnd(hWnd,GetDC(NULL),&ptDes,&size,hMemDC,NULL,0,&blendPixelFunction,ULW_ALPHA);
			::SelectObject(hMemDC,hbmp);
			DeleteObject(hBitmap);
			if (bUsePaint)
			{
				::ReleaseDC(hWnd,hdc);
			}
			else
			{
				EndPaint(hWnd,&ps);
			}
		}
		return 0;
	default:
		break;
	}
	return MessageHandle(hWnd,message,wParam,lParam);
}

LRESULT CALLBACK CRichWnd::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	CRichWnd *pThis = (CRichWnd*)GetWindowLong(hWnd,GWL_USERDATA);
	if (pThis != NULL)
	{
		if (message == WM_NCDESTROY)
		{
			::SetWindowLong(hWnd,GWL_USERDATA,(LONG)NULL);
			delete pThis;
		}
		else
		{
			return pThis->UserWndProc(hWnd,message,wParam,lParam);
		}
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

LRESULT CRichWnd::MessageHandle( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_NCHITTEST)
	{
		//return HTCAPTION;
	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

CRichWnd::~CRichWnd()
{
	if ( NULL != GetWindowLong(m_hWnd,GWL_USERDATA))
	{
		::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)NULL);
		::DestroyWindow(m_hWnd);
	}
}

void CDrapEffectWnd::OnPaint( HDC hDC )
{
	assert(::GetObjectType(hDC)==OBJ_DC || ::GetObjectType(hDC)==OBJ_MEMDC);

	typedef BOOL (WINAPI *LPALPHABLEND)(HDC, int, int, int, int,HDC, int, int, int, int, BLENDFUNCTION);
	static HMODULE handle = LoadLibrary(_T("msimg32.dll"));
	static LPALPHABLEND lpAlphaBlend = (LPALPHABLEND) ::GetProcAddress(handle, "AlphaBlend");
	HDC hCloneDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, m_hBitmap);
	::SetStretchBltMode(hDC, COLORONCOLOR);
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 200, AC_SRC_ALPHA };
	BITMAP bmp;
	::GetObject(m_hBitmap,sizeof(bmp),&bmp);
	lpAlphaBlend(hDC,0,0,bmp.bmWidth,bmp.bmHeight,hCloneDC,0,0,bmp.bmWidth,bmp.bmHeight,bf);
	::SelectObject(hCloneDC,hOldBitmap);
	::DeleteDC(hCloneDC);
}

LRESULT CDrapEffectWnd::MessageHandle( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_LBUTTONUP || message == WM_NCLBUTTONUP)
	{
		ReleaseCapture();
		ShowWindow(hWnd,SW_HIDE);
		::PostMessage(m_TargetWnd,WM_SHOWTIP_RESFIAL,0,0);
	}
	else if (message == WM_MOUSEMOVE)
	{
		POINT curPos;
		::GetCursorPos(&curPos);
		::MoveWindow(m_hWnd,curPos.x-m_offpt.x,curPos.y-m_offpt.y,96,96,FALSE);

	}
	return DefWindowProc(hWnd,message,wParam,lParam);
}

void CDrapEffectWnd::updateWindow( POINT pt,HBITMAP hBitmap,HWND hParent /*= NULL*/ )
{
	hParent = NULL;
	if (*this == ::GetCapture())
	{
		::ReleaseCapture();
	}
	ShowWindow(*this,SW_HIDE);
	m_offpt = pt;
	m_hBitmap = hBitmap;
	m_hParent = hParent;
	POINT curPos;
	::GetCursorPos(&curPos);
	OffsetRect(&m_wPos,-m_wPos.left,-m_wPos.top);
	OffsetRect(&m_wPos,curPos.x-m_offpt.x,curPos.y-m_offpt.y);
	if (hParent != GetParent(*this))
	{
		::SetParent(*this,hParent);
	}
	::SetWindowPos(*this,NULL,m_wPos.left,m_wPos.top,0,0,SWP_NOSIZE);
	Invalidate();
	ShowWindow(*this,SW_NORMAL);
	::SetCapture(*this);
}

CDrapEffectWnd::CDrapEffectWnd( POINT pt,HBITMAP hBitmap,HWND hParent /*= NULL*/,RECT *rt /*= NULL*/ ) 
	:CRichWnd(NULL,rt)
	,m_hBitmap(hBitmap)
	,m_offpt(pt)
{
	m_TargetWnd = hParent;
	updateWindow(pt,hBitmap,NULL);
	ShowWindow(*this,SW_NORMAL);
}
CDrapEffectWnd * CDrapEffect::g_pEffectWnd = NULL;

void CDrapEffect::updateWindow( HBITMAP hBitmap,POINT pt,HWND hParent /*= NULL*/ )
{
	if (g_pEffectWnd == NULL)
	{
		RECT rt = {0,0,96,96};
		g_pEffectWnd = new CDrapEffectWnd(pt,hBitmap,hParent,&rt);
	}
	else
	{
		g_pEffectWnd->updateWindow(pt,hBitmap,hParent);
	}
}

void CDrapEffect::clearWindow()
{
	if (g_pEffectWnd != NULL)
	{
		if (*g_pEffectWnd == ::GetCapture())
		{
			::ReleaseCapture();
		}
		delete g_pEffectWnd;
		g_pEffectWnd = NULL;
	}
}