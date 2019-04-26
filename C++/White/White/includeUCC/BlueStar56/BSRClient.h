/********************************************************************
*	Copyright (C) 2008 - BlueStar. All Rights Reserved
*
*	Date Created:2008-01-01
*	Filename	:BSRClient.h
*	Author		:BlueStar luoyanhua
*	Description	:BlueStar DVRClient SDK interface functions declare
*********************************************************************/
#ifndef __BLUESTAR_BSRCLIENT_INTERFACE_H__
#define __BLUESTAR_BSRCLIENT_INTERFACE_H__

#include <windows.h>

#ifdef __cplusplus
#define BSRCLIENT_API extern "C"
#else
#define BSRCLIENT_API
#endif

#ifndef BSRCLIENT_STDCALL
#define BSRCLIENT_STDCALL __stdcall
#endif

#ifndef BSCP_ENUM_VALUE

//BSCP错误码
typedef enum
{
	//协议错误码(1-31)===================>
	BSCPERR_BSCP_NONE				= 0,		//命令执行成功无错误
	BSCPERR_BSCP_UNKNOW_CMDTYPE,				//无法识别的命令类型
	BSCPERR_BSCP_UNKNOW_CMDSUBTYPE,				//无法识别的命令子类型
	BSCPERR_BSCP_UNKNOW_CMDOPT,					//无法识别的命令操作类型
	BSCPERR_BSCP_UNKNOW_CMDCODE,				//无法识别的命令码
	BSCPERR_BSCP_UNSUPPORT_CMD,					//不支持的命令
	BSCPERR_BSCP_CMD_DENIED,					//不具备命令执行权限
	BSCPERR_BSCP_CMD_UNLOGIN,					//未登录而执行该命令
	BSCPERR_BSCP_CMD_LOCKED,					//该命令已加锁
	BSCPERR_BSCP_CMD_TIMEOUT,					//操作超时
	BSCPERR_BSCP_POINTER_NULL,					//输入指针为空
	BSCPERR_BSCP_INPUTDATA_ERROR,				//输入的数据错误
	BSCPERR_BSCP_CMD_INPUTPARAM,				//输入参数错误(无效)
	BSCPERR_BSCP_CMD_INPUTLEN,					//输入数据长度错误
	BSCPERR_BSCP_CMD_SYSREBOOT,					//系统需要重新启动
	BSCPERR_BSCP_CMD_MAXTRANS,					//已超过最大传输通道数
	BSCPERR_BSCP_CMD_CREATETRANS,				//创建传输通道错误
	BSCPERR_BSCP_CMD_VERUNSUPPORT,				//版本不支持该命令
	BSCPERR_BSCP_CMD_DEVUNSUPPORT,				//此设备不支持该命令
	BSCPERR_BSCP_CMD_TRANSNOTEXIST,				//传输通道未建立
	BSCPERR_BSCP_CMD_RC5ERR,					//RC5加密密码错误

   //<===================协议错误码

   //=====================新加错误码
   BSR_ERR_CMD_RECORDWITHD1,              //21.正在使用D1画质录像，不能传子码流。CHQ*/
   BSR_ERR_CMD_WAITRECORDWITHD1,          ///*22.正在使用D1画质预录，不能传子码流。CHQ*/
   BSR_ERR_CMD_TRANSWITHD1,               ///*23.正在使用D1画质进行主码流传输，不能传子码流。changqing*/
   BSR_ERR_CMD_IDELWITHD1,                ///*24.正在使用D1画质进行空闲编码，不能传子码流。changqing*/
   BSR_ERR_CMD_TRANS_GOINGON,             ///*25.正在进行子码流传输，不能修改子码流参数*/

   //新加错误码结束
	

	BSCPERR_BSCP_CMD_INNER			= 32,		//内部错误起始编号

	//系统管理错误码(33-79)===================>
	BSCPERR_SYS_TIME_RECORDBACKUP,				//录像或者备份状态下不能设置时间
	BSCPERR_SYS_TIME_MODEERR,					//无效时间或者时间格式错误
	BSCPERR_SYS_SYSBUSYING,					    //系统忙
	//<===================系统管理错误码

	//用户管理错误码(80-119)===================>
	BSCPERR_USR_INVLADNAME			= 80,		//无效的用户名
	BSCPERR_USR_INVLADKEY,						//无效的密码
	BSCPERR_USR_LOGININVLAD,					//用户名,密码验证无效
	BSCPERR_USR_LOGINMAXNUM,					//已超过最大用户登录数
	BSCPERR_USR_LOGINLOGINED,					//此用户已经登录
	BSCPERR_USR_LOGOUTPARAM,					//注销参数错误
	BSCPERR_USR_INVALDUSRID,					//无效的用户编号
	BSCPERR_USR_LOGOUTED,						//当前已处于登出状态
	BSCPERR_USR_KICKSELFERR,					//不能踢出自己
	BSCPERR_USR_DELADMIN,						//不能修改或者删除超级管理员
	BSCPERR_USR_DELCURUSR,						//不能删除或修改当前登录用户
	BSCPERR_USR_OVERID,							//超过最大用户编号
	BSCPERR_USR_EXIST,							//此用户已存在
	BSCPERR_USR_NOTEXIST,						//此用户不存在
	BSCPERR_USR_FULL,							//用户已满
	BSCPERR_USR_NOTADMIN,						//没有操作权限
	//<===================用户管理错误码

	//日志管理错误码(120-159)===================>
	BSCPERR_LOG_NOTEXIST			= 120,		//日志不存在
	//<===================日志管理错误码

	//录像/显示/放像错误码(160-199)===================>
	BSCPERR_REC_NOTSETPARA			= 160,		//录像状态不能设置参数
	BSCPERR_REC_NOTSETPATROL			= 161,		//回放和多画面不能设置轮巡
	//<===================录像/显示/放像错误码

	//告警管理错误码(200-239)===================>
	//<===================告警管理错误码

	//磁盘存储相关管理错误码(240-279)===================>
	BSCPERR_DISK_NOTEXIST			= 240,		//硬盘不存在
	BSCPERR_DISK_OVERNUMBER,					//大于录像机的硬盘数
	BSCPERR_DISK_SLEEPERR,						//休眠失败
	BSCPERR_DISK_WAKEERR,						//唤醒失败
	BSCPERR_DISK_FORMATERR,						//硬盘格式化错误
	BSCPERR_DISK_RECORDMODE,					//录像盘的模式设置错误
	BSCPERR_DISK_BACKUPRECORDPLAY,				//在播放备份录像的时候不能对硬盘进行设置
	BSCPERR_DISK_NOTCHANAGERECORDDISK,			//不能更改当前录像盘的状态
	BSCPERR_DISK_NOTCHANAGEBACKUPDISK,			//不能更改当前备份的状态
	BSCPERR_DISK_DISKMODENUMBERERROR,			//硬盘状态值输入不合法
	BSCPERR_DISK_DISKMODEERROR,					//硬盘的状态错误
	BSCPERR_DISK_DISKISBACKUPDISK,				//备份盘不能作为录像盘
	BSCPERR_DISK_SPEEDERR,						//硬盘速度测试失败
	BSCPERR_DISK_WRITEERR,						//硬盘写测试失败
	BSCPERR_DISK_GETTIMERBACKUPERR,				//获取定时备份失败
	BSCPERR_DISK_SETTIMERBACKUPERR,				//设置定时备份失败
	BSCPERR_DISK_BACKUPBUSY,					//备份繁忙,备份失败
	BSCPERR_DISK_NOTFORMAT,						//磁盘没有格式化
	BSCPERR_DISK_TIMERBACKUP_TIMEEER,			//定时备份时间设置错误
	BSCPERR_DISK_TIME_MODEERR,					//时间格式设置不对
	BSCPERR_DISK_TIME_STARTBIGEND,				//起始时间大于结束时间
	BSCPERR_DISK_CHANNEL_SETERR,				//通道设置错误
	BSCPERR_DISK_FIELD_STARTBIGEND,				//文件起始ID大于结束ID
	BSCPERR_DEBUG_TASKINFOERR,					//获取任务信息失败
	BSCPERR_DEBUG_GETDPERR,						//获取DP信息失败
	BSCPERR_DEBUG_REBOOTTASKERR,				//重启任务失败	
	BSCPERR_CTRL_SETDVRCTLERR,					//设置DVRCTRL失败	
	BSCPERR_DISK_OVERCHANNEL,					//大于录像机的通道数
	BSCPERR_DISK_QUALITYERR,					//画质错误
	BSCPERR_USR_MODIPASS,						//当前用户只能修改密码
	//<===================磁盘存储相关管理错误码

	//网络相关管理错误码(280-319)===================>
	BSCPERR_NET_SENDEMAILL			= 280,		//邮件发送失败
	//<===================网络相关管理错误码

	//数据传输管理错误码(320-359)===================>
	BSCPERR_TRANS_RTPCALL			= 320,		//调用RTP错误 
	BSCPERR_TRANS_BIGERENDTIME,					//时间查询时,起始时间大于结束时间
	BSCPERR_TRANS_BIGERNOWTIME,					//时间查询时,起始时间大于当前时间
	BSCPERR_TRANS_RECORDTYPE,					//录像类型错误
	BSCPERR_TRANS_CHANNEL,						//通道错误
	BSCPERR_TRANS_PAGEERR,						//按页查询输入页错误
	BSCPERR_TRANS_NODISK,						//磁盘不存在
	BSCPERR_TRANS_NORECORD,						//不存在录像段
	BSCPERR_TRANS_CHANNELERR,					//通道错误
	BSCPERR_TRANS_DISKINFOERR,					//磁盘信息错误
	BSCPERR_TRANS_BEFOREFILERES,				//=====
	BSCPERR_TRANS_DATATYPE,						//数据类型错误
	BSCPERR_TRANS_PROTOCOL,						//协议错误
	BSCPERR_TRANS_TIMEOUT,						//超时
	BSCPERR_TRANS_STREAMOVERNUM,				//视频流已满
	BSCPERR_TRANS_NO_PERMISSION,				//传输层错误码,不允许打开流
	BSCPERR_TRANS_ENCODER_FULL,					//传输层错误码,编码器错误
	BSCPERR_TRANS_NULL_STREAM,					//传输层错误码,流已满
	BSCPERR_TRANS_ERROR,						//传输层错误码,打开错误
	BSR_ERR_TRANS_TAGSEARCH_FAIL,				//TAG检索失败
	BSR_ERR_TRANS_CHANVIEWERR,					//客户端试图连接无法预览的通道

	//345
	//<===================数据传输管理错误码

	//外设控制管理错误码(360-399)===================>
	BSCPERR_CTRL_UARTWRITEFPERR		= 360,		//======
	BSCPERR_CTRL_INPUTKEYERR,					//输入键值失败
	BSCPERR_CTRL_SENDKEYERR,					//发送键值失败
	BSCPERR_CTRL_GETDVRCTLERR,					//获取DVRCTRL失败
	BSCPERR_CTRL_1604EERR,						//控制1604E错误
	BSCPERR_CTRL_4004ERR,						//控制4004E错误
	//<===================外设控制管理错误码

	//从CPU控制错误码(400-439)===================>
	//<===================从CPU控制错误码

	//调试错误码(440-999)===================>
	//<===================调试错误码

	//本地错误码(1000-1300)===================>
	BSCPERR_SDK_NOINIT 				= 1000,		//没有初始化
	BSCPERR_SDK_REINIT,							//重复初始化
	BSCPERR_SDK_COPYRIGHT,						//版权错误,未经授权的错误版本
	BSCPERR_SDK_RELOGIN,						//当前连接重复登录
	BSCPERR_SDK_SESSIONEXCEPT,					//当前会话发生异常断开
	BSCPERR_SDK_ERRUSERHANDLE,					//无效用户句柄
	BSCPERR_SDK_ERRREALHANDLE,					//无效预览句柄
	BSCPERR_SDK_ERRDOWNHANDLE,					//无效下载句柄
	BSCPERR_SDK_ERRUPGRADEHANDLE,				//无效升级句柄
	BSCPERR_SDK_ERRVOICEHANDLE,					//无效对讲句柄
	BSCPERR_SDK_ERRALARMHANDLE,					//无效报警句柄
	BSCPERR_SDK_ERRLOGFINDHANDLE,				//无效日志查询句柄
	BSCPERR_SDK_ERRFILEFINDHANDLE,				//无效文件查询句柄
	BSCPERR_SDK_ERRWINDOWHANDLE,				//无效的窗口句柄
	BSCPERR_SDK_ALLOCRES,						//资源分配错误
	BSCPERR_SDK_CREATETHREAD,					//线程创建失败
	BSCPERR_SDK_INITSOCK,						//SOCKET初始化错误
	BSCPERR_SDK_CREATESOCKET,					//创建SOCKET出错
	BSCPERR_SDK_SETSOCKET,						//设置SOCKET出错
	BSCPERR_SDK_SOCKETLISTEN,					//SOCKET监听失败
	BSCPERR_SDK_SOCKETCLOSE,					//SOCKET被关闭
	BSCPERR_SDK_ALARMON,						//已开启报警接收
	BSCPERR_SDK_ALARMOFF,						//未开启报警接收
	BSCPERR_SDK_PATHERROR,						//路径错误
	BSCPERR_SDK_CREATEFILE,						//创建文件出错
	BSCPERR_SDK_READFILE,						//读取文件出错

	BSCPERR_SDK_CONNECT				= 1050,		//连接服务器失败
	BSCPERR_SDK_SENDDATA,						//向服务器发送数据失败
	BSCPERR_SDK_RECVDATA,						//从服务器接收数据失败
	BSCPERR_SDK_RECVTIMEOUT,					//从服务器接收数据超时
	BSCPERR_SDK_CMDTIMEOUT,						//命令执行超时
	BSCPERR_SDK_NOENOUGHBUF,					//缓冲区太小
	BSCPERR_SDK_ERRORDATA,						//传送的数据有误(校验失败)

	BSCPERR_SDK_PLAYERERR			= 1100,		//播放器中错误
	BSCPERR_SDK_OPENSOUND,						//声音开启失败
	BSCPERR_SDK_AUDIOMODE,						//声卡模式错误
	BSCPERR_SDK_DDRAWNOSUPPORT,					//本地显卡不支持
	//<===================本地错误码(1000-1300)

	BSCPERR_VALIDFLAG,							//无效错误码标识
	BSCPERR_UNKOWN_ERROR			= 0xFFFF	//未知错误
}BSRCLIENT_ERRCODE;

//DVR设备类型
typedef enum
{
	DVRTYPE_6001	= 6001,		//单路
	DVRTYPE_6003	= 6003,		//ATM(仅有3路同时可用,2/4通道互斥,加钞出钞互斥)
	DVRTYPE_6004	= 6004,		//4路(有可能是ATM的4路)
	DVRTYPE_6008	= 6008,		//8路
	DVRTYPE_6012	= 6012,		//12路
	DVRTYPE_6016	= 6016,		//16路
	DVRTYPE_UNKNOWN	= 6255		//未知类型
}BSRCLIENT_DVRTYPE;

//异常消息WPARAM值
typedef enum
{
	EXCEPTMSG_CONNECT		= 0x8001,	//会话连接异常,		LPARAM为hUserHandle
	EXCEPTMSG_SENDDATA		= 0x8002,	//数据发送异常,		LPARAM为hUserHandle
	EXCEPTMSG_ALARM			= 0x8003,	//报警异常,			LPARAM为hUserHandle

	EXCEPTMSG_PREVIEW		= 0x8011,	//网络预览异常,		LPARAM为hRealHandle
	EXCEPTMSG_VOICE			= 0x8012,	//语音对讲异常,		LPARAM为hRealHandle
	EXCEPTMSG_PREVALARMINT	= 0x8013,	//网络预览被报警通道中断(针对ATM),LPARAM为hRealHandle

	EXCEPTMSG_DOWNLOAD		= 0x8021,	//网络下载异常,		LPARAM为hDownHandle
	EXCEPTMSG_NETPLAYBACK	= 0x8022,	//网络回放异常,		LPARAM为hPlayHandle

	EXCEPTMSG_DOWNFINISH	= 0x8031,	//网络下载完毕,		LPARAM为hDownHandle
	EXCEPTMSG_PLAYFINISH	= 0x8032,	//网络回放完毕,		LPARAM为hPlayHandle

	EXCEPTMSG_UPGRADERROR	= 0x8041,	//升级发送异常,		LPARAM为hUpgradeHandle
	EXCEPTMSG_UPGRADEFINISH	= 0x8042	//升级发送完毕,		LPARAM为hUpgradeHandle
}BSRCLIENT_EXCEPTMSG;

//阈值定义
typedef enum
{
	MAX_LOGIN_NUM				= 5,		//最大客户端登录数
	MAX_USERS_NUM				= 10,		//最大用户数
	MAX_STREAM_NUM				= 18,		//最大传输流数
	MAX_CHAN_NUM				= 16,		//最大模拟通道数
	MAX_DISK_NUM				= 8,		//最大硬盘数
	MAX_ONEPAGEFILE_SIZE		= 50,		//单页最大文件个数
	MAX_ONEPAGELOG_SIZE			= 50,		//单页最大日志条数
	MAX_CRUISEDOT_NUM 			= 99,		//最大巡航点个数
	MAX_CHECKPOINTS_NUM			= 16,		//最大测试点个数
	MAX_ALARMIN_NUM				= 16,		//最大报警输入数
	MAX_STRINGLEN				= 16,		//最大字符串长度
	MAX_ALARMUPLOAD				= 512,		//最大报警上传路数
	MOTION_DETECTSEC_NUM		= 4,		//移动帧测检测时段数
	MULTI_DETECT_NUM			= 4,		//侦测区域数

	MAX_CHANNUM_ATM				= 4,		//ATM机最大通道数
	MAX_DISKNUM_ATM				= 4,		//ATM机最大磁盘数
  FOUR_DISK_MAX   = 4,
  FOUR_CHAN_MAX   = 4,
}BSRCLIENT_MAXVALUEDEF;

//传输端口
typedef enum
{
	BSTP_SERVICE_DEFTRANSPORT	= 3720,		//默认现场视频传输端口
	BSCP_SERVICE_DEFCTRLPORT	= 3721,		//默认命令控制端口
	BSTP_SERVICE_DEFDATAPORT	= 3722,		//默认数据传输端口(下载升级等)
	BSCP_SERVICE_DEFALARMPORT	= 2134,		//默认报警监听端口
	BSCP_SERVICE_DEFAUDIOPORT	= 8000,		//旧对讲机制的对讲端口
	BSTP_SERVICE_NEWAUDIOPORT	= 3720,		//新对讲机制的对讲端口(新版本已与现场传输统一)
}BSRCLIENT_DEVICEPORT;

#endif

//无效值定义
typedef enum
{
	INVALID_STREAMID	= -2,				//无效流ID号
	INVALID_SESSIONID	= -2				//无效SESSION号
}BSRCLIENT_INVALIDVALUE;

//设备面板按键
typedef enum
{
	BSRKEY_RECORD			= 1,		//开始录像(数字1)
	BSRKEY_PLAY				= 2,		//开始播放(数字2)
	BSRKEY_STOP				= 3,		//停止录像/播放(数字3)
	BSRKEY_FASTBACK			= 4,		//快退(数字4)
	BSRKEY_FASTFORWARD		= 6,		//快进(数字5)
	BSRKEY_PAUSE			= 5,		//暂停(数字6)
	BSRKEY_NEXTFRAME		= 8,		//逐帧播放(数字7)
	BSRKEY_PREVIOUS			= 9,		//上一段(数字8)
	BSRKEY_NEXT				= 16,		//下一段(数字9)
	BSRKEY_TOP				= 7,		//回到该录像段首(数字0)
	BSRKEY_CLEARALARM		= 19,		//报警清除
	BSRKEY_CHANGEQUALITY	= 20,		//画质选择(+)
	BSRKEY_DISPLAY			= 21,		//信息显示(-)

	BSRKEY_MENU				= 22,		//菜单
	BSRKEY_LEFT				= 23,		//方向-左
	BSRKEY_RIGHT			= 24,		//方向-右
	BSRKEY_UP				= 25,		//方向-上
	BSRKEY_DOWN				= 32,		//方向-下

	BSRKEY_CHAN1			= 33,		//通道1
	BSRKEY_CHAN2			= 34,		//通道2
	BSRKEY_CHAN3			= 35,		//通道3
	BSRKEY_CHAN4			= 36,		//通道4
	BSRKEY_CHAN5			= 37,		//通道5
	BSRKEY_CHAN6			= 38,		//通道6
	BSRKEY_CHAN7			= 39,		//通道7
	BSRKEY_CHAN8			= 40,		//通道8
	BSRKEY_CHAN9			= 41,		//通道9
	BSRKEY_CHAN10			= 42,		//通道10
	BSRKEY_CHAN11			= 43,		//通道11
	BSRKEY_CHAN12			= 44,		//通道12
	BSRKEY_CHAN13			= 45,		//通道13
	BSRKEY_CHAN14			= 46,		//通道14
	BSRKEY_CHAN15			= 47,		//通道15
	BSRKEY_CHAN16			= 48,		//通道16

	BSRKEY_MULTISHOW		= 49,		//组合画面
	BSRKEY_TIMER			= 128,		//定时
	BSRKEY_MOTION			= 129,		//移动侦测
	BSRKEY_DEFENCE			= 130,		//布/撤防
	BSRKEY_PTZ				= 133,		//云台控制
	BSRKEY_KEYLOCK			= 135,		//键盘锁
}BSRCLIENT_KEYBOARD;

//网络传输方式,如果SDK不支持所选方式则自动转换为TCP模式
typedef enum 
{
	PROTOCOL_TCPMODE		= 1,		//TCP方式
	PROTOCOL_UNICAST		= 2,		//UDP单播(暂不支持)
	PROTOCOL_MULTICAST		= 3,		//UDP组播(暂不支持)
	PROTOCOL_RTPMODE		= 4,		//RTP方式
	PROTOCOL_RESERVED					//保留
}BSRCLIENT_PROTOCOL;

//现场视频流参数类别(GET/SET)
typedef enum
{
	STREAMPARA_CHANSWITCH	= 1,		//通道切换(1~16; 0-当前现场; <0 表示现场切换到相应的多画面组合)
	STREAMPARA_LEISURE		= 2,		//空闲时间,0为不超时(0~3600)
	STREAMPARA_DIAGKEEP		= 3,		//会话结束后是否保持(0-不保持 1-保持)
	STREAMPARA_BANDWIDTH	= 4,		//带宽(字节/秒)
	STREAMPARA_SWITCHSTREAM	= 5,		//0-主码流, 1-子码流[设置时无参,填0即可]

	STREAMPARA_VCTRL		= 10,		//视频的打开/关闭
	STREAMPARA_VFRAMERATE	= 11,		//视频帧率(PAL:1-25, NTSC:1-30)
	STREAMPARA_VBITRATE		= 12,		//视频码率(64~4096 KBPS)
	STREAMPARA_VRESOLUTION	= 13,		//视频分辨率(0-CIF, 1-DCIF, 2-1/2D1, 3-2CIF 4-D1)

	STREAMPARA_LVBO			= 14,		//滤波(0, 1, 2)
	STREAMPARA_IFINTERVAL	= 15,		//I帧间隔(25-200)
	STREAMPARA_LIANGHUA		= 16,		//量化值(2-31)

	STREAMPARA_MAKEKEYFRAME	= 20,		//请求强制生成关键帧[无参,填0即可]
	STREAMPARA_BRIGHTNESS	= 21,		//视频亮度(0~255)
	STREAMPARA_CONTRAST		= 22,		//视频对比度(0~255)
	STREAMPARA_HUES			= 23,		//视频色度(0~255)

	STREAMPARA_AUDIOCTRL	= 30,		//伴音的打开/关闭(0-关闭 1-打开)
	STREAMPARA_AUDIOSAMPLE	= 31,		//伴音的采样率
	STREAMPARA_AUDIORATE	= 32,		//伴音的码率

	STREAMPARA_PTZCTRL		= 51,		//云台PTZ控制(高16位为命令值BSRCLIENT_PTZCTRLPARABYLOCAL,低16位为0)
	STREAMPARA_PTZPRESET	= 52,		//云台预置位(1-n)
	STREAMPARA_PTZPATH		= 53,		//云台巡航路径(0-n, 0为停止)
	STREAMPARA_PTZCRUISE	= 54,		//云台轨迹 (0-1)
	STREAMPARA_PTZAUX		= 55,		//云台辅助(1启动灯光,2停止灯光,3启动雨刷,4停止雨刷...)
}BSRCLIENT_STREAMPARA;

//语音对讲流参数类别(GET/SET)
typedef enum
{
	VOICEPARA_IDLETIME		= 2,		//空闲时间,0为不超时(0~3600)
	VOICEPARA_DIAGKEEP		= 3,		//是否保持会话(0不保持,1保持)
	VOICEPARA_BITRATE		= 4,		//码率(BYTE/S),该属性只读
	VOICEPARA_SAMPLE		= 5,		//采样率(KHZ),该属性只读 2(2k) | 4(4k) | 8(8k) | 16(16k) | 32(32k)
	VOICEPARA_ENCODE		= 6,		//编码格式,该属性只读 2(mp2) | 3(mp3) | 9(g.729)
	VOICEPARA_VIDEOON		= 10		//视频的打开关闭,0关闭,1打开
}BSRCLIENT_VOICEPARA;

//码流回调数据类型
typedef enum
{
	REALDATATYPE_SYSHEADER		= 1,	//系统文件头
	REALDATATYPE_VOICEDATA		= 2,	//对讲流数据(上行)
	REALDATATYPE_SCENEDATA		= 3,	//现场流数据
	REALDATATYPE_DOWNDATA		= 4,	//下载流数据
}BSRCLIENT_REALDATATYPE;

