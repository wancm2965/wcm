// SkinBase.h: interface for the CSkinBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKINBASE_H__D53DE9FD_C2BF_49DB_8B16_4F7874CA2579__INCLUDED_)
#define AFX_SKINBASE_H__D53DE9FD_C2BF_49DB_8B16_4F7874CA2579__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Image.h"

 
struct ObjPositionBase
{
	int		nAlignType;				//对齐方式
	int		nSpaceType;				//宽度或高度计算方式
	LONG	nLeft_Top;				//左值或顶值
	LONG	nRight_Bottom;			//右值或底值
	LONG	nWidth_Height;			//宽度或高度
	BOOL	bResizeWH;
	BOOL	bIgnore;				//忽略设置
	
	ObjPositionBase()
	{
		nAlignType = 0;
		nSpaceType = 3;
		nLeft_Top = 0;
		nRight_Bottom = 0;
		nWidth_Height = 0;
		bIgnore = FALSE;
		bResizeWH = FALSE;
	}
	
	virtual ~ObjPositionBase()
	{
	}
};

struct ObjPosition
{
	ObjPositionBase horzPos;
	ObjPositionBase vertPos;
};

struct SkinFont : public LOGFONT
{
	SkinFont()
	{
		::memset(this,0,sizeof(LOGFONT));
		_tcscpy_s(lfFaceName, 32, _T("Tahoma"));
		lfHeight = -12;
		lfWidth = 0;
		lfWeight = FW_NORMAL;
		lfOutPrecision = 3;
		lfClipPrecision = 2;
		lfQuality = 1;
		lfPitchAndFamily = 34;
	}

	void Copy(const SkinFont& fontSource)
	{
		_tcscpy_s(lfFaceName,32,fontSource.lfFaceName);
		lfHeight = fontSource.lfHeight;
		lfWidth = fontSource.lfWidth;
		lfWeight = fontSource.lfWeight;
		lfOutPrecision = fontSource.lfOutPrecision;
		lfClipPrecision = fontSource.lfClipPrecision;
		lfQuality = fontSource.lfQuality;
		lfPitchAndFamily = fontSource.lfPitchAndFamily;
	}
};

struct TextStyle
{
	int			nHAlign;//0-Left,1-Center,2-Right
	int			nVAlign;//0-Top,1-VCenter,2-Bottom
	COLORREF	clrText;
	int			nEffect;//0-Normal,1-Shadow
	COLORREF	clrShadow;
	int			nShadowHSpace;
	int			nShadowVSpace;
	int			nLeftOffset;
	int			nRightOffset;
	int			nTopOffset;
	int			nBottomOffset;
	int			nEndEllipsis; //是否带 DT_END_ELLIPSIS 分格
	int			nSingleLine; //是否带 DT_SINGLELINE 分格

	TextStyle()
	{
		nHAlign = 1;
		nVAlign = 1;
		clrText = 0;
		clrShadow = GetSysColor(COLOR_BTNSHADOW);
		nEffect = 0;
		nShadowHSpace = 1;
		nShadowVSpace = 1;
		nLeftOffset = 0;
		nRightOffset = 0;
		nTopOffset = 0;
		nBottomOffset = 0;
		nEndEllipsis=0;
		nSingleLine=0;
	}
};

struct SkinImageBase
{
	CImage* pEnBitmap;
	//图片的对象名称ID
	TCHAR	szImageObjectName[MAX_PATH];
	CRect	rect;
	int		nTopHeight;
	int		nBottomHeight;
	int		nLeftWidth;
	int		nRightWidth;
	BOOL	bLeftEdgeStretch;
	BOOL	bRightEdgeStretch;
	BOOL	bTopEdgeStretch;
	BOOL    bBottomEdgeStretch;
	BOOL	bClientAreaStretch;
	//transparent
	COLORREF clrTrans;
	BOOL	bTrans;

	SkinImageBase()
	{
		pEnBitmap = NULL;
		_tcscpy_s(szImageObjectName, 260, _T(""));
		rect.left = rect.top = 0;
		rect.right = 20;
		rect.bottom = 20;
		nTopHeight = nBottomHeight = 0;
		nLeftWidth = nRightWidth = 0;
	
		StretchAll();
		
		clrTrans = RGB(255,0,255);
		bTrans = TRUE;
	}

	void Copy(const SkinImageBase& d)
	{
		pEnBitmap = d.pEnBitmap;
		_tcscpy_s(szImageObjectName,260,d.szImageObjectName);
		rect = d.rect;
		nTopHeight = d.nTopHeight;
		nBottomHeight = d.nBottomHeight;
		nLeftWidth = d.nLeftWidth;
		nRightWidth = d.nRightWidth;
		bLeftEdgeStretch = d.bLeftEdgeStretch;
		bRightEdgeStretch = d.bRightEdgeStretch;
		bTopEdgeStretch = d.bTopEdgeStretch;
		bBottomEdgeStretch = d.bBottomEdgeStretch;
		bClientAreaStretch = d.bClientAreaStretch;
	}

	void StretchAll()
	{
		bLeftEdgeStretch = TRUE;
		bRightEdgeStretch = TRUE;
		bTopEdgeStretch = TRUE;
		bBottomEdgeStretch = TRUE;
		bClientAreaStretch = TRUE;
	}

	void TileAll()
	{
		bLeftEdgeStretch = FALSE;
		bRightEdgeStretch = FALSE;
		bTopEdgeStretch = FALSE;
		bBottomEdgeStretch = FALSE;
		bClientAreaStretch = FALSE;
	}

