#ifndef PL_IPLUGIN_H_8998233424234232323
#define PL_IPLUGIN_H_8998233424234232323

// 插件基类
#include <string>

namespace AVCONPlugin
{
	class IPlugin
	{
	public:
		virtual ~IPlugin(){};

	public:
		// 获取插件名称
		virtual std::string Name() = 0;
		// 获取插件功能点
		virtual std::string ExtendPoint() = 0;
		// 初始化插件
		virtual bool InitPlugin() = 0;
		// 插件退出处理
		virtual bool ClosePlugin() = 0;
	};
}

#endif