//下载进度错误值
typedef enum
{
	DOWNERRFLAG_TIMEOUT		= 1051,		//接收超时
	DOWNERRFLAG_SOCKERR		= 1052,		//SOCKET错误
	DOWNERRFLAG_SOCKCLOSE	= 1053,		//SOCKET被对方强制关闭
	DOWNERRFLAG_UNKNOWN		= 1054		//未知错误
}BSRCLIENT_DOWNERRFLAG;

//DVR消息回调类型
typedef enum
{
	UPMSGTYPE_ALARMINFO	= 0x1000,		//上传报警信息
	UPMSGTYPE_TRADEINFO	= 0x2000		//ATMDVR上传交易信息(暂未实现)
}BSRCLIENT_UPMSGTYPE;

//远程回放控制
typedef enum
{
	PLAYBACKCTRL_NORMAL			= 0x00,	//正常播放
	PLAYBACKCTRL_PAUSE,					//暂停播放
	PLAYBACKCTRL_ONEBYONE,				//单帧播放
	PLAYBACKCTRL_1OF2TIMES,				//1/2速(慢进)
	PLAYBACKCTRL_1OF4TIMES,				//1/4速(慢进)
	PLAYBACKCTRL_1OF8TIMES,				//1/8速(慢进)
	PLAYBACKCTRL_1OF16TIMES,			//1/16速(慢进)
	PLAYBACKCTRL_2TIMES,				//2倍速(快进)
	PLAYBACKCTRL_4TIMES,				//4倍速(快进)
	PLAYBACKCTRL_8TIMES,				//8倍速(快进)
	PLAYBACKCTRL_16TIMES,				//16倍速(快进)
	PLAYBACKCTRL_MAXSPEED,				//不限速
	PLAYBACKCTRL_IFRAME,				//I帧播放
	PLAYBACKCTRL_OPENSOUND,				//打开声音
	PLAYBACKCTRL_CLOSESOUND,			//关闭声音
	PLAYBACKCTRL_SETVOLUME,				//调节音量(音量值0-100之间)
	PLAYBACKCTRL_SETPLAYPOS,			//改变文件回放的进度(千分比0-1000)
	PLAYBACKCTRL_GETPLAYPOS,			//获取文件回放的进度(千分比0-1000,-1为无效,>1000为异常,BSRCLIENT_DOWNERRFLAG)
	PLAYBACKCTRL_GETPLAYEDTIME,			//获取当前已经播放的时间
	PLAYBACKCTRL_GETTOTALTIME,			//获取当前播放范围的总时间长度,单位秒
	PLAYBACKCTRL_GETPLAYEDFRAMES,		//获取当前已经播放的帧数
	PLAYBACKCTRL_GETBAND,             //获取远程播放带宽
	PLAYBACKCTRL_SETBAND,             //设置远程播放带宽
	PLAYBACKCTRL_INVALID		= 0xFF,	//无效控制码
}BSRCLIENT_PLAYBACKCTRL;

//报警事件类别(本地报警接收)
//报警输出,视频丢失,视频恢复,视频遮挡,移动侦测,磁盘错误等都包含channel的信息,对应的是具体的通道号或磁盘号
typedef enum
{
	ALARM_EVENT_REPORT_IP		= 0x2112,	//报告IP地址
	ALARM_EVENT_ALARM_OUT		= 0x2113,	//报警输出(报警端子的报警输入)
	ALARM_EVENT_HDISK_ERROR		= 0x2114,	//磁盘错误
	ALARM_EVENT_VIDEO_LOST		= 0x2115,	//视频丢失
	ALARM_EVENT_EVERYTHING_OK	= 0x2116,	//报告正常
	ALARM_EVENT_VIDEO_RESTORE	= 0x2117,	//视频恢复
	ALARM_EVENT_CONTROL_1604E	= 0x2118,	//专门控制1604E
	ALARM_EVENT_CONTROL_4000	= 0x2119,	//专门控制4000系列录像机本身
	ALARM_EVENT_CTRL_4004		= 0x211A,	//专门控制4000系列上的云台设备
	ALARM_EVENT_VIDEO_COVER		= 0x211B,	//视频遮挡
	ALARM_EVENT_MOTION_DETECT	= 0x211C,	//移动侦测
	ALARM_EVENT_NET_BREAK		= 0x211D,	//网络断,6.0中新增,通道值=0表示故障,其余表示恢复正常
	ALARM_EVENT_FUN_BREAK		= 0x211E,	//风扇错误,6.0中新增,通道值=0表示故障,其余表示恢复正常
	ALARM_EVENT_TIME_ERROR		= 0x211F,	//时间错,与通道无关,6.0中新增
	ALARM_EVENT_CHANNEL_ERROR	= 0x2120,	//通道错,与通道无关,6.0中新增
	ALARM_EVENT_DISK_FULL		= 0x2121,	//磁盘满,与通道无关,6.0中新增
	ALARM_EVENT_INVALID_LINK	= 0x2122,	//无效的链接,与通道无关,6.0中新增
	ALARM_EVENT_CHANNEL_CHANGE	= 0x2123,	//ATM机器2/4通道切换
	ALARM_EVENT_REQUEST_TALK	= 0x2124,	//DVR请求对讲
	ALARM_EVENT_CARDNO_ALM		= 0x2125,	//卡号报警
	ALARM_EVENT_ALG_ALM			= 0x2126	//智能报警
}BSRCLIENT_NETALARMEVENT;

//讯美实时流回调函数

typedef void (CALLBACK *fRealDataXM)( HANDLE hCallHandle, DWORD dwDataType, BYTE* lpHeadBuffer, DWORD dwHeaderSize, 
                                                                        BYTE*lpBuffer,  DWORD dwSize, LPVOID lpVoid );

