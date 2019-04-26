// UCC7-IM�û��Զ������ݲ������
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
		/* �յ�P2P�û��Զ��������¼� */
		virtual void OnNodeData(const char* strFromNodeID, const char* strCmdType, const char* strUserData){};

		/* �յ�IM�û��Զ��������¼� */
		virtual void OnIMUserData(const char* strFromID, const char* strCmdType, const char* strUserData){};
	};

	// UCC7-IM��¼����ʵ�ֲ��
	class IPlugin_U7_IM_UserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_UserData(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;

		// ����P2P�û����ݵ�ָ���Է�
		virtual void SendNodeData(const char* strNodeID, const char* strCmdType, const char* strUserData) = 0;
		virtual void SendNodeData(const char* strNodeID, const STRING_MAP_EX& mapUserData) = 0;

		// ����IM�û����ݵ�ָ���Է�
		virtual void SendIMUserData(const char* strToID, const char* strCmdType, const char* strUserData) = 0;
		
		// ������ͨ���ݵ�ָ���Է�
		virtual void CheckP2PConnect(const char* strNodeID) = 0;
	};
}
