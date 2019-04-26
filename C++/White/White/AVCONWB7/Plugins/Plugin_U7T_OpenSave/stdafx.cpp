// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Plugin_U7T_OpenSave.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
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
		// ����.��..
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

//��HBITMAP�����λͼ
BOOL SaveBmp(HBITMAP hBitmap, CString FileName)
{
	if(hBitmap==NULL || FileName.IsEmpty())
	{
		//MessageBox("��������");
		return FALSE;
	}

	CImage img;
	img.Attach(hBitmap);
	img.Save(FileName);
	return TRUE;
	/*
	HDC hDC;
	//��ǰ�ֱ�����ÿ������ռ�ֽ���
	int iBits;
	//λͼ��ÿ������ռ�ֽ���
	WORD wBitCount;
	//�����ɫ���С�� λͼ�������ֽڴ�С ��λͼ�ļ���С �� д���ļ��ֽ��� 
	DWORD dwPaletteSize=0, dwBmBitsSize=0, dwDIBSize=0, dwWritten=0; 
	//λͼ���Խṹ 
	BITMAP Bitmap;  
	//λͼ�ļ�ͷ�ṹ
	BITMAPFILEHEADER bmfHdr;  
	//λͼ��Ϣͷ�ṹ 
	BITMAPINFOHEADER bi;  
	//ָ��λͼ��Ϣͷ�ṹ  
	LPBITMAPINFOHEADER lpbi;  
	//�����ļ��������ڴ�������ɫ���� 
	HANDLE fh, hDib, hPal,hOldPal=NULL; 
	//����λͼ�ļ�ÿ��������ռ�ֽ��� 
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
	//Ϊλͼ���ݷ����ڴ� 
	hDib = GlobalAlloc(GHND,dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER)); 
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib); 
	*lpbi = bi;
	// �����ɫ��  
	hPal = GetStockObject(DEFAULT_PALETTE); 
	if (hPal) 
	{ 
		hDC = ::GetDC(NULL); 
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE); 
		RealizePalette(hDC); 
	}
	// ��ȡ�õ�ɫ�����µ�����ֵ 
	GetDIBits(hDC, hBitmap, 0, (UINT) Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) 
		+dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS); 
	//�ָ���ɫ��  
	if (hOldPal) 
	{ 
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE); 
		RealizePalette(hDC); 
	}
	::ReleaseDC(NULL, hDC); 

	//����λͼ�ļ�  
	fh = CreateFile(FileName, GENERIC_WRITE,0, NULL, CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL); 
	if (fh == INVALID_HANDLE_VALUE)  return FALSE; 
	// ����λͼ�ļ�ͷ 
	bmfHdr.bfType = 0x4D42; // "BM" 
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;  
	bmfHdr.bfSize = dwDIBSize; 
	bmfHdr.bfReserved1 = 0; 
	bmfHdr.bfReserved2 = 0; 
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize; 
	// д��λͼ�ļ�ͷ 
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL); 
	// д��λͼ�ļ��������� 
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); 
	//���  
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