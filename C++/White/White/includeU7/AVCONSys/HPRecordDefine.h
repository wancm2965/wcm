/************************************************************************/
/*录像定义                                                            */
/************************************************************************/

#ifndef __RECORD_DEFINE_H__
#define __RECORD_DEFINE_H__

#ifdef WIN32
#define RECORD_EXPORT
#ifdef RECORD_EXPORT
#define RECORD_API _declspec(dllexport)
#elif RECORD_DLL
#define RECORD_API _declspec(dllimport)
#else
#define RECORD_API
#endif

#else
#define RECORD_API
#define TRACE  printf
#endif

#include <map>
#include <string>
using namespace std;

#define ERR_LIVE_FAILED					70110		//连接Nginx服务失败;
#define ERR_LIVE_PAUSE					70111		//因视频编码有误，不能直播;
#define ERR_LIVE_FINISH					70112		//直播结束;

typedef enum _RECERR_CODE
{
	REC_START_SUCCESS,			//录像开始成功
	REC_START_RECMAXCNT,		//达到授权数量
	REC_STREAM_FAILED,			//接流失败
	REC_START_EXIST,			//已经在录
	REC_START_DISKFULL,			//磁盘空间不足
	REC_START_NODISE,			//没有磁盘
	REC_CREATEFILE_FAILED,		//创建文件失败
	REC_ONLYCAN_CHLREC,			//只能单路录像（后台设置）
	REC_ONLYCAN_CONFREC,		//只能会议录像（后台设置）
	REC_START_OTHER,			//其它错误
}RECERR_CODE;

//存储模式
typedef enum _RECSAVE_TYPE
{
	REC_SAVE_LOCAL,				//本地存储
	REC_SAVE_SAN,				//SAN存储区域网络
	REC_SAVE_DAS,				//DAS直联存储
}RECSAVE_TYPE;

//录像模式
typedef enum _RECMODE_TYPE
{
	REC_MODE_NORMAL,			//正常录像,磁盘不足停止
	REC_MODE_CYCLE,				//覆盖录像,磁盘不足则删除			
}RECMODE_TYPE;

//录像启动方式
typedef enum _RECSTART_TYPE
{
	RECSTART_TYPE_NORMAL,	//正常手动录像
	RECSTART_TYPE_ALARM,	//警报录像
	RECSTART_TYPE_PLAN,		//计划录像
	RECSTART_TYPE_CONF,		//会议录像
}RECSTART_TYPE;

typedef enum _RECDEL_TYPE
{
	RECDEL_TYPE_SIGNED,	//数据库标志的需要删除的
	RECDEL_TYPE_EARLY,	//磁盘满需要删除最早一天的
}RECDEL_TYPE;

typedef enum _RECSTOP_TYPE
{
	RECSTOP_TYPE_NORMAL,	//正常手动停止
	RECSTOP_TYPE_CUT,		//分割停止
	RECSTOP_TYPE_DISKFULL,	//磁盘满停止
	RECSTOP_TYPE_OTHER,		//其他方式停止
}RECSTOP_TYPE;

typedef enum _RECOVER_RESULT
{
	RECOVER_RESULT_NOFILE,		//文件不存在
	RECOVER_RESULT_NOTACTIVE,	//文件非法
	RECOVER_RESULT_COMPLITE,	//完成
	RECOVER_RESULT_NORMALAVC,	//正常无需修复
}RECOVER_RESULT;

//直播流方式;
typedef enum _STREAM_LIVE_TYPE
{
	STREAM_LIVE_TYPE_DEFAULT		= 0,	//不直播;		
	STREAM_LIVE_TYPE_ALL			= 1,	//直播音视频;
	STREAM_LIVE_TYPE_VIDEO			= 2,	//直播视频;
	STREAM_LIVE_TYPE_AUDIO			= 3,	//直播音频;
}STREAM_LIVE_TYPE;

//录像文件类型;
typedef enum _REC_FILE_TYPE
{
	REC_FILE_TYPE_MP4			= 1,		//mp4类型;
	REC_FILE_TYPE_FLV			= 2,		//flv类型;
	REC_FILE_TYPE_AVMS			= 3,		//avms类型;
	REC_FILE_TYPE_MKV			= 4,		//mkv类型;
	REC_FILE_TYPE_AVI			= 5,		//avi类型;
	REC_FILE_TYPE_TPC			= 6,		//tpc类型;(会议录制专用)
	REC_FILE_TYPE_MREC			= 7,		//mrec类型;(会议录制专用)
	REC_FILE_TYPE_MTPC			= 8,		//mtpc类型;(会议录制专用)
	REC_FILE_LIVE_STREAM		= 9,		//只直播
}REC_FILE_TYPE;

