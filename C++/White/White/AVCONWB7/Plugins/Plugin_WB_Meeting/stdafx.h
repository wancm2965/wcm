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
using namespace AVCONPlugin;

#include "CoUI/UIlib.h"

using namespace CoUI;
CString GetExePath(void);
CString GetFolderPath(void);
CString GetCommResFolderPath(void);
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//�����ı���д
void GetLangPath();
std::string U2A( CString strUnicodeString );
CString A2U( const char* strAnsiString );
#ifdef _DEBUG
#	pragma comment(lib, "..\\..\\..\\libU7\\Debug\\CoUI.lib")
#else
#	pragma comment(lib, "..\\..\\..\\libU7\\Release\\CoUI.lib")
#endif

