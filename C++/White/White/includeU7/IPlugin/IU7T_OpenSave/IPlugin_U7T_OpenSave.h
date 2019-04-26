#pragma once

#include "../../ISystem/IPlugin.h"

#define PLUGIN_NAME_U7T_OPENSAVE			"Plugin_U7T_OpenSave"

namespace AVCONPlugin
{
	// 普通插件OpenSave
	class IPlugin_U7T_OpenSave : public IPlugin
	{
	public:
		virtual ~IPlugin_U7T_OpenSave(){};

	public:
		virtual HWND CreateOpenSave(BOOL bOpenSave,		//TRUE --OpenDlg, FALSE ----SaveDlg
			LPCTSTR lpszDefExt = NULL,					//指定默认的文件扩展名 
			LPCTSTR lpszFilter = NULL,					//指明可供选择的文件类型和相应的扩展名
			LPCTSTR strMemberID=_T("Local"),			//用于保存人员打开文件记录,默认是保存在Local文件夹下
			HWND hParent=NULL,
			LPCTSTR strFilePath =_T("")				//另存时，需传所需保存文件的原路径);
			)=0;
		virtual void ShowDlg(bool bShow) = 0;
		virtual const char* GetOpenFile(void) = 0;
		virtual void DestroyOpenSave() = 0;
	};
}
