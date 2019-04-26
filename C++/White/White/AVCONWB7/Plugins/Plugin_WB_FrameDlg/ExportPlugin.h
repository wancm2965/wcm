#ifndef PL_IEXPORTPLUGIN_H_289223323242545453453454
#define PL_IEXPORTPLUGIN_H_289223323242545453453454

#include "ISystem/ISystem.h"

// 插件导出函数
extern "C"
{
	__declspec(dllexport) int LoadPlugin(AVCONPlugin::ISystem* pSystem);
}
#endif