
#include "stdafx.h"
#include "Thumbnail.h"
#include <Shlwapi.h>
#include <string>
#include <Shellapi.h>
#include "ResGallery.h"
#include <GdiPlus.h>
#include <commoncontrols.h>
#include <sys/stat.h> 
#include <share.h>
#include <fcntl.h>
#include <io.h>
#include "Shlwapi.h"
#pragma  comment(lib,"Shlwapi.lib") 
using namespace Gdiplus;
#define UWM_CHANGE_RES          (WM_USER+217)
#define UWM_DRAGOUT_RESOUCE		(WM_USER + 3333)
BOOL g_bShowDlg = TRUE;
CWBThumbnail::CWBThumbnail(HWND hWnd,CD2DDriver* d2dDriver, 
             float fpXPos, float fpYPos, float fpHeight, float fpXWndPos) :
	m_hAttachWnd(hWnd),
    m_d2dDriver(d2dDriver),
    m_fpHeight(fpHeight),
    m_fpWidth(fpHeight),//先改成这样
    m_pBitmap(NULL),
    m_fpYPos(fpYPos),
	m_fpYPosConst(m_fpYPos),
	m_fpXWndPos(fpXWndPos),
	m_fTotalAngle( 0.0f ),
	m_fTotalScaleX( 1.0f ),
	m_fTotalScaleY( 1.0f ),
	m_pBlackBrush(NULL),
	m_pDWriteFactory(NULL),
	m_pBindContext(NULL),
	m_pMalloc(NULL),
	m_psfDesktop(NULL),
	m_pGdiPlusBmp(NULL),
	m_bSelect(FALSE),
	m_IconImage(NULL),
	m_hDialog(NULL),
	m_hD2DBmp(NULL),
	m_bisVideoFile(FALSE),
	m_bHorDirection(FALSE)
{
    m_bounds.left = LONG_MIN;
    m_bounds.right = LONG_MAX;
    m_bounds.bottom = LONG_MAX;
    m_bounds.top = LONG_MIN;
	
	if (g_systemVersion >= 6)
	{
		m_spRT = m_d2dDriver->GetRenderTarget();
		//create a black brush
		if (m_spRT)
		{
			HRESULT hr = m_spRT->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Black),
				&m_pBlackBrush
				);
			hr = CreateBindCtx(0, &m_pBindContext);
		}
		m_matrix = D2D1::Matrix3x2F::Identity();                //创建单位矩阵
		m_MatrixTranslate = D2D1::Matrix3x2F::Identity();
		m_MatrixFinal = D2D1::Matrix3x2F::Identity();
	}
	else
	{
		m_hDC = ::GetDC(hWnd);
		InitializeXForm(&m_xform);
		InitializeXForm(&m_xformTranslate);
		InitializeXForm(&m_xformFinal);
	}

	Translate(fpXPos, fpYPos, true);
}

CWBThumbnail::~CWBThumbnail()
{
	DrapIcon(TRUE);
	if (m_pGdiPlusBmp != NULL)
	{
		delete m_pGdiPlusBmp;
	}
    if (m_pBitmap != NULL)
    {
        m_pBitmap->Release();
        m_pBitmap = NULL;
    }
	if (m_pBlackBrush != NULL)
	{
		m_pBlackBrush->Release();
		m_pBlackBrush = NULL;
	}
	if (m_pBindContext)
	{
		m_pBindContext->Release();
		m_pBindContext = NULL;
	}

	if (m_psfDesktop)
	{
		m_psfDesktop->Release();
		m_psfDesktop = NULL;
	}

	if (m_pMalloc)
	{
		m_pMalloc->Release();
		m_pMalloc = NULL;
	}
	if (m_hDialog != NULL)
	{
		FreeLibrary(m_hDialog);
		m_hDialog = NULL;
	}
}

void CWBThumbnail::OnGestureCombineHandle( const GestureCombinePar &gesPar )
{
	static bool bhas= false;
	POINT cPos;
	::GetCursorPos(&cPos);           //动态的得到鼠标在屏幕上的位置
	RECT rt;
	::GetClientRect(m_hAttachWnd,&rt);
	::ScreenToClient(m_hAttachWnd,&cPos);
	CWBResGallery* pResGallery = NULL;
	pResGallery = reinterpret_cast<CWBResGallery*>(GetWindowLongPtr(GetHWnd(),0));
	if (!PtInRect(&rt,cPos))
	{
		if (!bhas)
		{
			bhas = true;
			ResetYOffSet(); 
			InvalidateRect(m_hAttachWnd,NULL,FALSE);
			pResGallery->m_bDrapFlag = TRUE;    
			pResGallery->SetThumbnail(this);
			pResGallery->SetIsDrawRubbishIcon(FALSE);
			CDrapEffect::updateWindow(m_IconImage,m_FirstPoint,m_hAttachWnd);
			bhas = false;	
		}
	}
	else
	{
		if (m_bisVideoFile )   //可以删除的文件
		{
			if (abs(gesPar.xTransCumulative) < PREVENT_DEL_RANGE)
			{
				Translate(0, gesPar.ytrans);
				if (gesPar.yTransCumulative>SHOW_DELICON_RANGE && !m_bHorDirection)
				{
					pResGallery->SetIsDrawRubbishIcon(TRUE);
				}
				else
				{
					pResGallery->SetIsDrawRubbishIcon(FALSE);
				}
			}
			else
			{
				m_bHorDirection = TRUE;
				pResGallery->SetIsDrawRubbishIcon(FALSE);
			}
			if (gesPar.yTransCumulative>0)
			{
				bhas = true;
			}
			else
			{
				bhas = false;
			}
		}
		else
		{
			Translate(0, gesPar.ytrans);
		}
	}
}

bool CWBThumbnail::BInRegion( const POINT &pt ) const
{
	return (InRegion(pt.x, pt.y))?(true):(false);
}

