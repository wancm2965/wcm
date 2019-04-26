// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <afxwin.h>

// TODO: 在此处引用程序需要的其他头文件
#include <afxstr.h>
#include <map>
#include <vector>
#include <list>
#include <string>

#include "..\..\..\includeU7\CoUI\UIlib.h"
using namespace CoUI;
#include "ISystem\ISystem.h"
using namespace AVCONPlugin;
using namespace std;
#define MAX max
#include "HPDTS/IHPDTS.h"
#include "HPDTS/DTSCmdType.h"
#include "AVDataPacket.h"
#include "HPWBModel.h"
#include "WB/WBDefine.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/IU7T_OpenSave/IPlugin_U7T_OpenSave.h"
#include "IPlugin/IU7_MM/U7_MMSDefine.h"
#include "IPlugin/IU7_Config/IPlugin_U7_Cfg_Comm.h"

std::string U2A( CString strUnicodeString );
CString A2U( const char* strAnsiString );
void SetHPDTSSavePath(CString LoginId);
CString GetExePath(void);
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//配置文本读写
void GetLangPath();
CString GetFolderPath(void);
IPlugin_U7_Cfg_Comm* GetPluginCfgComm(void);
#ifdef _DEBUG
#pragma comment(lib, "../../../libU7/Debug/CoUI.lib")
#pragma comment(lib,"../../../libU7/Debug/KBASE.lib")
#pragma comment(lib,"../../../libU7/Debug/HPTM.lib")
#pragma comment(lib,"../../../libU7/Debug/KDBAPI.lib")
#pragma comment(lib,"../../../libU7/Debug/KRTL32.lib")
#pragma comment(lib,"../../../libU7/Debug/NETEC.lib")
#pragma comment(lib,"../../../libU7/Debug/HPDTS.lib")
#pragma comment(lib,"../../../libU7/Debug/whiteBoard.lib")
#else
#pragma comment(lib, "../../../libU7/Release/CoUI.lib")
#pragma comment(lib,"../../../libU7/Release/KBASE.lib")
#pragma comment(lib,"../../../libU7/Release/HPTM.lib")
#pragma comment(lib,"../../../libU7/Release/KDBAPI.lib")
#pragma comment(lib,"../../../libU7/Release/KRTL32.lib")
#pragma comment(lib,"../../../libU7/Release/NETEC.lib")
#pragma comment(lib,"../../../libU7/Release/HPDTS.lib")
#pragma comment(lib,"../../../libU7/Release/whiteBoard.lib")

#endif

// TODO: 在此处引用程序需要的其他头文件
void ShutDown(void);
void Reboot(void);