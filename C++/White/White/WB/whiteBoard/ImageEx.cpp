// GDIPlusHelper.cpp: implementation of the CGDIPlusHelper class.
//


#include "stdafx.h"
#include "ImageEx.h"
#include <process.h>
#include "whiteBoard.h"

#define  BitmapWidthPix  2000
#define  BitmapHeightPiy 2000
#define  BitmapCurWidthPix  5000
#define  BitmapCurHeightPiy 5000
//以类型为sResourceType，名称为sResource的资源构造图像
ImageEx::ImageEx( IN LPCTSTR sResourceType, IN LPCTSTR sResource )  //按钮图片
{
	m_pBitmapBuf = NULL;
	m_pTemp = NULL;
	lPause = 0;

	this->Initialize();
	
	if (Load(sResourceType, sResource))
	{
		m_pTemp = Bitmap::FromStream(m_pStream);//从数据流中获得位图
		CRect rect(0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());
		m_pBitmapBuf = new Bitmap(rect.Width(),rect.Height());               
		Graphics *g = Graphics::FromImage(m_pBitmapBuf);
		g->SetSmoothingMode(SmoothingModeHighQuality);
		g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
		g->DrawImage(m_pTemp,0,0,rect.Width(),rect.Height());
		delete g;
		g = NULL;

		m_bIsInitialized = true;

		if(TestForAnimatedGIF())
		{
			//g_pWBPage->m_vecGifList.push_back(m_pTemp);
			//InitAnimation();
		}
	}

}

//构造缓冲图
//以文件构造图像
ImageEx::ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) //图片->按钮图片
{
	m_pBitmapBuf = NULL;
	m_pTemp = NULL;
	CRect rect;
	lPause = 0;

	this->Initialize();
	m_pTemp = new Bitmap(filename);
	
	UINT Width = m_pTemp->GetWidth();
	UINT Height = m_pTemp->GetHeight();
	if (g_systemVersion <6)//判断XP下
	{
		if (Width > BitmapWidthPix || Height > BitmapHeightPiy)
		{
			UINT ChangeW,ChangeH;
			double PRE;
			PRE = (double)Width/(double)Height;
			if (Width > Height)
			{
				ChangeW = BitmapWidthPix;
				ChangeH = (UINT)(ChangeW/PRE);
			}
			else
			{
				ChangeH = BitmapHeightPiy;
				ChangeW = (UINT)(ChangeH*PRE);
			}
			rect.SetRect(0,0,ChangeW,ChangeH);
		}
 		else
 		{
 			rect.SetRect(0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());
 		}
	}
 	else
	{
		if (Width > BitmapCurWidthPix || Height > BitmapCurHeightPiy)
		{
			UINT ChangeW,ChangeH;
			double PRE;
			PRE = (double)Width/(double)Height;
			if (Width > Height)
			{
				ChangeW = BitmapCurWidthPix;
				ChangeH = (UINT)(ChangeW/PRE);
			}
			else
			{
				ChangeH = BitmapCurHeightPiy;
				ChangeW = (UINT)(ChangeH*PRE);
			}
			rect.SetRect(0,0,ChangeW,ChangeH);

		}
		else
		{
			rect.SetRect(0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());
		}
		//rect.SetRect(0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());
 	}
	m_pBitmapBuf = new Bitmap(rect.Width(),rect.Height()); 

	Graphics *g = Graphics::FromImage(m_pBitmapBuf);   //从指定的IMAGE中创建新的Graphics
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->DrawImage(m_pTemp,0,0,rect.Width(),rect.Height());
	delete g;
	g = NULL;

	m_bIsInitialized = true;


   if(TestForAnimatedGIF())
    {
   		//InitAnimation();
    }

}

ImageEx::ImageEx(HBITMAP hbmp)
{
	m_pBitmapBuf = NULL;
	m_pTemp = NULL;
	lPause = 0;

	this->Initialize();
	m_pTemp = new Bitmap(hbmp, NULL);
	CRect rect(0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());
	m_pBitmapBuf = new Bitmap(rect.Width(),rect.Height());               
	Graphics *g = Graphics::FromImage(m_pBitmapBuf);
	g->SetSmoothingMode(SmoothingModeHighQuality);  //对图像对象质量的渲染模式
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);   //控制图形的缩放质量
	g->DrawImage(m_pTemp,0,0,rect.Width(),rect.Height());
	delete g;
	g = NULL;

	m_bIsInitialized = true;

	if(TestForAnimatedGIF())
	{
		//InitAnimation();
	}
}

