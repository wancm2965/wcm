// UCC7-IM��¼����ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	// Ȩ��λ����
	typedef enum _PRIVILEGE_BIT
	{
		PRIVILEGE_BIT_SIP		= 1,	//SIP����
		PRIVILEGE_BIT_SMS		= 2,	//SMS����
		PRIVILEGE_BIT_MMS		= 3,	//�������(WEB)
		PRIVILEGE_BIT_MON		= 4,	//��ص㲥(WEB)
		PRIVILEGE_BIT_TEM		= 5,	//ģ������
		PRIVILEGE_BIT_FAX		= 6,	//�շ�����
		PRIVILEGE_BIT_MSG		= 7,	//�����㲥��Ϣ
		PRIVILEGE_BIT_BAD		= 8,	//��������
		PRIVILEGE_BIT_MAP		= 9,	//��ͼ�༭
		PRIVILEGE_BIT_MAT		= 10,	//�������
		PRIVILEGE_BIT_LOS		= 11,	//����Ȩ��
		PRIVILEGE_BIT_INFO		= 14,	//�޸ĸ�������
		PRIVILEGE_BIT_ORG		= 17,	//��ʾ����Ȩ��;
		PRIVILEGE_BIT_VOD		= 21,	//�ϴ�¼��㲥Ȩ��;
		PRIVILEGE_BIT_MEET_MON	= 23,	//������Ȩ��
		PRIVILEGE_BIT_REC		= 24,	//Զ��¼��
		PRIVILEGE_BIT_MCON		= 27,	//�������(���)
		PRIVILEGE_BIT_DEV		= 28,	//�豸����(���)
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

		// nErrCode:������룬Ϊ0���¼�ɹ�����Ϊ0���¼ʧ�ܣ���ΪERR_IMS_CLIENTAUTH(20013)ʱ����ʾ
		// ����֤ʧ�ܣ���ʱhr�����壬�ɽ���hr�еĺ�������ʾ������Ϣ�����������hr��ΪS_OK��������Դ�
		// �߼��е����������ڴ˴�ע��
		virtual void OnLogin(int nErrCode, HRESULT hr) = 0;
		virtual void OnLogout(int nErrCode) = 0;

		// ������ strDomain:������ ��iLevel:strDomain����ڱ���ļ�������0(ͬ��),1(�ϼ�)��-1(�¼�)
		virtual void OnDomainConnected(const std::string& strDomain,const int& iLevel){};
		/* ĳ�������ӶϿ� */
		virtual void OnDomainDisconnect(const std::string& strDomain,int nFromLevel){};
		virtual void OnCallCenterServiceItem(const std::string& strUserID, const std::string& strUserName, const std::string& strNodeID){};
		virtual void OnGetURLItem(const std::string& strName,const std::string& strURL,const std::string& strParams){};
		virtual void OnGetURLListEnd(){};
		// ����Host��ַ
		virtual void OnWEBServer(const std::string& strHost){};

		/* ��÷�����ʵʱʱ�� */
		virtual void OnServerRealTime(const std::string& strServerTime){};

		/* �յ��ն˹�����־���� */
		virtual void OnFaultLogEnable(bool bEnable){};

		virtual void OnGetBusinessInfoEnd(){};

		/* �汾��ƥ�� */
		virtual void OnServerVersionMismatch(const int nCmd){};
		/* ����ռ���ʹ��� */
		virtual void OnBandwidthOccupancyRate(const int nOccupancyRate){};
	};

	// UCC7-IM��¼����ʵ�ֲ��
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

		// ��ȡ�������ķ�����Ϣ
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

		// �����������ӵ�ַ
		virtual void GetUrlList() = 0;

		/* ��÷�����ʵʱʱ�� */
		virtual void GetServerRealTime(void) = 0;

		/* ��д��������־ */
		virtual void UploadFaultLog(const std::string& strLog) = 0;

		/* ��ȡ�������͵�����ҵ����Ϣ�б� */
		virtual void GetAllBusinessInfo(IMS_BUSINESSINFO_ITEM_MAP& mapBusinessInfo) = 0;

		/* �����ȡҵ����Ϣ */
		virtual void GetBusinessInfo() = 0;
	};
}
