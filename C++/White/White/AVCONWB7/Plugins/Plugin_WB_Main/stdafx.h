// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <afxwin.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//�����ı���д
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
void ShutDown(void);
void Reboot(void);