BOOL CWBThumbnail::InRegion(const float &x, const float &y)const
{
	if (g_systemVersion >= 6)
	{
		D2D1_POINT_2F inPoint;
		inPoint.x = x;
		inPoint.y = y;
		D2D1::Matrix3x2F MatrixFinal = D2D1::Matrix3x2F::Identity();
		MatrixFinal.SetProduct( MatrixFinal , m_MatrixTranslate );
		D2D1InvertMatrixEx(&MatrixFinal);
		D2D1_POINT_2F transPoint = MatrixFinal.TransformPoint(inPoint);
		return (
			transPoint.y > 0 &&
			transPoint.y < ICON_SIZE &&//m_fpHeight
			transPoint.x >= 0 &&
			transPoint.x < ICON_SIZE//m_fpWidth
			);
	}
	else
	{
		return x>=m_xformTranslate.eDx && y>=m_xformTranslate.eDy && x<= ICON_SIZE + m_xformTranslate.eDx && y<= ICON_SIZE + m_xformTranslate.eDy;
	}
}

void CWBThumbnail::Render()
{
	m_xformTranslate;
	GetItemBmp(GetFileURI());
	if (g_systemVersion >= 6)
	{
		D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Identity();

		D2D1::Matrix3x2F matrix2 = D2D1::Matrix3x2F::Translation( -m_fpXWndPos , 0 );
		matrix.SetProduct( matrix, matrix2 );
		matrix.SetProduct( matrix , m_MatrixTranslate );
		m_spRT->SetTransform( matrix );
		//绘制图片和文字
		//D2D1_RECT_F rct();
		if (m_hD2DBmp)
		{
			float startX;//理想图标宽度为96，为使图标居中显示加此变量
			float startHeight;
			if (m_fpWidth < ICON_SIZE)
			{
				startX = ((float)((ICON_SIZE-m_fpWidth)))/2;
			}
			else
			{
				startX = 0;
			}
			m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Pink));

			if (!m_isDir)
			{
				m_spRT->DrawRectangle(
					D2D1::RectF(0.0, 0.0, ICON_SIZE, ICON_SIZE),
					m_pBlackBrush
					);
			}
			if (m_fpHeight < ICON_SIZE)
			{
				startHeight = ((float)(ICON_SIZE - m_fpHeight))/2 ;
			}
			else
			{
				startHeight = 0.0;
			}
			//m_spRT->DrawBitmap(m_pBitmap,  D2D1::RectF(startX, startHeight, startX+m_fpWidth, startHeight+m_fpHeight)
			//	, 1.0
			//	, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
			//	, NULL);//指定大小

			s_pID2D1BmpAtlas->DrawBitmap(m_spRT,m_hD2DBmp,
				D2D1::RectF(startX, startHeight, startX+m_fpWidth, startHeight+m_fpHeight),
				1.0,D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			m_pBlackBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			m_ptIcon.x= startX+matrix._31;
			m_ptIcon.y = startHeight+matrix._32;
		}
		if (m_d2dDriver == NULL)
		{
			return;
		}
		ID2D1SolidColorBrush *pColorBrush = m_d2dDriver->GetColorBrush();
		IDWriteTextFormat *pTextFormat = m_d2dDriver->GetTextFormat();


		if (pColorBrush && pTextFormat)
		{
			int yOffSet = /*(int)*/5;
			m_spRT->DrawText(ToDRAWStr(m_strThumbName.c_str()).c_str(),
				ToDRAWStr(m_strThumbName.c_str()).length(),
				pTextFormat,
				D2D1::RectF(
				-10,
				ICON_SIZE + yOffSet,    //理想图标高度宽度都为ICON_SIZE
				ICON_SIZE+10,
				ICON_SIZE+ yOffSet + 14),
				pColorBrush
				);
		}
		m_spRT->SetTransform(D2D1::Matrix3x2F::Identity());
		
	}
	else
	{
		XFORM xform;
		XFORM xform2;
		InitializeXForm(&xform);
		xform2 = TranslateXForm(-m_fpXWndPos, 0);
		xform = SetProductXForm(xform, xform2);
		xform = SetProductXForm(xform, m_xformTranslate);
		if (m_d2dDriver == NULL || m_pGdiPlusBmp == NULL)
		{
			return;
		}
		float startX;//理想图标宽度为ICON_SIZE，为使图标居中显示加此变量
		float startY;//理想图标宽度为ICON_SIZE, 为使图标居中显示加此变量
		if (m_pGdiPlusBmp->GetWidth() < ICON_SIZE)
		{
			startX = ((float)(ICON_SIZE-m_pGdiPlusBmp->GetWidth()))/2;
			
		}
		else
		{
			startX = 0;
			
		}
		if (m_pGdiPlusBmp->GetHeight() < ICON_SIZE)
		{
			startY = ((float)(ICON_SIZE-m_pGdiPlusBmp->GetHeight()))/2;
		}
		else
		{
			startY = 0;
		}
		if (m_d2dDriver->GetGraphcis())
		{
			Rect destion(m_xformTranslate.eDx, m_xformTranslate.eDy, m_fpWidth, m_fpHeight);
			Pen pen(Color::Pink);
			if (!m_isDir)
			{
				m_d2dDriver->GetGraphcis()->DrawRectangle(&pen,destion.X,destion.Y,ICON_SIZE,ICON_SIZE);
			}
			destion.X+=startX;
			destion.Y+=startY;
			m_d2dDriver->GetGraphcis()->DrawImage(m_pGdiPlusBmp,destion,0,0,m_pGdiPlusBmp->GetWidth(),m_pGdiPlusBmp->GetHeight(),UnitPixel);
			m_ptIcon.x = destion.X;
			m_ptIcon.y = destion.Y;
			/*if(m_bSelect)
			{
			m_d2dDriver->GetGraphcis()->DrawRectangle(&Gdiplus::Pen(Gdiplus::Color(0xFF,0,0),5.0),destion);
			}*/
			StringFormat format;
			format.SetAlignment( StringAlignmentCenter);
			SolidBrush textBrush(Color::Black);

			DRAWSTR strThumbName = ToDRAWStr(m_strThumbName.c_str());
			m_d2dDriver->GetGraphcis()->DrawString(strThumbName.c_str(),strThumbName.length(), m_d2dDriver->GetFont(),RectF(m_xformTranslate.eDx, m_xformTranslate.eDy+ICON_SIZE, ICON_SIZE, 20),&format, &textBrush);
		}
	}
}

