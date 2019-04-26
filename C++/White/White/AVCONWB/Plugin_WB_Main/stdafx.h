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
#include <afxdhtml.h>
#include <atlimage.h>
#include <map>
#include <list>
#include <string>


#include "AutoLockResource.h"
#include "../IncludeUCC/ISystem/ISystem.h"
using namespace  AVCONPlugin;
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus")
#include "Resource.h"
//AVCON�ӿ��е��ļ��Ϳ�
#include "KBASE.h"
#include "KXNCP.h"


#include "HPDTS/DTSCmdType.h"


/*#include "IDataHandle.h"*/
#include "ICommandMediator.h"
#include "WB/WBDefine.h"

#include "rwinifile.h"
#include "errorcode.h"
#include "maindef.h"
#include "../../include/HPDTS/IHPDTS.h"
#include <algorithm>


#ifdef _DEBUG
/*#pragma comment(lib,"../../libUCC/Debug/UCCSDK.lib")*/
#pragma comment(lib,"../../libUCC/Debug/KBASE.lib")
//#pragma comment(lib,"../../libUCC/Debug/HPTM.lib")
#pragma comment(lib,"../../libUCC/Debug/KDBAPI.lib")
#pragma comment(lib,"../../libUCC/Debug/KRTL32.lib")
#pragma comment(lib,"../../libUCC/Debug/NETEC.lib")
#pragma comment(lib,"../../libUCC/Debug/HPWB.lib")			//�װ�
#pragma comment(lib,"../../libUCC/Debug/HPDTS.lib")
#pragma comment(lib,"../../libUCC/Debug/whiteBoard.lib")
#else
/*#pragma comment(lib,"../../libUCC/Release/UCCSDK.lib")*/
#pragma comment(lib,"../../libUCC/Release/KBASE.lib")
//#pragma comment(lib,"../../libUCC/Release/HPTM.lib")
#pragma comment(lib,"../../libUCC/Release/KDBAPI.lib")
#pragma comment(lib,"../../libUCC/Release/KRTL32.lib")
#pragma comment(lib,"../../libUCC/Release/NETEC.lib")
#pragma comment(lib,"../../libUCC/Debug/HPWB.lib")			//�װ�
#pragma comment(lib,"../../libUCC/Release/HPDTS.lib")
#pragma comment(lib,"../../libUCC/Release/whiteBoard.lib")

#endif

CString _A2W(std::string str);
std::string _W2A(CString str);

#define CONFIG_FILEPATH_OF_LOGIN	_T("WBConfig.ini")	//�����ļ�·��
//---------------------------------------------------------------------------------------

CString GetExePath(void);//��ó������еĸ�Ŀ¼
void WriteOptionConfigStr(LPCTSTR selection, LPCTSTR key, CString value, CString strDefPath = _T(""));
CString ReadOptionConfigStr(LPCTSTR selection, LPCTSTR key,CString strDefPath = _T(""));
void SetHPDTSSavePath(CString LoginInId);	 //��ʱĿ¼�ǵ�ǰ���̵�ȫ��·��+HPDTS+�û���+DTSTempDir	 ����LoginInId ���û�������

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8'  processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"") 
//
//#ifdef _UNICODE
//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif
//#endif
