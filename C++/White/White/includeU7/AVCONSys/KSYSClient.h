#pragma once

//=======================================================================================
// 网络连接类
// 描述: 是网络通讯的主SESSION,所有通讯的首要建立的类
//=======================================================================================
#include "KXNCP.h"
#include "KBASE/CmdPacket.h"

class AFX_EXT_CLASS KSYSSession
{
public:
	KSYSSession(void){};
	virtual ~KSYSSession(void){};

	/* 处理时间器事件 
	 * 对非WIN32的系统因为不使用时间器,需要外部循环调用该函数以处理已接受到的数据包 */
	virtual void ProcessTimerEvent(void)=0;

	/****************************************************************
	 * 获取本地参数(连接成功后调用)
	 ****************************************************************/

	/* 获得连接的服务器ID */
	virtual std::string GetServerID(void)=0;

	/* 获得服务器类型: SERVER_TYPE_MCU | SERVER_TYPE_MTS | SERVER_TYPE_MCU_MTS */
	virtual unsigned short GetServerType(void)=0;

	/* 获得服务器登录时间 */
	virtual std::string GetServerTime(void)=0;

	/* 获得NAT地址 */
	virtual std::string GetNatAddr(void)=0;

	/* 获得LOCAL地址 */
	virtual std::string GetLocalAddr(void)=0;

	/* 获得LOCAL端口 */
	virtual unsigned short GetLocalPort(void)=0;

	/****************************************************************
	 * 网络协议函数
	 ****************************************************************/

	/* 获得服务器列表,发送到服务器获取服务器列表(MCU/MTS/MCU+MTS)
	   返回事件:OnServerListStart/OnServerList/OnServerListEnd. */
	virtual void GetServerList(const std::string& strUserID)=0;

	/* 获得服务器实时时间 */
	virtual void GetServerRealTime(void)=0;

	/* 设置网络流量
	 * usType: 0=信令 1=音频 2=视频 3=电子白板 4=屏幕视频 5=... 
	 * ulUpStream: 上传网络流量KB
	 * ulDownStream: 上传网络流量KB
	 * strStartTime: 开始统计的时间,格式:YYYY-MM-DD HH:NN:SS
	 * strEditTime:  结束统计的时间,格式:YYYY-MM-DD HH:NN:SS	 */
	virtual void SetTraffic(unsigned short usType,unsigned long ulUpStream,unsigned long ulDownStream,
                            const std::string& strStartTime,const std::string& strEndTime,
							const std::string& strRoomID="")=0;

	/* 获取用户资源 */
	virtual std::string GetUserResource(void)=0;

	/****************************************************************
	 * 内部模块调用
	 ****************************************************************/

	/* 发送到NODE */
	virtual void SetUserID(const std::string& strUserID)=0;

	/* 发送到NODE */
	virtual void SendPacketToNode(const std::string& strNodeID,KCmdPacket& rCmdPacket)=0;

	/* 发送到IMS AGENT */
	virtual void SendPacketToIMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到MMS AGENT */
	virtual void SendPacketToMMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到MON AGENT */
	virtual void SendPacketToMONAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到SMS AGENT */
	virtual void SendPacketToSMSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到FAX AGENT */
	virtual void SendPacketToFAXAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到NDS AGENT */
	virtual void SendPacketToNDSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到REC AGNET */
	virtual void SendPacketToRECAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	/* 发送到定制 AGNET */
	virtual void SendPacketToCTSAgent(KCmdPacket& rCmdPacket,const std::string& strDomain="")=0;

	virtual void SendPacketToRECNode(char* pData, int nLen, const std::string& strNodeID)=0;

	/* 发送P2P数据到NODEID */
	virtual void SendP2PData(const std::string& strNodeID,KCmdPacket& rCmdPacket)=0;
	virtual void SendToCmdManager( KCmdPacket* pCmdPacket )=0;

	virtual void PushPacket(KCmdPacket* pCmdPacket)=0;
};

//=======================================================================================
// 网络连接事件类
//=======================================================================================
class AFX_EXT_CLASS KSYSSessionEvent
{
public:
	KSYSSessionEvent(void){};
	virtual ~KSYSSessionEvent(void){};

public:
	/* 正在连接服务器...... */
	virtual void OnServerConnecting(void)=0;

	/* 已重新连接到服务器 */
	virtual void OnServerReconnected(void)=0;

	/* 服务器网络忙 */
	virtual void OnServerBusy(void)=0;

	/* 未连接到服务器 */
	virtual void OnServerFailed(void)=0;

	/* 与服务器的网络连接已断开
	 * nErrCode: ERR_SYS_NETWORK=正常网络断开; ERR_SYS_SERVERTYPE=登录到错误类型的服务器 */
	virtual void OnServerDisconnected(unsigned int nErrCode,const std::string& strDomain="")=0;

	/* 已连接到服务器 */
	virtual void OnServerConnected(void)=0;

	/* 收到服务器列表开始 */
	virtual void OnServerListStart(void)=0;

	/* 收到服务器列表
	 * strMCUID:		服务器ID
	 * strMCUType:		服务器类型(MCU/MTS/MCU+MTS)
	 * ulConnections:	服务器连接数
	 * strBandWidth:	服务器理论带宽
	 * ulActivity:		服务器活跃度
	 * strIPAddr:		服务器IP地址
	 * usIPPort:		服务器IP端口
	 * strIPType:		服务器IP类型
	 * strIPName:		服务器IP名称 */
	virtual void OnServerList(const std::string& strMCUID,const std::string& strMCUType,
		                      unsigned long ulConnections,const std::string& strBandWidth,unsigned long ulActivity,
							  const std::string& strIPAddr,unsigned short usIPPort,const std::string& strIPName,int nMcuRightType)=0;

	/* 收到服务器列表结束 */
	virtual void OnServerListEnd(void)=0;

	/* 收到服务器实时时间 */
	virtual void OnServerRealTime(const std::string& strServerTime)=0;

	/* 收到服务器客户端版本列表(可能收到多条记录)
	 * strProduct:	产品号,每一个客户端唯一代号,由客户端和WEB端统一确定
	 * strVersion:	产品版本号
	 * bForce:		是否强制更新 */
	virtual void OnClientVersion(const std::string& strProduct,const std::string& strVersion,bool bForce)=0;
   
	/* 接收到P2P数据 */
	virtual void OnRcvP2PData(const std::string& strFromNodeID,KCmdUserDataPacket& rCmdPacket)=0;

	/* 版本不匹配 */
	virtual void OnVersionMismatch(const int nCmd)=0;
	/* 带宽占用率过高 */
	virtual void OnBandwidthOccupancyRate(const int nOccupancyRate)=0;
};

//=======================================================================================
// SYS实例
//=======================================================================================

/* 连接服务器
 * strServerType: 服务器类型SERVER_TYPE_MCU/SERVER_TYPE_MTS
 * strUserID:     用户帐号(用于获取机构分配给该帐号的服务器列表,
 *                当strUserID为空时系统将自动为其挑选最新服务器列表,帐号带域名做为登录域名
 * strResource:   用户资源，用于区别不同系统的用户(如果是图像综合管理系统用户就填空，如果是远程教育培训系统就填edu)
 */
AFX_EXT_CLASS bool SYSSessionConnect(KSYSSessionEvent* pSYSSessionEvent,unsigned short usServerType,const std::string& strUserID,const std::string& strUserResource="");

/* 断开服务器连接 */
AFX_EXT_CLASS void SYSSessionDisconnect(KSYSSessionEvent* pSYSSessionEvent);

/* 获得SYSSession实例 */
AFX_EXT_CLASS KSYSSession& GetSYSSession(void);

//=======================================================================================
// IMS模块类
//=======================================================================================
#include "KIMSDefine.h"
#include "KMMSDefine.h"

class AFX_EXT_CLASS KIMSSession
{
public:
	KIMSSession(void){};
	virtual ~KIMSSession(void){};

	/* SYS网络模块调用获取用户NODEID */
	virtual int OnGetUserNodeID(const std::string& strUserID,std::string& strNodeID)=0;

	/* SYS网络模块调用传入命令包(自用) */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* 获取IM登录状态(自用) 
	 * 返回true为IM在线 */
	virtual bool GetIMLoginStatus(void)=0;

public:


	/****************************************************************
	 * 本地函数
	 ****************************************************************/

	/* 清空本地数据 */
	virtual void ClearLocalData(void)=0;

	/* 获得本地用户帐号 */
	virtual std::string GetUserID(void)=0;

	/* 获得本地用户名 */
	virtual std::string GetUserName(void)=0;

	/* 获得本地用户级别 */
	virtual int GetUserLevel(void)=0;

	/* 获得本地联系人列表 */
	virtual IMS_CONTACT_ITEM_MAP& GetFriendItemMap(void)=0;

	/* 获得本地联系人项 */
	virtual PIMS_CONTACT_ITEM GetFriendItem(const std::string& strFriendID)=0;

	/* 发送初始登录命令 */
	virtual void SendLoginCommand(void)=0;

	/****************************************************************
	 * 功能函数(除Login函数外登录后才能操作)
	 ****************************************************************/

	/* 用户登录
	 * strStatus:   用户登录初始状态 
	 * strIdentifyID 客户端机器唯一识别码
	 * usTermType:	终端类型 0=软终端 1=硬终端 2=手机终端
	 * strTermName: 终端名称 PC Terminal record Digital*/
	virtual void Login(const std::string& strUserID,const std::string& strPassword,const std::string& strStatus,const std::string& strIdentifyID,const std::string& strAuthKey="",unsigned short usTermType=0,const std::string& strTermName="",const bool& Anonymous=false)=0;	

	/* 用户注销 */
	virtual void Logout(void)=0;

	/* 获取在线用户数 */
	virtual void GetOnlineUserCount(const std::string& strDomain="")=0;

	/* 设置我的密码 */
	virtual void SetMyPassword(const std::string& strPassword)=0;

	/* 设置我的姓名 */
	virtual void SetMyName(const std::string& strMyName)=0;

	/* 设置我的信息 */
	virtual void SetMyInfo(const IMS_USER_ITEM& item)=0;

	/* 获取我的信息 */
	virtual void GetMyInfo(void)=0;

	/* 获取权限 */
	virtual void GetMyPriv(void)=0;

	/* 设置头像 */
	virtual void SetImage(const std::string& strImgType,const std::string& strImgFile)=0;

	/* 获取头像 */
	virtual void GetImage(const std::string& strFriendid)=0;

	/* 设置登录用户状态 */
	virtual void SetStatus(const std::string& strStatus,const std::string& strTermType="")=0;

	/****************************************************************
	* 上传故障日志 */
	/****************************************************************/
	virtual void UploadFaultLog(const std::string& strLog)=0;

	/****************************************************************
	 * 联系人组类函数
	 ****************************************************************/

	/* 获取好友组列表 */
	virtual void FetchGroups(void)=0;

	/* 添加好友组 */
	virtual void InsertGroup(const std::string& strGroup,unsigned int nIndex)=0;

	/* 更改好友组名称 */
	virtual void UpdateGroupName(const std::string& strOldGroup,const std::string& strNewGroup)=0;

	/* 更改好友组排序索引 */
	virtual void UpdateGroupIndex(const std::string& strGroup,unsigned int nIndex)=0;

	/* 删除好友组 */
	virtual void DeleteGroup(const std::string& strGroup)=0;

	/****************************************************************
	 * 联系人类函数
	 ****************************************************************/

	/* 获取好友列表 */
	virtual void FetchFriends(void)=0;

	/* 获取联系人项 */
	virtual void FetchFriend(const std::string& strFriendID)=0;

	/* 获取联系人信息 */
	virtual void GetFriendInfo(const std::string& strFriendID)=0;

	/* 设置联系人昵称 */
	virtual void SetFriendNickname(const std::string& strFriendID,const std::string& strNickname)=0;

	/* 获取联系人昵称 */
	virtual void GetFriendNickname(const std::string& strFriendID)=0;

	/* 添加联系人 */
	virtual void ApplyFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* 接纳联系人 */
	virtual void AdmitFriend(const std::string& strFriendID,const std::string& strGroup)=0;

	/* 拒绝对方加自己为好友strFriendID */
	virtual void RejectFriend(const std::string& strFriendID)=0;

	/* 更改好友所在组 */
	virtual void UpdateFriendGroup(const std::string& strFriendID,const std::string& group)=0;

	/* 删除好友 */
	virtual void DeleteFriend(const std::string& strFriendID)=0;

	/* 搜索好友 
	 * nStatus: 1 在线（默认） 2 全部 */
	virtual void SearchFriend(const std::string& strOrgID="", const std::string& strFriendID="", 
							  const std::string& strFriendName="", const IMS_SEARCH_TYPE& searchtype=IMS_SEARCH_AND, const std::string& strDepartName="",
							  const std::string& strUserInfo="", const std::string& strAddress="",
							  const std::string& strGender="",int nStartID=0, int nCount=20, int nStatus=1,
							  const std::string& strDomain="")=0;

	/* 获取人员基本信息  */
	virtual void GetSysUserInfo(const std::string& strUserID)=0;

	/* 获取人员运行时信息 */
	virtual void GetSysUserRunInfo(const std::string& strUserID)=0;

	/*获取呼叫中心服务器类型的账号*/
	virtual void GetCallCenterServiceItem(void) = 0;

	/****************************************************************
	 * 机构函数
	 ****************************************************************/

	/* 拉取整个机构,人员列表
	 * strUpdateTime: 上次更新机构列表的时间 */
	virtual void FetchOrgs(const std::string& strUpdateTime)=0;

	/* 获取机构人员详细信息 */
	virtual void GetOrgUserInfo(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID)=0;

    /*获取指定部门下的子部门和人员列表*/
    virtual void GetOrgDepartItems(const std::string& strOrgID, const std::string& strDepartID)=0;

	/****************************************************************
	 * 消息函数
	 ****************************************************************/

	/* 发送即时消息 */
	virtual void SendOnlineMessage(const std::string& strNodeID,const std::string& strToID,XMSG& xmsg)=0;

	/* 发送离线消息 */
	virtual void SendOfflineMessage(const std::string& strToID,XMSG& xmsg)=0;

	/* 发送机构消息
	strDepartID：为空则向机构所有成员发送，否则向此部门发送
	bChildDepart：向部门发送时，是否向此部门的子部门发送*/
	virtual void SendOrgMessage(const std::string& strOrgID,const std::string& strDepartID,XMSG& xmsg,bool bChildDepart)=0;

	/* 获取离线消息 */
	virtual void GetOfflineMessage(void)=0;

	/* 获取系统消息 */
	virtual void GetSystemMessage(const std::string& strDateTime)=0;

	/****************************************************************
	 * 自定义函数
	 ****************************************************************/

    /* 发送用户数据到指定对方 */
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;

	/****************************************************************
	 * 办公类函数
	 ****************************************************************/

	/* 获取URL列表 */
	virtual void FetchURLs()=0;

	/****************************************************************
	 * 地图类函数
	 ****************************************************************/

	/* 增加地图信息 */
	virtual void AddMapInfo(const std::string& strGroupID,const std::string& strMapMD5)=0;

	/* 更新地图信息 */
	virtual void UpdateMapInfo(const std::string& strGroupID,const std::string& strMapMD5)=0;

	/* 删除地图信息 */
	virtual void DeleteMapInfo(const std::string& strGroupID)=0;

	/* 根据树节点ID得到对应的地图ID用于地图比较 */
	virtual void GetMapInfo(const std::string& strGroupID)=0;

	/****************************************************************
	 * 热点类函数
	 ****************************************************************/

	/* 增加热点信息
	   strGroupID: 区域ID
	   strTreeID:  可能是区域或通道ID */
	virtual void AddBzInfo(const std::string& strGroupID,const std::string& strTreeID,const std::string& strBzID)=0;

	/* 删除树节点的关联点信息
	   strGroupID ：区域ID
	   strTreeID  ：可能是区域或通道ID ，strTreeID  为空时，删除所有热点信息	*/
	virtual void DeleteBzInfo(const std::string& strGroupID,const std::string& strTreeID)=0;

	/* 根据区域ID得到关联的热点ID */
	virtual void GetBzInfo(const std::string& strGroupID)=0; 

	/****************************************************************
	 * 监控分区函数
	 ****************************************************************/
	/* 获取监控资源组 */
	virtual void GetMonGroupInfo(DOMAINUPDATETIMELST& lstTime)=0;

	/* 获取当前组中的设备 */
	virtual void GetMonDevInfo(const std::string& strGroupID,const std::string& strUpdateTime, bool bFromPersonlMeeting = false)=0;

	/* 获取当前设备下的通道 */
	virtual void GetMonChannelInfo(const std::string& strDevID,const std::string& strUpdateTime)=0;

	/* 订阅设备通道状态通知
		lstDev:设备列表， eOperation:操作类型 取值如下：
														ONLYREAD = 0,//仅取数据
														ONLYWRITE,//仅订阅
														READANDWRITE,//即取数据又订阅*/
	virtual void SubscribeStatusMsg(const std::list<std::string>& lstDev, const OPERATION &eOperation)=0;

	/* 取消订阅设备通道状态通知
		lstDev:设备列表，strSubscribeDomain：订阅域 */
	virtual void UnsubscribeStatusMsg(const std::list<std::string>& lstDev)=0;

	/* 向服务器搜索设备信息,支持模糊搜索，最多返回50条信息
	 * strDevString		设备ID或设备名称匹配串
	 * strDomain		为空时只搜索本域，否则搜索指定域 */
	virtual void SearchDevInfo(const std::string& strDevString,const std::string& strDomain="")=0;
	
	/* 获取各监控分组下的设备总数和在线设备总数*/
	virtual void GetMonDevCount(void)=0;

	/* 获取各监控分组下的通道总数和在线通道总数*/
	virtual void GetMonCHLCount(void)=0;

