#pragma once

//=======================================================================================
// 类型定义
//=======================================================================================
// 用户基本信息结构
typedef struct _IMS_USER_ITEM_BASE
{
	std::string orgid;			// 机构ID
	std::string departid;		// 部门ID
	std::string userid;			// 帐户ID
	std::string username;		// 帐户名称
	std::string nodeid;			// 节点
	std::string status;			// 状态
	std::string perlevel;		// 权限级别
	_IMS_USER_ITEM_BASE()
	{
		orgid		= "";
		departid	= "";
		userid		= "";
		username	= "";
		nodeid		= "";
		status		= "offline";
		perlevel	= "";
	};
	_IMS_USER_ITEM_BASE& operator=(const _IMS_USER_ITEM_BASE& ui)
	{
		orgid			= ui.orgid;
		departid		= ui.departid;
		userid			= ui.userid;
		username		= ui.username;
		nodeid			= ui.nodeid;
		status			= ui.status;
		perlevel		= ui.perlevel;

		return *this;
	}
}IMS_USER_ITEM_BASE,*PIMS_USER_ITEM_BASE;
typedef std::map<std::string,PIMS_USER_ITEM_BASE> IMS_USER_ITEM_BASE_MAP;

//-------------------------------------------
// 用户常规信息结构
typedef struct _IMS_USER_ITEM
{
	std::string		orgid;			// 机构ID
	std::string		departid;		// 部门ID
	std::string		userid;			// 帐号ID
	std::string		username;		// 帐号名称
	std::string		nickname;		// 账户昵称
	int				userlevel;		// 用户级别
	std::string		usertype;		// 用户类型 普通用户normal 会控 console 呼叫中心 callcenter
	unsigned short	termtype;		// 终端类型
	std::string		termname;		// 终端名称
	std::string		gender;			// 性别
	std::string		age;			// 年龄
	std::string		country;		// 国家
	std::string		province;		// 省份及州
	std::string		city;			// 城市
	std::string		address;		// 通讯地址
	std::string		postcode;		// 邮编
	std::string		email;			// 电邮
	std::string		smscode;		// 个人短信号
	std::string		voipcode;		// voip帐号己密码
	std::string		voippassword;	// voippassword
	unsigned char   authtype;
	unsigned int	faxcode;		// 传真分机号
	std::string		userinfo;		// 用户信息格式字符串
	std::string		show;			// public=公开资料, protected=仅联系人可见, private=不公开
	std::string		status;			// 状态
	unsigned int	orderid;		// orderid
	std::string     nodeid;         // nodeid

	_IMS_USER_ITEM()
	{
		orgid			= "";
		departid		= "";
		userid			= "";
		username		= "";
		nickname		= "";
		userlevel		= 0;
		usertype		= "normal";
		termtype		= 0;
		termname		= "";
		gender			= "";
		age				= "";
		country			= "";
		province		= "";
		city			= "";
		address			= "";
		postcode		= "";
		email			= "";
		smscode			= "";
		voipcode		= "";
		voippassword	= "";
        authtype        = 0;
		faxcode			= 0;
		userinfo		= "";
		show			= "private";
		status			= "offline";
		orderid			= 0;
		nodeid          = "";
	}

	_IMS_USER_ITEM& operator=(const _IMS_USER_ITEM& ui)
	{
		orgid			= ui.orgid;
		departid		= ui.departid;
		userid			= ui.userid;
		username		= ui.username;
		nickname		= ui.nickname;
		userlevel		= ui.userlevel;
		usertype		= ui.usertype;
		termtype		= ui.termtype;
		termname		= ui.termname;
		gender			= ui.gender;
		age				= ui.age;
		country			= ui.country;
		province		= ui.province;
		city			= ui.city;
		address			= ui.address;
		postcode		= ui.postcode;
		email			= ui.email;
		smscode			= ui.smscode;
		voipcode		= ui.voipcode;
		voippassword	= ui.voippassword;
        authtype        = ui.authtype;
		faxcode			= ui.faxcode;
		userinfo		= ui.userinfo;
		show			= ui.show;
		status			= ui.status;
		orderid			= ui.orderid;
		nodeid          = ui.nodeid;

		return *this;
	}

}IMS_USER_ITEM, *PIMS_USER_ITEM;
typedef std::map<std::string, PIMS_USER_ITEM> IMS_USER_ITEM_MAP;


