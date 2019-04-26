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
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//�����ı���д
void GetLangPath();

#ifdef _DEBUG
#	pragma comment(lib, "..\\..\\..\\libU7\\Debug\\CoUI.lib")
#else
#	pragma comment(lib, "..\\..\\..\\libU7\\Release\\CoUI.lib")
#endif