	virtual ~SkinImageBase()
	{
		if (pEnBitmap != NULL)
		{
			try{delete pEnBitmap;}catch(...){}
	//		delete pEnBitmap;
			pEnBitmap = NULL;
		}
	}
};

enum
{
	SBOS_95,
	SBOS_98,
	SBOS_ME,
	SBOS_NT4,
	SBOS_2K,
	SBOS_XP,
	SBOS_XPP, // after xp
};

// os
typedef BOOL (WINAPI *LPSetLayeredWindowAttributes)
(
  HWND hwnd,           // handle to the layered window
  COLORREF crKey,      // specifies the color key
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // action
);
 
// for transparency
typedef WINGDIAPI BOOL (WINAPI *PFNTRANSPARENTBLT)(HDC,int,int,int,int,HDC,int,int,int,int,UINT);

// for gradient fill
typedef WINGDIAPI BOOL (WINAPI *PFNGRADIENTFILL)(HDC,PTRIVERTEX,ULONG,PVOID,ULONG,ULONG);

typedef BOOL (WINAPI *lpfnUpdateLayeredWindow)(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, 
											  POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);

class DIRECTUI_API CSkinBase
{
public:
	CSkinBase();
	virtual ~CSkinBase();

	// helper methods
//	static void  DrawMarlettText(CDC *pDC,const CString& strItem,COLORREF clrText,CRect rect,UINT uHorzAlign);
	static HBITMAP CopyBitmap (HBITMAP hBitmapSrc,int x,int y,int width,int height);
	static UINT GetAlign(int nHAlign,int nVAlign,BOOL bEllipsis = TRUE);
	static void	DrawText(HDC hDC,const CString& strText,const CRect& rect,
				  const SkinFont* pFont,const TextStyle* pTextStyle,CRect *pRect = NULL,BOOL bCalcDraw = FALSE);
	static BOOL	 SupportsLayeredWindow();
	static BOOL	 SupportUpdateLayeredWindow();
	static void  CalcTextRect(HDC hDC, CString sText, CRect& rText, UINT nAlign = DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	static void	 AlignRect(CRect& rect, const CRect& rContainer, UINT nAlignment);
	static BOOL  DrawTileBmp(CImage * pbmpSc,HDC hDC,CRect r,CRect sr = CRect(0,0,0,0),COLORREF colTrans = -1, BOOL bTrans = FALSE);
	inline static BOOL  DrawTileBitmap(HDC hDC,HDC hSrcDC,CRect rcDest,CRect sr,COLORREF colTrans = -1,BOOL bTrans = FALSE);
	static BOOL  DrawSkinImage(HDC hDC,LPRECT rect,const SkinImageBase* pImageBase);
	static BOOL  DrawSkinBitmap(HDC hDC,LPRECT rect,const SkinImageBase* pImageBase);
	static void  DrawGradientBkgnd(HDC hDC, LPRECT pRect, LPRECT pClipp,COLORREF crFrom,COLORREF crTo);
	static int  GetOS();
	static COLORREF BlendColors(COLORREF crA, COLORREF crB, float fAmountA = 0.5f);

	// special blt functions which handle transparency
	static BOOL BitBlt(HDC hDCDest, 
							int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
							HDC hDCSrc, int nXOriginSrc, int nYOriginSrc, 
							UINT uROP = SRCCOPY,     
							COLORREF crTransparent = -1);

	static BOOL StretchBlt(HDC hDCDest, 
							int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
							HDC hDCSrc, int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,     
							UINT uROP = SRCCOPY,     
							COLORREF crTransparent = -1);

	// gradient fill from top->bottom or left->right
	static BOOL GradientFill(HDC hDCDest, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);
	static BOOL SetLayeredWindowAttributes( HWND hwnd,COLORREF crKey,BYTE bAlpha,DWORD dwFlags);
	static BOOL UpdateLayeredWindow(HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, 
									POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);

private:
	static int s_nOSVer;
	static BOOL s_bSupportsFastTransparentBlt;
	static PFNTRANSPARENTBLT s_pfnFastTransparentBlt;
	static BOOL s_bSupportsFastGradientFill;
	static PFNGRADIENTFILL s_pfnFastGradientFill;
	static BOOL s_bThemingEnabled;
	static LPSetLayeredWindowAttributes s_pfnSetLayeredWindowAttributes;
	static BOOL s_bSupportsLayeredWindow;

	static lpfnUpdateLayeredWindow s_pfnUpdateLayeredWindow;
	static BOOL s_bSupportsUpdateLayeredWindow;

	static BOOL SupportsFastTransparentBlt();
	static BOOL TransparentBltFast(HDC hDCDest, 
									int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
									HDC hDCSrc, 
									int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,     
									UINT crTransparent);

	static BOOL SupportsFastGradientFill();
	static BOOL GradientFillFast(HDC hDCDest, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	// fallbacks for win9x
	static BOOL TransparentBltSlow(HDC hDCDest, 
									int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
									HDC hDCSrc, 
									int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,     
									UINT crTransparent);
	static BOOL GradientFillSlow(HDC hDCDest, LPRECT lpRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

	static BOOL TransparentBlt(HDC hDCDest, 
								int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest,
								HDC hDCSrc, 
								int nXOriginSrc, int nYOriginSrc, int nWidthSrc, int nHeightSrc,     
								UINT crTransparent);
};

#endif // !defined(AFX_SKINBASE_H__D53DE9FD_C2BF_49DB_8B16_4F7874CA2579__INCLUDED_)
