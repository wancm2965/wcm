#pragma once
//此头文件定义AVView宏命令 便于管理

#include <string>
#include <vector>
#include <list>
#include <map>
#include "hptm\HPTM_Templet.h"
#include "..\include\HPDEV_Define.h"
using namespace std;

// [6/25/2009 lzy]
//---宏命令区域--------------------------------------------------------------------------
//数字定义区域
#define MONITOR_AUDIO_INDEX							201		//监控对讲通道ID
#define BEGIN_COMMAND_ID	0
#define END_COMMAND_ID		65535


#define ENABLE_CHANNEL_ONLINE		1 			//镜头在线(不可控)
#define ENABLE_CHANNEL_RCV			2 			//镜头单接图标
#define ENABLE_CHANNEL_REC			4			//录像
#define ENABLE_CHANNEL_RCV_A		8			//镜头单接音频图标
#define ENABLE_CHANNEL_RCV_V		16			//镜头单接视频图标
#define ENABLE_CHANNEL_POLL			32			//轮循

#define WM_DECODER_CLOSE_DISPLAY		WM_USER+2233		//通知硬解码卡关闭通道

//字符串定义区域
#define CMD_GET_AVWIZARD					"AVWIZARD_GETTING"
#define CMD_SET_AVWIZARD					"AVWIZARD_SETTING"
#define CMD_FILL_WND_SHOW					"FILL_WND_SHOW"
#define CMD_FILL_VIDEO_SHOW					"FILL_VIDEO_SHOW"
#define CMD_FILL_WND_SHOW_FOR_ALL_TVS		"FILL_WND_SHOW_FOR_ALL_TVS"
#define CMD_FILL_VIDEO_SHOW_FOR_ALL_TVS		"FILL_VIDEO_SHOW_FOR_ALL_TVS"
#define CMD_CHANGE_MAIN_SUB_FOR_ALL_TVS     "CHANGE_MAIN_SUB_FOR_ALL_TVS"
#define CMD_CHANGE_MAIN_SUB_FOR_TVS			"CHANGE_MAIN_SUB_FOR_TVS"
#define CMD_DEFAULT_SUB_WIN_BIG_FOR_ALL_TVS	"DEFAULT_SUB_WIN_BIG_FOR_ALL_TVS"
#define CMD_DEFAULT_SUB_WIN_SMALL_FOR_ALL_TVS	"DEFAULT_SUB_WIN_SMALL_FOR_ALL_TVS"
#define CMD_GET_AVWIZARD_CONFERENCE			"AVWIZARD_RUEST"
#define CMD_AVWIZARD_CONFERENCE_RESULT		"AVWIZARD_ACCEPT"
#define CMD_AVWIZARD_CONFERENCE_SETTING		"AVWIZARD_SETTING"

//命令头
#define RECEIVE_VIDEO			"RECEIVE_VIDEO"			//接收视频
#define CHANGE_VIDEO			"CHANGE_VIDEO"			//同一个TVS的窗口拖拽/交换
#define CLOSE_VIDEO				"CLOSE_VIDEO"			//关闭指定视频
#define CHANGE_TM				"CHANGE_TM"				//更改模板
#define VIDEO_FULLSCR			"VIDEO_FULLSCR"			//视频全屏
#define RECEIVE_VIDEO_RESULT	"RECEIVE_VIDEO_RESULT"	//TVS接收视频结果返回
#define CHANGE_VIDEO_RESULT		"CHANGE_VIDEO_RESULT"	//同一个TVS的窗口拖拽/交换结果返回
#define CLOSE_VIDEO_RESULT		"CLOSE_VIDEO_RESULT"	//关闭指定视频结果返回
#define CHANGE_TM_RESULT		"CHANGE_TM_RESULT"		//更改模板结果返回
#define VIDEO_FULLSCR_RESULT	"VIDEO_FULLSCR_RESULT"	//视频全屏结果返回
#define GET_TVS_TMINFO			"GET_TVS_TMINFO"		//得到当前模板信息
#define SEND_TVS_TMINFO			"SEND_TVS_TMINFO"		//回调给TVC当前自身模版
#define GET_FULLSCR_STATUS		"GET_FULLSCR_STATUS"	//索要TVS全屏状态
#define FULLSCR_STATUS_RESULT	"FULLSCR_STATUS_RESULT"	//返回TVS全屏状态
#define SEND_TVS_MONALERT		"SEND_TVS_MONALERT"		//发送报警
#define TVS_MONALERT_RESULT		"TVS_MONALERT_RESULT"	//TVS接收到TVC的报警回馈
#define TVS_DO_ALARM			"TVS_DO_ALARM"			//TVS显示警报动作
#define TVS_CMD_END_RESULT		"TVS_CMD_END_RESULT"	//TVS返回次命令
#define TVS_CMD_UDP_END			"TVC_CMD_UDP_END"		//119接警一条命令处理结束
#define TVS_CMD_PLUGIN			"TVS_CMD_PLUGIN"		//控制端发给服务端，下载插件的命令

