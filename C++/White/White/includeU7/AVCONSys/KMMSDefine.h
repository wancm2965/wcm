#pragma once

//=======================================================================================
// 类型定义
//=======================================================================================
//-------------------------------------
// 会议类型定义
//-------------------------------------
typedef enum _MMS_ROOM_TYPE
{
	MRT_ADVANCE			= 1,	//高级会议
	MRT_TEMPORARY		= 2,	//临时会议
	MRT_MONITOR			= 3,	//监控会议
	MRT_PERSONAL		= 4,	//个人会议
	MRT_FREE			= 5,	//自由会议
	MRT_HALL			= 6,	//沪江大厅
	MRT_MOBILECHANNEL   = 7,    //移动监控频道
	MRT_MULTIPLY        = 8,    //混合会议，用于消防跨级会议
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
	MMT_OBSERVER        = 18,   //医疗手术示教观摩者 定制AGENT处理
}MMS_MEMBER_TYPE;

//-------------------------------------
//会议属性 作用的对象
//--------------------------------------
typedef enum _MMS_SYNCOBJECT
{
	SYNCOBJ_ALL				= 0,	//会议所有人员
	SYNCOBJ_PRESIDER		= 1,	//主持人
	SYNCOBJ_ATTENDEE		= 2,	//与会者
	SYNCOBJ_APPOINT			= 3,	//特定人员
}MMS_SYNCOBJECT;

//--------------------------------------
//会议属性有效状态的条件
//--------------------------------------
typedef enum _MMS_PRM_AVLB_STTS
{
	PAS_PRIVATE           = 0,    //当设置人在会议时有效
	PAS_MEETING           = 1,    //当该会议在线时有效，当所有人退出该会议时失效
	PAS_MANUAL            = 2,    //手动控制该属性
}MMS_PRMAVLBSTTS;
//---------------------------------------------------------
// 成员SESSION结构
//---------------------------------------------------------
typedef struct _MEMBER_SESSION
{
	std::string strRoomID;
	std::string strGroupID;
	std::string strMemberID;
	unsigned short usMemberType;
	unsigned long ulActiveGroupID;
	std::string strNodeID;
	std::string strNatAddr;
	std::string strLocalAddr;
	unsigned short usLocalPort;
	unsigned short usTermType;
	std::string strTermName;
    std::string strOrderID;
	_MEMBER_SESSION()
	{
		strRoomID="";
		strGroupID="";
		usMemberType=0;
		ulActiveGroupID=0;
		usLocalPort = 0;
		usTermType = 0;
        strOrderID = "";
	};
}MEMBER_SESSION,*PMEMBER_SESSION;

//---------------------------------------------------------
// 成员信息项(可供用户修改的内容)
//---------------------------------------------------------
typedef struct _MEMBER_DEFINED_INFO
{
	std::string strMemberID;
	std::string strTermName;//终端名(TERMINAL,TR2,MOB,PC...)
	unsigned short usDisplayScreen;//用户屏幕数量
	//...
	_MEMBER_DEFINED_INFO()
	{
		strTermName="";
		usDisplayScreen=0;
	};
}MEMBER_DEFINED_INFO,*PMEMBER_DEFINED_INFO;

typedef std::list<MEMBER_DEFINED_INFO> MEMBER_DEFINED_INFO_LST,*PMEMBER_DEFINED_INFO_LST;

//-------------------------------------
// 会议成员状态定义
//-------------------------------------
typedef enum _MMS_MEMBER_STATUS
{
	MMS_OFFLINE			= 0,	//离线
	MMS_ONLINE			= 1,	//在线
}MMS_MEMBER_STATUS;