void CWBThumbnail::Translate(float x, float y, bool bInner/* = false*/)
{
  //  BOOL fPastBounds = IsPastBounds();

	if (g_systemVersion >= 6)
	{
		D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(x, y);
		m_matrix.SetProduct(trans, m_matrix);
		//目前
		m_MatrixTranslate = m_matrix;
	}
	else
	{
		XFORM trans = TranslateXForm(x, y);
		m_xform = SetProductXForm(trans, m_xform);
		//m_xformTranslate = m_xform;
		m_xformTranslate.eDx += x;
		m_xformTranslate.eDy += y;
	}

	//平移之前没有越界 到 平移之后越界，则发送消息
	//if (!fPastBounds && IsPastBounds())
	//{
	//	//UINT msg = (m_bIsInertiaActive) ? PS_RESOURCE_BOUNDARY_INERTIA : PS_RESOURCE_BOUNDARY;
	//	//SendMessage(GetHWnd(), PS_RESOURCE_BOUNDARY, (WPARAM)0, (LPARAM)(CWBThumbnail*)this);
	//}

	if (bInner)
	{
		return;
	}

	if (g_systemVersion >= 6)
	{
		//不用考虑水平方向
		if (m_matrix._31 > m_constraints.right) 
		{
			m_matrix._31 = (float)(m_constraints.right);
		}
		else if (m_matrix._31 < m_constraints.left)
		{
			m_matrix._31 = (float)(m_constraints.left);
		}

		if (m_isDir || !m_bisVideoFile)   //除视频照片文件和和文件夹其他的都只能上向移动
		{
			if (m_matrix._32 > m_constraints.bottom)     
			{
				m_matrix._32 = (float)(m_constraints.bottom);
			}
			else if (m_matrix._32 < m_constraints.top)
			{
				m_matrix._32 = (float)(m_constraints.top);
			}
		}
	}
	else
	{
		//不用考虑水平方向
		if (m_xformTranslate.eDx > m_constraints.right) 
		{
			m_xformTranslate.eDx = (float)(m_constraints.right);
		}
		else if (m_xformTranslate.eDx < m_constraints.left)
		{
			m_xformTranslate.eDx = (float)(m_constraints.left);
		}
		if (m_isDir ||  !m_bisVideoFile)
		{
			if (m_xformTranslate.eDy > m_constraints.bottom)
			{
				m_xformTranslate.eDy = (float)(m_constraints.bottom);
			}
			else if (m_xformTranslate.eDy < m_constraints.top)
			{
				m_xformTranslate.eDy = (float)(m_constraints.top);
			}
		}
	}

	////平移之前没有越界 到 平移之后越界，则发送消息
 //   if (!fPastBounds && IsPastBounds())
 //   {
 //       UINT msg = (m_bIsInertiaActive) ? PS_RESOURCE_BOUNDARY_INERTIA : PS_RESOURCE_BOUNDARY;
 //       SendMessage(m_hAttachWnd, msg, (WPARAM)0, (LPARAM)(CWBThumbnail*)this);
 //   }
}

//BOOL CWBThumbnail::IsPastBounds()
//{
//	if (g_systemVersion >= 6)
//	{
//		return (m_matrix._31 >= m_bounds.right || m_matrix._31 <= m_bounds.left || m_matrix._32 >= m_bounds.bottom || m_matrix._32 <= m_bounds.top);
//	}
//	else
//	{
//		return (m_xformTranslate.eDx >= m_bounds.right || m_xformTranslate.eDx <= m_bounds.left || m_xformTranslate.eDy >= m_bounds.bottom || m_xformTranslate.eDy <= m_bounds.top);
//	}
//}

void CWBThumbnail::SetFilePath(LPCTSTR const uri)
{
	m_strThumbURI = tstring(uri);
	m_strThumbName = tstring(PathFindFileName(uri));
	tstring sFileName;
	int nTag = m_strThumbURI.rfind(TEXT('\\'));
	if (nTag>=0)
	{
		sFileName = m_strThumbURI.substr(0, nTag+1);//目录名包括"\"符号
	}
	return; 
}

