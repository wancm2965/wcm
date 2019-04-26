#ifndef __AVFRECORD_H__
#define __AVFRECORD_H__

// include stl
#include <map>
#include <list>
#include <queue>   
#include <string>
#include <vector>
#pragma warning (disable:4786)
using namespace std;

#define	SEEK_EVENT_BEGIN				1
#define	SEEK_EVENT_END					2

//帧类型
#define	AVC_FRAME_TYPE_TIME				0	//时间帧
#define	AVC_FRAME_TYPE_START			1	//开始帧
#define	AVC_FRAME_TYPE_STOP				2	//结束帧
#define	AVC_FRAME_TYPE_VIDEO			3	//视频流
#define	AVC_FRAME_TYPE_AUDIO			4	//音频流

//录像主类型
#define AVC_RECORD_NORMAL				0	//常规录像
#define AVC_RECORD_ALERT				1	//报警录像

//录像类型
#define AVC_RECORD_SCHEDULE				0	//定时录像		
#define AVC_RECORD_MANUAL				1	//手动录像
#define AVC_RECORD_MOVEDETECT			2	//移动侦测录像
#define AVC_RECORD_ALARM				3	//IO报警录像
#define AVC_RECORD_MOVEORALARM			4	//动测|IO报警录像
#define AVC_RECORD_MOVEANDALARM			5	//动测&IO报警录像

#define AVC_REC_DSC_LEN					256 //描述文本长度
#define AVC_REC_PWD_LEN					128	//MD5密码长度
#define AVC_REC_NME_LEN					128	//文件名长度 
#define AVC_REC_CHK_LEN					32	//校验码长度

//录像时间
typedef struct tag_AVC_FILE_TIME
{	
	DWORD	dwYear;		// 年
	BYTE	byMonth;	// 月
	BYTE	byDay;		// 日
	BYTE	byHour;		// 时
	BYTE	byMinute;	// 分
	BYTE	bySecond;	// 秒
}AVC_FILE_TIME,*PAVC_FILE_TIME ;


//////////////////////////////////////////////////////////////////////////
//AVC文件结构
//文件头结构版本号
#define AVC_RECORD_FILE_NORMAL			0	//AVC_FILE_HEADER_OLD
#define AVC_RECORD_FILE_SURVEILLANCE	1	//AVC_FILE_HEADER_SURV
#define AVC_RECORD_FILE_EXTRA			2	//AVC_FILE_HEADEREx
#define AVC_RECORD_FILE_EXTRA3			3	//AVC_FILE_HEADEREx3
#define AVC_RECORD_FILE_VERSION_4		4	//AVC_FILE_HEADEREx3_高清库
//第一个版本
typedef struct tag_AVC_FILE_HEADER_OLD
{
	unsigned long	version;			//版本号
	unsigned long	flag;				//标识符 AVC_RECORD_FILE_NORMAL
	char			password[16];		//密码
}AVC_FILE_HEADER_OLD, *PAVC_FILE_HEADER_OLD;

//第二个版本
typedef struct tag_AVC_FILE_HEADER_SURV
{
	//原版本：
	unsigned long	version;			//版本号
	unsigned long	flag;				//标识符 AVC_RECORD_FILE_SURVEILLANCE
	char			password[16];		//密码
	DWORD			dwsize;				//以下各定义的字节数总和
	BYTE			byRecorderType;		//录像类型,指明计划录像、报警录像等
	AVC_FILE_TIME	starttime;			//录像开始时间
	AVC_FILE_TIME	endtime;			//录像结束时间
	BYTE			byRecorderDev[256];	//录像设备名称(系统内唯一设置，比如监控中的编码器序列号)
}AVC_FILE_HEADER_SURV, *PAVC_FILE_HEADER_SURV;

//第三个版本
typedef struct tag_AVC_FILE_HEADEREx
{
	DWORD			dwsize;					//本结构体的长度,本版本修订的内容
	int				flag;					// AVC_RECORD_FILE_EXTRA
	char			password[128];			//密码,MD5
	BYTE			byRecorderType;			//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;				//录像开始时间
	AVC_FILE_TIME	endtime;				//录像结束时间
	BYTE			byRecorderDev[128];		//录像设备名称,系统内唯一
	BYTE			bytChannelNo;			//通道号
	BYTE			bytDesc[256];			//录像文件描述性信息
	BYTE			bytPriorFile[128];		//前一个文件名称,NULL=无
	BYTE			bytNextFile [128];		//后一个文件名称,NULL=无
	BYTE			bytRecordState;			//录像状态,0=正在录像,1=录像完毕
	BYTE			bytUploadState;			//上传状态,0=未上传,1=正上传,2=上传完毕,3=校正完毕
	DWORD			bytUploadBytes;			//已经上传的节字数
}AVC_FILE_HEADEREx, *PAVC_FILE_HEADEREx;