//-------------------------------------------
// 不在会议的监控设备信息
typedef struct NOINROOMDEV
{
	std::string		strDevID;
	std::string     strDevName;
	unsigned long	strDevType;
	unsigned short  usStatus;
	std::string		strParentName;
	std::string		strBindMemberid;
	NOINROOMDEV()
	{
		strDevID="";
		strDevName="";
		strDevType=0;
		usStatus=0;
		strParentName="";
		strBindMemberid="";
	};
}NOINROOMDEV,*PNOINROOMDEV;
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
	double			m_dMaxCodeStream;			//会议的最大码流,单位M
	double			m_dMinCodeStream;			//会议的最小码流

    // add 2010/11/30
    int             nPhyRoomID;                 // 物理会议室ID
    int		        nPhyRoomTypeID;             // 物理会议室类型ID

    // add 2011/03/01
    bool 		    bUseMeetingRecord;			//是否启用会议录像
    int			    UseReocrdType;				//全屏录像类型
    std::string     strRecordUser;				//单路录像者

	std::string		hjid;						//沪江ID
	std::string		remark;						//备注
	bool			bBind;						//是否被绑定,医疗服务定制

	_MMS_ROOM()
	{
		roomtype=0;
		membersum=0;
		onlinesum=0;
        nPhyRoomID = 0;
        nPhyRoomTypeID = 0;

        bUseMeetingRecord = false;
        UseReocrdType = 0;
		m_dMaxCodeStream =0.0;
		m_dMinCodeStream=0.0;
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
	bool			bautotem;					//是否为自动模版
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
		bautotem=false;
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
	std::string		strOrderID;				//排列序号
	_MMS_MEMBER()
	{
		membertype=0;
		strOrderID="";
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
	std::string		statusoff;				//statusoff="":正常退会 statusoff="Abnormal"异常退会
	_MMS_MEMBER_EX()
	{
		videocount	= 0;
		audiocount	= 0;
		svrport		= 0;
		localport	= 0;
		termtype	= 0;
		status		= 0;
		statusoff	= "";
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
	    for(MMS_WND_PRO_MAP::iterator it=mapWndPro.begin();it!=mapWndPro.end();++it)
	    {
            if(it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
        }
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
	    for(MMS_SCREEN_PRO_MAP::iterator it=mapScreenPro.begin();it!=mapScreenPro.end();++it)
	    {
            if(it->second != NULL)
            {
                delete it->second;
                it->second = NULL;
            }
        }
		//for_each( mapScreenPro.begin(), mapScreenPro.end(), SafeDelMapSecond<int, MMS_SCREEN_PRO>);
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

typedef enum _MMS_SEQ_TYPE
{
    MMS_FREE_TYPE = 0,        //自由模式，所有人都可以发言
    MMS_DISCUSS_TYPE,         //讨论模式，麦序前几位的可以同时发言
    MMS_GRAP_TYPE,            //抢麦模式，仅麦序为1的可以发言
}MMS_SEQ_TYPE;

typedef struct _MMS_SEQ_MEMBER
{
    std::string strMemberID;    //成员ID
    unsigned long ulStartTime;  //开始讲话时间(服务器时间)
    unsigned int nExTime;     //会议主持人为其额外增加的时间

    _MMS_SEQ_MEMBER()
    {
        ulStartTime = 0;
        nExTime = 0;
    }

    bool operator==(const _MMS_SEQ_MEMBER& other)
    {
		return strMemberID.compare(other.strMemberID.c_str()) == 0;
    }
}MMS_SEQ_MEMBER;

typedef std::list<MMS_SEQ_MEMBER> MMS_SEQ_MEMBER_LST,*PMMS_SEQ_MEMBER_LST;

typedef struct _MMS_MIC_SEQ
{
    unsigned short usType;        //队列类型
    unsigned int nDuration;  //抢麦维持时间
    unsigned int nMaxSize;    //同时抢麦人数
    unsigned int nSpUserNum;  //当前在讲话的人数
    MMS_SEQ_MEMBER_LST lstMember;    //麦序人员列表
    std::string strTag;      //扩展
    _MMS_MIC_SEQ()
    {
        usType = MMS_FREE_TYPE;
        nDuration = 1200000;
        nMaxSize = 5;
        nSpUserNum = 0;
    }
}MMS_MIC_SEQ;
     
typedef std::map<std::string,MMS_MIC_SEQ> MIC_SEQ_MAP;


typedef enum _MMS_WNDPRESET_TYPE
{
	MMS_SELF_TYPE = 0,			//自适应
	MMS_PRE_TYPE,				//启用预设
	MMS_PLUS_TYPE,				//画质增强
}MMS_WNDPRESET_TYPE;

//-------------------------------------
// 窗口预设结构体
typedef struct _MMS_WNDPRESET
{
	std::string			strRoomID;					//会议室ID
	std::string			strGroupID;					//会议室组ID
	unsigned int		uTempID;					//模板号
	unsigned int		uWndID;						//模板窗口号
	unsigned int		uWidthSD;					//标清分辨率宽
	unsigned int		uHeightSD;					//标清分辨率高
	unsigned int		uWidthHD;					//高清分辨率宽
	unsigned int		uHeightHD;					//高清分辨率宽
	unsigned int		uStream;					//码流
	MMS_WNDPRESET_TYPE	eType;						//应用类型
	
	_MMS_WNDPRESET()
	{
		strRoomID	= "";
		strGroupID	= "";
		uTempID		= 0;
		uWndID		= 0;
		uWidthSD	= 0;
		uHeightSD	= 0;
		uWidthHD	= 0;
		uHeightHD	= 0;
		uStream		= 0;
		eType		= MMS_SELF_TYPE;
	};
}MMS_WNDPRESET,*PMMS_WNDPRESET;

typedef enum _IMAGEQUALITY
{
	IMAGEQUALITY_MIN=0,
	IMAGEQUALITY_CLEAR,	//画质优先
	IMAGEQUALITY_FLUENCY ,	//流畅优先
	IMAGEQUALITY_MAX
}IMAGEQUALITY;