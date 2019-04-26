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




#include "Resource.h"
#include <string>
#include <map>
#include <atlstr.h>
using namespace std;
#include <list>

#include "HPDTS/IHPDTS.h"
#include "HPDTS/DTSCmdType.h"
#include "AVDataPacket.h"
#include "HPWBModel.h"
#include "WB/WBDefine.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/IU7T_OpenSave/IPlugin_U7T_OpenSave.h"
#include "IPlugin/IU7_MM/U7_MMSDefine.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"

CString GetExePath(void);
CString A2U(std::string sstring);
std::string U2A(CString strUnicodeString);

#ifdef _DEBUG
#	pragma comment(lib, "../../../libU7/Debug/whiteBoard.lib")
#	pragma comment(lib, "../../../libU7/Debug/CoUI.lib")
#	pragma comment(lib, "../../../libU7/Debug/HPDTS.lib")
#	pragma comment(lib, "../../../libU7/Debug/HPWBModel.lib")
#	pragma comment(lib, "../../../libU7/Debug/NETEC.lib")
#	pragma comment(lib, "../../../libU7/Debug/DATEC.lib")

#else
#	pragma comment(lib, "../../../libU7/Release/whiteBoard.lib")
#	pragma comment(lib, "../../../libU7/Release/CoUI.lib")
#	pragma comment(lib, "../../../libU7/Release/HPDTS.lib")
#	pragma comment(lib, "../../../libU7/Release/HPWBModel.lib")
#	pragma comment(lib, "../../../libU7/Release/NETEC.lib")
#	pragma comment(lib, "../../../libU7/Release/DATEC.lib")
#endif
