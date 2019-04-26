#pragma once

//=======================================================================================
// 类型定义
//=======================================================================================

//-------------------------------------------
// 联系人信息结构(记载联系人的地址资料)
typedef struct _IMS_CONTACT_ITEM
{
	std::string userid;			// 帐户ID
	std::string username;		// 帐户名称
	int         userlevel;		// 用户级别

	std::string gender;			// 性别
	std::string age;			// 年龄
	std::string country;		// 国家
	std::string province;		// 省份及州
	std::string city;			// 城市
	std::string address;		// 通讯地址
	std::string postcode;		// 邮编
	std::string show;			// public=公开资料 protected=仅联系人可见 private=不公开

	std::string group;			// 联系人所属组
	std::string initiator;		// 添加联系人的发起人
	std::string acceptflag;		// 接收标志: true=已是联系人 false=还未成为联系人

	std::string departname;		//部门名称
	std::string orgid;			//机构域名
	std::string orgname;		//机构名称

	std::string phone;			// 电话
	std::string email;			// 电邮
	std::string smscode;		// 个人短信帐号
	std::string voipcode;		// voip帐号及密码
	std::string voippasswd;		// B类voip密码
	unsigned char authtype;		// 验证类型
	unsigned int faxcode;		// 传真分机号
	std::string userinfo;		// 用户信息格式字符串

	std::string svrid;			// 服务器ID
	std::string svraddr;		// 服务器地址
	unsigned short svrport;		// 服务器端口
	std::string svrtype;		// 服务器类型

	std::string nodeid;			// 联系人NODEID
	std::string nataddr;		// NAT地址
	std::string localaddr;		// 本地地址
	unsigned short localport;	// 本地端口
	unsigned short termtype;	// 终端类型
	std::string termname;		// 终端名称
	std::string status;			// online offline busy away

	// add 2010/11/30
	int 	    nUserTypeID;    // 用户类型ID

	// add 2012/01/05
	unsigned int nOrderID;

	_IMS_CONTACT_ITEM()
	{
		userid		= "";
		username	= "";
		userlevel	= 0;

		gender		= "";
		age			= "";
		country		= "";
		province	= "";
		city		= "";
		address		= "";
		postcode	= "";
		show		= "private";

		group		= "";
		initiator	= "";
		acceptflag	= "false";

		departname  = "";
		orgid		= "";
		orgname		= "";

		phone		= "";
		email		= "";
		smscode		= "";
		voipcode	= "";
		voippasswd	= "";
		authtype	= 0;
		faxcode		= 0;
		userinfo	= "";

		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";

		nodeid		= "";
		nataddr		= "";
		localaddr	= "";
		localport	= 0;
		termtype	= 0;
		termname	= "";
		status		= "offline";

		nUserTypeID = 0;
		nOrderID     = 0;
	};
}IMS_CONTACT_ITEM,*PIMS_CONTACT_ITEM;

typedef std::map<std::string,PIMS_CONTACT_ITEM> IMS_CONTACT_ITEM_MAP;

//-------------------------------------------
// 机构用户信息结构
typedef struct _IMS_ORGUSER_ITEM
{
	std::string userid;			// 帐户ID
	std::string username;		// 帐户名称
	std::string orgid;			// 机构ID
	std::string departid;		// 部门ID
	std::string nodeid;			// 联系人NODEID
	std::string status;			// 状态: 0=离线 1=在线

	_IMS_ORGUSER_ITEM()
	{
		userid		= "";
		username	= "";
		orgid		= "";
		departid	= "";
		nodeid		= "";
		status		= "offline";
	};
}IMS_ORGUSER_ITEM,*PIMS_ORGUSER_ITEM;

typedef std::map<std::string,PIMS_ORGUSER_ITEM> IMS_ORGUSER_ITEM_MAP;

//-------------------------------------------
// 即时消息信息结构(及时消息格式)
typedef struct _XMSG
{
	std::string type;		// 保留,不使用
	std::string head;
	std::string body;
	std::string format;
	std::string datetime;
	std::string attachment;
}XMSG,*PXMSG;

//-------------------------------------------
// 设备
typedef enum IMS_DEVICE
{
	IMS_DEVICE_CAMERA	  = 0,
	IMS_DEVICE_MICROPHONE = 1,
};

//-------------------------------------------
//监控分区
//区域（设备）信息
typedef struct _MONGROUP
{
	unsigned long	groupid;
	std::string		groupname;
	std::string		devid;
	unsigned long	devtype;
	unsigned long	parentid;
	_MONGROUP()
	{
		groupid=0;
		parentid=0;
		devtype=0;
	};
}MONGROUP,*PMONGROUP;

