#pragma once

// MCU结构
typedef struct _MMS_MCU
{
	std::string		mcuid;
	std::string		mcutype;
	unsigned long	connections;
	std::string		bandwidth;
	unsigned long	activity;

	std::string		ipaddr;
	unsigned short	ipport;
	std::string		iptype;
	std::string		ipname;

	_MMS_MCU()
	{
		mcuid="";
		mcutype="";
		connections=0;
		activity=0;

		ipaddr="";
		ipport=0;
		iptype="";
		ipname="";
	};
}MMS_MCU,*PMMS_MCU;

// 机构信息结构
typedef struct tagORG_ITEM
{
	std::string orgid;			// 机构ID
	std::string orgname;		// 机构名称
	int			membernumber;   // 机构总人数
}ORG_ITEM,*PORG_ITEM;

//区域（设备）信息
typedef struct tagGroupInfo
{
	std::string		groupid;
	std::string		groupname;
	std::string		devid;
	unsigned long	devtype;
	std::string		parentid;
	unsigned long   onlinenum;
	unsigned long   totalnum;
	unsigned long   meetonlinenum;
	unsigned long   meettotalnum;
	tagGroupInfo()
	{
		onlinenum = 0;
		totalnum = 0;
		meetonlinenum = 0;
		meettotalnum = 0;
	}
}GROUPINFO,*PGROUPINFO;

typedef struct tagMonDomainUpdateTime
{
	std::string	   strDomain;
	std::string	   strTime;
	std::string	   strMemberID;
	bool           bNeedWrite;

	tagMonDomainUpdateTime()
	{
		strDomain = "";
		strTime = "";
		strMemberID = "";
		bNeedWrite = true;
	}
}MONDOMAINUPDATETIME,*PMONDOMAINUPDATETIME;

typedef enum
{
	CHN_ADD = 0,
	CHN_DEL,
	CHN_UPDATE
}CHANNEL_UPDATE_TYPE;

//通道信息
typedef struct _CHANNELINFO
{
	std::string channelid;
	std::string channelname;
	std::string devid;
	std::string devinfo;
	std::string strbindmemberid;
	std::string nodeid;
	std::string groupid;
	std::string mcuid;
	std::string mcuaddr;
	unsigned short mcuport;
	std::string nataddr;
	std::string localaddr;
	unsigned short localport;
	std::string termtype;
	unsigned long videocid;
	unsigned long audiocid;
	unsigned short status;
	unsigned char controlflag;
	bool bChangeStatus;
	_CHANNELINFO()
	{
		mcuport=0;
		localport=0;
		videocid=0;
		audiocid=0;
		status=0;
		controlflag=0;
		bChangeStatus=false;
	}
}CHANNELINFO,*PCHANNELINFO;

typedef std::map<unsigned long,GROUPINFO*>		MapMonGroupItem;
typedef std::map<std::string,CHANNELINFO*>		MapMonChannelItem;

typedef std::map<unsigned int, MapMonGroupItem>		MapMonDomainGroupItem;
typedef std::map<unsigned int, MapMonChannelItem>	MapMonDomainChannelItem;

// 监控区信息结构
typedef struct tagREG_ITEM
{
	unsigned long rid;
	std::string name;
	std::string type;
	int level;
	//std::string upgradeid;
	unsigned long pid;
	std::string desc;
}REG_ITEM,*PREG_ITEM;

//-----------------------------------------------
// 监控机信息结构
typedef struct tagMAC_ITEM
{
	unsigned long pid;
	unsigned long rid;
	std::string devid;
	std::string name;
	int			chnum;
	int			status;
}MAC_ITEM,*PMAC_ITEM;

//-------------------------------------------

// 临时会议项结构
typedef struct _MMS_ROOM_TEMP
{
	std::string		domain;						//会议室所在域
	std::string		roomid;						//会议室ID
	std::string		roomname;					//会议室名称
	std::string		groupid;					//组ID
	std::string		groupname;					//组名称
	unsigned short	roomtype;					//会议室类型MMS_ROOM_TYPE定义
	std::string		roomcode;					//会议室CODE
	std::string		password;					//会议室密码
	std::string		limitcode;					//会议室限制位: 仅对临时会议类型有效
	std::string		desc;						//会议室描述
	std::string		adminid;					//会议室管理员帐号,对于临时会议类型adminid为创建者，对于高级会议类型adminid为机构管理者.
	int				membersum;					//会议室中的总人数
	int				onlinesum;					//会议室中的当前在线人数
	std::string		schedule;					//none=无事件安排计划，appointed=指定日期会议(定时会议)，weekly=每周例会，monthly=每月例会，dialy=每日会议，manual=手工会议
	std::string		startdate;					//会议开始日期
	std::string		enddate;					//会议结束日期
	std::string		starttime;					//会议开始时间
	std::string		endtime;					//会议结束时间
	std::string		dayflag;					//日期标志
	int				defaultgroup;				//默认组
	_MMS_ROOM_TEMP()
	{
		roomtype=0;
		membersum=1;
		onlinesum=0;
		defaultgroup=0;
	};
}MMS_ROOM_TEMP,*PMMS_ROOM_TEMP;

typedef std::list<PMMS_ROOM_TEMP> MMS_ROOM_TEMP_LST;

typedef enum
{
	SIP = 0,
	SIP_SECOND,
	H323,
	H323_SECOND,
	OTHER,
}CALL_TYPE;

typedef struct tag_MPX_DIAGNOSIS_LOG_INFO
{
	int			nLogID;
	std::string strSenderMemberID;
	std::string strAVMemberID;
	std::string strDevName;
	std::string strChannelName;
	int			nResultIndex;
	std::string strTime;
	std::string strFileName;

	tag_MPX_DIAGNOSIS_LOG_INFO()
	{
		nLogID = -1;
		strSenderMemberID = "";
		strAVMemberID = "";
		strDevName = "";
		strChannelName = "";
		nResultIndex = -1;
		strTime = "";
		strFileName = "";
	}
}MPX_DIAGNOSIS_LOG_INFO,*PMPX_DIAGNOSIS_LOG_INFO;
typedef std::map<int ,MPX_DIAGNOSIS_LOG_INFO> MAP_DIAGNOSIS_INFO;
typedef MAP_DIAGNOSIS_INFO::iterator IT_MAP_DIAGNOSIS_INFO;

//操作日志查询条件
typedef struct tag_OPTLOG_QUERY
{
	int nStructSize;
	std::string time_begin;
	std::string time_end;
	std::string devname;
	std::string channelname;
	int			resultindex;
	int			page;
	int			pagesize;
	tag_OPTLOG_QUERY()
	{
		memset(this, 0, sizeof(tag_OPTLOG_QUERY));
		nStructSize = sizeof(tag_OPTLOG_QUERY);
	}
}OPTLOG_QUERY;