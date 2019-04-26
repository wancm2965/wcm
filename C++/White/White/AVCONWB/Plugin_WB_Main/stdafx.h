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
#include <afxdhtml.h>
#include <atlimage.h>
#include <map>
#include <list>
#include <string>


#include "AutoLockResource.h"
#include "../IncludeUCC/ISystem/ISystem.h"
using namespace  AVCONPlugin;
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus")
#include "Resource.h"
//AVCON接口中的文件和库
#include "KBASE.h"
#include "KXNCP.h"


#include "HPDTS/DTSCmdType.h"


/*#include "IDataHandle.h"*/
#include "ICommandMediator.h"
#include "WB/WBDefine.h"

#include "rwinifile.h"
#include "errorcode.h"
#include "maindef.h"
#include "../../include/HPDTS/IHPDTS.h"
#include <algorithm>


#ifdef _DEBUG
/*#pragma comment(lib,"../../libUCC/Debug/UCCSDK.lib")*/
#pragma comment(lib,"../../libUCC/Debug/KBASE.lib")
//#pragma comment(lib,"../../libUCC/Debug/HPTM.lib")
#pragma comment(lib,"../../libUCC/Debug/KDBAPI.lib")
#pragma comment(lib,"../../libUCC/Debug/KRTL32.lib")
#pragma comment(lib,"../../libUCC/Debug/NETEC.lib")
#pragma comment(lib,"../../libUCC/Debug/HPWB.lib")			//白板
#pragma comment(lib,"../../libUCC/Debug/HPDTS.lib")
#pragma comment(lib,"../../libUCC/Debug/whiteBoard.lib")
#else
/*#pragma comment(lib,"../../libUCC/Release/UCCSDK.lib")*/
#pragma comment(lib,"../../libUCC/Release/KBASE.lib")
//#pragma comment(lib,"../../libUCC/Release/HPTM.lib")
#pragma comment(lib,"../../libUCC/Release/KDBAPI.lib")
#pragma comment(lib,"../../libUCC/Release/KRTL32.lib")
#pragma comment(lib,"../../libUCC/Release/NETEC.lib")
#pragma comment(lib,"../../libUCC/Debug/HPWB.lib")			//白板
#pragma comment(lib,"../../libUCC/Release/HPDTS.lib")
#pragma comment(lib,"../../libUCC/Release/whiteBoard.lib")

#endif

CString _A2W(std::string str);
std::string _W2A(CString str);

#define CONFIG_FILEPATH_OF_LOGIN	_T("WBConfig.ini")	//配置文件路径
//---------------------------------------------------------------------------------------

CString GetExePath(void);//获得程序运行的跟目录
void WriteOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString value, CString strDefPath = _T(""));
CString ReadOptionConfigStr(LPCTSTR selection, LPCTSTR key,CString strDefPath = _T(""));
void SetHPDTSSavePath(CString LoginInId);	 //临时目录是当前进程的全部路径+HPDTS+用户名+DTSTempDir	 参数LoginInId ：用户登入名

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8'  processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"") 
//
//#ifdef _UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif
