#pragma once



//=======================================================================================
#define  MON_KEY_ID             "LOGID:"
#define  MON_KEY_PASSWORD       "PASSWORD:"
#define  MON_KEY_FROM           "FROM:"
#define  MON_KEY_TO             "TO:"
#define  MON_KEY_ATTR           "ATTR:"
#define  MON_KEY_OPERATE        "OPERATE:"
#define  MON_KEY_REFID          "REFID:"
#define  MON_KEY_EERCODE        "ERRCODE:"
#define  MON_KEY_USERID         "USERID:"
#define  MON_KEY_CHNAME			"CHANNELNAME:"
#define  MON_KEY_CONTROLFLAG	"CONTROLFLAG"
#define  MON_KEY_DEVTYPE        "DEVTYPE:"
#define  MON_KEY_DEVID          "DEVID"
#define  MON_KEY_DEVNAME        "DEVNAME"
#define  MON_KEY_CHSTATUS       "STATUS:"
#define  MON_KEY_ORDERID		"ORDERID:"
#define  MON_KEY_MCUID			"MCUID:"
#define  MON_KEY_MCUADDR		"MCUADDR:"
#define  MON_KEY_MCUPORT		"MCUPORT:"
#define  MON_KEY_MCUTYPE		"MCUTYPE:"
#define  MON_KEY_VIDEOCID       "VIDEOCID:"
#define  MON_KEY_AUDIOCID       "AUDIOCID:"
#define  MON_KEY_CHNUM          "CHANNELNUM:"
#define  MON_KEY_CHCNT          "CHANNELCOUNT:"
#define  MON_KEY_M_E_C_ID       "M_E_C_ID:"
#define  MON_KEY_SDPDATA        "SDPDATA:"
#define  MON_KEY_NODEID         "NODEID:"
#define  MON_KEY_NATADDR        "NATADDR:"
#define  MON_KEY_LOCALADDR      "LOCALADDR:"
#define  MON_KEY_LOCALPORT      "LOCALPORT:"
#define  MON_KEY_CALLID         "CALLID:"
#define  MON_KEY_CTRLINFO       "CTRLINFO:"
#define  MON_KEY_ALERTLEVEL     "ALERTLEVEL:"
#define  MON_KEY_TREENAME       "TREENAME:"
#define  MON_KEY_TREEID         "TREEID:"
#define  MON_KEY_TREEPID        "TREEPARENTID:"
#define  MON_KEY_TREENODETYPE   "TREENODETYPE:"
#define  MON_KEY_RSSADDR        "RSSADDR:"
#define  MON_KEY_RSSPORT        "RSSPORT:"
#define  MON_KEY_VODIP          "VODIP:"
#define  MON_KEY_TYPE           "TYPE:"
#define  MON_KEY_DATA           "DATA:"
#define  MON_KEY_RSPCODE        "RSPCODE:"
#define  MON_KEY_MASK	        "MASK:"
#define  MON_KEY_ALERT_SUBLEVEL	"ALERT_SUBLEVEL:"
#define  MON_KEY_SRVADDR        "SRVADDR:"
#define  MON_KEY_FROMNODE       "FROMNODE:"
#define  MON_KEY_TALKTYPE       "TALKTYPE"  //0:为正常对讲 1:对讲测试
#define  MON_KEY_TERMTYPE       "TERMTYPE"  //0:为正常对讲 1:对讲测试
#define  MON_KEY_TERMNAME       "TERMNAME"  //0:为正常对讲 1:对讲测试
#define  MON_KEY_DEVINFO        "MON_KEY_DEVINFO" 
#define  MON_KEY_CHANNELID      "MON_KEY_CHANNELID"
#define  MON_KEY_NAME           "NAME"
#define  MON_KEY_SERIALNO       "SERIALNO"
#define  MON_KEY_VERSION        "VERSION"
#define  MON_KEY_SYSSOFTVERSION "SYSSOFTVERSION"
#define  MON_KEY_CLIENTTYPE     "CLIENTTYPE"
#define  MON_KEY_NATTYPE        "NATATYPE"
#define  MON_KEY_FACTORY		"FACTORY"
#define	 MON_KEY_BINDMEMBERID	"BINDMEMBERID"
#define  MON_KEY_MGSID          "MGSID"
//新增对讲相关lbj20110721
#define	 MON_KEY_USER_DATA		"USERDATA"
#define	 CMD_CH_AUDIO_START		"CH_AUDIO_START"
#define  CMD_CH_AUDIO_ON_START	"CH_AUDIO_ON_START"
#define	 CMD_CH_AUDIO_STOP		"CH_AUDIO_STOP"
#define  CMD_CH_AUDIO_DEV_STOP	"CH_AUDIO_DEV_STOP"
#define	 CMD_CH_AUDIO_CHANGE	"CH_AUDIO_CHANGE"
#define  TALK_TYPE_MEETING		"MEETINGTALK"
#define  TALK_TYPE_MON			"MONTALK"
#define MON_KEY_TALK_CUSTOM_ENABLE	"USEAUDIOCODETYPE"
#define MON_KEY_TALK_CODECTYPE		"AUDIOCODETYPE"
#define MON_KEY_TALK_DEVTYPE		"DEVTYPE"