HRESULT CWBThumbnail::LoadBitmapFromFileXP( LPCTSTR const uri )
{
	HRESULT hr = S_OK;

	if (this->m_id == 0 && PathFileExists(uri))
	{
		m_pGdiPlusBmp = Bitmap::FromFile(ToDRAWStr(uri).c_str());
		if (m_pGdiPlusBmp != NULL)
		{
			m_fpWidth = m_pGdiPlusBmp->GetWidth();
			m_fpHeight = m_pGdiPlusBmp->GetHeight();
			return hr;
		}
	}

	HBITMAP	hBmp = NULL;
	BITMAP bmp;
	hr = CreateThumbnail(uri, &hBmp);//创建自定义图标96*96

	if (FAILED(hr))
	{
		//获取自定义图标失败则获取默认图标
		//CoInitialize(NULL);
		HICON hIcon;
		SHFILEINFOW sfi = {0};
		SHGetFileInfo(uri, -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX); //48*48
		IImageList* imageList = NULL;
		HRESULT hResult = SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);  
		if (SUCCEEDED(hResult))
		{
			hResult = (imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
		}
		else
		{
			ZeroMemory(&sfi,sizeof(SHFILEINFO));
			SHGetFileInfo(uri,0,&sfi,sizeof(SHFILEINFO),SHGFI_LARGEICON|SHGFI_ICON);  //32*32
			hIcon = sfi.hIcon;
		}
		ICONINFO iconinfo;
		BOOL bGet = GetIconInfo(hIcon, &iconinfo);
		hBmp  =  iconinfo.hbmColor;
		hr = (hBmp)?S_OK:S_FALSE;

		if (FAILED(hr) && (m_id != 0))
		{
			TCHAR ppath[MAX_PATH] = {0};
			tstring defaultBitmap;
			GetModuleFileName(GetModuleHandle(NULL),ppath,MAX_PATH);
			_tcsrchr(ppath,TEXT('\\'))[0] = TEXT('\0');
			defaultBitmap = ppath;
			if (m_isDir)
			{
				defaultBitmap += TEXT("\\SKIN\\WhiteBoard\\DefaultFolder.png");
			}
			else
			{
				defaultBitmap += TEXT("\\SKIN\\WhiteBoard\\DefaultFile.png");
			}
			//如果是资源库图标,当hr失败时加载默认图标文件
			if (PathFileExists(defaultBitmap.c_str()))
			{
				m_pGdiPlusBmp = Bitmap::FromFile(defaultBitmap.c_str());
				if (m_pGdiPlusBmp != NULL)
				{
					m_fpWidth = m_pGdiPlusBmp->GetWidth();
					m_fpHeight = m_pGdiPlusBmp->GetHeight();
					return S_OK;
				}
			}
		}
	}

	//计算thumbnail的宽度
	GetObject(hBmp, sizeof(bmp), &bmp);
	m_fpWidth = bmp.bmWidth;
	m_fpHeight = bmp.bmHeight;

	BYTE *pByte = new BYTE[(int)(m_fpWidth*4*m_fpHeight)];
	if (pByte != NULL)
	{
		GetBitmapBits(hBmp,(int)(m_fpWidth*4*m_fpHeight),pByte);
		m_pGdiPlusBmp = new Bitmap(m_fpWidth,m_fpHeight,m_fpWidth*4,PixelFormat32bppARGB,NULL);
		Rect rcImage(0,0, m_fpWidth, m_fpHeight);
		BitmapData bmpData;
		m_pGdiPlusBmp->LockBits(&rcImage, ImageLockModeWrite, PixelFormat32bppARGB,&bmpData);
		memcpy(bmpData.Scan0,pByte,(int)(m_fpWidth*4*m_fpHeight));
		m_pGdiPlusBmp->UnlockBits(&bmpData);
		delete[] pByte;
	}
	DeleteObject(hBmp);

	if (NULL == m_pGdiPlusBmp)
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT CWBThumbnail::LoadBitmapFromFile(LPCTSTR uri)
{

	HRESULT hr = S_OK;

	if ((this->m_id == 0 ) && PathFileExists(uri))
	{
		IWICBitmapSource *pBit= NULL;
		HBITMAP pWrapBitmap = CWBThumbnail::LoadPictureFromFile(uri,m_d2dDriver,0,&pBit);
		if (pBit != NULL)
		{
			m_d2dDriver->GetRenderTarget()->CreateBitmapFromWicBitmap(pBit,&m_pBitmap);
			SafeRelease(&pBit);
			if (m_pBitmap!=NULL)
			{
				if (s_pID2D1BmpAtlas == NULL)
				{
					s_pID2D1BmpAtlas = ID2D1BmpAtlas::CreateBmpAtlas(ICON_SIZE,ICON_SIZE,m_spRT);
				}
				if (s_pID2D1BmpAtlas != NULL)
				{
					m_fpWidth = m_pBitmap->GetSize().width;
					m_fpHeight = m_pBitmap->GetSize().height;
					m_hD2DBmp = s_pID2D1BmpAtlas->AddBitmap(m_pBitmap);
					SafeRelease(&m_pBitmap);
				}
				return hr;
			}
		}
	}

	HBITMAP	hBmp = NULL;
	
	//转化为D2D数据
	if (m_d2dDriver == NULL)
	{
		hr = S_FALSE;
	}
	IWICImagingFactory *pFactory = m_d2dDriver->GetImageFactory();
		//获取图标
	if(NULL != pFactory)
	{
		{

			HRESULT hr = S_OK;
			HDC memoryDC = ::CreateCompatibleDC(NULL);
			HBITMAP iconImage = NULL;
			BYTE *pBits = NULL;
			IShellItemImageFactory *pShellItemImageFactory = NULL;


			static HMODULE hShell = NULL;
			typedef HRESULT (_stdcall *SHELLPRO)(PCWSTR ,IBindCtx *,REFIID,void **);
			SHELLPRO SHCreateItemFromParsingName = NULL;
			if(g_systemVersion >= 6)
			{
				if(hShell == NULL)
				{
					hShell = LoadLibrary(TEXT("Shell32.dll"));
				}
				if(hShell != NULL)
				{
					SHCreateItemFromParsingName = (SHELLPRO)GetProcAddress(hShell,"SHCreateItemFromParsingName");
				}
			}

			if(SHCreateItemFromParsingName != NULL)
			{
				HDC hdcMem1,hdc;
				hdc = ::GetDC(m_hAttachWnd);
				hdcMem1 = ::CreateCompatibleDC (hdc); 
				hr = SHCreateItemFromParsingName(
					ToDRAWStr(uri).c_str(),
					m_pBindContext,
					IID_PPV_ARGS(&pShellItemImageFactory)
					);
				SIZE iconSize;
				iconSize.cx = ICON_SIZE;
				iconSize.cy = ICON_SIZE;
				//获取目录项图标
				if(pShellItemImageFactory != NULL)
				{
					hr = pShellItemImageFactory->GetImage(iconSize,SIIGBF_RESIZETOFIT, &iconImage);
					int bisize = sizeof(BITMAPINFOHEADER)+ 256*sizeof(RGBQUAD);
					BITMAPINFO *bi = (BITMAPINFO*) new BYTE[sizeof(BITMAPINFOHEADER)+ 256*sizeof(RGBQUAD)];
					ZeroMemory(bi, sizeof(BITMAPINFO));
					bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

					if (iconImage != NULL)
					{
						//获取图标信息
						::GetDIBits(
							memoryDC,   // hdc
							iconImage,  // hbmp
							0,          // uStartScan
							0,          // cScanLines
							NULL,       // lpvBits
							bi,
							DIB_RGB_COLORS
							);
						if(bi->bmiHeader.biHeight > 0) bi->bmiHeader.biHeight = - bi->bmiHeader.biHeight;

						const UINT sc_bitsArraySize = (-bi->bmiHeader.biHeight) * (bi->bmiHeader.biWidth) * 4; // BGRA图片每像素4 bytes
						pBits = new (std::nothrow) BYTE[sc_bitsArraySize];
						hr = pBits ? S_OK : E_OUTOFMEMORY;

						::GetDIBits(
							memoryDC,
							iconImage,
							0,
							-bi->bmiHeader.biHeight,
							pBits,
							bi,
							DIB_RGB_COLORS);

						m_fpWidth = bi->bmiHeader.biWidth;
						m_fpHeight = -bi->bmiHeader.biHeight;
// 						if(m_pBitmap != NULL)
// 						{
// 							m_pBitmap->Release();
// 							m_pBitmap = NULL;
// 						}
// 						hr = m_spRT->CreateBitmap(
// 							D2D1::SizeU(m_fpWidth, m_fpHeight),
// 							D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
// 							&m_pBitmap
// 							);
// 						hr = m_pBitmap->CopyFromMemory(
// 							&D2D1::RectU(0,0,m_fpWidth,m_fpHeight),
// 							pBits,
// 							bi->bmiHeader.biSizeImage / -(bi->bmiHeader.biHeight)
// 							);

						if (s_pID2D1BmpAtlas == NULL)
						{
							s_pID2D1BmpAtlas = ID2D1BmpAtlas::CreateBmpAtlas(ICON_SIZE,ICON_SIZE,m_spRT);
						}
						if (s_pID2D1BmpAtlas != NULL)
						{
							m_hD2DBmp = s_pID2D1BmpAtlas->AddBitmap(m_fpWidth,m_fpHeight,(BYTE*)pBits,0);
						}
						if (pBits != NULL)
						{
							delete[] pBits;
							pBits = NULL;
						}

					}

					delete[] bi;
				}
				::DeleteDC (hdcMem1); 
				::ReleaseDC(m_hAttachWnd,hdc);
				SafeRelease(&pShellItemImageFactory);
				if (iconImage != NULL)
				{
					::DeleteObject(iconImage);
				}
			}
			if (memoryDC != NULL)
			{
				::DeleteDC(memoryDC);
			}	
		}
	}
	else
	{
		hr = S_FALSE;
	}
    return hr;
}

HBITMAP CWBThumbnail::LoadPictureFromFile( LPCTSTR FileName,CD2DDriver *d2DDriver,PVOID *pDate /*= NULL*/,IWICBitmapSource **pSour /*= NULL*/ )
{
	LRESULT hr = S_OK;
	IWICBitmapDecoder *pDecoder = NULL;
	hr = d2DDriver->GetImageFactory()->CreateDecoderFromFilename(
		FileName,                      // Image to be decoded
		NULL,                            // Do not prefer a particular vendor
		GENERIC_READ,                    // Desired read access to the file
		WICDecodeMetadataCacheOnDemand,  // Cache metadata when needed
		&pDecoder                        // Pointer to the decoder
		);
	IWICBitmapFrameDecode *pFrame = NULL;
	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pFrame);
	}
	IWICFormatConverter *pConverter = NULL;

	if (SUCCEEDED(hr))
	{
		hr = d2DDriver->GetImageFactory()->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}
	SafeRelease(&pFrame);
	SafeRelease(&pDecoder);

	if( SUCCEEDED(hr) )
	{
		UINT width;
		UINT height;
		pConverter->GetSize(&width,&height);
		BYTE *pBuff = NULL;
		BITMAPINFO bminfo;
		ZeroMemory(&bminfo, sizeof(bminfo));
		bminfo.bmiHeader.biSize         = sizeof(BITMAPINFOHEADER);
		bminfo.bmiHeader.biWidth        = width;
		bminfo.bmiHeader.biHeight       = -(LONG)height;
		bminfo.bmiHeader.biPlanes       = 1;
		bminfo.bmiHeader.biBitCount     = 32;
		bminfo.bmiHeader.biCompression  = BI_RGB;
		HDC hdc = GetDC(NULL);
		HBITMAP hbitmap = CreateDIBSection(hdc, &bminfo, DIB_RGB_COLORS,
			(void**)&pBuff, NULL, 0);
		hr = pConverter->CopyPixels(NULL,width*4,width*height*4,pBuff);
		ReleaseDC(NULL,hdc);
	

		if (FAILED(hr) || (pSour == NULL))
		{
			SafeRelease(&pConverter);
		}
		else
		{
			*pSour = pConverter;
		}
		return hbitmap;
	}

	return NULL;
}

