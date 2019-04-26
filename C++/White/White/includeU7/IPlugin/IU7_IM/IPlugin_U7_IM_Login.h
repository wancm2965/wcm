// UCC7-IM登录功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	// 权限位命令
	typedef enum _PRIVILEGE_BIT
	{
		PRIVILEGE_BIT_SIP		= 1,	//SIP呼叫
		PRIVILEGE_BIT_SMS		= 2,	//SMS短信
		PRIVILEGE_BIT_MMS		= 3,	//会议管理(WEB)
		PRIVILEGE_BIT_MON		= 4,	//监控点播(WEB)
		PRIVILEGE_BIT_TEM		= 5,	//模板设置
		PRIVILEGE_BIT_FAX		= 6,	//收发传真
		PRIVILEGE_BIT_MSG		= 7,	//机构广播消息
		PRIVILEGE_BIT_BAD		= 8,	//发布公告
		PRIVILEGE_BIT_MAP		= 9,	//地图编辑
		PRIVILEGE_BIT_MAT		= 10,	//矩阵控制
		PRIVILEGE_BIT_LOS		= 11,	//定损权限
		PRIVILEGE_BIT_INFO		= 14,	//修改个人设置
		PRIVILEGE_BIT_ORG		= 17,	//显示机构权限;
		PRIVILEGE_BIT_VOD		= 21,	//上传录像点播权限;
		PRIVILEGE_BIT_MEET_MON	= 23,	//会议监控权限
		PRIVILEGE_BIT_REC		= 24,	//远程录像
		PRIVILEGE_BIT_MCON		= 27,	//会议操作(会控)
		PRIVILEGE_BIT_DEV		= 28,	//设备管理(会控)
	}PRIVILEGE_BIT;

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

		// nErrCode:错误代码，为0则登录成功，不为0则登录失败，当为ERR_IMS_CLIENTAUTH(20013)时，表示
		// 域认证失败，此时hr有意义，可解析hr中的含义来显示错误信息，其他情况下hr均为S_OK，后续如对此
		// 逻辑有调整，必须在此处注明
		virtual void OnLogin(int nErrCode, HRESULT hr) = 0;
		virtual void OnLogout(int nErrCode) = 0;

		// 域重连 strDomain:域名称 ，iLevel:strDomain相对于本域的级别，其中0(同级),1(上级)，-1(下级)
		virtual void OnDomainConnected(const std::string& strDomain,const int& iLevel){};
		/* 某个域连接断开 */
		virtual void OnDomainDisconnect(const std::string& strDomain,int nFromLevel){};
		virtual void OnCallCenterServiceItem(const std::string& strUserID, const std::string& strUserName, const std::string& strNodeID){};
		virtual void OnGetURLItem(const std::string& strName,const std::string& strURL,const std::string& strParams){};
		virtual void OnGetURLListEnd(){};
		// 推送Host地址
		virtual void OnWEBServer(const std::string& strHost){};

		/* 获得服务器实时时间 */
		virtual void OnServerRealTime(const std::string& strServerTime){};

		/* 收到终端故障日志开关 */
		virtual void OnFaultLogEnable(bool bEnable){};

		virtual void OnGetBusinessInfoEnd(){};

		/* 版本不匹配 */
		virtual void OnServerVersionMismatch(const int nCmd){};
		/* 带宽占用率过高 */
		virtual void OnBandwidthOccupancyRate(const int nOccupancyRate){};
	};

	// UCC7-IM登录功能实现插件
	class IPlugin_U7_IM_Login : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Login(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Login_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Login_Observer* pObserver) = 0;

		virtual void Login(const char* strSvrHost, unsigned int uServerPort, const char* strUserID, const char* strUserPwd, const char* strStatus, const char* strTermName = "PC") = 0;
		virtual void Logout(void) = 0;
		virtual void Disconnect(void) = 0;
		virtual void LoadServerList(void) = 0;

		// 获取呼叫中心服务信息
		virtual void LoadCallCenterService(void) = 0;

		virtual const char* GetNodeID(void) = 0;
		virtual const char* GetLocalIP(void) = 0;
		virtual unsigned short GetLocalPort(void) = 0;
		virtual const char* GetNATIP(void) = 0;
		virtual unsigned short GetNATPort(void) = 0;
		virtual const char* GetMCUID(void) = 0;
		virtual const char* GetMCUIP(void) = 0;
		virtual unsigned short GetMCUPort(void) = 0;

		virtual bool GetMyPrivilege(PRIVILEGE_BIT priv) = 0;

		virtual const char* GetPBXServer(void) = 0;
		virtual const char* GetWEBServer(void) = 0;

		// 请求推送链接地址
		virtual void GetUrlList() = 0;

		/* 获得服务器实时时间 */
		virtual void GetServerRealTime(void) = 0;

		/* 书写服务器日志 */
		virtual void UploadFaultLog(const std::string& strLog) = 0;

		/* 获取服务推送的所有业务信息列表 */
		virtual void GetAllBusinessInfo(IMS_BUSINESSINFO_ITEM_MAP& mapBusinessInfo) = 0;

		/* 请求获取业务信息 */
		virtual void GetBusinessInfo() = 0;
	};
}
