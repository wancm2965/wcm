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

typedef enum _RECERR_CODE
{
	REC_START_SUCCESS,			//录像开始成功
	REC_START_RECMAXCNT,		//达到授权数量
	REC_STREAM_FAILED,			//接流失败
	REC_START_EXIST,			//已经在录
	REC_START_DISKFULL,			//磁盘空间不足
	REC_START_NODISE,			//没有磁盘
	REC_CREATEFILE_FAILED,		//创建文件失败
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
		recfilepath = "";
		recStatus	= 0;
		starttime	= "";
		endtime		= "";
		streamtype  = 0;
		stopType    = RECSTOP_TYPE_NORMAL;
		ulFileSize	= 0;
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
		recfilepath		= mtn.recfilepath;
		recStatus		= mtn.recStatus;
		starttime		= mtn.starttime;
		endtime			= mtn.endtime;
		streamtype		= mtn.streamtype;
		stopType		= mtn.stopType;
		ulFileSize		= mtn.ulFileSize;
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
	std::string		recfilepath;	//开始录像后生成
	unsigned short	recStatus;		//录像状态
	std::string		starttime;		//开始录像时间
	std::string     endtime;		//结束时间
	int				streamtype;
	RECSTOP_TYPE	stopType;		//停止类型
	unsigned long   ulFileSize;

}REC_INFO,*LPREC_INFO;


#endif


