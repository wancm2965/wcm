
#include "stdafx.h" //don‘t use the default 预编译头文件(通过文件的预编译设置进行设置)

#include <windowsx.h>
#include <tpcshrd.h>
#include <string>
#include "ResGallery.h"
#include "Shlwapi.h"
#include <shobjidl.h> // for interface : IShellItemImageFactory
#include <Shlobj.h>
#include "Define.h"
#include "resource.h"
#include "ResData.h"
#include <algorithm>
#include "../whiteBoard/TouchAPI.h"
#include "MTRes.h"
#include <assert.h>
#include <Dbt.h>
#include "LanguageNameDefine.h"
#include "ID2D1BmpAtlas.h"
#pragma comment(lib,"comctl32.lib")
#define UWM_CHANGE_RES (WM_USER+217)
#define WM_USERDEF_FILECHANGED (WM_USER+220)

//获取触摸数据的协议
EProtocolType g_MTPotocol = PROTOCOL_WINTOUCH_CONNECT;

CWBResGallery::CWBResGallery(HWND hWnd,
	int nWidth, int nHeight, int nXPos, int nYPos,
	ResTypeCallbackFun pResCallback) :
	m_hWnd(hWnd),
	m_nWidth(nWidth),
	m_nHeight(nHeight),
	m_nXPos(nXPos),
	m_nYPos(nYPos),
	m_fpXOffset(0),
	m_bMTAuthorize(FALSE),
	m_lThumbnail(NULL),
	m_pResChangeBitmap(NULL),
	m_mtSpecialClick(NULL),
	m_pResChangeGDIBmp(NULL),
	BSupportedResType(pResCallback),
	m_nCurResIndex(0),
	m_bResChange(FALSE),
	m_hFolder(NULL),
	m_hDialog(NULL),
	m_pThumbnail(NULL),
	m_bDrapFlag(FALSE),
	m_hWndToolTip(NULL),
	m_ResHinstance(NULL),
	m_nDrawTipFalg(0),
	m_nDrawIntermediateFalg(0),
	m_hDevNotify(NULL),
	m_bisDrawRubbishIcon(FALSE),
	m_bisSpecialResource(FALSE),
	m_fResTotalLen(0)
{
	{
		tstring strPath = CResManage::GetInstance()->GetAvconIniPath();
		strPath += CONFIG_FILEPATH_OF_TOOLBAR;
		LPCTSTR strSelection = TEXT("Resource");    //
		LPCTSTR Position = TEXT("bShowTip");
		UINT bShowTip = GetPrivateProfileInt(strSelection, Position, 0/*默认值*/, strPath.c_str());
		if (bShowTip != 0)
		{
			ConfigDrawTipFlag(LEFT_SLIDE|UP_DRAG|RIGHT_SLIDE|UP_DEL|CHANGE_LIST|ADD_TIP);
			m_nDrawIntermediateFalg = m_nDrawTipFalg;
			WritePrivateProfileString(strSelection,Position,TEXT("0"), strPath.c_str());
		}
	}
	//SetInitResPath();
	InitLanPathNode(MTRES_TEXT);
	InitSpecialResPath();
	INITCOMMONCONTROLSEX cx = {sizeof(INITCOMMONCONTROLSEX),ICC_BAR_CLASSES}; 
	InitCommonControlsEx(&cx);   //初始化组件（提示窗口）
#ifdef _DEBUG
	m_ResHinstance = GetModuleHandle(TEXT("MTResd"));
#else
	m_ResHinstance = GetModuleHandle(TEXT("MTRes"));
#endif
	m_pLstResData = CResManage::GetInstance()->GetResMap();
	m_panel = RESSHOW;
	m_fIsMouseDown = FALSE;
	m_lThumbnail = new LWBThumbnail;
	if (NULL == BSupportedResType)
	{
		assert(FALSE);
		BSupportedResType = CWBResGallery::GetResType;
	}
	m_pMapChildObject = new std::map<unsigned long, CMTObject*>;
}

CWBResGallery::~CWBResGallery()
{
	if (m_hFolder != NULL)
	{
		FreeLibrary(m_hFolder);
		m_hFolder = NULL;
	}
	FreePhotos();
	if (m_d2dDriver != NULL)
	{
		delete m_d2dDriver;
		m_d2dDriver = NULL;
	}
	if(m_lThumbnail)
	{
		delete m_lThumbnail;
		m_lThumbnail = NULL;
	}
	if(m_mtSpecialClick)
	{
		delete m_mtSpecialClick;
		m_mtSpecialClick = NULL;
	}
	if (NULL != m_pMapChildObject)
	{
		m_pMapChildObject->clear();
		delete m_pMapChildObject;
		m_pMapChildObject = NULL;
	}
	if (m_hDevNotify)
	{
		UnregisterDeviceNotification (m_hDevNotify);
	}
	if (IsWindow(m_hWndToolTip))
	{
		::DestroyWindow(m_hWndToolTip);
	}
	m_lstReceiveDragWnd.clear();
}

