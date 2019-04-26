#pragma once
#include "KIMSDefine.h"
#include "KMMSDefine.h"

//=======================================================================================
// 类型定义
//=======================================================================================

// 连接服务器状态
enum {
	SERVER_CONNECTING		= 1,		// 正在连接
	SERVER_RECONNECTED,					// 重新连接
	SERVER_BUSY,						// 服务器忙
	SERVER_FAILED,						// 连接失败
	SERVER_DISCONNECTED,				// 连接断开
	SERVER_CONNECTED,					// 已连接
};

//使用会议的方式 wangxin
typedef enum {
	MEETING_MODE_NORMAL = 0,	//普通会议
	MEETING_MODE_IM,			//IM视频聊天进入会议
	MEETING_MODE_CALL,
}TMeetingMode;

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
	HPROUNDCHANNEL_INDEX		= 200,		// 轮询通道200-599
	HPROUNDCHANNEL_LOCAL_INDEX	= 600,		// 轮循本地通道600-699
	HPPREVIEW_LOCAL_INDEX		= 700,		// 预览本地视频700-799
	HPH323CALL_INDEX			= 800,		// H323呼叫通道ID 800-899
	HPH323CALL_SECOND_INDEX		= 900,		// H323呼叫辅流通道900-999
	HPIMCAMER_INDEX				= 1000,		// IM使用的通道ID
	HPAVSET_INDEX,							// 音视频设置时内部的通道ID
	HPCHANNEL_INDEX_MAX			= 1100, 	// 成员通道的最大值
}TCardIndex;

//// 音视频类型定义
//typedef enum {
//	HPAV_TYPE_NONE		= 0,
//	HPAV_TYPE_VIDEO		= 100,	// 视频
//	HPAV_TYPE_AUDIO,			// 音频
//	HPAV_TYPE_AV,				// 音视频
//}THPAVStream;

// 音频编码器类型定义
typedef enum {
	HPAV_AUDIO_CODEC_8		= 19/*X_AUDIO_CODEC_AMR_WB_9*/,		// 8kbps
	HPAV_AUDIO_CODEC_16		= 22/*X_AUDIO_CODEC_AMR_WB_16*/,		// 16kbps
	HPAV_AUDIO_CODEC_24		= 42/*X_AUDIO_CODEC_G7221_14_24*/,	// 24kbps
	HPAV_AUDIO_CODEC_32		= 43/*X_AUDIO_CODEC_G7221_14_32*/,	// 32kbps
	HPAV_AUDIO_CODEC_48		= 44/*X_AUDIO_CODEC_G7221_14_48*/,	// 48kbps
}THPAVAudioCodec;

// 视频编码器类型定义
typedef enum {
	HPAV_VIDEO_CODEC_H263P		= 2/*VIDEC_CODEC_H263P*/,		// H263+
	HPAV_VIDEO_CODEC_H264		= 4/*VIDEC_CODEC_H264*/,			// H264+
	HPAV_VIDEO_CODEC_H264_SVC	= 10/*VIDEC_CODEC_H264_SVC*/,		// H264 SVC
}THPAVVideoCodec;

// 网络类型定义
typedef enum {
	HPAV_VIDEO_NETWORK_INTERNET		= 0,	// 互联网
	HPAV_VIDEO_NETWORK_LAN,					// 局域网
	HPAV_VIDEO_NETWORK_PRIVATE,				// 专网
	HPAV_VIDEO_NETWORK_SATELLITE,			// 卫星网
	HPAV_VIDEO_NETWORK_CDMA,				// CDMA
}THPAVVideoNetwork;

//---------------------------------------------------------------------------------------
// 服务器信息结构
typedef struct _IMS_SERVER_ITEM
{
	std::string		strMCUID;
	std::string		strMCUType;
	unsigned long	ulConnections;
	std::string		strBandWidth;
	unsigned long	ulActivity;
	std::string		strIPAddr;
	unsigned short	usIPPort;
	std::string		strIPName;
	int				nMcuRightType;
}IMS_SERVER_ITEM,*PIMS_SERVER_ITEM;

typedef std::map<std::string,IMS_SERVER_ITEM> IMS_SERVER_ITEM_MAP;
typedef IMS_SERVER_ITEM_MAP::iterator IMS_SERVER_ITEM_MAP_IT;

