#ifndef __HPDVR_ALARM_H__
#define __HPDVR_ALARM_H__

#include <string>
#include <hash_map>
#include <map>
#include <list>
#include <vector>

// 报警类型
//typedef enum 
//{
//	AT_ALL			=-1,	// 全部
//	AT_DISKFULL		= 0,	// 磁盘空间满
//	AT_VIDEOLOST	= 1,	// 视频丢失
//	AT_MOVEDETECT	= 2,	// 移动帧测
//	AT_ALARM		= 3,	// 主机报警
//	AT_MOVEORALARM	= 4,	// 帧测|主机
//	AT_MOVEANDALARM = 5,	// 帧测&主机
//}ALERT_TYPE;

#define MAX_AT		10

typedef struct tagALARM_CHANNEL { //报警通道
	string		devid;
	string		eid;
	ULONG		cid;
	bool		connect;	//计划用,true处理,false不处理
	int			nvstype;
	bool		state[MAX_AT];
	int			substate[MAX_AT];

	tagALARM_CHANNEL() {
		devid	= "";
		eid		= "";
		cid		= 0;
		connect	= false;
		nvstype = 0;
		memset(state, 0, sizeof(bool)*MAX_AT);
		memset(substate, 0, sizeof(int)*MAX_AT);
	}

	tagALARM_CHANNEL& operator=(const tagALARM_CHANNEL& ac){
		devid   = ac.devid;
		eid	    = ac.eid;
		cid	    = ac.cid;
		connect = ac.connect;
		nvstype = ac.nvstype;
		memcpy(state, ac.state, sizeof(bool)*MAX_AT);
		memcpy(substate, ac.substate, sizeof(int)*MAX_AT);
		return *this; }
}ALARM_CHANNEL, * LPALARM_CHANNEL;
typedef hash_map <string, ALARM_CHANNEL> MAP_ALARM_CHANNEL;
typedef hash_map <string, ALARM_CHANNEL>::iterator	MAP_ALARM_CHANNEL_IT; 

// HPDVR报警管理器通知
class IHPDVRAlarmMgrNotify
{
public:
	IHPDVRAlarmMgrNotify(){};
	virtual ~IHPDVRAlarmMgrNotify(){};

public://回调
	virtual bool OnAlarmNotify_GetChannelInfo(const string&eid, const ULONG cid, CHANNEL_INFO&ci) = 0;

public://事件
	//1上传中心
	virtual void OnAlarmNotify_UploadCenter(const string&eid, const ULONG cid, const int type, const int subtype, const bool alarm) = 0;
	//2触发录像
	virtual void OnAlarmNotify_TriggerRecord(const string&eid, const ULONG cid, const int type, const int subtype, const bool alarm) = 0;
	//3联动云台
	virtual void OnAlarmNotify_LinkageCamera(const string&eid, const ULONG cid, const int presetid) = 0;
	//4报警输出
	virtual void OnAlarmNotify_OutputDevice(const string&eid, const ULONG cid, const int type, const int subtype, const bool alarm) = 0;
};

// HPDVR报警管理器
class IHPDVRAlarmMgr
{
public:
	IHPDVRAlarmMgr(){};
	virtual ~IHPDVRAlarmMgr(){};

public:
	virtual bool Conncet(const string&dvrid) = 0;
	virtual void Release(void) = 0;

public:
	virtual int  GetChannelTotal() = 0;
	virtual bool OpenEncoder(const string&eid) = 0;
	virtual bool CloseEncoder(const string&eid) = 0;
	virtual bool OpenChannel(const string&devid, const string&eid, const ULONG cid, bool connect, int nvstype) = 0;
	virtual bool CloseChannel(const string&eid, const ULONG cid) = 0;
	virtual bool StartDetectIO(const string&eid, const ULONG cid) = 0;
	virtual void StopDetectIO(const string&eid, const ULONG cid) = 0;
	virtual bool IsDetectIO(const string&eid, const ULONG cid) = 0;

public:
	virtual void OnChannelStateChanged(const string&eid, const ULONG cid, bool connect) = 0;
	virtual void OnScheduleStateChanged(bool state) = 0;
	virtual void OnComStateChanged(int comport, int state) = 0;

public:
	virtual bool IsAlarming(const string&eid, const ULONG cid, const int type, const int subtype) = 0;
	virtual bool InputAlarm(const string&eid, const ULONG cid, const int type, const int subtype, const bool alarm) = 0;
	virtual int  GetState(const string&eid, const ULONG cid, bool*pState, int*pSubstate) = 0;
	virtual bool RelieveAlarm(const string&eid, const ULONG cid, const string&userid) = 0;

public:
	virtual bool SetAlarmLinkInfo(const ALARM_LINK_INFO&ali) = 0;
	virtual bool GetAlarmLinkInfo(const string&eid, const ULONG cid, const int iotype, const int presetid, ALARM_LINK_INFO&ali) = 0;
	virtual bool GetAlarmLinkInfoList(const string&eid, const ULONG cid, const int iotype, ALARM_LINK_INFO_LIST&alilst) = 0;

	virtual int GetAlarmInfoList(const string&begin, const string&end, const string&eid, const ULONG cid, const int type, ALARM_INFO_LIST&ail, int page, int pagesize) = 0;
	virtual bool RemoveAlarm(const string &before) = 0;	

	virtual bool SetAlarmPortInfo(const ALARM_PORT_INFO&aii) = 0;
	virtual bool GetAlarmPortInfo(const string&eid, ALARM_PORT_INFO&aii) = 0;

	virtual bool SetAlarmParam(const ALARM_PARAM&ap) = 0;
	virtual bool GetAlarmParam(const string&eid, ALARM_PARAM&ap) = 0;
	virtual bool GetAlarmParam(ALARM_PARAM_LIST&apl) = 0;
	virtual bool IsAlarmUploadCenter(string&eid, ULONG cid) = 0;
};

// HPDVR报警管理接口
AFX_EXT_API IHPDVRAlarmMgr* CreateHPDVRAlarmMgr(IHPDVRAlarmMgrNotify&notify);


// 主机IO报警回调接口
class IHPDVRAlarmIONotify
{
public:
	IHPDVRAlarmIONotify(){};
	virtual ~IHPDVRAlarmIONotify(){};

public:
	virtual void OnIONotify_InputAlarm(const string&eid, const int linkid, const bool alarm) = 0;
};

// 主机IO报警回调接口
class IHPDVRAlarmIO
{
public:
	IHPDVRAlarmIO(){};
	virtual ~IHPDVRAlarmIO(){};

public:
	virtual bool Connect(const string&eid) = 0;
	virtual void Release(void) = 0;

public:
	virtual bool StartIO(void) = 0;
	virtual void StopIO(void) = 0;
	virtual int  GetUartPort(void) = 0;
	virtual int  GetInputPortNum(void) = 0;
	virtual int  GetOutputPortNum(void) = 0;

public:
	virtual bool OutputAlarm(const string&eid, const ULONG cid, const int type, const int subtype, const bool alarm) = 0;
};

#endif//__HPDVR_ALARM_H__
