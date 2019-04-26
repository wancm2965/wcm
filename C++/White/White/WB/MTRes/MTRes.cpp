// MTRes.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
//#include <afxwin.h>
#include "MTRes.h"


ULONG_PTR m_gdiplusToken;

//创建触摸资源窗体
extern "C" LIB_AVCON_RESOURCE_DECL HRESULT CreateGalleryWnd(DWORD version, HINSTANCE hInstance, HWND hParent,
	int x, int y, int nWidth, int nHeight, ResTypeCallbackFun pResCallback, BOOL bAuthorize)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bReturnValue = S_FALSE;

	static BOOL bCreated = FALSE;

	g_systemVersion = version;
	if (!bCreated)
	{
		bReturnValue = CWBResGallery::CreateThumbGallery(hInstance, hParent, x, y, nWidth, nHeight, 
			&g_pResGallery, pResCallback, bAuthorize);
	}
	if (bReturnValue && (g_systemVersion<6))
	{
		GdiplusStartupInput gdiplusStartInput;
		GdiplusStartup(&m_gdiplusToken, &gdiplusStartInput, NULL);
	}
	return bReturnValue;
}

//移动资源窗体
extern "C" LIB_AVCON_RESOURCE_DECL BOOL MoveGalleryWnd( HWND hWndInsertAfter,
	int x, int y, int nWidth, int nHeight, UINT uFlags)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	BOOL bReturnValue = FALSE;
	if (g_pResGallery)
	{
		bReturnValue = ::SetWindowPos(g_pResGallery->GetHWND(), hWndInsertAfter, x, y, nWidth, nHeight, uFlags);
		if (nWidth && nHeight)
		{
			g_pResGallery->UpdateResSize(nWidth, nHeight);//与D2D绘图相关
		}
	}
	return bReturnValue;
}


extern "C" LIB_AVCON_RESOURCE_DECL int ShowGalleryWnd()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int bReturnValue = -1;
	if (g_pResGallery)
	{
		if (::IsWindowVisible(g_pResGallery->GetHWND()))
		{
			if (g_pResGallery->m_hFolder != NULL)
			{
				typedef BOOL (*CLOSEFOLDER)();
				CLOSEFOLDER CloseFolder = (CLOSEFOLDER)GetProcAddress(g_pResGallery->m_hFolder,"CloseFolder");
				if(CloseFolder != NULL)
				{
					CloseFolder();
				}
			}
			bReturnValue = ::ShowWindow(g_pResGallery->GetHWND(), SW_HIDE);
		}
		else
		{
			bReturnValue = ::ShowWindow(g_pResGallery->GetHWND(), SW_SHOW);
		}
	}
	return bReturnValue;
}


extern "C" LIB_AVCON_RESOURCE_DECL int HideGalleryWnd()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int bReturnValue = -1;
	if (g_pResGallery)
	{
		if (::IsWindowVisible(g_pResGallery->GetHWND()))
		{
			if (g_pResGallery->m_hFolder != NULL)
			{
				typedef BOOL (*CLOSEFOLDER)();
				CLOSEFOLDER CloseFolder = (CLOSEFOLDER)GetProcAddress(g_pResGallery->m_hFolder,"CloseFolder");
				if(CloseFolder != NULL)
				{
					CloseFolder();
				}
			}
			bReturnValue = ::ShowWindow(g_pResGallery->GetHWND(), SW_HIDE);
		}

		if (IsWindow(g_pResGallery->GetHWndToolTip()))   //隐藏资源条时删除提示窗口
		{
			::DestroyWindow(g_pResGallery->GetHWndToolTip());
		}
	}
	return bReturnValue;
}

extern "C" LIB_AVCON_RESOURCE_DECL void SwitchSpecialDirectory( LPCTSTR strPath )
{
	if (g_pResGallery)
	{
		g_pResGallery->SwitchVideoPictureDirectory(strPath);
	}
}

extern "C" LIB_AVCON_RESOURCE_DECL void SetWBResHWnd( HWND hParent )
{
	if (g_pResGallery)
	{
		g_pResGallery->SetWBResHWnd(hParent);
	}
}


extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndCreated()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int bReturnValue = FALSE;
	if (g_pResGallery)
	{
		bReturnValue = TRUE;
	}
	return bReturnValue;
}

extern "C" LIB_AVCON_RESOURCE_DECL BOOL IsGalleryWndShow()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	int bReturnValue = FALSE;
	if (g_pResGallery)
	{
		bReturnValue = ::IsWindowVisible(g_pResGallery->GetHWND());
	}
	return bReturnValue;
}