//---------------------------------------------------------------------------------------
// 机构信息结构
typedef struct _IMS_ORG_ITEM
{
	std::string strOrgID;
	std::string strOrgName;

	_IMS_ORG_ITEM()
	{
		strOrgID	= "";
		strOrgName	= "";
	};
}IMS_ORG_ITEM,*PIMS_ORG_ITEM;

typedef std::map<std::string,PIMS_ORG_ITEM> IMS_ORG_ITEM_MAP;
typedef IMS_ORG_ITEM_MAP::iterator IMS_ORG_ITEM_MAP_IT;

typedef struct _IMS_ORG_ITEM_EX :public IMS_ORG_ITEM
{
	std::string strParentKey;
	int		nLevelID;

	_IMS_ORG_ITEM_EX()
	{
		strParentKey	= "";
		nLevelID		= 1;
	};
}IMS_ORG_ITEM_EX,*PIMS_ORG_ITEM_EX;
typedef std::map<std::string,PIMS_ORG_ITEM_EX> IMS_ORG_ITEM_EX_MAP;
typedef IMS_ORG_ITEM_EX_MAP::iterator IMS_ORG_ITEM_EX_MAP_IT;


// 机构部门信息结构
typedef struct _IMS_DEPART_ITEM
{
	std::string strOrgID;
	std::string strDepartID;
	std::string strDepartName;
	int			nLevelID;
	std::string strUpgradeID;
	int			nOrderID;

	_IMS_DEPART_ITEM()
	{
		strOrgID		= "";
		strDepartID		= "";
		strDepartName	= "";
		nLevelID		= 0;
		strUpgradeID	= "";
		nOrderID		= 0;
	};
}IMS_DEPART_ITEM,*PIMS_DEPART_ITEM;

typedef std::map<std::string,PIMS_DEPART_ITEM> IMS_DEPART_ITEM_MAP;
typedef IMS_DEPART_ITEM_MAP::iterator IMS_DEPART_ITEM_MAP_IT;

// 机构用户信息结构
typedef struct _IMS_ORGUSER_ITEM_EX
{
	std::string			strOrgID;
	std::string			strDepartID;
	IMS_CONTACT_ITEM	item;
	int					nOrderID;

	_IMS_ORGUSER_ITEM_EX()
	{
		strOrgID	= "";
		strDepartID	= "";
		nOrderID	= 0;
	};
}IMS_ORGUSER_ITEM_EX,*PIMS_ORGUSER_ITEM_EX;

typedef std::map<std::string,PIMS_ORGUSER_ITEM_EX> IMS_ORGUSER_ITEM_EX_MAP;
typedef IMS_ORGUSER_ITEM_EX_MAP::iterator IMS_ORGUSER_ITEM_EX_MAP_IT;


//---------------------------------------------------------------------------------------
// 好友组列表
typedef std::map<std::string,std::string> IMS_FRIEND_GROUP_MAP;
typedef IMS_FRIEND_GROUP_MAP::iterator IMS_FRIEND_GROUP_MAP_IT;


//---------------------------------------------------------------------------------------
// 查找好友列表
typedef std::map<std::string,IMS_CONTACT_ITEM> IMS_SEARCH_FRIEND_MAP;
typedef IMS_SEARCH_FRIEND_MAP::iterator IMS_SEARCH_FRIEND_MAP_IT;


//---------------------------------------------------------------------------------------
// URL列表
typedef struct _IMS_URL_ITEM
{
	std::string strName;
	std::string strURL;
	std::string strParams;

	_IMS_URL_ITEM()
	{
		strName		= "";
		strURL		= "";
		strParams	= "";
	};
}IMS_URL_ITEM,*PIMS_URL_ITEM;

typedef std::map<std::string,IMS_URL_ITEM> IMS_URL_ITEM_MAP;
typedef IMS_URL_ITEM_MAP::iterator IMS_URL_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------
// 离线文件列表
typedef struct _NDS_FILE_ITEM
{
	unsigned long	ulFileID;
	std::string		strFileName;
	std::string		strFileDesc;
	unsigned long	ulFileSize;
	std::string		strSendID;
	std::string		strRecvID;
	std::string     strFileMD5;//liangchuan2011/9/20
	_NDS_FILE_ITEM()
	{
		ulFileID		= 0;
		strFileName		= "";
		strFileDesc		= "";
		strSendID		= "";
		strRecvID		= "";
		strFileMD5      = ""; //liangchuan2011/9/20
	};
}NDS_FILE_ITEM,*PNDS_FILE_ITEM;

