#include "stdafx.h"
#include "ExportPlugin.h"
#include "Plugin_Meeting_Member.h"

namespace AVCONPlugin
{
	AVCONPlugin::ISystem* g_System = NULL;
}

int LoadPlugin( AVCONPlugin::ISystem* pSystem )
{
	AVCONPlugin::g_System = pSystem;

	AVCONPlugin::IPlugin* plA = new AVCONPlugin::CPlugin_Meeting_Member;
	AVCONPlugin::g_System->AddPlugin(plA);

	return 1;
}
