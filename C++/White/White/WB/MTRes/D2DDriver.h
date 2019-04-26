#pragma once
#include <d2d1.h>
#include <d2d1helper.h>    
#include <comdef.h>
#include <wincodec.h>
#include "Define.h"
#include <dwrite.h>
#include "tstring.h"
using namespace Gdiplus;

// D2D 智能指针
_COM_SMARTPTR_TYPEDEF(ID2D1Factory, __uuidof(ID2D1Factory));
_COM_SMARTPTR_TYPEDEF(ID2D1HwndRenderTarget, __uuidof(ID2D1HwndRenderTarget));
_COM_SMARTPTR_TYPEDEF(ID2D1LinearGradientBrush, __uuidof(ID2D1LinearGradientBrush));

class CD2DDriver
{
public:
    CD2DDriver(HWND hwnd);
    ~CD2DDriver();
    HRESULT Initialize(BOOL fReverseColors = FALSE);

    // D2D Methods

    HRESULT CreateRenderTarget();
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources(BOOL fReverseColors);
    void DiscardDeviceResources();
    
    void RenderBackground(float clientWidth, float clientHeight);
	void RenderBackground(float clientLeft, float clientTop, float clientRight, float clientBottom);
	void RenderRubbishBackground(float clientLeft, float clientTop, float clientRight, float clientBottom);
	//
    //ID2D1HwndRenderTargetPtr GetRenderTarget();
	ID2D1DCRenderTarget* GetRenderTarget();
	//
	IDWriteFactory *GetWriteFactory();
	//
	IDWriteTextFormat *GetTextFormat();
	//
	IWICImagingFactory *GetImageFactory();
	//
	ID2D1SolidColorBrush * GetColorBrush();
    ID2D1LinearGradientBrushPtr get_GradBrush(unsigned int uBrushType);

    void BeginDraw();
    void EndDraw();
	void UpdateResWndSize();

    enum {GRB_Glossy, GRB_Blue, GRB_Orange, GRB_Red, GRB_Green};


	//GDI+相关
	Graphics* m_pGraphics;
	Graphics* m_pWndGraphics;
	Bitmap* m_bufBitmap;
	CachedBitmap* m_cacheBitmap;
	Rect clientRect;
	Gdiplus::Font* font;
	//内存绘制用变量
	HDC m_MemDC;
	HBITMAP m_MemBitMap;
	Graphics* GetGraphcis();
	Gdiplus::Font* GetFont() { return font; }

private:
    // Helper to create gradient resource
    HRESULT CreateGradient(ID2D1GradientStopCollection* pStops, 
        ID2D1LinearGradientBrush** pplgBrush, 
        D2D1::ColorF::Enum startColor, 
        float startOpacity, 
        float startPos, 
        D2D1::ColorF::Enum endColor, 
        float endOpacity, 
        float endPos);

    //窗口句柄
    HWND m_hWnd;

    // D2D 工厂
    ID2D1FactoryPtr m_spD2DFactory;

    // D2D 渲染窗口
    //ID2D1HwndRenderTargetPtr m_spRT;
	ID2D1DCRenderTarget* m_spRT;

    ID2D1LinearGradientBrushPtr m_spBGBrush;
	ID2D1LinearGradientBrushPtr m_spBGRubbishBrush;

	IDWriteFactory *m_pDWriteFactory;
	IDWriteTextFormat *m_pTextFormat;
	//
	ID2D1SolidColorBrush *m_pBlackBrush;

	//
	IWICImagingFactory *m_pImageFactory;

	//d2d1.dll导出函数
	HRESULT D2D1CreateFactoryEx( 
		__in D2D1_FACTORY_TYPE factoryType, 
		__out LPVOID* factory 
		);

	//dwrite.dll导出函数
	HRESULT DWriteCreateFactoryEx(
		__in   DWRITE_FACTORY_TYPE factoryType,
		__in   REFIID iid,
		__out  IUnknown **factory
		);

};