//码流回调(现场实时/远程回放)
typedef void (CALLBACK *CLIENTDATACALLBACKFUN)(HANDLE hCallHandle, DWORD dwDataType, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//显示窗口句柄HOOK回调
typedef void (CALLBACK *CLIENTHWNDHOOKFUN)(HANDLE hRealHandle, HWND *hWnd, LPVOID lpVoid);

//字符叠加回调
typedef void (CALLBACK *CLIENTTITLEDRAWFUN)(HANDLE hRealHandle, HDC hDC, DWORD dwWidth, DWORD dwHeight, LPVOID lpVoid);

//录像机消息回调[包含报警信息]函数(以IP区分设备)
typedef BOOL (CALLBACK *MESSAGECALLBACKFUN)(LONG nCommand, LPCSTR lpszDVRIP, BYTE *lpBuffer, DWORD dwSize, LPVOID lpVoid);

//音频数据采集回调
typedef void (CALLBACK *VOICEDATACALLBACKFUN)(BYTE *lpDataBuffer, DWORD dwSize, DWORD dwType, LPVOID lpVoid);

//音频回调/注册数据类型
typedef enum
{
	BSRWAVETYPE_PCM		= 0,	//编码前的PCM数据
	BSRWAVETYPE_ENCODE,			//编码后数据
	BSRWAVETYPE_BOTH			//两者(此参数可用于音频采集注册函数)
}BSRCLIENT_WAVEDATATYPE;

//音频采集参数
typedef struct tagBSRCLIENT_WAVEPARA
{
	WORD wBits;					//位
	WORD wChannels;				//声道
	DWORD dwSamplesPerSec;		//采样率
}BSRCLIENT_WAVEPARA, *PBSRCLIENT_WAVEPARA;
//所有1700系列DVR音频采集格式如下:
//WORD wBits = 16;
//WAVEFORMATEX wfx = {0};
//wfx.wFormatTag = WAVE_FORMAT_PCM;
//wfx.nChannels = 2;
//wfx.nSamplesPerSec = 16000;
//wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wBits / 8;
//wfx.nBlockAlign = wBits * wfx.nChannels / 8;
//wfx.wBitsPerSample = wBits;
//wfx.cbSize = 0;

//音频采集数据结构
typedef struct tagBSRCLIENT_AUDIOWAVEIN
{
	BSRCLIENT_WAVEPARA *lpWFX;				//音频采集格式指针(为NULL表示使用默认值)

	VOICEDATACALLBACKFUN lpfnNotEncode;		//编码前数据回调指针
	LPVOID lpVoidNotEncode;					//编码前数据回调上下文

	VOICEDATACALLBACKFUN lpfnEncoded;		//编码后数据回调指针
	LPVOID lpVoidEncoded;					//编码后数据回调上下文
}BSRCLIENT_AUDIOWAVEIN, *PBSRCLIENT_AUDIOWAVEIN;
//所有1700系列DVR音频采集格式如下:
//WORD wBits = 16;
//WAVEFORMATEX wfx = {0};
//wfx.wFormatTag = WAVE_FORMAT_PCM;
//wfx.nChannels = 2;
//wfx.nSamplesPerSec = 16000;
//wfx.nAvgBytesPerSec = wfx.nChannels * wfx.nSamplesPerSec * wBits / 8;
//wfx.nBlockAlign = wBits * wfx.nChannels / 8;
//wfx.wBitsPerSample = wBits;
//wfx.cbSize = 0;

//DVR时间日期格式
typedef struct tagBSCP_DATATIME
{
	USHORT year;			//年
	USHORT month;			//月
	USHORT day;				//日
	USHORT hour;			//时
	USHORT minute;			//分
	USHORT second;			//秒
	USHORT week;			//周
	USHORT res;				//保留
}BSCP_DATATIME, *PBSCP_DATATIME;

//报警上传回调结构
typedef struct tagBSRCLIENT_ALARMINFO
{
	ULONG nEvent;			//报警事件,BSRCLIENT_NETALARMEVENT
	ULONG nChannel;			//通道号
	ULONG nSrcIP;			//报警端IP(网络序)
	ULONG nSerial;			//序列号(录像机网络编号)
	ULONG nYear;			//年
	ULONG nMonth;			//月
	ULONG nDay;				//日
	ULONG nHour;			//时
	ULONG nMinute;			//分
	ULONG nSecond;			//秒
	CHAR szBuf[24];			//其它信息(如果是卡号报警则这里存放的是卡号的ASSCII码)
}BSRCLIENT_ALARMINFO, *PBSRCLIENT_ALARMINFO;

//当前传输流列表组
typedef struct tagBSRCLIENT_STREAMLISTITEM
{
	LONG nStreamID;			//流ID
	LONG nSessionID;		//会话ID
	LONG nChannel;			//模拟通道
	LONG nProtocol;			//协议类别
	CHAR szSrcIP[16];		//源IP
	LONG nSrcPort;			//源PORT
	CHAR szDstIP[16];		//目的IP
	LONG nDstPort;			//目的PORT
	LONG nStatus;			//流状态UNUSED(0) | OPEN(1) | START(2) | STOP(3) | CLOSE(4)
	LONG nResolve;			//保留
}BSRCLIENT_STREAMLISTITEM, *PBSRCLIENT_STREAMLISTITEM;

//当前传输流列表
typedef struct tagBSRCLIENT_STREAMLIST
{
	BSRCLIENT_STREAMLISTITEM item[MAX_STREAM_NUM];
}BSRCLIENT_STREAMLIST, *PBSRCLIENT_STREAMLIST;

//流系统信息
typedef struct tagBSCP_NETTRANS_STREMINFO
{
	INT streamId;			//流号
	INT sessionid;			//会话ID
	INT channel;			//通道号
	INT switchchannel;		//切换通道号
	INT timeout;			//超时
	INT Iskeep;				//是否保持
	INT BandWidth;			//带宽
	INT bit_rate_type;      //码率类型0:定码率 1:变码率 3:定上限变码流
	INT frame_rate;			//帧率1-25/30
	INT bit_rate;			//视频码率 64-4096 KBPS
	INT filter;				//滤波(无,1,2)
	INT Iframe_interval;	//I帧间隔25-200
	INT quantity;			//量化 2-31
	INT audio_bit_rate;		//音频码率16,32,64 KBPS
	INT media_type;			//码流类型: 0: 混合流;1 :视频流
	INT video_mode;			//压缩视频的大小 0:CIF 1:DCIF 2:1/2D1 3: 2CIF 4:D1
	INT bright;				//亮度
	INT contrast;			//对比度
	INT sature;				//饱和度
	INT states;				//流状态0-unUsed 1-Open 2-start 3-stop 4-busy
	INT audio;				//0-关闭 1-打开
	INT presetno;			//
	INT cruise;				//
	INT pattern;			//
	INT aux;				//
}BSCP_NETTRANS_STREMINFO, *PBSCP_NETTRANS_STREMINFO;

//当前流系统信息列表
typedef struct tagBSRCLIENT_STREAMINFOS
{
	BSCP_NETTRANS_STREMINFO item[MAX_STREAM_NUM];
}BSRCLIENT_STREAMINFOS, *PBSRCLIENT_STREAMINFOS;

//文件查找类别(录像文件类别)
typedef enum 
{
	RECORDFILETYPE_TIMER		= 0,		//定时录像
	RECORDFILETYPE_MOTODETEC	= 1,		//移动侦测
	RECORDFILETYPE_ALARM		= 2,		//报警录像
	RECORDFILETYPE_MAMUAL		= 3,		//手动录像
	RECORDFILETYPE_ALL			= 4,		//全部录像

	RECORDFILETYPE_CARDNO		= 5			//卡号录像(3路ATM支持,此项仅作为结果返回识别,不能作为参数输入)
}BSRCLIENT_RECORDFILETYPE;

//文件检索类别
typedef enum
{
	REMOTESEARCH_GENERAL	= 0,	//常规检索
	REMOTESEARCH_BYCARD,			//卡号检索
	REMOTESEARCH_BYALG,				//智能检索
	REMOTESEARCH_INVALID			//无效码
}BSRCLIENT_FINDTYPEENUM;

//扩展检索条件结构
typedef struct tagBSRCLIENT_REMOTEFINDEX
{
	LONG nChannel;			//通道号(1-N,暂不支持所有通道)
	DWORD dwSearchType;		//查询类别BSRCLIENT_FINDTYPEENUM
	DWORD dwMajorType;		//主类别BSRCLIENT_RECORDFILETYPE
	DWORD dwMinorType;		//子类别(智能查询时,智能类别BSRALGDECT_ALGDECTTYPE)
	DWORD dwDiskNumber;		//磁盘号(1-N,0表示所有盘)
	SYSTEMTIME tBegin;		//起始时间
	SYSTEMTIME tEnd;		//结束时间
	CHAR szCard[32];		//卡号(卡号查询时为卡号字符串,其余查询时此项无效)
	LONG nPageCount;		//页数输出(全盘检索时此项无效)
	LONG nFileCount;		//文件数输出(全盘检索时此项无效)
}BSRCLIENT_REMOTEFINDEX, *PBSRCLIENT_REMOTEFINDEX;

//文件详细信息
typedef struct tagBSCP_NETTRANS_FILEINFO
{
	CHAR name[64];			//文件名
	INT type;				//录像类型(BSRCLIENT_RECORDFILETYPE)
	UINT time;				//开始时间(32位time_t类型)
	INT channel;			//所属通道1-16
	INT disk;				//所属磁盘1-32
	INT cluster;			//所属簇
	UINT period;			//录像时长
	UINT64 size;			//文件大小(Bytes)
}BSCP_NETTRANS_FILEINFO, *PBSCP_NETTRANS_FILEINFO;

//文件列表
typedef struct tagBSRCLIENT_FILELIST
{
	BSCP_NETTRANS_FILEINFO fileinfo[MAX_ONEPAGEFILE_SIZE];	//文件组
}BSRCLIENT_FILELIST, *PBSRCLIENT_FILELIST;

//版本标识
enum 
{
	B61X_TYPE = 0x80000000,//61X类型
	B61X_SUBTYPE_SENSITIVE_AUTO = 0x00000001,//61X类型通道灵敏度自动
	B61X_SUBTYPE_OLNY_QCIF	= 0x00000002,//61X类型只支持QCIF
	B61X_SUBTYPE_OLNY_32K = 0x00000004,//61X类型不支持双码流
};
typedef struct tabBSRCLIENT_VERFLAG
{
	BOOL bNewAlarm;			//新报警结构ATM目前所采用,版本计划中是V6.11之后的都是新的报警结构
	BOOL bNewAudio;			//新对讲机制,V6.10B08D01之后的都是新的对讲机制
	BOOL bSATAType;			//是否SATA机型
	BOOL bAlgType;			//是否支持智能
	BOOL bSubStream;		//是否支持双码流
	BOOL bMultiMask;		//是否多遮盖区域,V6.10B09版本开始支持
	BOOL bArmyIA;       //是否是军队专用DVR
	BOOL bB02IA;        //是否是B02专用DVR   
	BOOL b650B02;       //6.50B02D14及之后版本
	BOOL b61XB0X;       //6.11B01 |6.12B01 |6.12B02 | 6.13b01 | 6.14B01版本
	BOOL b610B09;       //6.10B09
	BOOL bFourMask;     //"V6.14B01D07" "V6.11B01D22" 以及"V6.13B01D15"级之后版本
	BOOL bConduct;		//6.31B01D12（包括）以后支持打架斗殴
  BOOL bIAV640M;
  BOOL bIA_V640;
  BOOL bIA_V640QJ;  //V6.40B01D16之后版本支持

	DWORD b61X;			//V6.11B01D33 V6.11B01D33NXX V6.13B01D31 V6.13B01D31NXX V6.14B01D16 V6.14B01D16NXX支持视频大小CIF QCIF可选，通道灵敏度可选自动

  BOOL bAtm61XX;      //区分atm机支持的协议
  BOOL bAtm614X;      //区分atm机支持的协议,614X支持的协议比612、611、及613版本多一个
  BOOL bAtm615X;

  BOOL bOutputValid;  //判断报警输出时间段是否有效;

  BOOL bUseNewDevId;  //判断是否使用了新的设备标识结构

  BOOL bUseNewDDNS;  //6.30B02D09之后网络配置里添加了金万维DDNS服务;

  BOOL bV64XX; //V6.4版本云台设置
  BOOL bSupportQCIF; //
}BSRCLIENT_VERFLAG, *PBSRCLIENT_VERFLAG;

//设备信息
typedef struct tagBSRCLIENT_DEVICEINFO
{
	ULONG hardwarever;			//硬件版本号

	ULONG typeandmode;			//设备类型
	ULONG subType;				//设备子类型

	ULONG fpStatus;				//录像机状态
	ULONG quality;				//当前画质

	ULONG slaveCPUs;			//从CPU的个数
	ULONG channumber;			//通道数
	ULONG videoNumber;			//视频输入个数
	ULONG maxDiskNumber;		//最大硬盘个数
	ULONG nicNumber;			//网卡数目
	ULONG uartNumber;			//串口数目
	ULONG alarmNumber;			//报警端子数(次字段B09之后版本有效)

	ULONG r_version;			//读_版本号
	ULONG w_version;			//写_版本号

	CHAR sysver[40];			//系统版本号
	ULONG softwarever;			//软件版本号

	ULONG quarter_status;		//当前多画面状态

	SYSTEMTIME stTime;			//系统时间

	BSRCLIENT_VERFLAG verFlag;	//特殊版本标识
}BSRCLIENT_DEVICEINFO, *PBSRCLIENT_DEVICEINFO;

//录像机状态
typedef struct tagBSCP_DVR_DEVICESTAT
{
	UINT fpStatus;							//
	INT netmode;							//
	INT alert;								//
	INT quality;							//画质
	INT fs_mode;							//FS型号
	INT mode;								//
	BSCP_DATATIME net_DateTime;				//
}BSCP_DVR_DEVICESTAT, *PBSCP_DVR_DEVICESTAT;

//录像机信息
typedef struct tagBSCP_DVR_DEVICEINFO
{
	UCHAR ipaddr[4];						//
	UCHAR netmask[4];						//
	UINT fpStatus;							//
	INT netmode;							//
	INT alert;								//
	INT quality;							//
	INT fs_mode;							//
	INT mode;								//
	BSCP_DATATIME net_DateTime;				//

	ULONG r_ip;								//当前连接的IP地址
	INT delay_of_direct_sendto;				//delay值
	INT dqlen;								//Data_Qid中当前的消息数量
	INT dskqlen;							//writedisk_qid中当前的消息数量
	INT cqlen;								//Cmd_Qid中当前的消息数量
	INT misqlen;							//Misc_Qid中当前的消息数量
	INT ctrlqlen;							//msgQueue中当前的消息数量
	ULONG buildid;							//Master BuildID
	ULONG slaveId;							//Slave BuildID
	CHAR version[40];						//当前软件的版本号
	CHAR startup_date[80];					//系统启动日期
	CHAR m_b_date[48];						//主程序的编译日期
	CHAR m_s_date[48];						//Slave程序的编译日期
	USHORT uChannel;						//通道资源状态
	UCHAR message_timeout;					//Message Timeout
	UCHAR current_disk;						//Current disk for Transfer by Net
	// -1 for HddInUseHandle
	//0 - 7 for disk A, B, C, D

	//每一个bit表示一个channel的资源情况
}BSCP_DVR_DEVICEINFO, *PBSCP_DVR_DEVICEINFO;

//设备子类别
typedef enum
{
	DEVSUBTYPE_UNDEFINE	= 0,	//未定义
	DEVSUBTYPE_RECORDER,		//录像机
	DEVSUBTYPE_1604NE,			//1604NE
	DEVSUBTYPE_DECODER,			//解码器
	DEVSUBTYPE_TRANSBOX			//传输盒
}BSRCLIENT_DEVSUBTYPE;

//录像机状态(按位与TRUE)
typedef enum
{
	DEVSTATUS_NETERROR		= 0x80000000,		//网络错误
	DEVSTATUS_FANERROR		= 0x40000000,		//风扇错误

	DEVSTATUS_INTRANSFER	= 0x00200000,		//正在传输
	DEVSTATUS_TRANSFER		= 0x00100000,		//传输启动编码

	DEVSTATUS_WINDOWS		= 0x00080000,		//窗口状态
	DEVSTATUS_HDDERROR		= 0x00040000,		//硬盘错误
	DEVSTATUS_RECPLAY		= 0x00020000,		//
	DEVSTATUS_MOTION		= 0x00010000,		//移动侦测

	DEVSTATUS_RECORD		= 0x00008000,		//录像状态
	DEVSTATUS_PLAYSTATUS	= 0x00004000,		//播放状态
	DEVSTATUS_STOP			= 0x00002000,		//现场状态
	DEVSTATUS_PAUSE			= 0x00001000,		//暂停状态

	DEVSTATUS_FBPLAY		= 0x00000800,		//快退
	DEVSTATUS_FFPLAY		= 0x00000400,		//快进
	DEVSTATUS_STEP			= 0x00000200,		//单帧
	DEVSTATUS_TOP			= 0x00000100,		//本段开始

	DEVSTATUS_ALARM			= 0x00000080,		//报警录像
	DEVSTATUS_BALARM		= 0x00000040,		//报警前
	DEVSTATUS_TIMER			= 0x00000020,		//定时录像
	DEVSTATUS_FULL			= 0x00000010,		//磁盘满

	DEVSTATUS_BACKUP		= 0x00000008,		//备份
	DEVSTATUS_DIALOG		= 0x00000004		//会话
}BSRCLIENT_DEVSTATUS;

//现场预览/语音对讲参数
typedef struct tagBSRCLIENT_CLIENTINFO
{
	LONG lChannel;					//通道号(0-16,0为组合画面,其余则根据机型决定最大路数,-4/-14/-24/-9/-19/-29表示第N种4画面组合,第N种9画面组合)
	BOOL bPullMode;					//模式TRUE-被动, FALSE-主动(目前TCP仅支持被动,RTP仅支持主动)
	UINT nProtocol;					//传输协议类型(BSRCLIENT_PROTOCOL)
	HWND hWnd;						//播放窗口的句柄(如果为NULL表示仅接收数据不播放图像,语音对讲时也可以传NULL)
	DWORD dwDeviceNum;				//播放显示设备,0为默认设备(目前仅支持默认设备)
	WORD wTransPort;				//DVR端传输端口(BSRCLIENT_DEVICEPORT)
	BOOL bSubStream;				//子码流选项,TRUE子码流,FALSE主码流(如果设备不支持子码流则此参数无意义)
	DWORD dwBufFrameNum;			//缓冲帧数,0或无效值则为默认值5帧,有效范围(2-50)
	WORD wLocalPort;				//本地端口(仅用于主动模式,被动模式填0即可)
	LPCSTR lpszLocalIP;				//主动模式本地IP(仅用于主动模式,被动模式填NULL即可)
	CLIENTDATACALLBACKFUN lpfnReal;	//实时码流回调(如无需回调则填NULL即可)
	LPVOID lpRealVoid;				//用户回调上下文
}BSRCLIENT_CLIENTINFO, *PBSRCLIENT_CLIENTINFO;

//子码流参数项
typedef struct tagBSRCLIENT_SUBSTREAMPARAITEM
{
	UCHAR nVideoDIM;				//视频压缩格式,目前固定为0-CIF
	UCHAR nBitrateType;				//码率类型,0-变码率,1-定码率,3-网传定码率, 目前固定为3-网传定码率
	UCHAR nFrameRate;				//帧率1-25,100以上用于窄带传输 如102-1/2,103-1/3依次类推
	UCHAR nIFrameInterval;			//I帧间隔25-200
	UCHAR nFilter;                  //滤波值(0无, 1, 2), 3, 4, 5, 6, 7,后面几个可设置成功但无实际效果
	UCHAR nQuality;					//编码质量,目前固定为6
	UCHAR nMediaType;				//码流类型0-复合流,1-视频流
	UCHAR nQuantity;				//量化值2-31
	UCHAR nAudioBitrate;			//音频码率0-32,1-64(KBPS)
	UCHAR nRes;						//保留位
	USHORT nVideoBitrate;			//视频码率64-1024(KBPS),DVR端默认值为384
}BSRCLIENT_SUBSTREAMPARAITEM, *PBSRCLIENT_SUBSTREAMPARAITEM;

//子码流参数获取/设置(可对某一通道进行设置/获取,也可以对指定通道一次性设置/获取)
typedef struct tagBSRCLIENT_SUBSTREAMPARA
{
	BSRCLIENT_SUBSTREAMPARAITEM subPara[MAX_CHAN_NUM+1];
	ULONG  nChannel;				//设置通道,按位掩码(从低到高1为主画面,2-17为1-16通道)
}BSRCLIENT_SUBSTREAMPARA, *PBSRCLIENT_SUBSTREAMPARA;


//与捕捉器的连接方式
typedef enum
{
	CATCHERLINKTYPE_RS232		= 1,
	CATCHERLINKTYPE_RS485		= 2,
	CATCHERLINKTYPE_RS232HUB	= 3,
	CATCHERLINKTYPE_RJ45		= 4
}BSRCLIENT_CARDCATCHERLINKTYPE;

//卡号协议
typedef enum
{
	//捕捉器连接方式为RJ45时
	CARDPROTOCOL_RJ45_NONE		= 0,	//无
	CARDPROTOCOL_RJ45_XINJIANG	= 1,	//新疆
	CARDPROTOCOL_RJ45_CHENGDU	= 2,	//成都
	CARDPROTOCOL_RJ45_OTHER		= 3,	//其它
	CARDPROTOCOL_RJ45_YINDU		= 4,	//印度
	CARDPROTOCOL_RJ45_NINGXIA	= 5,	//宁夏
	CARDPROTOCOL_RJ45_GELUNBIYA	= 6,	//哥伦比亚
	CARDPROTOCOL_RJ45_SHANGHAI	= 7,	//上海

	//捕捉器连接方式为RS232时
	CARDPROTOCOL_RS232_NONE		= 0,	//无
	CARDPROTOCOL_RS232_GDYT		= 1,	//广电运通
	CARDPROTOCOL_RS232_RILI		= 2,	//日立
	CARDPROTOCOL_RS232_GELUNBIYA= 3,	//哥伦比亚
	CARDPROTOCOL_RS232_YINDU	= 4,	//印度
	CARDPROTOCOL_RS232_NCR		= 5,	//NCR
	CARDPROTOCOL_RS232_LANTIAN	= 6,	//蓝天
}BSRCLIENT_CARDPROTOCOL;

//ATM卡号捕捉器参数
typedef struct tagBSRCLIENT_CARDCATCHERPARA
{
	UCHAR nLinkType;				//与捕获器连接方式BSRCLIENT_CARDCATCHERLINKTYPE
	UCHAR nProtocol;				//卡号协议BSRCLIENT_CARDPROTOCOL
	UCHAR nForwards;				//网络转发内容:0-无,1-卡号
	UCHAR res;						//保留
	UINT nAddr;						//捕获器地址IP地址(网络字节序)或RS485地址
	UINT nPeriod;					//持续时间(录像时长,单位秒)
}BSRCLIENT_CARDCATCHERPARA, *PBSRCLIENT_CARDCATCHERPARA;

//激活器参数
typedef struct tagBSRCLIENT_ACTIVATORPARA
{
	CHAR nLink;						//连接类型:1-报警输入
	CHAR nPort;						//端口号:1-4
	CHAR bEnabled;					//起禁用状态:0-禁用,1-启用
	CHAR res;						//保留位
}BSRCLIENT_ACTIVATORPARA, *PBSRCLIENT_ACTIVATORPARA;

//串口代理参数获取的输出及设置的输入
typedef struct tagBSRCLIENT_UARTAGENTPARA
{
	CHAR nlink;						//连接类型:1-RS232,3-RS485
	CHAR port;						//端口号:1-4
	CHAR bEnabled;					//起禁用状态:0-禁用,1-启用
	CHAR nProtocol;					//协议:1-内保局
}BSRCLIENT_UARTAGENTPARA, *PBSRCLIENT_UARTAGENTPARA;

//设备端参数获取/配置命令
typedef enum
{
	CONFIGCMD_GETSYSGENERAL		= 101,	//获取系统常规参数BSCP_DVR_SYSCFG_GENERAL
	CONFIGCMD_SETSYSGENERAL,			//设置系统常规参数BSCP_DVR_SYSCFG_GENERAL
	CONFIGCMD_GETSYSDEVID,				//获取系统设备标识参数BSCP_DVR_PAGEDEVID
	CONFIGCMD_SETSYSDEVID,				//设置系统设备标识参数BSCP_DVR_PAGEDEVID

  CONFIGCMD_GETSYSDEVIDEX,				//获取系统设备标识参数BSCP_DVR_PAGEDEVIDEX
  CONFIGCMD_SETSYSDEVIDEX,				//设置系统设备标识参数BSCP_DVR_PAGEDEVIDEX

	CONFIGCMD_GETDEVICETIME,			//获取设备当前时间BSCP_DATATIME
	CONFIGCMD_SETDEVICETIME,			//设置设备当前时间BSCP_DATATIME

	CONFIGCMD_GETTIMERBAKINFO,			//获取定时备份信息BSCP_DISK_TIMERBACKUP
	CONFIGCMD_SETTIMERBAKINFO,			//设置定时备份信息BSCP_DISK_TIMERBACKUP

	CONFIGCMD_GETLOGCONFIGIP,			//获取日志配置IP BSCP_LOG_NETCFGIP
	CONFIGCMD_SETLOGCONFIGIP,			//设置日志配置IP BSCP_LOG_NETCFGIP

	CONFIGCMD_GETRTPCONFIGINFO,			//获取RTP配置信息BSRCLIENT_RTPCONFIGINFO
	CONFIGCMD_SETRTPCONFIGINFO,			//设置RTP配置信息BSRCLIENT_RTPCONFIGINFO

	CONFIGCMD_GETNETWORKPARAS,			//获取网络配置参数BSCP_NETWORK_NETCFGINFO
	CONFIGCMD_SETNETWORKPARAS,			//设置网络配置参数BSCP_NETWORK_NETCFGINFO

	CONFIGCMD_GETYTCTRLPARA,			//获取云台控制参数BSCP_CONTROL_CTRLPARA_LIST
	CONFIGCMD_SETYTCTRLPARA,			//设置云台控制参数BSCP_CONTROL_CTRLPARA_LIST

	CONFIGCMD_GETDVRHARDINFOS,			//获取DVR硬件信息BSCP_DVR_GETHARDINFO
	CONFIGCMD_SETDVRHARDINFOS,			//设置DVR硬件信息BSCP_DVR_GETHARDINFO

	CONFIGCMD_GETDVRTIMEOUT,			//获取DVR网络命令超时时限BSCP_NETWORK_TIMEOUT
	CONFIGCMD_SETDVRTIMEOUT,			//设置DVR网络命令超时时限BSCP_NETWORK_TIMEOUT

	CONFIGCMD_GETRECGENERAL,			//获取录像常规参数BSCP_RECORD_PAGEGENERAL
	CONFIGCMD_SETRECGENERAL,			//设置录像常规参数BSCP_RECORD_PAGEGENERAL
	CONFIGCMD_GETRECADVANCE,			//获取录像高级参数BSCP_RECORD_PAGEADVANCE
	CONFIGCMD_SETRECADVANCE,			//设置录像高级参数BSCP_RECORD_PAGEADVANCE

	CONFIGCMD_GETTIMERREC,				//获取定时录像参数BSCP_RECORD_PAGETIMER
	CONFIGCMD_SETTIMERREC,				//设置定时录像参数BSCP_RECORD_PAGETIMER

	CONFIGCMD_GETMOTION,				//获取移动侦测参数BSCP_RECORD_PAGEMOTION
	CONFIGCMD_SETMOTION,				//设置移动侦测参数BSCP_RECORD_PAGEMOTION

	CONFIGCMD_GETSCENECHAN,				//获取现场通道参数BSCP_RECORD_PAGESCENCE
	CONFIGCMD_SETSCENECHAN,				//设置现场通道参数BSCP_RECORD_PAGESCENCE
	CONFIGCMD_GETSCENEPATROL,			//获取现场轮巡参数BSCP_RECORD_AUTOPATROL
	CONFIGCMD_SETSCENEPATROL,			//设置现场轮巡参数BSCP_RECORD_AUTOPATROL

	//FOR OLD==>
	CONFIGCMD_GETALMDEFENCE,			//获取报警布撤防参数BSCP_ALARM_PAGEDEFENCE
	CONFIGCMD_SETALMDEFENCE,			//设置报警布撤防参数BSCP_ALARM_PAGEDEFENCE
	CONFIGCMD_GETALMALARMIN,			//获取报警输入参数BSCP_ALARM_PAGEALARMIN
	CONFIGCMD_SETALMALARMIN,			//设置报警输入参数BSCP_ALARM_PAGEALARMIN
	CONFIGCMD_GETALMRECORD,				//获取报警录像参数BSCP_ALARM_PAGERECORD
	CONFIGCMD_SETALMRECORD,				//设置报警录像参数BSCP_ALARM_PAGERECORD
	CONFIGCMD_GETALMEXCEPT,				//获取报警异常参数BSCP_ALARM_PAGEEXCEPT
	CONFIGCMD_SETALMEXCEPT,				//设置报警异常参数BSCP_ALARM_PAGEEXCEPT
	CONFIGCMD_GETALMPRERECORD,			//获取报警预录参数BSCP_ALARM_PAGEPRERECORD
	CONFIGCMD_SETALMPRERECORD,			//设置报警预录参数BSCP_ALARM_PAGEPRERECORD
	//FOR OLD<==

	//FOR ATM OR NEW==>
	CONFIGCMD_GETALARMZONE_NEW,			//获取报警防区设置BSRCLIENT_ALARMZONESET
	CONFIGCMD_SETALARMZONE_NEW,			//设置报警防区设置BSRCLIENT_ALARMZONESET
	CONFIGCMD_GETALARMRECORD_NEW,		//获取报警录像设置BSRCLIENT_ALARMRECORD_NEW
	CONFIGCMD_SETALARMRECORD_NEW,		//设置报警录像设置BSRCLIENT_ALARMRECORD_NEW
	CONFIGCMD_GETALARMALMOUT_NEW,		//获取报警输出设置BSRCLIENT_ALARMOUT_PARA
	CONFIGCMD_SETALARMALMOUT_NEW,		//设置报警输出设置BSRCLIENT_ALARMOUT_PARA
	//FOR ATM OR NEW<==

	//FOR ATM专用==>
	CONFIGCMD_GETUARTSETDATA_ATM,		//ATM机串口配置获取BSRCLIENT_ATMCTRLPARA_GROUP
	CONFIGCMD_SETUARTSETDATA_ATM,		//ATM机串口配置设置BSRCLIENT_ATMCTRLPARA_GROUP
	//FFOR ATM专用<==

	CONFIGCMD_GETSUBSTREAMPARA,			//获取子码流配置参数BSRCLIENT_SUBSTREAMPARA
	CONFIGCMD_SETSUBSTREAMPARA,			//设置子码流配置参数BSRCLIENT_SUBSTREAMPARA

	//仅支持V6.10B09版本==>
	CONFIGCMD_GETSCENECHANEX,			//获取现场通道参数BSCP_RECORD_PAGESCENCEEX
	CONFIGCMD_SETSCENECHANEX,			//设置现场通道参数BSCP_RECORD_PAGESCENCEEX

	CONFIGCMD_GETALMEXCEPTEX,			//获取报警异常参数BSCP_ALARM_PAGEEXCEPTEX
	CONFIGCMD_SETALMEXCEPTEX,			//获取报警异常参数BSCP_ALARM_PAGEEXCEPTEX
	//仅支持V6.10B09版本<==

	//仅支持带智能信息的机型==>
	CONFIGCMD_GETALGDECTVIDEOPARA,		//获取视频智能参数信息BSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_SETALGDECTVIDEOPARA,		//设置视频智能参数信息BSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_GETALGDECTAUDIOPARA,		//获取音频智能参数信息BSRCLIENT_ALGDECTAUDIOPARA
	CONFIGCMD_SETALGDECTAUDIOPARA,		//设置音频智能参数信息BSRCLIENT_ALGDECTAUDIOPARA

	//新版本智能DVR参数配置
	CONFIGCMD_GETALGDECTVIDEOPARA_NEW,		//获取视频智能参数信息BSRCLIENT_ALGDECTVIDEOPARA
	CONFIGCMD_SETALGDECTVIDEOPARA_NEW,		//设置视频智能参数信息BSRCLIENT_ALGDECTVIDEOPARA

	CONFIGCMD_GETALARMZONE_ALG,			//获取报警防区设置ALGBSRCLIENT_ALARMZONESET
	CONFIGCMD_SETALARMZONE_ALG,			//设置报警防区设置ALGBSRCLIENT_ALARMZONESET
	CONFIGCMD_GETALARMRECORD_ALG,		//获取报警录像设置ALGBSRCLIENT_ALARMRECORD
	CONFIGCMD_SETALARMRECORD_ALG,		//设置报警录像设置ALGBSRCLIENT_ALARMRECORD
	CONFIGCMD_GETALARMALMOUT_ALG,		//获取报警输出设置ALGBSRCLIENT_ALARMOUT_PARA
	CONFIGCMD_SETALARMALMOUT_ALG,		//设置报警输出设置ALGBSRCLIENT_ALARMOUT_PARA
	//仅支持带智能信息的机型<==

	//仅支持6004AMN
	CONFIGCMD_GETUARTHUBPARA,			//获取串口HUB参数设置BSRCLIENT_UARTHUBPARA
	CONFIGCMD_SETUARTHUBPARA,			//设置串口HUB参数设置BSRCLIENT_UARTHUBPARA

	//网络卡号
	CONFIGCMD_SETCARDPROTOCOL,			//设置网络卡号协议BSRCLIENT_NETCARDPROTOCOL
	CONFIGCMD_GETCARDPROTOCOL,			//获取网络卡号协议BSRCLIENT_NETCARDPROTOCOL

	//ATM卡号捕捉器相关
	CONFIGCMD_GETCARDCATCHER,			//获取ATM卡号捕捉器参数BSRCLIENT_CARDCATCHERPARA
	CONFIGCMD_SETCARDCATCHER,			//设置ATM卡号捕捉器参数BSRCLIENT_CARDCATCHERPARA
	CONFIGCMD_GETACTIVATOR,				//获取激活器参数BSRCLIENT_ACTIVATORPARA
	CONFIGCMD_SETACTIVATOR,				//设置激活器参数BSRCLIENT_ACTIVATORPARA

	//DVS专用配置
	CONFIGCMD_DVSGETALARMINZONE,		//获取1644报警输入防区设置BSCPDVS_ALARMZONEGROUP
	CONFIGCMD_DVSSETALARMINZONE,		//设置1644报警输入防区设置BSCPDVS_ALARMZONEGROUP
	CONFIGCMD_DVSGETALARMOUTRELATE,		//获取1644报警输出联动设置BSCPDVS_ALARMOUTGROUP
	CONFIGCMD_DVSSETALARMOUTRELATE,		//设置1644报警输出联动设置BSCPDVS_ALARMOUTGROUP
	CONFIGCMD_DVSGETSCENESTATUS,		//获取1644现场自动切换状态BSRCLIENT_DVS_SCENESWITCHGROUP
	CONFIGCMD_DVSSETSCENESTATUS,		//设置1644现场自动切换状态BSRCLIENT_DVS_SCENESWITCHGROUP
	CONFIGCMD_DVSGETDEVCONFIG,			//获取1644硬件设置信息BSRCLIENT_DVS_DEVCONFIGGROUP
	CONFIGCMD_DVSSETDEVCONFIG,			//设置1644硬件设置信息BSRCLIENT_DVS_DEVCONFIGGROUP
	CONFIGCMD_DVSGETNETCTLSTATUS,		//获取1644网络控制状态BSRCLIENT_DVS_NETCTLSTATUS
	CONFIGCMD_DVSSETNETCTLSTATUS,		//设置1644网络控制状态BSRCLIENT_DVS_NETCTLSTATUS

	CONFIGCMD_DVSGET2NDNICANDNVR,		//设置104/1644第二块网卡及其后接录像机参数BSRCLIENT_DVS_NICGROUP
	CONFIGCMD_DVSSET2NDNICANDNVR,		//获取104/1644第二块网卡及其后接录像机参数BSRCLIENT_DVS_NICGROUP

	//声讯专有命令
	CONFIGCMD_GETSECBUREAUSTATUS,       //获取注册状态
	CONFIGCMD_GETSECBUREAUCFG,          //获取注册参数
	CONFIGCMD_SETSECBUREAUCFG,          //设置注册参数

	//声讯命令完

  //网络卡号设置
  CONFIGCMD_GETNETCARDCFG,
  CONFIGCMD_SETNETCARDCFG,


  //设置网络检测

  CONFIGCMD_GETNETDETECT,
  CONFIGCMD_SETNETDETECT,

  CONFIGCMD_GETREGISTEREXP, //获取主动注册外设相关
  CONFIGCMD_SETREGISTEREXP, //设置主动注册外设相关

  CONFIGCMD_GETREGISTERPARA, //获取主动注册参数
  CONFIGCMD_SETREGISTERPARA, //设置主动注册参数

  CONFIGCMD_GETREGISTERSTANDBY, //获取重连间隔
  CONFIGCMD_SETREGISTERSTANDBY, //设置重连间隔

  CONFIGCMD_GETREGISTERSTAT, //获取主动注册状态

  CONFIGCMD_GETCHNWARNREPORTSTAT, //获取通道报警上传信息
  CONFIGCMD_SETCHNWARNREPORTSTAT, //设置通道报警上传信息

  CONFIGCMD_MANUALREGISTER, //手动注册 
  CONFIGCMD_MANUALLOGOUT, //手动注销

  CONFIGCMD_GETREGISTERAGENT, //获取内保局协议信息
  CONFIGCMD_SETREGISTERAGENT, //设置内保局协议信息
  
  //设置atm智能机全景
  CONFIGCMD_GETATMSCENEOBJ,
  CONFIGCMD_SETATMSCENEOBJ,

}BSRCLIENT_CONFIGCMD;

//用户组结构
typedef struct tagBSCP_USER_GROUP
{
	UCHAR UserId;				//用户id 0~9
	UCHAR grade;				//权限 0:普通用户 1:操作员 2:管理员 3:超级管理员
	USHORT resv;				//保留
	INT dialogId;				//会话session
}BSCP_USER_GROUP, *PBSCP_USER_GROUP;

//当前登陆用户
typedef struct tagBSCP_USER_CURLOGINUSR 
{
	BSCP_USER_GROUP usrLogin[MAX_LOGIN_NUM];	//
}BSCP_USER_CURLOGINUSR, *PBSCP_USER_CURLOGINUSR;

//用户详细信息
typedef struct tagBSCP_USER_DETAIL
{
	CHAR username[16];			//用户名 ,最多15位
	CHAR userpass[8];			//密码 ,最多6位
	UCHAR uid;					//用户id 0~9
	UCHAR grade;				//权限 0:普通用户 1:操作员 2:管理员 3:超级管理员
	USHORT backup;				//
}BSCP_USER_DETAIL, *PBSCP_USER_DETAIL;

//显示当前所有用户
typedef struct tagBSCP_USER_ALLUSER
{
	BSCP_USER_DETAIL usrAll[MAX_USERS_NUM];	//
}BSCP_USER_ALLUSER, *PBSCP_USER_ALLUSER;

//会话详细信息
typedef struct tagBSRCLIENT_DIALOGDETAIL
{
	LONG stat;				//会话状态
	LONG sock;				//socketid
	LONG sessionid;			//会话sessionid
	BOOL blogin;			//是否登录
	CHAR szClientIP[16];	//客户端IP地址
	LONG netport;			//客户端端口
	LONG uid;				//登录用户id
	LONG ugrade;			//登录用户权限
	ULONG idletime;			//空闲时间
	SYSTEMTIME logintime;	//登录时间
}BSRCLIENT_DIALOGDETAIL, *PBSRCLIENT_DIALOGDETAIL;

//传输通道详细信息
typedef struct tagBSCP_DEBUG_TRANSCHANINFO
{
	INT stat;			//传输状态
	INT sock;			//socketid
	INT trans_id;		//传输id
	INT sess_sid;		//启动传输的会话id
	INT sessSockid;		//启动传输的会话socketid
	INT rcv_timeout;	//接收超时时间
	INT snd_timeout;	//发送超时时间
	INT resv;			//保留1
	UINT64 snd_num;		//发送的字节数
	UINT64 rcv_num;		//接收到的字节数
	INT conn_time;		//连接时间
	INT resv1;			//保留2
}BSCP_DEBUG_TRANSCHANINFO, *PBSCP_DEBUG_TRANSCHANINFO;

//传输信息组
typedef struct tagBSRCLIENT_TRANSCHANINFO
{
	BSCP_DEBUG_TRANSCHANINFO TransInfo[MAX_LOGIN_NUM];
}BSRCLIENT_TRANSCHANINFO, *PBSRCLIENT_TRANSCHANINFO;

//当前所有会话信息
typedef struct tagBSRCLIENT_DIALOGINFOS
{
	BSRCLIENT_DIALOGDETAIL dialog[MAX_LOGIN_NUM];
}BSRCLIENT_DIALOGINFOS, *PBSRCLIENT_DIALOGINFOS;

//当前实际有效通道
typedef struct tagBSRCLIENT_ACTUALCHANS
{
	BOOL bValid[32];
}BSRCLIENT_ACTUALCHANS, *PBSRCLIENT_ACTUALCHANS;

//系统当前负载信息结构
typedef struct tagBSRCLIENT_DVRLOADINFO
{
	CHAR product[32];		//产品名称
	CHAR version[32];		//产品版本 
	CHAR buildtime[32];		//编译时间 
	UINT RealTime;			//系统时间(32位time_t类型)
	UINT Runningtime;		//运行时间(32位time_t类型)
	INT tasksNum;			//任务总数
	INT cpuLoad;			//CPU负载(%) 
	INT memUsage;			//内存使用(%)  
}BSRCLIENT_DVRLOADINFO, *PBSRCLIENT_DVRLOADINFO;

//通道状态
typedef struct tagBSRCLIENT_CHANSTATUS
{
	CHAR type;				//录像类型:无(0)|手工(1)|定时(2)|侦测(4)|报警(8)[位与]
	CHAR codec;				//录像编码格式:1(mpeg4)|3(h263)|4(h264)
	CHAR resolution;		//分辨率:PAL DCIF(1)|CIF(2)|1/2D1(3)|D1(4) NTSC+128
	CHAR flags;				//bit0:是否启用预录,bit1:是否有伴音
	CHAR videoFormat;		//视频画质
	CHAR audioFormat;		//音频音质
	SHORT res;				//保留
	UINT firstClipTime;		//最早录像时间(32位time_t类型)
	UINT lastClipTime;		//最晚录像时间(32位time_t类型),如果正在录像则为系统当前时间
}BSRCLIENT_CHANSTATUS, *PBSRCLIENT_CHANSTATUS;

//磁盘状态
typedef struct tagBSRCLIENT_DISKSTATUS
{
	INT status;				//当前状态,休眠(0)|活动(1)
	UINT totalSpace;		//磁盘总容量KB(厂商计算方式1000进制)
	UINT freeSpace;			//磁盘剩余容量KB(厂商计算方式1000进制)
	UINT startTime;			//最早录像段时间(32位time_t类型)
	UINT endTime;			//最晚录像段时间(32位time_t类型),如果正在录像则为系统当前时间
}BSRCLIENT_DISKSTATUS, *PBSRCLIENT_DISKSTATUS;

//系统当前运行状态信息结构(B09类型)
typedef struct tagBSRCLIENT_DVRRUNINFO
{
	INT serial;				//产品出厂序列号
	CHAR version[16];		//设备版本信息
	UINT localtime;			//设备系统时间(32位time_t类型)
	UINT uptime;			//设备从启动开始后的运行时间
	UCHAR load;				//当前cpu负载
	UCHAR streams;			//当前视频流数目
	UCHAR channels;			//实际通道数
	UCHAR disks;			//磁盘个数
	BSRCLIENT_CHANSTATUS channStatus[MAX_CHAN_NUM];
	BSRCLIENT_DISKSTATUS diskStatus[MAX_DISK_NUM];   
}BSRCLIENT_DVRRUNINFO, *PBSRCLIENT_DVRRUNINFO;

//系统当前运行状态信息结构(4通道 4硬盘)
typedef struct tagBSRCLIENT_DVRRUNINFO_EX
{
	INT serial;				//产品出厂序列号
	CHAR version[16];		//设备版本信息
	UINT localtime;			//设备系统时间(32位time_t类型)
	UINT uptime;			//设备从启动开始后的运行时间
	UCHAR load;				//当前cpu负载
	UCHAR streams;			//当前视频流数目
	UCHAR channels;			//实际通道数
	UCHAR disks;			//磁盘个数
	BSRCLIENT_CHANSTATUS channStatus[FOUR_CHAN_MAX];
	BSRCLIENT_DISKSTATUS diskStatus[FOUR_DISK_MAX];   
}BSRCLIENT_DVRRUNINFO_EX, *PBSRCLIENT_DVRRUNINFO_EX;

//命令等待超时
typedef struct tagBSCP_NETWORK_TIMEOUT
{
	INT nTimeout;			//超时时间
}BSCP_NETWORK_TIMEOUT, *PBSCP_NETWORK_TIMEOUT;

//日志查询主类别
typedef enum
{
	MAJOLOG_ALL		= 0,	//所有
	MAJOLOG_OPERATE	= 1,	//操作
	MAJOLOG_ALARM	= 2,	//报警
	MAJOLOG_EXCEPT	= 3,	//异常
	MAJOLOG_ADJTIME	= 4		//时间调整
}BSCP_LOGEVENT_MAJOR;

//日志查询子类别
typedef enum
{
	MINOR_ALL		= 0		//所有
}BSCP_LOGEVENT_MINOR;

//日志查询具体事件
typedef enum
{
	EVENTLOG_NULL		= 0,	//所有

	EVENTLOG_ALARM1		= 1,	//报警1
	EVENTLOG_ALARM2		= 2,	//报警2
	EVENTLOG_ALARM3		= 3,	//报警3
	EVENTLOG_ALARM4		= 4,	//报警4
	EVENTLOG_ALARM5		= 5,	//报警5
	EVENTLOG_ALARM6		= 6,	//报警6
	EVENTLOG_ALARM7		= 7,	//报警7
	EVENTLOG_ALARM8		= 8,	//报警8
	EVENTLOG_ALARM9		= 9,	//报警9
	EVENTLOG_ALARM10	= 10,	//报警10
	EVENTLOG_ALARM11	= 11,	//报警11
	EVENTLOG_ALARM12	= 12,	//报警12
	EVENTLOG_ALARM13	= 13,	//报警13
	EVENTLOG_ALARM14	= 14,	//报警14
	EVENTLOG_ALARM15	= 15,	//报警15
	EVENTLOG_ALARM16	= 16,	//报警16

	EVENTLOG_MOTION		= 17,	//侦测录像

	EVENTLOG_VLOST1		= 18,	//视频丢失1
	EVENTLOG_VLOST2		= 19,	//视频丢失2
	EVENTLOG_VLOST3		= 20,	//视频丢失3
	EVENTLOG_VLOST4		= 21,	//视频丢失4
	EVENTLOG_VLOST5		= 22,	//视频丢失5
	EVENTLOG_VLOST6		= 23,	//视频丢失6
	EVENTLOG_VLOST7		= 24,	//视频丢失7
	EVENTLOG_VLOST8		= 25,	//视频丢失8
	EVENTLOG_VLOST9		= 26,	//视频丢失9
	EVENTLOG_VLOST10	= 27,	//视频丢失10
	EVENTLOG_VLOST11	= 28,	//视频丢失11
	EVENTLOG_VLOST12	= 29,	//视频丢失12
	EVENTLOG_VLOST13	= 30,	//视频丢失13
	EVENTLOG_VLOST14	= 31,	//视频丢失14
	EVENTLOG_VLOST15	= 32,	//视频丢失15
	EVENTLOG_VLOST16	= 33,	//视频丢失16

	EVENTLOG_ENTERMENU		= 34,	//进入菜单
	EVENTLOG_EXITMENU		= 35,	//退出菜单
	EVENTLOG_BEGINDEFENCE	= 36,	//布防
	EVENTLOG_ENDDEFENCE		= 37,	//撤防
	EVENTLOG_DISKFORMAT		= 38,	//格式化硬盘
	EVENTLOG_RESTOREDEF		= 39,	//出厂设置
	EVENTLOG_SETDATETIME	= 40,	//时间日期设置
	EVENTLOG_SETRECPARA		= 41,	//录像参数设置
	EVENTLOG_SETVIDEOPARA	= 42,	//视频参数设置
	EVENTLOG_SETALARMREC	= 43,	//报警录像设置
	EVENTLOG_SETCOMMPARA	= 44,	//通信设置
	EVENTLOG_SETTIMERREC	= 45,	//定时录像设置
	EVENTLOG_SETMOTION		= 46,	//移动侦测设置
	EVENTLOG_SETALARMPREREC	= 47,	//待警录像设置
	EVENTLOG_SETDISKMODE	= 48,	//硬盘工作模式设置
	EVENTLOG_SETNETWORK		= 49,	//网络设置
	EVENTLOG_MANSTARTREC	= 50,	//手工启动录像
	EVENTLOG_MANSTOPREC		= 51,	//手工停止录像
	EVENTLOG_SETSYSTEMPWD	= 52,	//系统密码设置
	EVENTLOG_UPGRADE		= 53,	//程序升级成功
	EVENTLOG_FINDRECORD		= 54,	//录像检索
	EVENTLOG_BACKUPRECORD	= 55,	//录像备份
	EVENTLOG_LOCKKEYBOARD	= 56,	//键盘锁定
	EVENTLOG_UNLOCKKEYBOARD	= 57,	//键盘解锁
	EVENTLOG_OVERLAYCARDNO	= 58,	//卡号叠加

	EVENTLOG_POWERON		= 59,	//开机
	EVENTLOG_SHUTDOWN		= 60,	//关机
	EVENTLOG_LAWLESSSHUTDOWN= 61,	//非法关机
	EVENTLOG_LOCALLOGIN		= 62,	//本地登录
	EVENTLOG_LOCALLOGOUT	= 63,	//本地注销
	EVENTLOG_LOCALCONFIG	= 64,	//本地配置
	EVENTLOG_LOCALPLAY		= 65,	//本地回放
	EVENTLOG_LOCALPLAYBYTIME= 66,	//本地按时间回放
	EVENTLOG_LOCALSTARTREC	= 67,	//本地启动录像
	EVENTLOG_LOCALSTOPREC	= 68,	//本地停止录像
	EVENTLOG_LOCALPTZCTRL	= 69,	//本地云台控制
	EVENTLOG_LOCALUPGRADE	= 70,	//本地升级
	EVENTLOG_LOCALBAKBYTIME	= 71,	//本地按时间备份
	EVENTLOG_LOCALBAKBYFILE	= 72,	//本地按文件备份
	EVENTLOG_BACKCANCEL		= 73,	//备份中止
	EVENTLOG_BACKCOMPLITE	= 74,	//备份完成
	EVENTLOG_BAKDISKFULL	= 75,	//备份盘满

	EVENTLOG_REMOTELOGIN			= 76,	//远程登录
	EVENTLOG_REMOTELOGOUT			= 77,	//远程注销
	EVENTLOG_REMOTESTARTREC			= 78,	//远程启动录像
	EVENTLOG_REMOTESTOPREC			= 79,	//远程停止录像
	EVENTLOG_CREATETRANSCHAN		= 80,	//建立透明通道
	EVENTLOG_DISCONNTRANSCHAN		= 81,	//断开透明通道
	EVENTLOG_REMOTEGETPARA			= 82,	//远程获取参数
	EVENTLOG_REMOTESETPARA			= 83,	//远程配置
	EVENTLOG_REMOTEGETSTAT			= 84,	//远程获取状态
	EVENTLOG_REMOTEBEGINDDEFENCE	= 85,	//远程布防
	EVENTLOG_REMOTEENDDEFENCE		= 86,	//远程撤防
	EVENTLOG_REMOTEREBOOT			= 87,	//远程重启
	EVENTLOG_STARTVOICETALK			= 88,	//语音对讲开始
	EVENTLOG_STOPVOICETALK			= 89,	//语音对讲结束
	EVENTLOG_REMOTEUPGRADE			= 90,	//远程升级
	EVENTLOG_REMOTEFILEPLAY			= 91,	//远程文件回放
	EVENTLOG_REMOTEPLAYBYTIME		= 92,	//远程按时间回放
	EVENTLOG_REMOTEPTZCTRL			= 93,	//远程云台控制
	EVENTLOG_ALARMINPUT				= 94,	//报警输入
	EVENTLOG_ALARMOUTPUT			= 95,	//报警输出
	EVENTLOG_BEGINMOTION			= 96,	//移动侦测开始
	EVENTLOG_ENDMOTION				= 97,	//移动侦测结束
	EVENTLOG_VIDEOLOST				= 98,	//视频丢失
	EVENTLOG_COVERALARMBEGIN		= 99,	//遮挡告警开始
	EVENTLOG_COVERALARMEND			= 100,	//遮挡告警结束
	EVENTLOG_DISKERR				= 101,	//硬盘错
	EVENTLOG_DISKFULL				= 102,	//硬盘满
	EVENTLOG_NETWORKDISCONN			= 103,	//网络断
	EVENTLOG_IPADDRESSCONFLICT		= 104,	//IP地址冲突
	EVENTLOG_LAWLESSLOCALACCESS		= 105,	//非法本地访问
	EVENTLOG_DISKFAILUREALARM		= 106,	//硬盘故障报警
	EVENTLOG_FANFAILUREALARM		= 107,	//风扇故障报警
	EVENTLOG_FANFAILURERECOVER		= 108,	//风扇故障恢复
	EVENTLOG_MBOARDFAILUREALARM		= 109,	//主板故障报警
	EVENTLOG_CHANFAILUREALARM		= 110,	//通道故障报警
	EVENTLOG_TIMEERRALARM			= 111,	//时间错误报警
	EVENTLOG_ADJUSTTIME				= 112,	//校时
	EVENTLOG_NETTRANSBEGIN			= 113,	//网络传输启动
	EVENTLOG_NETTRANSEND			= 114,	//网络传输停止

	EVENTLOG_AUDIOANALYSE			= 115,	//音频分析报警
	EVENTLOG_NOFACEANALYSE			= 116,	//非人脸报警
	EVENTLOG_PERIMETERPROC			= 117,	//周界报警
	EVENTLOG_STUMLINEALARM			= 118,	//伴线报警
	EVENTLOG_PEPOLECOUNTALARM		= 119,	//人数统计报警
	EVENTLOG_LEFTOBJALARM			= 120,	//遗留物报警
	EVENTLOG_LOSSOBJALARM			= 121,	//丢失物报警

	EVENTLOG_INVALIDFLAG					//无效守护码
};

//录像详细信息
typedef struct tagBSCP_LOG_RECORDDETAIL
{	
	CHAR strname[16];			//操作用户名
	UINT dt;					//事件发生的时间(32位time_t类型)
	UINT beforedt;				//操作前的时间(32位time_t类型)
	UINT afterdt;				//操作后的时间(32位time_t类型)
	UCHAR type;					//具体事件
	UCHAR diskno;				//磁盘id
	USHORT rev;					//保留
	ULONG recordId;				//录像段编号
	ULONG endrecordId;			//录像段编号
	ULONG ChannelId;			//通道编号,按位掩码	
}BSCP_LOG_RECORDDETAIL, *PBSCP_LOG_RECORDDETAIL;

//日志信息
typedef struct tagBSCP_DVR_LOG
{
	BSCP_LOG_RECORDDETAIL logs[MAX_ONEPAGELOG_SIZE];
}BSCP_DVR_LOG, *PBSCP_DVR_LOG;

//日志网络配置IP
typedef struct tagBSCP_LOG_NETCFGIP
{
	CHAR szNetWorkIP[16];		//IP信息
}BSCP_LOG_NETCFGIP, *PBSCP_LOG_NETCFGIP;

//磁盘序号为0-7之间(最多支持8块硬盘)
#define BSCP_ALLDISK	0xFF	//所有磁盘

//磁盘子信息
typedef struct tagBSCP_DISK_SUBINFO
{
	UCHAR disk;			//磁盘序号0 - 7
	UCHAR sleep;		//是否在睡眠状态1:是 0:否
	UCHAR exist;		//该盘是否存在1:存在 0:不存在
	UCHAR rev;			//保留
	USHORT wrev;		//保留
	USHORT timeout;		//该盘的超时时间
	ULONG idle;			//该盘已经空闲了多长时间
	ULONG sleepTime;	//已经睡眠的时间
	ULONG totalSectors;	//磁盘容量(这里为扇区数,一个扇区为512BYTE)
	UCHAR flag;			//标志位
	UCHAR Selected;		//Current Selected disk through network
	UCHAR isInUsed;		//被用户设置为在用盘的盘符
	UCHAR bBackup;		//备份盘
	UCHAR bFormat;		//是否格式化
	UCHAR bRecord;		//录像盘
	UCHAR Backup;		//备份盘
	UCHAR waking;		//正在启动过程中
}BSCP_DISK_SUBINFO, *PBSCP_DISK_SUBINFO;

//磁盘信息
typedef struct tagBSRCLIENT_DISKINFO
{
	LONG nDiskCount;	//磁盘个数,当获取所有磁盘信息时才有意义
	BSCP_DISK_SUBINFO disk[MAX_DISK_NUM];
}BSRCLIENT_DISKINFO, *PBSRCLIENT_DISKINFO;

//DVR文件时间长度转换
typedef struct tagBSRCLIENT_FILETIME2LEN
{
	BOOL bTime2Len;		//TRUE为时间转长度,FALSE为长度转时间
	UINT stime;			//起始时间(32位time_t类型)
	UINT etime;			//结束时间(32位time_t类型)
	UINT64 bstart;		//起始字节数
	UINT64 blength;		//数据长度
}BSRCLIENT_FILETIME2LEN, *PBSRCLIENT_FILETIME2LEN;

//磁盘类别
typedef enum
{
	DISKTYPE_WORKDISK		= 0,	//工作盘
	DISKTYPE_RECORDDISK		= 1,	//录像盘
	DISKTYPE_BACKDISK		= 2		//备份盘
}BSRCLIENT_DISKTYPE;

//工作盘子参数类别
typedef enum
{
	WDISKTYPE_NONE			= 0,	//无
	WDISKTYPE_YES			= 1,	//是
	WDISKTYPE_NO			= 2		//否
}BSRCLIENT_WORKSUBTYPE;

//DVR本地回放控制命令
typedef enum
{
	LOCALPLAYCMD_PLAY		= 0,	//播放录像
	LOCALPLAYCMD_FASTBACK,			//快退
	LOCALPLAYCMD_FASTSPEED,			//快进
	LOCALPLAYCMD_PAUSE,				//暂停
	LOCALPLAYCMD_BACKTOFIRST,		//回到该录像段首
	LOCALPLAYCMD_SINGLEFRAME,		//逐帧播放
	LOCALPLAYCMD_PREVIOUS,			//上一段
	LOCALPLAYCMD_NEXT,				//下一段
	LOCALPLAYCMD_PREALARM,			//上一段报警录像
	LOCALPLAYCMD_NEXTALARM			//下一段报警录像
}BSRCLIENT_LOCALPLAYCMD;

//云台1604间接控制命令
typedef enum
{
	SELECT_RECORD1604	= 128,		//选择录像机
	CTRL_RECORD1604		= 129,		//控制录像机
	ALARM_INPUT1604		= 130,		//报警输入
	ALARM_OUTPUT1604	= 131,		//报警输出
	SELECT_PTZ1604		= 132,		//选择云台
	CTRL_PTZ1604		= 133,		//控制云台
	CLEAR_ALARMOUT1604	= 134		//清除报警
}BSRCLIENT_PTZCTRLCMDBY1604;

//云台本地直接控制命令
typedef enum
{
	BSRCTRLPTZ_CHAN01LOCAL	= 0xAA00,		//控制第1通道云台
	BSRCTRLPTZ_CHAN02LOCAL	= 0xAA01,		//控制第2通道云台
	BSRCTRLPTZ_CHAN03LOCAL	= 0xAA02,		//控制第3通道云台
	BSRCTRLPTZ_CHAN04LOCAL	= 0xAA03,		//控制第4通道云台
	BSRCTRLPTZ_CHAN05LOCAL	= 0xAA04,		//控制第5通道云台
	BSRCTRLPTZ_CHAN06LOCAL	= 0xAA05,		//控制第6通道云台
	BSRCTRLPTZ_CHAN07LOCAL	= 0xAA06,		//控制第7通道云台
	BSRCTRLPTZ_CHAN08LOCAL	= 0xAA07,		//控制第8通道云台
	BSRCTRLPTZ_CHAN09LOCAL	= 0xAA08,		//控制第9通道云台
	BSRCTRLPTZ_CHAN10LOCAL	= 0xAA09,		//控制第10通道云台
	BSRCTRLPTZ_CHAN11LOCAL	= 0xAA0A,		//控制第11通道云台
	BSRCTRLPTZ_CHAN12LOCAL	= 0xAA0B,		//控制第12通道云台
	BSRCTRLPTZ_CHAN13LOCAL	= 0xAA0C,		//控制第13通道云台
	BSRCTRLPTZ_CHAN14LOCAL	= 0xAA0D,		//控制第14通道云台
	BSRCTRLPTZ_CHAN15LOCAL	= 0xAA0E,		//控制第15通道云台
	BSRCTRLPTZ_CHAN16LOCAL	= 0xAA0F,		//控制第16通道云台
	BSRSELECT_DVRLOCAL		= 0xF070,		//选择DVR
	BSRSELECT_BAUDLOCAL		= 0xF081,		//选择波特率
	BSRSELECT_PROTOLOCAL	= 0xF083,		//选择协议
	BSRSELECT_PTZLOCAL		= 0xF084,		//选择云台
	BSRCTRL_PTZLOCAL		= 0xF085,		//控制云台(当前画面所在通道)
	BSRPRESS_SETLOCAL		= 0xF089,		//设置预置位
	BSRPRESS_GOLOCAL		= 0xF090,		//跳转预置位
	BSRCTRL_CUSTOMLOCAL		= 0xF0FE		//自定义
}BSRCLIENT_PTZCTRLCMDBYLOCAL;

//本地控制命令参数
typedef enum
{
	BSRYTCTRL_ACTION_UP					= 0,	//向上移动
	BSRYTCTRL_ACTION_DOWN				= 1,	//向下移动
	BSRYTCTRL_ACTION_LEFT				= 2,	//向左移动
	BSRYTCTRL_ACTION_RIGHT				= 3,	//向右移动
	BSRYTCTRL_ACTION_CLOSE				= 4,	//光圈缩小
	BSRYTCTRL_ACTION_OPEN				= 5,	//光圈放大
	BSRYTCTRL_ACTION_NEAR				= 6,	//焦距靠近
	BSRYTCTRL_ACTION_FAR				= 7,	//焦距拉远
	BSRYTCTRL_ACTION_WIDE				= 8,	//变倍增大
	BSRYTCTRL_ACTION_TELE				= 9,	//变倍减小
	BSRYTCTRL_ACTION_AUTO				= 10,	//自动状态
	BSRYTCTRL_ACTION_STOP				= 11,	//停止控制

	BSRYTCTRL_ACTION_SETPRE				= 12,	//设置预置位
	BSRYTCTRL_ACTION_GOPRE				= 13,	//转到预置位
	BSRYTCTRL_ACTION_DELPRE				= 14,	//删除预置位

	BSRYTCTRL_ACTION_SETPATTERNSTART	= 15,	//启动轨迹
	BSRYTCTRL_ACTION_SETPATTERNSTOP		= 16,	//停止轨迹
	BSRYTCTRL_ACTION_RUNPATTREN			= 17,	//运行轨迹

	BSRYTCTRL_ACTION_SETAUX				= 18,	//设置辅助
	BSRYTCTRL_ACTION_DELAUX				= 19,	//删除辅助

	BSRYTCTRL_ACTION_ADDSCANDOT			= 20,	//添加巡航点
	BSRYTCTRL_ACTION_DELSCANDOT			= 21,	//删除巡航点

	BSRYTCTRL_ACTION_RUNSCAN			= 22,	//开始巡航
	BSRYTCTRL_ACTION_ENDSCAN			= 23,	//结束巡航

	BSRYTCTRL_ACTION_AUXALM				= 24	//辅助报警
}BSRCLIENT_PTZCTRLPARABYLOCAL;

//云台控制预置位命令
typedef enum
{
	PTZPRESET_SET		= 1,	//设置预置点
	PTZPRESET_DEL		= 2,	//清除预置点
	PTZPRESET_GOTO		= 3		//转到预置点
}BSRCLIENT_PTZPRESET;

//云台轨迹控制命令
typedef enum
{
	PTZTRACK_START		= 1,	//开始记录
	PTZTRACK_STOP		= 2,	//停止记录
	PTZTRACK_RUN		= 3,	//开始运行
	PTZTRACK_STOPRUN	= 4		//停止运行
}BSRCLIENT_PTZTRACKCMD;

//云台协议类型
typedef enum
{
	PTZPROTOCOL_PLCOD		= 0,	//PLCO-D协议
	PTZPROTOCOL_PLCOP		= 1,	//PLCO-P协议
	PTZPROTOCOL_ADAB		= 2,	//AD/AB协议
	PTZPROTOCOL_KDT			= 3,	//KDT协议
	PTZPROTOCOL_ADT			= 4,	//ADT协议

	//后面的协议仅V6.03B09/V6.10B09支持
	PTZPROTOCOL_PHILIPS		= 5,	//PHILIPS协议
	PTZPROTOCOL_SONY		= 6,	//SONY协议

	PTZPROTOCOL_HY			= 7,	//HY协议
	PTZPROTOCOL_SAMSUNG		= 8,	//SAMSUNG协议
	PTZPROTOCOL_SHARP		= 9,	//SHARP协议
	PTZPROTOCOL_TIANDI		= 10,	//TIANDI协议
	PTZPROTOCOL_TWHC		= 11,	//TWHC协议
	PTZPROTOCOL_NONE		= 12,	//NONE协议
	PTZPROTOCOL_AD1614M		= 13,	//AD1614M协议
	PTZPROTOCOL_ADMATRIX	= 14,	//ADMATRIX协议

	PTZPROTOCOL_BANKNOTE	= 15,	//BANKNOTE协议
	PTZPROTOCOL_BOCOM		= 16,	//BOCOM协议
	PTZPROTOCOL_DHMATRIX	= 17,	//DH-MATRIX协议
	PTZPROTOCOL_DSCP		= 18,	//DSCP协议
	PTZPROTOCOL_KALATEL		= 19,	//KALATEL协议
	PTZPROTOCOL_CUSTOM		= 20,	//CUSTOM协议
	PTZPROTOCOL_JVC			= 21,	//JVC协议
	PTZPROTOCOL_YAAN		= 22,	//YAAN协议
	PTZPROTOCOL_B01			= 23,	//B01协议
	PTZPROTOCOL_MAINVAN		= 24,	//MAINVAN协议
	PTZPROTOCOL_LILIN		= 25,	//LILIN协议
	PTZPROTOCOL_722LD		= 26,	//722-LD协议
	PTZPROTOCOL_3032103X	= 27,	//3032103X协议
	PTZPROTOCOL_INFINOVA	= 28,	//INFINOVA协议
	PTZPROTOCOL_SYYT		= 29,	//SYYT协议
	PTZPROTOCOL_HONEYWELL	= 30,	//HONEYWELL协议
	PTZPROTOCOL_PIH717		= 31,	//PIH717协议
}BSRCLIENT_PTZPROTOCOL;

//云台地址类型
typedef enum
{
	//常规版本
	PTZADDTYPE_BLUESTAR	= 0,	//BSR地址
	PTZADDTYPE_COMMON	= 1,	//通用地址

	//B09版本
	PTZADDTYPE_PTZ485	= 0,	//云台485
	PTZADDTYPE_CTRL485	= 1,	//受控485
	PTZADDTYPE_RS232	= 2		//串口232
}BSRCLIENT_PTZADDTYPE;

//云台波特率
typedef enum
{
	PTZBITRATE_VALUE75		= 0,	//75
	PTZBITRATE_VALUE110		= 1,	//110
	PTZBITRATE_VALUE134		= 2,	//134
	PTZBITRATE_VALUE150		= 3,	//150
	PTZBITRATE_VALUE300		= 4,	//300
	PTZBITRATE_VALUE600		= 5,	//600
	PTZBITRATE_VALUE1200	= 6,	//1200
	PTZBITRATE_VALUE1800	= 7,	//1800
	PTZBITRATE_VALUE2400	= 8,	//2400
	PTZBITRATE_VALUE4800	= 9,	//4800
	PTZBITRATE_VALUE7200	= 10,	//7200
	PTZBITRATE_VALUE9600	= 11,	//9600
	PTZBITRATE_VALUE14400	= 12,	//14400
	PTZBITRATE_VALUE19200	= 13,	//19200
	PTZBITRATE_VALUE38400	= 14,	//38400
	PTZBITRATE_VALUE57600	= 15,	//57600
	PTZBITRATE_VALUE115200	= 16	//115200
}BSRCLIENT_PTZBITRATE;

//巡航点
typedef struct tagBSCP_CONTROL_CRUISEPOS
{	
	UCHAR presetId;			//预置位编号
	UCHAR cruiseV;			//巡航速度
	USHORT stopTime;		//停留时间
}BSCP_CONTROL_CRUISEPOS, *PBSCP_CONTROL_CRUISEPOS;

//巡航路径
typedef struct tagBSCP_CONTROL_CRUISEPOSITEM
{
	UCHAR chanId;			//通道编号
	UCHAR CruisepathId;		//巡航路径编号
	USHORT res;				//保留
	BSCP_CONTROL_CRUISEPOS cruise[MAX_CRUISEDOT_NUM];
}BSCP_CONTROL_CRUISEPOSITEM, *PBSCP_CONTROL_CRUISEPOSITEM;

//所有巡航路径
typedef struct tagBSCP_CONTROL_CRUISEPOSLIST
{
	BSCP_CONTROL_CRUISEPOSITEM item[MAX_CHAN_NUM];	//巡航路径组
}BSCP_CONTROL_CRUISEPOSLIST, *PBSCP_CONTROL_CRUISEPOSLIST;

//DVR云台控制受控参数组,串口设置
typedef struct tagBSCP_CONTROL_CTRLPARA_ITEM
{
	UCHAR addr;				//控制地址
	UCHAR resv;				//保留位
	USHORT protocol;		//云台控制协议BSRCLIENT_PTZPROTOCOL

	UCHAR data_bit;			//数据位4,5,6,7,8
	UCHAR stop_bit;			//停止位1, 1.5 2
	UCHAR check_bit;		//校验位 奇,偶,无
	UCHAR baud_rate;		//波特率BSRCLIENT_PTZBITRATE
}BSCP_CONTROL_CTRLPARA_ITEM, *PBSCP_CONTROL_CTRLPARA_ITEM;

//DVR云台控制受控参数,串口设置
typedef struct tagBSCP_CONTROL_CTRLPARA_LIST
{
	USHORT addr_type;		//云台地址类型BSRCLIENT_PTZADDTYPE
	USHORT dvr_addr;		//DVR地址(有效值为0-255之间)
	BSCP_CONTROL_CTRLPARA_ITEM uart_ctrl[MAX_CHAN_NUM+1];	//MAX_CHAN_NUM 个云台,一个受控
}BSCP_CONTROL_CTRLPARA_LIST, *PBSCP_CONTROL_CTRLPARA_LIST;

//=============>ATM专用串口设置结构,DVR型号6003
//DVR云台控制受控参数组
typedef struct tagBSCP_CONTROL_ATMCTRLPARA_ITEM
{
	UCHAR addr;				//控制地址
	UCHAR resv;				//保留位
	UCHAR protocol;			//云台控制协议BSRCLIENT_PTZPROTOCOL
	UCHAR data_bit;			//数据位4,5,6,7,8

	UCHAR stop_bit;			//停止位1, 1.5 2
	UCHAR check_bit;		//校验位 奇,偶,无 
	UCHAR baud_rate;		//波特率BSRCLIENT_PTZBITRATE
	UCHAR rev;				//保留位
}BSCP_CONTROL_ATMCTRLPARA_ITEM, *PBSCP_CONTROL_ATMCTRLPARA_ITEM;

#define BSRCLIENT_UARTCOUNT	3	//串口设置组总数
//DVR云台控制受控参数
typedef struct tagBSRCLIENT_ATMCTRLPARA_GROUP
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	//设置组
	UCHAR dvraddr;			//录像机地址1-255
	UCHAR AtmSelect;		//ATM选择0-无,1-广电运通,2-日立,3-哥伦比亚,4-印度(此项与串口HUB重复)
	UCHAR ConmmMode;		//通信方式0-无,1-串口232
	UCHAR AtmRecorddelay;	//录像延时10-255
	UCHAR startDepartment;	//是否启动内保局 0否 1是(此项与串口HUB重复)
	UCHAR startRecorddelay;	//是否启动录像延时0否 1是
	UCHAR rev1;				//保留
	UCHAR rev2;				//保留
}BSRCLIENT_ATMCTRLPARA_GROUP, *PBSRCLIENT_ATMCTRLPARA_GROUP;
//=============>ATM专用串口设置结构,DVR型号6003

