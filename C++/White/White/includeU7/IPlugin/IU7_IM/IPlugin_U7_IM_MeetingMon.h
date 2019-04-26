// UCC7-IM�����ع���ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_MeetingMon_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_MeetingMon_Observer(){};

	public:
		virtual void OnGetDomainInfo(const SUBSYSTEM_DOMAININFO& item){};
		virtual void OnGetDomainInfoEnd(){};

		virtual void OnGetMCUInfo(const SUBSYSTEM_MCUINFO& item){};
		virtual void OnGetMCUInfoEnd(const std::string& strDomain){};

		virtual void OnGetMCUDeviceInfo(const SUBSYSTEM_DEVICEINFO& item){};
		virtual void OnGetMCUDeviceInfoEnd(const std::string& strDomain,const std::string& strUpdateTime){};

		virtual void OnGetMCUMGWInfo(const SUBSYSTEM_DEVICEINFO& item){};
		virtual void OnGetMCUMGWInfoEnd(const std::string& strDomain,const std::string& strUpdateTime){};

		virtual void OnMCUDeviceStatus(const SUBSYSTEM_DEVICEINFO& item){};

		virtual void OnGetAlarmSearch(unsigned long ulSearchId, unsigned int unAlarmNum){};
		virtual void OnGetAlarmLogs(unsigned long ulSearchId, int nBeginRow, int nEndRow, bool bSort, 
			const ALARMLOG_INFO_LST& lstAlarmLog, int nErrorCode){};
		virtual void OnDelAlarmSearch(unsigned long ulSearchId){};
	};

	// UCC7-IM��ط�������ʵ�ֲ��
	class IPlugin_U7_IM_MeetingMon : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_MeetingMon(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_MeetingMon_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_MeetingMon_Observer* pObserver) = 0;

		/* ��ȡdomain��Ϣ(�����¼�����Ϣ) */
		virtual void GetDomainInfo() = 0;

		/* ��ȡָ����MCU��Ϣ */
		virtual void GetMCUInfo(const std::string& strDomain) = 0;

		/* ��ȡ�豸�б� */
		virtual void GetMCUDeviceInfo(const std::string& strDomain,const std::string& strUpdateTime) = 0;

		/* ��ȡ�����б� */
		virtual void GetMCUMGWInfo(const std::string& strDomain, const std::string& strUpdatetime) = 0;
	};
}
