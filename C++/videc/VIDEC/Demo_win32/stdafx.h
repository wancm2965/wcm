
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"
#include "Resource.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����

#include <mmsystem.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include <map>
#include <list>
#include <string>

using namespace std;


enum VIDEO_SHOW_TYPE{
	VIDEO_SHOW_D3D=0,
	VIDEO_SHOW_D3D_9,
	VIDEO_SHOW_D3D_11,
	VIDEO_SHOW_DRSHOW,
	VIDEO_SHOW_XIS_LIB,
};

#define USE_HPAVC 0


void ReadVidecIni(void);
int  GetMaxDXVAPlayers(void);
int  GetMaxEncoderCount(void);
int  GetLocalVideo(void);

//#pragma comment(lib,"./LibXIStream2.lib")
#pragma comment(lib,"./VIDECD3D.lib")


#ifdef _DEBUG
#pragma comment(lib,"strmbasd.lib")
#pragma comment(lib,"../../lib/Debug/HPAVC.lib")
#pragma comment(lib,"../../lib/Debug/VIDEC.lib")
#pragma comment(lib,"../../lib/Debug/NETEC.lib")
#pragma comment(lib,"../HPAVC/HPAVC_Common/lib/Debug/HPAVC_Common.lib")

#else
#pragma comment(lib,"STRMBASE.lib")
#pragma comment(lib,"../../lib/Release/HPAVC.lib")
#pragma comment(lib,"../../lib/Release/VIDEC.lib")
#pragma comment(lib,"../../lib/Release/NETEC.lib")
#pragma comment(lib,"../HPAVC/HPAVC_Common/lib/Release/HPAVC_Common.lib")
# endif


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


#include "VIDEC_AH2K.h"