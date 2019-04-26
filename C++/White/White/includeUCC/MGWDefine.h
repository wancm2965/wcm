#pragma once

// [7/28/2009 lzy]
//错误码最高位 

typedef enum
{
	//系统
	TYPE_SYS=0,			//系统
	TYPE_MTS,			//服务器
	TYPE_TOOLS,			//网关工具
	TYPE_ALARM,			//报警

	//DVR
	TYPE_HP=0x100,		//华平DVR	
	TYPE_HIK,			//海康DVR
	TYPE_DH,			//大华DVR
	TYPE_KEDA,			//科达DVR
	TYPE_HHS,			//黄河DVR
	TYPE_HBT,			//汉邦DVR
	TYPE_YAA,			//天津亚安
	TYPE_TDY,			//天地伟业
	TYPE_NCI,			//华北研究所
	TYPE_JTN,			//杭州竞天
	TYPE_TLV,			//上海通琅
	TYPE_BSC,			//蓝卫通
	TYPE_PLT,			//平台
	TYPE_XINQI,			//芯启
	TYPE_SKY,			//蓝色星际

	//RTSP
	TYPE_RTSP=0x200,	//RTSP协议
}ERROR_TYPE;

//enum {
//	//DVR/DVS/NVS/IPCAM
//	HPD_DVR = 0, /*华平*/
//	HIK_DVR, /*海康*/
//	DAH_DVR, /*大华*/
//	KEDA_DVR, /*科达*/
//	HHS_DVR, /*黄河*/
//	HBT_DVR, /*汉邦高科*/
//	YAA_DVR, /*天津亚安*/
//	TDY_DVR, /*天地伟业*/
//	CJV_DVR, /*十五所*/
//	JTN_DVR, /*杭州竞天*/
//	PLT_DVR, /*平台*/
//	TLV_DVR, /*上海通琅*/
//	BSC_DVR, /*蓝卫通*/
//	MAX_DVR,
//
//	HPD_MOB = 0x99,/*华平手机*/
//	HDS1000,/*单兵设备*/
//	NDS410,/*华平DVR*/
//
//	//NDS(OEM)
//	NDS_100=(0x0100|HIK_DVR),/*海康*/
//	NDS_200=(0x0200|HIK_DVR),/*海康*/
//	NDS_300=(0x0300|HHS_DVR),/*黄河*/
//	MAX_NDS,
//
//	//RTSP
//	RTSP_HHS = 0x1000, /*黄河*/
//	RTSP_ALK, /*艾立克*/
//	RTSP_KEDA, /*科达*/
//	RTSP_SANYO, /*三洋*/
//	MAX_RTSP,
//};

typedef enum 
{
	PRINT_ERROR	=0x01,/*错误*/
	PRINT_WARN	=0x02,/*警告*/
	PRINT_NOTICE=0x04,/*通知*/
	PRINT_INFO	=0x08,/*信息*/
}PRINT_LEVEL;//打印级别

#define  MOV_BIT		28
// [7/29/2009 lzy]
//定义相关的命令信息

//--------------------------------------------------------------------------------------------
//系统内部命令
typedef enum
{
	ENENT_BASE=120,
	
	/*
	网关连接
	*/
	EVENT_GWC2MON_GATEWAY_CHANGED,			//网关连接
	EVENT_GWC2MON_GATEWAY_CONNECT,
	EVENT_GWC2MON_GATEWAY_DISCONNECT,
	EVENT_GWC2MON_GATEWAY_LOGIN,
	EVENT_GWC2MON_GATEWAY_LOGOUT,

	EVENT_GWC2DVR_DEVICE_CONNECT,			
	EVENT_GWC2DVR_DEVICE_RECONNECT,
	EVENT_GWC2DVR_DEVICE_DISCONNECT,			
	EVENT_GWC2DVR_DEVICE_REMOVE,				
	EVENT_GWC2MON_DEVICE_REONLINE,			

	EVENT_DVR2MON_DEVICE_ONLINE,				
	EVENT_DVR2MON_DEVCIE_OFFLINE,			
	EVENT_DVR2MON_CHANNEL_ONLINE,			
	EVENT_DVR2MON_CHANNEL_OFFLINE,			
	EVENT_MON2MCU_CHANNEL_ONLINED,			
	EVENT_MON2MCU_CHANNEL_OFFLINED,			
	EVENT_MON2DVR_DISPATCH_MCUINFO,				
	EVENT_GWC2MCU_PROTOCOL_CHANGED,

	/*
	参数配置流程
	*/
	EVENT_DVR2MON_DEVICE_CHANGED,			
	EVENT_DVR2MON_CHANNEL_CHANGED,			
	EVENT_MON2DVR_GET_AVSETTING,			
	EVENT_MON2DVR_SET_AVSETTING	,		
	EVENT_DVR2MON_RESPOND_AVSETTING,
	EVENT_MON2DVR_CHANGE_CHINNAME,

	/*
	使用MomeryToString和StringToMemory的方式
	*/
	EVENT_MON2DVR_GET_DVRSETTING,
	EVENT_MON2DVR_SET_DVRSETTING,
	EVENT_DVR2MON_RESPOND_DVRSETTING,

	/*
	预览呼叫流程
	mcu request doplay --> dvr play
	mcu request replay --> dvr replay
	mcu request reconnect --> dvr disconnect --> dvr connect
	*/
	EVENT_MCU2DVR_REQUEST_DOPLAY,	
	EVENT_MCU2DVR_REQUEST_REPLAY,	
	EVENT_MCU2DVR_REQUEST_KEYFRAME,	

	/*
	对讲流程
	mon called --> mcu called (start receive and send)
	mon hunged --> mcu huanged (stop receive and send)
	mcu hunged(stop receive) --> mon huanged (stop send)
	*/
	EVENT_MON2DVR_VOICE_CALLED,	
	EVENT_MON2DVR_VOICE_HUNGED,	
	EVENT_MON2MCU_VOICE_CALLED,	
	EVENT_MON2MCU_VOICE_HUNGED,	
	EVENT_MCU2MON_VOICE_HUNGED,	
	EVENT_DVR2MON_VOICE_RESPOND,
	EVENT_MON2MCU_VOICE_CHANGE,

	/*
	云台控制
	*/
	EVENT_MON2DVR_PTZ_CONTROL,	
	EVENT_MON2DVR_PTZ_SET_SETTING,	
	EVENT_MON2DVR_PTZ_GET_SETTING,	
	EVENT_DVR2MON_PTZ_RESPOND_SETTING,	

	/*
	报警上传
	*/
	EVENT_DVR2MON_ALARM_SIGNAL, 

	/*
	报警策略更新
	*/
	EVENT_DVR2MON_ALARM_SGUPDATE,

	/**/
	EVENT_MON2DVR_STATE_NETCHECK,
#ifdef NVR_ENABLE
	/*
	NVR状态上报
	*/
	EVENT_NVR2MTS_SYSTEM_STATUS,
	EVENT_NVR2MTS_CHANNEL_STATUS,
	EVENT_NVR2MTS_REQUEST_VRSINFO,
#endif
}CMD_EVENT;

#define EVENT_DVR2DVR_DEVICE_CONNECT	EVENT_GWC2DVR_DEVICE_CONNECT
#define EVENT_DVR2DVR_DEVICE_RECONNECT	EVENT_GWC2DVR_DEVICE_RECONNECT
#define EVENT_DVR2DVR_DEVICE_DISCONNECT	EVENT_GWC2DVR_DEVICE_DISCONNECT
#define EVENT_DVR2DVR_DEVICE_REMOVE		EVENT_GWC2DVR_DEVICE_REMOVE	
#define EVENT_MCU2DVR_REQUEST_RECONNECT	EVENT_GWC2DVR_DEVICE_RECONNECT

