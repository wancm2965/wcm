#include "stdafx.h"
#include "ExportPlugin.h"
#include "ISystem/ISystem.h"
#include "Plugin_U7T_OpenSave.h"

#if !defined(UNDER_CE) && defined(_DEBUG)
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

namespace AVCONPlugin
{
	AVCONPlugin::ISystem* g_System = NULL;
}

int LoadPlugin(AVCONPlugin::ISystem* pSystem)
{
	AVCONPlugin::g_System = pSystem;

	AVCONPlugin::IPlugin* plA = new AVCONPlugin::CPlugin_U7T_OpenSave;
	AVCONPlugin::g_System->AddPlugin(plA);

	return 1;
}