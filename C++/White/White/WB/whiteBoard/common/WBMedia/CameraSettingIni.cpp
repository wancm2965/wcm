
#include "stdafx.h"
#include "whiteBoard.h"
#include "ResData.h"
#include "CameraSettingIni.h"

extern CameraSet* m_pVideoCameraSetting = NULL;

CString ReadConfigTextVideo( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[256] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,256,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}
CString ReadConfigTextAUDIO( CString strSelection, LPCTSTR ControlName )
{
	CString ControlText;
	wchar_t Text[256] = {0};
	GetPrivateProfileString(strSelection, ControlName, _T(""),Text,256,g_strAvconLanguageFile);
	ControlText = Text;
	return ControlText;
}

CString ReadConfigTextVideoIni( CString strSelection, LPCTSTR ControlName )
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString ControlText = _T("");
	wchar_t Text[256] = {0};
	GetPrivateProfileString(strSelection, ControlName,_T(""),Text, 256, strPath);
	ControlText = Text;
	return ControlText;
}

void WriteConfigTextVideoIni( CString strSelection, LPCTSTR ControlName ,int iValue)
{
	CString strPath = g_strAvconIniPath + CONFIG_FILEPATH_OF_TOOLBAR;
	CString strValue = _T("");
	strValue.Format(_T("%d"),iValue);
	WritePrivateProfileString(strSelection,ControlName,strValue, strPath);
}

CameraSet* GetVideoCameraSettingStruct()
{
	if (m_pVideoCameraSetting)
	{
		return m_pVideoCameraSetting;
	}
	return NULL;
	
}

void SetVideoCameraSettingStruct( CameraSet* pstruct )
{
	m_pVideoCameraSetting = pstruct;
}

CString ReadConfigTextVideoIndexIni( CString strSelection, LPCTSTR ControlName )
{
	return ReadConfigTextVideoIni(strSelection,ControlName);
}

void WriteConfigTextVideoIndexIni( CString strSelection, LPCTSTR ControlName,int iValue )
{
	WriteConfigTextVideoIni(strSelection,ControlName,iValue);
}