//网关错误信息
typedef enum
{
	MGW_ERR_MAX_DVR_NUMBER_LIMIT=0x1000,	/*网关到达最大设备数限制*/
	MGW_ERR_MAX_CHANNEL_NUMBER_LIMIT,		/*网关到达最大通道数限制*/
	MGW_ERR_DVR_RECONNECTING=65533,			/*设备断开中*/
	MGW_ERR_DVR_DISCONNECTING,				/*设备重连中*/
	MGW_ERR_DVR_REMOVING,					/*删除设备中*/
	MGW_ERR_DVR_CONNECTING,					/*连接设备中*/
}MGW_ERROR;

//MTS返回的错误信息
typedef enum
{
	MTS_ERR_MGW_CONNECTED=100,		//网关连接MTS成功
	MTS_ERR_MGW_LOGINED,			//网关登录成功
	MTS_ERR_DVR_ONLINED,			//DVR上线成功
	MTS_ERR_CHANNEL_ONLINED,		//通道上线成功
	MTS_ERR_MGW_CONNECTTING=1000,	//网关正在连接MTS
	MTS_ERR_MGW_RECONNECTED,		//网关重连MTS成功
	MTS_ERR_MGW_SERVERISBUSY,		//MTS服务器正忙
	MTS_ERR_MGW_CONNECTFAILED,		//网关连接MTS失败
	MTS_ERR_MGW_DISCONNECTED,		//网关与MTS断开连接
	MTS_ERR_DVR_ACCOUNTISNULL,		//DVR的AVCON帐号为空
	MTS_ERR_DVR_SERIALNOISNULL,		//DVR的序列号为空
	MTS_ERR_MGW_NOLOGIN,			//网关未登录
	MTS_ERR_DVR_NOLOGIN,			//网关未连接上DVR设备
}MTS_ERROR;

//TYPE_TOOLS  正常的命令(网关工具过来的命令,验证权限)				
typedef enum
{
	CMD_TOOLS_LOGIN=0,
	CMD_TOOLS_LOGOUT,
	CMD_TOOLS_HANDSHAKE,
	CMD_GET_DEV_ALLINFO,					//得到所有设备信息
	CMD_GET_DEV_INFO, 							
	CMD_GET_CHAN_INFO,						//得到通道信息
	CMD_ADD_DEV,							//增加设备
	CMD_MOD_DEV,							//修改
	CMD_DEL_DEV,							//删除
	CMD_MOD_PSW,							//修改用户密码
	CMD_GET_MGS_INFO,						//得到网关信息
	CMD_GET_NETWORK,				
	CMD_SET_NETWORK,						//设置网络
	CMD_GET_OPTLOG,							//获取操作日志
	CMD_TOOLS_DEL_OPTLOG,
	CMD_GET_ALARMLOG,
	CMD_UPD_DEV_INFO,						//更新设备信息	
	CMD_MOD_CHN,							//修改通道信息
	CMD_UPD_OPTALM,							//更新报警策略
}CMD_NORMAL;

//报警类型
typedef enum
{
	TYPE_DISK_FULL=0,		//硬盘满
	TYPE_SIGNAL_LOSS,		//信号丢失
	TYPE_MOTION_DETECT,		//移动侦测
	TYPE_IO,				//防区报警
	TYPE_BLOCK, 			//遮挡
	TYPE_MODEL_NOT_MATCH,	//制式不匹配
	TYPE_DISK_NO_FORMAT,	//硬盘未格式化
	TYPE_READ_WRITE_DISK,	//读写硬盘出错
	TYPE_ILLEGAL_ACCESS,	//非法访问
	TYPE_UNKANOWN,			//未知报警
}ALARM_TYPE;

//--------------------------------------------------------------
//海康错误码类型 
typedef enum
{
	HIK_ERR_NOERROR=0,		
	HIK_ERR_PASSWORD_ERROR, 	
	HIK_ERR_NOENOUGHPRI,		
	HIK_ERR_NOINIT, 		
	HIK_ERR_CHANNEL_ERROR, 	
	HIK_ERR_OVER_MAXLINK, 		
	HIK_ERR_VERSIONNOMATCH, 	
	HIK_ERR_NETWORK_FAIL_CONNECT, 	
	HIK_ERR_NETWORK_SEND_ERROR, 	
	HIK_ERR_NETWORK_RECV_ERROR, 	
	HIK_ERR_NETWORK_RECV_TIMEOUT, 	
	HIK_ERR_NETWORK_ERRORDATA, 	
	HIK_ERR_ORDER_ERROR, 		
	HIK_ERR_OPERNOPERMIT, 		
	HIK_ERR_COMMANDTIMEOUT, 	
	HIK_ERR_ERRORSERIALPORT, 	
	HIK_ERR_ERRORALARMPORT, 	
	HIK_ERR_PARAMETER_ERROR, 	
	HIK_ERR_CHAN_EXCEPTION, 	
	HIK_ERR_NODISK	, 		
	HIK_ERR_ERRORDISKNUM, 		
	HIK_ERR_DISK_FULL, 		
	HIK_ERR_DISK_ERROR, 		
	HIK_ERR_NOSUPPORT, 		
	HIK_ERR_BUSY, 			
	HIK_ERR_MODIFY_FAIL, 		
	HIK_ERR_PASSWORD_FORMAT_ERROR, 
	HIK_ERR_DISK_FORMATING, 	
	HIK_ERR_DVRNORESOURCE, 	
	HIK_ERR_DVROPRATEFAILED, 	
	HIK_ERR_OPENHOSTSOUND_FAIL, 	
	HIK_ERR_DVRVOICEOPENED, 	
	HIK_ERR_TIMEINPUTERROR, 	
	HIK_ERR_NOSPECFILE, 		
	HIK_ERR_CREATEFILE_ERROR, 	
	HIK_ERR_FILEOPENFAIL, 		
	HIK_ERR_OPERNOTFINISH, 	
	HIK_ERR_GETPLAYTIMEFAIL, 	
	HIK_ERR_PLAYFAIL, 		
	HIK_ERR_FILEFORMAT_ERROR, 	
	HIK_ERR_DIR_ERROR, 		
	HIK_ERR_ALLOC_RESOUCE_ERROR, 	
	HIK_ERR_AUDIO_MODE_ERROR, 	
	HIK_ERR_NOENOUGH_BUF, 		
	HIK_ERR_CREATESOCKET_ERROR, 	
	HIK_ERR_SETSOCKET_ERROR, 	
	HIK_ERR_MAX_NUM, 		
	HIK_ERR_USERNOTEXIST, 		
	HIK_ERR_WRITEFLASHERROR, 	
	HIK_ERR_UPGRADEFAIL, 		
	HIK_ERR_CARDHAVEINIT, 		
	HIK_ERR_PLAYERFAILED, 		
	HIK_ERR_MAX_USERNUM, 		
	HIK_ERR_GETLOCALIPANDMACFAIL, 	
	HIK_ERR_NOENCODEING, 		
	HIK_ERR_IPMISMATCH, 		
	HIK_ERR_MACMISMATCH, 		
	HIK_ERR_UPGRADELANGMISMATCH, 	
	HIK_ERR_DDRAWDEVICENOSUPPORT, 

	HIK_EXCEPTION_AUDIOEXCHANGE=0x8001,	
	HIK_EXCEPTION_ALARM, 		
	HIK_EXCEPTION_PREVIEW, 		
	HIK_EXCEPTION_SERIAL, 		
#ifdef WIN32                  
	HIK_EXCEPTION_RECONNECT, 		
#endif

}HIK_ERROR;

