#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <string>
#define WM_SHOWTIP_RESFIAL (WM_USER+222)

class CRichWnd
{
public:
	CRichWnd(HWND hParent = NULL,RECT *rt = NULL);
	virtual ~CRichWnd();
	void Invalidate();//Ë¢ÐÂ´°¿Ú
	virtual operator HWND() { return m_hWnd; };
protected:
	virtual void OnPaint(HDC hDC) = 0;
	virtual LRESULT MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	BOOL RenderRichWnd(HWND hWnd,HDC hDCdes,POINT *ptDes,SIZE *size,HDC hDCsrc,POINT *ptSrc,COLORREF color,BLENDFUNCTION *blen,DWORD flag);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT UserWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	HWND m_hParent;
	HWND m_hWnd;
	RECT m_wPos;
};

class CDrapEffectWnd:public CRichWnd
{
public:
	CDrapEffectWnd(POINT pt,HBITMAP hBitmap,HWND hParent = NULL,RECT *rt = NULL);
	void updateWindow(POINT pt,HBITMAP hBitmap,HWND hParent = NULL);
protected:
	void OnPaint(HDC hDC);
	LRESULT MessageHandle(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
	HBITMAP m_hBitmap;
	POINT m_offpt;
	HWND m_TargetWnd;
};

class CDrapEffect
{
public:
	static void updateWindow(HBITMAP hBitmap,POINT pt,HWND hParent = NULL);
	static void clearWindow();
private:
	static CDrapEffectWnd *g_pEffectWnd;
};