#define AVCON_CMD_CONNECTION_CHANGED	"CONNECTION_CHANGED"
#define AVCON_KEY_CLIENTID				"CLIENT_ID"
#define AVCON_KEY_SERVERIP				"SERVER_IP"
#define AVCON_KEY_SERVERPORT			"SERVER_PORT"
#define AVCON_KEY_ACCOUNT				"ACCOUNT"
#define AVCON_KEY_PASSWORD				"PASSWORD"

//REQ20080325001
#define  MON_KEY_YT_TYPE		"YTTYPE:"
#define  MON_KEY_YT_COM			"YTCOM:"
#define  MON_KEY_YT_NO			"YTNO:"		//云台编号
#define  MON_KEY_PRESET_NO		"PRESETNO:"	//预置点编号
#define  MON_KEY_PRESET_NAME	"PRESETNAME:"	//预置点名称
#define  MON_KEY_YT_SPEED		"YTSPEED:"
#define  MON_KEY_YT_CMD			"YTCMD:"
#define  MON_KEY_PTYPE			"PTYPE:"
#define  MON_CONTROL_TYPE		"CTRLTYPE:"


//=======================================================================================
#define  MON_AV_EID			    "EID"
#define  MON_AV_CID			    "CID"
#define  MON_AV_ENAME		    "ENAME"
#define  MON_AV_CHNAME		    "CH_NAME"
#define  MON_AV_HDDEVICE	    "MON_AV_HDDEVICE"

//=======================================================================================
// 网络代理信息
typedef struct _XPROXY_INFO
{
    unsigned int	proxytype;		// 代理类型: 0=direct 1=http proxy 2=socks5 3=http tunnel
    std::string		proxyhost;		// 代理服务器地址
    unsigned short	proxyport;		// 代理服务器端口
    std::string		proxyuser;		// 代理账户
    std::string		proxypass;		// 代理密码
}XPROXY_INFO,*PXPROXY_INFO;

#define ERRNO_INVALID_ACCOUNT		301
#define ERRNO_INVALID_PASSWORD		302
#define ERRNO_DUPLICATE_LOGIN		304
#define ERRNO_DATABASE				305
#define ERRNO_UNKNOWN				306

//=======================================================================================
enum _MON_TREENODE_TYPE
{
	MON_TREENODE_REGION = 0,
	MON_TREENODE_DEV,
	MON_TREENODE_DEVCH,
	MON_TREENODE_DEVCH_UPDATE
};

//=======================================================================================
//房间状态
enum _ROOM_STATUS
{
	ROOM_STATUS_NONE = 0,	//未进入
	ROOM_STATUS_MET,		//会议
	ROOM_STATUS_MON,		//监控
};

typedef enum _MON_DEVTYPE
{
    MON_DEV_MON = 0,    // 监控终端，普通设备
    MON_DEV_MGW,        // 监控网关
    MON_DEV_MP,         // 会议网关
    MON_DEV_MET,        // 会议终端
    MON_DEV_MOB,        // 手机终端
    MON_DEV_H323T,      // H323终端
    MON_DEV_H323M,      // H323 MCU(不用)
    MON_DEV_TVS,        // 电视墙端
    MON_DEV_OCX,        // OCX
    MON_DEV_ENCODE,     //视频合成终端

    MON_DEV_INVALID = 999,  // 非法
}MON_DEVTYPE;

