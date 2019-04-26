#pragma once

#define AVCON_SERVER_VERSION		"7.2.1.1457_T"
#define AVCON_CMD_VERSION			0x0020

//////////////////////////////////////////////////////////////////////////////////////
//									SYS									s			//
//////////////////////////////////////////////////////////////////////////////////////

#define IO_CONNECT_TYPE_CLIENT_SVC			100
#define IO_CONNECT_TYPE_CLIENT_NFS			103		//兼容老版本
#define IO_CONNECT_TYPE_CLIENT_MGW			104
#define IO_CONNECT_TYPE_CLIENT_VRS			105		//监控录像上传
#define IO_CONNECT_TYPE_CLIENT_PLT			106		//监控平台
#define IO_CONNECT_TYPE_CLIENT_NDU          107
#define IO_CONNECT_TYPE_CLIENT_NDD          108
#define IO_CONNECT_TYPE_CLIENT_NDI          109

#define IO_CONNECT_TYPE_MCU_ROUTER			200
#define IO_CONNECT_TYPE_NFS_ROUTER			201

#define SERVER_TYPE_MCU						300
#define SERVER_TYPE_MTS						301
#define SERVER_TYPE_MCU_MTS					302
#define SERVER_TYPE_GW						303
#define SERVER_TYPE_3GC						304
#define SERVER_TYPE_3GM						305
//#define SERVER_TYPE_MP					306
#define SERVER_TYPE_GW_TR2					307
#define SERVER_TYPE_TRS						308		//转码服务器
#define SERVER_TYPE_TPS						309		//协议转换服务器
#define SERVER_TYPE_TPS						309

#define IMS_AGENT_ID						400
#define MMS_AGENT_ID						401
#define MON_AGENT_ID						402
#define SMS_AGENT_ID						403
#define FAX_AGENT_ID						404
#define SIP_AGENT_ID						405
#define NDS_AGENT_ID						406
#define GPS_AGENT_ID						407
#define SYNC_AGENT_ID						408
#define REC_AGENT_ID                        409
#define CTS_AGENT_ID						410	//定制AGENT处理
#define VDS_AGENT_ID						411	//定制VDS
#define CRS_AGENT_ID						412	//定制CRS

//---------------------------------------------------------
// 文件传输常量
#define MAX_FILE_PACKET			65536 + 64
#define MAX_FILE_DATA_PACKET	65536
#define MAX_CMD_PACKET			32768 * 3

//---------------------------------------------------------
//数字功能广播窗口所处位置值
#define DIGITAL_SCREENID		999
#define DIGITAL_WINDOWID		999

//////////////////////////////////////////////////////////////////////////////////////
//									IMS												//
//////////////////////////////////////////////////////////////////////////////////////

// 认证类型
typedef enum _IMS_USER_AUTHTYPE
{
	IMS_AUTHTYPE_DB			= 0,		// 本地数据库认证
	IMS_AUTHTYPE_LDAP		= 1,		// LDAP认证
    IMS_AUTHTYPE_RADIUS		= 2,		// RADIUS认证
    IMS_AUTHTYPE_GUEST      = 4,        // 免检
	IMS_AUTHTYPE_SX			= 5,		// 
	IMS_AUTHTYPE_THIRDPART	= 99,		// 第三方认证
};

//////////////////////////////////////////////////////////////////////////////////////
//									MMS												//
//////////////////////////////////////////////////////////////////////////////////////
#include "RIGHT.h" // 会议权限控制， 由于改动较大，独立出一个单独文件

// 列表
typedef enum _MMS_LIST
{
	MET_LIST_ROLOCUTOR	= 1,	// 发言
	MET_LIST_QUESTIONER,		// 提问
};

//-------------------------------------
// 通道类型
typedef enum _MMS_CHLTYPE
{
	MMS_CHLTYPE_VID = 1,	// 视频通道
	MMS_CHLTYPE_AUD,		// 音频通道
	MMS_CHLTYPE_DOC,		// 文档通道
	MMS_CHLTYPE_CHT,		// 讨论通道
	MMS_CHLTYPE_WHB,		// 白板通道
	MMS_CHLTYPE_DTS,		// 文件传输通道
};

//---------------------------------------------------------------------------------------
// 群组离线功能
//---------------------------------------------------------------------------------------
typedef enum _IMS_TEAMTYPE
{
	IMS_TEAM_NONE = 0,		//none
	IMS_TEAM_PIC,			//群组图片
	IMS_TEAM_FILE,			//群组文件
};