//大华错误信息
#define  ZERO_16	0x00000000
typedef enum
{
	DHD_ERR_ERROR=-1,
	DHD_ERR_NOERROR=0,
	DHD_ERR_SYSTEM_ERROR,		
	DHD_ERR_NETWORK_ERROR,		
	DHD_ERR_DEV_VER_NOMATCH,		
	DHD_ERR_INVALID_HANDLE,		
	DHD_ERR_OPEN_CHANNEL_ERROR,	
	DHD_ERR_CLOSE_CHANNEL_ERROR,	
	DHD_ERR_ILLEGAL_PARAM,		
	DHD_ERR_SDK_INIT_ERROR,		
	DHD_ERR_SDK_UNINIT_ERROR,		
	DHD_ERR_RENDER_OPEN_ERROR,		
	DHD_ERR_DEC_OPEN_ERROR,		
	DHD_ERR_DEC_CLOSE_ERROR,		
	DHD_ERR_MULTIPLAY_NOCHANNEL,	
	DHD_ERR_TALK_INIT_ERROR, 		
	DHD_ERR_TALK_NOT_INIT,		
	DHD_ERR_TALK_SENDDATA_ERROR, 	
	DHD_ERR_REAL_ALREADY_SAVING, 	
	DHD_ERR_NOT_SAVING, 		
	DHD_ERR_OPEN_FILE_ERROR, 		
	DHD_ERR_PTZ_SET_TIMER_ERROR, 	
	DHD_ERR_RETURN_DATA_ERROR, 	
	DHD_ERR_INSUFFICIENT_BUFFER, 	
	DHD_ERR_NOT_SUPPORTED, 		
	DHD_ERR_NO_RECORD_FOUND, 		
	DHD_ERR_NOT_AUTHORIZED, 		
	DHD_ERR_NOT_NOW, 			
	DHD_ERR_NO_TALK_CHANNEL, 		
	DHD_ERR_NO_AUDIO, 			
	DHD_ERR_NO_INIT, 			
	DHD_ERR_DOWNLOAD_END, 		
	DHD_ERR_EMPTY_LIST, 		
	DHD_ERR_ERROR_GETCFG_SYSATTR, 	
	DHD_ERR_ERROR_GETCFG_SERIAL, 	
	DHD_ERR_ERROR_GETCFG_GENERAL, 	
	DHD_ERR_ERROR_GETCFG_DSPCAP, 	
	DHD_ERR_ERROR_GETCFG_NETCFG, 	
	DHD_ERR_ERROR_GETCFG_CHANNAME, 	
	DHD_ERR_ERROR_GETCFG_VIDEO, 	
	DHD_ERR_ERROR_GETCFG_RECORD, 	
	DHD_ERR_ERROR_GETCFG_PRONAME, 	
	DHD_ERR_ERROR_GETCFG_FUNCNAME, 	
	DHD_ERR_ERROR_GETCFG_485DECODER, 	
	DHD_ERR_ERROR_GETCFG_232COM, 	
	DHD_ERR_ERROR_GETCFG_ALARMIN, 	
	DHD_ERR_ERROR_GETCFG_ALARMDET, 	
	DHD_ERR_ERROR_GETCFG_SYSTIME, 	
	DHD_ERR_ERROR_GETCFG_PREVIEW, 	
	DHD_ERR_ERROR_GETCFG_AUTOMT, 	
	DHD_ERR_ERROR_GETCFG_VIDEOMTRX, 	
	DHD_ERR_ERROR_GETCFG_COVER, 	

	DHD_ERR_ERROR_SETCFG_GENERAL=DHD_ERR_ERROR_GETCFG_COVER+5, 	
	DHD_ERR_ERROR_SETCFG_NETCFG, 	
	DHD_ERR_ERROR_SETCFG_CHANNAME,	
	DHD_ERR_ERROR_SETCFG_VIDEO, 	
	DHD_ERR_ERROR_SETCFG_RECORD, 	
	DHD_ERR_ERROR_SETCFG_485DECODER, 	
	DHD_ERR_ERROR_SETCFG_232COM, 	
	DHD_ERR_ERROR_SETCFG_ALARMIN, 	
	DHD_ERR_ERROR_SETCFG_ALARMDET,	
	DHD_ERR_ERROR_SETCFG_SYSTIME, 	
	DHD_ERR_ERROR_SETCFG_PREVIEW, 	
	DHD_ERR_ERROR_SETCFG_AUTOMT, 	
	DHD_ERR_ERROR_SETCFG_VIDEOMTRX, 	
	DHD_ERR_ERROR_SETCFG_COVER=DHD_ERR_ERROR_SETCFG_VIDEOMTRX+2, 	
	DHD_ERR_AUDIOENCODE_NOTINIT, 	
	DHD_ERR_DATA_TOOLONGH, 		
	DHD_ERR_UNSUPPORTED, 		
	DHD_ERR_DEVICE_BUSY, 		
	DHD_ERR_SERVER_STARTED, 		
	DHD_ERR_SERVER_STOPPED, 		
	DHD_ERR_LISTER_INCORRECT_SERIAL=DHD_ERR_SERVER_STOPPED+5, 	
	DHD_ERR_LOGIN_ERROR_PASSWORD=100, 	
	DHD_ERR_LOGIN_ERROR_USER, 		
	DHD_ERR_LOGIN_ERROR_TIMEOUT, 	
	DHD_ERR_LOGIN_ERROR_RELOGGIN, 	
	DHD_ERR_LOGIN_ERROR_LOCKED, 	
	DHD_ERR_LOGIN_ERROR_BLACKLIST, 	
	DHD_ERR_LOGIN_ERROR_BUSY, 		
	DHD_ERR_LOGIN_ERROR_CONNECT, 	
	DHD_ERR_LOGIN_ERROR_NETWORK, 	
	DHD_ERR_RENDER_SOUND_ON_ERROR=120, 	
	DHD_ERR_RENDER_SOUND_OFF_ERROR, 	
	DHD_ERR_RENDER_SET_VOLUME_ERROR, 	
	DHD_ERR_RENDER_ADJUST_ERROR, 	
	DHD_ERR_RENDER_PAUSE_ERROR, 	
	DHD_ERR_RENDER_SNAP_ERROR, 	
	DHD_ERR_RENDER_STEP_ERROR, 	
	DHD_ERR_RENDER_FRAMERATE_ERROR, 	
	DHD_ERR_GROUP_EXIST=140, 		
	DHD_ERR_GROUP_NOEXIST, 		
	DHD_ERR_GROUP_RIGHTOVER, 		
	DHD_ERR_GROUP_HAVEUSER, 		
	DHD_ERR_GROUP_RIGHTUSE, 		
	DHD_ERR_GROUP_SAMENAME, 		
	DHD_ERR_USER_EXIST, 		
	DHD_ERR_USER_NOEXIST, 		
	DHD_ERR_USER_RIGHTOVER, 		
	DHD_ERR_USER_PWD, 			
	DHD_ERR_USER_FLASEPWD, 		
	DHD_ERR_USER_NOMATCHING, 		
	DHD_ERR_CONFIG_DEVBUSY=999, 		
	DHD_ERR_CONFIG_DATAILLEGAL,
}DHD_ERROR;