typedef std::map<unsigned long,NDS_FILE_ITEM> NDS_FILE_ITEM_MAP;
typedef NDS_FILE_ITEM_MAP::iterator NDS_FILE_ITEM_MAP_IT;


//---------------------------------------------------------------------------------------
// 视频分辨率
typedef std::map<int,std::string> VIDEO_SIZE_MAP;
typedef VIDEO_SIZE_MAP::iterator VIDEO_SIZE_MAP_IT;

// 音频设备信息结构
typedef struct _AUDIO_DEV
{
	std::string	strDeviceName;		// 设备名称
	int			nDeviceNo;			// 设备编号
}AUDIO_DEV,*PAUDIO_DEV;

typedef std::map<int,AUDIO_DEV> AUDIO_DEV_MAP;
typedef AUDIO_DEV_MAP::iterator AUDIO_DEV_MAP_IT;

// 视频设备信息结构
typedef struct _VIDEO_DEV
{
	std::string	strDeviceName;		// 设备名称
	int			nDeviceNo;			// 设备编号
	bool		bHDDevice;			// 是否为高清卡
	bool		bHaveSoundCard;		// 是否有声卡
}VIDEO_DEV,*PVIDEO_DEV;

typedef std::map<int,VIDEO_DEV> VIDEO_DEV_MAP;
typedef VIDEO_DEV_MAP::iterator VIDEO_DEV_MAP_IT;

/************************************************************************/
/*音视频设置参数信息													*/
//视频输入设置参数信息
typedef struct _VIDEOSET_INFO
{
	int	nVideoDev;			// 视频设备
	int nVideoCodec;		// 视频编码器
	int	nVideoFrame;		// 图像帧率

	int	nBitrateCtrlType;	// 码流控制
	bool bVideoDenoise;		// 图像去噪
	int	nVideoSize;			// 分辨率
	int	nVideoStream;		// 视频码流


}VIDEOSET_INFO, *PVIDEOSET_INFO;

typedef std::map<int, VIDEOSET_INFO> VIDEOSET_INFO_MAP;
typedef VIDEOSET_INFO_MAP::iterator VIDEOSET_INFO_MAP_IT;

//音视频设置参数信息
typedef struct _AUDIOSET_INFO
{
	int	nSpeakerDev;		// 扬声器

	int nMicDev;			// 麦克风
	int	nAudioInType;		// 音频输入类型
	bool bAutoSetMicVol;	// 自动调整麦克风音量

	int nAudioCodec;		// 音频编码器
	int nFEC;				// 前向纠错
	bool bMicBoost;			// 麦克风增强
	bool bVAD;				// 静音检测
	bool bEchoCancel;		// 回音消除
	bool bLipSync;			// 唇音同步

	int nPostposition;		// 后置自动增益
	int nNoisSuppression;	// 噪音抑制
	_AUDIOSET_INFO()
	{
		nSpeakerDev			= 0;
		nMicDev				= 0;		
		nAudioInType		= 0;	
		nAudioCodec			= 42/*X_AUDIO_CODEC_G7221_14_24*/; 

		bAutoSetMicVol		= 0;
		nFEC				= 0;			
		bMicBoost			= 0;		
		bVAD				= 0;			
		bEchoCancel			= 1;	
		bLipSync			= 0;		
		nPostposition		= 3;	
		nNoisSuppression	= 2;
	}

}AUDIOSET_INFO, *PAUDIOSET_INFO;

typedef std::map<int,AUDIOSET_INFO> AUDIOSET_INFO_MAP;
typedef AUDIOSET_INFO_MAP::iterator AUDIOSET_INFO_MAP_IT;
/************************************************************************/


//---------------------------------------------------------------------------------------
// 会议组列表类型
typedef std::map<std::string,PMMS_GROUP_EX> MMS_GROUP_EX_MAP;		//key:strDomain_strRoomID_strGroupID
typedef MMS_GROUP_EX_MAP::iterator MMS_GROUP_EX_MAP_IT;


//---------------------------------------------------------------------------------------
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


