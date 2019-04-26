#include "stdafx.h"
#include "ExportPlugin.h"
#include "ISystem/ISystem.h"
#include "Plugin_WB_Main.h"


namespace AVCONPlugin
{
	AVCONPlugin::ISystem* g_System = NULL;
}

int LoadPlugin(AVCONPlugin::ISystem* pSystem)
{
	AVCONPlugin::g_System = pSystem;

	AVCONPlugin::IPlugin* plA = new AVCONPlugin::CPlugin_WB_Main;
	AVCONPlugin::g_System->AddPlugin(plA);

	return 1;
}