HRESULT CWBThumbnail::CreateThumbnail( LPCTSTR strFileName, HBITMAP* pThumbnail )    //创建缩略图
{
	LPITEMIDLIST pidlURL = NULL;
	LPITEMIDLIST pidlWorkDir = NULL;
	HRESULT hr = S_OK;  
	TCHAR pszPath[MAX_PATH] = {0};  
	DWORD dwFlags = IEIFLAG_ASPECT;
	m_fpWidth = m_fpHeight;//长度和高度一样
	SIZE size = { static_cast<LONG>(m_fpWidth), static_cast<LONG>(m_fpHeight)};  
	IExtractImage* peiURL = NULL;	// interface is used to request a thumbnail image from a Shell folder
	IShellFolder* psfWorkDir = NULL;  
	int nTag = m_strThumbURI.rfind(TEXT('\\'));
	tstring sFilePathName = tstring(strFileName);
	tstring sFileDirectory = sFilePathName.substr(0, nTag);//目录名
	tstring sFileName = sFilePathName.substr(nTag+1, m_strThumbURI.size()-nTag-1);//文件名
	// 临时缓冲区
	//TCHAR wsBuffer[MAX_PATH];
	// 初始化Com库

	hr = CoInitialize( NULL );
	if ( FAILED( hr ) )
	{
		OutputDebugString( _T( "初始化Com库失败!" ) );
		CoUninitialize();
		return S_FALSE;
	}
	
	// ParseDisplayName:Translates a file object's or folder's display name into an item identifier list.
	//USES_CONVERSION;
	if (SUCCEEDED(hr))
	{
		hr = m_psfDesktop->ParseDisplayName(NULL, NULL, (DRAWCHAR*)ToDRAWStr(sFileDirectory).c_str(), NULL, &pidlWorkDir, NULL);
	}
	//if(FAILED(hr)) goto OnExit;
	if(SUCCEEDED(hr))
	{
		hr = m_psfDesktop->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&psfWorkDir);
	}
	
	if (SUCCEEDED(hr))
	{
		hr = psfWorkDir->ParseDisplayName(NULL, NULL, (DRAWCHAR*)ToDRAWStr(sFileName).c_str(), NULL, &pidlURL, NULL);
	}

	if (SUCCEEDED(hr))
	{
		hr = psfWorkDir->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST*)&pidlURL, IID_IExtractImage, NULL, (LPVOID*)&peiURL);
	}

	if (SUCCEEDED(hr))
	{
		hr = peiURL->GetLocation(pszPath, MAX_PATH, NULL, &size, 32, &dwFlags);
	}

	if (SUCCEEDED(hr))
	{
		hr = peiURL->Extract(pThumbnail);
	}

	if(peiURL != NULL) peiURL->Release();
	if(pidlURL != NULL) m_pMalloc->Free(pidlURL);
	if(pidlWorkDir != NULL) m_pMalloc->Free(pidlWorkDir);
	if(psfWorkDir != NULL) psfWorkDir->Release();
	//CoUninitialize();

	//hr = E_FAIL;
	return hr;
}

