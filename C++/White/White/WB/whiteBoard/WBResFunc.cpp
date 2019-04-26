#include "stdafx.h"
#include "WBResFunc.h"
#include "ResData.h"
#include <string>
#include <algorithm>

typedef int (__cdecl *ResTypeCallbackFun)(const std::string &_strExt);

static HINSTANCE g_hResModule = NULL;

int WBRESWND::GetResType(const std::string &_strExt)
{
	int type = -1;
	//解析

	std::string strExt = std::string(_strExt);
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower);
	if ( (strExt == ".bmp") || (strExt == ".jpg") || (strExt == ".gif") || (strExt == ".png") || (strExt == ".dib")||(strExt == ".jpe") 
		|| (strExt == ".tif") || (strExt == ".jfif") || (strExt == ".ico") || (strExt == ".jpeg")||(strExt == ".tiff") )
	{
		type = WBRES_PICTURE;
	}
	else if ((strExt == ".doc") || (strExt == ".ppt") || (strExt == ".xls") || (strExt == ".pdf")
				||(strExt == ".docx") ||(strExt == ".xlsx") || (strExt == ".pptx"))
	{
		type = WBRES_DOC;
		type = -1;//暂不需要
	}
	else if ((strExt == ".avi") || (strExt == ".wmv") || (strExt == ".mpeg") || (strExt == ".mpg")
				||(strExt == ".asf") || (strExt == ".mov") || (strExt == ".dat") || (strExt == ".rmvb")
				||(strExt == ".vob") || (strExt == ".mp4") || (strExt == ".f4v") || (strExt == ".mp3")
			 	||(strExt == ".wm") || (strExt == ".wma") || (strExt == ".rm") || (strExt == ".mts")||(strExt == ".mkv"))
	{
		type = WBRES_MOVIE;
		type = -1; //暂不需要
	}
	else if ((strExt == ".swf") )
	{
		type = WBRES_FLASH;
	}
	else if ((strExt == ".dxf"))
	{
		type = WBRES_DXF;
		type = -1; //暂不需要
	}
	return type;
}

BOOL WBRESWND::InitWBResModule()
{
	BOOL bReturnValue = FALSE;
	if (NULL == g_hResModule)
	{
		//确保动态加载的库路径正确
		HMODULE hModule = GetModuleHandle(TEXT("whiteboard.dll"));
		if (!hModule) return FALSE;

		TCHAR strName[MAX_PATH];
		GetModuleFileName(hModule, strName, MAX_PATH);
		CString strPath = strName;
		int nPos = strPath.ReverseFind('\\');
		if(-1 == nPos) nPos = strPath.ReverseFind('/');
		if(-1 == nPos) return FALSE;
		strPath = strPath.Left(nPos);
		SetCurrentDirectory(strPath.GetBuffer());
		CString strResPath = _T("");
#ifdef _DEBUG
		strResPath.Format(TEXT("%s\\MTResd.dll"), strPath);
#else 
		strResPath.Format(TEXT("%s\\MTRes.dll"), strPath);
#endif
		g_hResModule = LoadLibrary(strResPath.GetBuffer());
		strPath.ReleaseBuffer();
		strResPath.ReleaseBuffer();
		if (NULL != g_hResModule)
		{
			bReturnValue =  TRUE;
		}

	}
	return bReturnValue;
}

void WBRESWND::UnInitWBResModule()
{
	if (g_hResModule)
	{
		CloseResWnd();
		FreeLibrary(g_hResModule);
		g_hResModule = NULL;
	}
}


HRESULT WBRESWND::CreateGalleryWndEx( DWORD version, HINSTANCE hInstance, HWND hParent, int x, int y, int nWidth, int nHeight, BOOL bAuthorize )
{
	HRESULT hr = S_FALSE;
	if (NULL != g_hResModule)
	{
		typedef	HRESULT (__cdecl *WBRESFUN)(DWORD, HINSTANCE, HWND, int, int, int, int, ResTypeCallbackFun, BOOL);
		WBRESFUN pCreateGalleryWnd = (WBRESFUN)GetProcAddress(g_hResModule, "CreateGalleryWnd");

		if (NULL != pCreateGalleryWnd)
		{
 			hr = (*pCreateGalleryWnd)(version, hInstance, hParent, x, y, nWidth, nHeight,WBRESWND::GetResType, bAuthorize);
		}
	}

	return hr;
}

