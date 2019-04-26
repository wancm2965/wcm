#include "MainCore.h"

#include "ISystem/IApp.h"

#include "LoadConfig.h"
#include "LoadPlugins.h"
#include "SystemImpl.h"
#include "CommFun.h"

using namespace AVCONPlugin;

namespace AVCONPlugin
{
	ISystem* g_System = NULL;
}

int MainCore(const char* czAppConfig)
{
	SystemImpl* pSystemImpl = new SystemImpl;
	AVCONPlugin::g_System = pSystemImpl;

	// 默认的系统主插件配置文件
	// 以后需要根据命令行进行设定
	// to do
	CString strMainPlugin = GetCurrentPath();
	strMainPlugin += _T("AppConfig.cfg");

	std::string strAppPluginName = czAppConfig;
	std::vector<CString> vecDlls;

	// 解析插件信息
	// 解析结果返回主插件名称、插件所在的链接库文件名称
	if(!CLoadConfig::LoadConfig(strMainPlugin, strAppPluginName, vecDlls))
	{
		//::OutputDebugString("LoadConfig Faild\n");
		delete g_System;
		return -1;
	}
	pSystemImpl->SetMainPlugin(strAppPluginName);

	// 加载所有插件
	// 加载过程中会判断某个插件文件是否已经加载
	CLoadPlugins loader;
	if(!loader.LoadPlugins(vecDlls))
	{
		//::OutputDebugString("LoadPlugins Faild\n");
		delete g_System;
		return -1;
	}

	// 对所有插件进行插件初始化
	// 插件的退出处理是在g_System销毁时被调用
	std::vector<IPlugin*> vecPlugins = g_System->GetAllPlugins();
	for(size_t i=0; i<vecPlugins.size(); i++)
	{
		//DWORD dwTickCount = GetTickCount();
		vecPlugins[i]->InitPlugin();
		//DWORD dwTime = GetTickCount() - dwTickCount;
		//char czTag[1024] = {0};
		//sprintf_s(czTag, 1024, "InitPlugin: %s\n================%ums\n", vecPlugins[i]->Name().c_str(), dwTime);
		//::OutputDebugString(czTag);
	}

	if(vecPlugins.size() == 0)
	{
		//::OutputDebugString("vecPlugins Faild\n");
		delete g_System;
		return -1;
	}

	// 获取主插件
	std::vector<IPlugin*> vecAppPlugins;
	g_System->GetPluginByName(strAppPluginName,vecAppPlugins);

	if(vecAppPlugins.size() == 0)
	{
		//::OutputDebugString("vecAppPlugins Faild\n");
		delete g_System;
		return -1;
	}

	// 执行主插件
	// 这里没有进行类型验证
	// to do
	IApp* app = (IApp*)(void*)vecAppPlugins[0];
	app->Init();
	app->Run();
	app->Close();

	delete g_System;
	return 0;
}
