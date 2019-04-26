// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// WBScreenVedio.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

CString GetCurrentPath()
{
	TCHAR szPath[MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH); 

	CString strPath = szPath;
	int nPos; 
	nPos = strPath.ReverseFind('\\'); 
	strPath = strPath.Left(nPos); 

	return strPath;
}

//extern BOOL bMedioSupportU7Open = FALSE;
//extern IHPNetNotify* WBDataNotify = NULL;
extern CString g_strAvconLanguageFile  = _T("");
extern BOOL   m_bSupportU7OpenFile = FALSE;
extern IHPNetNotify* m_HPWBDataNotify = NULL;
extern HWND  m_hAreaVideoScreenHwnd = NULL;
extern void *m_pVedioScreen = NULL;

//��������ѡ��
void GetLangPath()
{
	CString strCfgFilePath = GetCurrentPath() + _T("\\U7Config.ini");
	wchar_t strValue[1024] = _T("");
	::GetPrivateProfileString(_T("LANGUAGE"), _T("Language"), _T("cs"), strValue, 1024, strCfgFilePath);
	g_strAvconLanguageFile = GetCurrentPath() + _T("\\config") +_T("\\") + strValue + _T("_WBConfig.ini");
}
//��ȡ�����ļ�
CString ReadConfigText( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[256] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,256,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}