//通道信息
typedef struct _tagRECINFO
{
	_tagRECINFO()
	{
		roomid		= "";
		roomname	= "";
		devid		= "";
		channelid	= "";
		channelname	= "";
		recuserid	= "";
		devinfo		= "";
		nodeid		= "";
		mcuid       = "";
		mcuaddr		= "";
		nataddr		= "";
		localaddr	= "";
		termtype	= "";
		mcuport		= 0;
		localport	= 0;
		videocid	= 0;
		audiocid	= 0;
		recfilename	= "";
		recnickname = "";
		recfilepath = "";
		reccutindex = 0;
		recStatus	= 0;
		starttime	= "";
		endtime		= "";
		streamtype  = 0;
		stopType    = RECSTOP_TYPE_NORMAL;
		ulFileSize	= 0;
		uniquekey	= 0;
		eLiveType	= STREAM_LIVE_TYPE_DEFAULT;
		ePreRecType	= REC_FILE_TYPE_AVMS;
		eRecType	= REC_FILE_TYPE_AVMS;
		//bIsConfRec	= false;
		strCheckNum = "";
	}
	_tagRECINFO& operator=(const _tagRECINFO& mtn)
	{
		roomid			= mtn.roomid;
		roomname		= mtn.roomname;
		devid			= mtn.devid;
		channelid		= mtn.channelid;
		channelname		= mtn.channelname;
		recuserid		= mtn.recuserid;
		devinfo			= mtn.devinfo;
		nodeid			= mtn.nodeid;
		mcuid			= mtn.mcuid;
		mcuaddr			= mtn.mcuaddr;
		mcuport			= mtn.mcuport;
		nataddr			= mtn.nataddr;
		localaddr		= mtn.localaddr;
		localport		= mtn.localport;
		termtype		= mtn.termtype;
		videocid		= mtn.videocid;
		audiocid		= mtn.audiocid;
		recfilename		= mtn.recfilename;
		recnickname     = mtn.recnickname;
		reccutindex     = mtn.reccutindex;
		recfilepath		= mtn.recfilepath;
		recStatus		= mtn.recStatus;
		starttime		= mtn.starttime;
		endtime			= mtn.endtime;
		streamtype		= mtn.streamtype;
		stopType		= mtn.stopType;
		ulFileSize		= mtn.ulFileSize;

		uniquekey		= mtn.uniquekey;
		eLiveType		= mtn.eLiveType;
		ePreRecType		= mtn.ePreRecType;
		eRecType		= mtn.eRecType;
		//bIsConfRec		= mtn.bIsConfRec;
		strCheckNum		= mtn.strCheckNum;

		return *this;
	}
	std::string		roomid;				//会议室ID
	std::string		roomname;			//会议室名称
	std::string		devid;				//被录像的设备号（或人员ID）
	std::string		channelid;			//被录像的通道号
	std::string		channelname;		//对录像通道的别名
	std::string		recuserid;			//发起录像的人员ID
	std::string		devinfo;			//设备信息
	std::string		nodeid;				
	std::string		mcuid;
	std::string		mcuaddr;
	unsigned short	mcuport;
	std::string		nataddr;
	std::string		localaddr;
	unsigned short localport;
	std::string		termtype;
	unsigned long	videocid;
	unsigned long	audiocid;
	std::string		recfilename;	//开始录像后生成
	std::string     recnickname;    //用户输入的别名
	unsigned int    reccutindex;    //切割索引
	std::string		recfilepath;	//开始录像后生成
	unsigned short	recStatus;		//录像状态
	std::string		starttime;		//开始录像时间
	std::string     endtime;		//结束时间
	unsigned short	streamtype;
	RECSTOP_TYPE	stopType;		//停止类型
	unsigned long   ulFileSize;

	unsigned long   uniquekey;		//唯一码，课堂评论功能使用
	STREAM_LIVE_TYPE eLiveType;		//是否直播 0:不直播 1：直播音视频
	REC_FILE_TYPE	ePreRecType;	//预设置录像文件类型;
	REC_FILE_TYPE	eRecType;		//当前的录像文件类型;
	//bool			bIsConfRec;		//主要用于通道录制时标记是否是会议录制MP4格式(服务使用，客户端无需处理)
	std::string		strCheckNum;	//录像文件校验码

}REC_INFO,*LPREC_INFO;

#endif


