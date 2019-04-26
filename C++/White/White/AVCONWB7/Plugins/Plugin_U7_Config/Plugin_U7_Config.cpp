// Plugin_U7_Config.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ISystem/ISystem.h"
#include "Plugin_U7_Cfg_Comm.h"
#include "Plugin_U7_Lang.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace AVCONPlugin
{
	AVCONPlugin::ISystem* g_System = NULL;
}

extern "C"
{
	__declspec(dllexport) int LoadPlugin(AVCONPlugin::ISystem* pSystem)
	{
		AVCONPlugin::g_System = pSystem;

		AVCONPlugin::IPlugin* plA = new AVCONPlugin::CPlugin_U7_Cfg_Comm;
		AVCONPlugin::g_System->AddPlugin(plA);

		AVCONPlugin::IPlugin* plB = new AVCONPlugin::CPlugin_U7_Lang;
		AVCONPlugin::g_System->AddPlugin(plB);

		return 1;
	}
};
