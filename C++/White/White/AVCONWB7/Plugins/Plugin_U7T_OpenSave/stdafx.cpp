// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_U7T_OpenSave.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

#include <io.h>
#include <direct.h>
#include <atlimage.h>
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
CString g_strExePath = _T("");
CString GetExePath( void )
{
	if (g_strExePath.IsEmpty())
	{
		TCHAR path[8192];
		int len = GetModuleFileName(GetModuleHandle(NULL),path,8192);
		path[len]=0;
		TCHAR* ret = _tcsrchr(path,'\\');
		if(!ret)
			return _T("");
		int idxlen = (int)(ret - path + 1);
		path[idxlen]=0;
		g_strExePath = CString(path);
	}

	return g_strExePath;
}

CString GetFolderPath( void )
{
	CString strSkinFolderPath = _T("");
	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(g_System->GetPluginByName("Plugin_U7_ComUI"));
	if (pluginU7ComUI != NULL)
	{
		strSkinFolderPath = pluginU7ComUI->GetSkinFolder();
	}

	return strSkinFolderPath + _T("Plugin_U7T_OpenSave\\");
}

CString GetFileSize(CString strFilePath)
{
	CString strFileSize = _T("");
	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFile( strFilePath, &wfd );
	if ( INVALID_HANDLE_VALUE == hFile || FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes 
		|| FILE_ATTRIBUTE_DEVICE & wfd.dwFileAttributes)
	{
		// nop
	}
	else
	{
		WIN32_FIND_DATA fileInfo;
		HANDLE hFind;
		unsigned long  fileSize = 0; 
		hFind = FindFirstFile(strFilePath, &fileInfo);
		if(hFind != INVALID_HANDLE_VALUE)
			fileSize = fileInfo.nFileSizeLow;
		FindClose(hFind);
		strFileSize = ConvertFileSize(fileSize);
	} 

	FindClose(hFile);
	return strFileSize;
}

CString ConvertFileSize(unsigned long fileSize)
{
	CString strFileSize;
	if ( fileSize < 1024)
	{
		strFileSize.Format(_T("%uB"),fileSize);
	}
	else if ( fileSize < 1024*1024 )
	{
		strFileSize.Format(_T("%0.2fKB"),fileSize/1024.00);
	}
	else if ( fileSize < 1024*1024*1024 )
	{
		strFileSize.Format(_T("%0.2fMB"),fileSize/1024.00/1024.00);
	}
	else 
	{
		strFileSize.Format(_T("%0.2fGB"),fileSize/1024.00/1024.00/1024.00);  
	}
	return strFileSize;
}

bool DeleteDirectory(CString strDirName, bool bDeleteDir)
{
	WIN32_FIND_DATA FindData;
	HANDLE hError;
	CString sTempFileFind;
	sTempFileFind.Format(_T("%s\\*.*"),strDirName);
	hError = FindFirstFile(sTempFileFind, &FindData);
	if (hError == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	while (::FindNextFile(hError, &FindData))
	{
		// 过虑.和..
		if (_tcsicmp(FindData.cFileName, _T(".")) == 0||_tcsicmp(FindData.cFileName, _T("..")) == 0)
		{
			continue;
		}
		CString sTempDir; 
		sTempDir.Format(_T("%s\\%s"),strDirName, FindData.cFileName);
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			DeleteDirectory(sTempDir, bDeleteDir); 
		} 
		else 
		{ 
			DeleteFile(sTempDir); 
		} 
	 
	}
	FindClose(hError);
	if (bDeleteDir)
	{
		::RemoveDirectory(strDirName);
	}
	return TRUE; 
}

bool MakeDirEx( const char* lpPath )
{
	CString strPath = CA2W(lpPath);
	if( strPath.Right(1) != _T("\\") )
		strPath += "\\";
	int nEnd = strPath.ReverseFind('\\');
	int nPt = strPath.Find('\\');
	if (strPath[nPt-1] == ':')
		nPt = strPath.Find('\\', nPt+1);

	CString strTemp = _T("");
	while(nPt != -1 && nPt<=nEnd)
	{
		strTemp = strPath.Left(nPt+1);
		if( -1 == _access(CW2A(strTemp), 0) )
		{
			if ( _mkdir(CW2A(strTemp)) !=  0)
				return false;
		}
		nPt = strPath.Find('\\', nPt+1);
	}
	return true;
}