///DVR云台控制受控参数
typedef struct tagBSRCLIENT_ATMCTRLPARA_B02
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	///设置组
	UCHAR dvraddr;			///录像机地址1-255
	UCHAR AtmSelect;		///ATM选择0-无,1-广电运通,2-日立,3-哥伦比亚,4-印度(此项与串口HUB重复)
	UCHAR ConmmMode;		///通信方式0-无,1-串口232
	UCHAR AtmRecorddelay;	///录像延时10-255
	UCHAR startDepartment;	///是否启动内保局 0否 1是(此项与串口HUB重复)
	UCHAR startRecorddelay;	///是否启动录像延时0否 1是
	UCHAR upload;				    ///是否卡号上传
	UCHAR rev;				
}BSRCLIENT_ATMCTRLPARA_B02, *PBSRCLIENT_ATMCTRLPARA_B02;
///=============>ATM专用串口设置结构,DVR型号B02

///DVR云台控制受控参数
typedef struct tagBSRCLIENT_ATMCTRLPARA_61X
{
	BSCP_CONTROL_ATMCTRLPARA_ITEM uart_ctrl[BSRCLIENT_UARTCOUNT];	///设置组
	UCHAR dvraddr;			///录像机地址1-255
	UCHAR AtmSelect;		///ATM选择0-无,1-广电运通,2-日立,3-哥伦比亚,4-印度(此项与串口HUB重复)
	UCHAR ConmmMode;		///通信方式0-无,1-串口232
	UCHAR AtmRecorddelay;	///录像延时10-255
	UCHAR startDepartment;	///是否启动内保局 0否 1是(此项与串口HUB重复)
	UCHAR startRecorddelay;	///是否启动录像延时0否 1是
	UCHAR upload;				    ///是否卡号上传
	UCHAR rev;				      ///recordDelay 高位
}BSRCLIENT_ATMCTRLPARA_61X, *PBSRCLIENT_ATMCTRLPARA_61X;
///=============>ATM专用串口设置结构,DVR型号B02

