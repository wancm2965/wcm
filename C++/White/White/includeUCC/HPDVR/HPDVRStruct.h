/************************************************************************/
/* HPDVRStruct.h                                                        */
/************************************************************************/
#pragma once

#include <map>
#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////
// + 结构体 [2/27/2008]

/*
*	NDVR
*/

typedef struct tagDVR_STATE/*DVR状态*/
{
	UINT size;						
	LONG cpu_usage;/*cpu占用率*/
	LONG mem_usage;/*内存占用量*/
	LONG part_space;/*分区空间*/
	LONG up_speed;/*上行速率*/
	LONG down_speed;/*下行速率*/
	LONG thread_num;/*线程数*/
	LONG issued_num;/*发布数*/
	CHAR boot_time[24];/*启动时间*/
	LONG reboot_num;/*重启次数*/
	LONG state;/*状态*/
}HD_DVR_STATE, *LPHD_DVR_STATE;

typedef struct tagDVR_CONNECT/*DVR连接*/
{
	UINT size;						
	CHAR server[64];/*服务*/
	CHAR ip0[64];/*IP0*/
	CHAR ip1[64];/*IP1*/
	CHAR ip2[64];/*IP2*/
	LONG port;/*端口*/
}HD_DVR_CONNECT, *LPHD_DVR_CONNECT;

typedef struct tagDVR_INFO/*DVR信息*/
{
	UINT size;						
	CHAR name[64];/*名称*/
	CHAR serial[48];/*序列号*/
	CHAR version[32];/*版本号*/
	LONG dev_num;/*设备数*/
	LONG chn_num;/*通道数*/
	LONG part_num;/*分区数*/
	LONG part_size;/*分区容量*/
	LONG uart_num;/*串口数*/
	LONG host_num;/*主机数*/
	LONG host_in_num;/*输入端口数*/
	LONG host_out_num;/*输出端口数*/
	HD_DVR_STATE state;/*状态*/
	HD_DVR_CONNECT connect;/*连接*/
}HD_DVR_INFO, *LPHD_DVR_INFO;


/*
 *	设备
 */
// +设备 [3/3/2008]
typedef struct tagCOD_STATE/*DEV状态*/
{
	UINT size;						
	LONG thread_num;/*线程数*/
	LONG issued_num;/*发布数*/
	CHAR boot_time[20];/*启动时间*/			
	LONG reboot_num;/*重启次数*/
	LONG state;/*状态*/
}HD_COD_STATE, *LPHD_COD_STATE;

typedef struct tagCOD_CONNECT/*设备连接*/
{
	UINT size;						
	CHAR codid[32];						
	CHAR nvs_server[64];/*NVS 服务*/
	CHAR nvs_ip[16];/*NVS IP*/
	LONG nvs_port;/*NVS 端口号*/
	CHAR nvs_uid[32];/*NVS 帐号*/
	CHAR nvs_pwd[32];/*NVS 密码*/
	CHAR avc_uid[32];/*AVCON 帐号*/
	CHAR avc_pwd[32];/*AVCON 密码*/
}HD_COD_CONNECT, *LPDH_COD_CONNECT;

typedef struct tagCOD_INFO/*设备信息*/
{
	UINT size;						
	CHAR codid[32];						
	CHAR name[64];/*名称*/
	LONG type;/*类型*/
	LONG chn_num;/*通道数*/
	CHAR serial[48];/*序列号*/
	CHAR licence[20];/*授权号*/
	HD_COD_STATE state;/*状态*/
	HD_COD_CONNECT connect;/*连接*/
}HD_COD_INFO, *LPHD_COD_INFO;

typedef map<string, HD_COD_INFO>		HD_COD_MAP;
typedef HD_COD_MAP::iterator			HD_COD_MAP_IT;

// +通道 [3/3/2008]
typedef struct tagCOD_CHN/*通道*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
}HD_COD_CHN, *LPHD_COD_CHN;

typedef struct tagHD_MCU/*MCU*/
{
	CHAR mcuid[128];/*MCUID*/
	CHAR mcuaddr[64];/*MCU地址*/
	UINT mcuport;/*MCUPORT*/
	CHAR mcutype[32];/*MCUTYPE*/
}HD_MCU, *LPHD_MCU;

