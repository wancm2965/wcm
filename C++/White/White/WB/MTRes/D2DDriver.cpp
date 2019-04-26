#include "D2DDriver.h"
#include "Define.h"
//#include <Shlobj.h>//for image facotory
CD2DDriver::CD2DDriver(HWND hwnd)
	: m_hWnd(hwnd)
	, m_pDWriteFactory(NULL)
	, m_spRT(NULL)
	, m_spBGBrush(NULL)
	, m_spD2DFactory(NULL)
	, m_pTextFormat(NULL)
	, m_pBlackBrush(NULL)
	, m_pImageFactory(NULL)
	, m_pGraphics(NULL)
	, m_bufBitmap(NULL)
	, m_cacheBitmap(NULL)
	, m_pWndGraphics(NULL)
	, font(NULL)
	, m_spBGRubbishBrush(NULL)
{
}

CD2DDriver::~CD2DDriver()
{
    DiscardDeviceResources();
}

HRESULT CD2DDriver::Initialize(BOOL fReverseColors)
{
	if (g_systemVersion >= 6)
	{
		HRESULT hr = CreateDeviceIndependentResources();

		if( SUCCEEDED(hr) )
		{
			hr = CreateDeviceResources(fReverseColors);
		}

		return hr;
	}
	else
	{
		RECT rect;
		GetClientRect(m_hWnd, &rect);
		clientRect = Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
		m_bufBitmap = new Bitmap(clientRect.Width, clientRect.Height);
		m_pWndGraphics = new Graphics(m_hWnd, TRUE);
		m_pGraphics = Graphics::FromImage(m_bufBitmap);
		m_cacheBitmap = new CachedBitmap(m_bufBitmap, m_pGraphics);
		m_pWndGraphics->SetSmoothingMode(SmoothingModeHighQuality);
		m_pWndGraphics->SetPixelOffsetMode(PixelOffsetModeHighSpeed);
		m_pGraphics->SetSmoothingMode(SmoothingModeHighQuality);
		m_pGraphics->SetPixelOffsetMode(PixelOffsetModeHighSpeed);
		font = new Gdiplus::Font(DRAWTEXT("宋体"), 12);
		// 设置字体渲染模式
		m_pGraphics->SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
		if (m_pWndGraphics && m_pGraphics)
		{
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
}

HRESULT CD2DDriver::CreateDeviceIndependentResources()
{
    // Create D2D factory
    HRESULT hr = D2D1CreateFactoryEx(D2D1_FACTORY_TYPE_SINGLE_THREADED, (LPVOID*)&m_spD2DFactory);
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactoryEx(DWRITE_FACTORY_TYPE_SHARED,__uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}
	if (SUCCEEDED(hr))
	{
		//create DWrite text format object
		hr = m_pDWriteFactory->CreateTextFormat(DRAWTEXT("宋体"),
			NULL,
			DWRITE_FONT_WEIGHT_THIN,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			14,
			DRAWTEXT(""), //locale
			&m_pTextFormat
			);
	}
	if (SUCCEEDED(hr))
	{
		//设置文本对齐
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		IDWriteInlineObject *pEllipsis = NULL;
		hr = m_pDWriteFactory->CreateEllipsisTrimmingSign(m_pTextFormat, &pEllipsis);
		if (SUCCEEDED(hr))
		{
			const DWRITE_TRIMMING sc_trimming =
			{
				DWRITE_TRIMMING_GRANULARITY_CHARACTER,
				0,
				0
			};

			// Set the trimming back on the trimming format object.
			hr = m_pTextFormat->SetTrimming(&sc_trimming, pEllipsis);

			pEllipsis->Release();
		}
	}
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pImageFactory)
			);
	}
	return hr;
}

