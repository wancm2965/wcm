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

//引入GDI+支持
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

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

#include <Vfw.h>
#include <MMSystem.h>
#pragma comment (lib,"vfw32.lib")//静态链接方式
#pragma comment (lib,"winmm.lib")

#include   <string> 
#include "../../../include/HPWBModel.h"
extern CString g_strAvconLanguageFile;
extern CString GetCurrentPath();
//extern BOOL   bMedioSupportU7Open;
//extern IHPNetNotify* WBDataNotify;
extern BOOL   m_bSupportU7OpenFile;
extern IHPNetNotify* m_HPWBDataNotify;
extern HWND  m_hAreaVideoScreenHwnd;
extern void *m_pVedioScreen;
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//配置文本读写
void GetLangPath();
