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

//����GDI+֧��
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

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

#include <Vfw.h>
#include <MMSystem.h>
#pragma comment (lib,"vfw32.lib")//��̬���ӷ�ʽ
#pragma comment (lib,"winmm.lib")

#include   <string> 
#include "../../../include/HPWBModel.h"
extern CString g_strAvconLanguageFile;
extern CString GetCurrentPath();
//extern BOOL   bMedioSupportU7Open;
//extern IHPNetNotify* WBDataNotify;
extern BOOL   m_bSupportU7OpenFile;
extern IHPNetNotify* m_HPWBDataNotify;
extern HWND  m_hAreaVideoScreenHwnd;
extern void *m_pVedioScreen;
CString ReadConfigText(CString strSelection, LPCTSTR ControlName);//�����ı���д
void GetLangPath();