HRESULT CWBThumbnail::Initialize()
{
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr))
	{
		// 获得IMalloc接口
		hr = SHGetMalloc( &m_pMalloc);
	}
	if (SUCCEEDED (hr))
	{
		// 获得桌面文件夹
		hr = SHGetDesktopFolder(&m_psfDesktop);  
	}
	if ( FAILED( hr ) )
	{
		if (m_pMalloc)
		{
			m_pMalloc->Release();
			m_pMalloc = NULL;
		}
		if (m_psfDesktop)
		{
			m_psfDesktop->Release();
			m_psfDesktop = NULL;
		}
	}
	return hr;
}

void CWBThumbnail::D2D1MakeRotateMatrixEx(
	__in FLOAT angle, 
	__in D2D1_POINT_2F center,
	__out D2D1_MATRIX_3X2_F *matrix 
	)
{
	static HMODULE hUserDll = LoadLibrary(TEXT("d2d1.dll"));

	if (NULL == hUserDll)
	{
		return;
	}

	typedef	BOOL (__stdcall *D2D1FUN)(FLOAT, D2D1_POINT_2F, D2D1_MATRIX_3X2_F*);
	static D2D1FUN D2D1MakeRotateMatrix = (D2D1FUN)GetProcAddress(hUserDll, "D2D1MakeRotateMatrix");

	if (D2D1MakeRotateMatrix == NULL)
	{
		return;
	}

	(*D2D1MakeRotateMatrix)(angle, center, matrix);
}

BOOL CWBThumbnail::D2D1InvertMatrixEx( __inout D2D1_MATRIX_3X2_F *matrix )const
{
	static HMODULE hUserDll = LoadLibrary(TEXT("d2d1.dll"));

	if (NULL == hUserDll)
	{
		return FALSE;
	}

	typedef	BOOL (__stdcall *D2D1FUN)(D2D1_MATRIX_3X2_F*);
	static D2D1FUN D2D1InvertMatrix = (D2D1FUN)GetProcAddress(hUserDll, "D2D1InvertMatrix");

	if (D2D1InvertMatrix == NULL)
	{
		return FALSE;
	}

	return (*D2D1InvertMatrix)(matrix);
}

float CWBThumbnail::GetYOffset()
{
	 if (g_systemVersion >= 6)
	 {
		 return m_matrix._32;
	 }
	 else
	 {
		 return m_xform.eDy;
	 }
}

float CWBThumbnail::GetXOffset()
{
	if (g_systemVersion >= 6)
	{
		return m_matrix._31;
	}
	else
	{
		return m_xform.eDx;
	}
}

void CWBThumbnail::CalibrationXPosition( float fXOffSet )
{
	Translate(fXOffSet,0);
}

void CWBThumbnail::SetYOffsetPos( float fYPos /*= 0.0*/ )
{
	if (g_systemVersion >= 6)
	{
		m_MatrixTranslate._32 = fYPos;
	}
	else
	{
		m_xformTranslate.eDy = fYPos;
	}
}

