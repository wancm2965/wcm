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

	// Ĭ�ϵ�ϵͳ����������ļ�
	// �Ժ���Ҫ���������н����趨
	// to do
	CString strMainPlugin = GetCurrentPath();
	strMainPlugin += _T("AppConfig.cfg");

	std::string strAppPluginName = czAppConfig;
	std::vector<CString> vecDlls;

	// ���������Ϣ
	// �������������������ơ�������ڵ����ӿ��ļ�����
	if(!CLoadConfig::LoadConfig(strMainPlugin, strAppPluginName, vecDlls))
	{
		//::OutputDebugString("LoadConfig Faild\n");
		delete g_System;
		return -1;
	}
	pSystemImpl->SetMainPlugin(strAppPluginName);

	// �������в��
	// ���ع����л��ж�ĳ������ļ��Ƿ��Ѿ�����
	CLoadPlugins loader;
	if(!loader.LoadPlugins(vecDlls))
	{
		//::OutputDebugString("LoadPlugins Faild\n");
		delete g_System;
		return -1;
	}

	// �����в�����в����ʼ��
	// ������˳���������g_System����ʱ������
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

	// ��ȡ�����
	std::vector<IPlugin*> vecAppPlugins;
	g_System->GetPluginByName(strAppPluginName,vecAppPlugins);

	if(vecAppPlugins.size() == 0)
	{
		//::OutputDebugString("vecAppPlugins Faild\n");
		delete g_System;
		return -1;
	}

	// ִ�������
	// ����û�н���������֤
	// to do
	IApp* app = (IApp*)(void*)vecAppPlugins[0];
	app->Init();
	app->Run();
	app->Close();

	delete g_System;
	return 0;
}
