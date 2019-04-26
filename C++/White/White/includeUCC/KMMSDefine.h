#pragma once

#include "KGeneralFunc.h"
//=======================================================================================
// 类型定义
//=======================================================================================
//-------------------------------------
// 会议类型定义
//-------------------------------------
typedef enum _MMS_ROOM_TYPE
{
	MRT_ADVANCE			= 1,	//高级会议
	MRT_TEMPORARY		= 2,	//标准会议
	MRT_MONITOR			= 3,	//监控会议
	MRT_PERSONAL		= 4,	//个人会议
	MRT_FREE			= 5,	//自由会议
	MRT_HALL			= 6,	//沪江大厅
	MRT_MOBILECHANNEL   = 7,    //移动监控频道
}MMS_ROOM_TYPE;

//-------------------------------------
// 会议成员类型定义
//-------------------------------------
typedef enum _MMS_MEMBER_TYPE
{
	MMT_OWNER			= 1,	//会议所有者(只针对临时会议类型有效)
	MMT_PRESIDER		= 2,	//主持人
	MMT_PRESIDERINVITEE	= 3,	//被邀请主持人
	MMT_PRESIDERTEMP	= 4,	//临时主持人
	MMT_ATTENDEE		= 5,	//与会者
	MMT_INVITEE			= 6,	//被邀请与会者
	MMT_CHANNEL			= 7,	//监控通道
	MMT_TVWALL			= 8,	//电视墙
	MMT_TVCTRL			= 9,	//电视墙控制器
	MMT_AUDITOR			= 10,	//旁听者
	MMT_RECORD			= 11,	//录像服务器
	MMT_MP				= 12,	//MP
	MMT_TVPRESIDERTEMP	= 13,	//TV临时主持人
	MMT_CONSOLE			= 14,	//会议控制器
    MMT_ANONYMOUS		= 15,	//匿名用户
	MMT_ENCODECLIENT	= 16,	//合成视频终端
	MMT_3GGW			= 17,	//3G手机网关
}MMS_MEMBER_TYPE;

//-------------------------------------
// 会议成员状态定义
//-------------------------------------
typedef enum _MMS_MEMBER_STATUS
{
	MMS_OFFLINE			= 0,	//离线
	MMS_ONLINE			= 1,	//在线
}MMS_MEMBER_STATUS;

//-------------------------------------
// 会议项结构
typedef struct _MMS_ROOM
{
	std::string		domain;						//会议室所在域
	std::string		roomid;						//会议室ID
	std::string		roomname;					//会议室名称
	unsigned short	roomtype;					//会议室类型MMS_ROOM_TYPE定义
	std::string		roomcode;					//会议室CODE
	std::string		roommode;					//会议模式
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

    // add 2010/11/30
    int             nPhyRoomID;                 // 物理会议室ID
    int		        nPhyRoomTypeID;             // 物理会议室类型ID

    // add 2011/03/01
    bool 		    bUseMeetingRecord;			//是否启用会议录像
    int			    UseReocrdType;				//全屏录像类型
    std::string     strRecordUser;				//单路录像者

	_MMS_ROOM()
	{
		roomtype=0;
		membersum=0;
		onlinesum=0;
        nPhyRoomID = 0;
        nPhyRoomTypeID = 0;

        bUseMeetingRecord = false;
        UseReocrdType = 0;
	};
}MMS_ROOM,*PMMS_ROOM;

typedef std::list<PMMS_ROOM> MMS_ROOM_LST;

//-------------------------------------
// 会议组结构
typedef struct _MMS_GROUP
{
	std::string domain;						//会议室所在域
	std::string	roomid;						//会议室ID
	std::string	groupid;					//会议室组ID
	std::string	groupname;					//会议室组名称
	int			levelid;					//组的层次
	int			orderid;					//组的序号
	std::string	parentid;					//父级组ID
	std::string devgroupid;					//设备组ID
	_MMS_GROUP()
	{
		levelid=0;
		orderid=0;
	};
}MMS_GROUP,*PMMS_GROUP;

typedef std::list<PMMS_GROUP> MMS_GROUP_LST;

//-------------------------------------
// 会议模板项结构
typedef struct _MMS_SCREEN
{
	int		temtype;				//屏模板类型
	int		temid;					//屏模板号
	bool	temshow;				//是否显示屏窗口
	int		temwinnum;				//屏窗口数
	int		temfloatx;				//浮动窗口X
	int		temfloaty;				//浮动窗口Y
	int		temfloatcx;				//浮动窗口CX
	int		temfloatcy;				//浮动窗口CY

	_MMS_SCREEN()
	{
		temtype		= -1;
		temid		= -1;
		temshow		= false;
		temwinnum	= -1;
		temfloatx	= 0;
		temfloaty	= 0;
		temfloatcx	= 0;
		temfloatcy	= 0;
	}
}MMS_SCREEN,*PMMS_SCREEN;