	virtual void AddMonTmpGroup(const std::string& strGroupName)=0;
	virtual void DelMonTmpGroup(const std::string& strGroupID)=0;
	virtual void RenameMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void AddMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void DelMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void GetMonTmpGroup(void)=0;
	virtual void GetMonTmpGroupMember(const std::string& strGroupID)=0;

   	/* 获取监控录像计划 */
	virtual void GetMONRecTask(const std::string& strUpdatetime) = 0;
	/* 获取监控通道录像计划 */
	virtual void GetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime) = 0;
	/* 获取监控录像策略(例如:录像的分割类型,大小) */
	virtual void GetMONRecStrategy(const std::string& strUpdatetime) = 0;

	/****************************************************************
	 * 记录类函数
	 ****************************************************************/

	/* 记录历史记录到服务器
	 * nType: 记录类型. 0=点对点文本聊天记录;1=点对机构文本聊天记录;2=图片;3=文件;4=短信;5=音视频呼叫;6=电话呼叫
	 * strFromID: 发起者
	 * strToID: 接收者
	 * strContent: 记录内容 */
	virtual void SaveHistory(int nType,const std::string& strFromID,const std::string& strToID,const std::string& strContent)=0;
	/****************************************************************
	 * 沪江用
	 ****************************************************************/
	virtual void SetUserRemark(const std::string& strUserID,const std::string& strRemark)=0;
	virtual void GetUserRemark(const std::string& strUserID)=0;
	
	/****************************************************************
	* IM群组功能
	****************************************************************/
	/****************************************************************
	* 讨论组
	****************************************************************/
	/* 创建讨论组 */
	virtual void CreatDiscuss(const std::string& name,const std::list<std::string>& lst)=0;
	/* 获取讨论组信息 */
	virtual void GetDiscussAreaData(void)=0;
	/* 获取讨论组成员信息 */
	virtual void GetDiscussMemberInfo(void)=0;
	/* 添加讨论组成员 */
	virtual void AddDiscussMember(const unsigned long& id,const std::list<std::string>& lst)=0;
	/* 退出讨论组 */
	virtual void ExitDiscuss(const unsigned long& id)=0;
	/* 发送讨论组消息 */
	virtual void SendDiscussMessage(const unsigned long& id,const XMSG& xmsg)=0;
	/* 修改讨论组名称 */
	virtual void ModDiscussName(const unsigned long& id,const std::string& name)=0;
	/****************************************************************
	* 群组
	********************************************D********************/
	/* 创建群 */
	virtual void AddTeamArea(const TEAM_DES& team)=0;
	/* 受理请求 */
	virtual void ProcessRequest(const TEAM_REQUEST& apply,const unsigned short& result)=0;
	/*添加成员*/
	virtual void AddTeamMember(const unsigned long& id , const std::list<std::string>& lstMember)=0;
	/*加入群*/
	virtual void JoinTeam(const unsigned long& id,const std::string& strDesc)=0;
	/* 获取群信息 */
	virtual void GetTeamInfo(void)=0;
	/* 获取群成员信息 */
 	virtual void GetTeamMemberInfo(void)=0;
	/* 获取待验证消息 */
	virtual void GetRequestMsg()=0;
	/* 获取验证完毕消息 */
	virtual void GetRequestReturnMsg()=0;
	/* 退出群 */
	virtual void ExitTeam(const unsigned long& id)=0;
	/* 踢出群 */
	virtual void KickTeam(const unsigned long& id , const std::string& strMember)=0;
	/* 解散群 */
	virtual void DissolveTeam(const unsigned long& id)=0;
	/* 发送群消息 */
	virtual void SendTeamMessage(const unsigned long& id,const XMSG& xmsg)=0;
	/* 晋升为管理员 */
	virtual void Promotion(const unsigned long& id,const std::string& strMember)=0;
	/* 降级为普通成员 */
	virtual void DemoteMember(const unsigned long& id,const std::string& strMember)=0;
	/* 更新群信息 */
	virtual void UpdateTeamInfo(const TEAM_DES& team)=0;
	/* 群查询 */
	virtual void FindTeam(const unsigned long& id,const std::string& strName,const std::string& introduce)=0;
	/* 获取群或讨论组某人员信息*/
	virtual void GetTeamDiscussMemberInfo(const std::string& strMemberID)=0;
	/* 向群发送图片或文件 */
	virtual void SendTeamData(PTNDSFileInfo ptNDSFileInfo, const int nType) = 0;
	virtual void RecvTeamData(PTNDSFileInfo ptNDSFileInfo, const std::string& strFilePath, const int nType) = 0;
	virtual void GetTeamShareFile(const unsigned long& ulTeamID) = 0;
	/* 删除群共享文件 */
	virtual	void DelTeamShareFile(const PTNDSFileInfo& ptNDSFileInfo) = 0;
	/* 编辑群共享文件名 */
	virtual	void EditTeamShareFileName(const PTNDSFileInfo& ptNDSFileInfo,const std::string& strNewFilename) = 0;
    /************************************************************************
     * 会议室相关
    /************************************************************************/
    /* 获取用户类型 */
    virtual void FetchUserTypes() = 0;

    /* 发送用户数据到所有人 */
    virtual void SendUserData2All(KCmdPacket& packet)=0;

	/* 是否有绑定的录像服务端 */
	virtual void GetBindRecordServer()=0;

	/************************************************************************
	* 点播服务器
	/************************************************************************/
	/* 获取点播服务器列表 */
	virtual void GetVODSvrList()=0;

	/* 获取本级及以下域列表 */
	virtual void GetLocalDomainList(void)=0;

	/* 获取机构列表 */
	virtual void GetOrgList(void)=0;

    /* 获取机构列表 */
	virtual void GetDepartList(void)=0;

	/* 获取指定域下的监控会议列表 
	* strUpdateTime: 上次更新机构列表的时间 
	* 如果strDomain为空，则默认获取本级会议列表*/ 
	virtual void GetAppointDomainRoomList(const std::string& strUpdateTime,const std::string& strDomain="")=0;

	/* 获取某个通道的信息 */
	virtual void GetAppointDevChannelInfo(const std::string& strDevID,const std::string& strChannelID)=0;

	/************************************************************************
	* 会议监控子系统
	************************************************************************/
	/* 获取domain信息(包含下级域信息) */
	virtual void GetDomainInfo()=0;

	/* 获取指定域MCU信息 */
	virtual void GetMCUInfo(const std::string& strDomain)=0;

	/* 获取设备列表 */
	virtual void GetMCUDeviceInfo(const std::string& strDomain, const std::string& strUpdatetime)=0;

	/* 获取网关列表 */
	virtual void GetMCUMGWInfo(const std::string& strDomain, const std::string& strUpdatetime)=0;

	virtual void SetMonTmpDefaultChannelNo(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;

	/*  获取诊断通道
	nStartID  数据库查询的ID值，开始查询值为0，随后下一次查询 nStartID = nStartID + nCount
	nCount    一次查询数量（目前一个一个查询,外部调用值为1）
	strUserData 用户查询带过来的标识数据，随查询结果回调一起返回，见OnGetVQDChannelInfo
	*/
	//virtual void GetVQDChannelInfo(int nStartID,int nCount,const std::string& strUserData)=0;

	/************************************************************************/
	/* 服务建行客服端获取业务类型信息                                         */
	/************************************************************************/
	virtual void GetBusinessInfo()=0;
	
	/*中石油*/
	//uSearchId, 查询句柄id; strBeginTime, 查询开始起止时间, strEndTime, 查询结束时间 nAlarmType, 报警类型, 返回错误码(0表示成功)
	//virtual int CreateAlarmSearch(unsigned long& ulSearchId, const std::string& strBeginTime, const std::string& strEndTime, const std::string& strAlarmType)=0;
	//uSearchId, 查询句柄id; nBeginRow 查询起始行; nEndRow, 查询结束行; bSort, 排序(true, 正序 false 逆序) 返回错误码(0表示成功)
	//virtual int GetAlarmDoSearch(unsigned long uSearchId, int nBeginRow, int nEndRow, bool bSort=true)=0;
	//uSearchId, 查询句柄id;
	//virtual int DestroyAlarmSearch(unsigned long ulSearchId)=0;

};

//=======================================================================================
// IMS模块事件类
//=======================================================================================
class AFX_EXT_CLASS KIMSSessionEvent
{
public:
	KIMSSessionEvent(void){};
	virtual ~KIMSSessionEvent(void){};

public:
	/* 登录
	 * nErrCode: 登录返回错误码,0=登录成功,
	 * strModuleString: 系统模块字符串: IMS+MMS+MON+SMS or IMS+MMS  */
	virtual void OnLogin(int nErrCode,const std::string& strModuleString)=0;

	/* 注销 
	 * nErrCode:注销返回错误码,0=正常注销 */
	virtual void OnLogout(int nErrCode)=0;

	/* 请求客户端第三方验证 */
	virtual void OnClientAuth(unsigned char ucAuthType,const std::string& strAuthKey,const std::string& strAuthConnection)=0;

	/* 收到在线用户总数 */
	virtual void OnOnlineUserCount(unsigned long ulCount)=0;

	/* 收到终端故障日志开关 */
	virtual void OnFaultLogEnable(bool bEnable)=0;
	/* 收到我的姓名事件 */
	virtual void OnMyName(const std::string& strUserName)=0;

	/* 收到我的信息事件 */
	virtual void OnMyInfo(const IMS_USER_ITEM& ui)=0;

	/* 收到权限码 */
	virtual void OnMyPriv(unsigned long ulPrivCode)=0;

	/* 收到头像事件 */
	virtual void OnImage(const std::string& strUserID,const std::string& strImgType,const std::string& strImgData)=0;

	/* 收到状态事件,bFirst表示是否第一个状态命令 */
	virtual void OnStatus(const std::string& strUserID,const std::string& strStatus,bool bFirst,const std::string& strNodeID)=0;

	/****************************************************************
	 * 联系人组类事件
	 ****************************************************************/

	/* 收到联系人组项 */
	virtual void OnGroupItem(const std::string& strGroup,unsigned int nIndex)=0;

	/* 收到联系人组项列表结束 */
	virtual void OnGroupItemEnd(void)=0;

	/* 收到新增联系人组事件  */
	virtual void OnGroupInsert(const std::string& strGroup,unsigned int nIndex)=0;

	/* 收到联系人组更名事件 */
	virtual void OnGroupUpdateName(const std::string& strOldGroup,const std::string& strNewGroup)=0;

	/* 收到联系人组更改组索引事件 */
	virtual void OnGroupUpdateIndex(const std::string& strGroup,unsigned int nIndex)=0;

	/* 收到删除联系人组事件 */
	virtual void OnGroupDelete(const std::string& strGroup)=0;

	/****************************************************************
	 * 联系人类事件
	 ****************************************************************/

	/* 收到联系人项 */
	virtual void OnFriendItem(const IMS_CONTACT_ITEM& item)=0;

	/* 收到联系人项列表结束事件 */
	virtual void OnFriendItemEnd(void)=0;

	/* 收到联系人信息事件 */
	virtual void OnFriendInfo(const std::string& strFriendID)=0;

	/* 收到联系人请求添加事件 */
	virtual void OnFriendApply(const std::string& strFriendID)=0;

	/* 收到添加联系人错误事件 */
	virtual void OnFriendApplyError(const std::string& strFriendID)=0;

	/* 收到联系人接纳请求事件 */
	virtual void OnFriendAdmit(const std::string& strFriendID)=0;

	/* 收到对方拒绝加自己为好友事件 */
	virtual void OnFriendReject(const std::string& strUserID)=0;

	/* 收到联系人更换组事件 */
	virtual void OnFriendUpdateGroup(const std::string& strFriendID,const std::string& strGroup)=0;

	/* 收到删除联系人事件 */
	virtual void OnFriendDelete(const std::string& strFriendID)=0;

	/* 收到搜索联系人结果事件 */
	virtual void OnFriendSearch(const std::string& strOrgID, const std::string& strOrgName,
								const std::string& strFriendID, const std::string& strFriendName,
								const std::string& strDepartName, const unsigned short& usTermType,
								const std::string& strTermname,	const std::string& strUserInfo,								
								const std::string& strGender, const std::string& strProvince,
								const std::string& strCity, const std::string& strNodeID, 
								const std::string& nStatus)=0;

    /* 收到搜索联系人结果结束事件 */
	virtual void OnFriendSearchEnd()=0;
    
	/* 收到人员基本信息 */
	virtual void OnGetSysUserInfo(const IMS_USER_ITEM& item)=0;

	/* 收到人员运行时信息 */
	virtual void OnGetSysUserRunInfo(const USER_SESSION& item)=0;

	/* 收到呼叫中心服务器的人员类型的信息 */
	virtual void OnGetCallCenterServiceItem(const std::string& strOrgID, const std::string& strDepartID, const std::string& strUserID, const std::string& strUserName, const std::string& strNodeID)=0;
	
	/* 收到呼叫中心服务器的人员类型的信息结束 */
	virtual void OnGetCallCenterServiceItemEnd(void)=0;

	/* 获取联系人昵称 */
	virtual void OnGetFriendNickname(const std::string& strFriendID,const std::string& strNickname)=0;

	/****************************************************************
	 * 机构类事件
	 ****************************************************************/

	/* 收到机构列表开始项 */
	virtual void OnOrgListStart(void)=0;

	/* 收到机构项 */
	virtual void OnOrgItem(const std::string& strOrgID,const std::string& strOrgName)=0;

	/* 收到机构项结束 */
	virtual void OnOrgItemEnd(void)=0;

