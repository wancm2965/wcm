#pragma once
#include "afxwin.h"
#include "CameraData.h"

extern CameraSet  *m_pVideoCameraSetting;

CString ReadConfigTextVideo(CString strSelection, LPCTSTR ControlName);//�����ı���д��Ƶ

CString ReadConfigTextAUDIO(CString strSelection, LPCTSTR ControlName);//�����ı���д��Ƶ

CString ReadConfigTextVideoIndexIni(CString strSelection, LPCTSTR ControlName);//��ȡ����
void WriteConfigTextVideoIndexIni(CString strSelection, LPCTSTR ControlName,int iValue);


CString ReadConfigTextVideoIni(CString strSelection, LPCTSTR ControlName);//�����ı���д��ȡֵ
void WriteConfigTextVideoIni(CString strSelection, LPCTSTR ControlName,int iValue);//�����ı���д

CameraSet* GetVideoCameraSettingStruct(); 
void       SetVideoCameraSettingStruct(CameraSet* pstruct);