//---------------------------------------------------------------------------------------
//屏幕窗口信息
typedef struct _MMS_WINDOW
{
	int				screenid;		//屏幕号
	int				windowid;		//窗口号
	HWND			hwnd;			//窗口句柄
	std::string		memberid;		//成员ID
	int				cardindex;		//卡ID

	_MMS_WINDOW()
	{
		screenid	= -1;
		windowid	= -1;
		hwnd		= NULL;
		memberid	= "";
		cardindex	= 0;
	};
}MMS_WINDOW,*PMMS_WINDOW;

typedef std::map<std::string,PMMS_WINDOW> MMS_WINDOW_MAP;	//key:screenid_windowid
typedef MMS_WINDOW_MAP::iterator MMS_WINDOW_MAP_IT;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//轮询
typedef struct _ROUND_CHANNEL
{
	int				nChannelType;	//1:成员  2:设备
	int				nChannelIndex;
	std::string		strMemberID;
	std::string		strMemberName;
	std::string		strChannelID;
	std::string		strChannelName;
	int				nCardID;

	_ROUND_CHANNEL()
	{
		nChannelType	= 0;
		nChannelIndex	= 0;
		strMemberID		= "";
		strMemberName	= "";
		strChannelID	= "";
		strChannelName	= "";
		nCardID			= 0;
	}
}ROUND_CHANNEL, *PROUND_CHANNEL;
typedef std::map<int, ROUND_CHANNEL> MAP_ROUND_CHANNEL;
typedef MAP_ROUND_CHANNEL::iterator MAP_ROUND_CHANNEL_IT;

typedef struct _ROUND_GROUP
{
	std::string				strGroupName;
	int						nMode;		//0:滚动模式  1:固定模式
	int						nScreen;
	std::list<int>			lstWin;
	int						nRunWinCnt;
	int						nIntervalTime;
	bool					bIsRecvAudio;
	bool					bIsBroadcast;
	MAP_ROUND_CHANNEL		mapChannel;
	std::string				strResolution;
	int						nGroupStatus;

	_ROUND_GROUP()
	{
		strGroupName	= "";
		nMode			= 1;
		nScreen			= 0;
		nRunWinCnt		= 1;
		nIntervalTime	= 30;
		bIsRecvAudio	= false;
		bIsBroadcast	= false;
		strResolution	= "";
		nGroupStatus	= 0;
	};
}ROUND_GROUP, *PROUND_GROUP;
typedef std::map<int, ROUND_GROUP> MAP_ROUND_GROUP;
typedef MAP_ROUND_GROUP::iterator MAP_ROUND_GROUP_IT;

//////////////////////////////////////////////////////////////////////////
//视频点名
typedef enum _TYPE_CALL_STATUS
{
	NO_CALL			= 0,	//未点名
	CALL_FAILD,				//点名失败
	CALL_SUCCESS,			//点名成功
}TYPE_CALL_STATUS;

typedef struct _CALL_MEMBER
{
	int					nSortIndex;
	CString				strMemberName;
	bool				bOnline;
	TYPE_CALL_STATUS	CallStatus;

	_CALL_MEMBER()
	{
		nSortIndex		= 0;
		strMemberName	= _T("");
		bOnline			= false;
		CallStatus		= NO_CALL;
	}
}CALL_MEMBER, *PCALL_MEMBER;
typedef std::map<std::string, CALL_MEMBER> MAP_CALL_MEMBER;
typedef MAP_CALL_MEMBER::iterator MAP_CALL_MEMBER_IT;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//视屏输出设置
typedef struct _VIDEO_OUTPUT_INFO 
{
	int nScreenID;			//屏幕号
	int nShowType;			//输出显示类型
	int	nVideoSize;			// 视频分辨率	
	int	nVideoFrame;		// 图像帧率
	int	nVideoStream;		// 视频码流
	int nTempleteBandWidth;	//模板总带宽
	int nTMSize;			//模板分辨率
	int nTempleteCutting;	//是否按模板比例切割
	int nWndShowType;		//按比例显示视频
	_VIDEO_OUTPUT_INFO()
	{
		nScreenID = 0;
		nShowType = 0;
		nVideoSize = 0;
		nVideoFrame=0;
		nVideoStream=0;
		nTempleteBandWidth=0;
		nTMSize=0;
		nTempleteCutting=1;
		nWndShowType=0;
	};

}VIDEO_OUTPUT_INFO,*PVIDEO_OUTPUT_INFO;
//////////////////////////////////////////////////////////////////////////
