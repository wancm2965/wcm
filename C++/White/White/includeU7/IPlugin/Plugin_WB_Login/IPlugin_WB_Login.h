#pragma once
#ifndef _IPLUGIN_WB_LOGIN_H__
#define _IPLUGIN_WB_LOGIN_H__

#include "../../ISystem/IPlugin.h"
namespace AVCONPlugin
{
	class IPlugin_WB_Login : public IPlugin
	{
	public:
		virtual ~IPlugin_WB_Login(){};
		//登入对话框插件的接口
		virtual void Show(bool bShow) = 0;
		virtual void Destroy() = 0;
	};
}
#endif
