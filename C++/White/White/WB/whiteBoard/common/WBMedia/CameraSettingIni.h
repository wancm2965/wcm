#pragma once
#include "afxwin.h"
#include "CameraData.h"

extern CameraSet  *m_pVideoCameraSetting;

CString ReadConfigTextVideo(CString strSelection, LPCTSTR ControlName);//配置文本读写视频

CString ReadConfigTextAUDIO(CString strSelection, LPCTSTR ControlName);//配置文本读写音频

CString ReadConfigTextVideoIndexIni(CString strSelection, LPCTSTR ControlName);//获取索引
void WriteConfigTextVideoIndexIni(CString strSelection, LPCTSTR ControlName,int iValue);


CString ReadConfigTextVideoIni(CString strSelection, LPCTSTR ControlName);//配置文本读写获取值
void WriteConfigTextVideoIni(CString strSelection, LPCTSTR ControlName,int iValue);//配置文本读写

CameraSet* GetVideoCameraSettingStruct(); 
void       SetVideoCameraSettingStruct(CameraSet* pstruct);