#define TVWALL_CMD				"USERCMD"			//命令头
#define TVC_ID					"TVC_ID"			//TVC的登录ID
#define TVS_ID					"TVS_ID"			//TVS的登录ID
#define FROM_SCR				"FromScr"			//源屏幕号
#define FROM_INDEX				"FromIndex"			//源窗口号
#define TO_SCR					"TOScr"				//目标屏幕号
#define TO_INDEX				"TOIndex"			//目标窗口号
#define TVS_SCR					"TVSScr"			//TVS的屏幕号
#define TVS_INDEX				"TVSIndex"			//TVS的窗口号
#define MEMBER_ID				"Video_memberid"	//视频的memberid
#define CARD_ID					"Video_cardid"		//视频的卡id
#define TM_TYPE					"TMType"			//模板类型4：3或者16：9
#define TM_SUBTYPE				"TMSubType"			//模板子类型
#define RESULT					"result"			//结果
#define FROMID					"FROMID"			//发送ID
#define TOID					"TOID"				//接收ID
#define TM_TYPEARR				"TMType%d"			//模板类型
#define TM_SUBTYPEARR			"TMSubType%d"		//模板子类型
#define TM_FULL_SCREEN			"TMFullScreen"		//模板全屏/取消全屏
#define SCR_COUNT				"SCR_COUNT"			//屏幕数
#define FULL_TVS_INDEX			"FULL_TVS_INDEX"	//全屏的窗口号
#define REV_TYPE				"REV_TYPE"			//接收类型（视频、地图等）
#define LASTPLAYINFO_NUM		"LastPlayInfo_Num"		//上次接收的视频个数
#define LASTPLAYINFO_SCR		"LastPlayInfo_Scr%d"	//上次接收的视频屏号
#define LASTPLAYINFO_INDEX		"LastPlayInfo_Index%d"	//上次接收的视频窗口号
#define LASTPLAYINFO_MEMID		"LastPlayInfo_Memid%d"	//上次接收的视频memberid
#define LASTPLAYINFO_CARDID		"LastPlayInfo_Cardid%d"	//上次接收的视频卡id
#define LASTPLAYINFO_BROUND		"LastPlayInfo_BRound%d" //是否是轮询视频
#define DECODE_TYPE		    	"DecoderType"			//软硬解码器类型 0:soft 1:hk,2DH
#define HARD_DECODER_MAX_INPUT	"HardDecoderMaxInput"		//硬解码卡最大的输入个数
#define TVC_CMD_END				"TVC_CMD_END"			//电视墙接警一组名字结束标示
#define TVC_CMD_UDP_END			"TVC_CMD_UDP_END"		//119接警一条命令处理结束
#define PLUGIN_TYPE				"PLUGIN_TYPE"			//下载插件的类型
#define PLUGIN_CURVER			"PLUGIN_CURVER"			//下载插件的当前版本
#define	PLUGIN_CMD				"PLUGIN_CMD"			//下载插件的命令
//接收会议人员视频使用
#define ChanName				"ChanName"
#define AudChanID				"AudChanID"	//人员的音频ID
#define	VidChanID				"VidChanID"	//人员的视频ID
#define PeerNodeID				"PeerNodeID"
#define PeerNATIP				"PeerNATIP"
#define PeerLocalIP				"PeerLocalIP"
#define PeerLocalPort			"PeerLocalPort"
#define PeerMCUID				"PeerMCUID"
#define PeerMCUIP				"PeerMCUIP"
#define PeerMCUPort				"PeerMCUPort"

