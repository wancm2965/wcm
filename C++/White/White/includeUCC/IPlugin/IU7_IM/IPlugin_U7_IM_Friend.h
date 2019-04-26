// UCC7-IM登录功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "KIMSDefine.h"
#include "U7_IMSDefine.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_Friend_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_Friend_Observer(){};

	public:
		/* 收到我的信息事件 */
		virtual void OnMyInfo(const IMS_CONTACT_ITEM& MyInfo) = 0;

		/* 收到头像事件 */
		virtual void OnImage(const char* strUserID,const char* strImgType,const char* strImgData) = 0;

		/* 收到状态改变事件 */
		virtual void OnUserStatus(const char* strUserID,const char* strStatus) = 0;

		/****************************************************************
		 * 联系人组类事件
		 ****************************************************************/

		/* 收到联系人组项 */
		virtual void OnGroupItem(const char* strGroup) = 0;

		/* 收到联系人组项列表结束 */
		virtual void OnGroupItemEnd(void) = 0;

		/* 收到新增联系人组事件  */
		virtual void OnGroupInsert(const char* strGroup) = 0;

		/* 收到联系人组更名事件 */
		virtual void OnGroupUpdateName(const char* strOldGroup,const char* strNewGroup) = 0;

		/* 收到删除联系人组事件 */
		virtual void OnGroupDelete(const char* strGroup) = 0;

		/****************************************************************
		 * 联系人类事件
		 ****************************************************************/

		/* 收到联系人项 */
		virtual void OnFriendItem(const IMS_CONTACT_ITEM& item) = 0;

		/* 收到联系人项列表结束事件 */
		virtual void OnFriendItemEnd(void) = 0;

		/* 收到联系人请求添加事件 */
		virtual void OnFriendApply(const char* strFriendID) = 0;

		/* 收到添加联系人错误事件 */
		virtual void OnFriendApplyError(const char* strFriendID) = 0;

		/* 收到联系人接纳请求事件 */
		virtual void OnFriendAdmit(const char* strFriendID) = 0;

		/* 收到联系人更换组事件 */
		virtual void OnFriendUpdateGroup(const char* strFriendID,const char* strGroup) = 0;

		/* 收到删除联系人事件 */
		virtual void OnFriendDelete(const char* strFriendID) = 0;

		/* 收到搜索联系人结果事件 */
		virtual void OnFriendSearch(const char* strFriendID, const char* strUserName, const char* strGender,const char* strAge) = 0;

		/****************************************************************
		 * 机构类事件
		 ****************************************************************/

		/* 收到机构项 */
		virtual void OnOrgItem(const char* strOrgID,const char* strOrgName) = 0;

		/* 收到机构项结束 */
		virtual void OnOrgItemEnd(void) = 0;

		/* 收到机构部门项 */
		virtual void OnOrgDepartItem(const char* strOrgID,const char* strDepartID,const char* strDepartName,int nLevelID,const char* strUpgradeID,int nOrderID) = 0;

		/* 收到机构部门项结束 */
		virtual void OnOrgDepartItemEnd(void) = 0;

		/* 收到机构部门用户项 */
		virtual void OnOrgDepartUserItem(const char* strOrgID,const char* strDepartID,const IMS_CONTACT_ITEM& item,int nOrderID) = 0;

		/* 收到机构部门用户项结束 */
		virtual void OnOrgDepartUserItemEnd(void) = 0;

		/* 收到结构列表结束 */
		virtual void OnOrgListEnd(void) = 0;

		/****************************************************************
		 * 消息类事件
		 ****************************************************************/

		/* 收到在线消息事件 */
		virtual void OnIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		/* 收到系统消息事件 */
		virtual void OnSystemMessage(const char* strDomain, const char* strAdminName, const char* strMsgTime,
			 const char* strHead, const char* strMessage, const char* strMsgFormat) = 0;

		/* 收到系统消息结束事件 */
		virtual void OnSystemMessageEnd(const char* strDateTime) = 0;
		
		/* 收到机构消息事件 */
		virtual void OnOrgMessage(const char* strOrgID,const char* strOrgName,
			const char* strDepartID,const char* strDepartName, const char* strUserID,
			const char* strUserName,const char* strNodeID, const char* strMsgTime, 
			const char* strMessage, const char* strMsgFormat) = 0;
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
		// 设置登录用户状态，具体参数详见U7_IMSDefine.h文件中描述
		virtual void ModifyStatus(const char* strStatus) = 0;
		// 修改登录用户密码
		virtual void ModifyPassword(const char* strPassword) = 0;
		// 修改登录用户信息
		virtual void SetMyInfo(IMS_CONTACT_ITEM& item) = 0;
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

		//************************************
		// 说明:    搜索好友，搜索结果只返回整个结果列表中从起始索引，到结束索引的项
		// 参数:	item		联系人结构体，包含搜索条件
		// 参数:	nStartID	搜索结果索引的起始
		// 参数:	nEndID		搜索结果索引的结束
		// 参数:	bOnlineOnly	只搜索在线
		// 返回值:  void
		//************************************
		virtual void SearchFriend(const IMS_CONTACT_ITEM& item, int nStartID, int nEndID, bool bOnlineOnly) = 0;
		// 添加好友进入某个好友分组
		virtual void AddFriend(const char* strFriendID,const char* strGroup) = 0;
		// 接纳联系人到某个分组
		virtual void AdmitFriend(const char* strFriendID,const char* strGroup) = 0;
		// 删除联系人
		virtual void DelFriend(const char* strFriendID) = 0;
		// 更改好友所在组
		virtual void UpdateFriendGroup(const char* strFriendID,const char* strGroup) = 0;
		
		// 发送消息到指定对方
		virtual void SendIMMessage(const char* strUserID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		// 发送机构消息
		// strDepartID：为空则向机构所有成员发送，否则向此部门发送
		// bChildDepart：向部门发送时，是否向此部门的子部门发送
		virtual void SendOrgMessage(const char* strOrgID,const char* strDepartID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat, bool bChildDepart) = 0;

		// 获取某个时间之后的系统公告，时间格式为 2012-01-01 00:00:00
		virtual void GetSysMsg(const char* strMsgTime) = 0;
		// 获取离线消息
		virtual void GetOfflineMsg(void) = 0;

		// 数据相关操作
		// 获取某个好友中或机构成员中联系人的信息
		virtual const PIMS_CONTACT_ITEM GetUserItem(const std::string& strUserID) = 0;	
		// 获取当前用户ID
		virtual const char * GetUserID(void) = 0;
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
		// 获取机构列表
		virtual const IMS_ORG_ITEM_MAP& GetOrgList(void) = 0;
		// 获取机构部门列表
		virtual const IMS_DEPART_ITEM_MAP& GetDepartList(void) = 0;
		// 获取机构成员列表
		virtual const IMS_ORGUSER_ITEM_EX_MAP& GetOrgUserList(void) = 0;
		// 获取某个好友中或机构成员中扩展信息
		virtual const PIMS_ORGUSER_ITEM_EX GetUserExItem(const std::string& strUserID) = 0;
		//获取部门信息
		virtual const PIMS_DEPART_ITEM GetDepartItem(const std::string&strDepartID)=0;
	};
}
