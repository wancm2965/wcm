#ifdef SUPPORT_VERSION_62	//为同时兼容AvconPlayer和HPRecord


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


#else


#ifndef _AVFRECORD_H
#define _AVFRECORD_H

#define		SEEK_EVENT_BEGIN			1
#define		SEEK_EVENT_END				2

#define AVC_FRAME_TYPE_VIDEO	0
#define AVC_FRAME_TYPE_AUDIO	1
#define AVC_FRAME_TYPE_TIME		2
#define	AVC_FRAME_TYPE_START	3			//开始帧
#define	AVC_FRAME_TYPE_STOP		4			//结束帧
#define	AVC_FRAME_TYPE_USER_LIST		5	//用户列表流
#define AVC_FRAME_TYPE_AVMSVIDEO 125
#define AVC_FRAME_TYPE_AVMSAUDIO 126
#define AVC_FRAME_TYPE_VIDEO2    127			//无压缩帧

#define AVFIM_FRAME_TYPE_AVDLGLIST	6	//AV窗口详细信息列表
#define AVFIM_FRAME_TYPE_ADDAVDLG	7	//增加的AV窗口详细信息
#define AVFIM_FRAME_TYPE_DELAVDLG	8	//减少的AV窗口信息


//中间过渡版 播放“远程录制到本地”无法播放的录像（技术支持需要）
//#define AVC_FILE_HEADER_OLD		AVC_FILE_HEADER 

#define AVC_RECORD_FILE_NORMAL			0			// AVC_FILE_HEADER_OLD
#define AVC_RECORD_FILE_FRIST			1			// AVC_FILE_HEADER
#define AVC_RECORD_FILE_SECOND			2			// 过渡版本，
#define AVC_RECORD_FILE_THIRD			3

#define AVC_REC_DSC_LEN					256 //描述文本长度
#define AVC_REC_PWD_LEN					128	//MD5密码长度
#define AVC_REC_NME_LEN					128	//文件名长度 
#define AVC_REC_CHK_LEN					32	//校验码长度

#define AVC_RECORD_NAMELEN					128
#define AVC_RECORD_DESCRIPTION				256			//录像文件的描述信息长度
#define AVC_RECORD_PASSWORDLEN				128			//密码长度
#define AVC_RECORD_NORMAL					0			//常规录像
#define AVC_RECORD_ALERT					1			//报警录像


//录象类型 0、1、2、3、4 对应于 _T("计划录像"),_T("手动录像"),_T("移动侦测录像"),_T("紧急报警录像"),_T("感应报警录像")
#define RT_SCHEDULE			0
#define RT_MANUAL			1
#define RT_MOVEDETECT		2
#define RT_ALARM			3
#define RT_MOVEORALARM		4
#define RT_MOVEANDALARM		5
#define RT_COMMAND			6

// 录像时间，这在录像查询时很有意义的关键字
typedef struct tag_AVC_FILE_TIME
{	
	unsigned long dwYear;
	unsigned char byMonth,byDay,byHour,byMinute,bySecond;
}AVC_FILE_TIME,*PAVC_FILE_TIME ;

//*********************************  文件头定义  *********************************
// version AVC_RECORD_FILE_NORMAL
// 会议录像avcon4 & avcon6
typedef struct tag_AVC_FILE_HEADER_OLD
{
	unsigned long		version		;		//版本号
	unsigned long		flag		;		//标识符
	char				password[16];		//密码
}AVC_FILE_HEADER_OLD, *PAVC_FILE_HEADER_OLD;

// version AVC_RECORD_FILE_FRIST
// AVC文件结构(扩展)  flag=AVC_RECORD_FILE_SURVEILLANCE时采用如下结构
typedef struct tag_AVC_FILE_HEADER_SURV
{
	//AVC_FILE_HEADER_OLD
	unsigned long		version		;		//版本号
	unsigned long		flag		;		//标识符
	char				password[16];		//密码

	// 新增字段
	unsigned long			dwsize;					//以下各定义的字节数总和
	unsigned char			byRecorderType;			//录像类型,指明计划录像、报警录像等
	AVC_FILE_TIME	starttime;				//录像开始时间
	AVC_FILE_TIME	endtime;				//录像结束时间
	unsigned char			byRecorderDev[256];		//录像设备名称(系统内唯一设置，比如监控中的编码器序列号)
}AVC_FILE_HEADER_SURV, *PAVC_FILE_HEADER_SURV;

