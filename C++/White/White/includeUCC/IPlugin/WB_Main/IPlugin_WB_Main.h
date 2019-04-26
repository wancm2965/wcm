// UCC7-T主插件
// 
#pragma once

#include "../../ISystem/IApp.h"

namespace AVCONPlugin
{
	// UCC7-T主插件
	class IPlugin_WB_Main : public IApp
	{
	public:
		virtual ~IPlugin_WB_Main(){};

	public:
		virtual void Exit(void) = 0;
	};
}
