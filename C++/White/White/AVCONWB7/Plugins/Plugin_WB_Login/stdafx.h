// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
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
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//�����ı���д
void GetLangPath();

 bool	MakeDirEx(const char* lpPath);
 /********************************************************************
 * ��UTF8�ַ��������ͨ���ַ���
 ********************************************************************/
 CString A2U(const char* strAnsiString);
 
 /********************************************************************
 * ��ͨ���ַ��������UTF8�ַ���
 ********************************************************************/
 std::string U2A(CString strUnicodeString);
 void Reboot(void);