//网络卡号配置(此项与网络配置中有重叠)
typedef struct tagBSRCLIENT_NETCARDPROTOCOL
{
	ULONG nRecvIP;			//接收的卡号的IP地址
	USHORT nRecordDelay;	//录像延时:10－600
	UCHAR nProtocol;		//卡号协议BSRCLIENT_CARDPROTOCOL(RJ45部分)
	UCHAR nRes;				//保留字段
}BSRCLIENT_NETCARDPROTOCOL, *PBSRCLIENT_NETCARDPROTOCOL;

//亮度,透明度,对比度获取类别
typedef enum
{
	VEFFECTCMD_BRIGHT	= 0xA,	//亮度
	VEFFECTCMD_TRANS	= 0xB,	//透明度
	VEFFECTCMD_CONTRAST	= 0xC	//对比度
}BSRCLIENT_EFFECTTYPE;

//现场临时字符串显示位置
typedef enum
{
	SCENESTRPOS_360_280	= 0,	//(360,280)
	SCENESTRPOS_60_40,			//(60,40)
	SCENESTRPOS_640_40,			//(640,40)
	SCENESTRPOS_60_450,			//(60,450)
	SCENESTRPOS_640_450			//(640,450)
}BSRCLIENT_SCENESTRINGPOS;

//DVR硬件信息
typedef struct tagBSCP_DVR_GETHARDINFO
{
	CHAR mark[8];				//是否有效的标记:bstar
	CHAR mode[16];				//产品型号
	CHAR sn[16];				//S/N
	CHAR pn[16];				//P/N
}BSCP_DVR_GETHARDINFO, *PBSCP_DVR_GETHARDINFO;

//缓冲区信息
typedef struct tagBSCP_SLAVECPU_BUFINFO
{
	UINT start;			//
	UINT end;			//
	UINT limit;			//
	UINT totalSize;		//
}BSCP_SLAVECPU_BUFINFO, *PBSCP_SLAVECPU_BUFINFO;

//录像参数
typedef struct tagBSCP_SLAVECPU_RECORDPARA
{
	UINT FrameRate;			//帧速
	UINT BitRate;			//码率
	UINT VideoMode;			//0: Pal制式(默认) 1: NTSC

	UINT quality;			//画质
	UINT MotionRecordType;	//移动侦测录像类型
	UINT RecordType;		//类型:普通,报警,定时,移动侦测
	UINT mv_JG;				//设定运动侦测的灵敏度

	UINT max_JG;			//设定编码的关键桢间隔
	UINT filter_level;		//指定燥声级别
	UINT method;			//录像方式:352X288,704X288等
	UINT method_select;		//录像方式的选择
	UINT quantity;			//量化值
}BSCP_SLAVECPU_RECORDPARA, *PBSCP_SLAVECPU_RECORDPARA;

//通道定时备份信息
typedef struct _tagBSCP_DISK_TIMERBACKUPINFO
{
	ULONG backup_day;		//备份天数 1-7
	ULONG enable;			//启动备份 0:否 1:是
	ULONG wHour;			//备份工作时间:小时
	ULONG wMinute;			//备份工作时间:分
	ULONG wSecond;			//备份工作时间:秒
	ULONG bHour;			//备份录像开始时间:小时
	ULONG eHour;			//备份录像结束时间:小时

	ULONG bMinute;			//备份录像开始时间:分钟
	ULONG eMinute;			//备份录像结束时间:分钟
	ULONG bSecond;			//备份录像开始时间:秒
	ULONG eSecond;			//备份录像结束时间:秒
	ULONG rec_type;			//备份录像类型
	ULONG backup_time;		//备份周期:备份天数,星期一 - 星期日
}BSCP_DISK_TIMERBACKUPINFO, *PBSCP_DISK_TIMERBACKUPINFO;

//定时备份信息
typedef struct _tagBSCP_DISK_TIMERBACKUP
{
	BSCP_DISK_TIMERBACKUPINFO time_backup[MAX_CHAN_NUM];		//信息组
}BSCP_DISK_TIMERBACKUP, *PBSCP_DISK_TIMERBACKUP;

//从CPU详细信息
typedef struct tagBSCP_SLAVECPU_SCPUINFO
{
	UINT CurState;					//Current System State
	ULONG PrevState;				//Current System State

	ULONG generalRecordTime;		//普通录像的录像时间
	UCHAR recordType;				//录像类型
	UCHAR recordStarted;			//录像是否已经启动
	UCHAR cpu;						//ID of Slave CPU
	UCHAR state;					//状态
	UINT waitTime;					//Wait Time of Stopping Record
	BSCP_SLAVECPU_BUFINFO bi;		//Buffer Information
	ULONG dataReady[2][16];			//Data Ready Answer Counts
	ULONG totalBlocks;				//Total Record Blocks of 512 bytes
	ULONG leftBlocks;				//Blocks left in buffer
	BSCP_SLAVECPU_RECORDPARA  rp;	//Record Parameters
	ULONG DataStatusUpdate[4];		//Data Status Update
	ULONG transFail;				//Times of transfer failure
	UCHAR isUp;						//Is this CPU UP ?
	UCHAR transfer;					//Is this CPU transfering ?
	UCHAR mode;						//Mode of Transferring data
	UCHAR onlyTransfer;				//Only Transfer ?
	ULONG resetTimes;				//Times of hard reset
	ULONG backup;					//Interrupt Acknowlegements
	UCHAR pause;					//Paused ?
	UCHAR reset;					//Reset ?
	UCHAR stopped;					//Stopped ?
	UCHAR disabled;					//Disabled ?
	ULONG RecordTime;				//录像时间
	ULONG remoteBlocks;				//Slave端产生的数据块
	ULONG localBlocks;				//拿到本地的数据块
	ULONG lostBlocks;				//缓冲区溢出丢掉的数据

	//测试点
	ULONG checkPoints[MAX_CHECKPOINTS_NUM];		//增加的测试点计数
	ULONG PrevInterrupts;			//
	ULONG CurrInterrupts;			//
}BSCP_SLAVECPU_SCPUINFO, *PBSCP_SLAVECPU_SCPUINFO;

//从CPU状态信息
typedef struct tagBSCP_SLAVECPU_SCPUSTATUS
{
	UINT tmAssert;			//is tmAssert ?

	UCHAR CurState;			//Current System State
	UCHAR PrevState;		//Current System State

	UCHAR cpu;				//Which CPU?
	UCHAR type;				//录像类型
	UCHAR ReallyStart;		//录像是否真正启动
	UCHAR res;				//
	USHORT res1;			//

	UINT bitRate;			//码率
	UINT totalBlocks;		//录像块总数

	BSCP_DATATIME sdt;		//从CPU的时间日期
}BSCP_SLAVECPU_SCPUSTATUS, *PBSCP_SLAVECPU_SCPUSTATUS;

//RTP配置信息
typedef struct tagBSRCLIENT_RTPCONFIGINFO
{
	CHAR szIPAddress[16];	//IP地址
	WORD wPort;				//端口
	WORD wMode;				//网络模式0-局域网 1-广域网
}BSRCLIENT_RTPCONFIGINFO, *PBSRCLIENT_RTPCONFIGINFO;

//DVR网络配置信息
#define	INTFS_MAX_NAME_LENGTH			32	//
#define	INTFS_MAX_SMTP_LENGTH			64	//
#define RTCP_VCTL_MAX					4	//控制方法个数

//网络上传报警类型
typedef enum
{
	NETUPEVENT_EVERYTHING_OK	= 0,	//报告正常
	NETUPEVENT_REPORT_IP		= 1,	//报告IP地址
	NETUPEVENT_ALARM_OUT		= 2,	//报警输出
	NETUPEVENT_HDISK_ERROR		= 3,	//磁盘错误
	NETUPEVENT_VIDEO_LOST		= 4,	//视频丢失
	NETUPEVENT_VIDEO_RESTORE	= 5,	//视频恢复
	NETUPEVENT_ALLEVENT			= 6		//所有类型
}BSRCLIENT_UPLOADALARMEVENT;

//组播包大小
typedef enum
{
	NETPACKSIZE_512		= 0,	//512BYTE
	NETPACKSIZE_1024	= 1,	//1024BYTE
	NETPACKSIZE_2048	= 2,	//2048BYTE
	NETPACKSIZE_4096	= 3,	//4096BYTE
	NETPACKSIZE_8192	= 4,	//8192BYTE
	NETPACKSIZE_16384	= 5,	//16384BYTE
}BSRCLIENT_NETPACKSIZE;

//SMTP主机
typedef enum
{
	SMTPHOST_SOHU	= 0,	//搜狐
	SMTPHOST_SINA	= 1,	//新浪
	SMTPHOST_CHINA	= 2,	//中华
	SMTPHOST_OTHER	= 3		//其它
}BSRCLIENT_SMTPHOST;

//DDNS服务商
typedef enum
{
	DDNSSVR_DISABLE		= 0,	//禁用
	DDNSSVR_HUASHENGKE	= 1,	//花生壳
	DDNSSVR_JINWANWEI	= 2		//金万维(仅V6.03/V6.10的B09版本支持此选项)
}BSRCLIENT_DDNSTYPE;

//网络配置结构
typedef struct tagBSCP_NETWORK_NETCFGINFO
{
	UCHAR mode;						//0:手工 1:DHCP 2:ADSL
	UCHAR which;					//第几路报警
	UCHAR network;					//网络环境 1:要回包 0:不要回包
	UCHAR transfer_mode;			//传输方式:0-TCP, 1-单播, 2-主播
	UCHAR delay;					//延迟:0-1024
	UCHAR packet_size;				//包大小BSRCLIENT_NETPACKSIZE

	UCHAR atm_protocol;				//卡号协议:0-禁用,1-成都,2-新疆,3-其它(此项与网络卡号配置重叠,但这里的不起作用)
	UCHAR alarm_method;				//报警发送协议:0-禁用, 1-TCP, 2-UDP

	ULONG MulticastIP;				//组播地址
	USHORT port;					//传输端口
	UCHAR ttl;						//广播包的TTL
	UCHAR res1;						//保留

	ULONG cardSelect;				//接收的卡号的IP地址(此项与网络卡号配置重叠)

	ULONG IPAddress;				//IP地址
	ULONG netMask;					//子网掩码
	ULONG defaultGateway;			//缺省路由

	ULONG alarmCenter;				//报警中心IP地址
	USHORT serial;					//上报IP地址时的序号
	USHORT res2;   
	CHAR ddnsname[INTFS_MAX_NAME_LENGTH];	//花生壳用户名
	CHAR ddnspass[INTFS_MAX_NAME_LENGTH];	//花生壳口令

	ULONG dnsServer;						//域名服务器IP地址
	CHAR mailName[INTFS_MAX_SMTP_LENGTH];	//邮件报警地址
	UCHAR enable_ddns;						//DDNS选项BSRCLIENT_DDNSTYPE
	UCHAR enable_mail;						//是否起用邮件报警

	UCHAR smtphost;					//邮件转发主机BSRCLIENT_SMTPHOST
	UCHAR alarmEvent;				//通过邮件报警的事件BSRCLIENT_UPLOADALARMEVENT

	CHAR mailuser[INTFS_MAX_NAME_LENGTH];	//ESMTP认证用户名
	CHAR mailpass[INTFS_MAX_NAME_LENGTH];	//ESMTP认证口令
	CHAR smtpHost[INTFS_MAX_NAME_LENGTH];	//ESMTP主机名

	CHAR username[INTFS_MAX_NAME_LENGTH];	//用户名
	CHAR password[INTFS_MAX_NAME_LENGTH];	//口令

	ULONG log_server;				//日志服务器IP地址,0表示禁用

	//RTP的可配置选项
	ULONG RTPMaxBitRate;			//RTCP最大码率
	ULONG RTPMinBitRate;			//RTCP最小码率
	USHORT RTPIncreaseStep;			//增加的步长
	USHORT RTPDecreaseStep;			//减小的步长

	UCHAR RTCPEnabled;				//0:不启用,1:启用
	UCHAR applyChanges;				//应用当前更改
	UCHAR AudioEnabled;				//音频开关

	UCHAR RTPFlowControl;			//RTP是否使用流控协议,默认不使用流控

	UCHAR StartReversedAudio;		//启动反向音频
	UCHAR ReversedAudioState;		//反向音频状态

	UCHAR displayRecordTime;		//是否显示最早录像

	UCHAR BitrateControlStrategy[RTCP_VCTL_MAX];	//码率控制策略

	UCHAR NiMode;    //网卡的工作模式,其中最高位为1时有效,
	//低三位的含义0: Auto Negotiation; 1: 100Mbps & Full-duplex;
	//2: 100Mbps & Half-duplex; 3: 10Mbps & Full-duplex; 4: 10Mbps & Half-duplex
	UCHAR res3[3];					//保留
}BSCP_NETWORK_NETCFGINFO, *PBSCP_NETWORK_NETCFGINFO;

/////////////////////////配置结构数据///////////////////////////////
//录像机系统信息(只读),总长度是512个字节
typedef struct tagBSCP_DVR_DEVICESYSINFO_READ
{
	UCHAR r_version;		//读_版本号
	UCHAR w_version;		//写_版本号
	UCHAR encoded;			//是否加密
	UCHAR alarm_in_num;		//报警端子数量(V6.10B09之后版本此字段有效)

	ULONG typeandmode;		//类型及模式
	ULONG subType;			//子类型及模式	BSRCLIENT_DEVSUBTYPE
	ULONG fpStatus;			//录像机状态  位与BSRCLIENT_DEVSTATUS, 当前画质(fpStatus & 0x7) + 1)

	UCHAR slaveCPUs;		//从CPU的个数
	UCHAR channumber;		//通道数
	UCHAR videoNumber;		//视频输入个数
	UCHAR maxDiskNumber;	//最大硬盘个数

	UCHAR transferType;		//传输编码器类型 0-MP4或者1-ASP(自有算法)
	UCHAR encoderType;		//存储编码器类型 0-MP4或者1-ASP(自有算法)
	UCHAR nicNumber;		//网卡数目
	UCHAR uartNumber;		//串口数目

	UCHAR flashType;		//Flash类型
	UCHAR flashSize;		//Flash大小
	SHORT trans_maxwidth;	//传输最大分辨率(宽度),高度固定为288, 704表示最大可为Half D1, 352表示只能CIF

	CHAR sysver[40];		//系统版本号
	ULONG softwarever;		//软件版本号

	UCHAR quarter_status;	//多画面的状态数
	UCHAR hardwarever;		//硬件版本号
	UCHAR ptz_ctrlmode;		//云台控制模式 0-关闭	1-打开
	UCHAR iic_opspeed;		//IIC操作速度 0-低速 1-高速
}BSCP_DVR_DEVICESYSINFO_READ, *PBSCP_DVR_DEVICESYSINFO_READ;

//录像机系统信息(可写),总长度是512个字节
typedef struct tagBSCP_DVR_DEVICESYSINFO_WRITE
{
	UCHAR rtpIncluded;			//是否支持RTP协议
	UCHAR transparentControl;	//透明协议串口

	USHORT year;				//年
	UCHAR month;				//月
	UCHAR day;					//日
	UCHAR hour;					//时
	UCHAR minute;				//分
	UCHAR second;				//秒
	UCHAR bRes;					//保留
	USHORT wRes;				//保留
}BSCP_DVR_DEVICESYSINFO_WRITE, *PBSCP_DVR_DEVICESYSINFO_WRITE;

//录像机系统的信息, 总的长度是1024个字节
typedef struct tagBSCP_DVR_DEVICESYSINFO
{
	union
	{
		UCHAR padding[512];					//字段填充
		BSCP_DVR_DEVICESYSINFO_READ data;	//只读信息
	} r;
	union
	{
		UCHAR padding[512];					//字段填充
		BSCP_DVR_DEVICESYSINFO_WRITE data;	//可写信息
	} w;
}BSCP_DVR_DEVICESYSINFO, *PBSCP_DVR_DEVICESYSINFO;

//菜单透明度范围
typedef enum
{
	TRANSRANGE_MIN	= 20,		//透明度最小值
	TRANSRANGE_MAX	= 255		//透明度最大值
}BSRCLIENT_TRANSRANGE;

//系统设定窗口常规参数
typedef struct tagBSCP_DVR_SYSCFG_GENERAL
{
	UCHAR language;				//语言0:中文 1:English 2:俄文
	UCHAR video_system;			//视频制式1:NTSC 0:PAL
	UCHAR motion_detect;		//移动侦测边框是否显示0:不显示 1:显示   
	UCHAR vga_mode;				//VGA分辨率:0:800X600 1:1024X768
	UCHAR vga_freq;				//VGA频率:0:60HZ 1:75HZ
	UCHAR menu_trans;			//菜单透明度20-255,参考BSRCLIENT_TRANSRANGE
	UCHAR vga_field;			//VGA是否单场输出,0:普通(2场)1:防抖(1场)
	UCHAR rev;					//保留
}BSCP_DVR_SYSCFG_GENERAL, *PBSCP_DVR_SYSCFG_GENERAL;

//系统设定窗口设备标识参数
typedef struct tagBSCP_DVR_PAGEDEVID
{
	ULONG id;					//设备编号1-9999
	UCHAR name[16];				//设备名称
	UCHAR addr[16];				//设备地址
	UCHAR remark[32];			//设备备注
}BSCP_DVR_PAGEDEVID, *PBSCP_DVR_PAGEDEVID;

//系统设定窗口设备标识参数
typedef struct tagBSCP_DVRPAGEDEVIDEX
{
  ULONG id;					    //设备编号1-9999
  UCHAR name[16];				//设备名称
  UCHAR addr[16];				//设备地址
  UCHAR remark[32];			//设备备注
  UCHAR namePos;        //设备名称显示位置
  UCHAR bShow;          //是否显示
  UCHAR res[2];         //保留字
}BSCP_DVR_PAGEDEVIDEX, *PBSCP_DVR_PAGEDEVIDEX;

//通道录像设定状态
typedef struct tagBSCP_RECORD_CHANRECSTAT
{
	UCHAR startup;			//是否启动 0不启动 1启动
	UCHAR quality;          //手动录像画质
	USHORT rev;             //保留
}BSCP_RECORD_CHANRECSTAT, *PBSCP_RECORD_CHANRECSTAT;

//录像设定常规参数
typedef struct tagBSCP_RECORD_PAGEGENERAL
{
	UCHAR rewrite;          //盘满是否自动覆盖 0:否 1:是
	UCHAR full_alarm;       //盘满是否报警 0:否 1:是
	UCHAR mode;             //模式 0:手动 1:自动
	UCHAR channel;          //单路用
	BSCP_RECORD_CHANRECSTAT state[MAX_CHAN_NUM];  //每个通道的状态:是否启动和手动录像的画质
}BSCP_RECORD_PAGEGENERAL, *PBSCP_RECORD_PAGEGENERAL;

//录像参数
typedef struct tagBSCP_RECORD_QUALITY
{
	UCHAR video_size;       //压缩视频的大小 0:CIF 1:DCIF 2:1/2D1 3: 2CIF 4:D1
	UCHAR bit_rate_type;    //码率类型 0:定码率 1:变码率 3:定上限变码流
	UCHAR frame_rate;		//帧率 1-25
	UCHAR filter;			//滤波 无,1,2
	UCHAR Iframe_interval;	//I帧间隔25-200
	UCHAR quantity;			//量化 2-31
	UCHAR audio_bit_rate;	//音频码率32,64 KBPS
	UCHAR quality;			//编码质量
	UCHAR media_type;		//码流类型:  0: 混合流;1 :视频流
	UCHAR rev;				//保留
	USHORT video_bit_rate;	//视频码率 64-4096 KBPS
}BSCP_RECORD_QUALITY, *PBSCP_RECORD_QUALITY;

#define INTERF_QP_MAX_NUM  (6)

//录像设定高级参数
typedef struct tagBSCP_RECORD_PAGEADVANCE
{
	BSCP_RECORD_QUALITY quality[INTERF_QP_MAX_NUM+1];		//画质定义 0-5 6为传输
}BSCP_RECORD_PAGEADVANCE, *PBSCP_RECORD_PAGEADVANCE;

//定时录像时间段
typedef struct tagBSCP_RECORD_TIMERSECT
{
	ULONG bMinute;			//开始时间的分 0-59
	ULONG eMinute;			//结束时间的分 0-59
	ULONG bHour;			//开始时间的小时 0-23
	ULONG eHour;			//结束时间的小时 0-23
	ULONG quality;			//画质 0 - 5
	ULONG start;			//是否启动 0:不启动 1:启动
	ULONG rev;				//保留
}BSCP_RECORD_TIMERSECT, *PBSCP_RECORD_TIMERSECT;

//定时录像参数
typedef struct tagBSCP_RECORD_PAGETIMER
{
	BSCP_RECORD_TIMERSECT dayTimer[MAX_CHAN_NUM][7][4];	//每天4个时间段,一周7天,0-7表示周日,周一...到周六
}BSCP_RECORD_PAGETIMER, *PBSCP_RECORD_PAGETIMER;

//点坐标
#ifndef _TAG_BSCP_INTERF_POINT_
#define _TAG_BSCP_INTERF_POINT_

typedef struct tagBSCP_INTERF_POINT
{
	USHORT x;				//横坐标
	USHORT y;				//纵坐标
}BSCP_INTERF_POINT, *PBSCP_INTERF_POINT;
typedef BSCP_INTERF_POINT  VCA_POINT;
typedef PBSCP_INTERF_POINT  PVCA_POINT;

#endif

//区域坐标
typedef struct tagBSCP_RECT
{
	ULONG left;				//左
	ULONG top;				//上
	ULONG bottom;			//下
	USHORT right;			//右
	USHORT inUse;			//是否在使用中
	USHORT rev;				//保留1
	USHORT rev_t;			//保留2
}BSCP_RECT, *PBSCP_RECT;