HRESULT CWBResGallery::Initialize()
{
	HRESULT hr = S_OK;//CWBInertiaObj::Initialize();
	if ( SUCCEEDED(hr) )
	{
		m_d2dDriver = new CD2DDriver(m_hWnd);
		if (m_d2dDriver == NULL)
		{
			hr = E_FAIL;
		}
		else
		{
			hr = m_d2dDriver->Initialize();
		}
	}
	if (SUCCEEDED(hr))
	{
		// 临时缓冲区
		TCHAR szPath[MAX_PATH];
		GetModuleFileName(NULL, szPath, MAX_PATH); 
		tstring sTemp = tstring(szPath);
		int nIndexFind = sTemp.rfind(TEXT("\\"));
		sTemp = sTemp.substr(0, nIndexFind+1);
		sTemp =  sTemp + tstring(PATH_RESOURCE_EXCHANGE_PICTURE);
		if (SUCCEEDED(hr))
		{
			LoadPhoto(sTemp.c_str());//即使失败，也不能影响主要功能
		}
	}

	int nIsXPE = ::GetPrivateProfileInt(L"AVCON_SELECTED", L"AVCONXPE", 0, 
		L"C:\\Program Files\\avcon\\ExePath.ini");
	if (nIsXPE != 0)  //终端机就注册可移动设备
	{
		//定义盘符改变消息的guid，并通过函数向窗体注册
		GUID InterfaceClassGuid = {0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED};
		DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
		ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
		NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
		NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
		NotificationFilter.dbcc_classguid = InterfaceClassGuid;
		m_hDevNotify = RegisterDeviceNotification(m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
	}
	else   //非终端机上注册文件监视
	{
		if (SUCCEEDED(hr))
		{
			static HMODULE hShell = NULL;
			if(hShell == NULL)
			{
				hShell = LoadLibrary(TEXT("Shell32.dll"));
				if (hShell == NULL)
				{
					return S_FALSE;
				}
			}
			pfnSHChangeNotifyRegister pfnRegister = NULL;
			pfnSHChangeNotifyDeregister pfnDeregister = NULL;
			pfnRegister = (pfnSHChangeNotifyRegister)GetProcAddress(hShell,(LPCSTR)MAKEINTRESOURCE(2));
			pfnDeregister = (pfnSHChangeNotifyDeregister)GetProcAddress(hShell,(LPCSTR)MAKEINTRESOURCE(4));
			if (pfnRegister == NULL || pfnDeregister == NULL)
			{
				return S_FALSE;
			}
			SHChangeNotifyEntry shEntry = {0};
			shEntry.fRecursive = TRUE;
			shEntry.pidl = 0;
			int ulNotifyId = 0;
			//注册Shell监视函数
			ulNotifyId = pfnRegister(
				m_hWnd,
				SHCNRF_InterruptLevel|SHCNRF_ShellLevel,
				SHCNE_ALLEVENTS,
				WM_USERDEF_FILECHANGED, //自定义消息
				1,
				&shEntry
				);
		}
	}
	return hr;
}

void CWBResGallery::FreePhotos()
{
	std::list<CWBThumbnail*>::iterator it;
	int nsize = m_lThumbnail->size();
	for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
	{
		CMTObject::RemoveChild(*it);
		(*it)->CleanUp();
	}
	m_lThumbnail->clear();
	CWBThumbnail::ClearAtlas();
}

BOOL CWBResGallery::LoadResFromDirectory(LPCTSTR lpszDirectory)
{
	XAutoLock lock(m_csListChildThumbnail);
	tstring sNewDir = tstring(lpszDirectory);
	tstring sPath = tstring(lpszDirectory);
	if ((!sNewDir.size() || m_strCurDir == sNewDir)&&!m_bResChange)
	{
		OutputDebugString(TEXT("\n资源目录为空或与之前资源相同"));
		return FALSE;
	}
	if (!PathFileExists(lpszDirectory))
	{
		//
		OutputDebugString(TEXT("----文件夹不存在---LoadResFromDirectory(LPCTSTR lpszDirectory)"));
		return FALSE;
	}
	
	FreePhotos();
	//锁定绘制
	//查找符合要求的文件，创建缩略图对象
	RECT rClient;
	GetClientRect(m_hWnd, &rClient);
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	rClient.top += 20;//标题栏的高度
	//图片间的间隔（水平）
	float fpXOffset = WB_TRG_THUMB_INTERNAL_MARGIN;
	//客户端的高度
	float fpCHeight = (float)rClient.bottom - (float)rClient.top;
	//竖直方向的偏移
	float fpYOffset = INTERNAL_Y_MARGIN_UP_PERCENTAGE * fpCHeight;

	float fpPhotoHeight = fpCHeight * (1 - INTERNAL_Y_MARGIN_UP_PERCENTAGE - INTERNAL_Y_MARGIN_DOWN_PERCENTAGE);

	BOOL fContinue = TRUE;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	tstring wstrSearchPath;
	int nDirSize = sNewDir.size();
	//ASSERT(nDirSize>2);
	tstring Temp1 = sNewDir.substr(nDirSize-1,1);
	BOOL bDirHasSlash = FALSE;//目录名称是否也有斜杠
	if ( tstring(TEXT("\\")) == Temp1)
	{
		wstrSearchPath = sNewDir + tstring(TEXT("*.*"));
		bDirHasSlash = TRUE;
	}
	else
	{
		wstrSearchPath = sNewDir + tstring(TEXT("\\")) + tstring(TEXT("*.*"));
	}

	hFind = FindFirstFile(wstrSearchPath.c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		fContinue = FALSE;
	}
	for (int iTimerId = 1; fContinue; iTimerId++)
	{
		if(lpszDirectory == TEXT("/")) break;
		BOOL bSupportFiles = !(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  &&
			!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     &&
			!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     &&
			!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY);
		HRESULT hr = S_OK;
		//进一步判定是否是所支持的文件
		tstring wstrPath;
		if (bDirHasSlash)
		{
			wstrPath = sNewDir + tstring( FindFileData.cFileName);
		}
		else
		{
			wstrPath = sNewDir + tstring(TEXT("\\")) + tstring( FindFileData.cFileName);
		}

		if (bSupportFiles)
		{
			int nFileType = BSupportedResType(ToStr(PathFindExtension(wstrPath.c_str())));
			bSupportFiles = (nFileType>=0);
		}
		if (bSupportFiles)
		{
			//创建对象
			if (m_d2dDriver == NULL)
			{
				hr = E_FAIL;
			}
			CWBThumbnail *pThumbnail = new CWBThumbnail(m_hWnd, 
				m_d2dDriver, fpXOffset, fpYOffset, fpPhotoHeight, 0);
			if (pThumbnail == NULL)
			{
				hr = E_FAIL;
			}
			if (SUCCEEDED(hr))
			{
				hr = pThumbnail->Initialize();
			}
			if (SUCCEEDED(hr))
			{
				pThumbnail->SetFilePath(wstrPath.c_str());
				pThumbnail->m_id = iTimerId;
				pThumbnail->m_isDir = FALSE;
				if (m_strVideoPicturePath.compare(sPath)==0)
				{
					pThumbnail->m_bisVideoFile = TRUE;
					m_bisSpecialResource = TRUE;
				}
			}

			//边界等设定
			if (SUCCEEDED(hr))
			{
				//bounds：限定边界，以判定边界消息的发生（拖动）
				//constraints : 限定实际能移动的位置
				RECT bounds, constraints;
				bounds.left = LONG_MIN;
				bounds.right = LONG_MAX;

				bounds.top = -(fpPhotoHeight/2);
				bounds.bottom = LONG_MAX;
				constraints = bounds;
				constraints.bottom = (LONG)(fpCHeight * INTERNAL_Y_MARGIN_UP_PERCENTAGE );


				//pThumbnail->SetBounds(bounds);
				pThumbnail->SetConstraints(constraints);

				fpXOffset += /*pThumbnail->GetWidth()暂时先用数字，以后统一用宏*/ICON_SIZE + WB_TRG_THUMB_INTERNAL_MARGIN;

				m_lThumbnail->push_back(pThumbnail);
				pThumbnail->AddGesture(TGT_DYNAMIC, (void*)(TGC_PAN));
			}
			if ( FAILED(hr) )
			{
				pThumbnail->CleanUp();
			}
		}
		fContinue = FindNextFile(hFind, &FindFileData);
	}
	
	FindClose(hFind);
	if(lpszDirectory == TEXT("/"))
	{
		int nSize = m_pLstResData->size();
		if (nSize > 0)
		{
			const unsigned int nCurResIndex = GetCurResIndex();
			ResMapArrayConstIter it = m_pLstResData->cbegin();
			for (int nIndex = 0; it != m_pLstResData->cend(); it++, nIndex++)
			{
				HRESULT hr = S_OK;
				if (m_d2dDriver == NULL)
				{
					hr = E_FAIL;
				}
				CWBThumbnail *pThumbnail = new CWBThumbnail(m_hWnd, 
					m_d2dDriver, fpXOffset, fpYOffset, fpPhotoHeight, 0);
				if (pThumbnail == NULL)
				{
					hr = E_FAIL;
				}

				if (SUCCEEDED(hr))
				{
					hr = pThumbnail->Initialize();
				}
				if (SUCCEEDED(hr))
				{
					pThumbnail->SetFilePath((*it)->GetMapPath());
					pThumbnail->SetFileName((*it)->GetMapName());
					pThumbnail->m_id = nIndex+1;
					pThumbnail->m_isDir = TRUE;
				}

				//边界等设定
				if (SUCCEEDED(hr))
				{
					//bounds：限定边界，以判定边界消息的发生（拖动）
					//constraints : 限定实际能移动的位置
					RECT bounds, constraints;
					bounds.left = LONG_MIN;
					bounds.right = LONG_MAX;

					bounds.top = -(fpPhotoHeight/2);
					bounds.bottom = LONG_MAX;
					constraints = bounds;
					constraints.bottom = (LONG)(fpCHeight * INTERNAL_Y_MARGIN_UP_PERCENTAGE );


				//	pThumbnail->SetBounds(bounds);
					pThumbnail->SetConstraints(constraints);

					fpXOffset += /*pThumbnail->GetWidth()暂时先用数字，以后统一用宏*/ICON_SIZE + WB_TRG_THUMB_INTERNAL_MARGIN;
					//AddChild(pThumbnail);
					if (m_strVideoPicturePath.compare((*it)->GetMapPath())==0)    //视频拍照文件夹不能有平移手势
					{
						pThumbnail->m_bisVideoFile = TRUE;
					}
					else
					{
						pThumbnail->AddGesture(TGT_DYNAMIC, (void*)(TGC_PAN));
						pThumbnail->m_bisVideoFile = FALSE;
					}
					pThumbnail->AddGesture(TGT_ACLICK, (void*)(TGC_CLICK|TGC_TAP));
					m_lThumbnail->push_back(pThumbnail);
				}
			}
		}
		//设置添加图片
		{     
			HRESULT hr = S_OK;
			if (m_d2dDriver == NULL)
			{
				hr = E_FAIL;
			}
			CWBThumbnail *pThumbnail = new CWBThumbnail(m_hWnd, 
				m_d2dDriver, fpXOffset, fpYOffset, fpPhotoHeight, 0);
			if (pThumbnail == NULL)
			{
				hr = E_FAIL;
			}

			if (SUCCEEDED(hr))
			{
				hr = pThumbnail->Initialize();
			}
			if (SUCCEEDED(hr))
			{
				TCHAR path[MAX_PATH] = {0};
				pThumbnail->m_id = 0;
				tstring Addbitmap;
				GetModuleFileName(GetModuleHandle(NULL),path,MAX_PATH);
				_tcsrchr(path,TEXT('\\'))[0] = TEXT('\0');
				Addbitmap = path;
				Addbitmap += TEXT("\\SKIN\\WhiteBoard\\add.png");
				pThumbnail->SetFilePath(Addbitmap.c_str());
				pThumbnail->SetFileName(m_lStr.GetLanguageValue(MTRES_ADD,TEXT("添加")));
				pThumbnail->m_isDir = TRUE;
			}

			//边界等设定
			if (SUCCEEDED(hr))
			{
				//bounds：限定边界，以判定边界消息的发生（拖动）
				//constraints : 限定实际能移动的位置
				RECT bounds, constraints;
				bounds.left = LONG_MIN;
				bounds.right = LONG_MAX;

				bounds.top = -(fpPhotoHeight/2);
				bounds.bottom = LONG_MAX;
				constraints = bounds;
				constraints.bottom = (LONG)(fpCHeight * INTERNAL_Y_MARGIN_UP_PERCENTAGE );


				//pThumbnail->SetBounds(bounds);
				pThumbnail->SetConstraints(constraints);

				fpXOffset += pThumbnail->GetWidth() + WB_TRG_THUMB_INTERNAL_MARGIN;

				m_lThumbnail->push_back(pThumbnail);
				pThumbnail->AddGesture(TGT_ACLICK, (void*)(TGC_CLICK|TGC_TAP));
			}
		}
	}
	//重置偏移数据量
	ResetXPosition();
	if (m_mtSpecialClick)
	{
		CMTObject::LayerTop(m_mtSpecialClick);
	}
	//::PostMessage(m_hWnd, UWM_LOAD_COMPLETE_RESOURCE_GALLERY, 0, 0);
	if (m_bisSpecialResource)
	{
		m_fResTotalLen = fpXOffset;
		RECT rc={0};
		::GetClientRect(m_hWnd,&rc);
		if (m_fResTotalLen+ICON_SIZE > rc.right)
		{
			float xoffset = m_fResTotalLen+ICON_SIZE-rc.right;
			TranslateXP(-xoffset);
		}
		m_bisSpecialResource = FALSE;
	}
	InvalidateRect(m_hWnd,NULL,FALSE);   //避免用局部刷新的方式会闪一下时 第三个参数设置为FALSE
	m_strCurDir = sNewDir;
	return TRUE;
}

void CWBResGallery::OnGestureCombineHandle( const GestureCombinePar &gesPar )
{
	if (m_bDrapFlag == FALSE)
	{
		/*if (gesPar.xtrans > PREVENT_DEL_RANGE)
		{
		m_bisDrawRubbishIcon = FALSE;
		}*/
		m_nDrawTipFalg = m_nDrawIntermediateFalg;
		TranslateXP(gesPar.xtrans, gesPar.ytrans);
		InvalidateRect(m_hWnd,NULL,FALSE);
	}
}


void CWBResGallery::Render(BOOL bRenderBackgroundOnly/* = FALSE*/)
{
	XAutoLock lock(m_csListChildThumbnail);
	if (!m_d2dDriver)
	{
		return;
	}

	m_d2dDriver->BeginDraw();
	m_d2dDriver->RenderBackground((float)m_nWidth, (float)m_nHeight);
	if ( !(bRenderBackgroundOnly))
	{
		std::list<CWBThumbnail*>::iterator it;
		for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
		{
			if ( ((*it)->GetXOffset() + (*it)->GetWidth() > m_nXPos) &&
				((*it)->GetXOffset() < m_nWidth + m_nXPos) )
			{
				(*it)->Render();
			}
		}

		if (g_systemVersion >= 6)
		{
			if(m_panel == RESSHOW)
			{
				ID2D1SolidColorBrush *TextBrush = m_d2dDriver->GetColorBrush();
				TextBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Blue));
				RECT rt;
				GetClientRect(m_hWnd,&rt);
				DRAWSTR strCurName = ToDRAWStr(m_strCurName.c_str());
				m_d2dDriver->GetRenderTarget()->DrawText(strCurName.c_str(),
					strCurName.length(),
					m_d2dDriver->GetTextFormat(),
					D2D1::RectF(
					0,
					0,
					rt.right,
					28),
					TextBrush
					);
				TextBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
			}

			if (NULL != m_mtSpecialClick)
			{
				RECT rct = {0};
				m_mtSpecialClick->GetRectRegion(&rct);

				//画切换图片所在竖直方向的背景
				m_d2dDriver->RenderBackground((float)(rct.left), (float)(rct.top), (float)(rct.right), (float)(m_nHeight) );
				//画切换图片
				if (m_panel == RESSHOW&&m_mtSpecialClick)
				{
					if (m_mtSpecialClick->m_pBitmap)
					{
						m_d2dDriver->GetRenderTarget()->DrawBitmap(m_mtSpecialClick->m_pBitmap, 
							D2D1::RectF((float)(rct.left+5), (float)rct.top+5, (float)(rct.left+50), (float)(rct.top+50)),1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
							D2D1::RectF((float)((m_mtSpecialClick->m_State)*50), (float)0, (float)((m_mtSpecialClick->m_State)*50+50), (float)50)
							);
					}
				}
				if (m_nDrawTipFalg !=0)
				{
					ID2D1SolidColorBrush *ChangeBrush = m_d2dDriver->GetColorBrush();
					RenderTip();
					ChangeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
				}
			}
			if (m_bisDrawRubbishIcon)
			{
				m_d2dDriver->RenderRubbishBackground(0.0f,(float)(m_nHeight-30),(float)m_nWidth,(float)m_nHeight);
				DrawRubbishIcon();
			}
		}
		else
		{
			if (m_pResChangeGDIBmp == NULL || m_mtSpecialClick == NULL)
			{
				return;
			}
			float bmpWidth = static_cast<float>(m_pResChangeGDIBmp->GetWidth());
			float bmpHeight = static_cast<float>(m_pResChangeGDIBmp->GetHeight());

			RECT rct = {0};
			m_mtSpecialClick->GetRectRegion(&rct);
			StringFormat format;
			format.SetAlignment(StringAlignmentCenter);
			SolidBrush textBrush(Color::Blue);
			if(m_panel == RESSHOW)
			{
				DRAWSTR strcurname = ToDRAWStr(m_strCurName.c_str());
				m_d2dDriver->GetGraphcis()->DrawString(strcurname.c_str(),strcurname.length(), m_d2dDriver->GetFont(),
					RectF(0,0,rct.right,28),&format, &textBrush);
			}
			//画切换图片所在竖直方向的背景
			m_d2dDriver->RenderBackground(
				static_cast<float>(rct.left),
				0.f,
				static_cast<float>(rct.right),
				static_cast<float>(m_nHeight)
				);

			Rect destion(
				static_cast<float>(m_nWidth - bmpWidth),
				0.f,
				bmpWidth,
				bmpHeight
				);

			if (m_panel == RESSHOW && (m_mtSpecialClick->m_pGdiPlusBmp != NULL))
			{

				m_d2dDriver->GetGraphcis()->DrawImage(m_mtSpecialClick->m_pGdiPlusBmp,
					Rect(rct.left+5,rct.top+5,50,50),
					(m_mtSpecialClick->m_State)*50,0,50,50,UnitPixel);
			}

			if (m_bisDrawRubbishIcon)
			{
				m_d2dDriver->RenderRubbishBackground(0.0f,(float)(m_nHeight-30),(float)m_nWidth,(float)30);
				DrawRubbishIcon();
			}

			//绘制提示信息
			/*if(m_bShowTip)
			{
			RenderTip();
			}*/
		}
	}
	m_d2dDriver->EndDraw();
}

