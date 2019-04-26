// UCC7-IM用户自定义数据操作插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

#ifndef STDSTRINGMAP
#define STDSTRINGMAP
typedef std::map<std::string,std::string> STRING_MAP_EX;
#endif

namespace AVCONPlugin
{
	class IPlugin_U7_IM_UserData_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_UserData_Observer(){};

	public:
		/* 收到P2P用户自定义数据事件 */
		virtual void OnNodeData(const char* strFromNodeID, const char* strCmdType, const char* strUserData){};

		/* 收到IM用户自定义数据事件 */
		virtual void OnIMUserData(const char* strFromID, const char* strCmdType, const char* strUserData){};
	};

	// UCC7-IM登录功能实现插件
	class IPlugin_U7_IM_UserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_UserData(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;

		// 发送P2P用户数据到指定对方
		virtual void SendNodeData(const char* strNodeID, const char* strCmdType, const char* strUserData) = 0;
		virtual void SendNodeData(const char* strNodeID, const STRING_MAP_EX& mapUserData) = 0;

		// 发送IM用户数据到指定对方
		virtual void SendIMUserData(const char* strToID, const char* strCmdType, const char* strUserData) = 0;
		
		// 发送连通数据到指定对方
		virtual void CheckP2PConnect(const char* strNodeID) = 0;
	};
}