#define ATTRIB_RESULT			"result"
#define ATTRIB_RESULT_SUCCESS	"success"
#define ATTRIB_RESULT_FAILUER	"failure"
#define ATTRIB_RESULT_BUSY		"busy"
#define ATTRIB_RESULT_FINISH	"finish"
#define ATTRIB_RESULT_REFUSE	"refuse"
#define ATTRIB_RESULT_REFUSE	"refuse"
#define ATTRIB_RESULT_SELF		"self"
#define ATTRIB_RESULT_SAMEROOM	"sameroom"

#define ATTRIB_USER				"user"
#define ATTRIB_USERNAME			"usrname"

// [6/26/2009 lzy] 此处定义的是TVS端发送给TVC端相关的结果
#define TVC_RESULT_OPEN_VIDEO_SUCCESS			0		//TVS端打开视频失败
#define TVC_RESULT_OPEN_VIDEO_FAILED			1		//TVS端打开视频失败
#define TVC_RESULT_CHANGE_TM_FAILED				2		//更改模板错误
#define TVC_RESULT_NO_INPORT_USED				3			//没有输入端口可用
#define TVC_RESULT_UNSUPPORT					4			//解码卡不支持此流
#define TVC_RESULT_CHANN_CONNECTING				5			//通道正在连接
#define TVC_RESULT_CHANN_PLAYBACK_FAILED		6		//录像回放打开失败

//---枚举区域--------------------------------------------------------------------------
enum {GROUP_ORG,GROUP_DEV};

//设备类型
typedef enum
{
	TVW_DEV_UNKNOWN	= 0,
	TVW_DEV_TVS_H	= 1,	//软解码电视墙服务器
	TVW_DEV_TVS_D,			//硬解码电视墙服务器
	TVW_DEV_MATRIX,			//矩阵
	TVW_DEV_ANALOG,			//模拟信号输出设备
}TVW_DEVICE_TYPE;

//解码卡类型
typedef enum
{
	DECODE_CARD_H	= 0,	//软解
	DECODE_CARD_HK,			//硬解码类型 海康
	DECODE_CARD_DH,			//硬解码类型 大华
	DECODE_CARD_AN,			//模拟信号
}DECODE_CARD_TYPE;

//电视墙布局类型
enum HPTVC_TM_LAYOUT
{
	TVC_TM_LAYOUT_NO = 0,
	TVC_TM_LAYOUT_3X1 = 3,
	TVC_TM_LAYOUT_3X2 = 6,
};

typedef enum _TVW_RECV_TYPE
{
	TVW_RECV_TYPE_VIDEO = 0,
	TVW_RECV_TYPE_MAP,
	TVW_RECV_TYPE_PLAYBACK,
}TVW_RECV_TYPE;


enum _MENU_TYPE
{
	M_RcvAVChannel=0,	//接收通道音视频
	M_RcvAChannel,		//接收音频
	M_RcvVChannel,		//接收视频
	M_FindRecFile,		//搜索录像文件
	M_RecChannel,		//录像
	M_CloseChannel,		//关闭正在接收中的音、视频
	M_DevTalk,			//监控对讲
	M_DevTalkTest,		//监控对讲测试
	M_DevMatrix,		//矩阵控制
	M_ShowTMControl,	//显示模板控制器
	M_ShowTitle,		//显示标题栏
	M_SetAudio,			//音频设置
	M_ChangeMainSub,	//切换视频主/子流
	M_PlugInManage,		//插件管理
	M_ExitMonClient,	//退出监控客户端
	M_ShowFillWin,		//填充窗口显示
	M_ShowPorportion,	//视觉比例全景显示
	M_DefSubWinBig,		//大窗口默认接收子流
	M_DefSubWinSmall,	//小窗口默认接收子流
	M_DevRound,			//设备轮循
	M_TempGroupRound,	//临时分组轮循（单窗口）
	M_TempGroupRoundM,	//临时分组轮循（多窗口）
	M_TempTVCGroupRound, //电视墙组轮循
	M_OpenMap,			//打开地图  
	M_EditMap,			//编辑地图  
	M_SaveMap,			//保存地图
	M_GoToChannel,
	M_DelMap,			//删除地图
	M_CloseMap,  		//关闭地图
	M_LOSS,				//定损
	M_OpenAVDiagnosis	//启用视频诊断
};

