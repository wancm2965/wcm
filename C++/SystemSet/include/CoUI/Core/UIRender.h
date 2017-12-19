#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#include "../Utils/Utils.h"
#include "../Core/UIManager.h"

namespace CoUI {
/////////////////////////////////////////////////////////////////////////////////////
//
#define RGBA(r,g,b,a) ((RGB(r,g,b)|((DWORD)((BYTE)(a))<<24)))  
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))

class UILIB_API CRenderClip
{
public:
    ~CRenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;

    static void GenerateClip(HDC hDC, RECT rc, CRenderClip& clip);
    static void GenerateRoundClip(HDC hDC, RECT rc, RECT rcItem, int width, int height, CRenderClip& clip);
    static void UseOldClipBegin(HDC hDC, CRenderClip& clip);
    static void UseOldClipEnd(HDC hDC, CRenderClip& clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//
#define COUI_DT_JUSTIFY (DT_CENTER|DT_RIGHT)//�ı����˶���

class UILIB_API CRenderEngine
{
public:
    static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
    static TImageInfo* LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0, CPaintManagerUI *pManager = NULL);
    static void FreeImage(const TImageInfo* bitmap);
    static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT& rc, const RECT& rcPaint, \
        const RECT& rcBmpPart, const RECT& rcCorners, bool alphaChannel, BYTE uFade = 255, 
        bool hole = false, bool xtiled = false, bool ytiled = false);
    static bool DrawImageString(HDC hDC, CPaintManagerUI* pManager, const RECT& rcItem, const RECT& rcPaint, 
        LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
    static void DrawColor(HDC hDC, const RECT& rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT& rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);
	static void DrawLine(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
    static void DrawRect(HDC hDC, const RECT& rc, int nSize, DWORD dwPenColor);
    static void DrawRoundRect(HDC hDC, const RECT& rc, int width, int height, int nSize, DWORD dwPenColor);
    static HRGN CreateHRgnFromBmp(HBITMAP hBitMap);//��ȡ32λλͼ��͸������
	static bool RenderRichWnd(HWND hWnd,HDC hDCdes,POINT *ptDes,SIZE *size,HDC hDCsrc,POINT *ptSrc,COLORREF color,BLENDFUNCTION *blen,DWORD flag);//�йֲܷ㴰�ڻ���

	// ���º����е���ɫ����alphaֵ��Ч
	static void DrawText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
        DWORD dwTextColor, int iFont, UINT uStyle);
	static void DrawThemeText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, \
		DWORD dwTextColor, int iFont, UINT uStyle, int iGlowSize);
    static void DrawHtmlText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, 
        DWORD dwTextColor, RECT* pLinks, CStdString* sLinks, int& nLinkRects, UINT uStyle);
    static HBITMAP GenerateBitmap(CPaintManagerUI* pManager, CControlUI* pControl, RECT rc);
};

} // namespace CoUI

#endif // __UIRENDER_H__
