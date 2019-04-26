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
//������ΪsResourceType������ΪsResource����Դ����ͼ��
ImageEx::ImageEx( IN LPCTSTR sResourceType, IN LPCTSTR sResource )  //��ťͼƬ
{
	m_pBitmapBuf = NULL;
	m_pTemp = NULL;
	lPause = 0;

	this->Initialize();
	
	if (Load(sResourceType, sResource))
	{
		m_pTemp = Bitmap::FromStream(m_pStream);//���������л��λͼ
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

//���컺��ͼ
//���ļ�����ͼ��
ImageEx::ImageEx(const WCHAR* filename, BOOL useEmbeddedColorManagement) //ͼƬ->��ťͼƬ
{
	m_pBitmapBuf = NULL;
	m_pTemp = NULL;
	CRect rect;
	lPause = 0;

	this->Initialize();
	m_pTemp = new Bitmap(filename);
	
	UINT Width = m_pTemp->GetWidth();
	UINT Height = m_pTemp->GetHeight();
	if (g_systemVersion <6)//�ж�XP��
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

	Graphics *g = Graphics::FromImage(m_pBitmapBuf);   //��ָ����IMAGE�д����µ�Graphics
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
	g->SetSmoothingMode(SmoothingModeHighQuality);  //��ͼ�������������Ⱦģʽ
	g->SetInterpolationMode(InterpolationModeHighQualityBicubic);   //����ͼ�ε���������
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
//�ѳ���ΪnSize���ڴ�pBuff�е����ݴ���ΪIStream�����������m_pStream��Ա�����У�
//���سɹ���񣬲��������ķǷ���
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
//װ������ΪlpName������ΪlpType����Դ��pResource�У�����װ�سɹ����
//nBufSize��ʾpResource����ĳ��ȡ�pResourceΪNULLʱ��nBufSize���������ڴ��С��
//��ΪNULLʱ������ʵ����Դ��С�����Ȳ���ʱ���൱��pResourceΪNULLʱ�����ã�ֻ�Ƿ���ֵΪfalse��
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
//װ������ΪsResourceType������ΪsResource����Դ���������Ա����m_pStream�У�����װ�سɹ����
//m_pStream�ڸú�����װ�سɹ���񱣴��ڳ�Ա����m_bIsInitialized�У�
//ͬʱ���������Ķ��������ļ�����ģ���m_bIsInitialized��ʾ����ɹ����
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
//����Ѿ�����Ķ����Ƕ������򴴽������̣߳�������true��
//���Ϊ��̬ͼ����Ѿ������������̣߳���Ҳ����false
// ͼ�񽫻�����m_hWnd�ͻ�����rect���򣬻����죬֧�־���
bool ImageEx::InitAnimation() //����gif�����߳�
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

//����ͼ���Ƿ�Ϊ�������ǵĻ���Ա����m_pPropertyItem����malloc�����ڴ�
bool ImageEx::TestForAnimatedGIF()  
{
   UINT count = 0;
   count = m_pTemp->GetFrameDimensionsCount(); //
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   m_pTemp->GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   m_nFrameCount = m_pTemp->GetFrameCount(&pDimensionIDs[0]);//������ά���ж��ٸ�֡

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   //��ø�֮֡���ʱ����
   //�Ȼ���ж��ٸ�ʱ������PropertyTagFrameDelay��GDI+��Ԥ�����һ��GIG����IDֵ����ʾ��ǩ֡���ݵ��ӳ�ʱ��
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

	//m_hExitEvent = CreateEvent(NULL,TRUE,FALSE,NULL);  //�ֶ��߳��¼� ����ʱδ����

	//m_hPause = CreateEvent(NULL,TRUE,TRUE,NULL);      //�ֶ��߳��¼�  ����ʱ����

}


//UINT WINAPI ImageEx::_ThreadAnimationProc(LPVOID pParam)  //�����̺߳���
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
//�ڿͻ���������ǰ֡������ֵ��ʾ�Ƿ�Ҫ�˳��̺߳���
bool ImageEx::DrawFrameGIF()
{
	//::WaitForSingleObject(m_hPause, INFINITE);//m_hPause�ֹ����ã���ʼ���ź�

	//pageGuid��ֵ����ʾGIFΪFrameDimensionTime,��ʾTIFʱΪFrameDimensionPage
	GUID   pageGuid = FrameDimensionTime;
	//��������ʾ��һ֡
	m_pTemp->SelectActiveFrame(&pageGuid, m_nFramePosition++);		
	
	if (m_nFramePosition == m_nFrameCount)
		m_nFramePosition = 0;
	
	//ʱ����
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

	//long lPause = ((long*) m_pPropertyItem->value)[m_nFramePosition] * 10; //gifͼƬʱ�䵥λΪ1/10���룬��������Ҫ����10

	//g_pWhiteBoardView->Invalidate();
	//wangxiaobin

	//DWORD dwErr = WaitForSingleObject(m_hExitEvent, lPause); //m_hExitEvent�ֹ����ã���ʼ���ź�
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
	g->DrawImage(m_pTemp,0,0,m_pTemp->GetWidth(),m_pTemp->GetHeight());//��ʾ��ǰ֡
	delete g;
	g = NULL;

	return true;
	//return dwErr == WAIT_OBJECT_0;// �����lPauseʱ���ڲ����źţ��򷵻�true����ʾҪ�˳��߳�

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

//�رն�������ʵ�ϻ���Image�л��е�������Ա����û�йرգ���Ϊ������������û��������������йرյ�
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