HRESULT CD2DDriver::CreateDeviceResources(BOOL fReverseColors)
{
	HRESULT hr = S_OK;

	if(!m_spRT) 
	{
		hr = CreateRenderTarget();

		if(SUCCEEDED(hr))
		{
            ID2D1GradientStopCollection *pGradientStops = NULL;
            if (fReverseColors)
            {
                hr = CreateGradient(pGradientStops, 
                    &m_spBGBrush, 
                    D2D1::ColorF::LightSlateGray, 
                    0.5f, 
                    0.6f, 
                    D2D1::ColorF::White, 
                    1.0f, 
                    0.3f);
            }
            else
            {
                hr = CreateGradient(pGradientStops, 
                    &m_spBGBrush, 
                    D2D1::ColorF::White, 
                    1.0f, 
                    0.6f, 
                    D2D1::ColorF::LightSlateGray, 
                    1.0f, 
                    0.0f);
            }
        }
		if (SUCCEEDED(hr))   //资源垃圾箱背景
		{
			ID2D1GradientStopCollection *pGradientStops = NULL;
			hr = CreateGradient(pGradientStops, 
				&m_spBGRubbishBrush, 
				D2D1::ColorF::Red, 
				0.7f, 
				0.6f, 
				D2D1::ColorF::White, 
				1.0f, 
				0.0f);

		}
		if (SUCCEEDED(hr))
		{
				hr = m_spRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pBlackBrush);
		}
    }


    return hr;
}
void CD2DDriver::RenderBackground(float clientWidth, float clientHeight)
{
    if (g_systemVersion >= 6)
    {
		m_spBGBrush->SetStartPoint(
			D2D1::Point2F(
			clientWidth/2, 
			0.0f)
			);
		m_spBGBrush->SetEndPoint(
			D2D1::Point2F(
			clientWidth/2, 
			clientHeight)
			);

		//创建背景矩形,并填充
		D2D1_RECT_F background = D2D1::RectF(
			0,
			0,
			clientWidth,
			clientHeight
			);

		m_spRT->FillRectangle(
			&background,
			m_spBGBrush
			);
    }
	else
	{
		Rect background(0, 0, clientWidth, clientHeight);
		LinearGradientBrush hbrush(background,Color::LightSlateGray, Color::White,  LinearGradientModeVertical);
		m_pGraphics->FillRectangle(&hbrush, background);
	}
    return;
}

void CD2DDriver::RenderBackground( float clientLeft, float clientTop, float clientRight, float clientBottom )
{
	float clientWidth = clientRight - clientLeft;
	float clientHeight = clientBottom - clientTop;

	if (g_systemVersion >= 6)
	{
		m_spBGBrush->SetStartPoint(
			D2D1::Point2F(
			clientWidth/2, 
			0.0f)
			);

		m_spBGBrush->SetEndPoint(
			D2D1::Point2F(
			clientWidth/2, 
			clientHeight)
			);

		//创建背景矩形,并填充
		D2D1_RECT_F background = D2D1::RectF(
			clientLeft,
			clientTop,
			clientRight,
			clientBottom
			);

		m_spRT->FillRectangle(
			&background,
			m_spBGBrush
			);
	}
	else
	{
		Rect background(clientLeft, clientTop, clientWidth, clientHeight);
		LinearGradientBrush hbrush(background, Color::LightSlateGray, Color::White,  LinearGradientModeVertical);
		m_pGraphics->FillRectangle(&hbrush, background);
	}

	return;
}

void CD2DDriver::RenderRubbishBackground( float clientLeft, float clientTop, float clientRight, float clientBottom)
{
	if (g_systemVersion >= 6)
	{
		m_spBGRubbishBrush->SetStartPoint(
			D2D1::Point2F(
			clientRight/2, 
			clientTop)
			);
		m_spBGRubbishBrush->SetEndPoint(
			D2D1::Point2F(
			clientRight/2, 
			clientBottom)
			);

		//创建背景矩形,并填充
		D2D1_RECT_F background = D2D1::RectF(
			clientLeft,
			clientTop,
			clientRight,
			clientBottom
			);

		m_spRT->FillRectangle(
			&background,
			m_spBGRubbishBrush
			);
	}
	else
	{
		Rect background(clientLeft, clientTop, clientRight, clientBottom);
		LinearGradientBrush hbrush(background,Color::White, Color::Red,  LinearGradientModeVertical);
		m_pGraphics->FillRectangle(&hbrush, background);
	}
	return;
}