ImageEx::~ImageEx()
{
	Destroy();
}
//把长度为nSize的内存pBuff中的内容创建为IStream流保存在类的m_pStream成员变量中，
//返回成功与否，不检查参数的非法性
bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);

		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &m_pStream) == S_OK)
			bResult = true;
	}


	return bResult;
}
//装载名称为lpName，类型为lpType的资源到pResource中，返回装载成功与否。
//nBufSize表示pResource缓存的长度。pResource为NULL时，nBufSize返回所需内存大小，
//不为NULL时，返回实际资源大小，长度不够时，相当于pResource为NULL时的作用，只是返回值为false。
bool ImageEx::GetResource(LPCTSTR lpName, LPCTSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	bool		bResult	= FALSE;

	// Find the resource

	hResInfo = FindResource(m_hInst , lpName, lpType);
	if (hResInfo == NULL) 
	{
		DWORD dwErr = GetLastError();
		return false;
	}

	// Load the resource
	hRes = LoadResource(m_hInst , hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(m_hInst , hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(m_hInst , hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}
//装载类型为sResourceType，名称为sResource的资源到流对象成员变量m_pStream中，返回装载成功与否，
//m_pStream在该函数中装载成功与否保存在成员变量m_bIsInitialized中，
//同时，如果该类的对象是以文件构造的，则m_bIsInitialized表示构造成功与否
bool ImageEx::Load(CString sResourceType, CString sResource)
{
	bool bResult = false;


	BYTE*	pBuff = NULL;
	int		nSize = 0;
	if (GetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];

			if (GetResource(sResource, sResourceType.GetBuffer(0), pBuff, nSize))
			{
				if (LoadFromBuffer(pBuff, nSize))
				{

					bResult = true;
				}
			}
			sResourceType.ReleaseBuffer();
			delete [] pBuff;
		}
	}

	sResource.ReleaseBuffer();
	sResourceType.ReleaseBuffer();
	m_bIsInitialized = bResult;

	return bResult;
}
//如果已经构造的对象是动画，则创建动画线程，并返回true，
//如果为静态图像或已经创建过动画线程，则也返回false
// 图像将绘制在m_hWnd客户区的rect区域，会拉伸，支持镜像
bool ImageEx::InitAnimation() //创建gif动画线程
{
	//if (!m_bIsInitialized)
	//{
	//	TRACE(_T("GIF not initialized\n"));
	//	return false;
	//}

	//if (IsAnimatedGIF())
	//{
	//	if (m_hThread == NULL)
	//	{
	//		unsigned int nTID = 0;
	//		m_hThread = (HANDLE) _beginthreadex( NULL, 0, _ThreadAnimationProc, this, CREATE_SUSPENDED,&nTID);
	//		if (!m_hThread)
	//		{
	//			TRACE(_T("Couldn't start a GIF animation thread\n"));
	//			return true;
	//		} 
	//		else 
	//			ResumeThread(m_hThread);
	//	}
	//}
	return false;	
}

CRect ImageEx::GetRect()
{
	return CRect(0,0,m_pTemp->GetWidth(), m_pTemp->GetHeight());
}

//测试图像是否为动画，是的话成员变量m_pPropertyItem将会malloc分配内存
bool ImageEx::TestForAnimatedGIF()  
{
   UINT count = 0;
   count = m_pTemp->GetFrameDimensionsCount(); //
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   m_pTemp->GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = m_pTemp->GetFrameCount(&pDimensionIDs[0]);//获得这个维度有多少个帧

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   //获得各帧之间的时间间隔
   //先获得有多少个时间间隔，PropertyTagFrameDelay是GDI+中预定义的一个GIG属性ID值，表示标签帧数据的延迟时间
   int nSize = m_pTemp->GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   m_pPropertyItem = (PropertyItem*) malloc(nSize);

   m_pTemp->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);

   delete []pDimensionIDs;

   return m_nFrameCount > 1;
}

void ImageEx::Initialize()
{
	m_nFramePosition = 0;
	m_nFrameCount = 0;
	m_pStream = NULL;
	//m_hThread = NULL;
	m_bIsInitialized = false;
	m_pPropertyItem = NULL;

#ifdef INDIGO_CTRL_PROJECT
	m_hInst = _Module.GetResourceInstance();
#else
	m_hInst = AfxGetResourceHandle();
#endif


	//m_bPause = false;

	//m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);  //手动线程事件 创建时未激活

	//m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);      //手动线程事件  创建时激活

}


//UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)  //代理线程函数
//{
//	ASSERT(pParam);
//	ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);
//	pImage->ThreadAnimation();
//
//	return 0;
//}


