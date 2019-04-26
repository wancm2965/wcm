// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Plugin_U7T_Messagebox.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

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