typedef enum _KDXF_ACTIONTYPE
{
	KDXF_LOGIN = 0,		        //登录
	KDXF_LOGOUT,			    //登出
	KDXF_ACCESS_CLASS,			//进入课堂
	KDXF_QUIT_CLASS,            //退出课堂
	KDXF_START_BROADCAST,       //开始广播
	KDXF_STOP_BROADCAST,        //停止广播
	KDXF_SWITCH_WINDOW,         //切换窗口
	KDXF_ZOOM_IN,               //放大
	KDXF_ZOOM_OUT,              //缩小
};
//-------------------------------------
// 通道结构
typedef struct _MMS_CHANNEL
{
	unsigned int	channeltype;
	unsigned long	channelid;
	unsigned long	codetype;
	_MMS_CHANNEL()
	{
		channeltype=0;
		channelid=0;
		codetype=0;
	};
}MMS_CHANNEL,*PMMS_CHANNEL;

#include <list>
typedef std::list<PMMS_CHANNEL> MMS_CHANNEL_LST;

//---------------------------------------------------------
// NODEID MAP
//---------------------------------------------------------
#include <map>
//nodeid,index
typedef std::map<std::string,int> NODEID_MAP;

//=======================================================================================
// 错误代码
//=======================================================================================
#define	ERR_NO							00000		//正确

//---------------------------------------------------------------------------------------
// 系统类或网络类错误代码
//---------------------------------------------------------------------------------------
#define	ERR_SYS_PROTOCOL				10000		//通讯协议版本不匹配
#define	ERR_SYS_UNKNOWN					10001		//未知错误
#define	ERR_SYS_COMMAND					10002		//协议命令错误
#define	ERR_SYS_DATABASE				10003		//数据库错误
#define	ERR_SYS_NETWORK					10004		//网络断开(网络通路断开)
#define	ERR_SYS_SERVERTYPE				10005		//服务器类型错误,登录到错误类型的服务器
#define	ERR_SYS_LICEXPIRED              10006       //服务器授权到期
//---------------------------------------------------------------------------------------
// 即时通讯类错误代码
//---------------------------------------------------------------------------------------
#define	ERR_IMS_PROTOCOL				20000		//通讯协议版本不匹配
#define	ERR_IMS_INVALIDACCOUNT			20001		//无效的账户(系统没有该账户ID)
#define	ERR_IMS_INVALIDPASSWORD			20002		//无效的密码
#define	ERR_IMS_DUPLICATELOGIN			20003		//该账户已经登录,不得重复登录
#define	ERR_IMS_REQUESTRELOGIN			20004		//服务器要求使用获得的新地址重新登录
#define	ERR_IMS_KICKOUT					20005		//被踢出(该账户在其它地方已登录,强制本地帐号退出)
#define ERR_IMS_MODULELIMIT				20006		//服务器功能限制(服务端即时通讯模块授权没有启用)
#define ERR_IMS_CONNECTLIMIT			20007		//连接服务器到达最大用户数限制
#define ERR_IMS_ACCOUNTLIMIT			20008		//用户登录到达最大用户帐号数限制
#define ERR_IMS_SERVER					20009		//用户帐号在该服务器上不存在或该服务器没有加载帐号验证模块
#define ERR_IMS_SERVERLIMIT				20010		//服务器地址限定,该用户不能在此服务器上登录
#define ERR_IMS_UNKNOWNAUTHTYPE			20011		//未知的帐户验证模式(NOT:DB/LDAP/RADIUS)
#define ERR_IMS_FORBIDDEN				20012		//该账户被禁用
#define ERR_IMS_CLIENTAUTH				20013		//请求客户端验证
#define ERR_IMS_SERIALNOERROR			20014		//序列号绑定错误
#define ERR_IMS_INVALIDRECORD			20015		//非录像服务器帐户错误
#define ERR_IMS_ADDDISCUSSMEMBER		20016		//添加讨论组成员错误(人数超出上线)
#define ERR_IMS_WEBSERVICEDISCONNECT	20017		//WebService连接失败
#define ERR_IMS_OTHERSERVERDISCONNECT	20018		//第三方服务器连接失败
#define	ERR_IMS_INVALIDIDENTIFYID		20019		//终端识别码错误
#define	ERR_IMS_3GLIMIT					20020		//3G用户登录到达最大用户帐号数限制
#define	ERR_IMS_INVALIDTERM				20021		//非终端帐户类型错误
#define	ERR_IMS_AUTHFAILD				20022		//认证失败
#define ERR_IMS_PLATFORMLIMIT			20023		//服务器平台限定,该终端不能在此服务器上登录