//void ImageEx::ThreadAnimation()
//{
//	m_nFramePosition = 0;
//
//	bool bExit = false;
//	while (bExit == false)
//	{
//		bExit = DrawFrameGIF();
//	}
//}
//在客户区画出当前帧，返回值表示是否要退出线程函数
bool ImageEx::DrawFrameGIF()
{
	//::WaitForSingleObject(m_hPause, INFINITE);//m_hPause手工重置，初始有信号

	//pageGuid的值在显示GIF为FrameDimensionTime,显示TIF时为FrameDimensionPage
	GUID   pageGuid = FrameDimensionTime;
	//来设置显示那一帧
	m_pTemp->SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;
	
	//时间间隔
	//if (lPause == 0)
	//{
		lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10;
	//}
	if (lPause <= 100)
	{
		lPause =20/*((long*) m_pPropertyItem->value)[m_nFramePosition] * 10 * 0.4*/;
	}
	else if (lPause > 100 && lPause < 200 )
	{
		lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10 * 0.5;
	}
	else if (lPause >= 200 && lPause < 300)
	{
		lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10* 0.5;
	}
	else if ( lPause >= 300 && lPause < 400)
	{
		lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10 * 0.5;
	}
	else
	{
		lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10* 0.5;
	}

	//long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10; //gif图片时间单位为1/10毫秒，所以这里要乘以10

	//g_pWhiteBoardView->Invalidate();
	//wangxiaobin

	//DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause); //m_hExitEvent手工重置，初始无信号
	//if (dwErr == WAIT_OBJECT_0)
	//{
	//	return true;
	//}
	CRect rect;
	rect = m_rect;
	if (g_pWhiteBoardView == NULL)
	{
		return true;
	}
	else
	{
		if (g_pWhiteBoardView->GetSafeHwnd() == NULL)
		{
			return true;
		}
	}
	//g_pWhiteBoardView->SetIsRectRefresh(true);
	//g_pWhiteBoardView->InvalidateRect(rect,1);
	g_pWhiteBoardView->Invalidate();

	Graphics *g = Graphics::FromImage(m_pBitmapBuf);
	g->SetSmoothingMode(SmoothingModeHighQuality);
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	g->DrawImage(m_pTemp,0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());//显示当前帧
	delete g;
	g = NULL;

	return true;
	//return dwErr == WAIT_OBJECT_0;// 如果在lPause时间内产生信号，则返回true，表示要退出线程

}


void ImageEx::SetPause(bool bPause)
{
	//if (!IsAnimatedGIF())
	//	return;

	//if (bPause && !m_bPause)
	//{
	//	::ResetEvent(m_hPause);
	//}
	//else
	//{

	//	if (m_bPause && !bPause)
	//	{
	//		::SetEvent(m_hPause);
	//	}
	//}

	//m_bPause = bPause;
}

//关闭动画，事实上基类Image中还有的两个成员变量没有关闭，因为析构函数会调用基类析构函数进行关闭的
void ImageEx::Destroy()
{	
	//if (m_hThread)
	//{
	//	// If pause un pause
	//	SetPause(false);

	//	SetEvent(m_hExitEvent);
	//	WaitForSingleObject(m_hThread, INFINITE);
	//}

	//CloseHandle(m_hThread);
	//CloseHandle(m_hExitEvent);
	//CloseHandle(m_hPause);

	free(m_pPropertyItem);

	m_pPropertyItem = NULL;
	//m_hThread = NULL;
	//m_hExitEvent = NULL;
	//m_hPause = NULL;

	if (m_pBitmapBuf != NULL)
	{
		delete m_pBitmapBuf;
		m_pBitmapBuf = NULL;
	}
	if (m_pTemp!= NULL)
	{
		delete m_pTemp;
		m_pTemp = NULL;
	}
	if (m_pStream != NULL)
	{
		m_pStream->Release();
	}
}


void ImageEx::Draw(HDC hDC,CRect rect,int alpha)
{
	m_rect = rect;
	Graphics graphics(hDC);
	float fBlend = (float)(alpha/255.0);
	ColorMatrix BitmapMatrix =	{     
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, fBlend, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	ImageAttributes ImgAttr;
	ImgAttr.SetColorMatrix(&BitmapMatrix, 
		ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	Rect destination(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());
	if (m_pBitmapBuf != NULL)
	{
		graphics.DrawImage(m_pBitmapBuf,destination,0,0,m_pBitmapBuf->GetWidth(),m_pBitmapBuf->GetHeight(),UnitPixel,&ImgAttr);
	}

	graphics.ReleaseHDC(hDC);
}

void ImageEx::Draw(HDC hDC,int alpha)
{	
	Graphics graphics(hDC);
	float fBlend = (float)(alpha/255.0);
	ColorMatrix BitmapMatrix =	{     
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, fBlend, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	ImageAttributes ImgAttr;
	ImgAttr.SetColorMatrix(&BitmapMatrix, 
		ColorMatrixFlagsDefault, 
		ColorAdjustTypeBitmap);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	graphics.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
	Rect destination(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height());
	graphics.DrawImage(m_pBitmapBuf,destination,0,0,m_pBitmapBuf->GetWidth(),m_pBitmapBuf->GetHeight(),UnitPixel,&ImgAttr);
	graphics.ReleaseHDC(hDC);
}

