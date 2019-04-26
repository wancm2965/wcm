#pragma once

#include "HPMC_Client.h"
#include "KMONDefine.h"

typedef enum
{
	AVCON_NORMAL = 0,		//IMS+MMS+MON
	AVCON_MONITOR,			//MON
	AVCON_MEETING,			//IMS+MMS
}AVCON_TYPE;

/*
*	CMonClient回调接口
*/

class CIHPMCNotify
{
public:
	virtual void OnHiteAvfonDlg(bool bHide) = 0;

	virtual void SetCurRoomStatus(_ROOM_STATUS nStatus)=0;	//0:无，1：在会议，2：在AView;
	virtual _ROOM_STATUS GetCurRoomStatus(void)=0;			//0:无，1：在会议，2：在AView;
	virtual void GetMonData(bool bRefresh = false) = 0;						//向服务器发送取数据命令
	virtual bool IsGetMonData(void) = 0;
	//virtual MapMonGroupItem GetMonGroupItemMap(void)=0;	
	virtual MapMonGroupItem     GetMonGroupItemMap(const std::string& strDomain)=0;
	virtual MapMonDomainGroupItem GetMonDomainGroupItemMap()=0;
	//virtual MapMonChannelItem GetMonChannelItemMap(void)=0;
	virtual MapMonChannelItem GetMonChannelItemMap(const std::string& strDomain)=0;
	virtual MapMonDomainChannelItem GetMonDomainChannelItemMap()=0;
	virtual GROUPINFO* GetMonGroupItem(const std::string& groupid) = 0;
	virtual GROUPINFO* GetMonDevItem(const std::string& devid) = 0;
	virtual CHANNELINFO* GetMonChannelItem(const std::string& channelid) = 0;

	//镜头分组
	virtual void DEV_AddMyGroup(std::string& strTmpGrpName)=0;
	virtual void DEV_DelMyGroup(const std::string& strTmpGrpID, std::string& strTmpGrpName)=0;
	virtual void DEV_RenameMyGroup(const std::string& strTmpGrpID, std::string& strTmpGrpName)=0;
	virtual void DEV_AddMyGroupMember(const std::string& strTmpGrpID,std::string& strMemberID)=0;
	virtual void DEV_DelMyGroupMember(const std::string& strTmpGrpID,std::string& strMemberID)=0;
	virtual void DEV_GetMyGroup()=0;
	virtual void DEV_GetMyGroupMember(const std::string& strTmpGrpID)=0;
	//电子地图
	virtual void AddMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void UpdateMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	virtual void DeleteMapInfo(const std::string& strGroupID)=0;
	virtual void GetMapInfo(const std::string& strGroupID)=0;
	virtual void AddBzInfo(const std::string& strGroupID,const std::string& strTreeID, const std::string& strBzID)=0;
	virtual void DeleteBzInfo(const std::string& strGroupID,const std::string& strTreeID)=0;
	virtual void GetBzInfo(const std::string& strGroupID)=0; 

	virtual void GetLoginAddress(CString& strAddress,unsigned short& urPort)=0;
	virtual bool GetPriv(int privilegebit)=0;

	virtual std::string GetUserPassword(void)=0;
	virtual void MenuFileLogout() = 0;

	virtual void OnEnterRoom(unsigned int errcode, const std::string& errText = "")=0;
	virtual void ProStartLoginMeeting_TVC()=0;
	virtual HWND GetMeetingListWnd() =0;
	virtual IDeviceClient* GetDeviceClientSession(void)=0;
	virtual CString GetAVCONHomePage()=0;
};

class CIHPMC
{
public:
	virtual bool Connect() = 0;
	virtual void SetXPESys(bool bIsXPE) = 0;
	virtual void EnterHPMC(const std::string& strDomain,const std::string& roomid, const std::string& password, const std::string& strMemberID, 
		const std::string& strMemberName, const std::string& strLocalIP, const std::string& strServerIP, unsigned short usServerPort, const std::string& strIPType) = 0;
	virtual void ShowHPMC() = 0;
	virtual bool IsEnterHPMC() = 0;
	virtual void ReleaseConnections() = 0;
	virtual void CloseSubWindow() = 0;

	virtual void OnChannelInfo(const CHANNELINFO& item) = 0;
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;

	virtual void OnChannelInfoEnd(const std::string& strDomain) = 0;
	virtual void OnGetGroupInfoEnd(const std::string& strDomain) = 0;
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, unsigned long ulGroupID)=0;
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName, unsigned long ulGroupID)=0;

	virtual void OnRcvP2PData(const std::string&strNodeID,KCmdPacket& packet)=0;

	//镜头分组
	virtual void OnDEVAddMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVDelMyGroup(const std::string& strTmpGrpID)=0;
	virtual void OnDEVRenameMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVAddMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;
	virtual void OnDEVDelMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;
	virtual void OnDEVGetMyGroup(const std::string& strTmpGrpID, std::string strTmpGrpName)=0;
	virtual void OnDEVGetMyGroupMember(const std::string& strTmpGrpID,std::string strMemberID)=0;

	//监控报警
	virtual void ProcessMonAlertData(const std::string strChannelID,unsigned short usType,unsigned short usSubtype)=0;

	virtual void ProcessOnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;
	virtual void ProcessOnUDPRcvGHJInfo(const UDP_RCV_GHJ_INFO& info)=0;

	//电子地图
	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;
	//根据树节点ID得到关联的热点ID
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;
	virtual void SetMonClientMCU(MMS_MCU* mcu)=0;
	virtual void SetAVCONType(AVCON_TYPE type)=0;
	virtual void OnScreenControlOnKeyDown(MSG* pMsg)=0;


	virtual void ProTVCInitTreeMember(const std::string& strRoomID,const std::string& strRommName,const unsigned short& uRoomType)=0;
	virtual string GetTVCMemberRoom()=0;
	virtual void SetTVCMemberRoom(const string& strRoomID)=0;

	////////////////////IDeviceClientNotify//////////////////////////////////////
	/*
	设备上线成功回调
	在此进行通道发布
	allow_count:允许发布的通道个数
	*/
	virtual void OnDeviceOnline(int allow_count)=0;
	/*
	设备下线通知
	*/
	virtual void OnDeviceOffline(void)=0;
	/*
	通道上线成功回调
	*/
	virtual void OnChannelOnline(void)=0;
	/*
	通道下线通知
	*/
	virtual void OnChannelOffline(void)=0;
	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;
	virtual void OnP2P(const std::string& strUserID,KCmdUserDataPacket& packet)=0;
};

AFX_EXT_CLASS CIHPMC* CreateHPMC(CIHPMCNotify& rNotify);
