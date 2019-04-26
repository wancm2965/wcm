// UCC7-IM�û��Զ������ݲ������
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
		 * �Զ����¼�
		 ****************************************************************/

		/* �յ��û��Զ��������¼� */
		virtual void OnIMUserData(const char* strFromID, const char* strCmdType, const char* strUserData) = 0;
	};

	// UCC7-IM��¼����ʵ�ֲ��
	class IPlugin_U7_IM_UserData : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_UserData(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_UserData_Observer* pObserver) = 0;

		
		// �����û����ݵ�ָ���Է�
		virtual void SendIMUserData(const char* strToID, const char* strCmdType, const char* strUserData) = 0;
	};
}