typedef struct tagCHN_CONNECT/*通道连接*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG audid;/*音频ID*/
	LONG vidid;/*视频ID*/
	LONG subid;/*子流ID*/
	HD_MCU mcu;/*MCU*/
	BOOL p2pfirst;/*p2p或MCU优先*/
	BOOL audio_tcpfirst;/*TCP或UDP优先*/
	BOOL video_tcpfirst;/*TCP或UDP优先*/
	CHAR locaddr[16];/*局域网IP*/
	UINT tcpport;/*局域网UDP接流用动态端口*/
	UINT udpport;/*局域网UDP接流用动态端口*/
}HD_CHN_CONNECT, *LPHD_CHN_CONNECT;

typedef struct tagID_NAME
{
	UINT size;						
	LONG id;							
	CHAR desc[64];/*描述*/
}HD_ID_NAME, *LPHD_ID_NAME;

//音频设备
typedef HD_ID_NAME		HD_AUDIO_DEVICE;		
typedef LPHD_ID_NAME	LPHD_AUDIO_DEVICE;	

//视频设备
typedef HD_ID_NAME		HD_VIDEO_DEVICE;		
typedef LPHD_ID_NAME	LPHD_VIDEO_DEVICE;	

//音频编码
typedef HD_ID_NAME		HD_AUDIO_CODEC;		
typedef LPHD_ID_NAME	LPHD_AUDIO_CODEC;	

//视频编码
typedef HD_ID_NAME		HD_VIDEO_CODEC;
typedef LPHD_ID_NAME	LPHD_VIDEO_CODEC;

//视频分辨率
typedef HD_ID_NAME		HD_VIDEO_SIZE;
typedef LPHD_ID_NAME	LPHD_VIDEO_SIZE;

//视频码流
typedef HD_ID_NAME		HD_VIDEO_BITRATE;
typedef LPHD_ID_NAME	LPHD_VIDEO_BITRATE;

//视频帧率
typedef HD_ID_NAME		HD_VIDEO_FRAMERATE;
typedef LPHD_ID_NAME	LPHD_VIDEO_FRAMERATE;

typedef struct tagPTZ_CTL
{
	UINT size;
	LONG ptzcmd;
}HD_PTZ_CTL, *LPHD_PTZ_CTL;

typedef struct tagPRESET/*预置点*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG id;/*预置点*/
	CHAR name[64];/*名称*/
	BOOL def_point;/*默认点*/
	LONG state;/*状态*/
}HD_PRESET, *LPHD_PRESET;

typedef struct tagPTZ_PROTOCOL/*云台协议*/
{
	UINT size;						
	LONG id;/*协议*/
	CHAR name[64];/*名称*/
	LONG baud;/*速率*/
	LONG parity;/*校验方式*/
}HD_PTZ_PROTOCOL, *LPHD_PTZ_PROTOCOL;

typedef struct tagUART/*串口*/
{
	UINT size;						
	CHAR name[64];/*串口名*/
	LONG port;/*串口号*/
	LONG type;/*类型*/
	LONG baud;/*速率*/
	LONG databit;/*数据位*/
	LONG stopbit;/*停止位*/
	LONG parity;/*校验方式*/
}HD_UART, *LPHD_UART;

typedef struct tagCHN_PTZ/*云台*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG delay;/*停留时间(回默认点)*/
	LONG protocol;/*协议*/
	LONG address;/*地址*/
	LONG uart;/*串口*/
	LONG speed;/*控制速度*/
	LONG baud;/*波特率*/
	LONG presetidx;/*预置点索引号*/
	LONG ctltype;/*控制类型 0:普通 1:转到预置点 2:设置预置点 3:清除预置点 4:取预置点(暂无效)*/
}HD_CHN_PTZ, *LPHD_CHN_PTZ;