extern "C" LIB_AVCON_RESOURCE_DECL void UpdateResGallery()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pResGallery)
	{
		g_pResGallery->UpdateResource();
	}
}

//
//STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	return AfxDllGetClassObject(rclsid, riid, ppv);
//}
//
//
//// DllCanUnloadNow - 允许 COM 卸载 DLL
//
//STDAPI DllCanUnloadNow(void)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	return AfxDllCanUnloadNow();
//}
//
//
//// DllRegisterServer - 将项添加到系统注册表
//
//STDAPI DllRegisterServer(void)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), GUID_LIB_AVCON_RESOURCE_DECL))
//		return SELFREG_E_TYPELIB;
//
//	if (!COleObjectFactory::UpdateRegistryAll())
//		return SELFREG_E_CLASS;
//
//	return S_OK;
//}
//
//
//// DllUnregisterServer - 将项从系统注册表中移除
//
//STDAPI DllUnregisterServer(void)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//	if (!AfxOleUnregisterTypeLib(GUID_LIB_AVCON_RESOURCE_DECL, 1, 0))
//		return SELFREG_E_TYPELIB;
//
//	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
//		return SELFREG_E_CLASS;
//
//	return S_OK;
//}

extern "C" LIB_AVCON_RESOURCE_DECL void CloseResGallery()
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pResGallery)
	{
		if (g_pResGallery->m_hFolder != NULL)
		{
			typedef BOOL (*CLOSEFOLDER)();
			CLOSEFOLDER CloseFolder = (CLOSEFOLDER)GetProcAddress(g_pResGallery->m_hFolder,"CloseFolder");
			if(CloseFolder != NULL)
			{
				CloseFolder();
			}
			typedef CLOSEFOLDER CLOSEDIALOG;
			CLOSEDIALOG CloseDialog = (CLOSEFOLDER)GetProcAddress(g_pResGallery->m_hFolder,"CloseDialogUI");
			if (CloseDialog != NULL)
			{
				CloseDialog();
			}
		}
		CDrapEffect::clearWindow();
		::DestroyWindow(g_pResGallery->GetHWND());
		g_pResGallery = NULL;
		if (g_systemVersion<6)
		{
			//GdiplusShutdown(m_gdiplusToken);
		}
	}
}

extern "C" LIB_AVCON_RESOURCE_DECL unsigned int GetCurSelectResIndex()
{
	unsigned int nIndex = 0;
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pResGallery)
	{
		nIndex = g_pResGallery->GetCurResIndex();
	}
	return nIndex;
}

extern "C" LIB_AVCON_RESOURCE_DECL void SetMTAuthorizeRes(BOOL MT)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (g_pResGallery)
	{
		g_pResGallery->SetMTAuthorize(MT);
	}
}
// extern "C" LIB_AVCON_RESOURCE_DECL void SetSupportU7OpenFile(BOOL bSupportU7OpenFile)
// {
// 	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
// 	g_bU7OpenFile = bSupportU7OpenFile;
// }
extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconIniPath(const LPCTSTR strAvconIniPath)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CResManage::GetInstance()->SetAvconIniPath(ToTStr(strAvconIniPath).c_str());
}

extern "C" LIB_AVCON_RESOURCE_DECL bool RegisterReceivedWnd( const HWND &hWnd )
{
	if (g_pResGallery)
	{
		return g_pResGallery->RegisterReceivedWnd(hWnd);
	}
	return false;
}

extern "C" LIB_AVCON_RESOURCE_DECL bool UnRegisterReceivedWnd( const HWND &hWnd )
{
	if (g_pResGallery)
	{
		return g_pResGallery->UnRegisterReceivedWnd(hWnd);
	}
	return false;
}

extern "C" LIB_AVCON_RESOURCE_DECL void SetAvconLock( BOOL bAvconLock )
{
	g_bAvconLock = bAvconLock;
	if (!bAvconLock)
	{
		//清空手势库中的残余手势
	}
}




//extern "C" LIB_AVCON_RESOURCE_DECL void LoadResFromDirectoryWB(const LPCTSTR trAvconTempPath)
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	if (g_pResGallery)
//	{
//		 g_pResGallery->LoadResFromDirectory(trAvconTempPath);
//	}
//	
//}

extern CWBResGallery *g_pResGallery = NULL;
extern DWORD g_systemVersion = 4;
extern BOOL g_bAvconLock = FALSE;



