// UCC7-IM登录功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "AVCONSys/KIMSDefine.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Friend_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Friend_Observer(){};

	public:
		/* 收到我的信息事件 */
		virtual void OnMyInfo(const IMS_USER_ITEM& MyInfo){};

		/* 收到头像事件 */
		virtual void OnImage(const char* strUserID,const char* strImgType,const char* strImgData){};

		/* 收到状态改变事件 */
		virtual void OnUserStatus(const char* strUserID,const char* strStatus){};

		/****************************************************************
		 * 联系人组类事件
		 ****************************************************************/

		/* 收到联系人组项 */
		virtual void OnGroupItem(const char* strGroup){};

		/* 收到联系人组项列表结束 */
		virtual void OnGroupItemEnd(void){};

		/* 收到新增联系人组事件  */
		virtual void OnGroupInsert(const char* strGroup){};

		/* 收到联系人组更名事件 */
		virtual void OnGroupUpdateName(const char* strOldGroup,const char* strNewGroup){};

		/* 收到删除联系人组事件 */
		virtual void OnGroupDelete(const char* strGroup){};

		/****************************************************************
		 * 联系人类事件
		 ****************************************************************/

		/* 收到联系人项 */
		virtual void OnFriendItem(const IMS_CONTACT_ITEM& item){};

		/* 收到联系人项列表结束事件 */
		virtual void OnFriendItemEnd(void){};

		/* 收到联系人请求添加事件 */
		virtual void OnFriendApply(const char* strFriendID){};

		/* 收到添加联系人错误事件 */
		virtual void OnFriendApplyError(const char* strFriendID){};

		/* 收到联系人接纳请求事件 */
		virtual void OnFriendAdmit(const char* strFriendID){};

		/* 收到对方拒绝加自己为好友事件 */
		virtual void OnFriendReject(const std::string& strFriendID){};

		/* 收到联系人更换组事件 */
		virtual void OnFriendUpdateGroup(const char* strFriendID,const char* strGroup){};

		/* 收到删除联系人事件 */
		virtual void OnFriendDelete(const char* strFriendID){};

		/* 收到搜索联系人结果事件 */
		virtual void OnFriendSearch(const std::string& strOrgID, const std::string& strOrgName,
			const std::string& strFriendID, const std::string& strFriendName,
			const std::string& strDepartName, const unsigned short& usTermType,
			const std::string& strTermname, const std::string& strUserInfo,
			const std::string& strGender, const std::string& strProvince,
			const std::string& strCity, const std::string& strNodeID, 
			const std::string& nStatus){};

		/* 收到搜索联系人结果结束事件 */
		virtual void OnFriendSearchEnd(){};

		/* 收到获取用户信息结果事件 */
		virtual void OnGetSysUserInfo(const IMS_USER_ITEM& item){};

		/****************************************************************
		 * 机构类事件
		 ****************************************************************/

		/* 收到机构项 */
		virtual void OnOrgItem(const char* strOrgID,const char* strOrgName){};

		// 获取机构列表
		virtual void OnGetOrgList(const ORGINFOLIST& lstOrgInfo){};

		/* 收到机构项结束 */
		virtual void OnOrgItemEnd(void){};

		/* 收到机构部门项 */
		virtual void OnOrgDepartItem(const char* strOrgID,const char* strDepartID,const char* strDepartName,int nLevelID,const char* strUpgradeID,int nOrderID){};

		/* 收到机构部门项结束 */
		virtual void OnOrgDepartItemEnd(const std::string& strOrgID,const std::string& strDepartID){};

		/* 收到机构部门用户项 */
		virtual void OnOrgDepartUserItem(const char* strOrgID,const char* strDepartID,const IMS_USER_ITEM& item,int nOrderID){};

		/* 收到机构部门用户项结束 */
		virtual void OnOrgDepartUserItemEnd(const std::string& strOrgID,const std::string& strDepartID){};

		/* 收到结构列表结束 */
		virtual void OnOrgListEnd(void){};

		/* 收到机构用户详细信息 */
		virtual void OnGetOrgUserInfo(const IMS_CONTACT_ITEM& item){};

		/****************************************************************
		 * 消息类事件
		 ****************************************************************/

		/* 收到在线消息事件 */
		virtual void OnOnlineMessage(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const XMSG& xmsg){};
		
		/* 收到离线消息事件 */
		virtual void OnOfflineMessage(const std::string& strUserID,const XOFLMSG& xmsg){};
		
		/* 收到离线消息结束事件 */
		virtual void OnOfflineMessageEnd(){};

		/* 收到系统消息事件 */
		virtual void OnSystemMessage(const char* strDomain, const char* strAdminName, const char* strMsgTime,
			 const char* strHead, const char* strMessage, const char* strMsgFormat){};

		/* 收到系统消息结束事件 */
		virtual void OnSystemMessageEnd(const char* strDateTime){};

		/* 收到机构消息事件 */
		virtual void OnOrgMessage(const char* strOrgID,const char* strOrgName,
			const char* strDepartID,const char* strDepartName, const char* strUserID,
			const char* strUserName,const char* strNodeID, const char* strMsgTime, 
			const char* strMessage, const char* strMsgFormat){};

		/*收到请求加入会议的回调*/
		virtual void OnRecvIntoMeetingRequset(const std::string& strUserID, const std::string& strNodeID, const std::string& strUserName, bool bNoAsk){};
		/*处理是否同意加入会议的回调*/
		virtual void OnProcessIntoMeetingRequest(bool bAgree, const std::string& strUserName, bool bPresider, const std::string& strDomainID,
			const std::string& strRoomID, const std::string& strRoomPwd, const std::string& strGroupID, const std::string& strMeetingType, const std::string& strErrorText){};

		/* 收到我的权限 */
		virtual void OnMyPriv(){};
};

	// UCC7-IM登录功能实现插件
	class IPlugin_U7_IM_Friend : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_Friend(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_Friend_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_Friend_Observer* pObserver) = 0;

		// 加载好友列表
		virtual void LoadFriendList(void) = 0;
		// 加载机构列表
		virtual void LoadOrgList(void) = 0;
		// 机构列表是否已经加载完成,完成则返回true
		virtual bool IsLoadOrgListEnd(void) = 0;
		// 获取机构人员详细信息
		virtual void GetOrgUserInfo(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID) = 0;
		// 设置登录用户状态，具体参数详见U7_IMSDefine.h文件中描述
		virtual void ModifyStatus(const char* strStatus,const std::string& strTermType="") = 0;
		// 修改登录用户密码
		virtual void ModifyPassword(const char* strPassword) = 0;
		// 修改登录用户信息
		virtual void SetMyInfo(const IMS_USER_ITEM& item) = 0;
		// 设置头像
		virtual void SetImage(const char* strImgType, char* strImgFile) = 0;
		// 获取头像
		virtual void GetImage(const char* strFriendid) = 0;
		// 添加一个好友分组， strGroup 为分组名
		virtual void InsertGroup(const char* strGroup) = 0;
		// 修改好友分组名
		virtual void UpdateGroupName(const char* strOldGroup,const char* strNewGroup) = 0;
		// 删除一个好友分组
		virtual void DeleteGroup(const char* strGroup) = 0;

		// 搜索好友，搜索结果只返回整个结果列表中从起始索引，到结束索引的项
		virtual void SearchFriend(const std::string& strOrgID="", const std::string& strFriendID="", 
			const std::string& strFriendName="", const std::string& strDepartName="",
			const std::string& strUserInfo="", const std::string& strAddress="",
			const std::string& strGender="",int nStartID=0, int nCount=20, int nStatus=1) = 0;

		// 获取用户信息
		virtual void GetSysUserInfo(const std::string& strUserID) = 0;

		// 添加好友进入某个好友分组
		virtual void AddFriend(const char* strFriendID,const char* strGroup) = 0;
		// 接纳联系人到某个分组
		virtual void AdmitFriend(const char* strFriendID,const char* strGroup) = 0;
		// 拒绝对方加自己为好友
		virtual void RejectFriend(const char* strFriendID) = 0;
		// 删除联系人
		virtual void DelFriend(const char* strFriendID) = 0;
		// 更改好友所在组
		virtual void UpdateFriendGroup(const char* strFriendID,const char* strGroup) = 0;
		
		// 发送消息到指定对方
		virtual void SendIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, const char* strHead = "") = 0;
		// 发送机构消息
		// strDepartID：为空则向机构所有成员发送，否则向此部门发送
		// bChildDepart：向部门发送时，是否向此部门的子部门发送
		virtual void SendOrgMessage(const char* strOrgID,const char* strDepartID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, bool bChildDepart) = 0;

		// 获取离线消息
		virtual void GetOfflineMessage(void) = 0;
		// 获取某个时间之后的系统公告，时间格式为 2012-01-01 00:00:00
		virtual void GetSysMsg(const char* strMsgTime) = 0;

		// 数据相关操作
		// 获取某个好友中或机构成员中联系人的信息
		virtual const PIMS_CONTACT_ITEM GetUserItem(const std::string& strUserID) = 0;	
		// 获取服务器地址、端口号
		virtual void GetSvrInfo(std::string& strHost, unsigned short& usPort) = 0;
		// 获取当前用户ID
		virtual const char* GetUserID(void) = 0;
		virtual const char* GetPassword(void) = 0;
		// 获取当前用户ID（包含域名称）
		virtual const char* GetUserIDWithDomain(void) = 0;
		// 获取当前用户名字
		virtual std::string GetUserName(void) = 0;
		// 获取当前用户信息
		virtual const IMS_CONTACT_ITEM& GetMyInfo(void) = 0;

		// 获取好友分组列表
		virtual const IMS_FRIEND_GROUP_MAP& GetFriendGroupList(void) = 0;
		// 获取好友列表，需要在OnFriendItemEnd之后使用
		virtual const IMS_CONTACT_ITEM_MAP& GetFriendList(void) = 0;
		// 获得所有机构列表(包含下级机构)
		virtual void GetAllOrgList(void) = 0;
		// 获取机构列表
		virtual const IMS_ORG_ITEM_MAP& GetOrgList(void) = 0;
		// 获取扩展的机构列表（包含机构的层级关系）
		virtual const IMS_ORG_ITEM_EX_MAP& GetOrgListEx(void) = 0;
		// 获取机构部门列表
		virtual const IMS_DEPART_ITEM_MAP& GetDepartList(void) = 0;
		// 获取机构成员列表
		virtual const IMS_ORGUSER_ITEM_EX_MAP& GetOrgUserList(void) = 0;
		// 获取某个好友中或机构成员中扩展信息
		virtual const PIMS_ORGUSER_ITEM_EX GetUserExItem(const std::string& strUserID) = 0;
		//获取部门信息
		virtual const PIMS_DEPART_ITEM GetDepartItem(const std::string&strDepartID)=0;

		// 申请进入好友会议
		// 是否需要询问
		virtual void RequsetIntoUserMeeting(const std::string& strUserID, const std::string& strNodeID, bool bNoAsk = false) = 0;
		//处理申请进会消息,需要传入会议信息
		virtual void ProcessIntoUserMeeting(const std::string& strUserID,const std::string& strNodeID, bool bAgree,bool bPresider,const std::string& strDomainID,const std::string& strRoomID, 
											const std::string& strRoomName,const std::string& strRoomPwd, const std::string& strGroupID, const std::string& strMeetingType, const std::string& strErrorText) = 0;
	};
}