typedef struct tagCHN_BASIC/*基本信息*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道(-1为对讲通道)*/
	CHAR name[64];/*名称*/
	LONG state;/*状态标记*/
	BOOL issue;/*发布标记*/
}HD_CHN_BASIC,*LPHD_CHN_BASIC;

typedef struct tagCHN_AUDIO/*通道音频*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG device;/*设备*/
	CHAR desc[64];/*描述*/
	LONG codec;/*编码*/
	BOOL cap_card;/*采集音频卡*/
	LONG cap_mode;/*采集模式：0:MIC，1:LINE_IN，2:WAVE_OUT_MIXER*/
	BOOL aec;/*回音消除*/
	BOOL agc;/*自动增益*/
	BOOL vad;/*静音检测*/
}HD_CHN_AUDIO,*LPHD_CHN_AUDIO;

typedef struct tagCHN_VSTREAM/*视频流*/
{
	LONG codec;/*编码*/
	UINT resolution;/*分辨率 (PS：-1时表示取主流分辨率的1/4尺寸)*/
	LONG framerate;/*帧率*/
	LONG frametype;/*帧率模式*/
	LONG quality;/*质量*/
	LONG autoquality;/*图像质量自动控制*/
}HD_CHN_VSTREAM, *LPHD_CHN_VSTREAM;

typedef struct tagCHN_VIDEO/*通道视频*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG device;/*设备*/
	CHAR desc[64];/*描述*/
	BOOL denoise;/*图像去噪*/
	LONG brightness;/*光亮度*/
	LONG contrast;/*对比度*/
	LONG saturation;/*饱和度*/
	LONG hue;/*色彩度*/
	LONG gama;/*伽马*/
	LONG sharpen;/*锐化*/
	LONG substream;/*是否启用子流*/
	LONG tomcu;/*是否发布到MCU*/

	HD_CHN_VSTREAM hp_video[2];/*华平*/
	NET_DVR_COMPRESSION_INFO hc_video[2];/*海康*/
	DH_VIDEOENC_OPT dh_video[6]; /*大华 0,1,2录像流，3,4,5辅助流*/
	DWORD dwImageSizeMask; /*分辨率掩码，按位表示设备能够支持的分辨率*/
	DWORD dwEncodeModeMask;/*编码模式掩码，按位表示设备能够支持的编码模式*/
}HD_CHN_VIDEO,*LPHD_CHN_VIDEO;

typedef struct tagCHN_AVINFO			//通道音视频	
{
	UINT size;
	HD_CHN_AUDIO audio;/*音频参数*/
	HD_CHN_VIDEO video;/*视频参数*/
}HD_CHN_AVINFO,*LPHD_CHN_AVINFO;

typedef enum 
{
	HD_VIDEO, /*视频*/
	HD_AUDIO /*音频*/
}HDEM_AVTYPE; /*音频或者视频*/
typedef struct tagCHN_INFO/*通道信息*/
{
	UINT size;				
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	HD_CHN_BASIC basic;/*基本信息*/
	HD_CHN_AUDIO audio;/*音频*/
	HD_CHN_VIDEO video;/*视频*/
	HD_CHN_PTZ ptz;/*云台*/
	HD_CHN_CONNECT connect;/*连接*/
	LONG hwnd;/*窗口指针*/
}HD_CHN_INFO, *LPHD_CHN_INFO;

typedef map<LONG, HD_CHN_INFO>		    HD_CHN_MAP;
typedef HD_CHN_MAP::iterator			HD_CHN_MAP_IT;
typedef map<string, HD_CHN_MAP>			HD_COD_CHN_MAP;
typedef HD_COD_CHN_MAP::iterator		HD_COD_CHN_MAP_IT;


// +区域功能 [3/5/2008]
typedef struct tagVIDEO_RECT/*视频区域*/
{
	UINT size;						
	LONG num;/*区数*/
	RECT rects[5];/*区域*/
}HD_VIDEO_RECT, *LPHD_VIDEO_RECT;