//-------------------------------------------
//通道信息
typedef struct _MONCHANNEL
{
	std::string channelid;
	std::string channelname;
	std::string devid;
	std::string devinfo;
	std::string strbindmemberid;
	std::string nodeid;
	unsigned long groupid;
	std::string mcuid;
	std::string mcuaddr;
	unsigned short mcuport;
	std::string nataddr;
	std::string localaddr;
	unsigned short localport;
	unsigned short termtype;
	std::string termname;
	unsigned long videocid;
	unsigned long audiocid;
	unsigned short status;
	unsigned char controlflag;
	_MONCHANNEL()
	{
		groupid=0;
		mcuport=0;
		localport=0;
		termtype=0;
		videocid=0;
		audiocid=0;
		status=0;
		controlflag=0;
	}
}MONCHANNEL,*PMONCHANNEL;

//-----------------------------------------------
//电子地图
typedef struct _MAP_BZ_ITEM
{
	std::string strGroupID;  //区域ID
	std::string strTreeID; //通道ID
	std::string strBzID;   //热点ID
}MAP_BZ_ITEM,*PMAP_BZ_ITEM;

typedef std::list<PMAP_BZ_ITEM> MAP_BZ_LST;

//-----------------------------------------------
// AVC呼叫项
typedef struct tagAVC_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}AVC_CALL_ITEM,*PAVC_CALL_ITEM;

//-----------------------------------------------
// AVC呼叫项MAP
typedef std::map<std::string,PAVC_CALL_ITEM> AVC_CALL_ITEM_MAP;

//-----------------------------------------------
// 白板通讯信息结构
typedef struct tagWHB_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}WHB_CALL_ITEM,*PWHB_CALL_ITEM;

//-----------------------------------------------
typedef std::map<std::string,PWHB_CALL_ITEM> WHB_CALL_ITEM_MAP;

//-----------------------------------------------
// SRV屏幕视频通讯信息结构
typedef struct tagSRV_CALL_ITEM
{
	std::string nodeid;
	int			status;
	bool		iscaller;
}SRV_CALL_ITEM,*PSRV_CALL_ITEM;

//-----------------------------------------------
// SRV MAP
typedef std::map<std::string,PSRV_CALL_ITEM> SRV_CALL_ITEM_MAP;

//讨论组结构
typedef struct _DIS_ITEM
{
	unsigned long ulID;  
	std::string strMember; 
	std::string strName;   
}DIS_ITEM,*PDIS_ITEM;

//////////////////////////////////////////////////////////////////////////
// 与会议室相关的数据结构

//-----------------------------------------------
// 用户类型
typedef struct _IMS_USERTYPE_ITEM
{
	int				nUserTypeID;            // 用户类型id
	std::string		strTypeName;            // 用户类型名称
	int				nOrderID;               // 用户级别
	std::string		strRoomType;            // 可创建的会议室类型
	std::string		strIMRight;		        // 用户权限
	std::string		strSeeUserType;         // 用户的可见人员类型
}IMS_USERTYPE_ITEM, *PIMS_USERTYPE_ITEM;

typedef std::list<PIMS_USERTYPE_ITEM> IMS_USERTYPE_ITEM_LST;

//-------------------------------------
// 点播服务器
typedef struct _IMS_VOD_SVR
{
	std::string			strIP;				// 点播服务器 IP 地址
	std::string			strName;			// 点播服务器名称
} IMS_VOD_SVR,*PIMS_VOD_SVR;

