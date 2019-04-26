#include "stdafx.h"
#include "ExportPlugin.h"
#include "Plugin_WB_Frame.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace AVCONPlugin
{
	AVCONPlugin::ISystem* g_System = NULL;
}

int LoadPlugin(AVCONPlugin::ISystem* pSystem)
{
	AVCONPlugin::g_System = pSystem;

	AVCONPlugin::IPlugin* plA = new AVCONPlugin::CPlugin_WB_FrameDlg;
	AVCONPlugin::g_System->AddPlugin(plA);

	return 1;
}