//-------------------------------------------
//用户运行信息结构
typedef struct _USER_SESSION
{
	std::string		userid;
	unsigned short	termtype;
	std::string		termname;
	std::string		svrid;
	std::string		svraddr;
	unsigned short  svrport;
	std::string		svrtype;
	std::string		nodeid;
	std::string		nataddr;
	std::string		nattype;
	std::string		localaddr;
	unsigned short	localport;
	std::string		macaddr;
	std::string		status;

	_USER_SESSION()
	{
		userid		= "";
		termtype	= 0;
		termname	= "";
		svrid		= "";
		svraddr		= "";
		svrport		= 0;
		svrtype		= "";
		nodeid		= "";
		nataddr		= "";
		nattype		= "";
		localaddr	= "";
		localport	= 0;
		macaddr		= "";
		status		= "offline";
	}

	_USER_SESSION& operator=(const _USER_SESSION& us)
	{
		userid		= us.userid;
		termtype	= us.termtype;
		termname	= us.termname;
		svrid		= us.svrid;
		svraddr		= us.svraddr;
		svrport		= us.svrport;
		svrtype		= us.svrtype;
		nodeid		= us.nodeid;
		nataddr		= us.nataddr;
		nattype		= us.nattype;
		localaddr	= us.localaddr;
		localport	= us.localport;
		macaddr		= us.macaddr;
		status		= us.status;

		return *this;
	}

}USER_SESSION, *PUSER_SESSION;
typedef std::list<PUSER_SESSION> USER_SESSION_LIST;

//-------------------------------------------
// 联系人信息结构(记载联系人的地址资料)
typedef struct _IMS_CONTACT_ITEM
{

	std::string group;			// 联系人所属组
	std::string initiator;		// 添加联系人的发起人
	std::string acceptflag;		// 接收标志: true=已是联系人 false=还未成为联系人

	std::string departname;		//部门名称
	std::string orgname;		//机构名称
	std::string strUserTypeID;  // 用户登录类型('normal','ocx','TV','channel','remote','record','console')

	IMS_USER_ITEM		userItem;
	USER_SESSION		userSession;

	_IMS_CONTACT_ITEM()
	{
		group		  = "";
		initiator	  = "";
		acceptflag	  = "false";

		departname    = "";
		orgname		  = "";
		strUserTypeID = "";
	}

	_IMS_CONTACT_ITEM& operator=(const _IMS_CONTACT_ITEM& ci)
	{
		group			= ci.group;
		initiator		= ci.initiator;
		acceptflag		= ci.acceptflag;
		departname  	= ci.departname;
		orgname			= ci.orgname;
		strUserTypeID	= ci.strUserTypeID;
		userItem		= ci.userItem;
		userSession 	= ci.userSession;

		return *this;
	}
}IMS_CONTACT_ITEM,*PIMS_CONTACT_ITEM;
typedef std::map<std::string,PIMS_CONTACT_ITEM> IMS_CONTACT_ITEM_MAP;

typedef enum MSG_TYPE
{
    MSG_PERSON = 0,
    MSG_ORG,
    MSG_TEAM,
    MSG_DISCUSS,
};

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
// 离线消息信息结构(离线消息格式)
typedef struct _XOFLMSG : public _XMSG
{
    unsigned int msgType;
    std::string nodeID;
    std::string orgID;
    std::string orgName;
    std::string departID;
    std::string departName;
    std::string userName;
    std::string teamID;
    std::string discussID;
    std::string discussName;

    _XOFLMSG()
        :msgType(MSG_PERSON)
    {
    };
}XOFLMSG,*PXOFLMSG;