//第四个版本
typedef struct tag_AVC_FILE_HEADEREx3
{
	DWORD 			dwsize;						    //本结构体的长度,本版本修订的内容
	DWORD 			flag;						    //录像库版本号 AVC_RECORD_FILE_EXTRA3
	BYTE	 		password[AVC_REC_PWD_LEN];	    //密码,MD5
	BYTE	 		bytRecordType;					//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;						//录像开始时间
	AVC_FILE_TIME	endtime;						//录像结束时间
	BYTE	 		bytRecordDev[AVC_REC_NME_LEN]; 	//录像设备名称,系统内唯一
	BYTE	 		bytChannelNo;					//通道号
	BYTE	 		bytDesc[AVC_REC_DSC_LEN];	  	//录像文件描述性信息
	BYTE	 		bytPriorFile[AVC_REC_NME_LEN]; 	//前一个文件名称,NULL=无
	BYTE	 		bytNextFile [AVC_REC_NME_LEN]; 	//后一个文件名称,NULL=无
	BYTE	 		bytRecordState;				  	//0=正在录像,1=录像完毕
	BYTE	 		bytUploadState;				  	//0=未上传,1=正上传,2=上传完,3=校正完
	DWORD 			bytUploadBytes;			       	//已经上传的节字数
	BYTE  			checksum[AVC_REC_CHK_LEN];      //文件篡改校验码,MD5
	DWORD 			timestamp;					    //开始时间轴
	BYTE			bytVersion[AVC_REC_NME_LEN];	//产品版本号 如:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)
}AVC_FILE_HEADEREx3, *PAVC_FILE_HEADEREx3;

// 文件头结构
typedef struct tag_AVC_FILE_HEADER //(最新 第四版)
{
	DWORD 			dwsize;						    //本结构体的长度,本版本修订的内容
	DWORD 			flag;						    //录像库版本号 AVC_RECORD_FILE_EXTRA3
	BYTE	 		password[AVC_REC_PWD_LEN];	    //密码,MD5
	BYTE	 		bytRecordType;					//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;						//录像开始时间
	AVC_FILE_TIME	endtime;						//录像结束时间
	BYTE	 		bytRecordDev[AVC_REC_NME_LEN]; 	//录像设备名称,系统内唯一
	BYTE	 		bytChannelNo;					//通道号
	BYTE	 		bytDesc[AVC_REC_DSC_LEN];	  	//录像文件描述性信息
	BYTE	 		bytPriorFile[AVC_REC_NME_LEN]; 	//前一个文件名称,NULL=无
	BYTE	 		bytNextFile [AVC_REC_NME_LEN]; 	//后一个文件名称,NULL=无
	BYTE	 		bytRecordState;				  	//0=正在录像,1=录像完毕
	BYTE	 		bytUploadState;				  	//0=未上传,1=正上传,2=上传完,3=校正完
	DWORD 			bytUploadBytes;			       	//已经上传的节字数
	BYTE  			checksum[AVC_REC_CHK_LEN];      //文件篡改校验码,MD5
	DWORD 			timestamp;					    //开始时间轴
	BYTE			bytVersion[AVC_REC_NME_LEN];	//产品版本号 如:(AVCON4.x.xx AVCON5.x.xx AVCON6.x.xx NDVR2.x.xx)
}AVC_FILE_HEADER, *PAVC_FILE_HEADER;
//////////////////////////////////////////////////////////////////////////

// 数据帧定义
typedef struct tag_AVC_DATA_FRAME
{	
	char				frame_type	;		//帧类型
	char				key_frame	;		//关键帧
	unsigned short		sequence	;		//会话次数
	unsigned long		session_id	;		//会话标识
	int					channel		;		//通道
	unsigned long		timestamp	;		//时间戳
	long				data_length	;		//数据长度
}AVC_DATA_FRAME,*PAVC_DATA_FRAME;

// 时间帧
typedef struct tag_AVC_TIME_FRAME
{
	unsigned long		timestamp	;		//时间戳
	unsigned long		position	;		//指向数据帧位置
}AVC_TIME_FRAME,*PAVC_TIME_FRAME;

// 索引数据头定义
typedef struct tag_AVC_INDEX_HEADER
{
	char				index[4]	;		//索引标志:'INDX'
	char				frame_type	;		//帧类型
	unsigned short		sequence	;		//会话次数
	unsigned long		session_id	;		//会话标识
	int					channel		;		//通道
	unsigned long		start_time	;		//本流的开始时间
	unsigned long		stop_time	;		//本流的结束时间
	long				data_length	;		//本段索引数据长度
}AVC_INDEX_HEADER, *PAVC_INDEX_HEADER;

// 索引定义
typedef struct tag_AVC_INDEX_DATA
{
	unsigned long		pos			;		//位置
	unsigned long		timestamp	;		//时间戳
	char				key_frame	;		//是否关键帧
}AVC_INDEX_DATA, *PAVC_INDEX_DATA;

typedef struct tag_STREAM_INFO
{
	char				frame_type;
	unsigned long		session_id;
	int					channel;
	unsigned short		sequence;	
}STREAM_INFO, *PSTREAM_INFO;
typedef list <STREAM_INFO> LIST_STREAM_INFO;

typedef struct tag_DATA_FRAME
{
	char*		pData;
	int			nLen;
	unsigned long ulTimestamp;
	unsigned long ulSessionId;
}DATA_FRAME, *PDATA_FRAME;
typedef list  <DATA_FRAME>	LIST_DATA_FRAME;
typedef queue <DATA_FRAME>	QUEUE_DATA_FRAME;
typedef vector <DATA_FRAME> VECTOR_DATA_FRAME;

#endif//__AVFRECORD_H__