typedef struct tagVIDEO_MOTION/*移动侦测*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG level;/*等级*/
	LONG state;/*状态*/
	HD_VIDEO_RECT rects;/*区域*/

	NET_DVR_MOTION hc_motion;/*海康*/
	DH_MOTION_DETECT_CFG dh_motion;/*大华*/
}HD_VIDEO_MOTION, *LPHD_VIDEO_MOTION;

typedef struct tagVIDEO_COVER/*视频遮掩*/
{
	UINT size;						
	CHAR codid[32];/*编码器*/
	LONG chnid;/*通道*/
	LONG state;/*状态*/
	HD_VIDEO_RECT rects;/*区域*/

	NET_DVR_SHELTER hc_cover[MAX_SHELTERNUM];/*海康*/
	DHDEV_VIDEOCOVER_CFG dh_cover;/*大华*/
}HD_VIDEO_COVER, *LPHD_VIDEO_COVER;

typedef struct tagVIDEO_BLOCK/*视频遮挡*/
{
	UINT size;						
	CHAR codid[32];/*设备*/
	LONG chnid;/*通道*/
	LONG state;/*状态*/
	HD_VIDEO_RECT rects;/*区域*/

	NET_DVR_HIDEALARM hc_block;/*海康*/
	DH_BLIND_CFG dh_block;/*大华*/
}HD_VIDEO_BLOCK, *LPHD_VIDEO_BLOCK;


/*
 *	报警
 */
typedef struct tagALM_INPORT			//输入端口
{
	UINT size;
	CHAR host[32];						//主机
	LONG port;							//端口
	CHAR desc[128];						//描述
	LONG type;							//类型
	LONG state;							//状态
}HD_ALM_INPORT, *LPHD_ALM_INPORT;

typedef struct tagALM_OUTPORT			//输出端口
{
	UINT size;
	CHAR host[32];						//主机
	LONG port;							//端口
	CHAR desc[128];						//描述
	LONG type;							//类型
	LONG state;							//状态
}HD_ALM_OUTPORT, *LPHD_ALM_OUTPORT;

typedef struct tagALM_DEVICE			//报警设备
{
	UINT size;					
	CHAR host[32];						//主机
	CHAR desc[128];						//描述
	LONG type;							//类型
	LONG uart;							//串口
	LONG state;							//状态
	HD_ALM_INPORT  inputs[16];			//输入
	HD_ALM_OUTPORT outputs[16];			//输出
}HD_ALM_DEVICE, *LPHD_ALM_DEVICE;

typedef enum
{
	EM_ALM_SRC_HOST,
	EM_ALM_SRC_VIDEO,
	EM_ALM_SRC_DISK,
	EM_ALM_SRC_SOFT
}HDEM_ALM_SRC_TYPE;

typedef struct tagALM_SOURCE			//报警源
{
	UINT size;
	LONG id;
	CHAR desc[128];
	LONG type;
	LONG state;
	union
	{
		struct // 主机报警
		{
			CHAR host[32];
			LONG port;
		}host;

		struct // 视频报警
		{
			CHAR codid[32];
			LONG chnid;
			LONG type;
		}video;

		struct // 硬盘异常
		{
			LONG type; 
		}disk;

		struct // 软件异常
		{
			LONG type;
		}soft;
	};
}HD_ALM_SOURCE, *LPHD_ALM_SOURCE;

typedef enum
{
	EM_ALM_TRG_HOST,
	EM_ALM_TRG_RECORD,
	EM_ALM_TRG_PICTURES,
	EM_ALM_TRG_PTZ,
	EM_ALM_TRG_CENTER,
	EM_ALM_TRG_CLIENT,
	EM_ALM_TRG_SMS,
	EM_ALM_TRG_EMAIL,
	EM_ALM_TRG_CALL
}HDEM_ALM_TRG_TYPE;