//-------------------------------------------
// 设备
typedef enum IMS_DEVICE
{
	IMS_DEVICE_CAMERA	  = 0,
	IMS_DEVICE_MICROPHONE = 1,
};

typedef enum ENUM_OPERATION
{
	ONLYREAD = 0,	//仅取数据
	ONLYWRITE,		//仅订阅
	READANDWRITE,	//即取数据又订阅
}OPERATION;
//-------------------------------------------
//监控分区
//区域信息
typedef struct _MONGROUP
{
	std::string		groupid;
	std::string		groupname;
	std::string		parentid;
	unsigned int	nLevelID;
    unsigned int    nOrderID;
	_MONGROUP()
	{
		groupid="";
		groupname="";
		parentid="";
		nLevelID=0;
        nOrderID=0;
	};
}MONGROUP,*PMONGROUP;

//-------------------------------------------
// 设备信息
typedef struct _MONDEV
{
	std::string		devid;
    std::string     devname;
	std::string     devpassword;
	unsigned long	devtype;
	std::string		parentid;
	std::string 	nodeid;
	std::string 	mcuid;
	std::string 	mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short	localport;
	std::string     strOrderID;
	std::string		strbindmemberid;
    unsigned short  status;
	unsigned short	usTermtype;
	std::string		strTermname;
    std::string     strMGSID;

	_MONDEV()
	{
		devid="";
        devname="";
		devtype=0;
		parentid="";
		nodeid="";
		mcuid="";
		mcuaddr="";
		mcuport=0;
		nataddr="";
		localaddr="";
		localport=0;
		strOrderID="";
		strbindmemberid="";	
        status=0;
		usTermtype=0;
		strTermname="";	
	};
}MONDEV,*PMONDEV;

//-------------------------------------------
// 通道信息
typedef struct _MONCHANNEL
{
	std::string channelid;
	std::string channelname;
	std::string devid;
	std::string devinfo;
	unsigned long videocid;
	unsigned long audiocid;
	unsigned short status;
	unsigned char controlflag;
	unsigned short videocodetype;
	_MONCHANNEL()
	{
		videocid=0;
		audiocid=0;
		status=0;
		controlflag=0;
		videocodetype=0;
	}
}MONCHANNEL,*PMONCHANNEL;

typedef struct _VQD_CHANINFO
{
	std::string		channelid;
	std::string		channelname;
	std::string		devid;
	unsigned long	videocid;
	unsigned long	audiocid;
	unsigned short	status;

	std::string     devname;
	std::string 	nodeid;
	std::string 	mcuid;
	std::string 	mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short	localport;
	std::string     strlevel;//诊断项目的阀值设置多项
	std::string     strData1;
	std::string		strData2;
	_VQD_CHANINFO()
	{
		channelid	= "";
		videocid	= 0;
		audiocid	= 0;
		status	    = 0;	
		strlevel	= "";
		strData1	= "";
		strData2	= "";
	}
}VQD_CHANINFO,*pVQD_CHANINFO;

typedef std::list<pVQD_CHANINFO> VQD_CHANNEL_LST;

typedef struct _ALARMLOG_INFO 
{
	unsigned int	unID;
	std::string		strDevID;
	std::string		strDevName;
	std::string		strChanID;
	std::string		strChanName;
	unsigned short	usChanNo;
	unsigned int	unChanType;
	std::string		strChanTypeName;
	std::string		strAlarmType;
	std::string		strAlarmTypeName;
	unsigned int	unAlarmLevelID;
	std::string		strAlarmTime;
	std::string		strAlarmDesc;
	std::string		strOperateTime;
	unsigned int	unOperateStatus;
	std::string		strAlarmUser;
	std::string		strResultDesc;

	_ALARMLOG_INFO()
	{
		unID			= 0;
		strDevID		= "";
		strDevName		= "";
		strChanID		= "";
		strChanName		= "";
		usChanNo		= 0;
		unChanType		= 0;
		strAlarmType	= "";
		unAlarmLevelID	= 0;
		strAlarmTime	= "";
		strAlarmDesc	= "";
		strOperateTime	= "";
		unOperateStatus	= 0;
		strAlarmUser	= "";
		strResultDesc	= "";
	}
}ALARMLOG_INFO, *PALARMLOG_INFO;