//---------------------------------------------------------------------------------------
// 会议通讯类错误代码
//---------------------------------------------------------------------------------------
#define ERR_MMS_PROTOCOL				30000		//通讯协议版本不匹配
#define	ERR_MMS_INVALIDPASSWORD			30001		//无效的会议密码
#define	ERR_MMS_ALREADYLOGIN			30002		//该账户已经进入会议
#define	ERR_MMS_WORKGROUPLIMITS			30003		//会议已经超过六人限制
#define	ERR_MMS_NOMCU					30004		//会议没有适当的MCU服务器
#define	ERR_MMS_OWNERLEAVED				30005		//会议所有者离开会议
#define	ERR_MMS_ROOMLOCKED				30006		//会议已经锁定
#define	ERR_MMS_INVALIDROOMTYPE			30007		//会议室类型不明确
#define	ERR_MMS_INVALIDMEMBERTYPE		30008		//登录者类型不明确
#define	ERR_MMS_LOSSINVALID				30009		//定损报案号不存在
#define	ERR_MMS_LOSSAGAIN				30010		//定损报案号已被其他人登记
#define	ERR_MMS_LOSSHAVEEND				30011		//定损报案号已经定损结束
#define ERR_MMS_MODULELIMIT				30012		//服务器功能限制(系统会议模块授权没有启用)
#define ERR_MMS_CONNECTLIMIT			30013		//系统到达最大会议成员数限制
#define ERR_MMS_HAVETV					30014		//电视墙设备号已存在
#define ERR_MMS_INVALIDTV				30015		//非有效的电视墙设备号
#define ERR_MMS_HAVETVMATRIX			30016		//电视墙矩阵设备号已存在
#define ERR_MMS_INVALIDTVMATRIX			30017		//非有效的电视墙矩阵设备号
#define ERR_MMS_SERVER					30018		//会议服务器断开连接
#define ERR_MMS_AUDITORLIMIT			30019		//会议旁听者最大数量限制
#define ERR_MMS_NONCONSOLE				30020		//该终端不是会议控制台类型
#define ERR_MMS_TOUCHSCREENLIMIT		30021		//电子白板授权限制
#define ERR_MMS_GROUP					30022		//会议组操作出错
#define ERR_MMS_SYNCPARAMETER			30023       //参数同步时的对象出错或未找到满足条件的会议参数
#define ERR_MMS_DUPLICATEMPS			30024       //已经存在相同类型的MPS
#define ERR_MMS_UNAVAILABLE				30025       //不符合邀请MPS条件
#define ERR_MMS_NOAVAILABLEMPS			30026       //没有空闲的MPS
#define ERR_MMS_APPLYMEMORYFAILED		30027       //申请内存失败
#define ERR_MMS_INVITEMPSSUCCESS		30028       //邀请MPS成功
#define ERR_MMS_LOGINSTOPEDROOM			30029		//会议还没有召开
#define ERR_MMS_NOAVAILABLEROOM         30030       //没有合适的会议室
#define ERR_MMS_ROLLCALLING				30031		//点名正在进行中(同一会议室内同时刻只能发起一次点名操作)
#define ERR_MMS_VOTEING					30032		//投票进行中(同一会议室内同时刻只能发起一次投票操作)
#define ERR_MMS_HANDLING				30033		//举手进行中(同一会议室内同时刻只能发起一次举手操作)
#define ERR_MMS_LIMITEONECONSOLE		30034		//会议中限制一个会议控制器在线
#define ERR_MMS_DIGITALCODETIMEOUT		30035		//数字功能进会邀请码已过期
#define ERR_MMS_SETTEMERR				30036		//模版设置错误
#define ERR_MMS_SETSWICHAUTTEMERR		30037		//手动切换自动模版时与广播冲突的错误
//#define ERR_MMS_SETAUTTEMERR			30038		//自动切换手动或手动切换自动模版错误
#define ERR_MMS_RRGOUPRUNNING			30039		//此会议有轮循分组正在轮循
#define	ERR_MMS_STARTRRFAILED			30040		//开始轮循失败
#define ERR_MMS_STOPRRFAILED			30041		//停止轮循失败
#define ERR_MMS_NORRGOUPRUNNING			30042		//此会议无轮循分组正在轮循

#define ERR_MMS_TRSCHLEXIST				30043		//所请求的转码通道已经被转过码
#define ERR_MMS_NOAVAILABLETRSERVER		30044		//没有空闲的转码服务器
#define ERR_MMS_TRSNOTFINDCHLINF		30045		//所请求的转码通道信息有误(未发布,音视頻id有误等)
//---------------------------------------------------------------------------------------
// 短信类错误代码
//---------------------------------------------------------------------------------------
#define ERR_SMS_PROTOCOL				40000		//通讯协议版本不匹配
#define	ERR_SMS_GATEWAY					40001		//短信网关发送失败
#define	ERR_SMS_INVALIDSEARIALNO		40002		//序列号码为空或无效
#define	ERR_SMS_NETWORK					40003		//连接到短信网关发生网络故障
#define	ERR_SMS_OTHERS					40004		//连接到短信网关发生其他故障
#define	ERR_SMS_ENTRYLIMITS				40005		//手机号码为空或者超过1000个
#define	ERR_SMS_SPLITCHAR				40006		//手机号码分割符号不正确
#define	ERR_SMS_ERRORNUMBER				40007		//部分手机号码不正确，已删除，其余手机号码被发送
#define	ERR_SMS_CONTENTOVERFLOW			40008		//短信内容为空或超长(70个汉字)
#define	ERR_SMS_BILLING					40009		//短信网关计费失败
#define	ERR_SMS_UNKNOWN					40010		//未知错误
#define ERR_SMS_MODULELIMIT				40011		//服务器功能限制(系统短信模块授权没有启用)
#define ERR_SMS_INVALIDTYPE				40012		//短信类型未定义
#define ERR_SMS_EMPTYCODE				40013		//发送号或接收号为空

