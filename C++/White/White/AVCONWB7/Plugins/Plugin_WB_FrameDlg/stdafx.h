// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT




#include "Resource.h"
#include <string>
#include <map>
#include <atlstr.h>
using namespace std;
#include <list>

#include "HPDTS/IHPDTS.h"
#include "HPDTS/DTSCmdType.h"
#include "AVDataPacket.h"
#include "HPWBModel.h"
#include "WB/WBDefine.h"
#include "IPlugin/IU7_MessageBox/IPlugin_U7T_MessageBox.h"
#include "IPlugin/IU7T_OpenSave/IPlugin_U7T_OpenSave.h"
#include "IPlugin/IU7_MM/U7_MMSDefine.h"
#include "IPlugin/Plugin_WB_Meeting/IPlugin_WB_Meeting.h"

CString GetExePath(void);
CString A2U(std::string sstring);
std::string U2A(CString strUnicodeString);

#ifdef _DEBUG
#	pragma comment(lib, "../../../libU7/Debug/whiteBoard.lib")
#	pragma comment(lib, "../../../libU7/Debug/CoUI.lib")
#	pragma comment(lib, "../../../libU7/Debug/HPDTS.lib")
#	pragma comment(lib, "../../../libU7/Debug/HPWBModel.lib")
#	pragma comment(lib, "../../../libU7/Debug/NETEC.lib")
#	pragma comment(lib, "../../../libU7/Debug/DATEC.lib")

#else
#	pragma comment(lib, "../../../libU7/Release/whiteBoard.lib")
#	pragma comment(lib, "../../../libU7/Release/CoUI.lib")
#	pragma comment(lib, "../../../libU7/Release/HPDTS.lib")
#	pragma comment(lib, "../../../libU7/Release/HPWBModel.lib")
#	pragma comment(lib, "../../../libU7/Release/NETEC.lib")
#	pragma comment(lib, "../../../libU7/Release/DATEC.lib")
#endif