//消息参数wParam
typedef enum
{
	Mon_TempShow = 0,			//预览
	Mon_Replay = 1,				//回放
	Mon_Map,					//地图
	Mon_SelectScr1,				//选1号屏
	Mon_SelectScr2,				//选2号屏
	Mon_SelectScr3,				//选3号屏
	Mon_SelectScr4,				//选4号屏
	Mon_LocalSetting,			//本地设置
	Mon_MatrixSet,				//矩阵控制器设置
	Mon_DVRSetting,				//远程DVR设置
	Mon_Lock,					//锁定
	Mon_Exit,					//退出
	Mon_Select,					//选择模板
	Mon_FullScr,				//全屏
	Mon_Video,					//录像
	Mon_BeginTalk,				//启动对讲
	Mon_Info,					//详细信息
	Mon_Photo,					//拍照
	Mon_PriScr,					//拍屏
	Mon_ConShoot,				//连拍
	Mon_ChkLoss,				//定损
	Mon_TVSSetting,				//电视墙屏幕设置
	Mon_DiagnosisLog,			//诊断日志

}TOOLBAR_TYPE;

enum ITEM_STATUS
{
	S_RES_ROOT=0,			//根结点
	S_GROUP,		        //区域
	S_DEV_ONLINE,			//设备在线
	S_DEV_OFFICE,			//设备不在线
	S_CHANNEL_ONLINE,		//镜头在线(不可控)
	S_CHANNEL_ONLINE_OPT,	//镜头在线(可控)
	S_CHANNEL_OFFLINE,		//镜头不在线
	S_CHANNEL_RCV,			//镜头单接图标(不可控)
	S_CHANNEL_RCV_OPT,		//镜头单接图标(可控)
	S_CHANNEL_REC,			//录像(不可控)
	S_CHANNEL_REC_OPT,		//录像(可控)
	S_CHANNEL_POLL, 		//轮循
	S_CHANNEL_POLL_OPT,		//轮循(可控)
	S_CHANNEL_POLLREC,		//轮循录像

	S_SELFDEFINE_DEV_ROOT,	//自定义设备根结点
	S_ANG_DEV,				//自定义设备
	S_ANG_PORT,				//自定义设备输出端口正常状态
	S_ANG_PORTLINK,			//自定义设备输出端口关连状态

	S_CHANNEL_RCV_A,		//镜头单接音频图标
	S_CHANNEL_RCV_A_OPT,	//镜头单接音频图标(可控)
	S_CHANNEL_RCV_V,		//镜头单接视频图标
	S_CHANNEL_RCV_V_OPT,	//镜头单接视频图标(可控)
};

//[3/16/2009 licong]
//监控报警定义
enum _ALARM_TYPE{
	ALARM_DISK_FULL = 0,	//硬盘满                        磁盘序号
	ALARM_VIDEO_LOST,		//信号丢失                      通道号
	ALARM_VIDEO_MOTION,		//移动侦测						通道号
	ALARM_DEFENT_IO,		//信号量报警                    报警输入端口号
	ALARM_VIDEO_BLOCK,		//遮挡报警						通道号
	ALARM_VIDEO_NOMATCH,	//制式不匹配					通道号
	ALARM_DISK_UNFORMAT,	//硬盘未格式化					磁盘序号
	ALARM_DISK_READ_ERROR,	//读写硬盘出错					磁盘序号
	ALARM_ACCESS_VIOLATION, //非法访问						保留
	ALARM_UNKNOW_ERROR,		//非法访问						保留
};



//---------结构体区域------------------------------------------------------------------

//TVS接像状态
typedef struct _TVS_LASTPLAY_INFO
{
	int screen;				//屏号
	int index;				//窗口号
	string memberid;		//视频的memberid
	unsigned long cardid;	//卡id
	bool bRound;			//是否为轮询残留窗口
}TVS_LASTPLAY_INFO;


//服务推下来的TVS列表
typedef struct _TVW_SERVER_INFO
{
	std::string         ServerID;  		//TVS-ID
	std::string			ServerName;		//TVS-名称
	std::string			Status;			//TVS-状态
}TVW_SERVER_INFO,*PTVW_SERVER_INFO;
typedef map<std::string, TVW_SERVER_INFO> MAP_TV_SERVER_INFO;
typedef MAP_TV_SERVER_INFO::iterator MAP_TV_SERVER_INFO_IT;