//-------------------------------------
//生产厂家
typedef enum _MON_FACTORY
{
	//前端设备
	HPD_DVR = 0, /*华平*/
	HIK_DVR, /*海康*/
	DAH_DVR, /*大华*/
	KEDA_DVR, /*科达*/
	HHS_DVR, /*黄河*/
	HBT_DVR, /*汉邦高科*/
	YAA_DVR, /*天津亚安*/
	TDY_DVR, /*天地伟业*/
	CJV_DVR, /*十五所*/
	JTN_DVR, /*杭州竞天*/
	PLT_DVR, /*平台*/
	TLV_DVR, /*上海通琅*/
	BSC_DVR, /*蓝卫通*/
	XINQI_DVR, /*芯启*/
	SKY_DVR, /*蓝色星际*/
	MAX_DVR,

	//手机、平板终端
	HPD_MOB = 0x99,		/*华平手机*/
	HDS_1000,			/*单兵设备 OEM NXP*/
	HDS_2000,			/*通琅*/

	//平台设备
	PLT_C7  = 0x300,	/*创世平台*/
	PLT_KEDA,			/*科达平台*/
	MAX_PLT,

	//HDS序列设备
	HDS_100 = 0x500,	/*华平车载终端*/
	MAX_HDS,

	//NDS序列设备
	NDS_100 = 0x600,	/*OEM海康*/
	NDS_200,			/*OEM海康*/
	NDS_300,			/*OEM黄河*/
	NDS_410,			/*OEM通立*/
	MAX_NDS,

	//MCS系列设备
	MCS_2000 = 0x700,	/*矩阵控制器*/
	MAX_MCS,

	//RTSP
	RTSP_HHS = 0x1000, /*黄河*/
	RTSP_ALK, /*艾立克*/
	RTSP_KEDA, /*科达*/
	RTSP_SANYO, /*三洋*/
	MAX_RTSP,
}MON_FACTORY;

#define HP_DVR HPD_DVR

//-------------------------------------
// 基本设备信息
typedef struct _BASE_MON_DEV_INFO
{
    _BASE_MON_DEV_INFO()
    {
        usMcuPort       = 0;
        usLocalPort     = 0;
        usTermType      = 0;
		usFactory		= 0;
        ulVersion       = 0;
		usControlFlag	= 0;
		usStatus		= 0;
		strOrderID		= "";
        eDevType        = MON_DEV_INVALID;
    }

    // 注册的服务信息
    std::string		strMcuID;
    std::string		strMcuAddr;
    unsigned short	usMcuPort;
    std::string     strMcuType;
    std::string     strMGSID;

    // 设备的基本信息
    std::string		strID;      // 设备的ID
    std::string		strNodeID;
    std::string		strNatAddr;
    std::string     strNatType;
    std::string		strLocalAddr;
    unsigned short	usLocalPort;
	unsigned short	usFactory;

    // 设备的标识等
    std::string		strPassword;
    MON_DEVTYPE     eDevType;
    unsigned short	usTermType;
    std::string		strTermName;
    std::string		strName;
    std::string		strSerialno;
    unsigned long   ulVersion;
    std::string		strSysSoftVersion;
	std::string		strBindMemberID;
    std::string     strParentID;
	unsigned short	usControlFlag;
	std::string     strOrderID;
	unsigned short  usStatus;
	unsigned short  usChannelNum;
	
} BASE_MON_DEV_INFO,*PBASE_MON_DEV_INFO;

//-------------------------------------
// 网关信息
typedef struct _MON_GATEWAY_INFO : public BASE_MON_DEV_INFO
{
} MON_GATEWAY_INFO,*PMON_GATEWAY_INFO;

//-------------------------------------
// 监控设备信息
typedef struct _MON_DEV_INFO : public BASE_MON_DEV_INFO
{
} MON_DEV_INFO, *PMON_DEV_INFO;

//-------------------------------------
// 监控通道信息
typedef struct _MON_CHANNEL_INFO : public BASE_MON_DEV_INFO
{
    _MON_CHANNEL_INFO()
    {
        usChannelNum = 0;
        usSpType = 0;
        usStatus = 0;
        ulVideocID = 0;
		usVideoCodeType = 0;
        ulAudiocID = 0;
		nControlFlag = 0;
    }
    std::string     strChannelID;
    unsigned short  usChannelNum;
    std::string		strChannelName;
    std::string		strDevName;
    std::string     strDevID;
    unsigned short  usSpType;
    unsigned short  usStatus;
    unsigned long	ulVideocID;
	unsigned short  usVideoCodeType;
    unsigned long	ulAudiocID;
    std::string		strDevInfo;
	unsigned int	nControlFlag;
} MON_CHANNEL_INFO, *PMON_CHANNEL_INFO;

