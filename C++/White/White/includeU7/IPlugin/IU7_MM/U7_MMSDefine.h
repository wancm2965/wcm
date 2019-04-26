#pragma once
#include "AVCONSys/KMMSDefine.h"

//------------------------------------------------------------------------
// 通道ID定义
typedef enum {
	HPCHANNEL_INDEX_MIN			= 0,		// 成员通道的最小值
	HPIPCAM_INDEX				= 50,		// IP Cam通道ID
	HPCOMPOUNDCHANNEL_INDEX		= 99,		// 合成通道ID
	HPMEDIACHANNEL_INDEX		= 100,		// 媒体共享通道ID
	HPXSSECHANNEL_INDEX,					// 屏幕共享通道ID
	HPCHATCHANNEL_INDEX,					// 讨论通道ID
	HPWBCHANNEL_INDEX,						// 白板通道ID
	HPDOCSHARECHANNEL_INDEX,				// 文档共享通道ID
	HPFILESENDCHANNEL_INDEX,				// 文件分发通道ID
	HPEMAPCHANNEL_INDEX,					// 电子地图通道ID
	HPAVWBCHANNEL_INDEX,					// 白板AV通道ID
	HPAVDOCSHARECHANNEL_INDEX,				// 文档共享AV通道ID
	HPROUNDCHANNEL_INDEX		= 200,		// 轮询通道200-599
	HPROUNDCHANNEL_LOCAL_INDEX	= 600,		// 轮循本地通道600-699
	HPPREVIEW_LOCAL_INDEX		= 700,		// 预览本地视频700-799
	HPH323CALL_INDEX			= 800,		// H323呼叫通道ID 800-899
	HPH323CALL_SECOND_INDEX		= 900,		// H323呼叫辅流通道900-999
	HPIMCAMER_INDEX				= 1000,		// IM使用的通道ID
	HPIMCAMER_INDEX_MAX			= 1009,		// IM使用的最大通道ID
	HPLOCALCOMPOUND_ACTIVE		= 1010,		// 本地合成通道ID，供当前激活屏使用
	HPLOCALCOMPOUND_INDEX		= 1011,		// 本地合成通道起始ID
	HPCOMPOUNDCHANNEL_INDEX_MAX	= 1049,		// 本地合成通道最大ID
	HPAVSET_INDEX,							// 音视频设置时内部的通道ID
	HPCHANNEL_INDEX_MAX			= 1100, 	// 成员通道的最大值
}TCardIndex;

//------------------------------------------------------------------------
// 会议组列表类型
typedef std::map<std::string,PMMS_GROUP_EX> MMS_GROUP_EX_MAP;		//key:strDomain_strRoomID_strGroupID
typedef MMS_GROUP_EX_MAP::iterator MMS_GROUP_EX_MAP_IT;

//------------------------------------------------------------------------
// 成员通道数据结构
typedef struct _MMS_CARD
{
	std::string		memberid;		//成员ID
	int				cardindex;		//卡ID
	std::string		cardname;		//卡名称
	unsigned long	audiochnid;		//音频通道ID
	unsigned long	videochnid;		//视频通道ID
	std::string		mcuid;			//MCU ID
	std::string		mcuaddr;		//MCU 地址
	unsigned short	mcuport;		//MCU 端口
	std::string		devinfo;		//设备信息

	int				screenid;		//屏幕号
	int				windowid;		//窗口号

	_MMS_CARD()
	{
		memberid	= "";
		cardindex	= 0;
		cardname	= "";
		audiochnid	= 0;
		videochnid	= 0;
		mcuid		= "";
		mcuaddr		= "";
		mcuport		= 0;
		devinfo		= "";

		screenid	= -1;
		windowid	= -1;
	};
}MMS_CARD,*PMMS_CARD;

typedef std::map<std::string,PMMS_CARD> MMS_CARD_MAP;		//key:memberid_cardindex
typedef MMS_CARD_MAP::iterator MMS_CARD_MAP_IT;

//////////////////////////////////////////////////////////////////////////
//MPS
typedef struct _ROOM_MPS
{
	std::string		strMPSUserID;
	std::string		strMCUID;
	std::string		strMCUIP;
	unsigned short	usMCUPort;
	std::string		strNodeID;
	std::string		strNATIP;
	std::string		strLocalIP;
	unsigned short	usLocalPort;
	unsigned long	ulMPSMaxCardIndex;
	std::string		strType;
	bool			bSecondFlow;

	_ROOM_MPS()
	{
		strMPSUserID		= "";
		strMCUID			= "";
		strMCUIP			= "";
		usMCUPort			= 0;
		strNodeID			= "";
		strNATIP			= "";
		strLocalIP			= "";
		usLocalPort			= 0;
		ulMPSMaxCardIndex	= 0;
		strType				= "";
		bSecondFlow			= false;
	}
}ROOM_MPS, *PROOM_MPS;
typedef std::map<std::string, PROOM_MPS> MAP_ROOM_MPS;
typedef MAP_ROOM_MPS::iterator MAP_ROOM_MPS_IT;

//报警信息
typedef struct _tagAVCON_MON_ALARMLOG
{
	unsigned long ulAlarmid;		//报警日志ID
	int			 nChanid;			//通道序号
	int          nChannelType;		//通道类型为枚举类型AVCON_ALARMLOG_CHANTYPE
	char		 szChannelType[32];	//通道类型名字
	char		 szDevID[64];		//设备ID
	char		 szDevName[64];		//设备名称
	char		 szChanID[64];		//视频通道ID
	char		 szChanName[64];	//通道名称
	int          nAlarmType;		//报警类型为枚举类型AVCON_ALARMLOG_ALARMTYPE
	char		 szAlarmType[32];	//报警类型名字
	int          nAlarmLevel;		//报警级别
	int          nAlarmlinkID;		//联动预案ID
	char		 szAlarmTime[64];	//报警时间
	_tagAVCON_MON_ALARMLOG()
	{
		ulAlarmid = -1;
		nChanid = -1;
		nChannelType = -1;
		nAlarmType = 0;
		nAlarmLevel = 0;
		nAlarmlinkID = 0;
		memset(szChannelType,0,32);
		memset(szDevID,0,64);
		memset(szDevName,0,64);
		memset(szChanID,0,64);
		memset(szChanName,0,64);
		memset(szAlarmType,0,32);
		memset(szAlarmTime,0,64);
	}
}AVCON_MON_ALARMLOG,*LPAVCON_MON_ALARMLOG;
//////////////////////////////////////////////////////////////////////////