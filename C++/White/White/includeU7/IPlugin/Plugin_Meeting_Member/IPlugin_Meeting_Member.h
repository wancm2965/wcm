//会议人员列表插件
// 
#pragma once
#include "ISystem/IPlugin.h"
namespace AVCONPlugin
{
	// UCC7-T会议插件
	class IPlugin_Meeting_Member : public IPlugin
	{
	public:
		virtual ~IPlugin_Meeting_Member(){};
	public:
		virtual void ShowUI(bool bShow) = 0;
		virtual void ResleseResource() = 0; //释放资源
		virtual void DestroyUI() = 0;
		virtual void ProcessTipMessage() =0;
	};
}
