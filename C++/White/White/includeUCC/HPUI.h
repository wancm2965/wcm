/*	HOUI.h	HPUI Classes Library */

#pragma once


//---------------------------------------------------------------------------------------
// ����ͷ�ļ�
//---------------------------------------------------------------------------------------
#include "hpui/HPDLG_SearchEdit.h"
#include "hpui/HPDLG_VideoParam.h"
#include "hpui/HPDLG_TalkView.h"
#include "hpui/HPDLG_ConnectProperties.h"
#include "hpui/HPDLG_FileBrowser.h"
#include "HPUI/HPDLG_VideoPreSet.h"
#include "HPUI/HPDLG_AVSetting.h"


/*���������ļ�·��
* strIniFullPath, �����ļ�ȫ·��*/
AFX_EXT_CLASS void SetLanguageIniPath(CString strIniFullPath);

/*����ͼƬ�ļ�·��
* strBmpFullPath, ͼƬ�ļ�ȫ·��*/
AFX_EXT_CLASS void SetBmpPath(CString strBmpFullPath);

/*���������ļ�·��
* strBmpFullPath, �����ļ�ȫ·��*/
AFX_EXT_CLASS void SetWavPath(CString strWavFullPath);

/*��ȡ��汾��*/
AFX_EXT_CLASS CString GetFileVersion();

/*��ȡ��building��*/
AFX_EXT_CLASS CString GetBuilding();