typedef struct tagALM_TARGET		// 报警目标
{
	UINT size;
	LONG id;
	CHAR desc[128];
	LONG type;
	LONG state;
	union
	{
		struct // 报警输出
		{
			CHAR host[32];
			LONG port;
		}host;

		struct // 触发录像
		{
			CHAR codid[32];
			LONG chnid;
		}record;

		struct // 触发连拍
		{
			CHAR codid[32];
			LONG chnid;
			LONG photo_num;
			LONG step;
		}pictures;

		struct // 联动云台
		{
			CHAR codid[32];
			LONG chnid;
			LONG prest;
		}ptz;

		struct // 上传中心
		{
			LONG type;
		}center;

		struct // 短信通知
		{
			CHAR name[32];
			CHAR phone[16];
		}sms;

		struct // 邮件通知
		{
			CHAR name[32];
			CHAR mail[128];
		}email;

		struct // 电话通知
		{
			CHAR name[32];
			CHAR phone[16];
		}call;
	};
}HD_ALM_TARGET, *LPHD_ALM_TARGET;


typedef struct tagALM_RELATION			//报警表
{
	UINT size;
	LONG id;
	CHAR sources[256];					//Format: S1&S2&S3 或 S1|S2|S3	
	CHAR targets[256];					//Format: T1,T2,T3
}HD_ALM_RELATION, *LPHD_ALM_RELATION;

typedef struct tagALM_RELATION_REQ		//查询
{
	UINT size;
	UINT mask;
	LONG id;							//Mask 0x01
	CHAR sources[256];					//Mask 0x02
	CHAR targets[256];					//Mask 0x03
}HD_ALM_RELATION_REQ, *LPHD_ALM_RELATION_REQ;


typedef struct tagALM_LOG				//报警日志
{
	UINT size;						
	LONG id;							//id
	LONG relation;						//关联
	CHAR time[20];						//时间	
	BOOL state;							//状态
	CHAR desc[256];						//描述
}HD_ALM_LOG, *LPHD_ALM_LOG;

typedef struct tagALM_LOG_REQ		//查询
{
	UINT size;
	UINT mask;
	CHAR begin[20];						//mask 0x0001
	CHAR end[20];						//mask 0x0002
	LONG idx_begin;						//mask 0x0004
	LONG idx_end;						//mask 0x0008
	LONG relation;						//mask 0x0010
	LONG source;						//mask 0x0020
	LONG target;						//mask 0x0040
	BOOL state;							//mask 0x0080
}HD_ALM_LOG_REQ, *LPHD_ALM_LOG_REQ;




/*
 *	计划
 */
typedef struct tagTIME_SLICE			//时间段
{
	UINT size;
	UINT id;							//时段
	CHAR name[64];						//描述(白天/黑夜/工作/休息/等自定义)
	CHAR begin[8];						//开始时间
	CHAR end[8];						//结束时间
}HD_TIME_SLICE, *LPHD_TIME_SLICE;


typedef enum //日类型
{
	EM_DT_MONTH,	
	EM_DT_WEEK,		
	EM_DT_REST_DAY, 
	EM_DT_WORK_DAY	
}HDEM_DT_TYPE; 

typedef enum //时间类型
{
	EM_TM_POINT,	
	EM_TM_SLICE		
}HDEM_TM_TYPE;

typedef struct tagSCH_SOURCE
{
	UINT size;
	LONG id;
	LONG type;
	CHAR desc[128];
	union
	{
		struct //月
		{
			LONG day;
			CHAR time[8];			
		}month;

		struct //周
		{
			LONG day;
			CHAR time[8];			
		}week;

		struct //休息日
		{
			LONG type;
			union
			{
				CHAR time[8];			
				HD_TIME_SLICE slice;
			};			
		}rest_day;

		struct //工作日
		{
			LONG type;
			union
			{
				CHAR time[8];			
				HD_TIME_SLICE slice;
			};			
		}work_day;
	};
}HD_SCH_SOURCE, *LPHD_SCH_SOURCE;


typedef enum //计划类型
{
	EM_SCH_ALARM,  
	EM_SCH_RECORD, 
	EM_SCH_ADJUST, 
	EM_SCH_SYSTEM, 
	EM_SCH_BACKUP, 
	EM_SCH_REPORT  
}HDEM_SCH_TYPE; 

