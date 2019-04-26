/*	HOUI.h	HPUI Classes Library */

#pragma once


//---------------------------------------------------------------------------------------
// 包含头文件
//---------------------------------------------------------------------------------------
#include "hpui/HPDLG_SearchEdit.h"
#include "hpui/HPDLG_VideoParam.h"
#include "hpui/HPDLG_TalkView.h"
#include "hpui/HPDLG_ConnectProperties.h"
#include "hpui/HPDLG_FileBrowser.h"
#include "HPUI/HPDLG_VideoPreSet.h"
#include "HPUI/HPDLG_AVSetting.h"


/*设置语言文件路径
* strIniFullPath, 语言文件全路径*/
AFX_EXT_CLASS void SetLanguageIniPath(CString strIniFullPath);

/*设置图片文件路径
* strBmpFullPath, 图片文件全路径*/
AFX_EXT_CLASS void SetBmpPath(CString strBmpFullPath);

/*设置声音文件路径
* strBmpFullPath, 声音文件全路径*/
AFX_EXT_CLASS void SetWavPath(CString strWavFullPath);

/*获取库版本号*/
AFX_EXT_CLASS CString GetFileVersion();

/*获取库building号*/
AFX_EXT_CLASS CString GetBuilding();

