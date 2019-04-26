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
#include "IPlugin/IU7_MM/U7_MMSDefine.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
using namespace std;
using namespace AVCONPlugin;

#include "CoUI/UIlib.h"

using namespace CoUI;

std::string U2A( CString strUnicodeString );
CString A2U( const char* strAnsiString );
CString A2U(std::string strAnsiString);
CString _A2W(std::string str);
std::string _W2A(CString str);
CString ULONG2STR(unsigned long ulValue);

CString GetExePath(void);
CString GetFolderPath();
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//配置文本读写
void GetLangPath();

#ifdef _DEBUG
#	pragma comment(lib, "..\\..\\..\\libU7\\Debug\\CoUI.lib")
#else
#	pragma comment(lib, "..\\..\\..\\libU7\\Release\\CoUI.lib")
#endif