//艾普视达错误信息
typedef enum
{
	AIP_ERR_HANDLE=0x00000010, 		
	AIP_ERR_PARAM, 		
	AIP_ERR_CREATETHREAD, 	
	AIP_ERR_CREATESOCKET, 	
	AIP_ERR_OUTMEMORY, 		
	AIP_ERR_MORECHANNEL,

	AIP_ERR_BEGINCONNECT=0x00001000, 	
	AIP_ERR_CONNECTSUCCESS, 	
	AIP_ERR_NETWORK, 		
	AIP_ERR_CONNECTERROR, 	
	AIP_ERR_CONNECTERROR_1000, 	
	AIP_ERR_SERVERSTOP, 	
	AIP_ERR_SERVERSTOP_1000, 	
	AIP_ERR_TIMEOUT, 		
	AIP_ERR_TIMEOUT_1000, 	
	AIP_ERR_SENDDATA, 		
	AIP_ERR_SENDDATA_1000, 	
	AIP_ERR_RECVDATA, 		
	AIP_ERR_RECVDATA_1000,

	AIP_ERR_CLOSECONNECT=0x00010000, 	
	AIP_ERR_SERVERNOSTART, 	
	AIP_ERR_SERVERERROR, 	
	AIP_ERR_CHANNELLIMIT, 	
	AIP_ERR_SERVERLIMIT, 	
	AIP_ERR_SERVERREFUSE, 	
	AIP_ERR_IPLIMIT, 		
	AIP_ERR_PORTLIMIT, 		
	AIP_ERR_TYPEERROR, 		
	AIP_ERR_USERERROR, 		
	AIP_ERR_PASSWORDERROR, 	
	AIP_ERR_DONTTURN, 		
	AIP_ERR_VERSION=0x00100000, 
}AIP_ERROR;

//科达错误信息
typedef enum
{
	KEDA_ERR_SUCCESS			  =    (0),			   // 成功
	KEDA_ERR_INVALID              =    (-1)  ,         // 内部动态创建对象错误,没有初始化对象,或者普通空指针返回
	KEDA_ERR_SIZE                 =    (-2)  ,         // 大小不正确     
	KEDA_ERR_TYPE                 =    (-3)  ,         // 类型不正确
	KEDA_ERR_RANGE                =    (-4)  ,         // 范围不正确
	KEDA_ERR_HANDLE               =    (-5)  ,         // 无效的handle
	KEDA_ERR_OVERCAP              =    (-6)  ,         // 设备能力不支持
	KEDA_ERR_TIMEOUT              =    (-7)  ,         // 超时错误
	KEDA_ERR_STATE                =    (-8)  ,         // 状态错误，前置条件不满足
	KEDA_ERR_SPACE                =    (-9)  ,         // 空间不足
	KEDA_ERR_PATH                 =    (-10) ,         // 错误的路径
	KEDA_ERR_LACK_RESOURECE       =    (-11) ,         // 资源不足
	KEDA_ERR_MSG_SEQUENCE         =    (-12) ,         // 消息队列错误
	KEDA_ERR_DLL_LINK             =    (-13) ,         // dll链接错误
	KEDA_ERR_LOCALFILE_OPEN       =    (-14) ,         // 没有打开本地文件
	KEDA_ERR_LOCALFILE_PLAY       =    (-15) ,         // 没有播放本地文件
	KEDA_ERR_PU_ERR               =    (-16) ,         // 前端返回错误
	KEDA_ERR_NO_DLL_CAMERA        =    (-17) ,         // 没有camera.dll库
	KEDA_ERR_CAMERA_TYPE          =    (-18) ,         // 摄像头类型不正确
	KEDA_ERR_GET_CAMERA_OPT_CODE  =    (-19) ,         // 获取摄像头操作码失败
	KEDA_ERR_FILE_OPT             =    (-20) ,         // 文件操作失败
	KEDA_ERR_PLAYER_HANDLE        =    (-21) ,         // handle句柄和palyer句柄不对应
	KEDA_ERR_PALYER_INVALID       =    (-22) ,         // player无效
	KEDA_ERR_PALYER_REC           =    (-23) ,         // player正在录像
	KEDA_ERR_PALYER_PLAY          =    (-24) ,         // player正在播放视频
	KEDA_ERR_SEARCH               =    (-25) ,         // 搜索错误
	KEDA_ERR_CREATE_BMP           =    (-26) ,         // 创建位图失败
	KEDA_ERR_CONNECT_EXIST        =    (-27) ,         // 连接已经存在
	KEDA_ERR_CONNECT_PU           =    (-28) ,         // 连接设备失败
	KEDA_ERR_REQ_RECV_AUDIO       =    (-29) ,         // 请求前端接收音频失败
	KEDA_ERR_OSP                  =    (-30) ,         // Osp组建错误
	KEDA_ERR_VSIP                 =    (-31) ,         // 协议栈组建错误
	KEDA_ERR_LICENSE              =    (-32) ,         // LICENSE错误
	KEDA_ERR_ENC                  =    (-40) ,         // 编码器错误
	KEDA_ERR_DEC                  =    (-41) ,         // 解码器错误
	KEDA_ERR_RPPLAY               =    (-50) ,         // 录像库错误
	KEDA_ERR_RPDOWNLOAD           =    (-51) ,         // 录像下载库错误
	KEDA_ERR_CAMERA               =    (-60) ,         // 抓拍库错误
	KEDA_ERR_PICISDOWN            =    (-61) ,         // 当前正在下载图片
	KEDA_ERR_PICISUPLOAD          =    (-62) ,         // 当前正在上传图片
	KEDA_ERR_DEC_ICOPEN           =    (-100),         // 本地解码库打开异常
	KEDA_ERR_SRART                =    (-100),       
	KEDA_ERR_LOGIN_OTHER          =    (KEDA_ERR_SRART+11), // 登陆其他错误
	KEDA_ERR_LOGIN_TIMEOUT        =    (KEDA_ERR_SRART+12), // 登陆超时
	KEDA_ERR_LOGIN_PASSWORD       =    (KEDA_ERR_SRART+13), // 用户名或密码不正确
	KEDA_ERR_LOGIN_ADMINLOGIN     =    (KEDA_ERR_SRART+14), // 管理员已经登录
	KEDA_ERR_LOGIN_MAX_USER       =    (KEDA_ERR_SRART+15), // USER用户登录已到最大值
	KEDA_ERR_LOGIN_MAX_LOGIN      =    (KEDA_ERR_SRART+16), // 用户登录总数已到最大值
	KEDA_ERR_LOGIN_NOT_CONN       =    (KEDA_ERR_SRART+17), // 没有连接
}KEDA_ERROR;

