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
#include <list>
#include <atlstr.h>
#include "COUI/UIlib.h"
using namespace CoUI;

#include "ISystem/ISystem.h"
#include "IPlugin/IU7_Config/IPlugin_U7_Lang.h"
#include "IPlugin/IU7T.Comm/IPlugin_U7_ComUI.h"
using namespace AVCONPlugin;

CString GetFolderPath( void );

#ifdef _DEBUG
#pragma comment(lib, "../../../libU7/Debug/CoUI.lib")
#else
#pragma comment(lib, "../../../libU7/Release/CoUI.lib")
#endif

enum BTN_VALUE
{
	UCCMB_BTN_OK =1,
	UCCMB_BTN_CANCLE ,
	UCCMB_BTN_ABORT ,
	UCCMB_BTN_RETRY ,
	UCCMB_BTN_IGNORE ,
	UCCMB_BTN_YES ,
	UCCMB_BTN_NO
};

IPlugin_U7_Lang* GetPluginU7Lang(void);