//现场设定
typedef struct tagBSCP_RECORD_SCENCE
{
	BSCP_INTERF_POINT mane_xy;		//显示通道名的坐标
	UCHAR mane[MAX_STRINGLEN];		//通道名(最多16个英文字符)
	BSCP_RECT mask_rect;			//遮盖区域
	UCHAR mask_display;				//遮盖状态 0:不遮盖 1:遮盖 2:遮盖但不显示
	UCHAR audio;					//现场音频选择:1-8,无
	UCHAR mane_display;				//是否显示通道名
	UCHAR res;						//保留
}BSCP_RECORD_SCENCE, *PBSCP_RECORD_SCENCE;

//现场设定通道设置参数
typedef struct tagBSCP_RECORD_PAGESCENCE
{
	BSCP_RECORD_SCENCE chanState[MAX_CHAN_NUM];	//
	BSCP_INTERF_POINT pip_xy;						//
}BSCP_RECORD_PAGESCENCE, *PBSCP_RECORD_PAGESCENCE;

//现场设定(扩展版,支持V6.10B09),扩展了遮盖区域数
typedef struct tagBSCP_RECORD_SCENCEEX
{
	BSCP_INTERF_POINT mane_xy;		//显示通道名的坐标
	UCHAR mane[MAX_STRINGLEN];		//通道名(最多16个英文字符)
	BSCP_RECT mask_rect[4];			//遮盖区域
	UCHAR mask_display;				//遮盖状态 0:不遮盖 1:遮盖 2:遮盖但不显示
	UCHAR audio;					//现场音频选择:1-8,无
	UCHAR mane_display;				//是否显示通道名
	UCHAR res;						//保留
}BSCP_RECORD_SCENCEEX, *PBSCP_RECORD_SCENCEEX;

//现场设定通道设置参数(扩展版,支持V6.10B09)
typedef struct tagBSCP_RECORD_PAGESCENCEEX
{
	BSCP_RECORD_SCENCEEX chanState[MAX_CHAN_NUM];	//
	BSCP_INTERF_POINT pip_xy;						//
}BSCP_RECORD_PAGESCENCEEX, *PBSCP_RECORD_PAGESCENCEEX;

//现场设定轮巡设置参数
typedef struct tagBSCP_RECORD_AUTOPATROL
{
	USHORT start;					//是否启动轮巡0: 不启动 1:启动
	USHORT audio;					//现场声音 无,1-8,随动
	USHORT multi_aduio;				//多画面音频 无,1-8
	USHORT mode;					//画面模式:0:单画面
	USHORT time;					//轮巡时间 0-3600秒
	USHORT res;						//
	UCHAR order[MAX_CHAN_NUM];		//轮巡顺序
}BSCP_RECORD_AUTOPATROL, *PBSCP_RECORD_AUTOPATROL;

//布防时间
typedef struct tagBSCP_RECORD_DEFENCETIME
{	
	ULONG bMinute;			//开始时间的分 0-59     
	ULONG eMinute;			//结束时间的分 0-59
	ULONG bHour;			//开始时间的小时 0-23
	ULONG eHour;			//结束时间的小时 0-23   
	ULONG sensity;			//灵敏度 0 - 9         
	ULONG quality;			//画质 0 - 5         
	ULONG start;			//是否启动 0:不启动 1:启动
	ULONG rev;				//保留
}BSCP_RECORD_DEFENCETIME, *PBSCP_RECORD_DEFENCETIME;

//报警联动
typedef struct tagBSCP_RECALARM_OUTMANAGE
{
	UCHAR alarm;        //报警输出,按位操作,1为有效
	UCHAR sound;        //声音提示 0:否 1:是
	UCHAR net_report;   //网络上传 0:否 1:是
	UCHAR PTZ;          //云台联动 0:无 1:预置位 2:轨迹 3:巡航路径
	USHORT rev;			//保留
	UCHAR pre_set;      //预置位号
	UCHAR path;         //巡航路径号
}BSCP_RECALARM_OUTMANAGE, *PBSCP_RECALARM_OUTMANAGE;

#ifdef BSCP_ENUM_VALUE

#define MULTI_DETECT_NUM     4
#define MOTION_DETECTSEC_NUM 4

#endif


//移动侦测时间段
typedef struct tagBSCP_RECORD_MOTIONSECT
{
	BSCP_RECT  md_rect[MULTI_DETECT_NUM];		//侦测区域
	BSCP_RECORD_DEFENCETIME time[MOTION_DETECTSEC_NUM];		//布防时间段
	BSCP_RECALARM_OUTMANAGE alarm_out;			//报警联动
	USHORT start;								//是否启动 0:否 1:是
	USHORT recordType;							//录像类型 0:普通 1:报警 
	USHORT rev;									//保留
	USHORT rec_second;							//录像时间 10 - 3600
}BSCP_RECORD_MOTIONSECT, *PBSCP_RECORD_MOTIONSECT;

//移动侦测页
typedef struct tagBSCP_RECORD_PAGEMOTION
{
	BSCP_RECORD_MOTIONSECT MotionDetect[MAX_CHAN_NUM];	//
}BSCP_RECORD_PAGEMOTION, *PBSCP_RECORD_PAGEMOTION;

//布撤防时间格式
typedef struct tagBSCP_ALARM_DEFENCETIME
{	
	ULONG bMinute;               //开始时间的分 0-59   
	ULONG eMinute;               //结束时间的分 0-59  
	ULONG bHour;                 //开始时间的小时 0-23
	ULONG eHour;                 //结束时间的小时 0-23    
	ULONG start;                 //是否启动 0:不启动 1:启动         
	ULONG rev;                   //保留
}BSCP_ALARM_DEFENCETIME, *PBSCP_ALARM_DEFENCETIME;

//获取/设置布撤防页的数据
typedef struct tagBSCP_ALARM_PAGEDEFENCE
{
	BSCP_ALARM_DEFENCETIME time[7][4];  //布防时间,一周7天,0-7表示周日,周一...到周六
	USHORT nStart;						//是否启动布防 0:否 1:是
	USHORT rev;							//保留
	USHORT out_delay[4];				//报警输出延时 0 - 3600秒
}BSCP_ALARM_PAGEDEFENCE, *PBSCP_ALARM_PAGEDEFENCE;

//报警录像状态
typedef struct tagBSCP_ALARM_ALARMRECORDSTAT
{
	UCHAR startup;			//是否启动 0:不启动 1:启动
	UCHAR quality;          //手动录像画质
	USHORT rev;             //保留
}BSCP_ALARM_ALARMRECORDSTAT, *PBSCP_ALARM_ALARMRECORDSTAT;

//报警异常错误类别
typedef enum
{
	INTERF_DISK_ERROR			= 0,		//磁盘错
	INTERF_DISK_FULL_ERROR,					//盘满
	INTERF_NET_ERROR,						//网络断开
	INTERF_USER_ERROR,						//非法连接
	INTERF_DISK_RW_ERROR,					//磁盘读写错误
	INTERF_FAN_ERROR,						//风扇错误
	INTERF_CHANNEL_ERROR,					//通道错误
	INTERF_DATETIME_ERROR,					//时间错误
	INTERF_MAX_ERROR			= 16		//最大异常数
}ALARM_EXCEPTION_TYPE;

//=============>新报警结构
//报警事件类型(用在BSRCLIENT_ALARMZONESET设置结构中)
typedef enum
{
	BSRALARM_EVENT_INPUT		= 0,		//报警输入,由输入端子决定
	BSRALARM_EVENT_VLOSS		= 1,		//视频丢失,由输入通道决定
	BSRALARM_EVENT_VCOVER		= 2,		//遮拦报警,由输入通道决定
	BSRALARM_EVENT_VMOTION		= 3,		//移动侦测报警,由输入通道决定
	BSRALARM_EVENT_CARDNO		= 4,		//卡号报警,由卡号决定
	BSRALARM_EVENT_EXCEPT		= 5,		//异常报警,不会启动录像
	BSRALARM_EVENT_MAX			= 6			//异常定义守护码
}ALARM_EVENT_TYPE;

//报警时间
typedef struct tagBSCP_ALARM_TIME
{	
	ULONG bMinute;		//开始时间的分 0-59
	ULONG eMinute;		//结束时间的分 0-59
	ULONG bHour;		//开始时间的小时 0-23
	ULONG eHour;		//结束时间的小时 0-23
	ULONG bStart;		//是否启动 0不启动 1启动
	ULONG nRev;			//保留
}BSCP_ALARM_TIME, *PBSCP_ALARM_TIME;

//防区类型
typedef enum
{
	BSRALARMAREA_24HOUR		= 0,
	BSRALARMAREA_PROMPT		= 1,
	BSRALARMAREA_DELAY		= 2,
	BSRALARMAREA_SIDEROAD	= 3
};

//报警防区
typedef struct tagBSCP_ALARMZOME_ITEM
{
	USHORT alarm_type;			//报警触发类型  0:开路 1:闭路
	USHORT alarm_area_type;		//防区类型 0:24小时防区 1:即时防区 2:延时防区 3:旁路
	USHORT alarm_area_delay;	//防区延时 0-3600
}BSCP_ALARMZOME_ITEM, *PBSCP_ALARMZOME_ITEM;

//报警侦测
typedef struct tagBSCP_ALARMDEFENCE_SET
{
	BSCP_ALARM_TIME time[7][4];		//布防时间,一周7天,0-7表示周日,周一...到周六
	USHORT bStart;					//是否启动布防 0:否 1:是
	USHORT nRev;
}BSCP_ALARMDEFENCE_SET, *PBSCP_ALARMDEFENCE_SET;

//报警防区设置
typedef struct tagBSRCLIENT_ALARMZONESET
{
	BSCP_ALARMZOME_ITEM alarmZone[BSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	BSCP_ALARMDEFENCE_SET alarmDefence;
}BSRCLIENT_ALARMZONESET, *PBSRCLIENT_ALARMZONESET;

//报警录像扩展结构
typedef struct tagBSCP_ALARMRECORD_EXT
{
	USHORT mode;		//模式 0:手动 1:自动
	USHORT rec_time;	//报警后录像时间 10 - 3600秒
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];		//启动录像
}BSCP_ALARMRECORD_EXT, *PBSCP_ALARMRECORD_EXT;

//报警轮询
typedef struct tagBSCP_ALARM_SWITCH
{
	USHORT almSwitchStart;	//是否启动报警轮询 0:否 1:是
	USHORT switchtime;		//报警轮询时间0-3600
}BSCP_ALARM_SWITCH, *PBSCP_ALARM_SWITCH;

//报警录像设置/获取
typedef struct tagBSRCLIENT_ALARMRECORD_NEW
{
	BSCP_ALARMRECORD_EXT alarm_rec[BSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	USHORT rec_time[MAX_CHAN_NUM];
	BSCP_ALARM_SWITCH almSwitch;
}BSRCLIENT_ALARMRECORD_NEW, *PBSRCLIENT_ALARMRECORD_NEW;

//报警输出控制
typedef struct tagBSCP_ALARMOUT_CONTROL
{
	USHORT out_delay[4];		//报警输出延时 0 - 3600秒
	ULONG bMinute1;				//开始时间的分 0-59
	ULONG eMinute1;				//结束时间的分 0-59
	ULONG bHour1;				//开始时间的小时 0-23
	ULONG eHour1;				//结束时间的小时 0-23
	ULONG out1;					//有效输出端子,按照位掩码
	ULONG res1;					//保留位1
	ULONG bMinute2;				//开始时间的分 0-59
	ULONG eMinute2;				//结束时间的分 0-59
	ULONG bHour2;				//开始时间的小时 0-23
	ULONG eHour2;				//结束时间的小时 0-23
	ULONG out2;					//有效输出端子,按照位掩码
	ULONG res2;					//保留位2
}BSCP_ALARMOUT_CONTROL, *PBSCP_ALARMOUT_CONTROL;

//报警输出参数
typedef struct tagBSRCLIENT_ALARMOUT_PARA
{
	BSCP_RECALARM_OUTMANAGE alarm_out[BSRALARM_EVENT_MAX][INTERF_MAX_ERROR];
	BSCP_ALARMOUT_CONTROL alarm_out_ctl;
}BSRCLIENT_ALARMOUT_PARA, *PBSRCLIENT_ALARMOUT_PARA;
//<=============新报警结构

//报警输入时间格式
typedef struct tagBSCP_ALARMIN_TIME
{
	USHORT alarm_type;					//报警类型 0:闭路 1:开路
	USHORT alarm_area_type;				//防区类型 0:24小时防区 1:即时防区 2:延时防区 3:旁路
	USHORT rev;							//保留
	USHORT alarm_area_delay;			//防区延时 0 - 3600
	BSCP_RECALARM_OUTMANAGE alarm_out;	//报警联动
}BSCP_ALARMIN_TIME, *PBSCP_ALARMIN_TIME;

//获取/设置报警输入页的数据
typedef struct tagBSCP_ALARM_PAGEALARMIN
{
	BSCP_ALARMIN_TIME alarm_manage[MAX_CHAN_NUM];		//报警输入管理
	BSCP_ALARMIN_TIME vloss_manage[MAX_CHAN_NUM];		//视频丢失管理
	BSCP_ALARMIN_TIME vcover_manage[MAX_CHAN_NUM];		//遮挡报警管理
	BSCP_ALARMIN_TIME vmotion_manage[MAX_CHAN_NUM];		//移动侦测报警管理
	BSCP_ALARMIN_TIME disk_full_manage;					//盘满报警管理
}BSCP_ALARM_PAGEALARMIN, *PBSCP_ALARM_PAGEALARMIN;

//报警录像页数据
typedef struct tagBSCP_ALARM_PAGERECORDSTAT
{
	USHORT mode;                    //模式 0:手动 1:自动
	USHORT rec_time;                //报警后录像时间 10 - 3600秒
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];      //启动录像
}BSCP_ALARM_PAGERECORDSTAT, *PBSCP_ALARM_PAGERECORDSTAT;

//获取/设置报警录像页的数据
typedef struct tagBSCP_ALARM_PAGERECORD
{
	BSCP_ALARM_PAGERECORDSTAT alarm_rec[MAX_CHAN_NUM];	//
}BSCP_ALARM_PAGERECORD, *PBSCP_ALARM_PAGERECORD;

//获取/设置异常页的数据
typedef struct tagBSCP_ALARM_PAGEEXCEPT
{
	BSCP_RECALARM_OUTMANAGE alarm_out[INTERF_MAX_ERROR];	//
}BSCP_ALARM_PAGEEXCEPT, *PBSCP_ALARM_PAGEEXCEPT;

//报警时间控制
typedef struct tagBSCP_ALARM_TIMECTRL
{
	UCHAR tBeginMinute;		//开始时间的分0-59
	UCHAR tEndMinute;		//结束时间的分0-59
	UCHAR tBeginHour;		//开始时间的小时0-23
	UCHAR tEndHour;			//结束时间的小时0-23
	UCHAR nMask;			//报警输出端子,按位操作,1为受控
	UCHAR nStart;			//使能标识,1为有效
	UCHAR res[2];			//保留
}BSCP_ALARM_TIMECTRL, *PBSCP_ALARM_TIMECTRL;

//获取/设置异常页的数据(V6.10B09扩展)
typedef struct tagBSCP_ALARM_PAGEEXCEPTEX
{
	BSCP_RECALARM_OUTMANAGE alarm_out[INTERF_MAX_ERROR];	//
	BSCP_ALARM_TIMECTRL tmZoneCtrl[4];
}BSCP_ALARM_PAGEEXCEPTEX, *PBSCP_ALARM_PAGEEXCEPTEX;

//获取/设置预录页的数据
typedef struct tagBSCP_ALARM_PAGEPRERECORD
{
	UCHAR start;            //是否启动预录
	UCHAR mode;             //模式 0:手动 1:自动
	UCHAR rec_time;         //预录时间 5,10,15,20,25,30,最大
	UCHAR cover_rec;        //遮挡报警录像 0:否 1:是
	UCHAR loss_rec;         //视频丢失录像 0:否 1:是
	UCHAR rev;				//保留
	UCHAR wrev;				//保留
	BSCP_ALARM_ALARMRECORDSTAT state[MAX_CHAN_NUM];
}BSCP_ALARM_PAGEPRERECORD, *PBSCP_ALARM_PAGEPRERECORD;

//串口HUB获取/设置结构
typedef struct tagBSRCLIENT_UARTHUBPARA
{
	UCHAR useHub;			//是否使用串口HUB,0-禁用,1-启用
	UCHAR type[4];			//协议类型0-禁用,1-内保局,2-卡号,3-BSTAR

	//注意:下面的这个结构与ATM新报警结构中完全是重复的
	UCHAR useNBJ;			//是否启用内保局,0-禁用,1-启用
	UCHAR cardType;			//卡号协议类型,0-无,1-广电运通,2-日立,3-哥伦比亚,4-印度
	UCHAR portBaud[5];			//IN口加4个PORT,5个串口的波特率
}BSRCLIENT_UARTHUBPARA, *PBSRCLIENT_UARTHUBPARA;
/////////////////////////配置结构数据///////////////////////////////

typedef enum
{
	ALGBSRALARM_EVENT_INPUT		= 0,		//报警输入,由输入端子决定
	ALGALARM_EVENT_INPUT = 0,    
	ALGBSRALARM_EVENT_VLOSS		= 1,		//视频丢失,由输入通道决定
	ALGALARM_EVENT_VLOSS = 1, 
	ALGBSRALARM_EVENT_VCOVER	= 2,		//遮拦报警,由输入通道决定
	ALGALARM_EVENT_VCOVER = 2,
	ALGBSRALARM_EVENT_VMOTION	= 3,		//移动侦测报警,由输入通道决定
	ALGALARM_EVENT_VMOTION = 3,
	ALGBSRALARM_EVENT_CARDNO	= 4,		//卡号,由卡号决定
	ALGALARM_EVENT_CARDNO = 4,
	ALGBSRALARM_EVENT_EXCEPT	= 5,		//异常报警,不会启动录像,注意异常永远放在倒数第二,如果需要添加报警类型,请在之前添加
	ALGALARM_EVENT_EXCEPT = 5,
	ALGBSRALARM_EVENT_ALG		= 6,		//智能报警,由输入通道决定
	ALGALARM_EVENT_ALG = 6, 
	ALGBSRALARM_EVENT_MAX		= 7,
	ALGALARM_EVENT_MAX = 7  
}ALGALARM_EVENT_TYPE;
//////////////////////////////B02智能信息专用结构,包括部分报警扩展////////////////////////////////////
//=================>智能机型报警扩展
//智能机型报警事件类型(用在ALGBSRCLIENT_ALARMZONESET设置结构中,仅供智能机型使用)
typedef BSCP_ALARMZOME_ITEM  VCA_ALARMZOME_ITEM;
typedef PBSCP_ALARMZOME_ITEM  PVCA_ALARMZOME_ITEM;
typedef BSCP_ALARMDEFENCE_SET VCA_ALARMDEFENCE_SET;
typedef	PBSCP_ALARMDEFENCE_SET PVCA_ALARMDEFENCE_SET;
typedef BSCP_ALARMRECORD_EXT   VCA_ALARMRECORD_EXT;
typedef PBSCP_ALARMRECORD_EXT  PVCA_ALARMRECORD_EXT;
typedef BSCP_ALARM_SWITCH   VCA_ALARM_SWITCH;
typedef PBSCP_ALARM_SWITCH  PVCA_ALARM_SWITCH;

//智能报警防区设置
typedef struct tagALGCLIENT_ALARMZONESET   
{   
	VCA_ALARMZOME_ITEM alarmZone[ALGALARM_EVENT_MAX][MAX_ALARMIN_NUM];   
	VCA_ALARMDEFENCE_SET alarmDefence;   
}ALGCLIENT_ALARMZONESET, *PALGCLIENT_ALARMZONESET; 

//智能报警录像设置/获取
typedef struct tagALGCLIENT_ALARMRECORD   
{   
	VCA_ALARMRECORD_EXT alarm_rec[ALGALARM_EVENT_MAX][MAX_ALARMIN_NUM];   
	USHORT rec_time[MAX_CHAN_NUM];   
	VCA_ALARM_SWITCH almSwitch;   
}ALGCLIENT_ALARMRECORD, *PALGCLIENT_ALARMRECORD;   

//智能检测类别
#ifndef _ENUM_VCA_ALGTYPE_
#define _ENUM_VCA_ALGTYPE_

typedef enum   
{   
	VCA_DECT_MIN = 0, //无效值,小值守护码(但在智能查询时表示全部智能类型)   
	VCA_VIDEO_TOTALNUM, //人数统计   
	VCA_VIDEO_LEFTOBJECT, //遗留物   
	VCA_VIDEO_STUMLINE, //拌线   
	VCA_VIDEO_PRIMITER, //周界保护   
	VCA_VIDEO_NONFACE, //非人脸检测   
	VCA_VIDEO_LOSSOBJECT, //丢失检测   
	VCA_VIDEO_SENTRY , //岗哨检测   
	VCA_VIDEO_CONDUCT,	//操行检测
	VCA_AUDIO_DETECT,  //音频报警   
	VCA_DECT_MAX       //无效值,大值守护码   
}VCA_ALGTYPE;   

#endif

//智能检测区域(注意:智能设置的各种区域边界范围为[(2,2), (350,286)],也就是说有2 像素 的边界)
#ifndef _ENUM_VCA_RECT_
#define _ENUM_VCA_RECT_

typedef enum   
{   
	VCA_RECTWIDTH = 350,  //区域设置的坐标系X 最大值   
	VCA_RECTHEIGHT = 280, //区域设置的坐标系Y 最大值   
	VCA_RECTBORDERX = 2,  //水平方向的边界   
	VCA_RECTBORDERY = 2   //垂直方向的边界   
}; 

#endif

#ifndef _TAG_VCA_LINE_
#define _TAG_VCA_LINE_

typedef struct tagVCA_LINE 
{   
	VCA_POINT begin; //起点   
	VCA_POINT end;   //终点   
}VCA_LINE, *PVCA_LINE;   

#endif

//针对非CIF格式需要自行进行大小比例映射(绘制时) 
#ifndef _TAG_VCA_RECT_
#define _TAG_VCA_RECT_

typedef struct tagVCA_RECT   
{   
	VCA_POINT left_up;    //左上角坐标   
	VCA_POINT right_down; //右下角坐标   
}VCA_RECT, *PVCA_RECT;   

#endif


#ifndef _TAG_VCA_QUAD_
#define _TAG_VCA_QUAD_

typedef struct tagVCA_QUAD   
{   
	VCA_POINT points[4]; //从左上角开始顺时针   
}VCA_QUAD, *PVCA_QUAD;   

#endif

//通用区域参数 
#ifndef _TAG_VCA_SPOT_
#define _TAG_VCA_SPOT_

typedef struct tagVCA_SPOT   
{   
	UCHAR vertices;       //2-16 
	UCHAR enabled;        //YES(1)|NO(0) 
	UCHAR type;           //警戒区域(1) |  辅助区域(2) |  排除区域(3) 
	UCHAR concerns;       //关注的行为集(bitmap)，取值见《智能信息帧格式》 
	VCA_POINT points[16]; //从左上角开始顺时针   
}VCA_SPOT, *PVCA_SPOT; 

#endif

//通用行为参数 
typedef struct tagVCA_PARAMS_56   
{ 
	USHORT delay_time;    //持续时间(秒)，0未设置 
	USHORT alarm_time;    //报警重置时间(秒),10-600,默认为30，仅遗留物/丢失物有效 
	USHORT lo_limit;      //下限(0-255)，0 未设置,    //仅人数统计有效 
	USHORT hi_limit;      //上限(2-255)，0 未设置，//仅人数统计有效 
	USHORT aux_port;      //辅助输入端子(1-8)，0 未设置，仅人脸监测和岗哨有效 
	USHORT scene_choice;
	USHORT stop_overtime;
  USHORT left_scene;    ///* 遗留物场景可选1:普通场景2:ATM场景 */
	UCHAR resx[16];   
} VCA_PARAMS_56, PVCA_PARAMS_56; 

//通用对象参数 
#ifndef _TAG_VCA_OBJECT_
#define _TAG_VCA_OBJECT_

typedef struct tagVCA_OBJECT   
{ 
	USHORT object_height;       //标准探测物实际高度(单位: cm)   
	UCHAR res;   
	UCHAR object_depth;         //目标景深，标的物个数：3--岗哨,1--其他   
	VCA_RECT object_figures[3]; //标准探测物外形轮廓表   
} VCA_OBJECT, PVCA_OBJECT; 

#endif

#ifndef _TAG_VCA_TIME_
#define _TAG_VCA_TIME_

typedef struct tagVCA_TIME   
{   
	UCHAR bMinute; //开始时间的分 0-59   
	UCHAR eMinute; //结束时间的分 0-59   
	UCHAR bHour;   //开始时间的小时 0-23   
	UCHAR eHour;   //结束时间的小时 0-23   
	UCHAR sensitivity; //对象灵敏度0-9,默认值自动0(前景与背景的差异)   
	UCHAR tolerance;   //对象宽容度0-9,默认值5(对象尺寸变化百分比%)   
	UCHAR rev;         //保留   
	UCHAR start;       //是否启动 0:不启动  1:启动   
}VCA_TIME, *PVCA_TIME;  

#endif

//某一类型分析算法   
typedef struct tagVCA_ALG_56   
{   
	UCHAR enabled;    //YES(1)|NO(0) 
	UCHAR algType;    //算法类型，取值VCA_ALGTYPE 
	UCHAR res[2];    //保留； 
	VCA_PARAMS_56 defined_params;    //行为的设置 
	VCA_OBJECT    defined_object; //对象的设置 
	VCA_SPOT defined_spots[4];    //区域的设置   
}VCA_ALG_56, *PVCA_ALG_56;   