void CD2DDriver::DiscardDeviceResources()
{
	if (g_systemVersion >= 6)
	{
		if (m_spRT)
		{
			m_spRT->Release();
		}
		if (m_spBGBrush)
		{
			m_spBGBrush.Release();
		}
		
		if (m_spBGRubbishBrush)
		{
			m_spBGRubbishBrush.Release();
		}
		if(m_pTextFormat)
		{
			m_pTextFormat->Release();
		}
		if (m_pDWriteFactory)
		{
			m_pDWriteFactory->Release();
		}
		if(m_pBlackBrush)
		{
			m_pBlackBrush->Release();
		}
	}
	else
	{
		if (m_pGraphics)
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
		}

		if (m_pWndGraphics)
		{
			delete m_pWndGraphics;
			m_pWndGraphics = NULL;
		}

		if (m_bufBitmap)
		{
			delete m_bufBitmap;
			m_bufBitmap = NULL;
		}

		if (m_cacheBitmap)
		{
			delete m_cacheBitmap;
			m_cacheBitmap = NULL;
		}
	}
}    

HRESULT CD2DDriver::CreateRenderTarget()
{
    HRESULT hr = S_OK;
	if (g_systemVersion >= 6)
	{
		// Get the size of our target area
		RECT rc;

		GetClientRect(m_hWnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left,
			rc.bottom - rc.top);

		//// Create a Direct2D render target
		//D2D1_RENDER_TARGET_PROPERTIES rtp = D2D1::RenderTargetProperties();
		//hr = m_spD2DFactory->CreateHwndRenderTarget(rtp,
		//    D2D1::HwndRenderTargetProperties(m_hWnd, size), &m_spRT);
		// Create a DC render target.
		D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
			DXGI_FORMAT_B8G8R8A8_UNORM,
			D2D1_ALPHA_MODE_IGNORE),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
			);

		hr = m_spD2DFactory->CreateDCRenderTarget(&props, &m_spRT);

		HDC dc = ::GetDC(m_hWnd);
		RECT rect;
		::GetClientRect(m_hWnd,&rect);
		m_spRT->BindDC(dc,&rect);
		if (SUCCEEDED(hr))
		{
			// Create a black brush.
			hr = m_spRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
				);
		}
	}

    return hr;
}

void CD2DDriver::BeginDraw()
{
	if (g_systemVersion >= 6)
	{
		m_spRT->BeginDraw();
		m_spRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	}
	else
	{
		HDC hdc = GetDC(m_hWnd);
		RECT rt;
		GetClientRect(m_hWnd,&rt);
		m_MemDC = ::CreateCompatibleDC(hdc);
		m_MemBitMap = ::CreateCompatibleBitmap(hdc,rt.right,rt.bottom);
		m_MemBitMap = (HBITMAP)::SelectObject(m_MemDC,m_MemBitMap);
		ReleaseDC(m_hWnd,hdc);
		if(m_pGraphics != NULL)
		{
			delete m_pGraphics;
			m_pGraphics = NULL;
		}
		m_pGraphics = new Graphics(m_MemDC);
		m_pGraphics->Clear(Color::LightSlateGray);
		return;
	}
}


void CD2DDriver::EndDraw()
{
	if (g_systemVersion >= 6)
	{
		HRESULT hr = m_spRT->EndDraw();
		if(hr == D2DERR_RECREATE_TARGET)
		{
			DiscardDeviceResources();
		}
	}
	else
	{
		if (m_pGraphics ==NULL || m_pWndGraphics == NULL || m_cacheBitmap == NULL || m_bufBitmap == NULL)
		{
			return;
		}
		RECT rt;
		::GetClientRect(m_hWnd,&rt);
		::BitBlt(GetDC(m_hWnd),0,0,rt.right,rt.bottom,m_MemDC,0,0,SRCCOPY);
		m_MemBitMap = (HBITMAP)SelectObject(m_MemDC,m_MemBitMap);
		DeleteObject(m_MemBitMap);
		DeleteDC(m_MemDC);
		//m_pGraphics->DrawCachedBitmap(m_cacheBitmap, 0, 0);
		//m_pWndGraphics->DrawImage(m_bufBitmap, clientRect, 0, 0, m_bufBitmap->GetWidth(), m_bufBitmap->GetHeight(), UnitPixel);
		return;
	}
}


