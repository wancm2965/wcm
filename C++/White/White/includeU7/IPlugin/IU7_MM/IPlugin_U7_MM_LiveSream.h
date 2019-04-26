// UCC7-MM服务器录播功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	typedef enum emStatusType
	{
		ON_LIVE_NONE = -1,
		ON_LIVE_START = 0, //开始直播的回调
		ON_LIVE_STOP,
		ON_LIVE_CHNG_STREAM, //直播流改变 的回调（高、中、低）
	} StatusType;

	typedef enum emStreamType
	{
		STREM_HIGH = 0, //高流
		STREM_MID , 
		STREM_LOW,
	} StreamType;

	typedef struct _stLiveParam
	{
		std::string strMemid;
		int nCardid;
		StreamType streamType;
		_stLiveParam()
		{
			strMemid = "";
			nCardid = -1;
			streamType = STREM_HIGH;
		}
	}stLiveParam;

	typedef struct _StuctOnLiveStatus
	{
		std::string strMemid;
		int nCardid;
		
		StatusType stusType;
		emStreamType stremType;
		unsigned int uErr; //错误码

		_StuctOnLiveStatus()
		{
			strMemid = "";
			nCardid = -1;
			stusType = ON_LIVE_NONE;
			stremType = STREM_HIGH;
			uErr = 0;
		}
		
		_StuctOnLiveStatus& operator = (const _StuctOnLiveStatus &from)
		{
			strMemid = from.strMemid;
			nCardid = from.nCardid;
			stusType = from.stusType;
			stremType = from.stremType;
			uErr = from.uErr;

			return *this;
		}
	}StuctOnLiveStatus;	

	typedef struct _StructLiveData
	{
		std::string strMemid;
		std::string strliveUrl;
		int nCardid;
		emStreamType stremType;
		_StructLiveData ()
		{
			strMemid = "";
			nCardid = -1;
			strliveUrl = "";
			stremType = STREM_HIGH;
		}
	} stuLiveData;

	class IPlugin_U7_MM_LiveSream_Observer
	{
	public:
		virtual ~IPlugin_U7_MM_LiveSream_Observer(){};

	public:
		virtual void OnLiveStatus(const StuctOnLiveStatus &status) = 0;
	};

	class IPlugin_U7_MM_LiveSream : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_MM_LiveSream(){};

	public:
		virtual void AddObserver(IPlugin_U7_MM_LiveSream_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_MM_LiveSream_Observer* pObserver) = 0;

	public:
		//开始直播 strLiveUrl：开始直播成功后返回的直播拉流地址
		virtual bool StartLive(const stLiveParam &liveparam) = 0;
		//停止直播
		virtual bool StopLive(const std::string &strMemid, int nCardIndex) = 0;

		//直播 流切换(高中低)
		virtual bool ChangeLiveStreamType(const stLiveParam &liveparam)=0;
		virtual void GetAllLiveData(std::list<stuLiveData> &allLiveData)=0;
	};
}
