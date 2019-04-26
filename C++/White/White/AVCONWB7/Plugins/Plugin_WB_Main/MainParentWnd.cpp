#include "StdAfx.h"
#include "MainParentWnd.h"


#include "IPlugin/Plugin_WB_Login/IPlugin_WB_Login.h"
#include "IPlugin/IU7_IM/IPlugin_U7_IM_Login.h"
#include "IPlugin/Plugin_WB_Main/IPlugin_WB_Main.h"
#include "IPlugin/IU7_MM/IPlugin_U7_MM_Room.h"

CMainParentWnd::CMainParentWnd(void)
{
}

CMainParentWnd::~CMainParentWnd(void)
{
}

LPCTSTR CMainParentWnd::GetWindowClassName() const
{
	return _T("AVCON_WB");
}

CString CMainParentWnd::GetSkinFile()
{
	return _T("ParentWnd.xml");
}
LRESULT CMainParentWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{


	bHandled = FALSE;
	return 0;
}

LRESULT CMainParentWnd::OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	IPlugin_WB_Login* pPlugin_Login = dynamic_cast<IPlugin_WB_Login*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Login"));
	if (pPlugin_Login)
	{
		pPlugin_Login->Destroy();
	}

	IPlugin_U7_MM_Room *pluginRoom = static_cast<IPlugin_U7_MM_Room*>(AVCONPlugin::g_System->GetPluginByName("Plugin_U7_MM_Room"));
	if (pluginRoom != NULL)
	{
		pluginRoom->ExitRoom(false);
	}
	WB_Close();
	//去掉强制关闭线程退出，下面这个调用顺序不能改动，不然出错wangxiaobin
	IPlugin_WB_Main *pluginMain = static_cast<IPlugin_WB_Main*>(AVCONPlugin::g_System->GetPluginByName("Plugin_WB_Main"));
	if (pluginMain != NULL)
	{
		pluginMain->Exit();
	}
	PostQuitMessage(0);
	return 0;
}