typedef struct tagVCA_NONFACE_56   
{   
	VCA_PARAMS_56 defined_params;     //行为的设置 
	VCA_OBJECT    defined_object;  //对象的设置 
	VCA_SPOT defined_spots[4];     //区域的设置   
}VCA_NONFACE_56, *PVCA_NONFACE_56;   

//周界保护阈值定义   
#define VCA_PERIMTER_AREA_NUM (4) //周界区域最大数   
#define VCA_STUM_LINE_NUM (4) //绊线最大数   
#define VCA_LEFT_AREA_NUM (4) //遗留物最大数   
#define VCA_LOSS_AREA_NUM (4) //丢失物最大数   
#define VCA_SENTRY_AREA_NUM (1) //岗哨区域最大数  

//周界保护   
typedef struct tagVCA_PERIMETER_56   
{   
	VCA_PARAMS_56 defined_params;    //行为的设置 
	VCA_OBJECT    defined_object; //对象的设置 
	VCA_SPOT defined_spots[4];    //区域的设置   
}VCA_PERIMETER_56, *PVCA_PERIMETER_56;   

//绊线组设置   
typedef struct tagVCA_STUMLINE_56   
{   
	VCA_PARAMS_56   defined_params;   //行为的设置 
	VCA_OBJECT    defined_object;  //标准探测物设置 
	VCA_SPOT defined_spots[4];     //拌线设置   
}VCA_STUMLINE_56, *PVCA_STUMLINE_56;  

//人数统计   
typedef struct tagVCA_TOTALPEPSONNUM_56   
{   
	VCA_PARAMS_56   defined_params;  //行为的设置 
	VCA_OBJECT    defined_object; //标准探测物设置 
	VCA_SPOT defined_spots[4];    //技术区域设置   
}VCA_TOTALPEPSONNUM_56, *PVCA_TOTALPEPSONNUM_56;   

//遗留物检测   
typedef struct tagVCA_LEFTOBJECT_56   
{   
	VCA_PARAMS_56 defined_params;    //行为的设置 
	VCA_OBJECT    defined_object; //标准探测物设置 
	VCA_SPOT defined_spots[4];    //技术区域设置   
}VCA_LEFTOBJECT_56, *PVCA_LEFTOBJECT_56;   

//丢失物品检测   
typedef struct tagVCA_LOSSOBJECT_56   
{   
	VCA_PARAMS_56 defined_params; //行为的设置 
	VCA_OBJECT    defined_object; //标准探测物设置 
	VCA_SPOT defined_spots[4]; //技术区域设置   
}VCA_LOSSOBJECT_56, *PVCA_LOSSOBJECT_56;   

//岗哨   
typedef struct tagVCA_SENTRY_56   
{   
	VCA_PARAMS_56 defined_params; //行为的设置 
	VCA_OBJECT    defined_object; //标准探测物设置 
	VCA_SPOT defined_spots[4]; //区域设置,包括警戒区[0]/岗哨区[1]/排除区[2-3]   
}VCA_SENTRYOBJECT_56, *PVCA_SENTRYOBJECT_56;   

//某一通道的智能分析总结构 
typedef struct tagVCA_CFG_56   
{   
	UCHAR start; //是否启动   
	UCHAR playDisplay; //是否回放显示,0-不启动,1-启动   
	UCHAR sceneDisplay; //是否现场显示,0-不启动,1-启动   
	UCHAR videoSize; //处理视频的分辨率大小,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,暂时只支持CIF   
	UINT rev; //保留   
	VCA_TIME time[7][4]; //检测时间段,一周7天,0-7表示周日,周一...到周六   
	//具体算法 
	VCA_ALG_56 alg[8];    //每通道最多可启用4 种不同类型算法,通过 alg[i].enabled判断 
	//当前仅使用[0]   
}VCA_CFG_56, *PVCA_CFG_56;   

typedef struct tagVCA_ALGDECTVIDEOPARA_56   
{   
	char channel; //通道编号：1-16 
	char res[3];    // 
	VCA_CFG_56 vcaPara;
}VCA_ALGDECTVIDEOPARA_56, *PVCA_ALGDECTVIDEOPARA_56;

//////////////////////////////智能信息专用结构,包括部分报警扩展////////////////////////////////////
//=================>智能机型报警扩展
//智能机型报警事件类型(用在ALGBSRCLIENT_ALARMZONESET设置结构中,仅供智能机型使用)

//智能报警防区设置
typedef struct tagALGBSRCLIENT_ALARMZONESET
{
	BSCP_ALARMZOME_ITEM alarmZone[ALGBSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	BSCP_ALARMDEFENCE_SET alarmDefence;
}ALGBSRCLIENT_ALARMZONESET, *PALGBSRCLIENT_ALARMZONESET;


//智能报警录像设置/获取
typedef struct tagALGBSRCLIENT_ALARMRECORD
{
	BSCP_ALARMRECORD_EXT alarm_rec[ALGBSRALARM_EVENT_MAX][MAX_ALARMIN_NUM];
	USHORT rec_time[MAX_CHAN_NUM];
	BSCP_ALARM_SWITCH almSwitch;
}ALGBSRCLIENT_ALARMRECORD, *PALGBSRCLIENT_ALARMRECORD;

//智能报警输出参数
typedef struct tagALGBSRCLIENT_ALARMOUT_PARA
{
	BSCP_RECALARM_OUTMANAGE alarm_out[ALGBSRALARM_EVENT_MAX][INTERF_MAX_ERROR];
	BSCP_ALARMOUT_CONTROL alarm_out_ctl;
}ALGBSRCLIENT_ALARMOUT_PARA, *PALGBSRCLIENT_ALARMOUT_PARA;
//<=================智能机型报警扩展

//智能检测类别
typedef enum
{
	BSR_ALG_DECT_MIN		= 0,	  //无效值,小值守护码(但在智能查询时表示全部智能类型)
	BSR_ALG_VIDEO_TOTALNUM,			//人数统计
	BSR_ALG_VIDEO_LEFTOBJECT,		//遗留物
	BSR_ALG_VIDEO_STUMLINE,			//拌线
	BSR_ALG_VIDEO_PRIMITER,			//周界保护
	BSR_ALG_VIDEO_NONFACE,			//非人脸检测
	BSR_ALG_VIDEO_LOSSOBJECT,		//丢失检测

	//添加哨兵监管职能分析
	BSR_ALG_VIDEO_ARMYGUARD,  //智能分析：哨兵监管

	BSR_ALG_AUDIO_DETECT,			//音频报警
	BSR_ALG_DECT_MAX				//无效值,大值守护码
}BSRALGDECT_ALGDECTTYPE;

//智能检测区域(注意:智能设置的各种区域边界范围为[(2,2), (350,286)],也就是说有2像素的边界)
typedef enum
{
	BSR_ALG_RECTWIDTH	= 350,		//区域设置的坐标系X最大值
	BSR_ALG_RECTHEIGHT	= 286,		//区域设置的坐标系Y最大值
	BSR_ALG_RECTBORDERX	= 2,		//水平方向的边界
	BSR_ALG_RECTBORDERY	= 2			//垂直方向的边界
}; 

//针对非CIF格式需要自行进行大小比例映射
#ifndef _TAG_BSRALGDECT_RECT_
#define _TAG_BSRALGDECT_RECT_

typedef struct tagBSRALGDECT_RECT
{
	BSCP_INTERF_POINT left_up;		//左上角坐标
	BSCP_INTERF_POINT right_down;	//右下角坐标
}BSRALGDECT_RECT, *PBSRALGDECT_RECT;

#endif

//时间参数
typedef struct tagBSRALGDECT_TIME
{
	UCHAR bMinute;                  //开始时间的分0-59
	UCHAR eMinute;                  //结束时间的分0-59
	UCHAR bHour;                    //开始时间的小时0-23
	UCHAR eHour;                    //结束时间的小时0-23
	UCHAR sensity;                  //灵敏度0-9,默认值5
	//UCHAR noise;                    //噪声抑制系数0-5,默认值4
	UCHAR tolerance;                //对象宽容度0-9,默认值5(对象尺寸变化百分比%)
	UCHAR rev;						//保留
	UCHAR start;                    //是否启动0:不启动 1:启动
}BSRALGDECT_TIME, *PBSRALGDECT_TIME;

//人脸分析
typedef struct tagBSRALGDECT_NONFACE
{    
	USHORT Detect_Timer;			//检测时间,20~180,默认值20
	USHORT alm_port;				//辅助报警端子,范围0-5,默认5
	BSRALGDECT_RECT Defined_size;	//人脸检测区域
}BSRALGDECT_NONFACE, *PBSRALGDECT_NONFACE;

//周界保护阈值定义
#define BSRALGDECT_PERIMTER_AREA_NUM		(4)		//周界区域最大数
#define BSRALGDECT_STUM_LINE_NUM			(4)		//绊线最大数
#define BSRALGDECT_LEFT_AREA_NUM			(4)		//遗留物最大数
#define BSRALGDECT_LOSS_AREA_NUM			(4)		//丢失物最大数

//周界保护
typedef struct tagBSRALGDECT_PERIMETER
{
	BSRALGDECT_RECT object_size;	//标准探测物设置
	ULONG Define_num;				//有效个数按照位掩码
	BSRALGDECT_RECT Defined_area[BSRALGDECT_PERIMTER_AREA_NUM];	//周界检测区域
}BSRALGDECT_PERIMETER, *PBSRALGDECT_PERIMETER;

//单条拌线设置
typedef struct tagBSRALGDECT_PER_STUMLINE
{
	BSRALGDECT_RECT object_area;	//对象区域
	INT direction;					//方向,1-向外, 2-向内, 0-表示无效
}BSRALGDECT_PER_STUMLINE, *PBSRALGDECT_PER_STUMLINE;

//绊线组设置
typedef struct tagBSRALGDECT_STUMLINE
{     
	BSRALGDECT_RECT object_size;	//标准探测物设置    
	BSRALGDECT_PER_STUMLINE Defined_area[BSRALGDECT_STUM_LINE_NUM];	//拌线设置
}BSRALGDECT_STUMLINE, *PBSRALGDECT_STUMLINE;

//人数统计
typedef struct tagBSRALGDECT_TOTALPEPSONNUM
{  
	USHORT num_limit;				//人数限制,1-99,默认为2
	USHORT delay_time;				//报警延时时间,10-600,默认为30
	BSRALGDECT_RECT object_size;	//标准探测物设置
	BSRALGDECT_RECT Defined_area;	//技术区域设置
}BSRALGDECT_TOTALPEPSONNUM, *PBSRALGDECT_TOTALPEPSONNUM;

//遗留物检测
typedef struct tagBSRALGDECT_LEFTOBJECT
{
	USHORT left_time;				//最大遗留时间,10-600,默认为30
	USHORT alm_resettime;			//报警重置时间,10-600,默认为30
	USHORT Define_num;				//有效个数按照位掩码
	USHORT res;						//保留位
	BSRALGDECT_RECT object_size;	//标准探测物设置
	BSRALGDECT_RECT Defined_area[BSRALGDECT_LEFT_AREA_NUM];	//技术区域设置
}BSRALGDECT_LEFTOBJECT, *PBSRALGDECT_LEFTOBJECT;

//丢失物品检测
typedef struct tagBSRALGDECT_LOSSOBJECT
{
	USHORT loss_time;				//最大丢失时间,10-600,默认为30
	USHORT alm_resettime;			//报警重置时间,10-600,默认为30
	USHORT Define_num;				//有效个数,分别按照位掩码
	USHORT res;						//保留位
	BSRALGDECT_RECT object_size;	//标准探测物设置
	BSRALGDECT_RECT Defined_area[BSRALGDECT_LOSS_AREA_NUM];	//技术区域设置
}BSRALGDECT_LOSSOBJECT, *PBSRALGDECT_LOSSOBJECT;

//视频分析通道结构
typedef struct tagBSRALGDECT_VIDEOPARA_CHAN
{
	UCHAR start;			//是否启动
	UCHAR playDisplay;		//是否回放显示,0-不启动,1-启动
	UCHAR SenceDisplay;		//是否现场显示,0-不启动,1-启动
	UCHAR video_size;		//处理视频的分辨率大小,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,暂时只支持CIF
	USHORT rev;				//保留
	USHORT starAlgType;		//启动智能的类型

	BSRALGDECT_TIME time[7][4];			//检测时间段,一周7天,0-7表示周日,周一...到周六
	BSRALGDECT_PERIMETER perimeter;		//周界保护
	BSRALGDECT_STUMLINE stum_line;		//拌线
	BSRALGDECT_LEFTOBJECT leftobject;	//遗留物体
	BSRALGDECT_TOTALPEPSONNUM TotalNum;	//人数统计
	BSRALGDECT_NONFACE  NonFace;		//非人脸分析
	BSRALGDECT_LOSSOBJECT LossObject;	//丢失物品检测
}BSRALGDECT_VIDEOPARA_CHAN, *PBSRALGDECT_VIDEOPARA_CHAN;

//视频分析配置总结构
typedef struct tagBSRCLIENT_ALGDECTVIDEOPARA
{
	BSRALGDECT_VIDEOPARA_CHAN algPara[MAX_CHAN_NUM];
}BSRCLIENT_ALGDECTVIDEOPARA, *PBSRCLIENT_ALGDECTVIDEOPARA;






//添加新型智能dvr视频智能分析相关结构，与之前的智能设备不兼容
typedef struct tagBSRALGDECT_QUAD
{
	BSCP_INTERF_POINT points[4];  //从左上角开始 
}BSRALGDECT_QUAD, *PBSRALGDECT_QUAD;


//智能分析区域
typedef struct tagBSRALGDECT_POLYGON
{
	unsigned char verices; //2-16;
	unsigned char res[3];
	BSCP_INTERF_POINT points[16];
}BSRALGDECT_POLYGON, *PBSRALGDECT_POLYGON;

#ifndef _TAG_BSRALGDECT_LINE_
#define _TAG_BSRALGDECT_LINE_

typedef struct tagBSRALGDECT_LINE   
{   
	BSCP_INTERF_POINT begin; //起点   
	BSCP_INTERF_POINT end; //终点   
}BSRALGDECT_LINE, *PBSRALGDECT_LINE;

#endif

#define MAX_STDOBJ_NUM 3 //最大检测物个数

#ifndef _TAG_BSRALGDECT_OBJECT_
#define _TAG_BSRALGDECT_OBJECT_

typedef struct tagBSRALGDECT_OBJECT
{
	USHORT object_height; //标准探测物实际高度(单位: cm)  
	UCHAR res; 
	UCHAR object_depth; //目标景深，1 表示仅 object_size 有效，   
	BSRALGDECT_RECT object_figures[MAX_STDOBJ_NUM]; //标准探测物大小   
}BSRALGDECT_OBJECT, *PBSRALGDECT_OBJECT;

#endif


//新DVR周界保护结构定义
typedef struct tagBSRALGDECT_PERIMETER_EX
{
	unsigned short Define_num;    //有效个数按照位掩码
	unsigned short res;
	BSRALGDECT_OBJECT  defined_size;//标准探测物设置
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_PERIMTER_AREA_NUM]; //周界检测区域

}BSRALGDECT_PERIMETER_EX, *PBSRALGDECT_PERIMETER_EX;

//单条拌线设置
#ifndef _TAG_BSRALGDECT_PER_STUMLINE_EX_
#define _TAG_BSRALGDECT_PER_STUMLINE_EX_

typedef struct tagBSRALGDECT_PER_STUMLINE_EX
{
	BSRALGDECT_LINE object_area; //对象区域
	INT direction;     //方向,1-向外, 2-向内, 0-表示无效
}BSRALGDECT_PER_STUMLINE_EX, *PBSRALGDECT_PER_STUMLINE_EX;

#endif

//绊线组设置
#ifndef  _TAG_BSRALGDECT_STUMLINE_EX_
#define  _TAG_BSRALGDECT_STUMLINE_EX_

typedef struct tagBSRALGDECT_STUMLINE_EX
{     
	BSRALGDECT_OBJECT  defined_size;//标准探测物设置
	BSRALGDECT_PER_STUMLINE_EX Defined_area[BSRALGDECT_STUM_LINE_NUM]; //拌线设置
}BSRALGDECT_STUMLINE_EX, *PBSRALGDECT_STUMLINE_EX;

#endif


//人数统计
typedef struct tagBSRALGDECT_TOTALPEPSONNUM_EX
{  
	USHORT num_limit;    //人数限制,1-99,默认为2
	USHORT delay_time;    //报警延时时间,10-600,默认为30
	BSRALGDECT_OBJECT  defined_size;//标准探测物设置
	BSRALGDECT_POLYGON Defined_area; //技术区域设置
}BSRALGDECT_TOTALPEPSONNUM_EX, *PBSRALGDECT_TOTALPEPSONNUM_EX;

//遗留物检测
typedef struct tagBSRALGDECT_LEFTOBJECT_EX
{
	USHORT left_time;    //最大遗留时间,10-600,默认为30
	USHORT alm_resettime;   //报警重置时间,10-600,默认为30
	USHORT Define_num;    //有效个数按照位掩码
	USHORT res;      //保留位
	BSRALGDECT_OBJECT  defined_size;//标准探测物设置
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_LEFT_AREA_NUM]; //技术区域设置
}BSRALGDECT_LEFTOBJECT_EX, *PBSRALGDECT_LEFTOBJECT_EX;

//丢失物品检测
typedef struct tagBSRALGDECT_LOSSOBJECT_EX
{
	USHORT loss_time;    //最大丢失时间,10-600,默认为30
	USHORT alm_resettime;   //报警重置时间,10-600,默认为30
	USHORT Define_num;    //有效个数,分别按照位掩码
	USHORT res;      //保留位
	BSRALGDECT_OBJECT  defined_size;//标准探测物设置
	BSRALGDECT_POLYGON Defined_area[BSRALGDECT_LOSS_AREA_NUM]; //技术区域设置
}BSRALGDECT_LOSSOBJECT_EX, *PBSRALGDECT_LOSSOBJECT_EX;


#define BSRALGDECT_SENTRY_AREA_NUM 16

//岗哨
typedef struct tagBSRALGDECT_SENTRYOBJECT
{
	USHORT shift_port;   //换岗端子(1-6)
	USHORT shift_time;    //换岗持续时间(秒) 
	USHORT Define_num;    //有效个数,分别按照位掩码
	USHORT zones;   ////  bit0 = 为警戒区域, bit1 = 表示岗哨区；2-15表示排除区，目前保留

	BSRALGDECT_OBJECT  defined_size;                              //标准探测物设置
	BSRALGDECT_POLYGON defined_area;  //警戒区域设置   
	BSRALGDECT_POLYGON guard_area;    //卫兵区域设置 

}BSRALGDECT_SENTRYOBJECT, *PBSRALGDECT_SENTRYOBJECT;

//视频分析通道结构
typedef struct tagBSRALGDECT_VIDEOPARA_CHAN_EX
{
	UCHAR start;   //是否启动
	UCHAR playDisplay;  //是否回放显示,0-不启动,1-启动
	UCHAR SenceDisplay;  //是否现场显示,0-不启动,1-启动
	UCHAR video_size;  //处理视频的分辨率大小,1:DCIF 2:CIF 3:1/2D1 4:D1 4:QCIF,暂时只支持CIF
	USHORT rev;    //保留
	USHORT starAlgType;  //启动智能的类型

	BSRALGDECT_TIME time[7][4];   //检测时间段,一周7天,0-7表示周日,周一...到周六
	BSRALGDECT_PERIMETER_EX perimeter;  //周界保护
	BSRALGDECT_STUMLINE_EX stum_line;  //拌线
	BSRALGDECT_LEFTOBJECT_EX leftobject; //遗留物体
	BSRALGDECT_TOTALPEPSONNUM_EX TotalNum; //人数统计
	BSRALGDECT_NONFACE  NonFace;  //非人脸分析
	BSRALGDECT_LOSSOBJECT_EX LossObject; //丢失物品检测
	BSRALGDECT_SENTRYOBJECT sentryObject; //岗哨
}BSRALGDECT_VIDEOPARA_CHAN_EX, *PBSRALGDECT_VIDEOPARA_CHAN_EX;

typedef struct tagCLIENT_ALGDECTVIDEOPARA   
{   
	char channel; //通道编号：1-16 

	char res[3];    // 

	BSRALGDECT_VIDEOPARA_CHAN_EX algPara;

}CLIENT_ALGDECTVIDEOPARA, *PCLIENT_ALGDECTVIDEOPARA;

//新加智能扩展接口
typedef struct tagCLIENT_ALGATMSCENEOBJ //atm全景接口参数结构
{
  unsigned char channel; //通道编号1-16;
  unsigned char algType; //对应的智能类型;
  unsigned char res1[2];
  BSRALGDECT_RECT peopleRec; //探测人员区域
  

}CLIENT_ALGATMSCENEOBJ;

//音频分析通道结构
typedef struct tagBSRALGDECT_AUDIOPARA_CHAN
{
	UCHAR start;				//是否启动
	UCHAR rev[3];				//保留位
	BSRALGDECT_TIME time[7][4];	//检测时间段,一周7天,0-7表示周日,周一...到周六
	UCHAR bottomTresh0;			//范围0~100 
	UCHAR topThresh0;			//范围bottomTresh0~100      
	UCHAR bottomTresh1;			//范围0~100
	UCHAR topThresh1;			//范围bottomTresh0~100
}BSRALGDECT_AUDIOPARA_CHAN, *PBSRALGDECT_AUDIOPARA_CHAN;

//音频分析配置总结构
typedef struct tagBSRCLIENT_ALGDECTAUDIOPARA
{
	BSRALGDECT_AUDIOPARA_CHAN AudioAlm[MAX_CHAN_NUM];
}BSRCLIENT_ALGDECTAUDIOPARA, *PBSRCLIENT_ALGDECTAUDIOPARA;
//////////////////////////////智能信息结构////////////////////////////////////


////////////////////////DVS数据结构定义///////////////////////////////////
typedef struct tagBSRCLIENT_DVS_NICITEM
{
	INT sn;				//编号0-17
	SHORT type;			//类型对于第二块网卡无效
	SHORT port;			//端口对于第二块网卡无效
	ULONG addr;			//ip地址
}BSRCLIENT_DVS_NICITEM, *PBSRCLIENT_DVS_NICITEM;

//DVS第二块网卡及其之后各DVR的网络信息
typedef struct tagBSRCLIENT_DVS_NICGROUP
{
	BSRCLIENT_DVS_NICITEM nicInfo[17];	//网卡信息组
}BSRCLIENT_DVS_NICGROUP, *PBSRCLIENT_DVS_NICGROUP;

//后端DVR状态码
typedef enum
{
	BSRCLIENT_DVSNOTEXIST	= 0,	//不存在
	BSRCLIENT_DVSTIMEOUT	= 1,	//PING超时
	BSRCLIENT_DVSCORRECT	= 2		//正常响应
}BSRCLIENT_DVSDVRSTATUS;

//DVS后端录像机状态
typedef struct tagBSRCLIENT_DVS_DVRSTATUS
{
	UCHAR nDVRState[16];		//具体含义参见BSRCLIENT_DVSDVRSTATUS
}BSRCLIENT_DVS_DVRSTATUS, *PBSRCLIENT_DVS_DVRSTATUS;

typedef struct tagBSRCLIENT_DVS_ALARMZONEITEM
{
	UCHAR bypass;				//旁路防区,暂时禁用,00-使能,01-旁路
	UCHAR alarm_type;			//报警输入类型00-常开,01-常闭
	UCHAR alarm_area_type;		//防区类型00-旁路,01-24小时,02-即时,03-延时
	UCHAR res;					//保留位
}BSRCLIENT_DVS_ALARMZONEITEM, *PBSRCLIENT_DVS_ALARMZONEITEM;

//1644报警输入防区结构
typedef struct tagBSCPDVS_ALARMZONEGROUP
{
	USHORT queue_time;			//音视频排队时间,0-3600秒
	USHORT entry_delay;			//进入延迟,0-3600秒
	USHORT exit_delay;			//退出延迟,0-3600秒
	UCHAR defence_start;		//启动布防
	UCHAR res;					//保留字段
	BSRCLIENT_DVS_ALARMZONEITEM alarmzone[MAX_ALARMIN_NUM];
}BSCPDVS_ALARMZONEGROUP, *PBSCPDVS_ALARMZONEGROUP;

typedef enum
{
	INTERF_M1644_VIDEO_IN	= 16,
	INTERF_M1644_AUDIO_IN	= 16,
	INTERF_M1644_VIDEO_CHAN	= 8,
	INTERF_M1644_UART_CHAN	= 4,
	INTERF_M1644_ALARM_OUT	= 4,
}BSRCLIENT_DVS_MAXVALUE;

typedef struct tagBSRCLIENT_DVS_ALARMOUTITEM
{
	UCHAR alarm[INTERF_M1644_ALARM_OUT];		//继电器关联,共4个,00-不关联,01-关联
	USHORT alarm_time[INTERF_M1644_ALARM_OUT];	//继电器持续时间,0-3600秒
	UCHAR sound;								//蜂鸣器关联,00 不关联,01模式1,02模式2,03模式3
	USHORT sound_time;							//蜂鸣器持续时间,0-3600秒
	UCHAR av_switch;							//自动切换关联,00-不关联,01-视频,02-音视频
	UCHAR ch[INTERF_M1644_VIDEO_CHAN];			//自动切换通道,数组下标代表8个输出,值代表输入,输入为1-16通道
	UCHAR ptz;									//是否关联云台00-不关联,01-关联
	UCHAR ptz_proto;							//云台协议00-PELCOD, 01-PELCOP, 02-JVC
	UCHAR ptz_addr;								//云台地址
	UCHAR ptz_preset;							//云台预置位
}BSRCLIENT_DVS_ALARMOUTITEM, *PBSRCLIENT_DVS_ALARMOUTITEM;

