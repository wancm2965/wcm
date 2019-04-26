// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif


#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 类
#include <afxodlgs.h>       // MFC OLE 对话框类
#include <afxdisp.h>        // MFC 自动化类
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 数据库类
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 数据库类
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>            // MFC 套接字扩展
#include <afxcontrolbars.h>

#define ULONG_PTR ULONG 
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include <io.h>
#include <sys\stat.h>

#include "../../include/WB/WBDefine.h"


#include "common\Define.h"
#include "common\exec.h"
#include "common\ResourceDef.h"
#include "common/XCritSec.h"
#include "common/XAutoLock.h"
#include "common/XBufferPool.h"
#include "common/DragDropDef.h"
#include <math.h>
#include <atlimage.h>
#define WM_APP_GRAPHNOTIFY (WM_APP + 1)    // Private message.
#pragma comment(lib, "strmiids.lib")

#include "../../include/WB/WBDefine.h"
#include "../../include/HPDTS/DTSCmdType.h"
#include "../../include/HPWBModel.h"
#include "../../include/hpdts/IHPDTS.h"

//#include "hpdts/HPWBDataTransfer.h"

#include "shai\XMemIni.h"
#include "tstring.h"
#include <wchar.h>

#ifdef AVCON_U7_WHITEBOARD
#ifdef _DEBUG
#pragma comment(lib,"../../libU7//debug/IMXEC.lib")
#pragma comment(lib,"../../libU7//debug/VIDEC.lib")
#pragma comment(lib,"../../libU7//debug/AUDEC.lib")
#pragma comment(lib,"../../libU7/Debug/NETEC.lib")
#pragma comment(lib,"../../libU7/Debug/DATEC.lib")
//
#pragma comment(lib,"../../libU7/Debug/HPDTS.lib")
#else
#pragma comment(lib,"../../libU7//release/IMXEC.lib")
#pragma comment(lib,"../../libU7//release/VIDEC.lib")
#pragma comment(lib,"../../libU7//release/AUDEC.lib")
#pragma comment(lib,"../../libU7/release/NETEC.lib")
#pragma comment(lib,"../../libU7/release/DATEC.lib")
//
#pragma comment(lib,"../../libU7/release/HPDTS.lib")
#endif
#else 
#ifdef _DEBUG
#pragma comment(lib,"../../libUCC//debug/IMXEC.lib")
#pragma comment(lib,"../../libUCC//debug/VIDEC.lib")
#pragma comment(lib,"../../libUCC//debug/AUDEC.lib")
#pragma comment(lib,"../../libUCC/Debug/NETEC.lib")
#pragma comment(lib,"../../libUCC/Debug/DATEC.lib")
//
#pragma comment(lib,"../../libUCC/Debug/HPDTS.lib")
#else
#pragma comment(lib,"../../libUCC/release/IMXEC.lib")
#pragma comment(lib,"../../libUCC/release/VIDEC.lib")
#pragma comment(lib,"../../libUCC/release/AUDEC.lib")
#pragma comment(lib,"../../libUCC/release/NETEC.lib")
#pragma comment(lib,"../../libUCC/release/DATEC.lib")
//
#pragma comment(lib,"../../libUCC/release/HPDTS.lib")
#endif
#endif


#define ZSYDZ 1

//库
//#define  SUPPORTMT 

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <afxrich.h>
bool AvconRename(const char*pPath,string& str_name);
bool AvconRename(const char*pPath, string& str_new_name,string& str_Path,string& str_title,string& str_ext);
bool AvconParseFilePath(const char* pPath,string& str_Path,string& str_Title,string& str_ext);
bool RemoveDir( const char* pDir_name );
DWORD CreateALLDirectory(LPCTSTR Directory) /*创建文件夹 */;
void AvconGet_path(const char* pStr_Path,char* pCrrent_Dir);  //得到当前路径
void AvconGet_name(const char* pStr_path,char* pFile_name);//得到当前文件名

CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//配置文本读写
int ReadWBConfigPathINI(CString strSelection,LPCTSTR Position); //读WBConfig.ini文件信息
void WriteWBConfigPathINI(CString strSelection,LPCTSTR Postion,LPCTSTR strValue); //写入WBConfig.ini文件信息
CString A2U(std::string sstring);
std::string U2A(CString strUnicodeString);
//#ifdef _UNICODE
// #if defined _M_IX86
// #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
// #elif defined _M_X64
// #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
// #endif
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif
