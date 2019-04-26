// stdafx.cpp : 只包括标准包含文件的源文件
// Plugin_U7T_Messagebox.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

IPlugin_U7_Lang* GetPluginU7Lang( void )
{
	static IPlugin_U7_Lang* s_pPluginU7Lang= dynamic_cast<IPlugin_U7_Lang*>(g_System->GetPluginByName("Plugin_U7_Lang"));
	return s_pPluginU7Lang;
}

CString GetFolderPath( void )
{
	CString strSkinFolderPath = _T("");
	IPlugin_U7_ComUI* pluginU7ComUI = dynamic_cast<IPlugin_U7_ComUI*>(g_System->GetPluginByName("Plugin_U7_ComUI"));
	if (pluginU7ComUI != NULL)
	{
		strSkinFolderPath = pluginU7ComUI->GetSkinFolder();
	}

	return strSkinFolderPath + _T("Plugin_U7T_MessageBox\\");
}