//黄河错误信息
typedef enum 
{
	HHS_ERR_SUCCESS,				//操作成功
	HHS_ERR_FAILURE,				//操作失败
	HHS_ERR_REFUSE_REQ,				//请求被拒绝
	HHS_ERR_USER_FULL,				//登录用户已满
	HHS_ERR_PREVIEW_FULL,			//预览用户已满
	HHS_ERR_TASK_FULL,				//系统任务繁忙，待会尝试连接
	HHS_ERR_CHANNEL_NOT_EXIST,		//要打开的通道不存在或已满
	HHS_ERR_DEVICE_NAME,			//打开的设备不存在
	HHS_ERR_IS_TALKING,				//正在对讲
	HHS_ERR_QUEUE_FAILUE,			//队列出错
	HHS_ERR_USER_PASSWORD,			//用户名或密码和系统不匹配
	HHS_ERR_SHARE_SOCKET,			//socket 错误
	HHS_ERR_RELAY_NOT_OPEN,			//转发请求的服务还未打开
	HHS_ERR_RELAY_MULTI_PORT,		//转发多播端口错误
	HHS_ERR_VIEWPUSH_CHANNEL_USING,	//视频输入的通道已经被占用
	HHS_ERR_VIEWPUSH_DECODE_TYPE,	//视频输入通道的解码格式错误，0通道(4cif,2cif,cif),1通道(2cif,cif),2通道(cif),3通道(cif)
	HHS_ERR_AUTO_LINK_FAILURE,		//转发的自动连接失败
	HHS_ERR_NOT_LOGON,				//设备还未登录
	HHS_ERR_IS_SETTING,				//设备正在设置
	HHS_ERR_COMMAND_FAILURE,		//命令失败
	HHS_ERR_RESTRICT_ID,			//ID使用受限

	HHS_ERR_INVALID_PARAMETER=100,	//输入参数无效
	HHS_ERR_LOGON_FAILURE,			//登录失败
	HHS_ERR_TIME_OUT,				//操作超时
	HHS_ERR_SOCKET_ERR,				//SOCKET错误
	HHS_ERR_NOT_LINKSERVER,			//还未连接服务器
	HHS_ERR_BUFFER_EXTCEED_LIMIT,	//使用缓冲超过限制	
	HHS_ERR_LOW_PRIORITY,			//操作权限不足
	HHS_ERR_BUFFER_SMALL,			//缓冲太小
	HHS_ERR_IS_BUSY,				//系统任务正忙
	HHS_ERR_UPDATE_FILE,			//升级文件错误
	HHS_ERR_UPDATE_UNMATCH,			//升级文件和机器不匹配
	HHS_ERR_PORT_INUSE,				//端口被占用
	HHS_ERR_RELAY_DEVICE_EXIST,		//设备名已经存在
	HHS_ERR_CONNECT_REFUSED,		//连接时被拒绝
	HHS_ERR_PROT_NOT_SURPPORT,		//不支持该协议

	HHS_ERR_FILE_OPEN_ERR,          //打开文件失败
	HHS_ERR_FILE_SEEK_ERR,          //fseek失败 
	HHS_ERR_FILE_WRITE_ERR,         //fwrite失败 
	HHS_ERR_FILE_READ_ERR,          //fread失败 
	HHS_ERR_FILE_CLOSING,           //正在关闭文件 

	HHS_ERR_ALLOC_BUF_ERR,
}HHS_ERROR;

//汉邦错误信息
typedef enum 
{
	HBT_ERR_SDVR_NOERROR					=	0,		//没有错误
	HBT_ERR_SDVR_PASSWORD_ERROR 			=	-1,		//用户名密码错误
	HBT_ERR_SDVR_NOENOUGHPRI 				=	-2,		//权限不足
	HBT_ERR_SDVR_NOINIT 					=	-3,		//没有初始化
	HBT_ERR_SDVR_CHANNEL_ERROR 				=	-4,		//通道号错误
	HBT_ERR_SDVR_OVER_MAXLINK 				=	-5,		//连接到DVR的客户端个数超过最大
	HBT_ERR_SDVR_VERSIONNOMATCH				=	-6,		//版本不匹配
	HBT_ERR_SDVR_NETWORK_FAIL_CONNECT		=	-7,		//连接服务器失败
	HBT_ERR_SDVR_NETWORK_SEND_ERROR			=	-8,		//向服务器发送失败
	HBT_ERR_SDVR_NETWORK_RECV_ERROR			=	-9,		//从服务器接收数据失败
	HBT_ERR_SDVR_NETWORK_RECV_TIMEOUT		=	-10,	//从服务器接收数据超时
	HBT_ERR_SDVR_NETWORK_ERRORDATA			=	-11,	//传送的数据有误
	HBT_ERR_SDVR_ORDER_ERROR				=	-12,	//调用次序错误
	HBT_ERR_SDVR_OPERNOPERMIT				=	-13,	//无此权限                                                      
	HBT_ERR_SDVR_COMMANDTIMEOUT				=	-14,	//DVR命令执行超时                                                     
	HBT_ERR_SDVR_ERRORSERIALPORT			=	-15,	//串口号错误                                                      
	HBT_ERR_SDVR_ERRORALARMPORT				=	-16,	//报警端口错误                                                      
	HBT_ERR_SDVR_PARAMETER_ERROR 			=	-17,	//参数错误                                                      
	HBT_ERR_SDVR_CHAN_EXCEPTION				=	-18, 	//服务器通道处于错误状态                                              
	HBT_ERR_SDVR_NODISK						=	-19, 	//没有硬盘
	HBT_ERR_SDVR_ERRORDISKNUM				=	-20, 	//硬盘号错误                                                      
	HBT_ERR_SDVR_DISK_FULL					=	-21, 	//服务器硬盘满                                                      
	HBT_ERR_SDVR_DISK_ERROR				 	=	-22, 	//服务器硬盘出错                                                      
	HBT_ERR_SDVR_NOSUPPORT					=	-23, 	//服务器不支持                                                      
	HBT_ERR_SDVR_BUSY						=	-24, 	//服务器忙                                                      
	HBT_ERR_SDVR_MODIFY_FAIL				=	-25, 	//服务器修改不成                                                      
	HBT_ERR_SDVR_PASSWORD_FORMAT_ERROR		=	-26, 	//密码输入格式不功                                                    
	HBT_ERR_SDVR_DISK_FORMATING				=	-27, 	//硬盘正在格式化正确                                                  
	HBT_ERR_SDVR_DVRNORESOURCE				=	-28, 	//DVR资源不足
	HBT_ERR_SDVR_DVROPRATEFAILED			=	-29,	//DVR操作失败，不能启动操作                                        
	HBT_ERR_SDVR_OPENHOSTSOUND_FAIL 		=	-30,	//打开PC声音失败                                                      
	HBT_ERR_SDVR_DVRVOICEOPENED 			=	-31,	//服务器语音对讲                                                      
	HBT_ERR_SDVR_TIMEINPUTERROR				=	-32,	//时间输入不正确                                                      
	HBT_ERR_SDVR_NOSPECFILE					=	-33,	//回放时服务器没被占用                                                
	HBT_ERR_SDVR_CREATEFILE_ERROR			=	-34, 	//创建文件出错                                                      
	HBT_ERR_SDVR_FILEOPENFAIL				=	-35,	//打开文件出错有指定的文件                                          
	HBT_ERR_SDVR_OPERNOTFINISH				=	-36,	//上次的操作还没                                                      
	HBT_ERR_SDVR_GETPLAYTIMEFAIL			=	-37,	//获取当前播放的                                                      
	HBT_ERR_SDVR_PLAYFAIL					=	-38,	//播放出错有完成                                                
	HBT_ERR_SDVR_FILEFORMAT_ERROR			=	-39,	//文件格式不正确时间出错                                              
	HBT_ERR_SDVR_DIR_ERROR					=	-40, 	//路径错误                                                      
	HBT_ERR_SDVR_ALLOC_RESOUCE_ERROR		=	-41,	//资源分配错误                                                      
	HBT_ERR_SDVR_AUDIO_MODE_ERROR			=	-42, 	//声卡模式错误                                                      
	HBT_ERR_SDVR_NOENOUGH_BUF				=	-43, 	//缓冲区太小                                                      
	HBT_ERR_SDVR_CREATESOCKET_ERROR		 	=	-44, 	//创建SOCKET出错                                                      
	HBT_ERR_SDVR_SETSOCKET_ERROR			=	-45, 	//设置SOCKET出错                                                      
	HBT_ERR_SDVR_MAX_NUM					=	-46, 	//个数达到最大                                                      
	HBT_ERR_SDVR_USERNOTEXIST				=	-47, 	//用户不存在                                                      
	HBT_ERR_SDVR_WRITEFLASHERROR			=	-48,	//写FLASH出错                                                      
	HBT_ERR_SDVR_UPGRADEFAIL				=	-49,	//DVR升级失败                                                      
	HBT_ERR_SDVR_CARDHAVEINIT				=	-50,	//解码卡已经初始                                                      
	HBT_ERR_SDVR_PLAYERFAILED				=	-51, 	//播放器中错误                                                      
	HBT_ERR_SDVR_MAX_USERNUM				=	-52,	//用户数达到最大化过                                                  
	HBT_ERR_SDVR_GETLOCALIPANDMACFAIL		=	-53,	//获得客户端的IP                                                      
	HBT_ERR_SDVR_NOENCODEING				=	-54, 	//该通道没有编码                                                      
	HBT_ERR_SDVR_IPMISMATCH					=	-55, 	//IP地址不匹配地址或物理地址失败                                    
	HBT_ERR_SDVR_MACMISMATCH				=	-56, 	//MAC地址不匹配                                                      
	HBT_ERR_SDVR_UPGRADELANGMISMATCH		=	-57, 	//升级文件语言不                                                      
	HBT_ERR_SDVR_USERISALIVE				=	-58, 	//用户已登陆                                                       
	HBT_ERR_SDVR_IPERR						=	-101,	//IP地址不匹配匹配                                                  
	HBT_ERR_SDVR_MACERR			    		=	-102,	//MAC地址不匹配                                                      
	HBT_ERR_SDVR_PSWERR			    		=	-103,	//MAC地址不匹配                                                      
	HBT_ERR_SDVR_USERERR			    	=	-104,	//MAC地址不匹配                                                      
	HBT_ERR_SDVR_USERISFULL			    	=	-105,	//MAC地址不匹配                                                      
	HBT_ERR_SDVR_NO_PERMISSION				=	0xf0,   //用户没有权限                                                      
	HBT_ERR_SDVR_FILE_SUCCESS				=	1000,	//获得文件信息                                                      
	HBT_ERR_SDVR_FILE_NOFIND				=	1001,	//没有文件                                                      
	HBT_ERR_SDVR_ISFINDING					=	1002,	//正在查找文件                                                      
	HBT_ERR_SDVR_NOMOREFILE					=	1003,	//查找文件时没有                                                      
	HBT_ERR_SDVR_FILE_EXCEPTION				=	1004,	//查找文件时异常
}HBT_ERROR;

