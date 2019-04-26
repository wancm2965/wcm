#ifndef _HP_PLAYBACK_FILENAME_DEFINE
#define _HP_PLAYBACK_FILENAME_DEFINE

#define PLAY_BACK_INFO_FILENAME			"FILE_NAME"			//文件名
#define PLAY_BACK_INFO_TYPE				"FILE_TYPE"			//对应值 0,:本地1:远程
#define PLAY_BACK_INFO_PLAYTYPE			"PLAY_TYPE"			//远程的回放方式 0,文件播放，1时间播放
#define PLAY_BACK_INFO_IP				"LOGIN_IP"			//远程登录IP
#define PLAY_BACK_INFO_USER				"LOGIN_USER"		//对远程来说 用户名，本地为空
#define PLAY_BACK_INFO_PWD				"LOGIN_PWD"			//远程登录密码
#define PLAY_BACK_INFO_PORT				"LOGIN_PORT"		//远程登录端口
#define PLAY_BACK_INFO_DEV_TYPE			"DEV_TYPE"			//设备类型 hik;dh;hhs;hp
#define PLAY_BACK_INFO_DEV_TYPEVALUE	"DEV_TYPEVALUE"		//对应的设备类型值
#define PLAY_BACK_INFO_DEV_SUB_TYPE     "DEV_SUB_TYPE"      //对应的设备类型的子类型 蓝星7K-8K
#define PLAY_BACK_INFO_CHAN_ID			"CHAN_ID"			//通道号
#define PLAY_BACK_INFO_START_TIME		"START_TIME"		//开始时间
#define PLAY_BACK_INFO_END_TIME			"END_TIME"			//结束时间
#define PLAY_BACK_INFO_URL				"FILE_URL"			//原先拼凑的字符段,兼容以前的写法
#define PLAY_BACK_INFO_FILE_SIZE		"FILE_SIZE"			//文件大小



//大华文件专用
#define PLAY_BACK_INFO_DH_BHINT			"DH_BHINT"			//大华文件的文件索引
#define PLAY_BACK_INFO_DH_STARTCLSTR	"DH_STARTCLUSTER"	//大华文件的起始簇号
#define PLAY_BACK_INFO_DH_DRIVENO		"DH_DERIVNO"		//磁盘号(区分网络录像和本地录像的类型，0－127表示本地录像，128表示网络录像)
#define PLAY_BACK_INFO_DH_LDURATIONTIME	"DH_LDURATIONTIME"	//播放时间

//NETDVR
#define PLAY_BACK_INFO_NETDVR_FILETYPE		"NETDVR_FILETYPE"		//分辨率类型
#define PLAY_BACK_INFO_NETDVR_IMAGEFORMAT	"NETDVR_IMAGEFORMAT"	//图象类型
#define PLAY_BACK_INFO_NETDVR_STREAMFLAGE	"NETDVR_STREAMFLAGE"	//流类型
#define PLAY_BACK_INFO_NETDVR_OFFSET		"NETDVR_OFFSET"			//


#endif