LRESULT CALLBACK CWBResGallery::S_WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	LRESULT result = 0;
	CWBResGallery* pResGallery;
	PAINTSTRUCT ps;
	HDC hdc;
	std::list<CWBThumbnail*>::iterator it;

	pResGallery = reinterpret_cast<CWBResGallery*>(GetWindowLongPtr(hWnd, 0));
	if ( !pResGallery)
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	switch(msg)
	{
// 	case WM_TOUCH:
// 		{
// 			pResGallery->OnTouch(wParam, lParam);
// 		}
// 		break;
	case WM_GESTURE:
		{
			result = S_OK;
		}
		break;
	case WM_ERASEBKGND:
		{
			return true;
		}
		break;
	case UWM_CHANGE_RES:
		{
			CWBThumbnail *thumbnail = (CWBThumbnail*)lParam;
			unsigned int nID = thumbnail->m_id;
			if(nID != 0)
			{
				if (pResGallery->m_hFolder != NULL)
				{
					typedef BOOL (*CLOSEFOLDER)();
					CLOSEFOLDER CloseFolder = (CLOSEFOLDER)GetProcAddress(pResGallery->m_hFolder,"CloseFolder");
					if(CloseFolder != NULL)
					{
						CloseFolder();	
					}
				}
				if(pResGallery->m_panel == RESLIST)
				{
					pResGallery->m_panel = RESSHOW;
					pResGallery->ChangeResTab((unsigned int)(nID-1));
					pResGallery->AdjustState();
				}	
			}
			else
			{
				if (pResGallery->m_hFolder != NULL)
				{
					typedef const wchar_t* (*SHOWFILEUI)(HWND,MTFOLDERCONFIG*);
					SHOWFILEUI showfile = (SHOWFILEUI)GetProcAddress(pResGallery->m_hFolder,"ShowFileUI");
					if(showfile == NULL) break;
					WCHAR pInitPage[MAX_PATH]={0};
					WCHAR pDriveName[MAX_PATH]={0};
					WCHAR pInitPath[MAX_PATH]={0};
					MTFOLDERCONFIG pconfig;
					pResGallery->InitLanPathNode(MTFOLDER_TEXT);
					pResGallery->ConfigFileSelectPlug(pconfig,pInitPage,pDriveName,pInitPath);
					showfile(hWnd,&pconfig);
				}
			}
		}
		break;
	case UWM_FOLERFILE:
		{
			const wchar_t *wResPath = (const wchar_t*)(wParam);
			const wchar_t *wResName = (const wchar_t*)(lParam);
			TCHAR path[MAX_PATH] = {0};
			TCHAR name[MAX_PATH] ={0};
#ifdef UNICODE	
			if(wResPath != NULL) wcscpy_s(path,MAX_PATH,wResPath);
			if(wResName != NULL) wcscpy_s(name,MAX_PATH,wResName);
#else
			if(wResPath != NULL) WideCharToMultiByte( CP_OEMCP,NULL,wResPath,MAX_PATH,path,MAX_PATH,NULL,NULL);		
			if(wResName != NULL) WideCharToMultiByte( CP_OEMCP,NULL,wResName,MAX_PATH,name,MAX_PATH,NULL,NULL);	
#endif
			tstring ResPath = path;
			tstring ResName = name;
			if (PathFileExists(ResPath.c_str()) && ResPath.compare(_T("/"))!=0)
			{   
				//判断该资源是否已经添加 
				BOOL folderIsExisted = FALSE;
				int nIndex = -1;
				//判断是否添加相同的文件夹
				ResMapArrayIter it = pResGallery->m_pLstResData->begin();
				for (int n = 0; it != pResGallery->m_pLstResData->end(); it++, n++)
				{
					if (ResPath == (*it)->GetMapPath())
					{
						folderIsExisted = TRUE;
						nIndex = n;
						break;
					}
				}
				if (folderIsExisted)
				{				
					//该资源文件夹已添加
				}
				else
				{
					//在资源数据中添加一个资源
					SResourceMap *pRes = new SResourceMap;
					pRes->SetMapPath(ResPath.c_str());
					TCHAR Filepath[MAX_PATH] = {0};
					const TCHAR *str = _tcsrchr(ResPath.c_str(),TEXT('\\'));
					if(str != NULL)
					{
						if (ResName.compare(TEXT(""))!=0)
						{
							pRes->SetMapName(ResName.c_str());
						}
						else
						{
							pRes->SetMapName(str+1);
						}
						
					}
					else
					{

						pRes->SetMapName(pResGallery->m_lStr.GetLanguageValue(MTFOLDER_MYRESOURCE,TEXT("我的资源")));
					}
					if (pResGallery->m_bisSpecialResource)     //视频拍照资源排在第一位
					{
						pResGallery->m_pLstResData->push_front(pRes);
						pResGallery->m_bisSpecialResource = FALSE;
					}
					else
					{
						pResGallery->m_pLstResData->push_back(pRes);
					}
					pResGallery->m_bResChange = TRUE;
					pResGallery->m_panel = RESLIST;
					pResGallery->LoadResFromDirectory(TEXT("/"));
				}
			}
		}
		break;
	case WM_LBUTTONUP:
		{
			if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH) && !pResGallery->m_bMTAuthorize) 
			{
				return 0;
			}
			SetCursor(LoadCursor(NULL,IDC_ARROW));
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			::ClientToScreen(pResGallery->GetHWND(), &pt);
			pResGallery->removeMouseCursor(pt.x, pt.y);
			ReleaseCapture();
			pResGallery->m_fIsMouseDown = FALSE;
			pResGallery->m_bDrapFlag = FALSE;
			//pResGallery->ProcessMouse(msg, wParam, lParam);
		}
		break;
	case WM_MOUSEMOVE:
		{
			if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)  && !pResGallery->m_bMTAuthorize) 
			{
				return 0;
			}
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			if ((wParam & MK_LBUTTON) && pResGallery->m_fIsMouseDown)
			{
				::ClientToScreen(pResGallery->GetHWND(), &pt);
				pResGallery->updateMouseCursor(pt.x, pt.y);
				//pResGallery->ProcessMouse(msg, wParam, lParam);
			}

			static POINT pt1={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)};
			if (pt1.x != pt.x && pt1.y != pt.y)
			{
				::SendMessage(pResGallery->m_hWndToolTip,TTM_TRACKACTIVATE, FALSE, (LPARAM) &pResGallery->m_ToolTip); 
				pt1.x = pt.x;
				pt1.y = pt.y;
			}
			TRACKMOUSEEVENT tme = {0};
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_HOVER|TME_LEAVE;
			tme.hwndTrack = pResGallery->m_hWnd;
			tme.dwHoverTime = pResGallery->m_hWndToolTip == NULL ? 400UL : (DWORD) ::SendMessage(pResGallery->m_hWndToolTip, TTM_GETDELAYTIME, TTDT_INITIAL, 0L);
			::TrackMouseEvent(&tme);   
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if( ((GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH)  && !pResGallery->m_bMTAuthorize) 
			{
				return 0;
			}
			SetCursor(LoadCursor(NULL,IDC_HAND));
			SetCapture(hWnd);
			pResGallery->m_fIsMouseDown = TRUE;
			//pResGallery->ProcessMouse(msg, wParam, lParam);
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			::ClientToScreen(pResGallery->GetHWND(), &pt);
			pResGallery->addMouseCursor(pt.x, pt.y);
		}
		break;
	case WM_PAINT:
		{
			hdc = BeginPaint( hWnd, &ps );
			if (pResGallery != NULL)
			{
				pResGallery->Render();
			}
			EndPaint( hWnd, &ps );
		}
		break;
	case WM_DESTROY:
		{
			if(pResGallery->m_bMTAuthorize)
			{
				switch (g_MTPotocol)
				{
				case  PROTOCOL_WINTOUCH_CONNECT:
// 					{
// 						CEncapsulateTouchAPI::U_UnRegisterTouchWindow(hWnd);
// 					}
// 					break;
				case  PROTOCOL_TUIO_CONNECT:
				case  PROTOCOL_HID_CONNECT:
// 					{
// 						AVCON_SDK_MTTouch::RemoveMTObject(pResGallery);
// 					}
 					break;
				default:
					{
						assert(false);
					}
					break;

				}
			}
			if (pResGallery != NULL)
			{
				pResGallery->CleanUp();
			}
			SetWindowLongPtr(hWnd, 0, 0);
			//PostQuitMessage(0);
			//PostMessage(hWnd, WM_DESTROY,0, 0);
			
		}
		break;
	case  WM_COMMAND:
		{
			//获取item对应的
			int nID = LOWORD(wParam) - ID_COMMAND_CHANGE_RES_DIRECTORY_START;
			if(nID>=0)
			{
				//更改
				pResGallery->ChangeResTab((unsigned int)(nID-1));
			}
		}
		break;
	case UWM_LOAD_COMPLETE_RESOURCE_GALLERY://一次线程结束时（即加载完成）
		{
			pResGallery->Render();
		}
		break;
	case WM_SIZE:
		{
			//
			//LPRECT rct;
			//GetClientRect(GetParent(hWnd),rct);
			//m_nWidth = rct->right - rct->left;
		}
		break;	
	case WM_SHOWTIP_RESFIAL:
		{
			POINT curPos;
			::GetCursorPos(&curPos);
			::ScreenToClient(pResGallery->m_hWnd,&curPos);
			pResGallery->m_bDrapFlag = FALSE;
			CDrapEffect::clearWindow();
			PostMessage(pResGallery->m_hWnd,WM_LBUTTONUP,MAKELPARAM(curPos.x,curPos.y),MAKEWPARAM(curPos.x,curPos.y));
		}
		break;
	case UWM_FOLERDEL:
		{
			BOOL bDelete = (BOOL)(wParam);
			g_bShowDlg = (BOOL)(lParam);
			if (bDelete)
			{
				::PostMessage(pResGallery->m_hWnd, UWM_CHANGE_NUME, 0, (LPARAM)((int)1));
			}
		}
		break;
	case UWM_CHANGE_NUME:
		{
			int bNum = (int)lParam;
			if(1 == bNum)
			{
				XAutoLock lock(pResGallery->m_csListChildThumbnail);
				if(RESLIST == pResGallery->m_panel)
				{
					{
						//删除文件夹选中项
						std::list<CWBThumbnail*>::iterator it = pResGallery->m_lThumbnail->begin();
						ResMapArrayIter ResIt = pResGallery->m_pLstResData->begin();
						float xOffSet;
						bool bDelete = false;
						for (; ResIt != pResGallery->m_pLstResData->end();)
						{
							if ((*it)->m_bSelect)
							{
								delete *ResIt;
								ResIt = pResGallery->m_pLstResData->erase(ResIt);
								xOffSet = (*it)->GetXOffset();
								delete *it;
								it = pResGallery->m_lThumbnail->erase(it);
								bDelete = true;
								break;
							}
							else
							{
								ResIt++;
								it++;
							}
						}
						if (bDelete)
						{
							for (;it != pResGallery->m_lThumbnail->end(); it++)
							{
								xOffSet = (*it)->SetXOffsetPos(xOffSet);
								if((*it)->m_id > 0) (*it)->m_id -= 1;
							}
						}
					}
					pResGallery->TranslateXP(0,0);
					InvalidateRect(hWnd,NULL,FALSE);
				}
				else if (RESSHOW == pResGallery->m_panel)
				{
					//删除视频照片文件夹里面的图片
					std::list<CWBThumbnail*>::iterator it = pResGallery->m_lThumbnail->begin();
					float xOffSet;
					bool bDelete = false;
					for (; it != pResGallery->m_lThumbnail->end();)
					{
						if ((*it)->m_bSelect)
						{
							xOffSet = (*it)->GetXOffset();
							delete *it;
							it = pResGallery->m_lThumbnail->erase(it);
							bDelete = true;
							break;
						}
						else
						{
							it++;
						}
					}
					if (bDelete)
					{
						for (;it != pResGallery->m_lThumbnail->end(); it++)
						{
							xOffSet = (*it)->SetXOffsetPos(xOffSet);
							if((*it)->m_id > 0) (*it)->m_id -= 1;
						}
					}
					pResGallery->TranslateXP(0,0);
					InvalidateRect(hWnd,NULL,FALSE);
				}
			}
			else
			{
				if(pResGallery->m_panel == RESSHOW)
				{
					pResGallery->m_panel = RESLIST;
					pResGallery->LoadResFromDirectory(TEXT("/"));
				}
			}
		}
		break;
	case WM_DEVICECHANGE:
		{
			::PostMessage(pResGallery->m_hWnd,WM_USERDEF_FILECHANGED,wParam,lParam);
		}
		break;
	case WM_USERDEF_FILECHANGED:
		{
			if (pResGallery->m_panel == RESSHOW)
			{
				if(!PathFileExists(pResGallery->GetCurResDirectory()))
				{
					pResGallery->m_panel = RESLIST;
					pResGallery->m_bResChange = TRUE;
					pResGallery->LoadResFromDirectory(TEXT("/"));
					pResGallery->m_bResChange = FALSE;
				}
			}
			if (pResGallery->m_panel == RESLIST)
			{
				{
					XAutoLock lock(pResGallery->m_csListChildThumbnail);
					//删除选中项
					std::list<CWBThumbnail*>::iterator it = pResGallery->m_lThumbnail->begin();
					ResMapArrayIter ResIt = pResGallery->m_pLstResData->begin();
					float xOffSet;
					bool bDelete = false;
					for (; ResIt != pResGallery->m_pLstResData->end();)
					{
						if (!PathFileExists((*it)->GetFileURI()))
						{
							delete *ResIt;
							ResIt = pResGallery->m_pLstResData->erase(ResIt);
							xOffSet = (*it)->GetXOffset();
							delete *it;
							it = pResGallery->m_lThumbnail->erase(it);
							bDelete = true;
							break;
						}
						else
						{
							ResIt++;
							it++;		
						}
					}
					if (bDelete)
					{
						for (;it != pResGallery->m_lThumbnail->end(); it++)
						{
							xOffSet = (*it)->SetXOffsetPos(xOffSet);
						}
					}
				}
				InvalidateRect(hWnd,NULL,FALSE);
			}
		}
		break;
	case WM_MOUSEHOVER:
		{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			for ( std::list<CWBThumbnail*>::iterator it =pResGallery->m_lThumbnail->begin();
				pResGallery->m_lThumbnail->end() != it; ++it)
			{
				if ((*it)->BInRegion(pt) &&(*it)->m_id!=0)
				{
					RECT rc;
					rc.left = (*it)->GetThumbPosition().x;
					rc.top = (*it)->GetThumbPosition().y;
					rc.right = ICON_SIZE;
					rc.bottom = ICON_SIZE;
					memset(&pResGallery->m_ToolTip, 0, sizeof(TOOLINFO));
#if _WIN32_WINNT>0x0500
					pResGallery->m_ToolTip.cbSize = sizeof(TOOLINFO)-sizeof(void*);
#else
					pResGallery->m_ToolTip.cbSize = sizeof(TOOLINFO);
#endif
					pResGallery->m_ToolTip.uFlags = TTF_SUBCLASS;
					pResGallery->m_ToolTip.hwnd = hWnd;
					pResGallery->m_ToolTip.hinst = pResGallery->m_ResHinstance;
					pResGallery->m_ToolTip.lpszText = (LPTSTR)(*it)->GetThumbName();
					pResGallery->m_ToolTip.rect = rc;
					pResGallery->CreateTipWnd(hWnd,pResGallery->m_ToolTip);
					break;
				}
			}

			RECT rc;
			GetClientRect(hWnd, &rc);   //由于图片的大小和定义的宏不匹配故用数字来作调整(切换给按钮加提示)
			rc.left = rc.right - CLICK_ZONE_WIDTH+5;
			rc.bottom = rc.top + CLICK_ZONE_HEIGHT-10;
			rc.right = rc.left+CLICK_ZONE_WIDTH-15;
			if(PtInRect(&rc, pt) && pResGallery->m_panel == RESSHOW)
			{
				rc.bottom  = CLICK_ZONE_HEIGHT;
				rc.right = CLICK_ZONE_WIDTH;
				memset(&pResGallery->m_ToolTip, 0, sizeof(TOOLINFO));
#if _WIN32_WINNT>0x0500
				pResGallery->m_ToolTip.cbSize = sizeof(TOOLINFO)-sizeof(void*);
#else
				pResGallery->m_ToolTip.cbSize = sizeof(TOOLINFO);
#endif
				pResGallery->m_ToolTip.uFlags = TTF_SUBCLASS;
				pResGallery->m_ToolTip.hwnd = hWnd;
				pResGallery->m_ToolTip.hinst = pResGallery->m_ResHinstance;
				pResGallery->m_ToolTip.lpszText = (LPWSTR)(pResGallery->m_lStr.GetLanguageValue(MTRES_SWITCH_RES,TEXT("切换到资源库")));
				pResGallery->m_ToolTip.rect = rc;
				pResGallery->CreateTipWnd(hWnd,pResGallery->m_ToolTip);
			}
		}
		break;
		case WM_MOUSELEAVE:
		{
			if (pResGallery->m_hWndToolTip !=NULL)
			{
				::SendMessage(pResGallery->m_hWndToolTip, TTM_TRACKACTIVATE, FALSE, (LPARAM) &pResGallery->m_ToolTip);
			}
		}
		break;
		
	default:
		result = DefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return result;
}