//=======================================================================================
//UDP监听报警信息
typedef struct _UDP_RCV_ALART_INFO
{
	_UDP_RCV_ALART_INFO()
	{
		usWall = 1; //0:不上大屏,1:上大屏; 如果是视频点(strType=SPD03)时大屏显示方式(全屏显示：0，追加显示：1)
		usMonitorType = 0; //0表示接警，1表示出警，默认为0
	}
	std::string strChannelID;
	std::string strType;
	std::string strSender;
	std::string strReceiver;
	std::string strSendTime;
	std::string	strEventID;
	std::string strOrg;
	std::string strLongitude;
	unsigned short  usWall;
	unsigned short  usMonitorType;
}UDP_RCV_ALART_INFO,*PUDP_RCV_ALART_INFO;
//=======================================================================================

typedef struct _YT_Control_Parameters
{
public:
	_YT_Control_Parameters()
	{
		nChannelNum = -1;
		strPresetNO = "";
		strPresetName= "";
		nSpeed = 50;
	}

	_YT_Control_Parameters& operator=(const _YT_Control_Parameters& rChannelItem)
	{
		nChannelNum = rChannelItem.nChannelNum;
		strPresetNO = rChannelItem.strPresetNO;
		strPresetName = rChannelItem.strPresetName;
		nSpeed = rChannelItem.nSpeed;

		return *this;
	}

public:
	unsigned short nChannelNum;	//通道号
	std::string strPresetNO;	//所有预置点的编号，如“1,3,5”，用','分隔开
	std::string strPresetName;	//所有预置点的名称，如“前门,后院,边门”，用','分隔开，与编号相对应！
	unsigned short nSpeed;

}YT_CONTROL_PARAMETERS;

//=======================================================================================
typedef enum _MON_DVR_ATTR
{
	MON_DVR_DEVICE = 0,
	MON_DVR_MEET,
	MON_DVR_VIEWER,
}MON_DVR_ATTR;

//=======================================================================================
typedef enum _MONERR_CODE 
{
	MONERR_PACKET=0,
	MONERR_DB,
	MONERR_CH_INVALID,   // 无效通道
	MONERR_OFFLINE,
	MONERR_CH_UPDATE_FAIL,
	MONERR_UNKNOW,
}MON_ERR_CODE;

//-------------------------------------
// 录像结构
typedef struct _MON_RECORD
{
	std::string filename;	//文件名
	std::string writedate;
	std::string completedate;
	unsigned long filesize;
	std::string dvrid;
	unsigned short channelid;
	unsigned short filetype;
	std::string filestarttime;
	std::string fileendtime;
	std::string serverip0;
	std::string serverip1;
	std::string serverip2;
}MON_RECORD,*PMON_RECORD;

//=======================================================================================
typedef enum _MONERR_DEVCHECK
{
	MONERR_DEVCHECK_NONE=0,
	MONERR_DEVCHECK_OLDSERIALNO_ISEMPTY,	//现有设备序列号为空
	MONERR_DEVCHECK_SERIALNO_ISNOTSAME,		//现有设备帐号和新设备帐号相同,但是序列号不同
	MONERR_DEVCHECK_DEVID_ISNOTSAME,		//现有设备帐号和新设备帐号不同,但是序列号相同
	MONERR_DEVCHECK_NEWDEVID_ISNOTEXIST,	//新设备帐号不存在
	MONERR_DEVCHECK_NEWDEVID_SERIALNO,		//新设备帐号已经绑定了序列号
	MONERR_DEVCHECK_NEWDEVID_LOGIN,			//新设备帐号已经登录
	MONERR_DEVCHECK_NEWSERIALNO_USED,		//新设备序列号已经被使用
}MONERR_DEVCHECK;

typedef struct _RegionItem
{
	unsigned int nType;
	unsigned int ID;
	std::string strName;
	std::string strDevID;
	_RegionItem()
	{
		nType=0;
		ID=0;
	};
}RegionItem;

typedef struct tagMemInfo
{
	std::string	nodeid;
	std::string memberid;
} MEMINFO;