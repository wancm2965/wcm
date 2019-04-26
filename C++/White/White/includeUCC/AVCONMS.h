#pragma once

//代理类型
typedef enum{
	PT_NONE=0,			//无代理，直接连接
	PT_HTTP_PROXY,		//HTTP代理
	PT_SOCK5,			//SOCK5代理
	PT_HTTP_TUNNEL		//HTTP隧道
}PROXY_TYPE;

//传输协议类型
typedef enum{
	PT_TCP=0,			//TCP传输协议
	PT_UDP,				//UDP传输协议
	PT_RTP,				//RTP传输协议
}PROTOCOL_TYPE;

//会议类型
typedef enum{
	ROOM_TYPE_WORKGROUP=0,		//工作组会议
	ROOM_TYPE_NORMAL,			//标准会议
	ROOM_TYPE_ADVANCE,			//高级会议
}ROOM_TYPE;

//会议成员类型
typedef enum{
	MEMBER_TYPE_ATTENDEE=0,		//与会者
	MEMBER_TYPE_PRESIDER,		//主持人
}MEMBER_TYPE;

//登录返回错误码类型
typedef enum{
	LOGIN_SUCCESS=0,			//0: 成功登录会议室
	LOGIN_PWD_ERROR,			//1: 密码验证失败
	LOGIN_ALREADY_LOGIN,		//2: 用户已经进入会议
	LOGIN_MORETHAN_SIX,			//3: 会议已经超过六人限制
	LOGIN_NOFEAT_SVR,			//4: 会议没有适当的服务器
	LOGIN_OWNER_LEFT,			//5: 会议所有者离开会议
	LOGIN_MEM_TYPE_ERROR,		//6: 登录者类型不明确
	LOGIN_ROOM_LOCKED,			//7: 会议已经锁定
	LOGIN_ROOM_TYPE_ERROR,		//8: 会议类型不明确
	LOGIN_SVR_ERROR=99,			//99:服务内部发生错误
}LOGIN_ERRCODE_TYPE;

//退出会议返回错误码类型
typedef enum{
	LOGOUT_SUCCESS=0,			//0: 表示正常退出会议室
	LOGOUT_OWNER_LEFT,			//1: 表示所有者退出会议室
	LOGOUT_KICKOUT,				//2: 表示被踢出会议室
}LOGOUT_ERRCODE_TYPE;


//=======================================================================================
// IAVCONMS 类定义
//=======================================================================================
class AFX_EXT_CLASS IAVCONMS
{
public:
	IAVCONMS(void){};
	virtual ~IAVCONMS(void){};

public:
	/* 网络连接 */
	virtual bool Connect(const std::string& strServerAddr,	//服务器IP地址(接受域名地址)
                         unsigned short usServerPort		//服务器端口
						 )=0;

	/* 断开连接 */
	virtual void Disconnect(void)=0;

	/* 设置网络通讯协议类型 */
	virtual void SetSessionProtocolType(PROTOCOL_TYPE type)=0;

	/* 设置音频传输协议类型 */
	virtual void SetAudioProtocolType(PROTOCOL_TYPE type)=0;

	/* 设置视频传输协议类型 */
	virtual void SetVideoProtocolType(PROTOCOL_TYPE type)=0;

	/* 设置代理类型 */
	virtual void SetProxyType(PROXY_TYPE pt)=0;

	/* 设置代理用户 */
	virtual void SetProxyUser(const char* cszUser)=0;

	/* 设置代理密码 */
	virtual void SetProxyPassword(const char* cszPassword)=0;

	/* 设置代理服务器 */
	virtual void SetProxyHost(const char* cszHost)=0;
	virtual void SetProxyPort(unsigned short usPort)=0;

	/* 在会议列表中创建会议室 */
	virtual void InsertRoom(const std::string& strRoomName,		//会议室创建者自定义的会议名称
							ROOM_TYPE rtRoomType,				//会议类型
							const std::string& strPassword,		//会议密码，会议成员进入会议时需要验证身份
							const std::string& strDesc			//会议描述信息
							)=0;

	/* 在会议列表中删除会议室 */
	virtual void DeleteRoom(const std::string& strRoomID)=0;

	/* 在会议列表中添加会议组 */
	virtual void InsertGroup(	const std::string& strRoomID,			//会议室ID
								const std::string& strParentGroupID,	//父节点组ID
								const std::string& strGroupName			//会议组名称
								)=0;

	/* 在会议列表中添加会议成员 */
	virtual void InsertMember(	const std::string& strRoomID,			//会议室ID
								const std::string& strGroupID,			//会议组ID，如果为空，则将成员添加到根组
								const std::string& strMemberID,			//会议成员ID
								const std::string& strMemberName,		//会议成员名称
								MEMBER_TYPE mtMemberType				//成员类型
								)=0;

	/* 登录会议室 */
	virtual void Login(	const std::string& strRoomID,					//会议室ID
						const std::string& strPassword,					//会议密码
						const std::string& strMemberID,					//会议成员ID
						const std::string& strMemberName,				//会议成员名称
						const std::string& strInviteeGroupID,			//如果为被邀请者，则需要确定其组ID
						MEMBER_TYPE mtMemberType=MEMBER_TYPE_ATTENDEE	//如果为被邀请者，则需要确定成员类型
						)=0;

	/* 退出会议室 */
	virtual void Logout(void)=0;
};


//=======================================================================================
// IAVCONMSEvent 事件类定义
//=======================================================================================
class AFX_EXT_CLASS IAVCONMSEvent
{
public:
	IAVCONMSEvent(void){};
	virtual ~IAVCONMSEvent(void){};

public:
	/* 连接服务器失败 */
	virtual void OnConnectFailed(void)=0;

	/* 已连接到服务器 */
	virtual void OnConnected(const std::string& strNatAddr)=0;

	/* 与服务器的网络连接已断开 */
	virtual void OnDisconnected(void)=0;

	/* 返回创建的会议室ID */
	virtual void OnRoomInsert(const std::string& strRoomID)=0;

	/* 返回被删除的会议室ID */
	virtual void OnRoomDelete(const std::string& strRoomID)=0;

	/* 返回创建的组ID及其会议室ID */
	virtual void OnGroupInsert(const std::string& strRoomID,const std::string& strGroupID)=0;

	/* 返回创建的成员ID及其会议室ID */
	virtual void OnMemberInsert(const std::string& strRoomID,const std::string& strMemberID)=0;

	/* 收到登录会议室事件 */
	virtual void OnLogin(LOGIN_ERRCODE_TYPE nErrCode)=0;

	/* 收到退出会议室事件 */
	virtual void OnLogout(LOGOUT_ERRCODE_TYPE nErrCode)=0;
};


//=======================================================================================
// IAVCONMS 实例
//=======================================================================================
AFX_EXT_CLASS IAVCONMS* CreateAVCONMSImpl(IAVCONMSEvent& rAVCONMSEvent);
AFX_EXT_CLASS void DestoryAVCONMSImpl(void);
AFX_EXT_CLASS IAVCONMS* GetAVCONMSImpl(void);