//把HBITMAP保存成位图
BOOL SaveBmp(HBITMAP hBitmap, CString FileName)
{
	if(hBitmap==NULL || FileName.IsEmpty())
	{
		//MessageBox("参数错误");
		return FALSE;
	}

	CImage img;
	img.Attach(hBitmap);
	img.Save(FileName);
	return TRUE;
	/*
	HDC hDC;
	//当前分辨率下每象素所占字节数
	int iBits;
	//位图中每象素所占字节数
	WORD wBitCount;
	//定义调色板大小， 位图中像素字节大小 ，位图文件大小 ， 写入文件字节数 
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0; 
	//位图属性结构 
	BITMAP Bitmap;  
	//位图文件头结构
	BITMAPFILEHEADER bmfHdr;  
	//位图信息头结构 
	BITMAPINFOHEADER bi;  
	//指向位图信息头结构  
	LPBITMAPINFOHEADER lpbi;  
	//定义文件，分配内存句柄，调色板句柄 
	HANDLE fh, hDib, hPal,hOldPal=NULL; 
	//计算位图文件每个像素所占字节数 
	hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); 
	DeleteDC(hDC); 
	if (iBits <= 1)  wBitCount = 1; 
	else if (iBits <= 4)  wBitCount = 4; 
	else if (iBits <= 8)  wBitCount = 8; 
	else      wBitCount = 24; 
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize   = sizeof(BITMAPINFOHEADER);
	bi.biWidth   = Bitmap.bmWidth;
	bi.biHeight   = Bitmap.bmHeight;
	bi.biPlanes   = 1;
	bi.biBitCount  = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage  = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed  = 0;
	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	//为位图内容分配内存 
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER)); 
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
	*lpbi = bi;
	// 处理调色板  
	hPal = GetStockObject(DEFAULT_PALETTE); 
	if (hPal) 
	{ 
		hDC = ::GetDC(NULL); 
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
		RealizePalette(hDC); 
	}
	// 获取该调色板下新的像素值 
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS); 
	//恢复调色板  
	if (hOldPal) 
	{ 
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
		RealizePalette(hDC); 
	}
	::ReleaseDC(NULL, hDC); 

	//创建位图文件  
	fh = CreateFile(FileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (fh == INVALID_HANDLE_VALUE)  return FALSE; 
	// 设置位图文件头 
	bmfHdr.bfType = 0x4D42; // "BM" 
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 
	// 写入位图文件头 
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 
	// 写入位图文件其余内容 
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 
	//清除  
	GlobalUnlock(hDib); 
	GlobalFree(hDib); 
	CloseHandle(fh);
	return TRUE;*/
}

IPlugin_U7_Lang* GetPluginU7Lang( void )
{
	static IPlugin_U7_Lang* s_pPluginU7Lang= dynamic_cast<IPlugin_U7_Lang*>(g_System->GetPluginByName("Plugin_U7_Lang"));
	return s_pPluginU7Lang;
}

CString GetPluginsLangString(CString strKey, CString strDefValue)
{
	if (GetPluginU7Lang() == NULL)
		return strDefValue;

	CString strReturn = GetPluginU7Lang()->GetPluginsLangString(_T("Plugin_U7T_OpenSave"), strKey, strDefValue);
	return strReturn;
}
void AdjustWndWithMenuBar(HWND hWnd, int nMenuBarHeight)
{
	if (!::IsWindow(hWnd))
		return;

	RECT rcWnd;
	::GetWindowRect(hWnd, &rcWnd);
	if (0 !=rcWnd.top)
	{
		MONITORINFO oMonitor = {};
		oMonitor.cbSize = sizeof(oMonitor);	
		UICRect dispDevRect;
		::GetMonitorInfo(::MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
		if (MONITORINFOF_PRIMARY == oMonitor.dwFlags)
		{
			SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID)&dispDevRect, 0);
		}
		else
		{
			dispDevRect = oMonitor.rcWork;
		}

		::GetWindowRect(hWnd, &rcWnd);
		int cx = rcWnd.right - rcWnd.left;
		int cy = rcWnd.bottom - rcWnd.top;

		if (dispDevRect.bottom - rcWnd.bottom<nMenuBarHeight)
		{
			int nBottomDis = nMenuBarHeight - (dispDevRect.bottom - rcWnd.bottom);
			rcWnd.top = rcWnd.top - nBottomDis;
			if (dispDevRect.top - rcWnd.top >0)
			{
				rcWnd.top = dispDevRect.top;
			}
		}
		::SetWindowPos(hWnd, NULL, rcWnd.left, rcWnd.top, 0, 0, SWP_NOSIZE|SWP_NOACTIVATE);
	}
}

IPlugin_U7_Cfg_Comm* GetPluginU7CfgComm(void)
{
	static IPlugin_U7_Cfg_Comm* s_pIPluginU7CfgComm= dynamic_cast<IPlugin_U7_Cfg_Comm*>(g_System->GetPluginByName("Plugin_U7_Cfg_Comm"));
	return s_pIPluginU7CfgComm;
}

void SetCfgStr(CString strSection, CString strKey, CString strValue)
{
	if (GetPluginU7CfgComm() == NULL)
		return;

	GetPluginU7CfgComm()->SetCfgString(strSection, strKey, strValue);
}

CString GetCfgStr(CString strSection, CString strKey, CString strDefValue /*= _T("")*/)
{
	if (GetPluginU7CfgComm() == NULL)
		return strDefValue;

	CString strReturn = GetPluginU7CfgComm()->GetCfgString(strSection, strKey, strDefValue);
	return strReturn;
}

void SetCfgInt(CString strSection, CString strKey, int nValue)
{
	if (GetPluginU7CfgComm() == NULL)
		return;

	GetPluginU7CfgComm()->SetCfgInt(strSection, strKey, nValue);
}

int GetCfgInt(CString strSection, CString strKey, int nDefValue /*= 0*/)
{
	if (GetPluginU7CfgComm() == NULL)
		return nDefValue;

	int nReturn = GetPluginU7CfgComm()->GetCfgInt(strSection, strKey, nDefValue);
	return nReturn;
}

void SetCfgBool(CString strSection, CString strKey, bool bValue)
{
	if (GetPluginU7CfgComm() == NULL)
		return;

	GetPluginU7CfgComm()->SetCfgBool(strSection, strKey, bValue);
}

bool GetCfgBool(CString strSection, CString strKey, bool bDefValue /*= false*/)
{
	if (GetPluginU7CfgComm() == NULL)
		return bDefValue;

	bool bReturn = GetPluginU7CfgComm()->GetCfgBool(strSection, strKey, bDefValue);
	return bReturn;
}