float CWBThumbnail::SetXOffsetPos(float fXPos)
{
	float OldXPos;
	if (g_systemVersion >= 6)
	{
		OldXPos = m_matrix._31;
		m_matrix._31 = fXPos;
		D2D1::Matrix3x2F trans = D2D1::Matrix3x2F::Translation(0,0);
		m_matrix.SetProduct(trans, m_matrix);
		m_MatrixTranslate = m_matrix;
	}
	else
	{
		OldXPos = m_xform.eDx;
		m_xform.eDx = fXPos;
		XFORM trans = TranslateXForm(0,0);
		m_xform = SetProductXForm(trans, m_xform);
		m_xformTranslate = m_xform;
	}
	return OldXPos;
}

void CWBThumbnail::ResetYOffSet()
{
	if (g_systemVersion >= 6)
	{
		m_matrix._32 = m_fpYPosConst;
		m_MatrixTranslate._32 = m_fpYPosConst;
	}
	else
	{
		m_xform.eDy = m_fpYPosConst;
		m_xformTranslate.eDy = m_fpYPosConst;
	}
}

void CWBThumbnail::InitializeXForm( XFORM * xForm )
{
	xForm->eM11 = 1.f;
	xForm->eM12 = 0.f;
	xForm->eM21 = 0.f;
	xForm->eM22 = 1.f;
	xForm->eDx  = 0.f;
	xForm->eDy  = 0.f;
}

XFORM CWBThumbnail::TranslateXForm( float x, float y )
{
	XFORM translation;
	InitializeXForm(&translation);
	translation.eDx = x;
	translation.eDy = y;
	return translation;
}


XFORM CWBThumbnail::SetProductXForm( 
	__in const XFORM & a,
	__in const XFORM & b
	)
{
	XFORM xform;
	xform.eM11 = a.eM11 * b.eM11 + a.eM12 * b.eM21;
	xform.eM12 = a.eM11 * b.eM12 + a.eM12 * b.eM22;
	xform.eM21 = a.eM21 * b.eM11 + a.eM22 * b.eM21;
	xform.eM22 = a.eM21 * b.eM12 + a.eM22 * b.eM22;
	xform.eDx  = a.eDx * b.eM11 + a.eDy * b.eM21 + b.eDx;
	xform.eDy  = a.eDx * b.eM12 + a.eDy * b.eM22 + b.eDy;
	return xform;
}

Point CWBThumbnail::XFormTransformPoint( const XFORM & a, Point & point )
{
	Point outPoint;
	outPoint.X = point.X * a.eM11 + point.Y * a.eM21 + a.eDx;
	outPoint.Y = point.X * a.eM12 + point.Y * a.eM22 + a.eDy;
	return outPoint;
}

void CWBThumbnail::touchUpHandler( const TouchUp& te )
{
	POINT pt;
	pt.x = te.xpos;
	pt.y = te.ypos;
	m_bHorDirection = FALSE;
	CWBResGallery *pGallery = ((CWBResGallery*)GetWindowLongPtr(GetHWnd(), 0));
	if(m_isDir && m_id != 0 &&!m_bisVideoFile)
	{
		RECT rt;
		::GetClientRect(m_hAttachWnd,&rt);
		::ScreenToClient(m_hAttachWnd,&pt);
		if(!PtInRect(&rt,pt))
		{
			if (g_bShowDlg)
			{
				if (m_hDialog == NULL)
				{
#ifdef _DEBUG
					m_hDialog = LoadLibrary(TEXT("MTFolder.dll"));
#else
					m_hDialog = LoadLibrary(TEXT("MTFolder.dll"));
#endif
				}
				if (m_hDialog == NULL)
				{
					return;
				}
				typedef const wchar_t* (*SHOWDIALOGUI)(HWND);
				SHOWDIALOGUI showdialog = (SHOWDIALOGUI)GetProcAddress(m_hDialog,"ShowDialogUI");
				if(showdialog == NULL) return;
				m_bSelect = TRUE;
				showdialog(m_hAttachWnd);
			}
			else
			{
				m_bSelect = TRUE;
				::PostMessage(m_hAttachWnd, UWM_CHANGE_NUME, 0, (LPARAM)((int)1));
			}
		}
		return;
	}
	HWND hWnd = ::WindowFromPoint(pt);
	if (pGallery && pGallery->BInReceiveDragWndList(hWnd) && !m_isDir)
	{
	::PostMessage(hWnd, UWM_DRAGOUT_RESOUCE, (WPARAM)(TCHAR*)(m_strThumbURI.c_str()), (LPARAM)(MAKELPARAM(te.xpos, te.ypos)));
	}
	if (pGallery->GetIsDrawRubbishIcon())  //删除视频照片中的文件
	{
		bool isDelFile = DeleteFile(m_strThumbURI.c_str());
		if (isDelFile)
		{
			m_bSelect = TRUE;
			::PostMessage(m_hAttachWnd, UWM_CHANGE_NUME, 0, (LPARAM)((int)1));
			pGallery->SetIsDrawRubbishIcon(FALSE);
		}
		else
		{
			::PostMessage(pGallery->m_hWBViewHwnd, UWM_DELFILE_OPEN, (WPARAM)(TCHAR*)(m_strThumbName.c_str()), 0);
			pGallery->SetIsDrawRubbishIcon(FALSE);
		}
	}
	//ResetYOffSet();
}

void CWBThumbnail::touchDownHandler( const TouchDown& te )
{
	m_FirstPoint.x = te.xpos;
	m_FirstPoint.y = te.ypos;
	::ScreenToClient(m_hAttachWnd,&m_FirstPoint);
	m_FirstPoint.x -= m_ptIcon.x;
	m_FirstPoint.y -= m_ptIcon.y;

	if (m_id != 0)
	{
		m_IconImage = DrapIcon();
	}
}