//-------------------------------------
// 会议组详细信息结构
typedef struct _MMS_GROUP_EX : public _MMS_GROUP
{
	int				curscrid;					//当前屏幕号(1号屏/2号屏/3号屏/4号屏)
	
	MMS_SCREEN		screen[4];					//4套屏幕信息

	std::string		flag;						//该组是否允许召开子组会议 "Y": 允许   "N":不允许
	std::string		devid;						//设备ID(空为会议组,非空则为设备)

	int				membernum;					//组中总成员数
	int				onlinenum;					//组中在线成员数

	bool			subnode;					//是否为叶子节点（20081102 为组递归判断增加标记位add by dqs）
	int				subonline;					//在线子节点总数
	int				subtotal;					//子节点总数

	_MMS_GROUP_EX()
	{
		curscrid=0;		

		flag="Y";
		devid="";

		membernum=0;
		onlinenum=0;

		subnode=true;
		subonline=0;
		subtotal=0;
	};
}MMS_GROUP_EX,*PMMS_GROUP_EX;

//-------------------------------------
// 会议成员基础数据结构
typedef struct _MMS_MEMBER
{
	std::string		domain;					//会议室所在域
	std::string		roomid;					//会议室ID
	std::string		groupid;				//会议室组ID
	std::string		memberid;				//成员ID
	std::string		membername;				//成员名称
	unsigned short	membertype;				//成员类型MMS_MEMBER_TYPE定义
	unsigned int	orderid;				//排列序号
	_MMS_MEMBER()
	{
		membertype=0;
		orderid=0;
	};
}MMS_MEMBER,*PMMS_MEMBER;

//-------------------------------------
// 会议成员扩展数据结构
typedef struct _MMS_MEMBER_EX : public _MMS_MEMBER
{
	std::string		activegroupid;			//活动组
	int				videocount;				//视频数
	int				audiocount;				//音频数
	std::string		operflag;				//操作标志
	std::string		svrid;					//服务器ID
	std::string		svraddr;				//服务器地址
	unsigned short	svrport;				//服务器端口
	std::string		svrtype;				//服务器类型
	std::string		nodeid;					//NODEID
	std::string		nataddr;				//NAT地址
	std::string		localaddr;				//本地地址
	unsigned short	localport;				//本地端口
	unsigned short	termtype;				//终端类型
	std::string		termname;				//终端名称
	unsigned char	status;					//状态: 0=不在会议中 1=在会议中
	_MMS_MEMBER_EX()
	{
		videocount	= 0;
		audiocount	= 0;
		svrport		= 0;
		localport	= 0;
		termtype	= 0;
		status		= 0;
	}
}MMS_MEMBER_EX,*PMMS_MEMBER_EX;

typedef std::map<std::string,PMMS_MEMBER_EX> MMS_MEMBER_MAP;
typedef std::list<PMMS_MEMBER_EX> MMS_MEMBER_LST;

//-------------------------------------
// 会议投票结构
typedef struct _MMS_VOTE
{
	unsigned long	id;
	std::string		name;
	unsigned short	count;
	_MMS_VOTE()
	{
		id=0;
		count=0;
	};
}MMS_VOTE,*PMMS_VOTE;

typedef std::list<PMMS_VOTE> MMS_VOTE_LST;

//-------------------------------------
// 定损结构
typedef struct _MMS_LOSS
{
	std::string		id;			//报案号
	std::string		carid;		//车牌号
	std::string		userid;		//报损员账号
	std::string		factoryid;	//工厂号
	std::string		starttime;	//开始时间
	double			money;		//定损金额
	unsigned short	status;		//定损状态（0=定损中,1=定损结束,2=稍后定损）
	_MMS_LOSS()
	{
		money=0.00;
	};
}MMS_LOSS,*PMMS_LOSS;

//-------------------------------------
// 录像计划结构
typedef struct _MMS_RECPLAN
{
	std::string		strStartTime;
	std::string		strEndTime;
}MMS_RECPLAN,*PMMS_RECPLAN;

typedef std::list<PMMS_RECPLAN> MMS_RECPLAN_LST;