typedef struct tagSCH_TARGET
{
	UINT size;
	LONG id;
	LONG type;
	LONG state;
	CHAR desc[128];
	union 
	{
		struct // 报警计划
		{
			CHAR host[32];
		}alarm;

		struct // 录像计划
		{
			CHAR codid[32];
			LONG chnid;
		}record;

		struct // 调节计划
		{
			CHAR codid[32];
			LONG chnid;
		}adjust;

		struct // 系统计划
		{
			LONG type;
		}system;

		struct // 备份计划
		{
			LONG type;
		}backup;

		struct // 报告计划
		{
			LONG type;
		}report;
	};
}HD_SCH_TARGET, *LPHD_SCH_TARGET;

typedef struct tagSCH_RELATION			//计划表
{
	UINT size;	
	LONG id;
	CHAR sources[256];					//Format: S1|S2|S3	
	CHAR targets[256];					//Format: T1,T2,T3
}HD_SCH_RELATION, *LPHD_SCH_RELATION;

typedef struct tagSCH_RELATION_REQ		//查询
{
	UINT size;
	UINT mask;
	LONG id;							//Mask 0x01
	CHAR sources[256];					//Mask 0x02
	CHAR targets[256];					//Mask 0x03
}HD_SCH_RELATION_REQ, *LPHD_SCH_RELATION_REQ;



/*
*	录像
*/
typedef struct tagREC_PARTITION
{
	UINT size;
	LONG id;							//分区
	CHAR name[64];						//区名
	LONG state;							//状态
	UINT total;							//容量
	UINT space;							//空间
	UINT blk_size;						//块容量
	UINT blk_space;						//空闲块
	CHAR channels[128];					//通道
	CHAR drivers[128];					//磁盘
	CHAR desc[256];						//描述
}HD_REC_PARTITION, *LPHD_REC_PARTITION;

typedef struct tagREC_BLOCK
{
	UINT size;
	LONG part_id;						//区号
	LONG id;							//块号
	CHAR name[64];						//块名
	LONG state;							//状态
	CHAR folder[256];				    //目录
	CHAR file[128];						//文件
	UINT total;							//容量（32M、64M、128M、256M、512M）
	LONG space;							//空间
	CHAR create_dt[20];					//创建时间
	CHAR modify_dt[20];					//修改时间
	LONG rec_num;						//录像数
	CHAR desc[256];						//描述
}HD_REC_BLOCK, *LPHD_REC_BLOCK;

typedef struct tagREC_LOG				//录像日志
{
	UINT size;
	LONG part_id;						//区号
	LONG blk_id;						//块号
	LONG id;							//日志号
	CHAR codid[32];						//设备号
	LONG chnid;							//通道号
	LONG type;							//类型
	LONG type_detail;					//详细类型
	LONG state;							//状态
	CHAR file[256];						//文件
	CHAR url0[128];						//点播地址0
	CHAR url1[128];						//点播地址1
	CHAR url2[128];						//点播地址2
	LONG length;						//长度
	LONG begin_pos;						//开始位置
	LONG end_pos;						//结束位置
	CHAR begin_dt[20];					//开始时间
	CHAR end_dt[20];					//结束时间
	LONG prev_id;						//prev录像
	LONG next_id;						//next录像
	CHAR desc[256];						//描述
}HD_REC_LOG, *LPHD_REC_LOG;

typedef struct tagREC_PARAM				//录像参数
{
	UINT size;						
	CHAR codid[32];						//设备
	LONG chnid;							//通道
	LONG type;							//类型
	BOOL pre_record;					//预先录像
	LONG pre_length;					//预录时间
	BOOL ext_record;					//延长录像		
	LONG ext_length;					//延长时间
	LONG cycle_mode;					//循环方式
	LONG cover_mode;					//覆盖方式
	LONG cover_days;					//覆盖天数
	LONG storage_mode;					//上传录像
}HD_REC_PARAM, *LPHD_REC_PARAM;

typedef struct tagSTREAM
{
	UINT size;							//结构体+内容的长度
	CHAR codid[32];						//设备id
	LONG chnid;							//通道id
	CHAR stream[1];						//流
}HD_STREAM, *LPHD_STREAM;

