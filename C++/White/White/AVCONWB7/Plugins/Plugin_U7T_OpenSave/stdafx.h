// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

// TODO: 在此处引用程序需要的其他头文件
#include <atlstr.h>
#include <list>
#include <vector>
#include <map>
using namespace std;

#include "COUI/UIlib.h"
using namespace CoUI;

#include "ISystem/ISystem.h"
using namespace AVCONPlugin;

#include "U7_CommDef/CommFunction.h"
#include "IPlugin/IU7_Config/IPlugin_U7_Lang.h"
#include "IPlugin/IU7T.Comm/IPlugin_U7_ComUI.h"
#include "IPlugin/IU7_Config/IPlugin_U7_Cfg_Comm.h"

CString GetExePath(void);
CString GetFolderPath( void );
CString GetFileSize(CString strFilePath);
CString ConvertFileSize(unsigned long  fileSize);
bool DeleteDirectory(CString strDirName, bool bDeleteDir = false);//删除文件夹下的所有文件  bDeleteDir是否删除文件夹
bool MakeDirEx(const char* lpPath);
BOOL SaveBmp(HBITMAP hBitmap, CString FileName);
CString GetPluginsLangString(CString strKey, CString strDefValue);

enum OPENSAVEBTN
{
	OSB_BTN_NULL ,
	OSB_BTN_CLOSE ,
	OSB_BTN_CANCEL ,
	OSB_BTN_OPEN ,
	OSB_BTN_GOBACK ,
	OSB_BTN_UPWARD ,
	OSB_BTN_NEWFOLDER ,
	OSB_BTN_PREVIEWS,
	OSB_BTN_RENAME,
	OSB_BTN_DELETE,
};

enum OPENSAVECOMBOX
{
	OSB_COM_SEARCHRANGE =0x1A,
	OSB_COM_FILENAME,
	OSB_COM_FILETYPE,
	OSB_COM_NONETYPE
};
enum FILETYPE
{
	F_RECENT = 0,
	F_DESKTOP,
	F_PERSONAL,
	F_COMPUTER,
	F_DEVICE,
	F_NETWORK,
	F_NORMAL,
};
typedef struct tagListItem
{	
	CString        strFileName;
	CString        strFilePath;
	FILETYPE       dwFileType;
}ListItem, *pListItem;

#ifdef _DEBUG
#	pragma comment(lib, "../../../libU7/Debug/CoUI.lib")
#else
#	pragma comment(lib, "../../../libU7/Release/CoUI.lib")
#endif

IPlugin_U7_Lang* GetPluginU7Lang(void);
void AdjustWndWithMenuBar(HWND hWnd, int nMenuBarHeight);
IPlugin_U7_Cfg_Comm* GetPluginU7CfgComm(void);

// 读写公用配置
void SetCfgStr(CString strSection, CString strKey, CString strValue);
CString GetCfgStr(CString strSection, CString strKey, CString strDefValue = _T(""));

void SetCfgInt(CString strSection, CString strKey, int nValue);
int GetCfgInt(CString strSection, CString strKey, int nDefValue = 0);

void SetCfgBool(CString strSection, CString strKey, bool bValue);
bool GetCfgBool(CString strSection, CString strKey, bool bDefValue = 0);