HRESULT CWBResGallery::CreateThumbGallery( HINSTANCE hInstance, HWND hParent, 
	int x, int y, int nWidth, int nHeight,
	CWBResGallery **ppStrip, ResTypeCallbackFun pResCallback, BOOL bAuthorize)
{
	static BOOL s_fRegistered = FALSE;
	HRESULT hr = S_OK;
	(*ppStrip) = NULL;
	HWND hWnd = 0;

#ifdef _DEBUG
	HINSTANCE ReshInstance= GetModuleHandle(TEXT("MTResd"));
#else
	HINSTANCE ReshInstance = GetModuleHandle(TEXT("MTRes"));
#endif
	if (!s_fRegistered)
	{
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(wcex));
		wcex.cbSize = sizeof( WNDCLASSEX );
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = CWBResGallery::S_WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = ReshInstance;
		wcex.hCursor = LoadCursor( NULL, IDC_ARROW );
		wcex.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = RESGALLERY_CLASS_NAME;

		hr = RegisterClassEx( &wcex );
		if (SUCCEEDED( hr ))
		{
			s_fRegistered = TRUE;
		}
	}
	if( SUCCEEDED( hr ) ) 
	{
		//创建窗口
		hWnd = CreateWindowEx(0,RESGALLERY_CLASS_NAME, TEXT("ThumbnailGallery"), 
			WS_CHILD | WS_BORDER| WS_VISIBLE /*| WS_CLIPCHILDREN*/ | WS_CLIPSIBLINGS, x, y, 
			nWidth, nHeight, hParent, NULL,ReshInstance, 0);
		if (!hWnd)
		{
			hr = E_FAIL;
		}
		else
		{
			DisableTheTabFeedback(hWnd);
		}
	}

	if( SUCCEEDED( hr ) )
	{
		(*ppStrip) = new CWBResGallery(hWnd, nWidth, nHeight, 0, 0, pResCallback);
		if ((*ppStrip) == NULL)
		{
			hr = E_FAIL;
		}
	}
	//窗口类关联，并初始化缩略图
	if ( SUCCEEDED( hr ) )
	{
		SetWindowLongPtr(hWnd, 0, (LONG_PTR)(*ppStrip));
		hr = (*ppStrip)->Initialize();
		if ( FAILED(hr) )
		{
			SetWindowLongPtr(hWnd, 0, NULL);
			(*ppStrip)->CleanUp();
			hr = E_FAIL;
		}
		else
		{
			(*ppStrip)->InitialResData();
			(*ppStrip)->SetMTAuthorize(bAuthorize);
			(*ppStrip)->AddGesture(TGT_DYNAMIC, (void *)(TGC_PAN|TGC_INERTIAL));
			SNotifyType notifyType = {true, true, false};
			(*ppStrip)->ConfigNotifyType(notifyType);
		}

		//语言配置
		CWBResGallery* pResGallery;
		pResGallery = reinterpret_cast<CWBResGallery*>(GetWindowLongPtr(hWnd, 0));
		if (pResGallery->m_hFolder == NULL)
		{
#ifdef _DEBUG
			pResGallery->m_hFolder = LoadLibrary(TEXT("MTFolder.dll"));
#else
			pResGallery->m_hFolder = LoadLibrary(TEXT("MTFolder.dll"));
#endif
		}
		typedef const wchar_t* (*INILANGUAGE)(wchar_t*,wchar_t*);
		INILANGUAGE iniLanguage = (INILANGUAGE)GetProcAddress(pResGallery->m_hFolder,"IniLanguage");
		WCHAR iniPath[MAX_PATH]={0};
		wcscpy_s(iniPath,pResGallery->m_lanPath.c_str());
		iniLanguage(iniPath,MTFOLDER_TEXT);
	}
	return hr;
}