typedef struct tagREC_SOURCE			//录像源
{
	UINT size;
	LONG id;
	CHAR codid[32];
	LONG chnid;	
	LONG type;
	union
	{
		struct	// 普通
		{
			LONG type; /*手动、定时*/
		}normal;

		struct // 报警
		{
			LONG type; /*移动侦测、主机IO*/
		}alarm;
	};
}HD_REC_SOURCE, *LPHD_REC_SOURCE;


/*
 *	点播
 */
typedef struct tagRECORD_LOG
{
	UINT size;
	LONG id;
	CHAR type[32];
	CHAR device[64];
	CHAR begin[20];
	CHAR end[20];
	CHAR local_url[256];
	CHAR remote_url[256];
	LONG state;
}HD_RECORD_LOG, *LPHD_RECORD_LOG;

typedef struct tagRECORD_LOG_REQ
{
	UINT size;
	UINT mask;	
	CHAR begin[20];			//Mask 01
	CHAR end[20];			//Mask 02
	LONG id_begin;			//Mask 04
	LONG id_end;			//Mask 08
	CHAR codid[32];			//Mask 10
	LONG chnid;				//Mask 20
	LONG type;				//Mask 40
	LONG state;				//Mask 80
}HD_RECORD_LOG_REQ, *LPHD_RECORD_LOG_REQ;

/*
 *	日志
 */
typedef struct tagSYSTEM_LOG
{
	UINT size;
	LONG id;
	UINT session;
	CHAR user[32];
	CHAR cmd[32];
	CHAR type[32];
	CHAR time[20];
	LONG state;
	CHAR desc[256];
}HD_SYSTEM_LOG, *LPHD_SYSTEM_LOG;

typedef struct tagSYSTEM_LOG_REQ
{
	UINT size;
	UINT mask;
	CHAR begin[20];			//Mask 01
	CHAR end[20];			//Mask 02
	LONG id_begin;			//Mask 04
	LONG id_end;			//Mask 08
	CHAR user[32];			//Mask 10
	UINT cmd;				//Mask 20
	LONG type;				//Mask 40
	LONG state;				//Mask 80
}HD_SYSTEM_LOG_REQ, *LPHD_SYSTEM_LOG_REQ;


/*
 *	权限
 */
typedef struct tagUSER
{
	UINT size;
	CHAR user[32];
	CHAR password[32];
}HD_USER, *LPHD_USER;

typedef struct tagUSER_INFO
{
	UINT size;
	UINT user_id;				//for get&modify
	CHAR user_name[32];
	CHAR password[32];
	CHAR reg_time[20];			//注册时间，模块内系统生成
	CHAR act_time[20];			//最后活动时间，模块内系统生成
	UINT len;
	UINT permission[1];
}HD_USER_INFO, *LPHD_USER_INFO;

/*
 *	地图
 */
typedef struct tagMAP_INFO		//地图传递信息
{
	UINT size;					//结构体大小
	CHAR map_id[40];			//地图ID GUID
	CHAR region_id[256];		//域ID
	CHAR file_hash[40];			//MD5检验码 同时也是文件名称
	LONG type;					//0 地图块文件 1 地图热点图片 2 地图索引文件
	LONG length;				//图片长度
	CHAR content[1];			//文件内容
}HD_MAP_INFO, *LPHD_MAP_INFO;



/*
 *	消息/回调返回结构体参数
 */
typedef struct tagMSG_DATA
{
	CHAR*data;					//返回数据 发包结构体
	LONG err_no;				//错误号 0正确 负数:错误
}HD_MSG_PARAM, *LPHD_MSG_PARAM;

/*
 *	日志/权限封包
 */
typedef struct tagCMD_DATA
{
	UINT size;
	UINT cmd;
	LONG state;
	UINT len;
	CHAR data[1];				//命令包
}HD_CMD_DATA, *LPHD_CMD_DATA;
//  [2/27/2008]
//////////////////////////////////////////////////////////////////////////