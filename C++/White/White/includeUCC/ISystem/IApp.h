// 插件系统应用程序主插件，该插件将被第一个执行
// 具体哪个插件是主插件由配置文件确定。

#pragma once

#include "IPlugin.h"

namespace AVCONPlugin
{
	// 插件消息
	// 主插件初始化消息
	struct I_App_Init
	{};

	// 主插件关闭消息
	struct I_App_Close
	{};

	// 应用程序主插件
	class IApp : public IPlugin
	{
	public:
		virtual ~IApp(){};

	public:
		// 执行接口
		virtual bool Init() = 0;
		virtual bool Run() = 0;        // to do   添加命令行参数
		virtual bool Close() = 0;
	};
}