const HWND CWBResGallery::GetHWND()const
{
	return m_hWnd;
}

LPCTSTR CWBResGallery::GetCurResDirectory()
{
	return m_strCurDir.c_str();
}

unsigned int  CWBResGallery::GetCurResIndex() const
{
	return m_nCurResIndex;
}

void CWBResGallery::ChangeResTab( unsigned int nResItemIndex )
{
	tstring strNewDir;
	tstring strNewName;
	if(nResItemIndex >= m_pLstResData->size())
	{
		return;
	}
	else
	{
		int nIndex = 0;
		for (ResMapArrayIter it = m_pLstResData->begin(); it != m_pLstResData->end(); it++)
		{
			if (nIndex == nResItemIndex)
			{
				strNewDir = tstring((*it)->GetMapPath());
				strNewName = tstring((*it)->GetMapName());
				break;
			}
			nIndex++;
		}
	}
	if (!strNewDir.empty())
	{
		if (LoadResFromDirectory(strNewDir.c_str()))
		{
			m_nCurResIndex = nResItemIndex;
			m_strCurName = strNewName;
		}
	}
}

void CWBResGallery::InitialResData()
{
	//创建各个标签页
	ResMapArrayIter it = m_pLstResData->begin();
	tstring strPath;
	if (it != m_pLstResData->end())
	{
		for (unsigned int nIndex = 0; it != m_pLstResData->end(); it++, nIndex++)
		{
			if (m_nCurResIndex == nIndex)
			{
				LoadResFromDirectory((*it)->GetMapPath());
				tstring strNewName = (*it)->GetMapName();
				m_strCurName = strNewName;
				break;
			}
		}
	}
	else
	{
		m_panel = RESLIST;
		m_bResChange = TRUE;
		LoadResFromDirectory(TEXT("/"));//请添加资源
	}

	//创建触摸点击对象的位置
	if (!m_mtSpecialClick)
	{
		CreateSpecialClickObject();
	}

}

HRESULT CWBResGallery::LoadPhoto( LPCTSTR const uri )
{
	HRESULT hr = S_OK;
	if (g_systemVersion >= 6)
	{
		ID2D1DCRenderTarget*  spRT = NULL;
		if (m_d2dDriver)
		{
			spRT = m_d2dDriver->GetRenderTarget();
		}
		if (!m_d2dDriver || !spRT)
		{
			return S_FALSE;
		}

		IWICBitmapDecoder *pDecoder = NULL;
		IWICBitmapFrameDecode *pSource = NULL;
		IWICStream *pStream = NULL;
		IWICFormatConverter *pConverter = NULL;
		IWICBitmapScaler *pScaler = NULL;
		IWICImagingFactory *pFactory = NULL;
		UINT originalWidth = 0, originalHeight = 0;
		//m_strFile = std::wstring( uri );

		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pFactory)
			);

		// Create the initial frame from our desired image
		if (SUCCEEDED(hr))
		{
			hr = pFactory->CreateDecoderFromFilename(
				ToDRAWStr(uri).c_str(),
				NULL,
				GENERIC_READ,
				WICDecodeMetadataCacheOnLoad,
				&pDecoder
				);
		}
		if (SUCCEEDED(hr))
		{
			hr = pDecoder->GetFrame(0, &pSource);
		}

		//prepare to scale the bitmap to our desired height, width determined by the 
		//original aspect ratio
		if (SUCCEEDED(hr))
		{
			hr = pSource->GetSize(&originalWidth, &originalHeight);
		}
		//定义宽度
		int nWidth = CLICK_ZONE_HEIGHT;
		int nHeight = CLICK_ZONE_HEIGHT;
		if (SUCCEEDED(hr))
		{
			FLOAT scalar = (FLOAT)(nHeight) / (FLOAT)originalHeight;
			nWidth = scalar * originalWidth;
			hr = pFactory->CreateBitmapScaler(&pScaler);
		}
		if (SUCCEEDED(hr))
		{
			hr = pScaler->Initialize(
				pSource,
				static_cast<UINT>(nWidth),
				static_cast<UINT>(nHeight),
				WICBitmapInterpolationModeCubic
				);
		}

		//create the format converter
		if (SUCCEEDED(hr))
		{
			hr = pFactory->CreateFormatConverter(&pConverter);
		}
		if (SUCCEEDED(hr))
		{
			hr = pConverter->Initialize(
				pScaler,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.0f,
				WICBitmapPaletteTypeMedianCut
				);
		}

		//create a Direct2D bitmap from the WIC bitmap
		if (SUCCEEDED(hr))
		{
			hr = spRT->CreateBitmapFromWicBitmap(
				pConverter,
				NULL,
				&m_pResChangeBitmap
				);
		}

		SafeRelease(&pFactory);
		SafeRelease(&pDecoder);
		SafeRelease(&pSource);
		SafeRelease(&pStream);
		SafeRelease(&pConverter);
		SafeRelease(&pScaler);
		if (m_pResChangeBitmap)
		{
			InvalidateRect(m_hWnd,NULL,FALSE);
		}
	}
	else
	{
		m_pResChangeGDIBmp = Bitmap::FromFile(ToDRAWStr(uri).c_str());

		if (m_pResChangeGDIBmp == NULL)
		{
			return S_FALSE;
		}
		InvalidateRect(m_hWnd,NULL,FALSE);
	}
	return hr;
}

void CWBResGallery::CreateSpecialClickObject()
{
	if (m_mtSpecialClick)
	{
		return;
	}
	//暂时是整个高度区都能响应，以便于触摸
	CWBTouchClick *pTemp = new CWBTouchClick(m_hWnd, m_pLstResData);
	if (pTemp)
	{
		m_mtSpecialClick = pTemp;
		m_mtSpecialClick->AddGesture(TGT_ACLICK, (void *)(TGC_TAP | TGC_CLICK));
		CMTObject::AddChild(m_mtSpecialClick);
		CMTObject::LayerTop(m_mtSpecialClick);

		TCHAR path[MAX_PATH] = {0};
		GetModuleFileName(GetModuleHandle(NULL),path,MAX_PATH);
		_tcsrchr(path,TEXT('\\'))[0] = TEXT('\0');
		tstring resbitmap = path;
		resbitmap += TEXT("\\SKIN\\WhiteBoard\\ResourceManage.png");
		Bitmap *pWrapBitmap = Bitmap::FromFile(ToDRAWStr(resbitmap.c_str()).c_str());//Bitmap::FromHBITMAP(hBmp, NULL);
		HBITMAP hbmp = NULL;
		if (pWrapBitmap != NULL)
		{
			BitmapData bitmapData;
			Rect rcImage(0,0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			pWrapBitmap->LockBits(&rcImage, ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 
			Bitmap *pGdiPlusBmp = new Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride, 
				PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);
			pGdiPlusBmp->GetHBITMAP(Gdiplus::Color(0,0,0,0),&hbmp);
			m_mtSpecialClick->m_pGdiPlusBmp = pGdiPlusBmp;
		}

		if ((g_systemVersion >= 6) && hbmp)
		{
			BITMAP bitmap;
			GetObject(hbmp, sizeof(BITMAP), &bitmap);
			DWORD dwSize = bitmap.bmHeight * bitmap.bmWidthBytes;
			unsigned char* pBits = new unsigned char[dwSize];
			LONG dl = GetBitmapBits(hbmp, dwSize, pBits);

			ID2D1Bitmap *pBitmap = NULL;
			m_d2dDriver->GetRenderTarget()->CreateBitmap(
				D2D1::SizeU(bitmap.bmWidth,bitmap.bmHeight),
				D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)),
				&pBitmap
				);
			pBitmap->CopyFromMemory(
				&D2D1::RectU(0,0, bitmap.bmWidth,bitmap.bmHeight),
				pBits,
				bitmap.bmWidthBytes
				);
			m_mtSpecialClick->m_pBitmap = pBitmap;
			::DeleteObject(hbmp);
			delete []pBits;
		}
	}
}

void CWBResGallery::UpdateResource()
{
	if(m_panel == RESLIST)
	{
		m_bResChange = TRUE;
		LoadResFromDirectory(TEXT("/"));
		m_bResChange = FALSE;
		return;
	}

	ResMapArrayIter it = m_pLstResData->begin();

	BOOL bGetCurrent = FALSE;//如存在当前的目录路径，则不出路，否则更新

	if(m_strCurDir.empty())
	{
		return;
	}

	while(it != m_pLstResData->end())
	{
		if ( (*it)->GetMapPath() == m_strCurDir )
		{
			bGetCurrent = TRUE;
			break;
		}
		it++;
	}
	if (!bGetCurrent)//如果没有找到,则清空当前显示的资源视图
	{
		if(m_panel == RESSHOW)
		{
			m_panel = RESLIST;
			LoadResFromDirectory(TEXT("/"));
		}
	}
}