//1644报警输出联动结构
typedef struct tagBSCPDVS_ALARMOUTGROUP
{
	BSRCLIENT_DVS_ALARMOUTITEM alarmout[INTERF_MAX_ERROR];
}BSCPDVS_ALARMOUTGROUP, *PBSCPDVS_ALARMOUTGROUP;

typedef struct tagBSRCLIENT_DVS_SCENESWITCHITEM
{
	UCHAR start;			//开启自动切换00-停止,01 - 开启
	UCHAR mode;				//自动切换模式00视频,01音频,02音视频
	USHORT switch_time;		//自动切换时间间隔,0-3600秒
	USHORT channel;			//自动切换通道号按位操作
	USHORT wres;			//保留字段
}BSRCLIENT_DVS_SCENESWITCHITEM, *PBSRCLIENT_DVS_SCENESWITCHITEM;

//1644现场自动切换状态
typedef struct tagBSRCLIENT_DVS_SCENESWITCHGROUP
{
	BSRCLIENT_DVS_SCENESWITCHITEM videoch[INTERF_M1644_VIDEO_CHAN];
}BSRCLIENT_DVS_SCENESWITCHGROUP, *PBSRCLIENT_DVS_SCENESWITCHGROUP;

typedef struct tagBSRCLIENT_DVS_DEVCONFIGITEM
{
	UCHAR baud_rate;		//波特率00-1200, 01-2400, 02-4800, 03-9600
	UCHAR bres;				//保留位
	UCHAR wres;				//保留位
}BSRCLIENT_DVS_DEVCONFIGITEM, *PBSRCLIENT_DVS_DEVCONFIGITEM;

//1644硬件配置
typedef struct tagBSRCLIENT_DVS_DEVCONFIGGROUP
{
	UCHAR targetid;			//644EN主机地址
	UCHAR dvr_proto;		//00-BSR-DVR协议
	UCHAR ptz_proto;		//00:PELCO_D, 01:PELCO_P, 02:JVC云台协议
	UCHAR res;				//保留位
	BSRCLIENT_DVS_DEVCONFIGITEM uart[INTERF_M1644_UART_CHAN];	//0-键盘,1-录像机,2-云台,3-RS232
}BSRCLIENT_DVS_DEVCONFIGGROUP, *PBSRCLIENT_DVS_DEVCONFIGGROUP;

//1644EN网络控制状态
typedef struct tagBSRCLIENT_DVS_NETCTLSTATUS
{
	INT PanelLock;			//00-网络不可控,01-网络可控
}BSRCLIENT_DVS_NETCTLSTATUS, *PBSRCLIENT_DVS_NETCTLSTATUS;

///////////声讯协议结构///////////
//当前注册状态
#ifndef _TAG_NET_SEC_BUREAU_STATE_
#define _TAG_NET_SEC_BUREAU_STATE_

typedef struct tagNET_SEC_BUREAU_STATE
{
	SHORT status;		//当前注册状态，小于0为离线； WAITING(-2), TRYING(-1), OFFLINE(0), ONLINE(1)
	SHORT error;		//上次注册返回的错误码
	UINT  serverAddr;	//当前服务器地址
} BSRCLIENT_NET_SEC_BUREAU_STATE, *PBSRCLIENT_NET_SEC_BUREAU_STATE;

#endif

//注册服务器设置
#ifndef _TAG_NET_SEC_BUREAU_CFG_
#define _TAG_NET_SEC_BUREAU_CFG_

typedef struct tagNET_SEC_BUREAU_CFG
{
	CHAR	enabled;	//NO(0) | YES(1) | WAKEN(2), 是否启用注册功能,缺省2 
	CHAR	dial;		//NONE(0)|PPPOE(1)|CDMA(2)
	CHAR	res[2];
	UINT	serverAddr1; //注册服务器IPAddr,缺省为0 
	UINT	serverAddr2; //注册服务器IPAddr,缺省为0
	USHORT	serverPort; //注册服务器端口
	USHORT	resx; 
}BSRCLIENT_NET_SEC_BUREAU_CFG, *PBSRCLIENT_NET_SEC_BUREAU_CFG;

#endif

////////////////////////DVS数据结构定义///////////////////////////////////


/////////////////////////新加网络卡号////////////////////////////////////////
/*  atm卡号协议结构体   */

typedef struct
{
  unsigned char link;     /*  连接方式    */
  /*  RS232(1)    RS485(2)    RS232Hub(3) RJ45(4) */

  unsigned char proto;    /*  卡号协议    */
  unsigned char forwards; /*  网络转发内容:NONE(0)    CARDNO(1)   */
  unsigned char res;  /*  reserved    */
  unsigned long addr;  /*  捕获器地址  IP地址或rs485地址   */
  unsigned int period;    /*  持续时间(录像时长,单位 秒)  */

  /* proto卡号协议
  * 协议编号： 
  *
  * 网络(link=RJ45)： 
  * 新疆(1)|成都(2)|其它(3)|印度(4)|
  * 宁夏(5)|哥伦比亚(6)上海(7) 
  *
  * 串口(link=RS232)： 
  * 广电运通(1)|日立(2)|哥伦比亚(3)|
  * 印度(4)|NCR(5)|蓝天(6) 
  */

}BSRCLIENT_NETCARDCFG;


/*断网检测配置*/
struct BSRCLIENT_NETDETECT
{
  unsigned long serverip;      /*参考IP，默认是124.65.141.254*/
  unsigned char retry;         /*ping的次数*/
  unsigned char controlport;   /*ping功能控制端口*/
  unsigned char timerenabled;  /*定时ping功能是否开启，1:开启，0:关闭,缺省关闭*/
  unsigned char recv0;                                 
  BSCP_DATATIME DateTime1;         /*定时ping 时间点1*/
  BSCP_DATATIME DateTime2;         /*定时ping 时间点2*/
  unsigned char periodicenable; /*定时间间隔ping功能是否开启，1:开启，0:关闭,缺省关闭*/
  unsigned char recv[3];                                   
  unsigned long periodictime;    /*时间间隔,单位是秒*/

} ;

/*主动注册外设配置*/
struct BSRCLIENT_RIGISTEREXP
{
  char enabled; //0 do not enable 
                //1 enable 正常转发内保局产生的报警 
                // 2 enable 但是不转发内保局报警
  char res; 
  char link; //!连接类型：报警输入(1) 
  char port; //查扣号：1-4
};

/**主动注册参数配置*/
struct BSRCLIENT_AUTOREGISTER
{
  char            enabled;    //!是否启用注册功能,缺省 0   //NO(0) | YES(1) 
  char            res[3]; 
  unsigned int    serverAddr;  //注册服务器 IPAddr,缺省为 0  
  unsigned short  serverPort;  //注册服务器端口，缺省为 8008 
  unsigned char   retries;  //连续失败的最大重试次数 //0 为永久重试 //缺省为5  
  unsigned char   res2;   
  unsigned int serverKey;   //注册服务器需要的认证码， //缺省为123456 
	unsigned int register_standby;    //注册失败后，再次发起注册的时间间隔(单位:秒)
};

/** 重连间隔 */
struct BSRCLIENT_STANDBY
{
  unsigned int register_standby;  //服务器standby时间
};

/** 自动注册状态，该状态只读 */
struct BSRCLIENT_REGISTERSTAT
{
  short status;   //!当前注册状态,<=0 为离线 
                 //WAITING(-2)|TRYING(-1)|  
                 //OFFLINE(0) |ONLINE (1)| 
  short error;   //上次注册返回的错误码 
  char  serverName[16];   //当前注册成功的服务器名称
};

/** 通道报警上传状态 */
struct BSRCLIENT_CHNWARNREPORTSTAT 
{
  unsigned long device_num; /* dvr 编号 从0开始 */ 
  unsigned long state[16]; /* 16代表1-16通道 */ /* 状态按位定义 转发与否 YES(1) NO(0)*/ /* bit1-32:bitmap 报警使能(8)|报警禁止(7) |布防(6)|撤防(5)| 硬盘错误(4)|视频丢失(3)| 遮挡(2)|移动侦测(1)| 报警输入(0) */
};

/** 内保局协议设置 */
struct BSRCLIENT_REGISTERAGENT
{
  char enabled; //（开关）
  char proto; //内保局(1) 
  char link; //!连接类型： //RS232 (1)|RS485 
  char port; //端口号：1-4
};

/** 手动上下线 */
struct BSRCLIENT_MANUALOPRATING
{
  char res; //保留
};
////////////////////////////API//////////////////////////////////////////////
//错误码操作
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetLastError();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetLastError(LONG nError);

//常规操作
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_IsSupport();
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetSDKVersion();
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetSDKPubishName();
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetSDKUpdateInfos();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistMessage(HWND hWnd, UINT nMessage);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetConnectTime(DWORD dwWaitTime, DWORD dwTryTimes);
BSRCLIENT_API VOID BSRCLIENT_STDCALL BSR_DVR_SetDebugPara(LONG nLevel, BOOL bScopeMode);

//SDK初始化
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_InitSDK();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DeInitSDK();

//登录相关
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_Login(LPCSTR lpDVRIP, WORD wDVRPort, LPCSTR lpUserName, LPCSTR lpPasswd, BSRCLIENT_DEVICEINFO *lpDeviceInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_Logout(HANDLE hUserHandle);

//预览相关
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_StartRealPlay(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopRealPlay(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetChannelTitle(HANDLE hRealHandle, LPCSTR lpszTitle, BOOL bDVRShow);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetChannelTitleOther(HANDLE hUserHandle, LONG nChannel, LPCSTR lpszTitle, BOOL bDVRShow);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDScenePara(HANDLE hRealHandle, BOOL bShow, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDTimePara(HANDLE hRealHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetOSDTitlePara(HANDLE hRealHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistDrawFunc(HANDLE hRealHandle, CLIENTTITLEDRAWFUN lpDrawFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RegistWndHookFunc(HANDLE hRealHandle, CLIENTHWNDHOOKFUN lpWndFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetRealDataCallBack(HANDLE hRealHandle, CLIENTDATACALLBACKFUN lpfnReal, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetRealDataCallBackXM( HANDLE hRealHandle, fRealDataXM lpfnReal, LPVOID lpVoid );

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CaptureBMPPicture(HANDLE hRealHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SaveRealData(HANDLE hRealHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopSaveRealData(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetRealSaveStat(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetPlayerBufNumber(HANDLE hRealHandle, DWORD dwBufNum);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoEffectByReal(HANDLE hRealHandle, LONG *lpBright, LONG *lpTrans, LONG *lpContrast);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoEffectByReal(HANDLE hRealHandle, UINT nType, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoEffectByUser(HANDLE hUserHandle, LONG nChannel, LONG *lpBright, LONG *lpTrans, LONG *lpContrast);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoEffectByUser(HANDLE hUserHandle, LONG nChannel, UINT nType, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_OpenSound(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseSound();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVolume(HANDLE hRealHandle, DWORD dwVolume);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamPara(HANDLE hRealHandle, UINT nType, INT *lpValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVideoStreamPara(HANDLE hRealHandle, UINT nType, INT nValue);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_GetUserHandle(HANDLE hRealHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetPlayerPort(HANDLE hRealHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetStreamID(HANDLE hRealHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetIconState(HANDLE hRealHandle, BOOL *lpVideoCap, BOOL *lpAudioCom);

//现场流控制函数，此类函数只负责打开、关闭现场流ID

//申请现场流
BSRCLIENT_API int  BSRCLIENT_STDCALL BSR_DVR_RealStreamOpen(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
//关闭现场流
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamClose(HANDLE hUserHandle, int streamId );
//开始，如果是被动模式，必须调用此函数开始数据传输，若是主动模式则在调用BSR_DVR_RealStreamStop函数之后开始现场流传输
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamStart(HANDLE hUserHandle, int streamId );
//暂停现场流传输，无论是主动还是被动
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealStreamPause(HANDLE hUserHandle,  int streamId );

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RealCapOneBmp( HANDLE hDvr, int chnnl, int port, HWND hWnd, LPCSTR pathname, BOOL bShowTime );

//语音对讲
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_VoiceComStartByUser(HANDLE hUserHandle, BSRCLIENT_CLIENTINFO *lpClientInfo);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_VoiceComStartByReal(HANDLE hRealHandle, WORD wDVRPort);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_VoiceComStop(HANDLE hVoiceHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_VoiceComSendData(HANDLE hVoiceHandle, BYTE *lpBuffer, DWORD dwSize);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetTalkStreamPara(HANDLE hVoiceHandle, UINT nType, INT *lpValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetTalkStreamPara(HANDLE hVoiceHandle, UINT nType, INT nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetVoiceStatus(HANDLE hUserHandle, LONG nStatus, LONG nChannel);



//音频采集
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientAudioStart(BSRCLIENT_AUDIOWAVEIN *lpWaveInPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientSetVoiceCallBack(VOICEDATACALLBACKFUN lpfnWaveProc, LPVOID lpVoid, UINT nType);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClientAudioStop();

//流相关控制
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RtpStateCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PurgeVideoStream(HANDLE hUserHandle, LONG nSessionID);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamList(HANDLE hUserHandle, BSRCLIENT_STREAMLIST *lpList);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetVideoStreamInfo(HANDLE hUserHandle, BSRCLIENT_STREAMINFOS *lpInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseVideoStream(HANDLE hUserHandle, LONG nStreamID);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PurgeTalkStream(HANDLE hUserHandle, LONG nSessionID);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetTalkStreamList(HANDLE hUserHandle, BSRCLIENT_STREAMLIST *lpList);

//用户信息
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetSessionID(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetObjectIP(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetCurUserName(HANDLE hUserHandle);
BSRCLIENT_API LPCSTR BSRCLIENT_STDCALL BSR_DVR_GetCurPassword(HANDLE hUserHandle);

//DVR用户操作
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DeleteUser(HANDLE hUserHandle, DWORD dwUserID);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ModifyUser(HANDLE hUserHandle, DWORD dwUserID, LPCSTR lpszUserName, LPCSTR lpszPasswd, DWORD dwUserGrade);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_AddUser(HANDLE hUserHandle, DWORD dwUserID, LPCSTR lpszUserName, LPCSTR lpszPasswd, DWORD dwUserGrade);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetLoginUsers(HANDLE hUserHandle, BSCP_USER_CURLOGINUSR *lpUsers);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetAllUsers(HANDLE hUserHandle, BSCP_USER_ALLUSER *lpUsers);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_KickNetUser(HANDLE hUserHandle, LONG nSession);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDialogInfos(HANDLE hUserHandle, BSRCLIENT_DIALOGINFOS *lpInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetTransInfos(HANDLE hUserHandle, BSRCLIENT_TRANSCHANINFO *lpInfos);

//报警相关
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StartListen(LPCSTR lpszLocalIP, WORD wLocalPort);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_StopListen();
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDVRMsgCallBack(MESSAGECALLBACKFUN lpfnMsgCallBack, LPVOID lpVoid);

//告警管理
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DefenceCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PreRecordCtrl(HANDLE hUserHandle, BOOL bStart);

//面板相关
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendVirtualKey(HANDLE hUserHandle, INT nVirtualKey);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetPassApplyStat(HANDLE hUserHandle, BOOL bApply);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetPassApplyStat(HANDLE hUserHandle, BOOL *lpApply);

//云台操作
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZConfirmCruisePath(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZGetAllCruisePath(HANDLE hUserHandle, BSCP_CONTROL_CRUISEPOSLIST *lpAllCruisePath);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZPreset(HANDLE hRealHandle, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZPresetOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPresetCmd, DWORD dwPresetIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAddCruisePos(HANDLE hRealHandle, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex, DWORD dwPresetIndex, DWORD dwSpeed, DWORD dwStopTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAddCruisePosOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex, DWORD dwPresetIndex, DWORD dwSpeed, DWORD dwStopTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZDelCruisePos(HANDLE hRealHandle, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZDelCruisePosOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, DWORD dwPTZDotIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZCruiseCtrl(HANDLE hRealHandle, DWORD dwPTZPathIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZCruiseCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZPathIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZTrackCtrl(HANDLE hRealHandle, DWORD dwPTZCommand);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZTrackCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwPTZCommand);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAssistCtrl(HANDLE hRealHandle, DWORD dwAuxIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZAssistCtrlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwAuxIndex, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZControl(HANDLE hRealHandle, DWORD dwCtrlPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PTZControlOther(HANDLE hUserHandle, LONG nChannel, DWORD dwCtrlPara);

//现场操作
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ScenePatrolCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SceneDisplayInfos(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SceneDisplayString(HANDLE hUserHandle, LPCSTR lpszString, UINT nPos);

//录像管理
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RecordCtrl(HANDLE hUserHandle, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ModifyQuality(HANDLE hUserHandle, LONG nChannel, LONG nQuality);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DVRLocalPlayCtrl(HANDLE hUserHandle, DWORD dwPlayCommand);

//从CPU控制
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUReboot(HANDLE hUserHandle, UINT nCPUIndex);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPURecordCtrl(HANDLE hUserHandle, LONG nChannel, BOOL bStart);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUGetStates(HANDLE hUserHandle, LONG nChannel, BSCP_SLAVECPU_SCPUINFO *lpStates);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SCPUGetStatus(HANDLE hUserHandle, LONG nChannel, BSCP_SLAVECPU_SCPUSTATUS *lpStatus);

//参数配置
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDVRConfig(HANDLE hUserHandle, DWORD dwCommand, LPVOID lpOutBuffer, DWORD dwBufSize, LPDWORD lpBytesRecved);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDVRConfig(HANDLE hUserHandle, DWORD dwCommand, LPVOID lpInBuffer, DWORD dwBufSize);

//设备相关信息
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetBriefDevInfo(HANDLE hUserHandle, PBSRCLIENT_DEVICEINFO lpDeviceInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceSysInfos(HANDLE hUserHandle, BSCP_DVR_DEVICESYSINFO *lpDevinfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceStatus(HANDLE hUserHandle, BSCP_DVR_DEVICESTAT *lpDevStatus);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceInfos(HANDLE hUserHandle, BSCP_DVR_DEVICEINFO *lpDevInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetActualChans(HANDLE hUserHandle, BSRCLIENT_ACTUALCHANS *lpChanInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceLoadInfos(HANDLE hUserHandle, BSRCLIENT_DVRLOADINFO *lpDVRLoadInfos);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDeviceRunInfos(HANDLE hUserHandle, BSRCLIENT_DVRRUNINFO *lpDVRRunInfos);

//控制相关
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendEmail(HANDLE hUserHandle, LPCSTR lpszContent);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetCardNumber(HANDLE hUserHandle, LPCSTR lpszCardNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DVRLocalControl(HANDLE hUserHandle, DWORD dwCommand, DWORD dwCtrlPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_Reboot(HANDLE hUserHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RestoreConfig(HANDLE hUserHandle);

//串口代理
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetUartAgentPara(HANDLE hUserHandle, DWORD dwLinkType, DWORD dwPort, BSRCLIENT_UARTAGENTPARA *lpAgentPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetUartAgentPara(HANDLE hUserHandle, BSRCLIENT_UARTAGENTPARA *lpAgentPara);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SendUartData(HANDLE hUserHandle, DWORD dwLinkType, DWORD dwPort, BYTE *lpBuffer, DWORD dwDataSize);

//日志操作
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindDVRLogs(HANDLE hUserHandle, LPCSTR lpszOperator, DWORD dwMajorType, DWORD dwMinorType, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpLogCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextLog(HANDLE hLogHandle, LONG nPageIndex, BSCP_DVR_LOG *lpLogData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindLogClose(HANDLE hLogHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ClearAllLogs(HANDLE hUserHandle, DWORD dwDiskNumber);

//远程升级
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_RemoteUpgrade(HANDLE hUserHandle, WORD wDVRPort, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_CloseUpgradeHandle(HANDLE hUpgradeHandle);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_GetUpgradeState(HANDLE hUpgradeHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_MakeUpgradeDisk(HANDLE hUserHandle);

//磁盘操作
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SleepHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_WakenHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FormatHardDisk(HANDLE hUserHandle, DWORD dwDiskNumber);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_GetDiskInfo(HANDLE hUserHandle, DWORD dwDiskNumber, BSRCLIENT_DISKINFO *lpDiskInfo);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_SetDiskType(HANDLE hUserHandle, DWORD dwDiskNumber, UINT nType, UINT nSubType);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_WriteDisk(HANDLE hUserHandle, LONG nLBAAddress, LONG nCount, LONG nValue);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ChangeDisk(HANDLE hUserHandle, DWORD dwDiskNumber);

//文件检索

BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByTime(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByTimeEx(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, UINT beginTime, UINT endTime, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileByCard(HANDLE hUserHandle, LONG nChannel, DWORD dwFileType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LPCSTR lpszCardNumber, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextFile(HANDLE hFindHandle, LONG nPageIndex, BSRCLIENT_FILELIST *lpFileData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileClose(HANDLE hFindHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileConvert(HANDLE hUserHandle, UINT64 nFileHandle, BSRCLIENT_FILETIME2LEN *lpInfos);

//智能检索下载
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_AlgFindFileByTime(HANDLE hUserHandle, LONG nChannel, DWORD dwAlgType, DWORD dwDiskNumber, SYSTEMTIME *lpBegin, SYSTEMTIME *lpEnd, LONG *lpPageCount, LONG *lpFileCount);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_AlgDownFileByName(HANDLE hUserHandle, WORD wDVRPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nDownSize, LPCSTR lpszSaveFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_AlgFindFileConvert(HANDLE hUserHandle, UINT64 nFileHandle, BSRCLIENT_FILETIME2LEN *lpInfos);

//文件检索扩展接口
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_FindFileEx(HANDLE hUserHandle, BSRCLIENT_REMOTEFINDEX *lpFindCond);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindNextFileEx(HANDLE hFindHandleEx, LONG nPageIndex, BSRCLIENT_FILELIST *lpFileData, DWORD *lpdwCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_FindFileCloseEx(HANDLE hFindHandleEx);

//远程下载
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_DownFileByName(HANDLE hUserHandle, WORD wDVRPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nDownSize, LPCSTR lpszSaveFileName);
BSRCLIENT_API LONG BSRCLIENT_STDCALL BSR_DVR_DownFileGetPos(HANDLE hDownHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DownFileStop(HANDLE hDownHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_DownSetDataCallBack(HANDLE hDownHandle, CLIENTDATACALLBACKFUN lpfnCallBackFun, LPVOID lpVoid);

//远程回放
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_ChangeCurTransRate(HANDLE hUserHandle, LONG nBitrate);
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVR_PlayBackByName(HANDLE hUserHandle, WORD wDVRPort, WORD RealPort, UINT64 nFileHandle, UINT64 nOffset, UINT64 nFileSize, HWND hWnd, DWORD dwDeviceNum, BOOL bFrameMode = FALSE, BOOL bShowTime = TRUE );
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetPlayList(HANDLE hPlayHandle, PBSCP_NETTRANS_FILEINFO pzfileInfo, DWORD nFileCount);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackPlayListSetTimePos(HANDLE hPlayHandle, DWORD time);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackStop(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackCaptureBMPPicture(HANDLE hPlayHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackRefresh(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackGetOsdTime(HANDLE hPlayHandle, SYSTEMTIME *lpOsdTime);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetOSDTimePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetOSDTitlePara(HANDLE hPlayHandle, BOOL bShow, POINT *lpPoint, COLORREF *lpColor);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSetDataCallBack(HANDLE hPlayHandle, CLIENTDATACALLBACKFUN lpfnCallBackFun, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackWndHookFunc(HANDLE hPlayHandle, CLIENTHWNDHOOKFUN lpfnWndFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackDrawFunc(HANDLE hPlayHandle, CLIENTTITLEDRAWFUN lffnDrawFunProc, LPVOID lpVoid);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackSaveData(HANDLE hPlayHandle, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackStopSave(HANDLE hPlayHandle);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_PlayBackControl(HANDLE hPlayHandle, DWORD dwControlCode, DWORD dwInValue, DWORD *lpOutValue);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_RecordBMPCapture( HANDLE hUserHandle, int chnnl, int port, SYSTEMTIME time, LPCSTR pathname, HWND hWnd );

//DVS专用接口
BSRCLIENT_API HANDLE BSRCLIENT_STDCALL BSR_DVS_FrontBoardUpgrade(HANDLE hUserHandle, WORD wDVSPort, LPCSTR lpszFileName);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_GetBackDVRStatus(HANDLE hUserHandle, BSRCLIENT_DVS_DVRSTATUS *lpDVRStatus);
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_SendRS485Command(HANDLE hUserHandle, LPCSTR lpszCmdChar, DWORD dwCMDSize);

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVS_Control0816AlarmHost(HANDLE hUserHandle, UINT nHostID, LPCSTR lpszCmdChar, DWORD dwCMDSize);
//////////////////////////////////////////////////////////////////////////

//DVS声讯接口
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_NetSecBureauLogon( HANDLE hUserHandle );
BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_NetSecBureauLogoff( HANDLE hUserHandle );


/*
  fn:BSR_DVR_OutputControl
  brief:输出端子控制
  param[in]: hUserHandle dvr句柄
  param[in]: port 输出端子 0-3
  param[in]: bOpen TRUE = 输出端子开; FALSE = 输出端子闭合
*/

BSRCLIENT_API BOOL BSRCLIENT_STDCALL BSR_DVR_OutputControl( HANDLE hUserHandle, unsigned char port, BOOL bOpen );


//////////////////////////////////////////////////////////////////////////
#endif //__BLUESTAR_BSRCLIENT_INTERFACE_H__