//-------------------------------------
// 录像格式结构
typedef struct _MMS_RECFORMAT
{
	std::string		strVODDis;		//屏幕点播分辨率
	unsigned short  usVODFPS;		//屏幕点播帧率
	unsigned short	usVODBPS;		//屏幕点播码流
	std::string		strArchiveDis;	//屏幕归档分辨率
	unsigned short  usArchiveFPS;	//屏幕归档帧率
	unsigned short	usArchiveBPS;	//屏幕归档码流
	unsigned short  usPPTFPS;		//PPT帧率
	unsigned short	usPPTBPS;		//PPT码流
	std::string		strSingleDis;	//个人分辨率
	unsigned short  usSingleFPS;	//个人帧率
	unsigned short	usSingleBPS;	//个人码流
	bool			bRecTogether;	//是否一起录象
	_MMS_RECFORMAT()
	{
		strVODDis="680*480";
		usVODFPS=15;
		usVODBPS=148;
		strArchiveDis="680*480";
		usArchiveFPS=30;
		usArchiveBPS=512;
		usPPTFPS=15;
		usPPTBPS=148;
		strSingleDis="320*240";
		usSingleFPS=15;
		usSingleBPS=148;
		bRecTogether=false;
	};
}MMS_RECFORMAT,*PMMS_RECFORMAT;

//////////////////////////////////////////////////////////////////////////
// 与会议室相关的数据结构
// add 2010/11/30

//-------------------------------------
// 窗口
typedef struct _MMS_WND_PRO
{
	int			                nWndid;             // 大,中,小 <==> 0,1,2
	int			                resolution;			// 
	int			                imaquality;			// 图像质量
	int			                stream;				// 码流
	int			                framerate;			// 帧率
}MMS_WND_PRO,*PMMS_WND_PRO;

typedef std::map<int, PMMS_WND_PRO> MMS_WND_PRO_MAP;

//-------------------------------------
// 屏幕
typedef struct _MMS_SCREEN_PRO
{
	int 	                    nScreenID;          // 屏号，(0,1,2,3)
	int				            nTMID;		        // 模板号
	MMS_WND_PRO_MAP             mapWndPro;          // 屏中的窗口表

	_MMS_SCREEN_PRO()
	{
		nScreenID = 0;
		nTMID = 0;
	}

	~_MMS_SCREEN_PRO()
	{
		for_each( mapWndPro.begin(), mapWndPro.end(), SafeDelMapSecond<int, MMS_WND_PRO>);
	}
}MMS_SCREEN_PRO,*PMMS_SCREEN_PRO;

typedef std::map<int, PMMS_SCREEN_PRO> MMS_SCREEN_PRO_MAP;

//-------------------------------------
// 会议室类型
typedef struct _MMS_ROOMTYPE
{
	int							nRoomTypeID;        // 会议室类型id
	std::string					strRoomTypeName;	// 会议室类型名称
	std::string					strFucList;			// 功能列表 |1|2|3|4|5|
	int							nScreenCount;		// 会议室屏幕数
	int							nPreempType;        // 是否是抢占式会议室类型
	int 						nExspokeswin;		// 除发言人外其他窗口的动作

	MMS_SCREEN_PRO_MAP          mapScreenPro;       // 会议室中的屏幕表

	_MMS_ROOMTYPE()
	{
		nRoomTypeID = 0;
		nScreenCount = 0;
	}

	~_MMS_ROOMTYPE()
	{
		for_each( mapScreenPro.begin(), mapScreenPro.end(), SafeDelMapSecond<int, MMS_SCREEN_PRO>);
	}

	bool operator == ( const struct _MMS_ROOMTYPE& lhs)
	{
		return (lhs.nRoomTypeID == this->nRoomTypeID);
	}
}MMS_ROOMTYPE,*PMMS_ROOMTYPE;

typedef std::list<PMMS_ROOMTYPE> MMS_ROOMTYPE_LST;

//-------------------------------------
// 物理会议
typedef struct _MMS_PHY_ROOM
{
	int					        nPhyRoomID;			    // 会议id
	std::string			        strRoomName; 	  		// 会议名称
	int					        nRoomTypeID;			// 会议类型ID
	std::string			        strOrderID; 	  		// 会议序号
} MMS_PHY_ROOM,*PMMS_PHY_ROOM;

typedef std::list<PMMS_PHY_ROOM> MMS_PHY_ROOM_LST;

//-------------------------------------
// 讨论设置
typedef struct _MMS_TALK_SET
{
	std::string			strFucList;				// conference
	int					resolution;				//  resolution
	int					stream;					//  stream
	int					framerate;				//  framerate
} MMS_TALK_SET,*PMMS_TALK_SET;

