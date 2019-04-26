// UCC7-IM群组功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_IM_TeamGroup_Observer
	{
	public:
		virtual ~IPlugin_U7_IM_TeamGroup_Observer(){};

	public:
		/*讨论组*/
		// 创建讨论组回调
		virtual void OnAddDiscussArea(const unsigned long& ulDiscussID,const char* strGroupName,const char* strMember) = 0;

		//************************************
		// 说明:    创建讨论组结束回调
		// 参数:	ulDiscussID 讨论组ID
		// 参数:	ErrorCode	ErrorCode不为0则创建失败,错误代码参照KXNCP.h文件
		// 返回值:  void
		//************************************
		virtual void OnAddDiscussAreaEnd(const unsigned long& ulDiscussID, unsigned int ErrorCode) = 0;

		// 收到讨论组回调
		virtual void OnGetDiscussInfo(const unsigned long& ulDiscussID,const char* strGroupName,const char* strMember) = 0;
		// 收到讨论组结束回调
		virtual void OnGetDiscussInfoEnd(const unsigned long& ulDiscussID) = 0;
		// 收到讨论组添加人员 
		virtual void OnAddDiscussMember(const unsigned long& ulDiscussID,const char* member) = 0;
		// 收到讨论组添加人员结束
		virtual void OnAddDiscussMemberEnd(const unsigned long& ulDiscussID, unsigned int ErrorCode) = 0;
		// 收到退出讨论组回调
		virtual void OnExitDiscuss(const unsigned long& ulDiscussID,const char* member)=0;
		// 收到讨论组消息回调
		virtual void OnDiscussMessage(const unsigned long& ulDiscussID, const char* strFromID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		// 收到修改讨论组名称回调
		virtual void OnModDiscussName(const unsigned long& ulDiscussID, const char*strName) = 0;

		/*群*/
		/*创建群回调（LoadTeamList后也会进此回调*/
		virtual void OnCreatTeam(const TEAM_DES& team) = 0;
		/*服务器发送的待处理消息*/
		virtual void OnTeamRequest(const TEAM_REQUEST& apply) = 0;
		/*新成员加入返回*/
		virtual void OnAddTeamMember(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*受理完毕返回*/
		virtual void OnRequestReturn(const TEAM_REQUEST_RESULT& HandleReturn) = 0;
		/*离开群回调*/
		virtual void OnExitTeam(const unsigned long& ulTeamID) = 0;
		/*退出成员返回*/
		virtual void OnMinusTeamMember(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*收到群消息回调(自己发送到消息也会进入此回调)*/
		virtual void OnTeamMessage(const unsigned long& ulTeamID, const char* strFromID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;
		/*晋升返回*/
		virtual void OnPromotion(const unsigned long& ulTeamID,const char* strMember) = 0;
		/*降级返回*/
		virtual void OnDemote(const unsigned long& ulTeamID,const char*strMember) = 0;
		/*更改群信息返回*/
		virtual void OnUpdateTeamInfo(const TEAM_DES& team) = 0;
		/*查找群后返回*/
		virtual void OnFindTeam(const unsigned long& ulTeamID,const char* strName,const char* strIntroduce, const unsigned short nAuthenType) = 0;
		/*查找完毕后返回*/
		virtual void OnFindTeamEnd() = 0;

		/* 群组离线功能回调 */
		//************************************
		// 说明:    开始发送群组数据
		// 参数:	ptNDSFileInfo	离线文件结构
		// 参数:	ulSerialNO		文件序列每次启动从0开始，每上传一个文件+1
		// 参数:	ulError			错误代码ERR_NO表示无错
		// 返回值:  void
		//************************************
		virtual void OnSendTeamData(const PTNDSFileInfo& ptNDSFileInfo, const unsigned long& ulSerialNO, const unsigned long& ulError) = 0;
		virtual void OnSendTeamDataEnd(const unsigned long& ulFileID) = 0;

		virtual void OnDownloadTeamData(const unsigned long& ulSerialNO, const PTNDSFileInfo& ptNDSFileINfo, const unsigned long& ulError) = 0;
		virtual void OnDownloadTeamDataEnd(const unsigned long& ulFileID) = 0;

		//************************************
		// 说明:    群数据上传完成，可以开始下载的通知
		// 参数:	ulTeamID		群ID
		// 参数:	ptNDSFileInfo	离线文件结构
		// 返回值:  void
		//************************************
		virtual void OnReadyDownloadTeamData(const unsigned long& ulTeamID, const PTNDSFileInfo& ptNDSFileInfo) = 0;

		//************************************
		// 说明:    取得的群共享文件列表
		// 参数:	ulTeamID		群ID
		// 参数:	ptNDSFileInfo	离线文件结构
		// 返回值:  void
		//************************************
		virtual void OnGetTeamShareFile(const unsigned long& ulTeamID, const PTNDSFileInfo& ptNDSFileInfo) = 0;
		virtual void OnGetTeamShareFileEnd(const unsigned long& ulTeamID) = 0;
		
		//************************************
		// 说明:    删除群共享文件
		// 参数:	tNDSFileInfo	离线文件结构
		// 参数:	usErrCode		错误代码ERR_NO表示无错
		// 返回值:  void
		virtual	void OnDelTeamShareFile(const TNDSFileInfo& tNDSFileInfo,const unsigned short& usErrCode) = 0;
	};

	// UCC7-IM群组功能实现插件
	class IPlugin_U7_IM_TeamGroup : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_IM_TeamGroup(){};

	public:
		virtual void AddObserver(IPlugin_U7_IM_TeamGroup_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_IM_TeamGroup_Observer* pObserver) = 0;

		//////////////////////////////////////////////////////////////////////////
		// 讨论组

		// 从服务器上下载讨论组列表
		virtual void LoadDiscussGroupList(void) = 0;

		// 获取讨论组列表（必须在LoadDiscussGroupList结束之后调用才能得到完整的列表）
		virtual const IMS_GROUP_MAP& GetGroupList(void) = 0;

		// 获取某个讨论组的结构体（必须在LoadDiscussGroupList结束之后调用才能得到正确得到）
		virtual const PIMS_GROUP_INFO GetGroupItem(unsigned long ulGroupID) = 0;

		// 创建讨论组
		virtual void CreateDiscussGroup(const char* strName, const std::list<std::string>& lstMember) = 0;

		// 添加讨论组成员
		virtual void AddDiscussGroupMember(unsigned long ulDiscussGroupID, const std::list<std::string>& lstMember) = 0;

		// 退出讨论组
		virtual void ExitDiscussGroup(unsigned long ulDiscussGroupID) = 0;

		// 发送讨论组消息
		virtual void SendGroupMessage(unsigned long ulDiscussGroupID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		// 修改讨论组名称
		virtual void ModDiscussGroupName(unsigned long ulDiscussGroupID, const char* strName) = 0;

		//////////////////////////////////////////////////////////////////////////
		// 群

		// 从服务器上下载群列表
		virtual void LoadTeamList(void) = 0;

		// 获取群列表（必须在LoadTeamList结束之后调用才能得到完整的列表）
		virtual const IMS_TEAM_MAP& GetTeamList(void) = 0;

		// 获取某个群的结构体（必须在LoadTeamList结束之后调用才能得到正确得到）
		virtual const PTEAM_DES GetTeamItem(unsigned long ulTeamID) = 0;

		// 发送群消息
		virtual void SendTeamMessage(unsigned long ulTeamID, const char* strMsgTime, const char* strMessage, const char* strMsgFormat) = 0;

		//************************************
		// 说明:    创建群
		// 参数:	strTeamName		群名称
		// 参数:	strFounder		创建者ID
		// 参数:	nTeamType		群类型（暂不使用）
		// 参数:	strIntroduce	群介绍
		// 参数:	strNotice		群公告
		// 参数:	nAuthenType		群设置1:不需要认证 2:需要认证 3:禁止主动加入
		// 参数:	mapMember		群成员列表,key成员ID；value成员等级：
		//							1:普通人员	2:管理员	3:创建者
		// 返回值:  void
		//************************************
		virtual void CreateTeam(const char* strTeamName, const char* strFounder, unsigned int nTeamType,
			const char* strIntroduce, const char* strNotice, unsigned int nAuthenType,
			const std::map<std::string,unsigned short>& mapMember) = 0;

		//************************************
		// 说明:    处理群请求
		// 参数:	ulTeamID		群ID
		// 参数:	strTeamName		群名称
		// 参数:	strSenderID		发送者ID
		// 参数:	strReceiverID	接收者ID（可忽略此参数，通常情况下此参数不会被使用）
		// 参数:	nResult			受理结果 1.成功 2.失败
		// 参数:	strDesc			验证信息
		// 参数:	nMsgType		请求类型 在KIMSDefine.h中 _TEAM_REQUEST 定义
		//							msgInvite = 1	邀请
		//							msgApply = 2	申请
		// 返回值:  void
		//************************************
		virtual void ProcessTeamRequest(unsigned long ulTeamID, const char* strTeamName, const char* strSenderID,
			const char* strReceiverID, unsigned short nResult, const char* strDesc, unsigned int nMsgType) = 0;

		// 添加群成员
		virtual void AddTeamMember(unsigned long ulTeamID, const std::list<std::string>& lstMember) = 0;

		//************************************
		// 说明:    申请加入群
		// 参数:	ulTeamID	群ID
		// 参数:	strDesc		验证信息
		// 返回值:  void
		//************************************
		virtual void JoinTeam(unsigned long ulTeamID, const char* strDesc) = 0;

		// 获取待验证信息
		virtual void GetTeamUnAuthenMsg(void) = 0;

		// 获取验证完毕消息
		virtual void GetTeamAuthenedMsg(void) = 0;

		// 退出群
		virtual void ExitTeam(unsigned long ulTeamID) = 0;

		// 踢出群
		virtual void KickTeam(unsigned long ulTeamID, const char* strMemberID) = 0;

		// 解散群
		virtual void DissolveTeam(unsigned long ulTeamID) = 0;

		// 晋升为管理员
		virtual void SetTeamAdmin(unsigned long ulTeamID, const char* strMemberID) = 0;

		// 降级为普通成员
		virtual void SetTeamMember(unsigned long ulTeamID, const char* strMemberID) = 0;

		//************************************
		// 说明:    更新群信息，即修改群设置
		// 参数:	ulTeamID	群ID
		// 其余参数参见CreateTeam
		//************************************
		virtual void UpdateTeam(unsigned long ulTeamID, const char* strTeamName, const char* strFounder,
			unsigned int nTeamType, const char* strIntroduce, const char* strNotice, 
			unsigned int nAuthenType, const std::map<std::string,unsigned short>& mapMember) = 0;

		// 群查询
		virtual void FindTeam(unsigned long ulTeamID, const char* strTeamName, const char* strIntroduce) = 0;

		//************************************
		// 说明:    向群发送图片或文件
		// 参数:	ulTeamID		群ID
		// 参数:	strPicPath		图片路径
		// 参数:	nType	群文件类型：在KXNCP.h文件中 _IMS_TEAMTYPE
		//			IMS_TEAM_PIC	群图片
		//			IMS_TEAM_FILE	群文件
		// 返回值:  void
		//************************************
		virtual void SendTeamData(unsigned long ulTeamID, const char* strPicPath, int nType) = 0;

		// 获取群文件数据
		virtual void GetTeamData(unsigned long ulTeamID) = 0;

		//************************************
		// 说明:    接收群组数据（图片或共享文件）
		// 参数:	ptNDSFileInfo	离线文件结构
		// 参数:	strFilePath		保存文件路径
		// 参数:	nType			IMS_TEAM_PIC	群图片
		//							IMS_TEAM_FILE	群文件
		// 返回值:  void
		//************************************
		virtual void RecvTeamData(const PTNDSFileInfo& ptNDSFileInfo, const std::string& strFilePath, const int nType) = 0;

		//************************************
		// 说明:    删除群组数据（图片或共享文件）
		// 参数:	ptNDSFileInfo	离线文件结构
		// 返回值:  void
		//************************************
		virtual	void DelTeamShareFile(const PTNDSFileInfo& ptNDSFileInfo) = 0;
	};
}
