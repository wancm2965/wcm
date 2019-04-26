// UCC7-IM登录功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Login_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Login_Observer(){};

	public:
		virtual void OnConnectStatus(unsigned long ulStatus, int nErrCode) = 0;
		virtual void OnServerListItem(const char* strMCUID,const char* strMCUType,
			unsigned long ulConnections,const char* strBandWidth,unsigned long ulActivity,
			const char* strIPAddr,unsigned short usIPPort,const char* strIPName) = 0;
		virtual void OnServerListEnd(void) = 0;

		virtual void OnLogin(int nErrCode) = 0;
		virtual void OnLogout(int nErrCode) = 0;
	};

	// UCC7-IM登录功能实现插件
	class IPlugin_U7_IM_Login : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Login(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Login_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Login_Observer* pObserver) = 0;

		virtual void Login(const char* strSvrHost, unsigned int uServerPort, const char* strUserID, const char* strUserPwd, const char* strStatus) = 0;
		virtual void Logout(void) = 0;
		virtual void LoadServerList(void) = 0;

		virtual const char* GetPBXServer() = 0;
	};
}