	/* 收到机构部门项 */
	virtual void OnOrgDepartItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strDepartName,int nLevelID,const std::string& strUpgradeID,int nOrderID,const std::string& strOrgTree)=0;

	/* 收到机构部门项结束 */
	virtual void OnOrgDepartItemEnd(const std::string& strOrgID,const std::string& strDepartID)=0;

	/* 收到机构部门用户项 */
	virtual void OnOrgDepartUserItem(const std::string& strOrgID,const std::string& strDepartID,const IMS_USER_ITEM& item,int nOrderID)=0;

	/* 收到机构部门用户项结束 */
	virtual void OnOrgDepartUserItemEnd(const std::string& strOrgID,const std::string& strDepartID)=0;

	/* 收到机构/部门/用户删除项 */
	virtual void OnOrgDeleteItem(const std::string& strOrgID,const std::string& strDepartID,const std::string& strUserID)=0;

	/* 收到机构/部门/用户删除项结束 */
	virtual void OnOrgDeleteItemEnd(void)=0;

	/* 收到结构列表结束 */
	virtual void OnOrgListEnd(const std::string& strUpdateTime)=0;

	/* 收到机构人员详细信息 */
	virtual void OnGetOrgUserInfo(const IMS_CONTACT_ITEM& item)=0;

	/****************************************************************
	 * 消息类事件
	 ****************************************************************/

	/* 收到在线消息事件 */
	virtual void OnOnlineMessage(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const XMSG& xmsg)=0;

	/* 收到离线消息事件 */
	virtual void OnOfflineMessage(const std::string& strUserID,const XOFLMSG& xmsg)=0;

    /* 收到离线消息结束事件 */
	virtual void OnOfflineMessageEnd()=0;

	/* 收到系统消息事件 */
	virtual void OnSystemMessage(const std::string& strDomain,const std::string& strAdminName,const XMSG& xmsg)=0;

	/* 收到系统消息结束事件 */
	virtual void OnSystemMessageEnd(const std::string& strDateTime)=0;

	/* 收到机构消息事件 */
	virtual void OnOrgMessage(const std::string& strOrgID,const std::string& strOrgName,
		                      const std::string& strDepartID,const std::string& strDepartName,
							  const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,
							  const XMSG& xmsg)=0;

	/****************************************************************
	 * 自定义事件
	 ****************************************************************/

	/* 收到用户自定义数据事件 */
	virtual void OnUserData(KCmdUserDataPacket& packet)=0;

	/****************************************************************
	 * 服务器列表事件
	 ****************************************************************/

	/* 返回WEB服务器地址和端口,客户端解析地址和端口 */
	virtual void OnWEBServer(const std::string& strHost)=0;

	/* 返回PBX服务器地址,字符串格式应该和WEB添加到数据库中的格式一致 */
	virtual void OnPBXServer(const std::string& strHost)=0;

	/* 收到FAX服务,客户端解析地址和端口 */
	virtual void OnFAXServer(const std::string& strHost)=0;

	/****************************************************************
	 * 办公类事件
	 ****************************************************************/

	/* 收到URL列表项 */
	virtual void OnGetURLItem(const std::string& strName,const std::string& strURL,const std::string& strParams)=0;

	/* 收到URL列表项结束 */
	virtual void OnGetURLListEnd()=0;

	/* 收到新传真计数 */
	virtual void OnGetFAXCount(unsigned int nCount)=0;	

	/****************************************************************
	 * 地图类事件
	 ****************************************************************/

	/* 收到回调地图信息
	   strGroupID：区域ID
	   strMapMD5 ：地图MD5值	*/
	virtual void OnMapInfo(const std::string& strGroupID, const std::string& strMapMD5)=0;

	/****************************************************************
	 * 热点类事件
	 ****************************************************************/

	/* 根据树节点ID得到关联的热点ID
	   strGroupID：区域ID
	   listMAPBZ ：区域下所有热点list */
	virtual void OnBzIDInfo(const std::string& strGroupID, MAP_BZ_LST listMAPBZ)=0;

	/****************************************************************
	 * 监控分区事件
	 ****************************************************************/

	virtual void OnMonUpdateTime(const std::string& strUpdateTime,const std::string& strDomain)=0;

	 /* 获取监控资源组回调 */
	virtual void OnGetMonGroupInfo(const MONGROUP& item, const int nDomainLevel)=0;

	 /* 获取监控资源组回调结束 */
	virtual void OnGetMonGroupInfoEnd(const std::string& strDomain)=0;

	/* 获取当前组中的设备回调 */
	virtual void OnGetMonDevInfo(const MONDEV& item, const int nDomainLevel, bool bChngStatus=false)=0;

	/* 获取当前组中的设备回调结束 */
	virtual void OnGetMonDevInfoEnd(const std::string& strGroupID,const std::string& strUpdateTime,bool bFromPersonalMeeting)=0;

	/* 获取当前设备下的通道回调 */
	virtual void OnGetMonChannelInfo(const MONCHANNEL& item, const int nDomainLevel, bool bChngStatus=false)=0;

	/* 获取当前设备下的通道回调结束 */
	virtual void OnGetMonChannelInfoEnd(const std::string& strDevID,const std::string& strUpdateTime)=0;

	/* 向服务器搜索设备信息回调,支持模糊搜索，最多返回50条信息，strDomain为空，则只搜索本域，否则搜索指定域 */
	virtual void OnSearchDevInfo(const MONDEV& item,const std::string& strDomain)=0;

	/* 向服务器搜索设备信息结束回调，参数为搜索时的参数 */
	virtual void OnSearchDevInfoEnd(const std::string& strDevString,int nCount,const std::string& strDomain)=0;

	/* 向服务器搜索设备在线数量和总量的回调 */
	virtual void OnGetMonDevCount(const std::string& strGroupID,const unsigned int& nOnlineCount,const unsigned int& nAllCount,const std::string& strDomain)=0;

	/* 向服务器搜索通道在线数量和总量的回调 */
	virtual void OnGetMonCHLCount(const std::string& strGroupID,const unsigned int& nOnlineCount,const unsigned int& nAllCount,const std::string& strDomain)=0;

	/* 某个域连接断开 */
	virtual void OnDomainDisconnect(const std::string& strDomain,int nFromLevel)=0;
	/* 获取某个通道的信息 */
	virtual void OnGetAppointDevChannelInfo(const MONCHANNEL& item)=0;
	/* 设备通道音视频ID更新 */
	virtual void OnDevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;
	virtual void OnMonAlert(const std::string& strChannelID,unsigned short usType,unsigned short usSubtype)=0;			

	/* 增加监控设备管理分组回调 */
	virtual void OnAddMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnDelMonTmpGroup(const std::string& strGroupID)=0;
	virtual void OnRenameMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnAddMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void OnDelMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID)=0;
	virtual void OnGetMonTmpGroup(const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnGetMonTmpGroupMember(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;
	virtual void OnSetMonTmpDefaultChannelNo(const std::string& strGroupID,const std::string& strMemberID,const std::string& strChannelNo)=0;
	virtual void OnGetMonTmpGroupMemberEnd()=0;

	/* 收到监控设备名称 */
	virtual void OnUpdateDevName(const std::string& strDevID,const std::string& strName, const std::string& strParentID)=0;

	/* 收到监控通道名称 */
	virtual void OnUpdateChannelName(const std::string& strChannelID,const std::string& strName,const std::string& strDevID)=0;

   	/* 获取监控录像计划 */
	virtual void OnGetMONRecTaskItem(const std::string& strChannelID, const std::string& strUpdatetime, const std::string& strRecTask) = 0;
	virtual void OnGetMONRecTaskEnd() = 0;
	/* 获取监控通道录像计划 */
	virtual void OnGetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime, const std::string& strChnlRecTask) = 0;
	/* 获取监控录像策略(例如:录像的分割类型,大小) */
	virtual void OnGetMONRecStrategy(const std::string& strUpdatetime, const std::string& strStrategy) = 0;

		/*讨论组*/
	virtual void OnAddDiscussArea(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnAddDiscussAreaEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnGetDiscussInfo(const unsigned long& ulDiscussID,const std::string& name,const std::string& member)=0;
	virtual void OnGetDiscussInfoEnd(const unsigned long& ulDiscussID)=0;
	/* 返回讨论组成员信息 */
	virtual void OnGetDiscussMemberInfo(const IMS_USER_ITEM& contactItem)=0;
	/* 返回讨论组成员信息结束 */
	virtual void OnGetDiscussMemberInfoEnd(void)=0;
	virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID,size_t ErrorCode)=0;
	virtual void OnExitDiscuss(const unsigned long& ulDiscussID,const std::string& member)=0;
	virtual void OnRecvDiscussMSG(const unsigned long& ulDiscussID,const std::string& FromID,const XMSG& xmsg)=0;
	virtual void OnModDiscussName(const unsigned long& ulDiscussID,const std::string& strName)=0;
	/*群组*/
	/*返回群信息*/
	virtual void OnCreatTeam(const TEAM_DES& team)=0;
	/*返回群信息结束*/
	virtual void OnCreatTeamEnd(void)=0;
	/* 返回群成员信息 */
	virtual void OnGetTeamMemberInfo(const IMS_USER_ITEM& contactItem)=0;
	/* 返回群成员信息结束 */
	virtual void OnGetTeamMemberInfoEnd(void)=0;
	/*服务器发送的待处理消息*/
	virtual void OnTeamRequest(const TEAM_REQUEST& apply)=0;
	/*新成员加入返回*/
	virtual void OnAddTeamMember(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*受理完毕返回*/
	virtual void OnRequestReturn(const TEAM_REQUEST_RESULT& HandleReturn)=0;
	/*离开群组返回*/
	virtual void OnExitTeam(const unsigned long& ulTeamID)=0;
	/*退出成员返回*/
	virtual void OnMinusTeamMember(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*发送群消息返回*/
	virtual void OnRecvTeamMsg(const unsigned long& ulTeamID,const std::string& strFromID,const XMSG& xmsg)=0;
	/*晋升返回*/
	virtual void OnPromotion(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*降级返回*/
	virtual void OnDemote(const unsigned long& ulTeamID,const std::string& strMember)=0;
	/*更改群信息返回*/
	virtual void OnUpdateTeamInfo(const TEAM_DES& team)=0;
	/*查找群后返回*/
	virtual void OnFindTeam(const unsigned long& ulTeamID,const std::string& strName,const std::string& strIntroduce,const size_t& set)=0;
	/*查找完毕后返回*/
	virtual void OnFindTeamEnd()=0;
	/* 获取群或讨论组某人员信息返回*/
	virtual void OnGetTeamDiscussMemberInfo(const std::string& strMemberID,const IMS_USER_ITEM& contactItem)=0;
	/* 群组离线功能回调 */
	virtual void OnSendTeamData(const PTNDSFileInfo ptNDSFileInfo, const unsigned long& ulSerialNO, const unsigned long& ulError) = 0;
	virtual void OnSendTeamDataEnd(const unsigned long& ulFileID) = 0;
	virtual void OnDownloadTeamData(const unsigned long& ulSerialNO, const PTNDSFileInfo ptNDSFileINfo, const unsigned long& ulError) = 0;
	virtual void OnDownloadTeamDataEnd(const unsigned long& ulFileID) = 0;

	virtual void OnReadyDownloadTeamData(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;

	virtual void OnGetTeamShareFile(const unsigned long& ulTeamID, const PTNDSFileInfo ptNDSFileInfo) = 0;
	virtual void OnGetTeamShareFileEnd(const unsigned long& ulTeamID) = 0;
	virtual	void OnDelTeamShareFile(const TNDSFileInfo& tNDSFileInfo,const unsigned short& usErrCode) = 0;
	
	/************************************************************************
     *会议室相关
    /************************************************************************/

    /* 收到用户类型数据开始 */
    virtual void OnUserTypeListStart(void)=0;

    /* 收到用户类型数据 */
    virtual void OnUserTypeItem ( const std::string& strOrgID ,const IMS_USERTYPE_ITEM& Item)=0;

    /* 收到用户类型数据结束 */
    virtual void OnUserTypeListEnd (void)=0;

	/* 是否有绑定的录像服务端 */
	virtual void OnGetBindRecordServer(const unsigned int& unCount)=0;

	/************************************************************************
	*点播服务器
	/************************************************************************/
	/* 收到点播服务器列表 */
	virtual void OnVODSvrList( const IMS_VOD_SVR_LST& listVodSvr)=0;

	
	/****************************************************************
	 * 获取本级及以下域列表
	 ****************************************************************/
	virtual void OnGetLocalDomainList(const DOMAININFOLIST& lstDomainInfo)=0;
	/* 获取机构列表 */
	virtual void OnGetOrgList(const ORGINFOLIST& lstOrgInfo)=0;
    /* 获取部门列表 */
	virtual void OnGetDepartList(const DEPARTINFOLIST& lstDepartInfo)=0;
	/* 获取指定域下的监控会议列表 
	 * strUpdateTime: 上次更新机构列表的时间 
	 * 如果strDomain为空，则默认获取本级会议列表*/
	virtual void OnGetAppointDomainRoomList(const std::string& strDomain,const MMS_ROOM_LST& lstRoom)=0;
	virtual void OnGetAppointDomainRoomListEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

    /* 授权过期 */
    virtual void OnLicenseExperied(unsigned int nErrCode,const std::string& strDomain="")=0;

	/****************************************************************
	 * 沪江用
	 ****************************************************************/
	virtual void OnGetUserRemark(const std::string& strUserID,const std::string& strRemark)=0;

	/************************************************************************
	* 会议监控子系统 
	************************************************************************/
	virtual void OnGetDomainInfo(const SUBSYSTEM_DOMAININFO& item)=0;
	virtual void OnGetDomainInfoEnd()=0;

	virtual void OnGetMCUInfo(const SUBSYSTEM_MCUINFO& item)=0;
	virtual void OnGetMCUInfoEnd(const std::string& strDomain)=0;
	
	virtual void OnGetMCUDeviceInfo(const SUBSYSTEM_DEVICEINFO& item)=0;
	virtual void OnGetMCUDeviceInfoEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

	
	virtual void OnGetMCUMGWInfo(const SUBSYSTEM_DEVICEINFO& item)=0;
	virtual void OnGetMCUMGWInfoEnd(const std::string& strDomain,const std::string& strUpdateTime)=0;

	virtual void OnMCUDeviceStatus(const SUBSYSTEM_DEVICEINFO& item)=0;
    
	/************************************************************************
	* 天津消防地铁监控
	************************************************************************/
    virtual void OnYaAnMonAlert(const std::string& strMemberID,unsigned long ulChannelIndex, bool bPopMsg = false)=0;

	/* 获取当前需要做视频诊断的通道 */
	//virtual void OnGetVQDChannelInfo(const unsigned int& uStartID, const bool& bIsEnd,const VQD_CHANNEL_LST& lstVQDChannel,const std::string& strUserData)=0;

	virtual void OnGetAlarmSearch(unsigned long ulSearchId, unsigned int unAlarmNum)=0;
	virtual void OnGetAlarmLogs(unsigned long ulSearchId, int nBeginRow, int nEndRow, bool bSort, const ALARMLOG_INFO_LST& lstAlarmLog, int nErrorCode)=0;
	virtual void OnDelAlarmSearch(unsigned long ulSearchId)=0;
	/* 域重连 strDomain:域名称 ，iLevel:strDomain相对于本域的级别，其中0（同级）,1(上级)，-1（下级 */
	virtual	void OnDomainConnected(const std::string& strDomain,const int& iLevel)=0;
	virtual void OnGetBusinessInfo(int id, int business_type, int parent_business_id, std::string business_name) = 0;
	virtual void OnGetBusinessInfoEnd() = 0;
};

//=======================================================================================
// IMS实例
//=======================================================================================
AFX_EXT_CLASS bool IMSSessionAddEvent(KIMSSessionEvent* pIMSSessionEvent);
AFX_EXT_CLASS void IMSSessionRemoveEvent(KIMSSessionEvent* pIMSSessionEvent);
AFX_EXT_CLASS void IMSSessionRelease(void);
AFX_EXT_CLASS KIMSSession& GetIMSSession(void);

#include "KMMSDefine.h"

class AFX_EXT_CLASS KMMSSessionEvent;
//=======================================================================================
// MMS模块类
//=======================================================================================
class AFX_EXT_CLASS KMMSSession
{
public:
	KMMSSession(void){};
	virtual ~KMMSSession(void){};

	/* SYS网络模块调用获取用户NODEID */
	virtual int OnGetUserNodeID(const std::string& strUserID,std::string& strNodeID)=0;
	virtual void OnDispatchCmd(KCmdPacket& t)=0;
	virtual void ClearLocalData(void)=0;

	virtual void SetUserID(const std::string& strUserID)=0;
	virtual std::string GetUserID(void)=0;
	virtual void SetUserName(const std::string& strUserName)=0;
	virtual std::string GetUserName(void)=0;

	/****************************************************************
	 * 获取本地会议室列表方法
	 ****************************************************************/
	virtual MMS_ROOM_LST& GetLocalRoomList(void)=0;
	virtual MMS_MEMBER_LST& GetLocalMemberList(void)=0;

	virtual PMMS_ROOM GetLocalRoomItem(const std::string& strDomain,const std::string& strRoomID)=0;
	virtual PMMS_MEMBER_EX GetLocalMemberItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strMemberID)=0;

	/*获取指定会议室的一个在线成员*/
	virtual void GetAppointRoomOnlineMember(const std::string& strRoomID)=0;

	/*获取指定会议室的一个在线合成终端或主持人通道信息*/
	virtual void GetAppointRoomOnlineMemberInfo(const std::string& strRoomID,const std::string& strDomain="",const std::string& strTag="")=0;

	/* 发送初始登录命令 */
	virtual void SendLoginCommand(void)=0;

	/****************************************************************
	 * 沪江用
	 ****************************************************************/
	virtual void SetRoomRemark(const std::string& strRoomID,const std::string& strRemark)=0;
	virtual void GetRoomRemark(const std::string& strRoomID)=0;

	/****************************************************************
	 * 获取IPCAM列表方法
	 ****************************************************************/
	virtual void GetIPCamList(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * 会议室列表方法
	 ****************************************************************/
	/* 激活临时会议
	 * IM登陆成功后调用*/
	virtual void ActiveTemporaryRoom()=0;
	/* 获得会议列表
	 * bAllRoomType=true: 获取所有会议项/组/成员
	 * bAllRoomType=false:只取标准会议项+组+成员+高级会议项*/
	virtual void GetRoomList(bool bAllRoomType=false)=0;

	/* 获得某个会议相关信息
	 *可以取到当前用户所分配到的(包括被邀请的)某个会议室信息*/
	virtual void GetRoomItem(const std::string& strRoomID)=0;

	/****************************************************************
	 * 临时会议室方法
	 ****************************************************************/

	/* 添加临时会议组
	 * strGroupName: 临时会议组名称 */
	virtual void InsertTempGroup(const std::string& strGroupName)=0;

	/* 修改临时会议组名称
	 * ulGroupID: 临时会议组ID
	 * strGroupName: 临时会议组名称 */
	virtual void UpdateTempGroupName(const std::string& strGroupID,const std::string& strGroupName)=0;

	/* 删除临时会议组
	 * ulGroupID: 临时会议组ID */
	virtual void DeleteTempGroup(const std::string& strGroupID)=0;

	/* 添加临时会议成员 */
	virtual void InsertTempMember(const std::string& strGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;

	/* 删除临时会议成员 */
	virtual void DeleteTempMember(const std::string& strGroupID,const std::string& strMemberID)=0;

	/****************************************************************
	 * 会议功能函数
	 ****************************************************************/
	/* 登录会议室
	 * strDomain:			会议室所在域
	 * strRoomID:			会议室ID
	 * strPassword:			需要MD5编码
	 * strInviteeGroupID:	被邀请者组ID
	 * bPresider:			如果为被邀请者,则需要确定被邀请者是否为主持人
	 * strUpdateTime:		会议信息更新时间,送到服务器判断是否一致,如果不一致服务器将传回成员列表
	 * strTermType:			终端类型 0=软终端 1=硬终端 2=手机平板端 3=TR*
	 * strTermName:			终端名称 PC Terminal record Digital MOB TR2..
	 * uDigitalCode			数字功能进会邀请码 */
	virtual void Login(const std::string& strDomain,const std::string& strRoomID,const std::string& strPassword,
		               const std::string& strMemberID,const std::string& strMemberName,
					   const std::string& strInviteeGroupID,bool bPresider=false,
					   const std::string& strUpdateTime="",unsigned short usTermType=0,const std::string& strTermName="",
					   bool bHasVideo=false, bool bInvite= false,const unsigned int& uDigitalCode=0)=0;

	/* 退出会议室 */
	virtual void Logout(const std::string& strRoomID, const bool bAllLogout=false)=0;

	/* 邀请成员与会
	 * strInviteeID:		被邀请者的ID
	 * strPassword:			会议密码
	 * bPresider:			是否邀请为主持人,true=以主持人身份,false=以与会者身份 
	 * ulTempGroupID		临时会议时用，因为要根据此ID得到“临时会议组名称” */
	virtual void Invite(const std::string& strInviteeID,const std::string& strPassword,bool bPresider, const std::string& strTempGroupID = "",const std::string& strTempRoomNickname = "")=0;
	
	
	/*	一键邀请会议所有不在会(但IM在线)成员
	 * strPassword:			所在会议的密码
	 * bPresider:			是否邀请为主持人,true=以主持人身份,false=以与会者身份 */
	virtual void InviteAll(const std::string& strPassword,bool bPresider,const std::string& strTempRoomNickname = "")=0;

	/* 邀请成员到个人会议
	 * strRoomID:			邀请进入的会议ID
	 * strRoomName:			邀请进入的会议名称
	 * strInviteeID:		被邀请者的ID
	 * strPassword:			会议密码
	 * bPresider:			是否邀请为主持人,true=以主持人身份,false=以与会者身份 
	 * ulTempGroupID		临时会议时用，因为要根据此ID得到“临时会议组名称” */

	virtual void InviteToPersonalRoom(const std::string& strRoomID, const std::string& strRoomName,const std::string& strInviteeID,const std::string& strPassword,bool bPresider, const std::string& strDevID, const std::string& strTempGroupID = "")=0;

	/* 拒绝与会邀请 */
	virtual void Reject(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviterNodeID)=0;

	/* 踢出会议 */
	virtual void Kickout(const std::string& strMemberID)=0;

	/* 发送会议消息 */
	virtual void SendMessage(const std::string& strHead,const std::string& strBody,const std::string& strFormat)=0;

	/* 发送用户自定义数据(仅发送到会议当中的成员)
	 * ucObject对象: 
	 *   0:向会议所有主持人(不包括自己)发送数据
	 *   1:向会议激活组所有成员发送数据
	 *   2:向会议所有成员发送数据
	 *   >=3:向packet中的toid发送数据 */
	virtual void SendUserData(KCmdUserDataPacket& packet,unsigned char ucObject=0)=0;

	/****************************************************************
	 * 会议操作函数
	 ****************************************************************/

	/* 邀请进会议并广播 */
	virtual void InviteInRoomBroadcast(const std::string& strMemberName,const std::string& strMeetingName,const std::string& strMeetingPass)=0;

	/* 设置活动会议组ID */
	virtual void SetActiveGroupID(const std::string& strActiveGroupID)=0;

	/* 设置会议组当前屏幕号 */
	virtual void SetCurrentScreenID(const std::string& strGroupID,int nCurScrID,int nPrevTM)=0;

	/* 获取成员通道卡列表(VID/AUD/DOC) */
	virtual void GetCardList(void)=0;

	/* 设置通道卡项,并广播给主持人
	 * strCardType: 缺省为空,如果是3G类型则填入"3G" */
	virtual void SetCardItem(int nCardIndex,const std::string& strCardName,MMS_CHANNEL_LST& lstChannel,const std::string& strCardType="")=0;

	/* 设置音频通道卡项,并广播给主持人 */
	virtual void SetCardAudioItem(int nCardIndex,unsigned long ulAudioChannelID)=0;

	/* 设置通道卡名称(只能设置本地用户卡) */
	virtual void SetCardName(int nCardIndex,const std::string& strCardName)=0;

	/* 广播成员卡 */
	virtual void BroadcastCard(int nScreenID,int nWindowID,const std::string& strMemberID,int nCardIndex,int nChannelType,const std::string& strTag)=0;

	/* 关闭广播成员卡 */
	virtual void CloseBroadcastCard(const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* 切换窗口 */
	virtual void DragWindow(int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* 设置会议模板 */
	virtual bool SetTemplet(int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber)=0;

	/* 设置会议自动模版
	 * bAutoTem=true:自动模版
	 * bAutoTem=false:手动模版*/
	virtual void SetAutoTemplet(bool bAutoTem)=0;

	/* 设置会议屏属性
	 * strAllAttribute所有屏的属性串
	 * strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
	virtual void SetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* 设置会议窗口属性virtual void OnSetTemplet
	* strAllAttribute所有窗口的属性串
	* strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
	virtual void SetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* 设置浮动窗口位置
	 * x = 像素比例 * 100
	 * y = 像素比例 * 100
	 * cx= 宽度比例 * 100
	 * cy= 高度比例 * 100 */
	virtual bool SetFloatWindow(int nScreenID,int nX,int nY,int nCX,int nCY)=0;	

	/* 通用会议参数配置接口 */
	virtual void SetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID, const std::string& strKeyword, const std::string& strParameter, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL)=0;
	
	/* 获取会议参数配置接口 */
	virtual void GetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID)=0;

	/* 删除通用会议参数配置 */
	virtual void DelMeetingParameter(const std::string& strRoomID, const std::string& strGroupID, const std::string& strKeyword, MMS_SYNCOBJECT synObject=SYNCOBJ_ALL, const std::string& strSynAppoint="", MMS_PRMAVLBSTTS prmAvlbStts=PAS_MANUAL)=0;

	/* 设置个人或会议权限 */
	virtual void SetRight(int nRight, bool bEnable, const std::string& strMemberID="")=0;

	/* 获取个人或会议权限 */
	virtual void GetRight(const std::string& strMemberID="")=0;

	/* 获取组同步信息 */
	virtual void GetGroupSynInfo(const std::string& strGroupID)=0;

	/* 允许strGroupID组召开子组会议 */
	virtual void AllowSubMeet(const std::string& strGroupID,const std::string& strMemberID)=0;

	/* 要求strGroupID组及其所有子组与strGroupID组同步 */
	virtual void Sync(const std::string& strGroupID,const std::string& strMemberID)=0;
	
	/* 设置临时主持人 */
	virtual void SetTempPresider(const std::string& strMemberID,bool bSet)=0;

	/* 申请或删除发言人 */
	virtual void SetProlocutor(const std::string& strMemberID="")=0;

	/* 广播或取消广播发言人 */
	virtual void EnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* 申请或删除提问人 */
	virtual void SetQuestioner(const std::string& strMemberID="")=0;

	/* 单接或取消单接提问人*/
	virtual void EnableQuestioner(const std::string& strMemberID,bool bEnable)=0; 

	/* 设置内容 */
	virtual void SetAnnexData(int nType,const std::string& strData,const std::string& strFormat)=0;

	/* 设置 */
	virtual void EnableAnnex(int nType,bool bEnable=true)=0;
	
	/* 
	获取公告、标题内容
	*/
	virtual void GetAnnexData(void)=0;

	/*获取指定会议室的会议终端在线数和总数*/
	virtual void GetRoomMemberCount(const std::string& strRoomID)=0;

	/* 向服务器获取当前会议室的人员列表
		bOnlyOnline＝true  只取在线人员
		bOnlyOnline＝false 取在线和不在线人员 */
	virtual void GetServerMemberList(bool bOnlyOnline=true)=0;

	/****************************************************************
	 * 会议投票函数
	 ****************************************************************/

	/* 只有位于root组的主持人才可以发起投票,子组会议中不能发起
	/* 发起投票 
	topic:		投票主题
	type:		投票类型，记名或不记名（"public"或"anonymous"）
	ulTime:		投票时间，超时不投票算弃权
	bMulti:		投票时是否可以选择多个选项
	strItems:	投票选项*/
	virtual void StartVote(const std::string& strTopic,const bool& bType,const unsigned long& ulTime,const bool& bMulti,MMS_VOTE_LST& strItems)=0;

	/* 结束投票 
	endtype:	投票结束方式，手动或自动（"hand" 或 "auto"）*/
	virtual void StopVote(const std::string& strVoteID,const std::string& strEndType)=0;

	/* 投票
	result:		同意、反对或弃权（"agree"、"disagree"或"abstain"）
	strMemberID:	投票者的ID，对于不记名投票则为空("")*/
	virtual void Vote(const std::string& strMemberID,const std::string& strVoteID,MMS_VOTE_LST& strItems)=0;

	/****************************************************************
	 * 会议举手函数
	 ****************************************************************/

	/* 开始举手:发起人调用 */
	virtual void StartHandle(const unsigned long& ulTime)=0;

	/* 举手:举手人调用 */
	virtual void Handle(const unsigned long& ulID,const bool& bAgree)=0;

	/* 停止举手:发起人调用 */
	virtual void StopHandle(const unsigned long& ulID)=0;

	/****************************************************************
	 * 会议点名函数
	 ****************************************************************/
	virtual void Calling(const unsigned int& usTime)=0;
	virtual void CloseCalling()=0;
	virtual void CallingInConference(const std::string& strFile)=0;

	/****************************************************************
	 * 会议监控函数
	 ****************************************************************/

	//发送监控数据
	virtual void MonUserData(KCmdUserDataPacket& packet)=0;

	//监控P2P
	virtual void MonP2P(const std::string& strChannelID, KCmdUserDataPacket& packet)=0;

	/****************************************************************
	 * 电视墙函数
	 ****************************************************************/
	virtual void InsertTVMonitor(unsigned short usIndex,const std::string& strData)=0;
	virtual void UpdateTVMonitor(unsigned short usIndex,const std::string& strData)=0;
	virtual void DeleteTVMonitor(unsigned short usIndex)=0;
	virtual void InsertTVMatrix(unsigned short usIndex,const std::string& strData)=0;
	virtual void UpdateTVMatrix(unsigned short usIndex,const std::string& strData)=0;
	virtual void DeleteTVMatrix(unsigned short usIndex)=0;
	virtual void GetTVList(void)=0;

	/****************************************************************
	 * SIP 呼入与挂断函数
	 ****************************************************************/
	virtual void SIPCallIn(void)=0;
	virtual void SIPHangUp(void)=0;

	/****************************************************************
	 * 录播函数
	 ****************************************************************/
	virtual void GetStorageServer()=0;
	virtual void GetUploadPlan()=0;
	virtual void GetRecordFormat()=0;
	virtual void SetRecordFormat(const MMS_RECFORMAT& recFormat)=0;

	virtual void StartRecord()=0;
	virtual void PauseRecord()=0;
	virtual void ContinueRecord()=0;
	virtual void StopRecord(const std::string & strRoomID)=0;

	// 增加直播的接口
	virtual void StartLive( const std::string& liveIP, unsigned short livePort, const std::string& liveChannelName)=0;
	virtual void StopLive() = 0;

	/****************************************************************
	 * TV会议
	 ****************************************************************/

	/* 发送用户自定义数据(仅发送到会议当中的成员)
	* ucObject对象: 
	*   0=向会议所有主持人(不包括自己)发送数据
	*   1=向会议所有成员发送数据
	*   除0和1外的所有数据=向packet中的toid发送数据 */
	virtual void SendUserDataEx(const std::string& strConferenceID, KCmdUserDataPacket& packet,unsigned char ucObject=0)=0;
	virtual void RegisterConference(const std::string& strConferenceID,const std::string& strUpdateTime,unsigned short usTermType,const std::string& strTermName)=0;
	virtual void UnregisterConference(const std::string& strConferenceID)=0;

    /************************************************************************
    * 会议室相关
    /************************************************************************/
    // 获取会议室类型列表
    virtual void GetRoomTypeList(void) = 0;

    // 获取本地会议室类型列表
    virtual MMS_ROOMTYPE_LST& GetLocalRoomTypeList(void) = 0;
    
    // 获取物理会议列表
    virtual void GetPhysicalRoomList() = 0;

    // 获取本地物理会议列表
    virtual MMS_PHY_ROOM_LST& GetLocalPhysicalRoomList() = 0;

    // 获取会话设置
    virtual void GetTalkSet() = 0;

	/************************************************************************
	* 自由会议
	/************************************************************************/
	// 获取空闲自由会议号
	virtual void GetFreeRoomID() = 0;

    /************************************************************************
    * 触摸屏授权
    /************************************************************************/
    // 获取、释放电子触摸屏授权
    virtual void StartTouchScreen() = 0;
    virtual void StopTouchScreen() = 0;

    /************************************************************************
    * 会议组管理
    /************************************************************************/
    // 用户当前所在会议是已知的，不必作为参数
	virtual void AddGroup( const std::string& strGroupName, const std::string& strParentGroupID, const std::string& strTopic /*levelid, orderid, topic*/)=0;
	virtual void DelGroup( const std::string& strGroupID)=0;
    // virtual void AddGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // virtual void DelGroupMember( unsigned long ulGroupID, const std::string& strMemberID)=0;
    // 改变用户所属组，不必
	virtual void ChangeMemberGroup( const std::string& strGroupID, const std::string& strMemberID)=0;

	//获取会议室组成员信息
	virtual void GetRoomGroupMemberInfo(const std::string& roomid, const std::string& groupid)=0;

    /************************************************************************
    * 个人分组会议
    /************************************************************************/
	virtual void GetPersonalRoom(void)=0;
	virtual void GetPersonalRoomGroup(const std::string& strRoomID)=0;
	virtual void GetPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void AddPersonalRoom(const std::string& strRoomName)=0;
	virtual void DelPersonalRoom(const std::string& strRoomID)=0;
	virtual void AddPersonalRoomGroup(const MMS_GROUP& group)=0;
	virtual void DelPersonalRoomGroup(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void StartPersonalRoom(const std::string& strRoomID)=0;

    virtual void GetRoomMemberList()=0;

	/* 获取会议人员权限类型
	strRoomID:		会议ID
	strMemberID:	人员ID*/
	virtual void GetRoomMemberType(const std::string& strRoomID,const std::string& strMemberID)=0;

	/* 添加个人会议组 */
	virtual void SendPersonalAddGroupEnd(const std::string& strRoomID)=0;

	//如果不重新设置会议类型，默认保存的是无层次结构
	virtual void SetPersonalRoomType(const std::string& strRoomID,bool bTreeMode=false)=0;
	virtual void RenamePersonalRoom(const std::string& strRoomID,const std::string& strRoomName)=0;
	virtual void AddPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void SetPersonalRoomMemberType(const std::string& strRoomID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void DelPersonalRoomMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	/************************************************************************/
	/* 联系人分组                                                           */
	/************************************************************************/
	virtual void GetContactGroup(void)=0;
	virtual void GetContactGroupMember(const std::string& strGroupID)=0;

	/* 获取未进会会议监控终端列表
	 * nBegin 从哪条开始取
	 * nEnd   取到哪条结束
	 * bOnline : 为true时，取在线的，为false时，取所有的
	 * strDevName 设备名称 */
	virtual void GetNoInRoomDevList(int nBegin/*=1*/,int nEnd/*=50*/,bool bOnline = false, const std::string& strDevName = "")=0;

    virtual void GetLoginInfo()=0;
    
    //设置麦序配置
	virtual void SetMicSeqCfg(std::string& strRoomID,unsigned short usType,unsigned int nDuration,unsigned int nMaxNum,unsigned int nSpUserNum,std::string &strTag)=0;
	
	//抢麦
	virtual void GrapMicSeq(std::string& strRoomID,const std::string& strMemberID)=0;
	
	//结束通话
	virtual void ExitMicSeq(std::string& strRoomID,const std::string& strMemberID)=0;
	
	//会议主持人更改麦序
	virtual void ModifyMicSeq(std::string& strRoomID,MMS_SEQ_MEMBER_LST & lstMember)=0; 	
	//会议主持人为某成员增加额外时间
    virtual void SetExTime(std::string strMemberID,std::string& strRoomID,unsigned long nExTime)=0;
	
	//清空麦序列表用于重新抢麦
	virtual void ClearMicSeq(std::string& strRoomID)=0; 
    
	/*开启 MPS*/
	virtual void StartMP(void)=0;
	virtual void Start3GC(void)=0;
	/*结束 MPS*/
	virtual void StopMPS(const std::string& strMPSUserID)=0;
	/*会议DTS初始数据接受完毕*/

	/************************************************************************/
	/* 请求转码 
	strMemberID_unCardIndex:将被转码的通道
	/************************************************************************/
	virtual void RequestTranscode(const std::string strMemberID,unsigned short unCardIndex)=0;
	virtual void StopTranscode(const std::string strMemberID,unsigned short unCardIndex)=0;

	//virtual void SetDTSDataRecive(const bool& bDTSDataRecive = true)=0;
	//virtual void SendDelayCmd()=0;
	virtual std::string GetGroupID()=0;

    //移动监控
    //获取移动监控频道列表
    virtual void GetMobileChannelList(const std::string& strDomain)=0;
    //申请频道控制权
    virtual void ApplyMobileChannelControl()=0;
    //处理收到的对频道控制权的申请
    virtual void HandleApplyMobileChannelControl(std::string& strApplyID,bool bIsSuccess)=0;
    //释放频道控制权
    virtual void ReleaseMobileChannelControl()=0;

	//获取串口服务器ip
	virtual void GetSerialServerIP(const std::string& strDevID)=0;

	virtual void GetMultiplyRoom(const std::string& strDomain)=0;
	
	/****************************************************************
	 * 福建建行
	 ****************************************************************/
	//评价客服StrServiceID:客服ID,usSatisfaction:满意度（0:不予评价，1:不满意，2:基本满意，3:满意，4:非常满意），usServiceTime:服务时长（单位秒）
	virtual	void JudgeService(const std::string& strServiceID,const unsigned short& usSatisfaction=0,const unsigned long& ulServiceTime=0) = 0;
	virtual void UploadServiceSumary(int business_type, int business_first_level, int business_second_level, std::string strDepart = "", unsigned long ulTalkTime = 0)=0;
	//招商证券sip呼叫入会
	/************************************************************************/
	/* 参数说明     
	strCallID		终端呼叫申请入会呼叫id
	strUserID		终端呼叫申请入会的人员id
	strUserName		终端呼叫申请入会的人员名称
	strMcuID		mpserver id
	strRoomID		终端申请入会的会议id
	bAccept			是否同意该sip终端入会 =true同意；=false不同意
	*/
	/************************************************************************/
	virtual void ResponseForSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID, bool bAccept)=0;

	//发布数字功能邀请进会码，此码由服务生成后推送给发布者
	virtual void SetDigitalInvitationCode(const std::string& strRoomID) = 0;

	//设置会议最小码流
	virtual void SetRoomMinCodestream(const std::string& strRoomID,double dRoomMinCodestream) = 0;

	//设置用户会议信息
	virtual void SetRoomMemberInfo(const MEMBER_DEFINED_INFO& memberinfo) = 0;

	//获取本会议用户信息
	/* 参数说明     
	strMemberID		=""时：获取本会议所有在线成员列表信息;!=""时获取此成员信息
	*/
	virtual void GetRoomMemberInfo(const std::string& strMemberID ="") = 0;
	
	/***************************************************
	//函数功能：开始轮循
	//参数：
	//		strRoundRobinGroupID	轮循分组id
	//		nTMWndCnt				窗口数
	//		nPrerecvCnt				最大预接路数
	//		strReceiverlist			接收者userid列表(以“;”隔开，如：cs0001;cs0002，有域带域)
	//		bRecvAudio				是否接音频
	//返回值：无
	***************************************************/
	virtual void StartRoundRobin(const std::string& strRoundRobinGroupID, const int& nTMWndCnt, const int& nPrerecvCnt, const std::string& strReceiverlist, const bool bRecvAudio=false) = 0;

	/***************************************************
	//函数功能：停止轮循
	//参数：
	//		strRoundRobinGroupID		轮循分组id
	//返回值：无
	***************************************************/
	virtual void StopRoundRobin(const std::string& strRoundRobinGroupID) = 0;

	/***************************************************
	//函数功能：设置当前预接信息
	//参数：
	//		strPreRecvData			当前预接信息
	//返回值：无
	***************************************************/
	virtual void SetRRCurPreRecvAV(const std::string& strPreRecvData) = 0;

	/***************************************************
	//函数功能：设置当前显示信息
	//参数：
	//		strViewShowData			当前显示信息
	//返回值：无
	***************************************************/
	virtual void SetRRCurViewShowAV(const std::string& strViewShowData) = 0;

	/***************************************************
	//函数功能：设置全屏
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uChannelID		通道ID
	//		bFull				true:设置全屏，false：取消全屏
	//返回值：无
	***************************************************/
	virtual void SetFullScreen(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, bool bFull) = 0;

	/***************************************************
	//函数功能：模板预置
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		uNumber				模板号
	//		uBandwidth			模板带宽
	//		uWidth				模板分辨率宽
	//		uHeight				模板分辨率高
	//		strPara				保留参数
	//返回值：无
	***************************************************/
	virtual void TemplatePreset(const std::string& strRoomID, const std::string& strGroupID, const unsigned int uNumber, const unsigned int uBandwidth, const unsigned int uWidth, const unsigned int uHeight, const std::string& strPara) = 0;

	/***************************************************
	//函数功能：窗口预设
	//参数：
	//		WndPreSet			请参考结构体定义
	//返回值：无
	***************************************************/
	virtual void WindowPreset(const MMS_WNDPRESET& WndPreSet) = 0;

	/***************************************************
	//函数功能：改变通道的音视频信息
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uChannelID			通道ID
	//		uStream				码流
	//		uWidth				模板分辨率宽
	//		uHeight				模板分辨率高
	//返回值：无
	***************************************************/
	virtual void ChangeAudioVedioInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const unsigned int uStream, const unsigned int uWidth, const unsigned int uHeight) = 0;

	/***************************************************
	//函数功能：采集锁定
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uChannelID			通道ID
	//		bLock				是否采集锁定（true锁定，反之）
	//返回值：无
	***************************************************/
	virtual void CollectionLock(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const bool bLock) = 0;

	/***************************************************
	//函数功能：上报通道设备信息
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMerberID			人员ID
	//		uChannelID			通道ID
	//		strDevInfo			设备详细信息
	//注意：
	//		关于strDevInfo组包样式，lcodec表示编码协议，chldef表示清晰度，chlrate表示帧率
	//		h264、h265；HD表示高清，SUD超清，SD表示标清
	//		"chlcodec:h264;chldef:HD;chlrate:30;" 表示h264编码的 高清设备 30帧
	//		字符串可以有一个或者多个的组合，也可以自定义
	//返回值：无
	***************************************************/
	virtual void ReportChlDevInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMerberID, const unsigned int uChannelID, const std::string& strDevInfo) = 0;
};

//=======================================================================================
// MMS模块事件类
//=======================================================================================
class AFX_EXT_CLASS KMMSSessionEvent
{
public:
	KMMSSessionEvent(void){};
	virtual ~KMMSSessionEvent(void){};

public:
	/****************************************************************
	 * 临时会议事件
	 ****************************************************************/
	/* 激活临时会议回调，nErrorCode=0：首次激活成功 1：激活失败（未授权） 2：已被激活过 */
	virtual void OnActiveTemporaryRoom(int nErrorCode)=0;
	virtual void OnTempGroupItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strGroupName,bool bDefault)=0;
	virtual void OnTempGroupUpdateName(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strGroupName)=0;
	virtual void OnTempGroupDelete(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnTempMemberItem(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,const std::string& strMemberName)=0;
	virtual void OnTempMemberDelete(const std::string& strDomain,const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID)=0;

	/****************************************************************
	 * IPCAM列表事件
	 ****************************************************************/
	virtual void OnIPCamItem(unsigned long ulIPCamID,const std::string& strIPCamName,const std::string& strOrgID,const std::string& strModel,const std::string& strIPAddr,const std::string& strAdminID,const std::string& strAdminPass,const std::string& strOperID,const std::string& strOperPass,unsigned char ucStatus)=0;
	virtual void OnIPCamListEnd(const std::string& strUpdateTime)=0;

	/****************************************************************
	 * 会议列表事件
	 ****************************************************************/
	virtual void OnRoomListEnd(const std::string& strDomain)=0;

	virtual void OnRoomItem( const PMMS_ROOM& pItem)=0;

	/****************************************************************
	 * 会议事件
	 ****************************************************************/

	/* 收到登录会议室事件
	 * errcode返回登录结果值
	 * 0: 成功登录会议室
	 * 1: 密码验证失败
	 * 2: 用户已经进入会议
	 * 3: 会议已经超过六人限制
	 * 4: 会议没有适当的MCU服务器
	 * 5: 会议所有者离开会议
	 * 6: 登录者类型不明确
	 * 7: 会议已经锁定
	 * 8: 会议类型不明确
	 *99: 服务内部发生错误 */
	virtual void OnLogin(unsigned int nErrCode,const std::string& strDomainID,const std::string& strRoomID,const std::string& strRoomName,unsigned short usRoomType,
		const std::string& strRootGroupID,const std::string& strGroupID,const std::string& strActiveGroupID, const std::string& strTempGroupName,
        const std::string& strMemberID,unsigned short usMemberType,bool bAllowVideo,double dMaxRoomCodestream,double dMinRoomCodestream,const std::string& strPassword)=0;

	/* 收到退出会议室事件
	 * nErrCode返回登录结果值
	 * 0: 表示正常退出会议室
	 * 1: 表示所有者退出会议室
	 * 2: 表示被踢出会议室
	 * 3: 会议成员帐号在该服务器上不存在或该服务器没有加载会议模块 
	 * 4: 主持人宣布会议结束 
	 * 5: 父域断开连接，退出上级会议*/
	virtual void OnLogout(const std::string& strRoomID,unsigned int nErrCode)=0;

	/* 收到会议组项 */
	virtual void OnRoomGroupItem(MMS_GROUP_EX& item)=0;

	/* 收到会议组项结束 */
	virtual void OnRoomGroupListEnd(const std::string& strUpdateTime="")=0;

	/* 收到会议成员项 */
	virtual void OnRoomMemberItem(MMS_MEMBER_EX& item)=0;

	/* 收到会议成员项结束 */
	virtual void OnRoomMemberListEnd(void)=0;

	/* 收到预定窗口成员项 */
	virtual void OnWindowMemberItem(const std::string& strGroupID,unsigned int nScreenID,unsigned int nWindowID,const std::string& strMemberID)=0;

	/* 收到预定窗口成员项结束 */
	virtual void OnWindowMemberListEnd(void)=0;

	/* 收到会议数据已准备,收到该消息表示会议登录成功后收到所有实时数据 */
	virtual void OnReady(void)=0;

	/* 收到成员状态 */
	virtual void OnStatus(const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,
		                  const std::string& strSvrID,const std::string& strSvrAddr,unsigned short usSvrPort,const std::string& strSvrType,
						  const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
						  unsigned short usTermType,const std::string& strTermName,unsigned char ucStatus,const std::string& strstaoff)=0;

	/* 收到会议邀请 */
	virtual void OnInvite(const std::string& strRoomDomain,const std::string& strRoomID,const std::string& strGroupID,
		const std::string& strRoomName,const std::string& strTempRoomNickname,const std::string& strInviterNodeID,const std::string& strInviterID,
		const std::string& strInviterName,const std::string& strPassword,bool bPresider, const std::string& strTempGroupID, 
		const std::string& strTempGroupName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;

	/* 收到拒绝邀请 */
	virtual void OnReject(const std::string& strDomain,const std::string& strRoomID,const std::string& strInviteeID,const std::string& strInviteeName)=0;

	/* 收到会议消息 */
	virtual void OnMessage(const std::string& strMemberID,const std::string& strHead,const std::string& strBody,const std::string& strFormat)=0;

	/* 收到用户定义数据
     * ucObject对象: 
	 *   0=收到向主持人发送的数据
	 *   1=收到广播数据
	 *   除0和1外=收到点到点数据 */
	virtual void OnUserData(KCmdUserDataPacket& packet,unsigned char ucObject)=0;

	/* 设置激活组 */
	virtual void OnSetActiveGroupID(const std::string& strGroupID)=0;

	/* 设置当前屏幕号 */
	virtual void OnSetCurrentScreenID(const std::string& strOwnerID,const std::string& strGroupID,int nCurScrID,int nPrevTM)=0;

	/* 删除通道 */
	virtual void OnCardItemOffline(const std::string& strMemberID,int nCardIndex,const std::string& strCardType)=0;

	/* 收到通道项 */
	virtual void OnCardItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardType,const std::string& strCardName,
		                    unsigned int nChannelType,unsigned long ulChannnelID,
							const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
							const std::string& strDevInfo, unsigned long ulCodeType)=0;

	/* 通道项列表完成 */
	virtual void OnCardListEnd(void)=0;

	/* 收到音频通道项 */
	virtual void OnCardAudioItem(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,unsigned long ulAudioID)=0;

	/* 设置通道名称 */
	virtual void OnSetCardName(const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,const std::string& strCardName)=0;

	/* 组同步信息获取完成 */
	virtual void OnGroupSynInfoEnd(void)=0;
	
	/*设置会议参数*/
	virtual void OnSetMeetingParameter(const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword, const std::string& strParameter, const std::string& strAdminID)=0; 
	
	/*获取会议参数  错误码为0表示获取到相应会议参数 */
	virtual void OnGetMeetingParameter(int ErrCode,const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword = "", const std::string& strParameter = "")=0; 

	/*删除会议参数*/
	virtual void OnDelMeetingParameter(const std::string& strRoomID, const std::string& strGroupID,const std::string& strKeyword)=0;

	/* 设置个人或会议权限 */
	virtual void OnSetRight(int nRight, bool bEnable, const std::string& strMemberID)=0;
	/* 获取个人或会议权限 */
	virtual void OnGetRight(const std::string& strMemberID,const std::string& strMemRight)=0;

	/* 申请或删除发言人 */
	virtual void OnSetProlocutor(const std::string& strMemberID,bool bRequest)=0;

	/* 广播或取消广播发言人 */
	virtual void OnEnableProlocutor(const std::string& strMemberID,bool bEnable)=0;

	/* 申请或删除提问人 */
	virtual void OnSetQuestioner(const std::string& strMemberID,bool bRequest)=0;

	/* 单接或取消单接提问人 */
	virtual void OnEnableQuestioner(const std::string& strMemberID,bool bEnable)=0;

	/* 设置公告内容 */
	virtual void OnAnnexData(int nType,const std::string& strData,const std::string& strFormat)=0;

	/* 设置公告内容 */
	virtual void OnEnableAnnex(int nType,bool bEnable)=0;

	/* 会议解锁 */
	virtual void OnUnlocked(void)=0;

	/* 广播卡 */
	virtual void OnBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,int nScreenID,int nWindowID,
		                         const std::string& strMemberID,int nCardIndex,const std::string strCardName,int nChannelType,unsigned long ulChannelID,
								 const std::string& strMCUID,const std::string& strMCUAddr,unsigned short usMCUPort,
                                 const std::string& strNodeID,const std::string& strNatAddr,const std::string& strLocalAddr,unsigned short usLocalPort,
								 const std::string& strTag,const MMS_MEMBER_TYPE& memberType, const std::string& strMemberName, unsigned short usCodeType)=0;

	/* 关闭广播卡 */
	virtual void OnCloseBroadcastCard(const std::string& strOwnerID,const std::string& strGroupID,const std::string& strMemberID,int nCardIndex,int nChannelType)=0;

	/* 拖动窗口 */
	virtual void OnDragWindow(const std::string& strOwnerID,int nFromScreenID,int nFromWindowID,int nToScreenID,int nToWindowID)=0;

	/* 改变模板 */
	virtual void OnSetTemplet(const std::string& strOwnerID,int nScreenID,int nTemType,int nTemID,bool bTemShow,int nWinNumber,unsigned short unErr)=0;

	/* 自动模版 */
	virtual void OnSetAutoTemplet(const std::string& strFromID,unsigned int nErr,const std::string& strRoomID,const std::string& strGroupID,bool bAutoTem)=0;

	/* 设置会议屏属性
	 * strAllAttribute所有屏的属性串
	 * strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
	virtual void OnSetScreenAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* 设置会议窗口属性
	* strAllAttribute所有窗口的属性串
	* strCurAttribute当前设置的属性串，为空，则应用strAllAttribute的值*/
	virtual void OnSetWindowsAttribute(const std::string& strAllAttribute,const std::string& strCurAttribute="")=0;

	/* 浮动窗口 */
	virtual void OnSetFloatWindow(const std::string& strOwnerID,int nScreenID,int nX,int nY,int nCX,int nCY)=0;	

	/* 邀请到会议室并广播 */
	virtual void OnInviteInRoomBroadcast(const std::string& strRoomDomain,const std::string& strRoomID,const std::string& strGroupID,
		                                 const std::string& strMemberID,const std::string& strMeetingName,const std::string& strMeetingPass,
										 const std::string& strMemberName,int nInviteAdmOrderID,int nBeInviteAdmOrderID)=0;

	/* 设置临时主持人
	 * bSet: true=设置;false=取消 */
	virtual void OnSetTempPresider(const std::string& strMemberID,bool bSet)=0;

	/* 投票 */
	virtual void OnStartVote(const std::string& strVoteID,const std::string& strInitiator,const std::string& strTopic,const bool& bType,const unsigned long& ulTime,const bool& bMulti,MMS_VOTE_LST& strItems)=0;
	virtual void OnStartVoteErr(const unsigned int& nErrCode)=0;
	virtual void OnStopVote(const std::string& strVoteID,const std::string& strEndType,const int& nSum,const int& nAnswer,MMS_VOTE_LST& strItems)=0;

	/* 举手 */
	virtual void OnStartHandle(const unsigned long& ulID,const unsigned long& ulTime)=0;
	virtual void OnStartHandleErr(const unsigned int& nErrCode)=0;
	virtual void OnStopHandle(const unsigned long& ulID,const unsigned short& usTotal,const unsigned short& usCount)=0;

	/* 子组与同步 */
	virtual void OnAllowSubMeet(const std::string& strGroupID)=0;
	virtual void OnSync(const std::string& strGroupID)=0;

	/* 点名 */
	virtual void OnCalling(const std::string& strMemberID,const unsigned int& usTime)=0;
	virtual void OnCallingErr(const unsigned int& nErrCode)=0;
	virtual void OnCloseCalling(const std::string& strMemberID)=0;

	/* 返回监控数据 */
	virtual void OnMonUserData(KCmdUserDataPacket& packet)=0;

	/* 返回监控P2P */
	virtual void OnMonP2PData(const std::string& strDevID,KCmdUserDataPacket& packet)=0;
    
	/* 电视墙 */
	virtual void OnInsertTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMonitor(unsigned short usIndex,unsigned short usErrorCode)=0;

	/* 电视墙控制器 */
	virtual void OnInsertTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnUpdateTVMatrix(unsigned short usIndex,unsigned short usErrorCode)=0;
	virtual void OnTVServerItem(const std::string& strTV,const std::string& strTVName,const std::string& strStatus, 
		const std::string& strMemNodeid, unsigned long ulTVWallTemplate)=0;
	virtual void OnTVMonitorItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVMatrixItem(unsigned short usIndex,const std::string& strData)=0;
	virtual void OnTVListEnd(void)=0;
	
	/*获取指定会议室的会议终端在线数和总数*/
	virtual void OnGetRoomMemberCount(const std::string& strRoomID,int nOnline,int nTotal)=0;
	/****************************************************************
	* 录播函数
	****************************************************************/
	virtual void OnGetStorageServer(const std::string& strServer)=0;
	virtual void OnGetUploadPlan(bool bAutoUpload, MMS_RECPLAN_LST& listRecPlan)=0;
	virtual void OnGetRecordFormat(MMS_RECFORMAT& recFormat)=0;

	virtual void OnStartRecord(const std::string& strUserID,const std::string& strServerID="")=0; // strServerID空为失败只给申请者回调，否则所有主持人
	virtual void OnPauseRecord()=0;
	virtual void OnContinueRecord()=0;
	virtual void OnStopRecord()=0;

	/****************************************************************
	 * TV会议
	 ****************************************************************/
	/*usRet：0为成功；1为失败*/
	virtual void OnRegisterConference(const std::string& strConferenceID,const std::string& strConferenceName,const std::string& strConferenceType,unsigned short usRet)=0;

	/****************************************************************
	 * 移动手机终端
	 ****************************************************************/

	/* 手机通知用户切换到GSM编码 */
	virtual void OnOpenAudioChannelForMobile(const std::string& strMemberID,unsigned int nCardIndex)=0;

	/* 手机通知用户恢复到原有编码 */
	virtual void OnCloseAudioChannelForMobile(const std::string& strMemberID,unsigned int nCardIndex)=0;

    /************************************************************************
    * 会议室相关
    /************************************************************************/
    // 获取会议类型列表结束
    virtual void OnRoomTypeListEnd(const std::string& strDomain) = 0;

    // 获取物理会议类型列表结束
    virtual void OnPhysicalRoomListEnd(const std::string& strDomain) = 0;

    // 收到讨论设置
    virtual void OnGetTalkSet( const MMS_TALK_SET& tTalkSet ) = 0;

	/************************************************************************
	* 自由会议
	/************************************************************************/
	// 获取自由会议号返回
	virtual void OnFreeRoomID( const std::string& strFreeRoomID ) = 0;

    /************************************************************************
    * 触摸屏授权
    /************************************************************************/
    // 触摸屏授权返回
    virtual void OnStartTouchScreen( int nErrCode ) = 0;

    /************************************************************************
    * 会议组管理
    /************************************************************************/
	virtual void OnAddGroup( const std::string& strGroupID, const std::string& strGroupName, const std::string& strParentGroupID, const std::string& strTopic, unsigned int nErrCode) = 0;
	virtual void OnDelGroup( const std::string& strGroupID, unsigned int nErrCode) = 0;
	virtual void OnChangeGroupMember( const std::string& strGroupID, const std::string& strMemberID, unsigned int nErrCode) = 0;

	/************************************************************************
    * 个人分组会议
    /************************************************************************/
	virtual void OnGetPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,bool bHaveFrame,unsigned short usStatus)=0;
	virtual void OnGetPersonalRoomEnd(void)=0;

	virtual void OnGetPersonalRoomGroup(const std::string& strRoomID,const MMS_GROUP& group)=0;
	virtual void OnGetPersonalRoomGroupEnd(const std::string& strRoomID)=0;

	virtual void OnGetPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void OnGetPersonalRoomMemberEnd(const std::string& strRoomID,const std::string& strGroupID)=0;
	virtual void OnAddPersonalRoom(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMeetingName,const std::string& strOwnerID)=0;
	virtual void OnDelPersonalRoom(const std::string& strRoomID)=0;
	virtual void OnRenamePersonalRoom(const std::string& strRoomID,const std::string& strMeetingName)=0;
	virtual void OnAddPersonalRoomMember(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMemberID,unsigned short usMemberType,bool bSuccessFlag)=0;
	virtual void OnSetPersonalRoomMemberType(const std::string& strRoomID,const std::string& strMemberID,unsigned short usMemberType)=0;
	virtual void OnDelPersonalRoomMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	virtual void OnAddPersonalRoomGroup(const MMS_GROUP& group)=0;
	virtual void OnAddPersonalRoomGroupEnd(const std::string& strRoomID)=0;
	
	/*会议中人员都退出*/
	virtual void OnStopPersonalRoom(const std::string& strRoomID)=0;

	/*获取指定会议室的一个在线成员*/
	virtual void OnGetAppointRoomOnlineMember(const std::string& strRoomID,const std::string& strMemberID)=0;

	/*获取指定会议室的一个在线合成终端或主持人通道信息*/
	virtual void OnGetAppointRoomOnlineMemberInfo(const std::string& strDomain,const std::string& strRoomID,std::string& strMemberID,unsigned int unCardIndex,unsigned long ulAudioID,unsigned long ulVideoID,
		std::string& strMCUID,std::string& strMCUAddr,unsigned short usMCUPort,
		std::string& strNodeID,std::string& strNatAddr,std::string& strLocalAddr,unsigned short usLocalPort,const std::string& strTag="")=0;

	/*通知个人会议发起人会议已经开始(第一个成员已经登录会议)*/
	virtual void OnNotifyPersonalAdminMeetingStart(const std::string& strRoomID)=0;

	/************************************************************************/
	/* 联系人分组                                                           */
	/************************************************************************/
	virtual void OnGetContactGroup(const std::string& strParentGroupID,const std::string& strGroupID,const std::string& strGroupName,const unsigned int nLevelID, const unsigned int nOrderID)=0;
	virtual void OnGetContactGroupEnd()=0;

	virtual void OnGetContactGroupMember(const std::string& strGroupID,const IMS_USER_ITEM_BASE& item)=0;
	virtual void OnGetContactGroupMemberEnd(const std::string& strGroupID)=0;

	virtual void OnContactGroupMemberStatus(const std::string& strMemberID,const std::string& strStatus)=0;

    //移动监控
	//获取到移动频道列表
    virtual void OnGetMobileChannelList(const std::string& strDomain,const MMS_ROOM_LST& lstChnl)=0;
    //处理其他用户对控制权的申请
    virtual void OnApplyMobileChannelControl(const std::string& strUserID,const std::string& strRoomID)=0;
    //收到申请控制权的处理结果
    virtual void OnHandleApplyMobileChannelControl(const std::string& strAdminID,const std::string& strRoomID,bool bIsSuccess)=0;
    //释放控制权
    virtual void OnReleaseMobileChannelControl(const std::string& strRoomID)=0;

	/****************************************************************
	 * 沪江用
	 ****************************************************************/
	virtual void OnGetRoomRemark(const std::string& strRoomID,const std::string& strRemark)=0;

   	//广播麦序
	virtual void OnBroadCastMicSeq(MMS_SEQ_MEMBER_LST& lstMember,unsigned long ulServerTime)=0;
	
	//广播房间配置
	virtual void OnBroadCastMicSeqCfg(unsigned short usType,unsigned int nDuration,unsigned int nMaxNum,unsigned int nSpUserNum,std::string &strTag)=0;
	
    //增加额外时间
    virtual void OnSetExTime(std::string& strMemberID,unsigned int nTime)=0;
    
    //清空列表
    virtual void OnClearMicSeq()=0;

    //列表已满
    virtual void OnMicSeqFull()=0;

	/* 获取未进会监控终端列表回调 */
	virtual void OnGetNoInRoomDevList(const NOINROOMDEV& dev)=0;

	/* 获取未进会会议监控终端列表回调结束 
	 * nCount 本次取到的条数*/
	virtual void OnGetNoInRoomDevListEnd(int nCount)=0;

	//获取串口服务器IP
	virtual void OnGetSerialServerIP(const std::string& strDevID,const std::string& strSerialServerIP)=0;

    virtual void OnGetMultiplyRoom(const std::string& strRoomID,const std::string& strPasswd,const unsigned int nMultiplyLimit,const unsigned int nErrCode)=0;

	//获取会议人员权限类型
	virtual void OnGetRoomMemberType(const std::string& strRoomID,const std::string& strMerberID,const unsigned short& usMemberType)=0;
	//招商证券投票详情和sip呼叫申请入会
	/************************************************************************/
	/* 参数说明：
	ulVoteID		此次投票id
	strTopic		此次投票主题
	strUserID		此选票的投票人id
	strUserName	此选票的投票人名称
	itemid		此选票的选项id
	itemnam		此选票的选项名称                                            */
	/************************************************************************/
	virtual void OnGetVoteDetail(unsigned long ulVoteID, const std::string& strTopic, const std::string& strUserID,const std::string& strUserName, unsigned int itemid, const std::string& itemname)=0;
	virtual void OnGetVoteDetailEnd(unsigned long ulVoteID)=0;
	/************************************************************************/
	/* 参数说明：
	strCallID		终端呼叫申请入会呼叫id
	strUserID		终端呼叫申请入会的人员id
	strUserName		终端呼叫申请入会的人员名称
	strMcuID		mpserver id
	strRoomID		终端申请入会的会议id
	                                            */
	/************************************************************************/
	virtual void OnRequestSipCallIn(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID)=0;
    virtual void OnSipQuitRequest(const std::string& strCallID, const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID)=0;

	/************************************************************************/
	/* 参数说明：
	strCallID		终端呼叫申请入会呼叫id
	strUserID		终端呼叫申请入会的人员id
	strUserName		终端呼叫申请入会的人员名称
	strMcuID		mpserver id
	strRoomID		终端申请入会的会议id
	bAccept			其他主持人是否同意sip终端入会=true同意，=false不同意
	                                            */
	/************************************************************************/
	virtual void OnResponseForOtherPresider(const std::string& strCallID,const std::string& strUserID, const std::string& strUserName, const std::string& strMcuID, const std::string& strRoomID, bool bAccept)=0;
	//收到会议人员变化(供会议列表使用)
	virtual void OnRoomMemberStatusChange(const std::string& strRoomID,const std::string& strGroupID,const std::string& strMerberID,const unsigned short& usStatus,const unsigned short& usMemberType,const unsigned int& nOnlineNum, unsigned int& nTotalNum)=0;
	//收到数字功能会议邀请码
	virtual void OnRoomDigitalCode(const std::string& strRoomID,const unsigned int& unDigitalCode)=0;

	virtual void OnSetRoomMinCodestream(unsigned short usErr, const std::string& strRoomID,double dRoomMinCodstream)=0;
	//web通信
	virtual void OnRoomMaxCodestream(const std::string& strRoomID,double dRoomCodstream)=0;
	//用户会议个人信息
	virtual void OnRoomMemberInfo(const MEMBER_DEFINED_INFO& memberdefinfo)=0;
	virtual void OnRoomMemberLstInfo(const MEMBER_DEFINED_INFO_LST& lstmemberdefinfo)=0;
	//通知合成白板/文档共享所在窗口的像
	virtual void OnRoomDigitalComp(const _MMS_CHLTYPE& channelType)=0;

	/***************************************************
	//函数功能：开始轮循回调
	//参数：
	//		strRoundRobinGroupID	轮循分组id
	//		nTMWndCnt				窗口数
	//		nPrerecvCnt				最大预接路数
	//		strReceiverlist			接收者userid列表(以“;”隔开，如：cs0001;cs0002)
	//		bRecvAudio				是否接音频
	//		uErr					错误码（值:ERR_MMS_RRGOUPRUNNING、ERR_MMS_STARTRRFAILED）
	//返回值：无
	***************************************************/
	virtual void OnStartRoundRobin (const std::string& strRoundRobinGroupID, const int& nTMWndCnt, const int& nPrerecvCnt, const std::string& strReceiverlist, const bool bRecvAudio, const unsigned int& uErr) = 0;

	/***************************************************
	//函数功能：停止轮循回调
	//参数：
	//		strRoundRobinGroupID	轮循分组id
	///		uErr					错误码（值：ERR_MMS_STOPRRFAILED）
	//返回值：无
	***************************************************/
	virtual void OnStopRoundRobin(const std::string& strRoundRobinGroupID, const unsigned int& uErr) = 0;

	/***************************************************
	//函数功能：设置当前预接信息回调
	//参数：
	//		strPreRecvData			当前预接信息
	//返回值：无
	***************************************************/
	virtual void OnSetRRCurPreRecvAV(const std::string& strPreRecvData) = 0;

	/***************************************************
	//函数功能：设置当前显示信息回调
	//参数：
	//		strViewShowData			当前显示信息
	//返回值：无
	***************************************************/
	virtual void OnSetRRCurViewShowAV(const std::string& strViewShowData) = 0;

	//转码回调
	virtual void OnRevNewTransCode(const std::string& strTRSMCUID,const std::string& strTRSMCUAddr,unsigned short usTRSMCUPort,const std::string& strTRSNodeID,const std::string& strTRSNatAddr,unsigned short usTRSNATPort,const std::string& strLocalAddr,unsigned short usLocalPort,unsigned long ulOldVideID,unsigned long ulNewAudio,unsigned long ulNewVideID)=0;

	/***************************************************
	//函数功能：设置全屏信息回调
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		strChannelID		通道ID
	//		bFull				true:设置全屏，false：取消全屏
	//		uErr				错误码
	//返回值：无
	***************************************************/
	virtual void OnSetFullScreen(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const std::string& strChannelID, bool bFull, const unsigned int& uErr) = 0;
	
	/***************************************************
	//函数功能：模板预置回调
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		uNumber				模板号
	//		uBandwidth			模板带宽
	//		uWidth				模板分辨率宽
	//		uHeight				模板分辨率高
	//		strPara				保留参数
	//		uErr				错误码
	//返回值：无
	***************************************************/
	virtual void OnTemplatePreset(const std::string& strRoomID, const std::string& strGroupID, const unsigned int uNumber, const unsigned int uBandwidth, const unsigned int uWidth, const unsigned int uHeight, const std::string& strPara, const unsigned int& uErr) = 0;

	/***************************************************
	//函数功能：窗口预设回调
	//参数：
	//		WndPreSet			请参考结构体定义
	//		uErr				错误码
	//返回值：无
	***************************************************/
	virtual void OnWindowPreset(const MMS_WNDPRESET& WndPreSet, const unsigned int& uErr) = 0;

	/***************************************************
	//函数功能：改变通道的音视频信息回调
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uChannelID			通道ID
	//		uStream				码流
	//		uWidth				模板分辨率宽
	//		uHeight				模板分辨率高
	//		bLock				是否采集锁定（true锁定，反之）
	//		uErr				错误码
	//返回值：无
	***************************************************/
	virtual void OnChangeAudioVedioInfo(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const unsigned int uStream, const unsigned int uWidth, const unsigned int uHeight, const unsigned int& uErr) = 0;

	/***************************************************
	//函数功能：采集锁定回调
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uChannelID			通道ID
	//		bLock				是否采集锁定（true锁定，反之）
	//		uErr				错误码
	//返回值：无
	***************************************************/
	virtual void OnCollectionLock(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uChannelID, const bool bLock, const unsigned int& uErr ) = 0;

	/***************************************************
	//函数功能：会议客户端的编码数据回调
	//参数：
	//		strRoomID			会议ID
	//		strGroupID			组ID
	//		strMemberID			人员ID
	//		uCardIndex
	//		uHeiStream
	//		uHWidth
	//		uHHeight
	//		uMidStream
	//		uMWidth
	//		uMHeight
	//		uLowStream
	//		uLWidth
	//		uLHeight
	//返回值：无
	***************************************************/
	virtual void OnBitStreamCalced(const std::string& strRoomID, const std::string& strGroupID, const std::string& strMemberID, const unsigned int uCardIndex, const unsigned int uHeiStream, const unsigned int uHWidth, const unsigned int uHHeight, const unsigned int uMidStream, const unsigned int uMWidth, const unsigned int uMHeight, const unsigned int uLowStream, const unsigned int uLWidth, const unsigned int uLHeight) = 0;
};

//=======================================================================================
// MMS实例
//=======================================================================================
AFX_EXT_CLASS bool MMSSessionAddEvent(KMMSSessionEvent* pMMSSessionEvent);
AFX_EXT_CLASS void MMSSessionRemoveEvent(KMMSSessionEvent* pMMSSessionEvent);
AFX_EXT_CLASS void MMSSessionRelease(void);
AFX_EXT_CLASS KMMSSession& GetMMSSession(void);


#include "KMONDefine.h"

class AFX_EXT_CLASS KMONSessionEvent;
//=======================================================================================
// MMS模块类
//=======================================================================================
class AFX_EXT_CLASS KMONSession
{
public:
	KMONSession(void){};
	virtual ~KMONSession(void){};

	virtual void OnDispatchCmd(KCmdPacket& t)=0;
	virtual void ClearLocalData(void)=0;

	/* 发送初始登录命令 */
	virtual void SendLoginCommand(void)=0;

	/* 登录
	 * nTermType(终端类型):0-软件终端 1-硬件终端 2-移动终端
	 * nTermName(终端名称):自定义
	 * strClientType(客户端类型): "SYS" "MON" "OCX" "MOB" *
     * strVersion(系统/软件版本号): 例 "<version> <system>XP</system> <software>6.2.3.7.2292</software> </version>" */
	virtual void Login(const std::string& strUserID,const std::string& strPassword, MON_DEVTYPE eDevType,const std::string& strName,const std::string& strSerialno, unsigned short usTermType=0,const std::string& strTermName="", const std::string& strVersion ="", const std::string& strMGSID = "")=0;

	/* 注销 */
	virtual void Logout(void)=0;

	/* 发送短信 */
	virtual void SendSMS(const std::string& strMobilNumber,const std::string& strMessage)=0;

	/* 设备上线 
	cstrBindMemberID用于会议终端,其它为空
	usFactory参照MON_FACTORY*/
	virtual void DeviceOnline(const MON_DEVTYPE& eDevType, const std::string& strDevID,const std::string& strPwd,const std::string& strMask,
		const std::string& strName, unsigned short usFactory = 0, const std::string& strVersion = "",const std::string& strBindMemberID = "",
		const std::string& strMGSID = "")=0;

	/* 设备下线 */
	virtual void DeviceOffline(const std::string& strDevID,const std::string& strBindMemberID = "")=0;

	/* 验证设备新帐号和序列号的绑定关系
	 * strDevID: 为现有设备帐号
	 * strNewDevID: 为新的设备帐号
	 * strSerialNo: 为设备的序列号 */
	virtual void DeviceCheck(const std::string& strDevID,const std::string& strNewDevID,const std::string& strSerialNo)=0;

	/* 修改设备名称 */
	virtual void UpdateDeviceName(const std::string& strDevID,const std::string& strName)=0;

	/* 发送DVR通道上线信息 */
	virtual void ChannelOnline(const MON_CHANNEL_INFO& tMonChannelInfo)=0;

	/* 发送DVR通道注销请求 nCHNum=-1 所有通道注销 */
	virtual void ChannelOffline(const std::string& strDevID,unsigned short nCHNum)=0; 

	/* 修改通道名称 */
	virtual void UpdateChannelName(const std::string& strChannelID,const std::string& strName)=0;
	
	/* 获取基础信息 */
	virtual void GetDeviceBasicInfo(const std::string& strDevID)=0;

	/* 定期更新(客户端每隔10分钟发起一次命令,
	 * strXMLData: 本地设备CPU/MEM等信息格式字符串,
	 * strSwitchSkedTime: 当前保存的上次获取关机计划的时间,
	 * 异步返回: 关机计划OnDeviceClick */
	virtual void DeviceClick(const std::string& strXMLData,const std::string& strSwitchSkedTime)=0;

	/* 发起对讲 */
	virtual void A2AInvite(const std::string &strCALLID,const MON_CHANNEL_INFO &tMonChannelInfo,const std::string &strSDPData)=0;

	/* 响应对讲 */
	virtual void A2ARespond(const std::string &strCALLID,const MON_CHANNEL_INFO &tMonChannelInfo,const std::string &strSDPData)=0;

	/* 中断对讲 */
	virtual void A2ABye(const std::string &strCALLID)=0;

	/* 报警 */
	virtual void Alert(const std::string &strDevID,unsigned short nCHNum,unsigned short nLevel,unsigned short nsubLevel=0)=0;
	
	/* 云台控制 */
	virtual int YTControl(const std::string &strDevID,unsigned short nCHNum,unsigned short nYTCmd, unsigned short nSpeed, unsigned short nPresetNO,const std::string &strPresetName )=0;// 云台控制

	/* 发出获取云台控制参数请求 */
	virtual void GetYTControlParameters(const std::string &strDevID, unsigned short nCHNum)=0;

	/* 发出获取云台连接参数请求 */
	virtual void GetYTConnectionParameters(const std::string &strDevID, unsigned short nCHNum)=0;

	/* 设置云台控制参数 */
	virtual void SetYTControlParameters(const std::string &strDevID,const std::string &strToID, const YT_CONTROL_PARAMETERS & YTParam)=0;

	/* 设置云台连接参数 */
	virtual void SetYTConnectionParameters(const std::string &strDevID,const std::string &strToID, unsigned short nChannelNum, const std::string &strType, unsigned short nCom,  unsigned short  nSerialNo)=0;

	/* 会议P2P */
	virtual void P2P(const std::string& strUserID, KCmdUserDataPacket& packet)=0;

	/* 获取录像服务器地址 */
	virtual void GetVRSServer(void)=0;

	/* 定期更新(客户端每隔10分钟发起一次命令,
	* strXMLData: 本地设备CPU/Disk等信息格式字符串,
	* strSwitchSkedTime: 当前保存的上次获取计划的时间,
	* strSwitchStrategyTime: 当前保存的上次获取策略的时间,
	* 异步返回: NVR录像及上传的计划OnNVRSchedule；NVR录像策略OnNVRStrategy
	*/
	virtual void MGSStatusReport(const std::string& strXMLData,const std::string& strSwitchSkedTime,const std::string& strSwitchStrategyTime)=0;


	/* 上报通道录像及上传状态，每当发生状态改变时调用
	* strDevID: 设备ID，
	* nCHNum：通道ID，
	* nRecordStatus：录像状态
	* nUploadStatus：上传状态
	*/
	virtual void ChannelStatusReport(const std::string &strDevID,unsigned short nCHNum, unsigned short nRecordStatus, unsigned short nUploadStatus)=0;

	/* 设备通道音视频ID更新 */
	virtual void DevChannelChange(const std::string& strDevID,const std::string& strChannelID,unsigned long ulVideoID,unsigned long ulAudioID)=0;

   	//录像计划MTS_R不装载MON模块 所以需要传入userid(以下三个接口在IMSession中调用)
	/* 获取监控录像计划 */
	virtual void GetMONRecTask(const std::string& strUpdatetime, const std::string& strUserID) = 0;

	/* 获取监控通道录像计划 */
	virtual void GetMONRecChnlTask(const std::string& strChnlID, const std::string& strUpdatetime, const std::string& strUserID) = 0;

	/* 获取监控录像策略(例如:录像的分割类型,大小) */
	virtual void GetMONRecStrategy(const std::string& strUpdatetime, const std::string& strUserID) = 0;
	
	/* 获取设备信息 */
	virtual void GetMonDevInfo(const std::string& strDevID) = 0;
};

//=======================================================================================
// MON模块事件类
//=======================================================================================
class KMONSessionEvent
{
public:
	KMONSessionEvent(void){};
	virtual ~KMONSessionEvent(void){};
	
public:
	/* 登录返回码nErrCode: 
	 * 0=登录成功
	 * 1=用户账号不存在
	 * 2=用户密码无效
	 * 3=用户已登录
	 * 99=系统错误 */
	virtual void OnLogin(int nErrCode)=0;

	/* 注销返回码nErrCode: 
	 * 0=登录成功
	 * 1=系统断开
	 * 99=系统错误 */
	virtual void OnLogout(int nErrCode,const std::string& strDomain)=0;

	/* 设备上线
	* nErrCode=0 上线成功
	* nErrCode!=0上线失败
	*/
	virtual void OnDeviceOnline(int nErrCode,KCmdPacket& rCmdPacket)=0;

	/* 设备下线 */
	virtual void OnDeviceOffline(KCmdPacket& rCmdPacket)=0;

	/* 设备检查 */
	virtual void OnDeviceCheck(KCmdPacket& rCmdPacket)=0;

	/* 收到通道上线 (DEVID,CHNUM,ERRCODE) */
	virtual void OnChannelOnline(KCmdPacket& rCmdPacket)=0;

	/* 收到通道下线 */
	virtual void OnChannelOffline(KCmdPacket& rCmdPacket)=0;

	/* 获取基本信息 */
	virtual void OnDeviceBasicInfo(KCmdPacket& rCmdPacket)=0;

	/* 收到关机计划
	 * GetAttrib("XMLDAT"): 关机计划,
	 * GetAttrib("SSTIME"): 本次获取的关机计划时间 */
	virtual void OnDeviceClick(KCmdPacket& rCmdPacket)=0;

	/* 报警 */
	virtual void OnAlert(KCmdPacket& rCmdPacket)=0;

	/* 报警 */
	virtual void OnUDPRcvAlert(const UDP_RCV_ALART_INFO& info)=0;

	/* 处理对讲 */
	virtual void OnA2A(KCmdPacket& rCmdPacket)=0;

	/* 处理对讲 */
	virtual void OnA2ABye(KCmdPacket& rCmdPacket)=0;

	/* 处理云台控制请求 */
	virtual void OnYTControl(KCmdPacket& rCmdPacket)=0;

	/* 获取云台连接参数请求 */
	virtual void OnGetYTConnectionParameters(KCmdPacket& rCmdPacket)=0;

	/* 获取云台控制参数请求 */
	virtual void OnGetYTControlParameters(KCmdPacket& rCmdPacket)=0;

	/* 设置云台连接参数 */
	virtual void OnSetYTConnectionParameters(KCmdPacket& rCmdPacket)=0;

	/* 设置云台控制参数 */
	virtual void OnSetYTControlParameters(KCmdPacket& rCmdPacket)=0;

	/* 返回会议P2P */
	virtual void OnP2P(const std::string& strUserID,KCmdUserDataPacket& packet)=0;

	/* 获得VRS服务器VRSID,VRSADDR */
	virtual void OnVRSServer(KCmdPacket& rCmdPacket)=0;

	/* 收到NVR录像策略
	* GetAttrib("XMLDAT_STRATEGY"): 录像策略,
	* GetAttrib("SSTIME"): 本次获取计划的时间 
	*/
	virtual void OnNVRStrategy(KCmdPacket& rCmdPacket)=0;


	/* 收到NVR录像及上传的计划
	* GetAttrib("DEVID"): 设备ID，
	* GetAttrib("CHANNELNUM:")：通道ID，
	* GetAttrib("XMLDAT_RECORD"): 录像计划,
	* GetAttrib("XMLDAT_UPLOAD"): 上传计划,
	* GetAttrib("SSTIME"): 本次获取计划的时间
	*/
	virtual void OnNVRSchedule(KCmdPacket& rCmdPacket)=0;
	/* 获取设备信息 */
	virtual void OnGetMonDevInfo(const MONDEV& item,const std::string& strDomain)=0;
};

//=======================================================================================
// MON实例
//=======================================================================================
AFX_EXT_CLASS bool MONSessionAddEvent(KMONSessionEvent* pMONSessionEvent);
AFX_EXT_CLASS void MONSessionRemoveEvent(KMONSessionEvent* pMONSessionEvent);
AFX_EXT_CLASS void MONSessionRelease(void);
AFX_EXT_CLASS KMONSession& GetMONSession(void);

//=======================================================================================
// AVC模块类
//=======================================================================================
class AFX_EXT_CLASS KAVCSession
{
public:
	KAVCSession(void){};
	virtual ~KAVCSession(void){};

	enum{CALLINGOUT,CALLINGIN,TALKING};

public:
	//-------------------------------------------
	// 外调事件
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual AVC_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PAVC_CALL_ITEM GetCallItem(const std::string& toid)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// AVC模块事件类
//=======================================================================================
class AFX_EXT_CLASS KAVCSessionEvent
{
public:
	KAVCSessionEvent(void){};
	virtual ~KAVCSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// AVC实例
//=======================================================================================
AFX_EXT_CLASS bool AVCSessionAddEvent(KAVCSessionEvent* pAVCSessionEvent);
AFX_EXT_CLASS void AVCSessionRemoveEvent(KAVCSessionEvent* pAVCSessionEvent);
AFX_EXT_CLASS void AVCSessionRelease(void);
AFX_EXT_CLASS KAVCSession& GetAVCSession(void);

//=======================================================================================
// WHB模块类
//=======================================================================================
class AFX_EXT_CLASS KWHBSession
{
public:
	KWHBSession(void){};
	virtual ~KWHBSession(void){};
	
	/* 清理SESSION */
	virtual void Close()=0;	

	enum{CALLINGOUT,CALLINGIN,TALKING};

	//-------------------------------------------
	// 外调事件
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual WHB_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PWHB_CALL_ITEM GetCallItem(const std::string& strNodeID)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// WHB模块事件类
//=======================================================================================
class AFX_EXT_CLASS KWHBSessionEvent
{
public:
	KWHBSessionEvent(void){};
	virtual ~KWHBSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// WHB实例
//=======================================================================================
AFX_EXT_CLASS bool WHBSessionAddEvent(KWHBSessionEvent* pWHBSessionEvent);
AFX_EXT_CLASS void WHBSessionRemoveEvent(KWHBSessionEvent* pWHBSessionEvent);
AFX_EXT_CLASS void WHBSessionRelease(void);
AFX_EXT_CLASS KWHBSession& GetWHBSession(void);

//=======================================================================================
// SVR模块类
//=======================================================================================
class AFX_EXT_CLASS KSRVSession
{
public:
	KSRVSession(void){};
	virtual ~KSRVSession(void){};
	
	/* 清理SESSION */
	virtual void Close()=0;

	enum{CALLINGOUT,CALLINGIN,TALKING};

public:
	//-------------------------------------------
	// 外调事件
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual SRV_CALL_ITEM_MAP& GetCallItemMap(void)=0;
	virtual PSRV_CALL_ITEM GetCallItem(const std::string& toid)=0;
	virtual std::string GetUserID(void)=0;

	virtual bool Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strData)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strData)=0;
	virtual void Pickup(const std::string& strNodeID,const std::string& strData)=0; 
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// SVR模块事件类
//=======================================================================================
class AFX_EXT_CLASS KSRVSessionEvent
{
public:
	KSRVSessionEvent(void){};
	virtual ~KSRVSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnPickup(const std::string& strUserID,const std::string& strNodeID,const std::string& strData)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// SRV实例
//=======================================================================================
AFX_EXT_CLASS bool SRVSessionAddEvent(KSRVSessionEvent* pSRVSessionEvent);
AFX_EXT_CLASS void SRVSessionRemoveEvent(KSRVSessionEvent* pSRVSessionEvent);
AFX_EXT_CLASS void SRVSessionRelease(void);
AFX_EXT_CLASS KSRVSession& GetSRVSession(void);

//=======================================================================================
// FIL模块类
//=======================================================================================
class AFX_EXT_CLASS KFILSession
{
public:
	KFILSession(void){};
	virtual ~KFILSession(void){};
	
	/* 清理SESSION */
	virtual void Close()=0;

public:
	//-------------------------------------------
	// 外调事件
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	virtual std::string GetUserID(void)=0;

	virtual void Call(const std::string& strNodeID,const std::string& strUserName,const std::string& strFileName,unsigned long ulFileSize)=0;
	virtual void Hangup(const std::string& strNodeID,const std::string& strFileName)=0;
	virtual void SendUserData(const std::string& strNodeID,KCmdPacket& packet)=0;
};

//=======================================================================================
// FIL模块事件类
//=======================================================================================
class AFX_EXT_CLASS KFILSessionEvent
{
public:
	KFILSessionEvent(void){};
	virtual ~KFILSessionEvent(void){};

public:
	virtual void OnCall(const std::string& strUserID,const std::string& strUserName,const std::string& strNodeID,const std::string& strFileName,unsigned long ulFileSize)=0;
	virtual void OnHangup(const std::string& strUserID,const std::string& strNodeID,const std::string& strFileName)=0;
	virtual void OnUserData(KCmdPacket& packet)=0;
};

//=======================================================================================
// FIL实例
//=======================================================================================
AFX_EXT_CLASS bool FILSessionAddEvent(KFILSessionEvent* pFILSessionEvent);
AFX_EXT_CLASS void FILSessionRemoveEvent(KFILSessionEvent* pFILSessionEvent);
AFX_EXT_CLASS void FILSessionRelease(void);
AFX_EXT_CLASS KFILSession& GetFILSession(void);

//=======================================================================================
// SMS模块类
//=======================================================================================
class AFX_EXT_CLASS KSMSSession
{
public:
	KSMSSession(void){};
	virtual ~KSMSSession(void){};

	/* 清理SESSION */
	virtual void Close()=0;

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* 发送个人短信
	 * strReceiverCode: 接收者手机号码,支持多个号码,用分号(;)隔开 */
	virtual void Send(const std::string& strReceiverCode,const std::string& strContent)=0;

	/* 发送企业短信
	 * strOrgID:        机构ID
	 * strDepartID;		部门ID,如果向该机构所有在线用户发送则strDepartID设为空 */
	virtual void Send(const std::string& strOrgID,const std::string& strDepartID,const std::string& strContent)=0;
};

//=======================================================================================
// SMS模块事件类
//=======================================================================================
class AFX_EXT_CLASS KSMSSessionEvent
{
public:
	KSMSSessionEvent(void){};
	virtual ~KSMSSessionEvent(void){};

public:
	/* 短信发送后的反馈信息
	 * nType: 0=个人短信 1=企业短信 */
	virtual void OnSend(unsigned short nType,bool bSuccess,const std::string& strErrText,const std::string& strRecvCode,const std::string& strContent)=0;

	/* 收到短信
	 * nType: 0=个人短信 1=企业短信 */
	virtual void OnReceive(unsigned short nType,const std::string& strUserID,const std::string& strSenderCode,const std::string& strContent,const std::string& strRecvTime)=0;
};

//=======================================================================================
// SMS实例
//=======================================================================================
AFX_EXT_CLASS bool SMSSessionAddEvent(KSMSSessionEvent* pSMSSessionEvent);
AFX_EXT_CLASS void SMSSessionRemoveEvent(KSMSSessionEvent* pSMSSessionEvent);
AFX_EXT_CLASS void SMSSessionRelease(void);
AFX_EXT_CLASS KSMSSession& GetSMSSession(void);

//=======================================================================================
// NDS模块类
//=======================================================================================
#include "KNDSDefine.h"
class AFX_EXT_CLASS KNDSSession
{
public:
	KNDSSession(void){};
	virtual ~KNDSSession(void){};

	/* 清理SESSION */
	virtual void Close()=0;

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

    /* 获取全局配置参数 */
    virtual void GetGlobalParam()=0;

    /* 获取本用户的NDS文件列表，待下载 */
    virtual void GetNDSFileInfo() = 0;

    /* 上传文件，此时， fileid已生成，文件已处于发送状态 */
    virtual void UploadFile( PTNDSFileInfo ptNDSFileInfo) = 0;

    /* 下载文件 */
    virtual void DownloadFile( PTNDSFileInfo ptNDSFileInfo, const std::string& strFilePath) = 0;

    /* 停止上传，与数据库操作无关，只填文件 fileid 即可 */
    virtual void StopUploadFile( unsigned long ulFileID/*, const std::string& strMCUNodeID*/) =0;

    /* 停止下载 */
    virtual void StopDownloadFile( unsigned long ulFileID/*, const std::string& strMCUNodeID*/) =0;

    /* 取消上传，要更改数据库内容 */
    virtual void CancelUploadFile( PTNDSFileInfo ptNDSFileInfo/*, unsigned long ulNETECFileID, const std::string& strMCUNodeID*/) = 0;

    /* 取消下载 */
    virtual void CancelDownloadFile( PTNDSFileInfo ptNDSFileInfo/*, unsigned long ulNETECFileID = 0, const std::string& strMCUNodeID = ""*/) = 0;


    //////////////////////////////////////////////////////////////////////////
    // 以下参见 NETEC_File.h
    virtual unsigned long GetFileTransferredBytes( unsigned long ulFileID, bool bUpload )=0;
    virtual int GetBitrate( unsigned long ulFileID, bool bUpload )=0;
    virtual int GetPercent( unsigned long ulFileID, bool bUpload )=0;
};

//=======================================================================================
// NDS模块事件类
//=======================================================================================
class AFX_EXT_CLASS KNDSSessionEvent
{
public:
	KNDSSessionEvent(void){};
	virtual ~KNDSSessionEvent(void){};

public:
    /* 返回全局配置参数(最大文件大小,单位: M) */
    virtual void OnGlobalParam( unsigned long ulMaxFileSize) = 0;

    /* 返回下载文件列表 */
    virtual void OnNDSFile( PTNDSFileInfo ptNDSFileInfo) = 0;

    /* 返回下载文件列表结束 */
    virtual void OnNDSFileEnd() = 0;

    /* 上传开始 */
	virtual void OnUploadFileStart(PTNDSFileInfo ptNDSFileInfo ) = 0;

    /* 上传完成 */
    virtual void OnUploadFileEnd( unsigned long ulFileID )=0;

    /* 下载所需信息返回 */
    virtual void OnDownloadFileStart( PTNDSFileInfo ptNDSFileInfo )=0;

    /* 下载所需信息返回 */
    virtual void OnDownloadFileEnd( unsigned long ulFileID )=0;

    /* 上传错误 */
    virtual void OnUploadError( PTNDSFileInfo ptNDSFileInfo, int nErrCode)=0;

    /* 下载错误 */
    virtual void OnDownloadError( PTNDSFileInfo ptNDSFileInfo, int nErrCode)=0;
};

//=======================================================================================
// NDS实例
//=======================================================================================
AFX_EXT_CLASS bool NDSSessionAddEvent(KNDSSessionEvent* pNDSSessionEvent);
AFX_EXT_CLASS void NDSSessionRemoveEvent(KNDSSessionEvent* pNDSSessionEvent);
AFX_EXT_CLASS void NDSSessionRelease(void);
AFX_EXT_CLASS KNDSSession& GetNDSSession(void);

//=======================================================================================
// FAX模块类
//=======================================================================================
class AFX_EXT_CLASS KFAXSession
{
public:
	KFAXSession(void){};
	virtual ~KFAXSession(void){};

public:
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

	/* 获取传真服务器信息 */
	virtual void GetServer(void)=0;
	virtual void GetFax(void)=0;

	/*发送传真开始*/
	virtual void SendFaxStart(const std::string& strNodeID, unsigned long ulFileID,unsigned long ulFileLength,const std::string& strMCUID,const std::string& strMCUIP,unsigned short usMCUPort,const std::string& strMCUType,const std::string& strFaxNum)=0;
	/*发送传真结束*/
	virtual void SendFaxStop(const std::string& strNodeID, unsigned long nFileID)=0;
	/*接收传真开始*/
	virtual void RecvFaxStart(const std::string& strNodeID, unsigned long ulFaxID)=0;
	/*接收传真结束*/
	virtual void RecvFaxStop(const std::string& strNodeID, unsigned long ulFaxID, unsigned long nFileID)=0;
};

//=======================================================================================
// FAX模块事件类
//=======================================================================================
class AFX_EXT_CLASS KFAXSessionEvent
{
public:
	KFAXSessionEvent(void){};
	virtual ~KFAXSessionEvent(void){};

public:
	/* 获取传真服务器信息 */
	virtual void OnServer(const std::string& strNodeID)=0;

	/* 传真提示 */
	virtual void OnFax(const std::string& strNodeID, unsigned long ulFaxID)=0;
	virtual void OnFaxEnd()=0;
	/*接收传真开始*/
	virtual void OnRecvFaxStart(const std::string& strNodeID, unsigned long ulFaxID, unsigned long ulFileID,unsigned long ulFileLength,const std::string& strMCUID,const std::string& strMCUIP,unsigned short usMCUPort,const std::string& strMCUType)=0;
};

//=======================================================================================
// FAX实例
//=======================================================================================
AFX_EXT_CLASS bool FAXSessionAddEvent(KFAXSessionEvent* pFAXSessionEvent);
AFX_EXT_CLASS void FAXSessionRemoveEvent(KFAXSessionEvent* pFAXSessionEvent);
AFX_EXT_CLASS void FAXSessionRelease(void);
AFX_EXT_CLASS KFAXSession& GetFAXSession(void);

//=======================================================================================
// 网络文件模块类
//=======================================================================================
#include "KNFSDefine.h"

class AFX_EXT_CLASS KNFSSession
{
public:
	KNFSSession(void);
	virtual ~KNFSSession(void);

	static bool Start(void);
	static void Stop(void);
};

//---------------------------------------------------------
// KNFSUploadSessionEvent 事件接口类
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSUploadSessionEvent
{
public:
	KNFSUploadSessionEvent(void){};
	virtual ~KNFSUploadSessionEvent(void){};
	
	/* 连接成功  */
	virtual void OnConnected(void)=0;

	/* 连接断开  */
	virtual void OnDisconnected(int nErrCode)=0;

	/* 上传错误 */
	virtual void OnUploadError(int nErrCode)=0;

	/* 上传进程 */
	virtual void OnUploadProgress(double dPercent,double dSpeed)=0;

	/* 上传结束 */
	virtual void OnUploadEnd(void)=0;

	/* 删除 */
	virtual void OnDelete(int nErrCode)=0;
};

//---------------------------------------------------------
// KNFSUploadSession 接口类
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSUploadSession
{
public:
	KNFSUploadSession(void){};
	virtual ~KNFSUploadSession(void){};

	/* 连接服务器 */
	virtual bool Connect(const std::string& strServerAddr,unsigned short usServerPort)=0;

	/* 断开服务器连接 */
	virtual void Disconnect(void)=0;

	/* 上传文件:
	 * strLocalFileFullName:  本地上传文件全路径名(包含文件名)
	 * strUploadFilePath:     上传文件路径(服务器相对路径):/emaps/part01
	 * strUploadFileName:     上传文件名称: map0001.jpg
     * 说明:保存到服务器上的文件位置: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * 返回: 0=成功执行 */
	virtual int Upload(const std::string& strLocalFileFullName,const std::string& strUploadFilePath,const std::string& strUploadFileName)=0;

	/* 删除文件
	 * strUploadFilePath:     上传文件路径(服务器相对路径):/emaps/part01
	 * strUploadFileName:     上传文件名称: map0001.jpg
     * 说明:保存到服务器上的文件位置: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * 返回: 0=成功执行 */
	virtual int Delete(const std::string& strDeleteFilePath,const std::string& strDeleteFileName)=0;
};

//---------------------------------------------------------
// KNFSDownloadSessionEvent 事件接口类
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSDownloadSessionEvent
{
public:
	KNFSDownloadSessionEvent(void){};
	virtual ~KNFSDownloadSessionEvent(void){};
	
	/* 连接成功  */
	virtual void OnConnected(void)=0;

	/* 连接断开  */
	virtual void OnDisconnected(int nErrCode)=0;

	/* 下载开始 */
	virtual void OnDownloadStart(unsigned long ulFileSize)=0;

	/* 下载错误 */
	virtual void OnDownloadError(int nErrCode)=0;

	/* 下载进程 */
	virtual void OnDownloadProgress(double dPercent,double dSpeed)=0;

	/* 下载结束 */
	virtual void OnDownloadEnd(void)=0;

	/* 删除 */
	virtual void OnDelete(int nErrCode)=0;
};

//---------------------------------------------------------
// KNFSDownloadSession 接口类
//---------------------------------------------------------
class AFX_EXT_CLASS KNFSDownloadSession
{
public:
	KNFSDownloadSession(void){};
	virtual ~KNFSDownloadSession(void){};

	/* 连接服务器 */
	virtual bool Connect(const std::string& strServerAddr,unsigned short usServerPort)=0;

	/* 断开服务器连接 */
	virtual void Disconnect(void)=0;

	/* 上传文件:
	 * strLocalFileFullName: 本地文件全路径
	 * strDownloadFilePath:  下载文件路径(服务器相对路径):/emaps/part01
	 * strDownloadFileName:  下载文件名称: map0001.jpg
     * 说明:从服务器上下载文件/usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * 返回: 0=成功执行 */
	virtual int Download(const std::string& strLocalFileFullName,const std::string& strDownloadFilePath,const std::string& strDownloadFileName)=0;

	/* 删除文件
	 * strUploadFilePath:     上传文件路径(服务器相对路径):/emaps/part01
	 * strUploadFileName:     上传文件名称: map0001.jpg
     * 说明:保存到服务器上的文件位置: /usr/local/avcond/storage/emaps/part01/map0001.jpg
	 * 返回: 0=成功执行 */
	virtual int Delete(const std::string& strDeleteFilePath,const std::string& strDeleteFileName)=0;
};

//---------------------------------------------------------
// 创建上传SESSION
//---------------------------------------------------------
AFX_EXT_CLASS KNFSUploadSession* CreateNFSUploadSession(KNFSUploadSessionEvent& rNFSUploadSessionEvent);

//---------------------------------------------------------
// 创建下载SESSION
//---------------------------------------------------------
AFX_EXT_CLASS KNFSDownloadSession* CreateNFSDownloadSession(KNFSDownloadSessionEvent& rNFSDownloadSessionEvent);


//=======================================================================================
// AAA
//=======================================================================================

//---------------------------------------------------------------------------------------
/* 验证用户LDAP帐户
 * Return Values: true=验证成功, false=验证失败 */
 AFX_EXT_CLASS bool AAA_LDAP_Authorize(const std::string& strServerHost,unsigned short usServerPort,
	                     const std::string& strUserID,const std::string& strPassword);

 
//=======================================================================================
// 录播服务器接口
//=======================================================================================

#include "KRECDefine.h"
//---------------------------------------------------------
// KRECSession 接口类
//---------------------------------------------------------
class KRECSession
{
public:
	KRECSession(){};
	~KRECSession(){};

	virtual void SetUserID(const std::string& strUserID)=0;

	virtual void SetDomain(const std::string& strDomain)=0;

	/* SYS网络模块调用传入命令包 */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

public:
	//默认录制avms，如要录制mp4格式，请将recfile.ePreRecType赋值为REC_FILE_TYPE_MP4
	virtual void StartChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopChlRec(const RECFILEINFO& recfile)=0;
	virtual void StopRecAll(const std::string& strRoomID)=0;
	//默认录制tpc，如要录制mrec格式，请将recfile.ePreRecType赋值为REC_FILE_TYPE_MREC
	virtual void StartConfRec(const RECFILEINFO& recfile)=0;
	virtual void StopConfRec(const std::string& strRoomID)=0;
	//录制mrec格式时，请将recfile.ePreRecType赋值为REC_FILE_TYPE_MREC，并注意给recfile.strCheckNum赋值
	virtual void StartConfAVRec(const RECFILEINFO& recfile)=0;
	//停止录制mrec格式时，请将recfile.ePreRecType赋值为REC_FILE_TYPE_MREC
	virtual void StopConfAVRec(const RECFILEINFO& recfile)=0;
	virtual void SaveConfStream(const std::string& strRecSvrNodeID,char* outPacket, int nLen)=0;
	virtual void StartConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;
	virtual void StopConfDosRec(const std::string& strRoomID, const std::string& strMemberID)=0;
	//客户端从正在录像的视频抓取图片保存到录播服务器
	virtual void SendPicToRecServer(const std::string& strRoomID, const std::string& strMemberID, const std::string& strChnID, const std::string& strFile, bool bConfRecord)=0;
	//uStreamType：0高码流，1中码流，2低码流
	virtual void ChangeStreamType(const std::string& strRoomID,const std::string& strDevid,const std::string& strChnID,unsigned short uStreamType,const std::string& strUserid,const REC_FILE_TYPE eRecType=REC_FILE_TYPE_TPC)=0;
};

//---------------------------------------------------------
// KRECSessionEvent 接口回调类
//---------------------------------------------------------
class KRECSessionEvent
{
public:
	KRECSessionEvent(){};
	~KRECSessionEvent(){};

public:
	/************************************************************************
	* ERRCODE 说明：
	*  宏                           值                       说明
	* ERR_REC_SVRREC_BUSY			    70001					录播服务器忙，或没有可用的录播服务器
	* ERR_REC_SVR_EXCEPTION				70002					录播服务器异常
	* REC_START_SUCCESS					0						开始录像
	* REC_START_RECMAXCNT				1						达到授权数
	* REC_STREAM_FAILED					2						接流失败
	* REC_START_EXIST					3						已经在录
	* REC_START_DISKFULL				4						磁盘空间不足
	* REC_START_NODISE					5						没有磁盘
	* REC_START_OTHER					6						其他错误
	* 如果有更新请参考HPRecordDefine.h -> RECERR_CODE定义
	************************************************************************/
	virtual void OnRecServerStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecLiveStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecServerConfStatus(const RECFILEINFO& recfile, const std::string& strRecSvrNodeID, int nErrorCode)=0;

	virtual void OnRecServerConfAVStatus(const RECFILEINFO& recfile, int nErrorCode)=0;

	virtual void OnRecServerConfFull() = 0;
};

//=======================================================================================
// REC实例
//=======================================================================================
AFX_EXT_CLASS bool RECSessionAddEvent(KRECSessionEvent* pRECSessionEvent);
AFX_EXT_CLASS void RECSessionRemoveEvent(KRECSessionEvent* pRECSessionEvent);
AFX_EXT_CLASS void RECSessionRelease(void);
AFX_EXT_CLASS KRECSession& GetRECSession(void);

#include "KCTSDefine.h"
//---------------------------------------------------------
// KCTSession 定制业务接口基类
//---------------------------------------------------------
class AFX_EXT_CLASS KCTSSession
{
public:
	KCTSSession(){};
	~KCTSSession(){};

	virtual void SetDomain(const std::string& strDomain)=0;
	/* SYS网络模块调用传入命令包 */
	virtual void OnDispatchCmd(KCmdPacket& t)=0;

public:
//数字化手术示教使用---------------------------------------------------------------------------------
	//请求绑定会议()
	//strRoomID:请求绑定或解绑定的会议室ID，bBind（true:请求绑定，false:请求解除绑定）
	virtual void ApplyBindMeeting(const std::string& strRoomID, const bool& bBind)=0;
	//添加病人信息
	//strOperator:信息添加者，patientInfo：患者信息
	virtual void AddPatientInfo(const std::string& strOperator, const MEDICAID_PATIENT& patientInfo)=0;
	//查找病人信息
	//strInpatientNO:患者住院号,strPatientName:患者姓名
	virtual void SearchPatientInfo(const std::string& strInpatientNO, const std::string& strPatientName)=0;
	//删除所有病人信息
	//strOperator:删除患者信息操作者
	virtual void DelAllPatients(const std::string& strOperator) = 0;
	//删除某个病人信息
	//strOperator:删除患者信息操作者, strInpatientNO:被删除患者信息的住院号
	virtual	void DelOnePatient(const std::string& strOperator,const std::string& strInPatientNO) = 0;
	//更新病人信息
	//strOperator:更新患者信息操作者， strPatientNO:住院号,newPatientInfo:新的病人信息
	virtual void UpdateOnePatientInfo(const std::string& strOperator, std::string& strInPatientNO,const MEDICAID_PATIENT& newPatientInfo) = 0;
	//获取患者列表
	//iCounts:需要获取的患者数量（小于0：获取所有）
	virtual void FetchPatients(const int& iCounts) = 0;
//数字化手术示教使用---------------------------------------------------------------------------------

//edu二期--------------------------------------------------------------------------------------------
	//提交课堂操作日志
	//ulLogID:本次操作日志的id（具有唯一性,某次操作日志开始和结束共用同一ID,值由客户端自己生成）,strOperateMemberID:此次操作者ID，
	//operateLogType:操作日志类型，bStart:（true操作开始，false:操作结束）
	virtual void SendClassOperateLog(const unsigned long& ulClassLogID,const unsigned long& ulLogID,const std::string& strOperateMemberID,const std::string& strRoomID,const std::string& strRoomName,const MMS_CALSSOPERATELOG_TYPE& operateLogType,const bool& bStart) = 0;
//edu二期--------------------------------------------------------------------------------------------
};

//---------------------------------------------------------
// KCTSessionEvent 定制业务回调接口基类
//---------------------------------------------------------
class AFX_EXT_CLASS KCTSSessionEvent
{
public:
	KCTSSessionEvent(){};
	~KCTSSessionEvent(){};
//数字化手术示教使用---------------------------------------------------------------------------------
	//收到绑定、解除绑定会议失败回调
	virtual void OnApplyBindMeetingErr(const std::string& strRoomID, const unsigned int& uError)=0;
	//收到会议绑定或解除绑定回调(bBind:true会议被绑定，false会议被解除绑定)
	virtual void OnMeetingBind(const std::string& strRoomID,const bool& bBind)=0;
	//收到新添加患者信息
	virtual void OnAddPatientInfo(const std::string& strOperator,const MEDICAID_PATIENT& patientInfo) = 0;
	//收到添加患者信息失败
	virtual void OnAddPatientInfoErr(const unsigned int& uError)=0;
	virtual void OnSearchPatientInfo(const MEDICAID_PATIENT& patientInfo)=0;
	virtual void OnSearchPatientInfoEnd()=0;
	//收到查询患者信息失败
	virtual void OnSearchPatientInfoErr(const unsigned int& uError)=0;
	//收到删除患者信息失败回调
	virtual void OnDelOnePatientErr(const std::string& strPatientNO,const unsigned int& uError) = 0;
	//收到删除患者信息回调
	virtual void OnDelOnePatient(const std::string& strOperator,const std::string& strPatientNO) = 0;
	//收到删除所有患者信息回调
	virtual void OnDelAllPatients(const std::string& strOperator) = 0;
	//收到更新某病人信息回调
	//成功更新患者信息，strOldPatientNO：原住院号,newPatientInfo：新的患者信息
	virtual	void OnUpdatePatientInfo(const std::string& strOperator,const std::string& strOldPatientNO, const MEDICAID_PATIENT& newPatientInfo) = 0;
	//更新患者信息失败
	virtual void OnUpdatePatientErr(const std::string& strOldPatientNO,const unsigned int& uError) = 0;
//数字化手术示教使用---------------------------------------------------------------------------------

//edu二期--------------------------------------------------------------------------------------------
	//推送课堂模式列表
	virtual void OnRecvEduRoomModeList(const EDUROOMMODEINFOLST& lstEduRoomModeInfo)=0;
	//收到课堂权限码
	virtual void OnRecevEduRoomPrivcode(const std::string& strRoomID,const std::string& strMemberID,const unsigned long& ulPriv)=0;
//edu二期--------------------------------------------------------------------------------------------
};

//=======================================================================================
// 定制业务全局接口
//=======================================================================================
AFX_EXT_CLASS bool CTSSessionAddEvent(KCTSSessionEvent* pCTSSessionEvent);
AFX_EXT_CLASS void CTSSessionRemoveEvent(KCTSSessionEvent* pCTSSessionEvent);
AFX_EXT_CLASS void CTSSessionRelease(void);
AFX_EXT_CLASS KCTSSession& GetCTSSession(void);