//---------------------------------------------------------------------------------------
// 监控类类错误代码
//---------------------------------------------------------------------------------------
#define ERR_MON_PROTOCOL				50000		//通讯协议版本不匹配
#define ERR_MON_REUSESERIAL				50001		//序列号已被其他帐户绑定
#define	ERR_MON_INVALIDACCOUNT			50002		//无效的监控设备(或网关)账户(系统没有该账户ID)
#define	ERR_MON_INVALIDPASSWORD			50003		//无效的密码
#define	ERR_MON_INVALIDSERIAL			50004		//无效的序列号
#define	ERR_MON_ALREADYLOGIN			50005		//已经登录
#define ERR_MON_INVALIDTYPE				50006		//非法的终端类型
#define ERR_MON_MODULELIMIT				50007		//服务器功能限制(系统监控模块授权没有启用)
#define ERR_MON_CONNECTLIMIT			50008		//系统到达最大通道数限制
#define ERR_MON_MGWCOUNTLIMIT			50009		//网关设备数超过最大限制
#define ERR_MON_MOBCOUNTLIMIT			50010		//移动设备数超过最大限制

//---------------------------------------------------------------------------------------
// 离线文件类错误代码
//---------------------------------------------------------------------------------------
#define ERR_NDS_PROTOCOL				60000		//通讯协议版本不匹配
#define ERR_NDS_MCUDISKFULL				60001		//MCU磁盘满
#define ERR_NDS_RECEIVERSPACEFULL		60002		//接收者空间满
#define ERR_NDS_MD5		                60003		//MD5验证失败
#define ERR_NDS_FILENOTEXIST		    60004		//文件不存在
#define ERR_NDS_FILEISUPLOADING		    60005		//文件正在上传中
#define ERR_NDS_FILEISDOWNLOADING		60006		//文件正在下载中
#define ERR_NDS_NETERROR                60007       //网络错误
#define ERR_NDS_DELFAILED				60008		//文件删除失败

//----------------------------------------------------------------------------------------
// 录像服务器错误代码
//----------------------------------------------------------------------------------------
#define ERR_REC_NOSERVER				70000		//没有可用的录像服务器
#define ERR_REC_SVRRES_BUSY				70001		//录播服务器忙
#define ERR_REC_SVR_EXCEPTION			70002		//录播服务器异常
#define ERR_REC_SERVER_INVALIDTYPE		70003		//无效的账户类型
#define ERR_REC_SERVER_CANNOT_STOP		70004		//无法停止录像
#define ERR_REC_STOP					70005		//停止录像
#define ERR_REC_CUT						70006		//切割视频
#define ERR_REC_NO_CONF					70007		//会议没在录像
#define ERR_REC_CONF_CUT				70008		//会议录像分割
#define ERR_REC_CUT_FINISH				70009		//切割视频完成(主要针对单录)
#define ERR_REC_SEVER_UNKNOWN			70499		//未知错误(保留)

//---------------------------------------------------------------------------------------
// 转码类错误代码
//---------------------------------------------------------------------------------------
#define ERR_TRS_COUNT_LIMIT				70501		//通讯协议版本不匹配

//----------------------------------------------------------------------------------------
// 定制业务错误代码
//----------------------------------------------------------------------------------------
#define ERR_CTS_MEDIC_ROOMNONEXIST		80001		//无此手术室
#define ERR_CTS_MEDIC_UNBINDFAILED		80002		//手术室解除绑定失败
#define ERR_CTS_MEDIC_BINDFAILED		80003		//手术室绑定失败
#define ERR_CTS_MEDIC_PATIENTADDED		80004		//病人已经添加
#define ERR_CTS_MEDIC_ADDPATIENTFAILED	80005		//添加病人信息失败
#define ERR_CTS_MEDIC_PATIENTNOTEXIST	80006		//病人信息不存在
#define ERR_CTS_MEDIC_NORECORD			80007		//没有查询到病患记录