// version AVC_RECORD_FILE_SECOND
// AVC文件扩展结构《AVCON系统录像文件结构定义》
typedef struct tag_AVC_FILE_HEADER
{
	unsigned long   dwsize;								//本结构体的长度,本版本修订的内容
	int		flag;								// =AVC_RECORD_FILE_SECOND
	char	password[96];
	char	checksum[32];
	//char	password[AVC_RECORD_PASSWORDLEN];	//密码,MD5
	unsigned char	byRecorderType;						//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME starttime;					//录像开始时间
	AVC_FILE_TIME endtime;						//录像结束时间
	unsigned char	byRecorderDev[AVC_RECORD_NAMELEN];	//录像设备名称,系统内唯一
	unsigned char	bytChannelNo;						//通道号
	unsigned char	bytDesc[AVC_RECORD_DESCRIPTION];	//录像文件描述性信息
	unsigned char	bytPriorFile[AVC_RECORD_NAMELEN];	//前一个文件名称,NULL=无
	unsigned char	bytNextFile [AVC_RECORD_NAMELEN];	//后一个文件名称,NULL=无
	unsigned char	bytRecordState;						//录像状态,0=正在录像,1=录像完毕
	unsigned char	bytUploadState;						//上传状态,0=未上传,1=正上传,2=上传完毕,3=校正完毕
	unsigned long	bytUploadBytes;						//已经上传的节字数
}AVC_FILE_HEADER, *PAVC_FILE_HEADER;

// version AVC_RECORD_FILE_THIRD
// 增加timestamp字段，checksum移至后边
typedef struct tag_AVC_FILE_HEADER_THIRD
{
	unsigned long 			dwsize;						    //本结构体的长度,本版本修订的内容
	unsigned long 			flag;						    //=AVC_RECORD_FILE_THIRD
	unsigned char	 		password[AVC_REC_PWD_LEN];	    //密码
	unsigned char	 		bytRecordType;					//AVC_RECORD_ NORMAL\ AVC_RECORD_ ALERT
	AVC_FILE_TIME	starttime;						//录像开始时间
	AVC_FILE_TIME	endtime;						//录像结束时间
	unsigned char	 		bytRecordDev[AVC_REC_NME_LEN]; 	//录像设备名称,系统内唯一
	unsigned char	 		bytChannelNo;					//通道号
	unsigned char	 		bytDesc[AVC_REC_DSC_LEN];	  	//录像文件描述性信息
	unsigned char	 		bytPriorFile[AVC_REC_NME_LEN]; 	//前一个文件名称,NULL=无
	unsigned char	 		bytNextFile [AVC_REC_NME_LEN]; 	//后一个文件名称,NULL=无
	unsigned char	 		bytRecordState;				  	//0=正在录像,1=录像完毕
	unsigned char	 		bytUploadState;				  	//0=未上传,1=正上传,2=上传完,3=校正完
	unsigned long 			bytUploadBytes;			       	//已经上传的节字数
	unsigned char  			checksum[AVC_REC_CHK_LEN];      //文件MD5校验码
	unsigned long 			timestamp;					    //开始时间轴

	// 20071025
	unsigned char			bytVersion[AVC_REC_NME_LEN];	//产品版本号
}AVC_FILE_HEADER_THIRD, *PAVC_FILE_HEADER_THIRD;
//*********************************  End  *********************************

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