void CWBThumbnail::OnCallbackGesture( const unsigned int &nMessage, const unsigned int &nAppendMessage /*= 0*/ )
{
	int nMsg = LOWORD((DWORD)nMessage);
	if(nMsg == TGC_TAP||nMsg == TGC_CLICK)
	{
		if (m_id == 0 || ::PathFileExists(m_strThumbURI.c_str()))
		{
			::PostMessage(m_hAttachWnd,UWM_CHANGE_RES,0,(LPARAM)this);
		}
		OutputDebugString(TEXT("\nCWBThumbnail::OnCallbackGesture"));
	}
}

HRESULT CWBThumbnail::GetItemBmp( LPCTSTR const str )
{
	HRESULT hr = S_OK;
	if (g_systemVersion >= 6)
	{
		if (m_hD2DBmp == NULL)
		{
			hr = LoadBitmapFromFile(str);	//重新计算了实际的宽度
		}
	}
	else
	{
		if (m_pGdiPlusBmp == NULL)
		{
			hr = LoadBitmapFromFileXP(str);
		}
	}
	return hr;
}

HBITMAP CWBThumbnail::DrapIcon( BOOL bClear /*= FALSE*/ )
{
	static HBITMAP hDrapIcon = NULL;
	if (hDrapIcon != NULL)
	{
		::DeleteObject(hDrapIcon);
		hDrapIcon = NULL;
	}
	if (!bClear)
	{
		if (g_systemVersion >= 6)
		{
			IWICImagingFactory *pFactory = m_d2dDriver->GetImageFactory();
			//获取图标
			if(NULL != pFactory)
			{
				IShellItemImageFactory *pShellItemImageFactory = NULL;
				static HMODULE hShell = NULL;
				typedef HRESULT (_stdcall *SHELLPRO)(PCWSTR ,IBindCtx *,REFIID,void **);
				static SHELLPRO SHCreateItemFromParsingName = NULL;
				if(hShell == NULL)
				{
					hShell = GetModuleHandle(TEXT("Shell32.dll"));
				}
				if(hShell != NULL)
				{
					SHCreateItemFromParsingName = (SHELLPRO)GetProcAddress(hShell,"SHCreateItemFromParsingName");
				}

				if(SHCreateItemFromParsingName != NULL)
				{
					SHCreateItemFromParsingName(
						ToDRAWStr(m_strThumbURI).c_str(),
						m_pBindContext,
						IID_PPV_ARGS(&pShellItemImageFactory)
						);
					SIZE iconSize;
					iconSize.cx = ICON_SIZE;
					iconSize.cy = ICON_SIZE;
					//获取目录项图标
					if(pShellItemImageFactory != NULL)
					{
						pShellItemImageFactory->GetImage(iconSize,SIIGBF_RESIZETOFIT, &hDrapIcon);
					}
				}
			}
		} 
		else
		{
			CreateThumbnail(m_strThumbURI.c_str(), &hDrapIcon);//创建自定义图标
			if (hDrapIcon == NULL)
			{
				//获取自定义图标失败则获取默认图标
				if (m_isDir && (m_id != 0))
				{
					TCHAR ppath[MAX_PATH] = {0};
					tstring defaultBitmap;
					GetModuleFileName(GetModuleHandle(NULL),ppath,MAX_PATH);
					_tcsrchr(ppath,TEXT('\\'))[0] = TEXT('\0');
					defaultBitmap = ppath;
					defaultBitmap += TEXT("\\SKIN\\WhiteBoard\\DefaultFolder.png");
					//如果是资源库图标,当hr失败时加载默认图标文件
					if (PathFileExists(defaultBitmap.c_str()))
					{
						Bitmap bitmap(defaultBitmap.c_str());
						bitmap.GetHBITMAP(Color(0),&hDrapIcon);
					}
				}

				if (hDrapIcon == NULL)
				{
					HICON hIcon;
					SHFILEINFOW sfi = {0};
					SHGetFileInfo(m_strThumbURI.c_str(), -1, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX);
					IImageList* imageList = NULL;
					HRESULT hResult = SHGetImageList(SHIL_EXTRALARGE, IID_IImageList, (void**)&imageList);
					if (SUCCEEDED(hResult))
					{
						hResult = (imageList)->GetIcon(sfi.iIcon, ILD_TRANSPARENT, &hIcon);
					}
					else
					{
						ZeroMemory(&sfi,sizeof(SHFILEINFO));
						SHGetFileInfo(m_strThumbURI.c_str(),0,&sfi,sizeof(SHFILEINFO),SHGFI_LARGEICON|SHGFI_ICON);
						hIcon = sfi.hIcon;
					}

					ICONINFO iconinfo;
					BOOL bGet = GetIconInfo(hIcon, &iconinfo);
					hDrapIcon = iconinfo.hbmColor;
				}
			}
		}
	}
	return hDrapIcon;
}

bool CWBThumbnail::DeleteFile( LPCTSTR filePath )
{
	int handle = 0;
	handle = _tsopen(filePath,_O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	if (handle ==-1)   //说明该文件被占用
	{
		return false;
	}
	else
	{
		_close(handle);
		SHFILEOPSTRUCT FileOP;
		ZeroMemory(&FileOP,sizeof(FileOP));
		TCHAR ToBuf[MAX_PATH] = {0};
		TCHAR FromBuf[MAX_PATH] ={0};
		ZeroMemory(ToBuf, sizeof(ToBuf));
		ZeroMemory(FromBuf, sizeof(FromBuf));
		lstrcpy(FromBuf, filePath);
		FileOP.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION ;
		FileOP.hNameMappings = NULL;
		FileOP.hwnd = NULL;
		FileOP.lpszProgressTitle = NULL;
		FileOP.pFrom = FromBuf;
		FileOP.pTo = ToBuf;
		FileOP.wFunc = FO_DELETE;
		return SHFileOperation(&FileOP)==0;
	}

}

void CWBThumbnail::ClearAtlas()
{
	SafeRelease(&s_pID2D1BmpAtlas);
}

ID2D1BmpAtlas* CWBThumbnail::s_pID2D1BmpAtlas = NULL;