typedef std::list<ALARMLOG_INFO> ALARMLOG_INFO_LST;
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
		ulID = 0;
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

// 群组图片,继承NDS(离线文件)
#ifndef NDS_TEAM__
#define NDS_TEAM__
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

typedef struct _DEPARTINFO
{
	std::string	   strDepartID;
	std::string	   strDepartName;

	_DEPARTINFO()
	{
		strDepartID	= "";
		strDepartName  = "";
	}
}DEPARTINFO,*PDEPARTINFO;

typedef std::list<PDEPARTINFO> DEPARTINFOLIST;

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


/************************************************************************
* 监控会议子系统
************************************************************************/
typedef struct _SUBSYSTEM_DOMAININFO
{
	std::string domain;				//域名
	std::string domainname;         //域名称
	std::string dtype;				//域类型 parent=父域， child=子域， NULL=独立域
	std::string parent;				//父域名
	std::string svraddr;			//域地址

	unsigned short status;			//域状态 status=1在线， status=0不在线
}SUBSYSTEM_DOMAININFO, *PSUBSYSTEM_DOMAININFO;
typedef std::map<std::string, SUBSYSTEM_DOMAININFO> MAP_SUBSYSTEM_DOMAININFO;
typedef std::map<std::string, SUBSYSTEM_DOMAININFO>::iterator MAP_SUBSYSTEM_DOMAININFO_IT;


typedef struct _SUBSYSTEM_MCUINFO
{
	std::string domain;				//MCU所在域
	std::string mcuid;				//MCUID
	std::string mtype;				//MCU的类型
	std::string version;			//MCU版本
	std::string mcuaddr;			//MCU IP地址

	unsigned long activity;			//MCU激活数
	unsigned short status;			//MCU的状态 status=1在线， status=0不在线
}SUBSYSTEM_MCUINFO, *PSUBSYSTEM_MCUINFO;
typedef std::map<std::string, SUBSYSTEM_MCUINFO> MAP_SUBSYSTEM_MCUINFO;
typedef std::map<std::string, SUBSYSTEM_MCUINFO>::iterator MAP_SUBSYSTEM_MCUINFO_IT;

typedef struct _SUBSYSTEM_DEVICEINFO
{
	std::string devid;				//设备ID
	std::string devname;			//设备名字
	std::string bindmemberid;		//绑定设备的人员
	
	std::string localaddr;			//设备地址
	std::string nataddr;			//设备映射地址
	
	std::string mcuid;				//设备登录的MCUID
	std::string mcuaddr;			//设备登录的MCU地址
	std::string mgsid;              //网关ID
	
	unsigned short devicetype;		//设备类型
	unsigned short localprot;		//设备端口
	unsigned short mcuport;			//设备登录的MCU端口
	unsigned short channelnum;		//设备通道数
	unsigned short status;			//设备状态 status=1在线， status=0不在线
}SUBSYSTEM_DEVICEINFO, *PSUBSYSTEM_DEVICEINFO;
typedef std::map<std::string, SUBSYSTEM_DEVICEINFO> MAP_SUBSYSTEM_DEVICEINFO;
typedef std::map<std::string, SUBSYSTEM_DEVICEINFO>::iterator MAP_SUBSYSTEM_DEVICEINFO_IT;

//-------------------------------------
// 机构搜索类型定义
//-------------------------------------
typedef enum _IMS_SEARCH_TYPE
{
	IMS_SEARCH_AND = 0,
	IMS_SEARCH_ID,
	IMS_SEARCH_NAME,
	IMS_SEARCH_OR,
}IMS_SEARCH_TYPE;