// AVC文件读接口
//int Info(void *buffer, unsigned int dwFlag) dwFlag值
enum {
	AVC_FILE_INFO_RAW=0,		//文件结构
	AVC_FILE_INFO_SUBRAW,		//其中某文件的头结构,buffer传入文件名char*
	AVC_FILE_INFO_FILENAME,		//取文件名,多个文件名之间用'\0'分割,使用前先取缓冲区(buffer)所需大小
	AVC_FILE_INFO_VERSION,
	AVC_FILE_INFO_PASSWORD, 
	AVC_FILE_INFO_CHECKSUM,
	AVC_FILE_INFO_RECORDTYPE, 
	AVC_FILE_INFO_START, 
	AVC_FILE_INFO_STARTEX,
	AVC_FILE_INFO_STATRSTAMP,
	AVC_FILE_INFO_STATRSTAMPEX,
	AVC_FILE_INFO_STOP, 
	AVC_FILE_INFO_STOPEX,
	AVC_FILE_INFO_TIMESTAMP, 
	AVC_FILE_INFO_TIMESTAMPEX, 
	AVC_FILE_INFO_DURATIONEX,
	AVC_FILE_INFO_CHANNELNO,
	AVC_FILE_INFO_RECORDNAME,
	AVC_FILE_INFO_DESC,
	AVC_FILE_INFO_KEYFRAMECOUNT,
	AVC_FILE_INFO_VFRAMECOUNT,
	AVC_FILE_INFO_AFRAMECOUNT,
	AVC_FILE_INFO_STATUS,
	AVC_FILE_INFO_VFRAMECURRENT,
	AVC_FILE_INFO_AUDIO_BITRATE,
	AVC_FILE_INFO_VIDEO_BITRATE,
	AVC_FILE_INFO_MAX
};
enum {
	AVC_FILE_ERROR_SUCESS=0,//成功
	AVC_FILE_ERROR_EXIST,	//已经存在
	AVC_FILE_ERROR_ACCESS,	//文件读取失败
	AVC_FILE_ERROR_RANGE,	//Seek超出范围
	AVC_FILE_ERROR_CHANNEL,	//通道不匹配
	AVC_FILE_ERROR_DATALACK,	//数据未到达，网络用
	AVC_FILE_RRROR_CONNECTFAIL,	//连接失败
	AVC_FILE_ERROR_UNKNOWN=0x80000000//未知错误
};
class IAVFReadEvent
{
public:
	virtual void AVCNetEvent(int nErrCode, const char *ErrDesc)=0;
	virtual void AVCIOEvent()=0;
};
class IAVFReadFactory
{
public:
	virtual	bool Connect(const char* strAVCFile) = 0;
	virtual void ReleaseConnections() = 0;

	virtual bool MoveNext() = 0;
	virtual int ReadFrame(PAVC_DATA_FRAME padf) = 0;
	virtual int ReadData(void *pData, int len) = 0;

	//|__________________|+++++++++++++++++|__________________  /* +指当前的可能位置 |关键帧位置*/
	//SeekPrevKeyFrame   SeekKeyFrame     SeekNextKeyFrame	    /*以下接口将Seek到的位置*/
	virtual int SeekKeyFrame()=0;		//返回关键帧内的帧数
	virtual bool SeekNextKeyFrame()=0;
	virtual bool SeekPrevKeyFrame()=0;

	// 返回往后最近帧的位置(时间)
	virtual unsigned long Seek(unsigned long ulSeekTime)=0;

	//virtual bool Eof() = 0;		//读到文件尾
	//virtual bool Bof() = 0;		//读到文件开头

	// 取文件信息，buffer为NULL时只返回dwFlag字段所需内存大小，返回0时读取字段失败（可能文件版本不对）
	virtual int Info(void *buffer, unsigned int dwFlag)=0;
	//virtual int LastErrCode()=0;

	virtual unsigned long GetDuration() = 0;
	virtual unsigned long GetPosition() = 0;
	//virtual void DelayVideo(long ulMS) = 0;
	//virtual int	 DropPreAudio() = 0;
	virtual unsigned long SeekKeyFrame(unsigned long value, int type)=0;
	virtual int GetFileHeaderLength()=0;
	virtual bool IsBuffering()=0;
};
#ifdef __cplusplus
extern "C" {
#endif
	bool CreateAVFReadFactory(IAVFReadFactory** ppARF, int type=0);
#ifdef __cplusplus
}
#endif
#endif


#endif


