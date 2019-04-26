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
#include <map>
#include <vector>
#include <list>
#include <string>
#include "ISystem/ISystem.h"
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/IU7T_OpenSave/IPlugin_U7T_OpenSave.h"
using namespace AVCONPlugin;
using namespace std;

#include "CoUI/UIlib.h"

using namespace CoUI;
#ifdef _DEBUG
#	pragma comment(lib, "..\\..\\..\\libU7\\Debug\\CoUI.lib")
#else
#	pragma comment(lib, "..\\..\\..\\libU7\\Release\\CoUI.lib")
#endif

using namespace AVCONPlugin;


 CString GetExePath(void);
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//配置文本读写
void GetLangPath();

 bool	MakeDirEx(const char* lpPath);
 /********************************************************************
 * 将UTF8字符串解码成通用字符串
 ********************************************************************/
 CString A2U(const char* strAnsiString);
 
 /********************************************************************
 * 将通用字符串编码成UTF8字符串
 ********************************************************************/
 std::string U2A(CString strUnicodeString);
 void Reboot(void);
