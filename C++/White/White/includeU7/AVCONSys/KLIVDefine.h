#pragma once
//#include "HPRecordDefine.h"

#define TERM_TYPE_LIVSVR	"live"

//直播流方式;
typedef enum _STREAM_RTMPLIVE_TYPE
{		
	STREAM_RTMPLIVE_TYPE_ALL			= 1,	//直播音视频;
	STREAM_RTMPLIVE_TYPE_VIDEO			= 2,	//直播视频;
	STREAM_RTMPLIVE_TYPE_AUDIO			= 3,	//直播音频;
}STREAM_RTMPLIVE_TYPE;

//直播通道信息
typedef struct _tagLIVEINFO
{
	_tagLIVEINFO()
	{
		roomid			= "";
		roomname		= "";
		devid			= "";
		channelid		= "";
		channelname		= "";
		liveuserid		= "";
		devinfo			= "";
		nodeid			= "";
		mcuid       	= "";
		mcuaddr			= "";
		nataddr			= "";
		localaddr		= "";
		termtype		= "";
		mcuport			= 0;
		localport		= 0;
		videocid		= 0;
		audiocid		= 0;
		livename		= "";
		streamtype  	= 0;
		eLiveType		= STREAM_RTMPLIVE_TYPE_ALL;
		pullurl			= "";
		pushurl			= "";
		startdatetime	= "";
		enddatetime		= "";
	}
	_tagLIVEINFO& operator=(const _tagLIVEINFO& mtn)
	{
		roomid			= mtn.roomid;
		roomname		= mtn.roomname;
		devid			= mtn.devid;
		channelid		= mtn.channelid;
		channelname		= mtn.channelname;
		liveuserid		= mtn.liveuserid;
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
		livename		= mtn.livename;
		streamtype		= mtn.streamtype;
		eLiveType		= mtn.eLiveType;
		pullurl			= mtn.pullurl;
		pushurl			= mtn.pushurl;
		startdatetime	= mtn.startdatetime;
		enddatetime		= mtn.enddatetime;

		return *this;
	}
	std::string				roomid;				//会议室ID
	std::string				roomname;			//会议室名称
	std::string				devid;				//被直播的设备号（或人员ID）
	std::string				channelid;			//被直播的通道号
	std::string				channelname;		//对直播通道的别名
	std::string				liveuserid;			//发起直播的人员ID
	std::string				devinfo;			//设备信息
	std::string				nodeid;				
	std::string				mcuid;
	std::string				mcuaddr;
	unsigned short			mcuport;
	std::string				nataddr;
	std::string				localaddr;
	unsigned short			localport;
	std::string				termtype;
	unsigned long			videocid;
	unsigned long			audiocid;
	std::string				livename;			//直播名称（可不设置）
	unsigned short			streamtype;			//直播码流，0：高码流 1：中码流 2：低码流
	STREAM_RTMPLIVE_TYPE	eLiveType;			//1：直播音视频 2: 直播视频 3：直播音频
	std::string				pullurl;			//直播拉流地址（开始直播成功后生成）
	std::string				pushurl;			//直播推流地址（目前不对客户端开放）
	std::string				startdatetime;		//直播开始时间
	std::string				enddatetime;		//直播结束时间

}LIVEINFO,*LPLIVEINFO;

typedef struct tag_LIVREMOTE
{
	std::string			serverid;
	std::string			serverip;
	std::string			servernatip;
	std::string			storageaddr;
	unsigned int		policystorage;
	unsigned int		policytime;
	unsigned int		policysize;
	unsigned short		status;
	unsigned int		userport;
	unsigned int		licport;
	//unsigned long		freespace;
	//unsigned long		totalspace;

	tag_LIVREMOTE()
	{
		serverid = "";
		serverip = "";
		servernatip = "";
		storageaddr = "";
		policystorage = 0;
		policytime = 60;
		policysize = 500;
		status = 0;
		userport = 0;
		licport = 0;
		//freespace = 0;
		//totalspace = 0;
	}
}LIVREMOTE, *PLIVREMOTE;