BOOL WBRESWND::MoveGalleryWndEx( HWND hWndInsertAfter, int x, int y, int nWidth, int nHeight, UINT uFlags )
{
	if (NULL != g_hResModule)
	{
		typedef	BOOL (__cdecl *WBRESFUN)(HWND hWndInsertAfter, int x, int y, int nWidth, int nHeight, UINT uFlags);
		WBRESFUN MoveGalleryWnd = (WBRESFUN)GetProcAddress(g_hResModule, "MoveGalleryWnd");

		if (NULL != MoveGalleryWnd)
		{
			return (*MoveGalleryWnd)(hWndInsertAfter, x, y, nWidth, nHeight, uFlags);
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

int WBRESWND::ShowGalleryWndEx()
{
	if (NULL != g_hResModule)
	{
		typedef	int (__cdecl *WBRESFUN)();

		//函数检索指定的动态链接库(DLL)中的输出库函数地址
		WBRESFUN ShowGalleryWnd = (WBRESFUN)GetProcAddress(g_hResModule, "ShowGalleryWnd");

		if (NULL != ShowGalleryWnd)
		{
			return (*ShowGalleryWnd)();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}


int WBRESWND::HideGalleryWndEx()
{
	if (NULL != g_hResModule)
	{
		typedef	int (__cdecl *WBRESFUN)();
		WBRESFUN ShowGalleryWnd = (WBRESFUN)GetProcAddress(g_hResModule, "HideGalleryWnd");

		if (NULL != ShowGalleryWnd)
		{
			return (*ShowGalleryWnd)();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void WBRESWND::SwitchSpecialDirectoryEx( LPCTSTR strPath )
{
	if (NULL != g_hResModule)
	{
		typedef void (__cdecl *WBRESFUN)(LPCTSTR);
		WBRESFUN pSwitchSpecialDirectory = (WBRESFUN)GetProcAddress(g_hResModule, "SwitchSpecialDirectory");
		if (NULL != pSwitchSpecialDirectory)
		{
			(*pSwitchSpecialDirectory)(strPath);
		}
	}
}

void WBRESWND::SetWBResHWndEx(HWND hParent)
{
	if (NULL != g_hResModule)
	{
		typedef void (__cdecl *WBRESFUN)(HWND);
		WBRESFUN pSetWBResHWnd = (WBRESFUN)GetProcAddress(g_hResModule, "SetWBResHWnd");
		if (NULL != pSetWBResHWnd)
		{
			(*pSetWBResHWnd)(hParent);
		}
	}
}


void WBRESWND::UpdateResGalleryEx()
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)();
		WBRESFUN pUpdateResGallery = (WBRESFUN)GetProcAddress(g_hResModule, "UpdateResGallery");

		if (NULL != pUpdateResGallery)
		{
			(*pUpdateResGallery)();
		}
	}
}

int WBRESWND::GetResTypeEx( CString url )
{
	if (NULL != g_hResModule)
	{
		typedef	int (__cdecl *WBRESFUN)(CString url);
		WBRESFUN GetResType = (WBRESFUN)GetProcAddress(g_hResModule, "GetResType");

		if (NULL != GetResType)
		{
			return (*GetResType)(url);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

void WBRESWND::RemoveResPathEx( LPCTSTR strPath )
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)(LPCTSTR, BOOL);
		WBRESFUN pRemoveResPathEx = (WBRESFUN)GetProcAddress(g_hResModule, "AddResDataItem");

		if (NULL != pRemoveResPathEx)
		{
			(*pRemoveResPathEx)(strPath, FALSE);
		}
	}
}

void WBRESWND::AddResPathEx( LPCTSTR strPath )
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)(LPCTSTR, BOOL);
		WBRESFUN pAddResPathEx = (WBRESFUN)GetProcAddress(g_hResModule, "AddResDataItem");

		if (NULL != pAddResPathEx)
		{
			(*pAddResPathEx)(strPath, TRUE);
		}
	}
}

BOOL WBRESWND::IsGalleryWndCreated()
{
	BOOL bCreated = FALSE;
	if (NULL != g_hResModule)
	{
		typedef	BOOL (__cdecl *WBRESFUN)();
		WBRESFUN pIsGalleryWndCreated = (WBRESFUN)GetProcAddress(g_hResModule, "IsGalleryWndCreated");

		if (NULL != pIsGalleryWndCreated)
		{
			bCreated = (*pIsGalleryWndCreated)();
		}
	}
	return bCreated;
}

BOOL WBRESWND::IsGalleryWndShow()
{
	BOOL bWndShow = FALSE;
	if (NULL != g_hResModule)
	{
		typedef	BOOL (__cdecl *WBRESFUN)();
		WBRESFUN pIsGalleryWndShow = (WBRESFUN)GetProcAddress(g_hResModule, "IsGalleryWndShow");

		if (NULL != pIsGalleryWndShow)
		{
			bWndShow = (*pIsGalleryWndShow)();
		}
	}
	return bWndShow;
}

void WBRESWND::CloseResWnd()
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)();
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule,"CloseResGallery");

		if (NULL != pCloseResWnd)
		{
			(*pCloseResWnd)();
		}
	}
}

