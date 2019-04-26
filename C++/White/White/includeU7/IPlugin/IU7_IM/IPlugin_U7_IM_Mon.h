// UCC7-IM��ط�������ʵ�ֲ��
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "AVCONSys/KIMSDefine.h"
#include "AVCONSys/KMONDefine.h"

namespace AVCONPlugin
{
	typedef std::map<std::string,MONGROUP> MAP_MONGROUP;
	typedef MAP_MONGROUP::iterator MAP_MONGROUP_IT;

	typedef std::map<std::string,MONDEV> MAP_MONDEV;
	typedef MAP_MONDEV::iterator MAP_MONDEV_IT;

	typedef std::map<std::string,MONCHANNEL> MAP_MONCHANNEL;
	typedef MAP_MONCHANNEL::iterator MAP_MONCHANNEL_IT;

	typedef std::map<std::string,MMS_ROOM_LST> MAP_ROOM_LST;
	typedef MAP_ROOM_LST::iterator MAP_ROOM_LST_IT;

	class IPlugin_U7_IM_Mon_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Mon_Observer(){};

	public:
		virtual void OnGroupInfo(const MONGROUP& item, int nDomainLevel){};
		virtual void OnGroupInfoEnd(const std::string& strDomain){};
		virtual void OnDevInfo(const MONDEV& item, const int nDomainLevel, bool bChngStatus){};
		virtual void OnDevInfoEnd(const std::string& strGroupID){};
		virtual void OnChannelInfo(const MONCHANNEL& item, int nDomainLevel, bool bChngStatus){};
		virtual void OnChannelInfoEnd(const std::string& strDevID){};
		virtual void OnMonDevCount(const std::string& strGroupID,unsigned int nOnlineCount,unsigned int nAllCount,const std::string& strDomain){};
		virtual void OnMonAlert(const std::string& strChannelID,unsigned short usType,unsigned short usSubtype){};
		virtual void OnDeviceOnline( int nAllowCount ){};
		virtual void OnDeviceClientLogin(int nErrCode){};

		virtual void OnGetDomainList(const DOMAININFOLIST& lstDomainInfo){};
		//virtual void OnGetAppointDomainRoomList(const std::string& strDomain,const MMS_ROOM_LST& lstRoom){};
		virtual void OnGetAppointDomainRoomListEnd(const std::string& strDomain){};
	};

	// UCC7-IM��ط�������ʵ�ֲ��
	class IPlugin_U7_IM_Mon : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Mon(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Mon_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Mon_Observer* pObserver) = 0;

		/* ��ȡ�����Դ�� */
		virtual void GetGroupInfo(void) = 0;
		/* ��ȡ��ǰ���е��豸 */
		virtual void GetDevInfo(const std::string& strGroupID) = 0;
		/* �����豸�µ�ͨ�� */
		virtual void SetChannelInfo(const MONCHANNEL& rMonChnn) = 0;
		/* ��ȡ��ǰ�豸�µ�ͨ�� */
		virtual void GetChannelInfo(const std::string& strDevID) = 0;
		/* �����豸ͨ��״̬֪ͨ
			lstDev:�豸�б� eOperation:�������� ȡֵ���£�
			ONLYREAD = 0,		//��ȡ����
			ONLYWRITE,			//������
			READANDWRITE,	//��ȡ�����ֶ���*/
		virtual void SubscribeStatusMsg(const std::list<std::string>& lstDev, const OPERATION &eOperation) = 0;
		/* ȡ�������豸ͨ��״̬֪ͨ */
		virtual void UnsubscribeStatusMsg(const std::list<std::string>& lstDev) = 0;
		/* ��ȡ�ü�ط����µ��豸�����������豸����*/
		virtual void GetMonDevCount() = 0;

		virtual MAP_MONGROUP& GetMonGroupList(void) = 0;
		virtual MONGROUP* GetMonGroupItem(const std::string& ulGroupID) = 0;
		virtual MAP_MONDEV& GetMonDevList(void) = 0;
		virtual void SetMonDevItem(const MONDEV& rMonDev) = 0;
		virtual MONDEV* GetMonDevItem(const char* cszDevID) = 0;
		virtual MAP_MONCHANNEL& GetMonChannelList(void) = 0;
		virtual MONCHANNEL* GetMonChannelItem(const char* cszChannelID) = 0;
		virtual std::string GetMonDevIP(const char* cszChannelID) = 0;

		// ȫ�������ؽӿ�
		virtual void GetDomainList(void) = 0;
		virtual void LoadAppointDomainRoomList(const std::string& strDomain="") = 0;
		virtual MAP_ROOM_LST& GetAppointDomainRoomList(void) = 0;
 
		virtual std::string GetMacAddr(void) = 0;
		virtual std::string AnotherGetMacAddr(void) = 0;
		virtual BOOL IsLocalAdapter(char *pAdapterName) = 0;
		virtual CString GetLocalVersion(void) = 0;

		/* ��ȡ������Ϣ */
		virtual void GetDeviceInfo(std::string &strDevID, std::string &strDomain) = 0;
		virtual unsigned long AVGenerateSSRC(void) = 0;

		virtual void DeviceOnline(MON_DEVTYPE eDevType, const std::string &strDevID, const std::string &strPwd, const std::string &strName, const std::string &strMask, 
			const std::string &strVersion="", const std::string &strDomain="") = 0;
		virtual void Login(const std::string& strUserID, const std::string& strPassword, MON_DEVTYPE eDevType,const std::string& strDevName, const std::string& strSerialno,
			unsigned short usTermType=0, const std::string& strTermName="") = 0;
		virtual void Logout(void) = 0;
		virtual void UpdateChannelName(const std::string& strChannelID,const std::string& strName) = 0;
		virtual void ChannelOnline(int nChannelIndex, const std::string &strChannelName, const std::string &mcuid, const std::string &mcuaddr, unsigned short mcuport, unsigned long ulAudioChannel, 
			unsigned long ulVideoChannel, unsigned long ulVideoChannelSub=0, const std::string &strDomain="", const std::string &strDevID="",const std::string& strDevName="") = 0;
		virtual void DeviceOffline(const std::string &strDevID, const std::string &strDomain="") = 0;
		virtual void UpdateDeviceName(const std::string &strDevID,const std::string &strName, const std::string &strDomain="") = 0;
		virtual void ChannelOffline(const std::string &strDevID,unsigned short nCHNum, const std::string &strDomain="") = 0;
		virtual BOOL IsDeviceAlreadyLogin(void) = 0;
		virtual void Release(void) = 0;
		virtual	void GetLocalInfo(std::string &strDevID, std::string &strPwd) = 0;
		// �õ��豸�����Ƿ��Ѿ����ع�
		virtual bool GetMonGroupLoaded() = 0;
	};
}