typedef std::list<PIMS_VOD_SVR> IMS_VOD_SVR_LST;
//-------------------------------------
//群组信息结构
typedef struct _TEAM_DES
{
	unsigned long ulID;                              //群ID
	std::string strTeamName;                         //群名称
	std::string strFounder;                          //创建者
	size_t TeamType;                                 //群类型（暂不使用）
	std::string strIntroduce;                        //群介绍
	std::string strNotice;                           //群公告
	size_t AuthType;                                 //群设置1:不需要认证 2:需要认证 3:禁止主动加入
	std::map<std::string,unsigned short> mapMember;  //群成员列表,key成员名称；value成员等级
	//1:普通人员
	_TEAM_DES()                                      //2:管理员
	{                                                //3:创建者
		TeamType = 0;
		AuthType  = 2;
	}
	bool operator == (const _TEAM_DES& a) const
	{
		return this->strTeamName==a.strTeamName&&this->TeamType==a.TeamType&&this->strIntroduce==a.strIntroduce&&this->strNotice==a.strNotice&&this->AuthType==a.AuthType;
	}
	bool operator != (const _TEAM_DES& a) const
	{
		return this->strTeamName!=a.strTeamName||this->TeamType!=a.TeamType||this->strIntroduce!=a.strIntroduce||this->strNotice!=a.strNotice||this->AuthType!=a.AuthType;
	}
}TEAM_DES,*PTEAM_DES;
//---------------------------------------
//群待受理消息结构
typedef struct _TEAM_REQUEST
{
	unsigned long ulID;               //群ID
	std::string strSender;            //发送者
	std::string strReceiver;          //接收者
	std::string strTeamName;          //群名称
	enum type                         
	{
		msgInvite = 1,                //邀请
		msgApply  = 2                 //申请
	};
	type MsgType;                     //消息类型
	std::string strDesc;              //自我描述
}TEAM_REQUEST,*PTEAM_REQUEST;
//群受理消息结构
typedef struct _TEAM_HANDLE_ACCEPT
{
	unsigned long ulID;               //群ID
	std::string strHandleMember;      //受理人
	std::string strFromID;            //请求来源
	short result ;                    //受理结果 1.同意 2.不同意
	std::string strTeamName;          //群名称
	enum type                         
	{
		msgInvite = 1,                //邀请
		msgApply  = 2                 //申请
	};
	type MsgType;                     //消息类型
}TEAM_HANDLE_ACCEPT,*PTEAM_HANDLE_ACCEPT;
//群受理完毕消息消息返回结构
typedef struct _TEAM_REQUEST_RESULT
{
	unsigned long ulID;               //群ID
	std::string strSendTo;            //消息接收者
	std::string strHandleMember;      //受理人
	std::string strBeHandleMember;    //被受理人
	short result ;                    //受理结果 1.成功 2.失败
	std::string strTeamName;          //群名称
	enum type                         
	{
		msgInvite    = 1,                //邀请
		msgApply     = 2,                //申请
		msgExit      = 3,                //退出
		msgKick      = 4,                //踢出
		msgDissolve  = 5,                //解散
		msgPromotion = 6,                //晋升
		msgDemote    = 7                 //降级
	};
	type MsgType;                     //消息类型
}TEAM_REQUEST_RESULT,*PTEAM_REQUEST_RESULT;

//---------------------------------------------------------
// 用户SESSION定义
typedef struct _USERSESSION
{
	std::string userid;
	std::string username;
	std::string svrid;
	std::string svraddr;
	unsigned short svrport;
	std::string svrtype;
	std::string nodeid;
	std::string nataddr;
	std::string localaddr;
	unsigned short localport;
	unsigned short termtype;
	std::string termname;
	std::string status;
	std::string macaddr;
	std::string phone;
	std::string email;
	std::string smscode;
	std::string voipcode;
	unsigned int faxcode;
	std::string userinfo;
	_USERSESSION()
	{
		userid		= "";
		username	= "";
		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";
		nodeid		= "";
		nataddr		= "";
		localaddr	= "";
		localport	= 0;
		termtype	= 0;
		termname	= "";
		status		= "offline";
		macaddr		= "";
		phone		= "";
		email		= "";
		smscode		= "";
		voipcode	= "";
		faxcode		= 0;
		userinfo	= "";
	};
}USERSESSION,*PUSERSESSION;

// 用户SESSION列表
typedef std::list<PUSERSESSION> USERSESSIONLIST;

// 群组图片,继承NDS(离线文件)
#ifdef NDS_TEAM__
#include "KNDSDefine.h"
#endif

//---------------------------------------------------------
//　域信息
typedef struct _DOMAININFO
{
	std::string		strDomain;
	std::string		strParentDomain;
	std::string		strOrgName;
	_DOMAININFO()
	{
		strDomain	= "";
		strParentDomain = "";
		strOrgName = "";
	}
}DOMAININFO,*PDOMAININFO;

//　域信息列表
typedef std::list<PDOMAININFO> DOMAININFOLIST;

typedef struct _ORGINFO
{
	std::string	   strOrgID;
	std::string	   strOrgName;

	_ORGINFO()
	{
		strOrgID	= "";
		strOrgName  = "";
	}
}ORGINFO,*PORGINFO;

typedef std::list<PORGINFO> ORGINFOLIST;

//取某域资源列表时间
typedef struct _DOMAINUPDATETIME
{
	std::string	   strDomain;
	std::string	   strTime;

	_DOMAINUPDATETIME()
	{
		strDomain	= "";
		strTime  = "";
	}
}DOMAINUPDATETIME,*PDOMAINUPDATETIME;

typedef std::list<DOMAINUPDATETIME> DOMAINUPDATETIMELST;