unsigned int WBRESWND::GetCurSelectResIndex()
{
	unsigned int nIndex = 0;
	if (NULL != g_hResModule)
	{
		typedef	unsigned int (__cdecl *WBRESFUN)();
		WBRESFUN pGetCurSelectResIndex = (WBRESFUN)GetProcAddress(g_hResModule, "GetCurSelectResIndex");

		if (NULL != pGetCurSelectResIndex)
		{
			nIndex = (*pGetCurSelectResIndex)();
		}
	}
	return nIndex;
}

void WBRESWND::SetMTAuthorizeRes( bool bMTAuthorize )
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)(bool);
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "SetMTAuthorizeRes");

		if (NULL != pCloseResWnd)
		{
			(*pCloseResWnd)(bMTAuthorize);
		}
	}
}
// void WBRESWND::SetSupportU7OpenFile(BOOL bSupportU7OpenFile)
// {
// 	if (NULL != g_hResModule)
// 	{
// 		typedef	void (__cdecl *WBRESFUN)(BOOL);
// 		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "SetSupportU7OpenFile");
// 		if (NULL != pCloseResWnd)
// 		{
// 			(*pCloseResWnd)(bSupportU7OpenFile);
// 		}
// 		
// 	}
// }
void WBRESWND::SetAvconIniPath(const LPCTSTR strAvconIniPath)
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)(LPCTSTR);
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "SetAvconIniPath");
		if (NULL != pCloseResWnd)
		{
			(*pCloseResWnd)(strAvconIniPath);
		}

	}
}

bool WBRESWND::RegisterReceivedWnd(const HWND &hWnd)
{
	if (NULL != g_hResModule)
	{
		typedef	bool (__cdecl *WBRESFUN)(const HWND &);
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "RegisterReceivedWnd");
		if (NULL != pCloseResWnd)
		{
			return (*pCloseResWnd)(hWnd);
		}
	}
	return false;
}

bool WBRESWND::UnRegisterReceivedWnd(const HWND &hWnd)
{
	if (NULL != g_hResModule)
	{
		typedef	bool (__cdecl *WBRESFUN)(const HWND &);
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "UnRegisterReceivedWnd");
		if (NULL != pCloseResWnd)
		{
			return (*pCloseResWnd)(hWnd);
		}
	}
	return false;
}

void WBRESWND::SetAvconLock( BOOL bAvconLock )
{
	if (NULL != g_hResModule)
	{
		typedef	void (__cdecl *WBRESFUN)(BOOL);
		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "SetAvconLock");
		if (NULL != pCloseResWnd)
		{
			(*pCloseResWnd)(bAvconLock);
		}

	}
}


// void WBRESWND::LoadResFromDirectoryWB( LPCTSTR DirectoryWB )
// {
// 	if (NULL != g_hResModule)
// 	{
// 		typedef	void (__cdecl *WBRESFUN)(LPCTSTR);
// 		WBRESFUN pCloseResWnd = (WBRESFUN)GetProcAddress(g_hResModule, "LoadResFromDirectoryWB");
// 		if (NULL != pCloseResWnd)
// 		{
// 			(*pCloseResWnd)(DirectoryWB);
// 		}
// 
// 	}
// }
