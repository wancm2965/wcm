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