void CWBResGallery::ResetXPosition()
{
	if (m_hWnd)
	{
		m_fpXOffset = 0;
		RECT rct;
		::GetClientRect(m_hWnd, &rct);
		m_nWidth = rct.right - rct.left;
		m_nHeight = rct.bottom - rct.top;
	}
}

void CWBResGallery::ResetYPosition()
{
	std::list<CWBThumbnail*>::iterator it;
	for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
	{
		(*it)->SetYOffsetPos(INTERNAL_Y_MARGIN_UP_PERCENTAGE * m_nHeight);
	}
}

void CWBResGallery::UpdateResSize( unsigned int nNewWidth, unsigned int nNewHieght)
{
	HWND  hWnd = GetHWND();
	if (!hWnd)
		return;

	m_nWidth = nNewWidth;
	m_nHeight = nNewHieght;
	//更新点击点的位置

	if (m_d2dDriver)
	{
		if (g_systemVersion >= 6)
		{
			ID2D1DCRenderTarget* pRender = m_d2dDriver->GetRenderTarget();
			if (pRender)
			{
				HDC dc = ::GetDC(hWnd);
				RECT rect;
				::GetClientRect(m_hWnd, &rect);
				pRender->BindDC(dc,&rect);
			}
		}
		else
		{
			m_d2dDriver->UpdateResWndSize();
		}
	}
}

void CWBResGallery::GetResSize( unsigned int &nNewWidth, unsigned int &nNewHieght )
{
	nNewWidth = m_nWidth;
	nNewHieght = m_nHeight;
}

HRESULT CWBResGallery::TranslateXP( int xOffset /*= 0 */, int yOffset /*= 0*/ )
{
	HRESULT hr = S_OK;

	//
	float fpTotalWidth = WB_TRG_THUMB_INTERNAL_MARGIN, fpNewXOffset;
	std::list<CWBThumbnail*>::iterator it;
	for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
	{
		fpTotalWidth += ICON_SIZE + WB_TRG_THUMB_INTERNAL_MARGIN;
	}

	fpNewXOffset = m_fpXOffset + xOffset;

	float fpXUpperBound = m_nWidth*0.3;//最大右移距离（非负数）
	float fpXLowerBound = min(-fpTotalWidth + m_nWidth*0.7, 0);//最大左移距离（非正数）


	fpNewXOffset = max(fpXLowerBound, fpNewXOffset);
	fpNewXOffset = min(fpXUpperBound, fpNewXOffset);

	xOffset = fpNewXOffset - m_fpXOffset;
	m_fpXOffset = fpNewXOffset;

	if ( 0.0 != xOffset )
	{
		for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
		{
			(*it)->Translate(xOffset, 0);
		}
	}

	return hr;
}

bool CWBResGallery::BInRegion( const POINT &pt ) const
{
	return (::WindowFromPoint(pt) == GetHWND())?(true):(false);
}


int CWBResGallery::GetResType( const std::string &_strExt )
{
	int type = -1;
	//解析

	std::string strExt = std::string(_strExt);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);
	if ( (strExt == ".bmp") || (strExt == ".jpg") || (strExt == ".gif") || (strExt == ".png") || (strExt == ".dib") 
		|| (strExt == ".tif") || (strExt == ".jfif") || (strExt == ".ico") || (strExt == ".jpeg")||(strExt == ".tiff") )
	{
		type = WBRES_PICTURE;
	}
	else if ((strExt == ".doc") || (strExt == ".ppt") || (strExt == ".xls") || (strExt == ".pdf") 
		||(strExt == ".docx") ||(strExt == ".xlsx") || (strExt == ".pptx"))
	{
		type = WBRES_DOC;
	}
	else if ((strExt == ".avi") || (strExt == ".wmv") || (strExt == ".mpeg") || (strExt == ".mpg"))
	{
		type = WBRES_MOVIE;
	}
	else if ((strExt == ".swf") )
	{
		type = WBRES_FLASH;
	}

	return type;
}

void CWBResGallery::addMTCursor( MTCursor *tcur )
{
	if (g_bAvconLock || m_bDrapFlag) 
		return;

	XAutoLock lock(m_csMapChildObject);
	const DWORD dwCursorID = tcur->getCursorID();

	///当前状态
	POINT pt;
	pt.x = tcur->getX();
	pt.y = tcur->getY();
	::ScreenToClient(GetHWND(), &pt);

	for ( std::list<CWBThumbnail*>::iterator it = m_lThumbnail->begin();
		m_lThumbnail->end() != it; ++it)
	{
		if ((*it)->BInRegion(pt) && AddCurorMap(dwCursorID, *it))
		{
			(*it)->addMTCursor(tcur);
			break;
		}
	}
	CMTObject::addMTCursor(tcur);
}

void CWBResGallery::removeMTCursor( MTCursor *tcur )
{
	XAutoLock lock(m_csMapChildObject);

	std::map<DWORD, CMTObject*>::iterator it = m_pMapChildObject->find(tcur->getCursorID());
	if (m_pMapChildObject->end() != it)
	{

		it->second->removeMTCursor(tcur);
		RemoveCurorMap(tcur->getCursorID());
	}

	CMTObject::removeMTCursor(tcur);

	if (m_pMapChildObject->empty())
	{
		std::list<CWBThumbnail*>::iterator it;
		for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
		{
			(*it)->ResetYOffSet();
		}
		InvalidateRect(m_hWnd,NULL,FALSE);
	}

}

void CWBResGallery::updateMTCursor( MTCursor *tcur )
{
	if (g_bAvconLock /*|| m_bDrapFlag*/) 
		return;

	XAutoLock lock(m_csMapChildObject);

	std::map<DWORD, CMTObject*>::iterator it = m_pMapChildObject->find(tcur->getCursorID());
	if (m_pMapChildObject->end() != it)
	{
		if (m_bDrapFlag == FALSE)
		{
			it->second->updateMTCursor(tcur);
		}
	}
	CMTObject::updateMTCursor(tcur);
}

bool CWBResGallery::AddCurorMap( const unsigned long &dID, CMTObject *pObject )
{
	std::map<unsigned long, CMTObject*>::iterator it = m_pMapChildObject->find(dID);
	if (m_pMapChildObject->end() == it)
	{
		m_pMapChildObject->insert(std::pair<unsigned long, CMTObject*>(dID, pObject));
		return true;
	}
	else
	{
#ifdef _DEBUG
		assert(false);
#endif
		m_pMapChildObject->erase(it);
		return false;
	}
}