//设备信息
typedef struct _TVW_DEVICE_INFO
{
	TVW_DEVICE_TYPE		Type;			//设备类型
	std::string			ID;				//设备ID（TVS-ID或矩阵ID）
	std::string			Name;			//设备名称（TVS-名称或矩阵名称）
	int					InPort;			//输入端口数量（矩阵的输入端口个数）
	int					OutPort;		//输出端口数量（矩阵输出端口个数、TVS屏幕个数）
	bool				bStatus;		//服务器状态
}TVW_DEVICE_INFO,*PTVW_DEVICE_INFO;
typedef map<std::string, TVW_DEVICE_INFO> MAP_TVW_DEVICE_INFO;		//此map的key由Type和ID的组合 为Type_ID
typedef MAP_TVW_DEVICE_INFO::iterator MAP_TVW_DEVICE_INFO_IT;



//设备和TV之间关联信息
typedef struct _TV_INFO
{
	int 				ScreenID;   	//屏幕ID
	std::string			ScreenName;  	//屏幕名称
	std::string			DevInfoKey;		//关联的设备信息
	int					Port;			//关联到设备的某个端口
	TVW_DEVICE_TYPE		Type;
	DECODE_CARD_TYPE	CardType;		//对应TVS的解码类型
}TV_INFO,*PTV_INFO;
typedef map<int, TV_INFO> MAP_TV_INFO;
typedef MAP_TV_INFO::iterator MAP_TV_INFO_IT;

typedef map<int,int> MAP_TM2TVPOS;
typedef MAP_TM2TVPOS::iterator MAP_TM2TVPOS_IT;
//设备端口信息
typedef struct _DEV_PORT_INFO
{
	TVW_DEVICE_TYPE     deviceType;		//设备类型（矩阵或TVS）
	DECODE_CARD_TYPE	CardType;		//针对TVS的解码类型
	int 				PortID;			//端口ID（对应矩阵的几号端口或者TVS的几号屏）
	string           	PortName;		//端口名称
	string 				strDevID;		//设备ID（TVS-ID或矩阵ID）
	string				strDevName;		//设备名称（TVS-名称或矩阵名称）
	BOOL             	bIsOnLine;		//是否在线
	int					tm_index;		//模板编号
	HPTM_TM_TYPE		tm_type;		//模版类型
	HPTM_TM_SUBTYPE		tm_subtype;		//模版子类型
	int					iPos;			//关联电视机信息（-1 未关联；其他值为关联位置）
	BOOL				bLinkMatrix;	//是否与矩阵关联

	int					iFullWndIndex;	//视频全屏的索引
	BOOL				bLink;			//是否已被关联
	CWnd*				pWndShow;		//关联的显示信息
}DEV_PORT_INFO,*PDEV_PORT_INFO;
typedef std::map<string, DEV_PORT_INFO> MAP_DEV_PORT_INFO;		//此map的key由deviceType_strDevID_PortID组合
typedef MAP_DEV_PORT_INFO::iterator MAP_DEV_PORT_INFO_IT;



typedef struct _MATRIX_INPUT_INFO
{
	string 				MatrixID;		//矩阵ID
	string				strMatrixName;	//矩阵名称
	UINT				InputID;
	UINT				InputCnt;
	UINT				OutputID;
	UINT				OutputCnt;
	string 				strDevID;		//设备ID（TVS-ID或矩阵ID）	
	string				strDevName;		//设备名称（TVS-名称或矩阵名称）
	int 				DevPortID;			//端口ID（对应矩阵的几号端口或者TVS的几号屏）
	TVW_DEVICE_TYPE     deviceType;		//设备类型（矩阵或TVS）
	DECODE_CARD_TYPE	CardType;		//针对TVS的解码类型
}MATRIX_INPUT_INFO,*PMATRIX_INPUT_INFO;

typedef std::map<int, MATRIX_INPUT_INFO> MAP_MATRIX_INPUT_INFO;//此KEY由矩阵ID*1000 + inputID 组成
typedef MAP_MATRIX_INPUT_INFO::iterator  MAP_MATRIX_INPUT_INFO_IT;


typedef struct tagGroupTempAdminItem
{
	std::string channelid;
	std::string channelname;
	std::string groupid;
	std::string groupname;
	std::string tempgroupid;
}GROUPTEMP,*PGROUPTEMP;

typedef struct tagTmpChannel
{
	std::string channelid;
	int status;
	std::string tempgroupid;
}TMPCHANNEL,*PTMPCHANNEL;

typedef std::map<HTREEITEM,TMPCHANNEL*>			Map_TmpGrupChnl;


//-------------------------------------------------------------------------------------