void CD2DDriver::UpdateResWndSize()
{
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	clientRect = Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	if (NULL != m_pWndGraphics)
	{
		delete m_pWndGraphics;
		m_pWndGraphics = Graphics::FromHDC(GetDC(m_hWnd));
	}
	if (NULL != m_bufBitmap)
	{
		delete m_bufBitmap;
		m_bufBitmap = new Bitmap(clientRect.Width, clientRect.Height);
	}
	if (NULL != m_pGraphics)
	{
		delete m_pGraphics;
		m_pGraphics = Graphics::FromImage(m_bufBitmap);
	}
	if (NULL != m_cacheBitmap)
	{
		delete m_cacheBitmap;
		m_cacheBitmap = new CachedBitmap(m_bufBitmap, m_pGraphics);
	}
}

HRESULT CD2DDriver::CreateGradient(ID2D1GradientStopCollection* pStops, 
    ID2D1LinearGradientBrush** pplgBrush, 
    D2D1::ColorF::Enum startColor, 
    float startOpacity, 
    float startPos, 
    D2D1::ColorF::Enum endColor, 
    float endOpacity, 
    float endPos)
{
    
    HRESULT hr = S_OK;

    D2D1_GRADIENT_STOP stops[2];
    stops[0].color = D2D1::ColorF(startColor, startOpacity);
    stops[0].position = startPos;
    stops[1].color = D2D1::ColorF(endColor, endOpacity);
    stops[1].position = endPos;

    hr = m_spRT->CreateGradientStopCollection(
        stops,
        2,
        &pStops
        );

    if(SUCCEEDED(hr))
    {
        hr = m_spRT->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(0.0f, 0.0f)),
            D2D1::BrushProperties(),
            pStops,
            pplgBrush);
    }
    return hr;
}

//ID2D1HwndRenderTargetPtr* CD2DDriver::GetRenderTarget() 
ID2D1DCRenderTarget* CD2DDriver::GetRenderTarget() 
{
    return m_spRT;
}

IDWriteFactory * CD2DDriver::GetWriteFactory()
{
	return m_pDWriteFactory;
}

IDWriteTextFormat * CD2DDriver::GetTextFormat()
{
	return m_pTextFormat;
}

ID2D1SolidColorBrush * CD2DDriver::GetColorBrush()
{
	return m_pBlackBrush;
}

IWICImagingFactory * CD2DDriver::GetImageFactory()
{
	return m_pImageFactory;
}

HRESULT CD2DDriver::D2D1CreateFactoryEx(
	__in D2D1_FACTORY_TYPE factoryType,
	__out LPVOID* factory 
	)
{

	static HMODULE hUserDll = LoadLibrary(TEXT("d2d1.dll"));

	if (NULL == hUserDll)
	{
		return S_FALSE;
	}

	typedef	BOOL (__stdcall *D2D1FUN)(D2D1_FACTORY_TYPE, REFIID, const D2D1_FACTORY_OPTIONS, LPVOID*);
	static D2D1FUN D2D1CreateFactory = (D2D1FUN)GetProcAddress(hUserDll, "D2D1CreateFactory");
	if (D2D1CreateFactory == NULL)
	{
		return FALSE;
	}

	D2D1_DEBUG_LEVEL debug_level = D2D1_DEBUG_LEVEL_NONE;
	D2D1_FACTORY_OPTIONS option;
	option.debugLevel = debug_level;

	return (*D2D1CreateFactory)(factoryType, IID_ID2D1Factory, option, factory);

}

HRESULT CD2DDriver::DWriteCreateFactoryEx( 
	__in DWRITE_FACTORY_TYPE factoryType,
	__in REFIID iid,
	__out IUnknown **factory
	)
{
	static HMODULE hUserDll = LoadLibrary(TEXT("DWrite.dll"));

	if (NULL == hUserDll)
	{
		return S_FALSE;
	}

	typedef	BOOL (__stdcall *DWRITEFUN)(DWRITE_FACTORY_TYPE, REFIID, IUnknown**);
	static DWRITEFUN DWriteCreateFactory = (DWRITEFUN)GetProcAddress(hUserDll, "DWriteCreateFactory");

	if (DWriteCreateFactory == NULL)
	{
		return FALSE;
	}

	return (*DWriteCreateFactory)(factoryType, iid, factory);
}

Graphics* CD2DDriver::GetGraphcis()
{
	return m_pGraphics;
}