bool CWBResGallery::RemoveCurorMap( const unsigned long &dID )
{
	std::map<unsigned long, CMTObject*>::const_iterator it = m_pMapChildObject->find(dID);
	if (m_pMapChildObject->end() != it)
	{
		m_pMapChildObject->erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

bool CWBResGallery::RegisterReceivedWnd( const HWND &hWnd )
{
	if (hWnd && IsWindow(hWnd))
	{
		m_lstReceiveDragWnd.push_front(hWnd);
		return true;
	}
	return false;
}

bool CWBResGallery::UnRegisterReceivedWnd( const HWND &hWnd )
{
	for (std::list<HWND>::const_iterator it = m_lstReceiveDragWnd.begin();
		m_lstReceiveDragWnd.end() != it; ++it)
	{
		if (hWnd == *it)
		{
			m_lstReceiveDragWnd.erase(it);
			return true;
		}
	}
	return false;
}

bool CWBResGallery::BInReceiveDragWndList( const HWND &hWnd )
{
	for (std::list<HWND>::const_iterator it = m_lstReceiveDragWnd.begin();
		m_lstReceiveDragWnd.end() != it; ++it)
	{
		if (hWnd == *it)
		{
			return true;
		}
	}
	return false;
}

BOOL CWBResGallery::SetMTAuthorize( BOOL bMTAuthorize)
{
	BOOL bNSame = ((!m_bMTAuthorize) && bMTAuthorize)||(m_bMTAuthorize && (!bMTAuthorize));
	if(bNSame)
	{
/*		//获取触摸数据的协议类型
		g_MTPotocol = AVCON_SDK_MTTouch::GetConnectProtocol();

		OSVERSIONINFOEX os; 
		os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
		if(GetVersionEx((OSVERSIONINFO *)&os))                  
		{
			//如果是win8系统强制使用系统触摸
			if((os.dwMajorVersion>6) || ((os.dwMajorVersion==6) && (os.dwMinorVersion>=2)))
				g_MTPotocol = PROTOCOL_WINTOUCH_CONNECT;
		}
*/
		g_MTPotocol = PROTOCOL_WINTOUCH_CONNECT;
		switch (g_MTPotocol)
		{
		case  PROTOCOL_WINTOUCH_CONNECT:
// 			{
// 				if(bMTAuthorize)
// 				{
// 					CEncapsulateTouchAPI::U_RegisterTouchWindow(m_hWnd);
// 				}
// 				else
// 				{
// 					CEncapsulateTouchAPI::U_UnRegisterTouchWindow(m_hWnd);
// 				}
// 			}
// 			break;
		case  PROTOCOL_TUIO_CONNECT:
		case  PROTOCOL_HID_CONNECT:
// 			{
// 				if(bMTAuthorize)
// 				{
// 					AVCON_SDK_MTTouch::AddMTObject(this, true);
// 				}
// 				else
// 				{
// 					AVCON_SDK_MTTouch::RemoveMTObject(this);
// 				}
// 			}
 			break;
		default:
			{
				assert(false);
			}
			break;
		}
		m_bMTAuthorize = bMTAuthorize;
	}
	return bNSame;
}

void CWBResGallery::RenderTip()
{
	if(!m_nDrawTipFalg || (((m_panel == RESSHOW)&&((m_nDrawTipFalg&0x0F)==0))||((m_panel == RESLIST)&&((m_nDrawTipFalg&0x30)==0)))) return;
	if ((m_panel == RESLIST)&&((m_nDrawTipFalg&ADD_TIP)==0)&&(m_lThumbnail->size() == 1))return;
	DarwTipBackground();
	TCHAR path[MAX_PATH] = {0};
	tstring tipbitmap;
	HBITMAP hBitmap = NULL;
	GetModuleFileName(GetModuleHandle(NULL),path,MAX_PATH);
	_tcsrchr(path,TEXT('\\'))[0] = TEXT('\0');
	tipbitmap = path;
	tipbitmap += TEXT("\\skin\\WhiteBoard\\tip.png");
	IWICBitmapSource *pBit= NULL;
	hBitmap = CWBThumbnail::LoadPictureFromFile(tipbitmap.c_str(),m_d2dDriver,0,&pBit);
	if (hBitmap == NULL) return ;
	if (pBit != NULL)
	{
		ID2D1Bitmap *pBitmap = NULL;
		m_d2dDriver->GetRenderTarget()->CreateBitmapFromWicBitmap(pBit,&pBitmap);
		SafeRelease(&pBit);
		RECT rc;
		::GetClientRect(m_hWnd,&rc);
		if (pBitmap != NULL)
		{
			ID2D1DCRenderTarget* pRendTarget = NULL;
			pRendTarget = m_d2dDriver->GetRenderTarget();
			if(pRendTarget != NULL)
			{
				//为了是提示均匀分布，故作此计算，设间隔InterVal变量
				float fInterVal = (float)(rc.right-450-WB_SINGLETIP_WIDTH-WB_SINGLETIP_HEIGHT)/3;
				if (m_panel == RESSHOW)
				{
					if (LEFT_SLIDE & GetDrawTipFlag())   //向左滑动资源
					{
						pRendTarget->DrawBitmap(pBitmap,
							D2D1::RectF((float)100,(float)(rc.bottom-WB_SINGLETIP_HEIGHT)/2, (float)100+WB_SINGLETIP_WIDTH,(float)((rc.bottom-WB_SINGLETIP_HEIGHT)/2+WB_SINGLETIP_HEIGHT)),
							1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
							D2D1::RectF((float)15,(float)30, (float)165,(float)130)
							);//指定大小
						m_nDrawIntermediateFalg &= (~LEFT_SLIDE);
					}
					

					if (UP_DRAG & GetDrawTipFlag())     //向上拖拽资源
					{
						
						pRendTarget->DrawBitmap(pBitmap,
							D2D1::RectF((float)fInterVal+250,(float)(rc.bottom-WB_CHANGE_LIST_WIDTH)/2, (float)350+fInterVal,(float)((rc.bottom-WB_CHANGE_LIST_WIDTH)/2+WB_CHANGE_LIST_WIDTH)),
							1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
							D2D1::RectF((float)178,(float)5, (float)(278),(float)155)
							);//指定大小
						m_nDrawIntermediateFalg &= (~UP_DRAG);
					}
					if (RIGHT_SLIDE & GetDrawTipFlag())   //向右滑动资源
					{
						pRendTarget->DrawBitmap(pBitmap,
							D2D1::RectF((float)(350+fInterVal*2),(float)(rc.bottom-WB_SINGLETIP_HEIGHT)/2, (float)(500+fInterVal*2),(float)((rc.bottom-WB_SINGLETIP_HEIGHT)/2+WB_SINGLETIP_HEIGHT)),
							1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
							D2D1::RectF((float)300,(float)30, (float)450,(float)130)
							);//指定大小
						m_nDrawIntermediateFalg &= (~RIGHT_SLIDE);
					}
					if (CHANGE_LIST & GetDrawTipFlag())    //点击进入资源目录切换面板
					{
						pRendTarget->DrawBitmap(pBitmap,
							D2D1::RectF((float)rc.right-45-WB_CHANGE_LIST_WIDTH,(float)(rc.bottom-WB_CHANGE_LIST_HEIGHT)/2, (float)rc.right-45,(float)(rc.bottom-WB_CHANGE_LIST_HEIGHT)/2+WB_CHANGE_LIST_HEIGHT),
							1.0,
							D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
							D2D1::RectF((float)565,(float)35, (float)715,(float)120)
							);//指定大小
						m_nDrawIntermediateFalg &= (~CHANGE_LIST);
					}
				}
				else if (m_panel == RESLIST)
				{
					if (m_lThumbnail->size() == 1)
					{
						if (ADD_TIP & GetDrawTipFlag())    //添加按钮提示
						{
							std::list<CWBThumbnail*>::iterator it;
							int dis = 0;
							for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
							{
								dis+= (*it)->GetWidth()+WB_TRG_THUMB_INTERNAL_MARGIN;
							}
							pRendTarget->DrawBitmap(pBitmap,
								D2D1::RectF((float)dis-10,(float)60, (float)dis+140,(float)150),
								1.0,
								D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
								D2D1::RectF((float)720,(float)30, (float)870,(float)120)
								);//指定大小
							m_nDrawIntermediateFalg &= (~ADD_TIP);
						}
					}
					else
					{
						if (UP_DEL & GetDrawTipFlag())    //向上拖拽删除资源
						{
							pRendTarget->DrawBitmap(pBitmap,
								D2D1::RectF((float)30,(float)10, (float)125,(float)160),
								1.0,
								D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
								D2D1::RectF((float)460,(float)0, (float)555,(float)150)
								);//指定大小
							m_nDrawIntermediateFalg &= (~UP_DEL);
						}
						{//添加按钮提示
							std::list<CWBThumbnail*>::iterator it;
							int dis = 0;
							for (it = m_lThumbnail->begin(); it != m_lThumbnail->end(); it++)
							{
								dis+= (*it)->GetWidth()+WB_TRG_THUMB_INTERNAL_MARGIN;
							}
							pRendTarget->DrawBitmap(pBitmap,
								D2D1::RectF((float)dis-10,(float)60, (float)dis+140,(float)150),
								1.0,
								D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
								D2D1::RectF((float)720,(float)30, (float)870,(float)120)
								);//指定大小
							m_nDrawIntermediateFalg &= (~ADD_TIP);
						}
					}
				}
			}
		}
	}
}

// LRESULT CWBResGallery::OnTouch( WPARAM wParam, LPARAM lParam )
// {
// 	int iNumContacts = LOWORD(wParam);
// 	HTOUCHINPUT hInput = (HTOUCHINPUT)lParam;
// 
// #ifdef _DEBUG
// 	PTOUCHINPUT pInputs = new TOUCHINPUT[iNumContacts];
// #else
// 	PTOUCHINPUT pInputs = new (std::nothrow) TOUCHINPUT[iNumContacts];
// #endif
// 
// 
// 	if(pInputs != NULL)
// 	{
// 		//获取每个触摸点信息，然后分发到各个处理容器中。
// 		if( CEncapsulateTouchAPI::U_GetTouchInputInfo(lParam, iNumContacts, pInputs, sizeof(TOUCHINPUT)) )
// 		{
// 			for(int i = 0; i < iNumContacts; i++)
// 			{
// 				//////转换成客户区坐标
// 				POINT ptInputs;
// 
// 				ptInputs.x = TOUCH_COORD_TO_PIXEL(pInputs[i].x);
// 				ptInputs.y = TOUCH_COORD_TO_PIXEL(pInputs[i].y);
// 				pInputs[i].x = ptInputs.x;
// 				pInputs[i].y = ptInputs.y;
// 
// 				if ((pInputs[i].dwFlags & TOUCHEVENTF_DOWN) == TOUCHEVENTF_DOWN)// 触摸按下事件
// 				{
// 						std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
// 						if (m_mapTouchCursor.end() != it)
// 						{
// 							delete it->second;
// 							m_mapTouchCursor.erase(it);
// 						}
// 						static long session_id = 1;
// 						MTCursor *pCursor = new MTCursor(MTTime::getSessionTime(),++session_id, pInputs[i].dwID, pInputs[i].x, pInputs[i].y);
// 						m_mapTouchCursor.insert(std::pair<DWORD, MTCursor*>(pInputs[i].dwID, pCursor));
// 						addMTCursor(pCursor);
// 					
// 				}               
// 				else if ((pInputs[i].dwFlags & TOUCHEVENTF_MOVE) == TOUCHEVENTF_MOVE)// 触摸移动事件
// 				{
// 					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
// 					if (m_mapTouchCursor.end() != it)
// 					{
// 						it->second->update(MTTime::getSessionTime(),pInputs[i].x, pInputs[i].y);
// 						updateMTCursor(it->second); 
// 					}
// 				}               
// 				else if ((pInputs[i].dwFlags & TOUCHEVENTF_UP) == TOUCHEVENTF_UP)// 触摸弹起事件
// 				{
// 					std::map<DWORD, MTCursor*>::iterator it = m_mapTouchCursor.find(pInputs[i].dwID);
// 					if (m_mapTouchCursor.end() != it)
// 					{
// 						it->second->update(MTTime::getSessionTime(),pInputs[i].x, pInputs[i].y);
// 						removeMTCursor(it->second);
// 						delete it->second;
// 						m_mapTouchCursor.erase(it);
// 					}
// 				}
// 			}
// 		}
// 		if (!CEncapsulateTouchAPI::U_CloseTouchInputHandle(lParam))
// 		{
// 			OutputDebugString(TEXT("warning, the touch input handle was not closed successfully!"));
// 		}
// 	}
// 
// 	delete [] pInputs;
// 	return S_OK;
// }

void CWBResGallery::ConfigFileSelectPlug( MTFOLDERCONFIG &pconfig,wchar_t *pInitPage,wchar_t *pDriveName,wchar_t*pInitPath)
{
	//分屏是获取当前屏坐标
	POINT pt;
	::GetCursorPos(&pt);
	MONITORINFO oMonitor = {}; 
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	RECT rcWork = oMonitor.rcWork;
	int   nScreenWidth = rcWork.right - rcWork.left; 
	int   nScreenHeight = rcWork.bottom - rcWork.top;
	int   nX = rcWork.left + (nScreenWidth-370)/2;
	int   nY = rcWork.top + (nScreenHeight-470)/2;
	pconfig.x = nX;
	pconfig.y = nY;
	pconfig.WndWidth = 370;
	pconfig.WndHeight = 470;
	int nIsXPE = ::GetPrivateProfileInt(L"AVCON_SELECTED", L"AVCONXPE", 0, 
		L"C:\\Program Files\\avcon\\ExePath.ini");
	DRAWSTR configInitPage = L"";
	DRAWSTR configDriveName = L"";

	if (nIsXPE != 0)     //终端
	{
		pconfig.DiscConfig = DRIVECONFIG_REMOVABLE;
		DRAWSTR myresource = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_MYRESOURCE,TEXT("我的资源")));
		DRAWSTR removabledisk = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_REMOVABLE_DISK,TEXT("可移动磁盘")));
		configInitPage+=_T("/<");
		configInitPage+=myresource;
		configInitPage+=_T(">");
		wcscpy_s(pInitPage,MAX_PATH,configInitPage.c_str());
		wcscpy_s(pInitPath,MAX_PATH,myresource.c_str());
		wcscpy_s(pDriveName,MAX_PATH,removabledisk.c_str());
		pconfig.InitPage = pInitPage;
		pconfig.InitPath = pInitPath;
		configDriveName+=_T("<");
		configDriveName+=pDriveName;
		configDriveName+=_T(">");
		wcscpy_s(pDriveName,MAX_PATH,configDriveName.c_str()); 
		pconfig.DriveName = pDriveName;
	}
	else                
	{
		DRAWSTR mycomputer = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_MYCOMPUTER,TEXT("我的电脑")));
		DRAWSTR mydocument = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_MYDOCUMENT,TEXT("我的文档")));
		DRAWSTR desktop = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_DESKTOP,TEXT("桌面")));
		DRAWSTR systemdisk = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_SYSTEM_DISK,TEXT("系统磁盘")));
		DRAWSTR localdisk = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_LOCAL_DISK,TEXT("本地磁盘")));
		DRAWSTR removabledisk = ToDRAWStr(m_lStr.GetLanguageValue(MTFOLDER_REMOVABLE_DISK,TEXT("可移动磁盘")));
		TCHAR MyDocument[MAX_PATH] = {0};
		SHGetSpecialFolderPath(GetHWND(),MyDocument,CSIDL_PERSONAL,0); //我的文档
		TCHAR MyDeskTop[MAX_PATH] = {0};
		SHGetSpecialFolderPath(GetHWND(),MyDeskTop,CSIDL_DESKTOP,0); //桌面
		configInitPage+=_T("/<");
		configInitPage+=mycomputer;
		configInitPage+=_T(">");
		configInitPage+=MyDocument;
		configInitPage+=_T("<");
		configInitPage+=mydocument;
		configInitPage+=_T(">");
		configInitPage+=MyDeskTop;
		configInitPage+=_T("<");
		configInitPage+=desktop;
		configInitPage+=_T(">");
		wcscpy_s(pInitPage,MAX_PATH,configInitPage.c_str());        //路径<标签名>路径<标签名>...）
		pconfig.InitPage = pInitPage;
		configDriveName+=_T("<");
		configDriveName+=systemdisk;
		configDriveName+=_T("()><");
		configDriveName+=localdisk;
		configDriveName+=_T("()><");
		configDriveName+=removabledisk;
		configDriveName+=_T("()>");
		wcscpy_s(pDriveName,MAX_PATH,configDriveName.c_str());     //驱动器别名（格式:<系统磁盘()><本地磁盘()><可移动磁盘(0)>）别名后面加()表示显示驱动器符号,加(0)则所有别名都在其后自动加序号
		pconfig.DriveName = pDriveName;
	}
}