//添津亚安错误信息
typedef enum
{
	YAA_ERR_SDVR_NOERROR					=	0,		//没有错误
	YAA_ERR_SDVR_PASSWORD_ERROR 			=	-1,		//用户名密码错误
	YAA_ERR_SDVR_NOENOUGHPRI 				=	-2,		//权限不足
	YAA_ERR_SDVR_NOINIT 					=	-3,		//没有初始化
	YAA_ERR_SDVR_CHANNEL_ERROR 				=	-4,		//通道号错误
	YAA_ERR_SDVR_OVER_MAXLINK 				=	-5,		//连接到DVR的客户端个数超过最大
	YAA_ERR_SDVR_VERSIONNOMATCH				=	-6,		//版本不匹配
	YAA_ERR_SDVR_NETWORK_FAIL_CONNECT		=	-7,		//连接服务器失败
	YAA_ERR_SDVR_NETWORK_SEND_ERROR			=	-8,		//向服务器发送失败
	YAA_ERR_SDVR_NETWORK_RECV_ERROR			=	-9,		//从服务器接收数据失败
	YAA_ERR_SDVR_NETWORK_RECV_TIMEOUT		=	-10,	//从服务器接收数据超时
	YAA_ERR_SDVR_NETWORK_ERRORDATA			=	-11,	//传送的数据有误
	YAA_ERR_SDVR_ORDER_ERROR				=	-12,	//调用次序错误
	YAA_ERR_SDVR_OPERNOPERMIT				=	-13,	//无此权限                                                      
	YAA_ERR_SDVR_COMMANDTIMEOUT				=	-14,	//DVR命令执行超时                                                     
	YAA_ERR_SDVR_ERRORSERIALPORT			=	-15,	//串口号错误                                                      
	YAA_ERR_SDVR_ERRORALARMPORT				=	-16,	//报警端口错误                                                      
	YAA_ERR_SDVR_PARAMETER_ERROR 			=	-17,	//参数错误                                                      
	YAA_ERR_SDVR_CHAN_EXCEPTION				=	-18, 	//服务器通道处于错误状态                                              
	YAA_ERR_SDVR_NODISK						=	-19, 	//没有硬盘
	YAA_ERR_SDVR_ERRORDISKNUM				=	-20, 	//硬盘号错误                                                      
	YAA_ERR_SDVR_DISK_FULL					=	-21, 	//服务器硬盘满                                                      
	YAA_ERR_SDVR_DISK_ERROR				 	=	-22, 	//服务器硬盘出错                                                      
	YAA_ERR_SDVR_NOSUPPORT					=	-23, 	//服务器不支持                                                      
	YAA_ERR_SDVR_BUSY						=	-24, 	//服务器忙                                                      
	YAA_ERR_SDVR_MODIFY_FAIL				=	-25, 	//服务器修改不成                                                      
	YAA_ERR_SDVR_PASSWORD_FORMAT_ERROR		=	-26, 	//密码输入格式不功                                                    
	YAA_ERR_SDVR_DISK_FORMATING				=	-27, 	//硬盘正在格式化正确                                                  
	YAA_ERR_SDVR_DVRNORESOURCE				=	-28, 	//DVR资源不足
	YAA_ERR_SDVR_DVROPRATEFAILED			=	-29,	//DVR操作失败，不能启动操作                                        
	YAA_ERR_SDVR_OPENHOSTSOUND_FAIL 		=	-30,	//打开PC声音失败                                                      
	YAA_ERR_SDVR_DVRVOICEOPENED 			=	-31,	//服务器语音对讲                                                      
	YAA_ERR_SDVR_TIMEINPUTERROR				=	-32,	//时间输入不正确                                                      
	YAA_ERR_SDVR_NOSPECFILE					=	-33,	//回放时服务器没被占用                                                
	YAA_ERR_SDVR_CREATEFILE_ERROR			=	-34, 	//创建文件出错                                                      
	YAA_ERR_SDVR_FILEOPENFAIL				=	-35,	//打开文件出错有指定的文件                                          
	YAA_ERR_SDVR_OPERNOTFINISH				=	-36,	//上次的操作还没                                                      
	YAA_ERR_SDVR_GETPLAYTIMEFAIL			=	-37,	//获取当前播放的                                                      
	YAA_ERR_SDVR_PLAYFAIL					=	-38,	//播放出错有完成                                                
	YAA_ERR_SDVR_FILEFORMAT_ERROR			=	-39,	//文件格式不正确时间出错                                              
	YAA_ERR_SDVR_DIR_ERROR					=	-40, 	//路径错误                                                      
	YAA_ERR_SDVR_ALLOC_RESOUCE_ERROR		=	-41,	//资源分配错误                                                      
	YAA_ERR_SDVR_AUDIO_MODE_ERROR			=	-42, 	//声卡模式错误                                                      
	YAA_ERR_SDVR_NOENOUGH_BUF				=	-43, 	//缓冲区太小                                                      
	YAA_ERR_SDVR_CREATESOCKET_ERROR		 	=	-44, 	//创建SOCKET出错                                                      
	YAA_ERR_SDVR_SETSOCKET_ERROR			=	-45, 	//设置SOCKET出错                                                      
	YAA_ERR_SDVR_MAX_NUM					=	-46, 	//个数达到最大                                                      
	YAA_ERR_SDVR_USERNOTEXIST				=	-47, 	//用户不存在                                                      
	YAA_ERR_SDVR_WRITEFLASHERROR			=	-48,	//写FLASH出错                                                      
	YAA_ERR_SDVR_UPGRADEFAIL				=	-49,	//DVR升级失败                                                      
	YAA_ERR_SDVR_CARDHAVEINIT				=	-50,	//解码卡已经初始                                                      
	YAA_ERR_SDVR_PLAYERFAILED				=	-51, 	//播放器中错误                                                      
	YAA_ERR_SDVR_MAX_USERNUM				=	-52,	//用户数达到最大化过                                                  
	YAA_ERR_SDVR_GETLOCALIPANDMACFAIL		=	-53,	//获得客户端的IP                                                      
	YAA_ERR_SDVR_NOENCODEING				=	-54, 	//该通道没有编码                                                      
	YAA_ERR_SDVR_IPMISMATCH					=	-55, 	//IP地址不匹配地址或物理地址失败                                    
	YAA_ERR_SDVR_MACMISMATCH				=	-56, 	//MAC地址不匹配                                                      
	YAA_ERR_SDVR_UPGRADELANGMISMATCH		=	-57, 	//升级文件语言不                                                      
	YAA_ERR_SDVR_USERISALIVE				=	-58, 	//用户已登陆                                                       
	YAA_ERR_SDVR_IPERR						=	-101,	//IP地址不匹配匹配                                                  
	YAA_ERR_SDVR_MACERR			    		=	-102,	//MAC地址不匹配                                                      
	YAA_ERR_SDVR_PSWERR			    		=	-103,	//MAC地址不匹配                                                      
	YAA_ERR_SDVR_USERERR			    	=	-104,	//MAC地址不匹配                                                      
	YAA_ERR_SDVR_USERISFULL			    	=	-105,	//MAC地址不匹配                                                      
	YAA_ERR_SDVR_NO_PERMISSION				=	0xf0,   //用户没有权限                                                      
	YAA_ERR_SDVR_FILE_SUCCESS				=	1000,	//获得文件信息                                                      
	YAA_ERR_SDVR_FILE_NOFIND				=	1001,	//没有文件                                                      
	YAA_ERR_SDVR_ISFINDING					=	1002,	//正在查找文件                                                      
	YAA_ERR_SDVR_NOMOREFILE					=	1003,	//查找文件时没有                                                      
	YAA_ERR_SDVR_FILE_EXCEPTION				=	1004,	//查找文件时异常
}YAA_ERROR;


