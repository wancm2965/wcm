// UCC7-IM用户自定义数据操作插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_UserData_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_UserData_Observer(){};

	public:
		/****************************************************************
		 * 自定义事件
		 ****************************************************************/

		/* 收到用户自定义数据事件 */
		virtual void OnIMUserData(const char* strFromID, const char* strCmdType, const char* strUserData) = 0;
	};

	// UCC7-IM登录功能实现插件
	class IPlugin_U7_IM_UserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_UserData(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;

		
		// 发送用户数据到指定对方
		virtual void SendIMUserData(const char* strToID, const char* strCmdType, const char* strUserData) = 0;
	};
}