void CWBResGallery::InitLanPathNode(tstring node)
{
	//int nIsXPE = ::GetPrivateProfileInt(L"AVCON_SELECTED", L"AVCONXPE", 0, 
	//	L"C:\\Program Files\\avcon\\ExePath.ini");
	//TCHAR strKeyValue[MAX_PATH];
	//TCHAR szLanguageChoosePath[MAX_PATH] = {0};
	//::GetModuleFileName(NULL,szLanguageChoosePath,MAX_PATH);
	//(_tcsrchr(szLanguageChoosePath,TEXT('\\')))[0]=TEXT('\0');
	//m_lanPath = szLanguageChoosePath;
	//_tcscat_s(szLanguageChoosePath,TEXT("\\U7Config.ini"));
	//m_lanPath += TEXT("\\config\\");

	//if (nIsXPE!=0) //终端
	//{
	//	tstring pePath = _T("D:\\AVCON7_XPE\\U7Config.ini");
	//	_tcscpy(szLanguageChoosePath,pePath.c_str());
	//}
	TCHAR strKeyValue[MAX_PATH];
	tstring iniPath = CResManage::GetInstance()->GetAvconIniPath();
	tstring szLanguageChoosePath;
	m_lanPath = iniPath+TEXT("\\");
	szLanguageChoosePath = iniPath + TEXT("\\U7Config.ini");
	GetPrivateProfileString(TEXT("LANGUAGE"),TEXT("Language"),TEXT("cs"),strKeyValue,MAX_PATH,szLanguageChoosePath.c_str());
	//m_lanPath+=strKeyValue;
	TCHAR szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH); 
	_tcsrchr(szPath,TEXT('\\'))[0] = TEXT('\0');	
	tstring strpathname = szPath;
	strpathname += TEXT("\\config\\");
	strpathname += strKeyValue;
	m_lanPath = strpathname;
	m_lanPath += TEXT("_WBConfig.ini");
	m_lStr.SetLanguagePath(m_lanPath);
	m_lStr.SetNodeStr(node);
	m_lStr.InitLanguageStr();
}

 DWORD CWBResGallery::GetDrawTipFlag() const
{
	return m_nDrawTipFalg;
}

void CWBResGallery::ConfigDrawTipFlag( DWORD nflag )
{
	m_nDrawTipFalg = nflag&(LEFT_SLIDE|UP_DRAG|RIGHT_SLIDE|UP_DEL|CHANGE_LIST|ADD_TIP);
	
}

void CWBResGallery::DarwTipBackground()
{
	RECT rct = {0};
	m_mtSpecialClick->GetRectRegion(&rct);
	ID2D1SolidColorBrush *ChangeBrush = m_d2dDriver->GetColorBrush();
	ChangeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	ChangeBrush->SetOpacity(0.7);
	m_d2dDriver->GetRenderTarget()->FillRectangle(D2D1::RectF((float)0, (float)0, (float)rct.right, (float)3*rct.bottom),ChangeBrush);
	ChangeBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
	ChangeBrush->SetOpacity(1.0);
}

void CWBResGallery::CreateTipWnd(HWND hWnd, TOOLINFO toolTip )
{
	if( !IsWindow( m_hWndToolTip ) )
	{
		m_hWndToolTip = ::CreateWindowEx(WS_EX_TOPMOST,
			TOOLTIPS_CLASS, 
			NULL,
			WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			hWnd,
			NULL,
			m_ResHinstance,
			NULL);
		SendMessage(m_hWndToolTip,TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &toolTip);
	}
	::SendMessage(m_hWndToolTip, TTM_SETTOOLINFO, 0, (LPARAM) &toolTip);    //更新信息
	::SendMessage(m_hWndToolTip,TTM_TRACKACTIVATE, TRUE, (LPARAM) &toolTip); //显示窗口
	SendMessage(m_hWndToolTip,TTM_SETMAXTIPWIDTH,0,(LPARAM)(INT)(ICON_SIZE+WB_TRG_THUMB_INTERNAL_MARGIN)); //设置提示窗口的最大宽度
}

PANEL CWBResGallery::AdjustState()
{
	if ( m_strCurDir == _T("/") )
	{
		m_panel = RESLIST;
	}
	else
	{
		m_panel = RESSHOW;
	}
	return m_panel;
}

void CWBResGallery::DrawRubbishIcon()
{
	TCHAR path[MAX_PATH] = {0};
	tstring rubbishIconPath;
	HBITMAP hbitmap = NULL;
	GetModuleFileName(GetModuleHandle(NULL),path,MAX_PATH);
	_tcsrchr(path,TEXT('\\'))[0] = TEXT('\0');
	rubbishIconPath = path;
	rubbishIconPath += TEXT("\\skin\\WhiteBoard\\Rubbish.png");
	RECT rc;
	::GetClientRect(m_hWnd,&rc);
	int startX = (rc.right/2)-15;
	int startY = rc.bottom-30;
	if (g_systemVersion>=6)
	{
		IWICBitmapSource *pBit = NULL;
		hbitmap = CWBThumbnail::LoadPictureFromFile(rubbishIconPath.c_str(),m_d2dDriver,0,&pBit);
		if (hbitmap == NULL)
		{
			return;
		}
		if (pBit != NULL)
		{
			ID2D1Bitmap *pBitmap = NULL;
			m_d2dDriver->GetRenderTarget()->CreateBitmapFromWicBitmap(pBit,&pBitmap);
			SafeRelease(&pBit);
			if (pBitmap != NULL)
			{
				m_d2dDriver->GetRenderTarget()->DrawBitmap(pBitmap,D2D1::RectF(startX, startY, startX+30, startY+30)
					, 1.0
					, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
					, NULL);
			}
		}
	}
	else
	{
		Bitmap *pWrapBitmap = NULL;
		pWrapBitmap = Bitmap::FromFile(rubbishIconPath.c_str());
		if (pWrapBitmap != NULL)
		{
			/*BitmapData bitmapData;
			Rect rcImage(0,0, pWrapBitmap->GetWidth(), pWrapBitmap->GetHeight());
			pWrapBitmap->LockBits(&rcImage, ImageLockModeRead, pWrapBitmap->GetPixelFormat(), &bitmapData); 
			Bitmap *pGdiPlusBmp = new Bitmap(bitmapData.Width, bitmapData.Height, bitmapData.Stride, 
				PixelFormat32bppARGB, (BYTE*)bitmapData.Scan0);*/

		m_d2dDriver->GetGraphcis()->DrawImage(pWrapBitmap,Rect(startX,startY,30,30)
		,0,0,pWrapBitmap->GetWidth(),pWrapBitmap->GetHeight(),UnitPixel);
		}
	}
}

void CWBResGallery::SwitchVideoPictureDirectory( LPCTSTR strPath )
{
	if (::PathFileExists(strPath))
	{
		m_strVideoPicturePath = strPath;
		tstring sFileName;
		int nTag = m_strVideoPicturePath.rfind(TEXT('\\'));
		if (nTag>=0)
		{
			sFileName = m_strVideoPicturePath.substr(nTag+1, m_strVideoPicturePath.size()-nTag-1);//文件名
			m_strCurName = sFileName;
		}
		m_bisSpecialResource = TRUE;
		::SendMessage(m_hWnd,UWM_FOLERFILE,(WPARAM)(strPath),(LPARAM)(_T("视频拍照")));
		m_panel = RESSHOW;
		m_bResChange = TRUE;
		LoadResFromDirectory(strPath);
	}
}

void CWBResGallery::InitSpecialResPath()
{
	int nIsXPE = ::GetPrivateProfileInt(L"AVCON_SELECTED", L"AVCONXPE", 0, 
		L"C:\\Program Files\\avcon\\ExePath.ini");
	TCHAR specialResPath[MAX_PATH] = {0};
	if (nIsXPE!=0) //终端
	{
		tstring pePath = _T("D:\\AVCON7_XPE\\视频拍照");
		_tcscpy(specialResPath,pePath.c_str());
		m_strVideoPicturePath = specialResPath;
	}
	else
	{
		::GetModuleFileName(NULL,specialResPath,MAX_PATH);
		(_tcsrchr(specialResPath,TEXT('\\')))[0]=TEXT('\0');
		m_strVideoPicturePath = specialResPath;
		m_strVideoPicturePath += TEXT("\\user\\视频拍照");
	}
}


//copy from 标准
void DisableTheTabFeedback(HWND hWnd)
{
	DWORD_PTR dwHwndTabletProperty = 
		TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
		TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
		TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
		TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)

	ATOM atom = GlobalAddAtom(TEXT("MicrosoftTabletPenServiceProperty"));  
	SetProp(hWnd, TEXT("MicrosoftTabletPenServiceProperty"), reinterpret_cast<HANDLE>(dwHwndTabletProperty));
	GlobalDeleteAtom(atom);
}