//RTSP错误信息
typedef enum 
{
	RTSP_ERR_SUCCESS,										//操作成功
	RTSP_ERR_FAILURE,										//操作失败

	RTSP_ERR_CONTINUE = 100,								// 继续; Continue (all 100 range)
	RTSP_ERR_OK	= 200,										// 成功; OK 
	RTSP_ERR_CREATE = 201,									// 已创建; Created 
	RTSP_ERR_LOW_ON_STORAGE_SPACE = 250,					// 存储空间不足; Low on Storage Space 
	RTSP_ERR_MULTIPLE_CHOICES = 300,						// 有多个选项; Multiple Choices 
	RTSP_ERR_MOVED_PERMANENTLY = 301,						// 被永久移除; Moved Permanently 
	RTSP_ERR_MOVED_TEMPORARILY = 302,						// 被临时移除; Moved Temporarily 
	RTSP_ERR_SEE_OTHER = 303,								// 见其他; See Other  
	RTSP_ERR_NOT_MODIFIED = 304,							// 没有修改; Not Modified
	RTSP_ERR_USE_PROXY = 305,								// 使用代理; Use Proxy 
	RTSP_ERR_GOING_AWAY = 350,								// 断开连接; Going Away
	RTSP_ERR_LOAD_BALANCING = 351,							// 负载均衡; Load Balancing
	RTSP_ERR_BAD_REQUEST = 400,								// 错误的请求; Bad Request 
	RTSP_ERR_UNAUTHORIZED = 401,							// 未通过认证; Unauthorized 
	RTSP_ERR_PAYMENT_REQUIRED = 402,						// 需要付费; Payment Required 
	RTSP_ERR_FORBIDDEN = 403,								// 禁止; Forbidden
	RTSP_ERR_NOT_FOUND = 404,								// 没有找到; Not Found
	RTSP_ERR_METHOD_NOT_ALLOWED = 405,						// 不允许该方法; Method Not Allowed 
	RTSP_ERR_NOT_ACCEPTABLE = 406,							// 不接受; Not Acceptable 
	RTSP_ERR_PROXY_AUTHENTICATION_REQUIRED = 407,			// 代理需要认证; Proxy Authentication Required
	RTSP_ERR_REQUEST_TIMEOUT = 408,							// 请求超时; Request Time-out 
	RTSP_ERR_GONE = 410,									// 不在服务器; Gone 
	RTSP_ERR_LENGTH_REQUIRED = 411,							// 需要长度; Length Required 
	RTSP_ERR_PRECONDITION_FAILED = 412,						// 预处理失败; Precondition Failed
	RTSP_ERR_REQUEST_ENTITY_TOO_LARGE = 413,				// 请求实体过长; Request Entity Too Large 
	RTSP_ERR_REQUEST_URI_TOO_LARGE = 414,					// 请求-URI过长; Request-URI Too Large 
	RTSP_ERR_UNSUPPORTED_MEDIA_TYPE = 415,					// 媒体类型不支持; Unsupported Media Type 
	RTSP_ERR_PARAMETER_NOT_UNDERSTOOD = 451,				// 不理解此参数; Parameter Not Understood 
	RTSP_ERR_RESERVED = 452,								// 找不到会议; reserved 
	RTSP_ERR_NOT_ENOUGH_BANDWIDTH = 453,					// 带宽不足; Not Enough Bandwidth 
	RTSP_ERR_SESSION_NOT_FOUND = 454,						// 找不到会话; Session Not Found 
	RTSP_ERR_METHOD_NOT_VALID_IN_THIS_STATE = 455,			// 此状态下此方法无效; Method Not Valid in This State 
	RTSP_ERR_HEADER_FIELD_NOT_VALID_FOR_RESOURCE = 456,		// 此头部域对该资源无效; Header Field Not Valid for Resource
	RTSP_ERR_INVALID_RANGE = 457,							// 无效范围; Invalid Range 
	RTSP_ERR_PARAMETER_IS_READONLY = 458,					// 参数是只读的; Parameter Is Read-Only
	RTSP_ERR_AGGREGATE_OPERATION_NOT_ALLOWED = 459,			// 不允许合控制; Aggregate operation not allowed
	RTSP_ERR_ONLY_AGGREGATE_OPERATION_ALLOWED = 460,		// 只允许合控制; Only aggregate operation allowed
	RTSP_ERR_UNSUPPORTED_TRANSPORT = 461,					// 传输方式不支持; Unsupported transport 
	RTSP_ERR_DESTINATION_UNREACHABLE = 462,					// 无法到达目的地址; Destination unreachable 
	RTSP_ERR_INTERNAL_SERVER_ERROR = 500,					// 服务器内部错误; Internal Server Error 
	RTSP_ERR_NOT_IMPLEMENTED = 501,							// 未实现; Not Implemented
	RTSP_ERR_BAD_GATEWAY = 502,								// 网关错误; Bad Gateway 
	RTSP_ERR_SERVICE_UNAVAILABLE = 503,						// 无法得到服务; Service Unavailable
	RTSP_ERR_GATEWAY_TIMEOUT = 504,							// 网关超时; Gateway Time-out
	RTSP_ERR_VERSION_NOT_SUPPORTED = 505,					// 不支持此RTSP版本; RTSP Version not supported 
	RTSP_ERR_OPTION_NOT_SUPPORTED= 551,						// 不支持选项; Option not supported 
}RTSP_ERROR;

