#ifndef PL_LOADCONFIG_H_2892344543534534434234
#define PL_LOADCONFIG_H_2892344543534534434234

#include <string>
#include <vector>
#include <atlstr.h>

namespace AVCONPlugin
{
	class CLoadConfig
	{
	public:
		// [In]
		//		strAppConfig:   启动插件配置
		// [Out]
		//		strAppPluginName:   主插件名称
		//		vecDllNames:        插件链接库
		static bool LoadConfig(const CString& strAppConfig,std::string& strAppPluginName,std::vector<CString>& vecDllNames);
	};
}

#endif