//天地伟业 错误号
typedef enum
{
	TIANDY_ERR_NO_DEV = 0x10000001,		//"没有找到设备",				
	TIANDY_ERR_UNLOGON,					//"未登陆",					
	TIANDY_ERR_PARAM_OVER,				//"参数越界",					
	TIANDY_ERR_REOPERATION,				//"重复操作",					
	TIANDY_ERR_WSASTARTUP,				//"WSAStartup执行失败",		
	TIANDY_ERR_CREATEMSG,				//"创建消息循环失败",			
	TIANDY_ERR_NOSUPPORTRECORD,			//"不支持前端录像",			
	TIANDY_ERR_INITCHANNELNET,			//"通道网络创建失败",			
	TIANDY_ERR_CREATEDDRAW,				//"无法创建更多的DirectDraw",	
	TIANDY_ERR_NO_CHANNEL,				//"通道没有创建",				
	TIANDY_ERR_NO_FUN,					//"无此功能",					
	TIANDY_ERR_PARAM_INVALID,			//"参数无效",					
	TIANDY_ERR_DEV_FULL,				//"设备列表已满",				
	TIANDY_ERR_LOGON,					//"设备已经登录，正在登录",	
	TIANDY_ERR_CREATECPUTHREAD,			//"创建CPU检测线程失败",		
	TIANDY_ERR_CREATEPLAYER,			//"创建Player失败",			
	TIANDY_ERR_NOAUTHORITY,				//"权限不足",					
	TIANDY_ERR_NOTALK,					//"未对讲",					
	TIANDY_ERR_NOCALLBACK,				//"没有设备回调函数",			
	TIANDY_ERR_CREATEFILE,				//"创建文件失败",				
	TIANDY_ERR_NORECORD,				//"不是从当前Player发起的录像"
	TIANDY_ERR_NOPLAYER,				//"有对应Player",				
	TIANDY_ERR_INITCHANNEL,				//"通道没有初始化",			
	TIANDY_ERR_NOPLAYING,				//"Player没有播放",			
	TIANDY_ERR_PARAM_LONG,				//"字符串参数长度过长",		
	TIANDY_ERR_INVALID_FILE,			//"文件不符合要求",			
	TIANDY_ERR_USER_FULL,				//"用户列表已满",				
	TIANDY_ERR_USER_DEL,				//"当前用户无法删除",			
	TIANDY_ERR_CARD_LOAD,				//"加载卡dll失败",			
	TIANDY_ERR_CARD_CORE,				//"加载卡内核失败",			
	TIANDY_ERR_CARD_COREFILE,			//"加载卡内核文件失败",		
	TIANDY_ERR_CARD_INIT,				//"卡初始化失败",				
	TIANDY_ERR_CARD_COREREAD,			//"读取卡内核文件失败",		
	TIANDY_ERR_CHARACTER_LOAD,			//"加载字库失败",				
	TIANDY_ERR_NOCARD,					//"卡未初始化",				
	TIANDY_ERR_SHOW_MODE,				//"显示模式未设置",			
	TIANDY_ERR_FUN_LOAD,				//"函数未加载",				
	TIANDY_ERR_CREATE_DOWNLOAD,			//"没有更多的下载通道可用",	
	TIANDY_ERR_PROXY_DELACT,			//"删除的是主动模式的服务器",	
	TIANDY_ERR_PROXY_HASCONNECT,		//"还有连接",					
	TIANDY_ERR_PROXY_NOPROXY,			//"代理没有启动",				
	TIANDY_ERR_PROXY_IDENTITY,			//"不是代理的身份",			
	TIANDY_ERR_CONNECT_MAX,				//"连接已经到达最大",			
	TIANDY_ERR_NO_DISK,					//"没有挂接存储设备",			
}TIANDY_ERROR;

// 天地伟业参数配置
typedef enum
{
	TDY_SDVR_SET_DEVICECFG = 100,//设备参数
	TDY_SDVR_GET_DEVICECFG,//设备参数
	TDY_SDVR_SET_NETCFG,//网络参数
	TDY_SDVR_GET_NETCFG,//网络参数
	TDY_SDVR_SET_COMPRESSCFG,//压缩参数}
	TDY_SDVR_GET_COMPRESSCFG,//压缩参数}
	TDY_SDVR_SET_RECORDCFG,//录像参数
	TDY_SDVR_GET_RECORDCFG,//录像参数
	TDY_SDVR_SET_DECODERCFG,//解码器参数
	TDY_SDVR_GET_DECODERCFG,//解码器参数
	TDY_SDVR_SET_RS232CFG,//232串口参数
	TDY_SDVR_GET_RS232CFG,//232串口参数
	TDY_SDVR_SET_ALARMINCFG,//报警输入参数
	TDY_SDVR_GET_ALARMINCFG,//报警输入参数
	TDY_SDVR_SET_ALARMOUTCFG,//报警输出参数
	TDY_SDVR_GET_ALARMOUTCFG,//报警输出参数
	TDY_SDVR_SET_TIMECFG,//设置DVR时间
	TDY_SDVR_GET_TIMECFG,//设置DVR时间
	TDY_SDVR_SET_PREVIEWCFG,//预览参数
	TDY_SDVR_GET_PREVIEWCFG,//预览参数
	TDY_SDVR_SET_VIDEOCFG,//视频参数
	TDY_SDVR_GET_VIDEOCFG,//视频参数
	TDY_SDVR_SET_AUDIOCFG,//音频参数
	TDY_SDVR_GET_AUDIOCFG,//音频参数
	TDY_SDVR_SET_EXCEPTIONCFG,//异常参数
	TDY_SDVR_GET_EXCEPTIONCFG,//异常参数
	TDY_SDVR_SET_SHOWSTRING,//叠加字符参数
	TDY_SDVR_GET_SHOWSTRING,//叠加字符参数
	TDY_SDVR_SET_PICCFG_EX,//图象参数(SDK_V14扩展命令)
	TDY_SDVR_GET_PICCFG_EX,//图象参数(SDK_V14扩展命令)
	TDY_SDVR_SET_EVENTCOMPCFG,//事件触发录像参
	TDY_SDVR_GET_EVENTCOMPCFG,//事件触发录像参
}TIANDY_PARA;

typedef enum
{
	YAA_ERR_NOERROR = 0,		//"成功"
	YAA_ERR_FAILUAR,			//"失败"
}YAA_ERR;

typedef enum
{
	NCI_ERR_NOERROR = 0,		//"成功",						
	NCI_ERR_LOGINERR,			//"登录失败",				    
	NCI_ERR_REALPLAYERR,		//"预览失败",		
}NCI_ERROR;

typedef enum
{
	JTN_ERR_NOERROR = 0,		//"成功",						
	JTN_ERR_LOGINERR,			//"登录失败",				    
	JTN_ERR_REALPLAYERR,		//"预览失败",		
}JTN_ERROR;

typedef enum
{
	TLV_ERR_NOERROR = 0,		//"成功",						
	TLV_ERR_LOGINERR,			//"登录失败",				    
	TLV_ERR_REALPLAYERR,		//"预览失败",		
}TLV_ERROR;

typedef enum
{
	BSC_ERR_NOERROR = 0,		//"成功",						
	BSC_ERR_LOGINERR,			//"登录失败",				    
	BSC_ERR_REALPLAYERR,		//"预览失败",		
}BSC_ERROR;

typedef enum
{
	XQI_ERR_NOERROR = 0,		//成功

}XinQi_ERROR;