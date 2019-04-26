#ifndef __HPDVR_RECORD_H__
#define __HPDVR_RECORD_H__

#include <string>
#include <hash_map>
#include <map>
#include <list>
#include <vector>

//// 录像类型
//typedef enum 
//{
//	RT_ALL			=-1,	// 全部
//	RT_SCHEDULE		= 0,	// 定时录像
//	RT_MANUAL		= 1,	// 手动录像
//	RT_MOVEDETECT	= 2,	// 移动帧测
//	RT_ALARM		= 3,	// 报警录像
//	RT_MOVEORALARM	= 4,	// 动测|报警
//	RT_MOVEANDALARM	= 5,	// 动测&报警
//}RECORD_TYPE;

#define MAX_RT		10

typedef struct tagRECORD_CHANNEL { //录像通道
	string		devid;
	string		eid;
	ULONG		cid;
    bool		connect;
	int			nvstype;
	bool		upfrontstream[MAX_RT-1];
	int			upfronttime[MAX_RT-1];
	bool		syncaudio;
	bool		state[MAX_RT];
	int			substate[MAX_RT];
	DWORD_PTR	ptr[MAX_RT];

	tagRECORD_CHANNEL() {
		devid	= "";
		eid		= "";
		cid		= 0;
		connect	= false;
		nvstype = 0;
		syncaudio = false;
		memset(upfrontstream, 0, sizeof(bool)*(MAX_RT-1));
		memset(upfronttime, 0, sizeof(int)*(MAX_RT-1));
		memset(state, 0, sizeof(bool)*MAX_RT);
		memset(substate, 0, sizeof(int)*MAX_RT);
		memset(ptr, 0, sizeof(DWORD_PTR)*MAX_RT);
	}
	tagRECORD_CHANNEL& operator=(const tagRECORD_CHANNEL& rc){
		devid   = rc.devid;
		eid	    = rc.eid;
		cid	    = rc.cid;
		connect = rc.connect;
		nvstype = rc.nvstype;
		syncaudio = rc.syncaudio;
		memcpy(upfrontstream, rc.upfrontstream, sizeof(bool)*(MAX_RT-1));
		memcpy(upfronttime, rc.upfronttime, sizeof(int)*(MAX_RT-1));
		memcpy(state, rc.state, sizeof(bool)*MAX_RT);
		memcpy(substate, rc.substate, sizeof(int)*MAX_RT);
		memcpy(ptr, rc.ptr, sizeof(DWORD_PTR)*MAX_RT);
		return *this; }
}RECORD_CHANNEL, * LPRECORD_CHANNEL;
typedef hash_map <string, RECORD_CHANNEL> MAP_RECORD_CHANNEL;
typedef hash_map <string, RECORD_CHANNEL>::iterator MAP_RECORD_CHANNEL_IT; 

// HPDVR录像管理器通知
class IHPDVRRecordMgrNotify
{
public:
	IHPDVRRecordMgrNotify(){};
	virtual ~IHPDVRRecordMgrNotify(){};

public://回调
	virtual bool OnRecordNotify_GetChannelInfo(const string&eid, const ULONG cid, CHANNEL_INFO&ci) = 0;
	virtual string OnRecordNotify_GetDevId(const string&eid) = 0;

public://事件
	virtual void OnRecordNotify_RecordStateChanged(const string&eid, const ULONG cid, const int type, const bool state, const int substate) = 0;
	virtual void OnRecordNotify_RecordDiskFull(const int type, const int subtype) = 0;
	virtual void OnRecordNotify_AlarmDevice(const string&eid, const ULONG cid, const int type, const bool start) = 0;
	virtual void OnRecordNotify_ScheduleStateChanged(bool state) = 0;
};

// HPDVR录像管理器
class IHPDVRRecordMgr
{
public:
	IHPDVRRecordMgr(){};
	virtual ~IHPDVRRecordMgr(){};

public:
	virtual bool Conncet() = 0;
	virtual void Release() = 0;

public:
	virtual bool StartServer() = 0;
	virtual void StopServer() = 0;
	virtual bool IsExecSchema() = 0;
	virtual bool ExecSchema(bool bExec) = 0;

public:
	virtual int  GetChannelTotal() = 0;
	virtual bool OpenChannel(const string&devid, const string&eid, const ULONG cid, bool connect, int nvstype) = 0;
	virtual bool CloseChannel(const string&eid, const ULONG cid) = 0;

public:
	virtual void OnChannelStateChanged(const string&eid, const ULONG cid, bool connect) = 0;
	virtual void OnRecordParamChanged(int type) = 0;
    
public:
	virtual bool Alarm2Record(const string&eid, const ULONG cid, const int type, const int subtype, bool bRecord) = 0;
	virtual bool Manual2Record(const string&eid, const ULONG cid, const int type, const int subtype, bool bRecord) = 0;
	virtual bool IsRecording(const string&eid, const ULONG cid, const int type, const int subtype) = 0;
	virtual int  GetState(const string&eid, const ULONG cid, bool*pState, int*pSubstate) = 0;

	virtual void InputAudioPacket(const string&eid, const ULONG cid, const void*pData, const int nLen, const bool bKeyFrame) = 0;
	virtual void InputVideoPacket(const string&eid, const ULONG cid, const void*pData, const int nLen, const bool bKeyFrame) = 0;

	virtual void OnDispatchUploadInfo(const UINT port, const string&ipaddr0, const string&ipaddr1, const string&ipaddr2) = 0;
	virtual void OnUploadFlagChanged(bool bUpload) = 0;
};



// HPDVR录像管理接口
AFX_EXT_API IHPDVRRecordMgr* CreateHPDVRRecordMgr(IHPDVRRecordMgrNotify&notify);

//////////////////////////////////////////////////////////////////////////
//录像配置SDK
//////////////////////////////////////////////////////////////////////////
//{{录像方式
//文件自动分割
AFX_EXT_API bool	RecGetAutoComminute(int type);
AFX_EXT_API void	RecSetAutoComminute(int type, bool value);
//自动预录像
AFX_EXT_API bool	RecGetAutoPrepRecord(int type);
AFX_EXT_API void	RecSetAutoPrepRecord(int type, bool value);
AFX_EXT_API int		RecGetPrepRecordTime(int type);
AFX_EXT_API void	RecSetPrepRecordTime(int type, int value);
//自动执行计划
AFX_EXT_API bool	RecGetAutoExecScheme();
AFX_EXT_API void	RecSetAutoExecScheme(bool value);
//计划通道
AFX_EXT_API string	RecGetSchemeChannels();	
AFX_EXT_API void	RecSetSchemeChannels(const string&value);
//录像目录
AFX_EXT_API string	RecGetRecordFolders();
AFX_EXT_API void	RecSetRecordFolders(const string&value);
//录像方式}}


//{{循环方式
AFX_EXT_API bool	RecGetCycleMode();	
AFX_EXT_API void	RecSetCycleMode(bool value);	
//循环
AFX_EXT_API int		RecGetCoverDays();
AFX_EXT_API void	RecSetCoverDays(int value);
//非循环
AFX_EXT_API bool	RecGetAcylicAlert();
AFX_EXT_API void	RecSetAcylicAlert(bool value);
AFX_EXT_API int		RecGetAcylicAlertMode();	
AFX_EXT_API void	RecSetAcylicAlertMode(int value);	
//循环录像}}

//{{文件分割
AFX_EXT_API int		RecGetComminuteMode();	
AFX_EXT_API void	RecSetComminuteMode(int value);	
AFX_EXT_API int		RecGetComminuteTime();	
AFX_EXT_API void	RecSetComminuteTime(int value);	
AFX_EXT_API int		RecGetComminuteSize();	
AFX_EXT_API void	RecSetComminuteSize(int value);	
//文件分割}}

//{{存储方式
AFX_EXT_API string	RecGetRecordDrivers();	
AFX_EXT_API void	RecSetRecordDrivers(const string&value);	
AFX_EXT_API char	RecGetCurrentDriver();
AFX_EXT_API void	RecSetCurrentDriver(char value);
AFX_EXT_API bool	RecGetAutoNextDriver();
AFX_EXT_API void	RecSetAutoNextDriver(bool value);
AFX_EXT_API int		RecGetReservedSpace();
AFX_EXT_API void	RecSetReservedSpace(int value);
//}}

//{{计划录像
AFX_EXT_API bool	RecGetFullDayRecord(const string&eid, unsigned long cid, int nWeek);
AFX_EXT_API void	RecSetFullDayRecord(const string&eid, unsigned long cid, int nWeek, bool bSupport);
AFX_EXT_API int		RecGetFullDayRecordType(const string&eid, unsigned long cid, int nWeek);
AFX_EXT_API void	RecSetFullDayRecordType(const string&eid, unsigned long cid, int nWeek, int nType);
AFX_EXT_API int		RecGetRecordTimeRangNumber(const string&eid, unsigned long cid, int nWeek);
AFX_EXT_API void	RecSetRecordTimeRangNumber(const string&eid, unsigned long cid, int nWeek, int nNumber);
AFX_EXT_API string	RecGetRecordTimeRang(const string&eid, unsigned long cid, int nWeek, int nIndex);
AFX_EXT_API void	RecSetRecordTimeRang(const string&eid, unsigned long cid, int nWeek, int nIndex, const string&lpszTimeRang);
AFX_EXT_API int		RecGetRecordType(const string&eid, unsigned long cid, int nWeek, int nIndex);
AFX_EXT_API void	RecSetRecordType(const string&eid, unsigned long cid, int nWeek, int nIndex, int nType);
//计划录像}}

//{{录像文件名和目录
AFX_EXT_API int		RecGetRecSaveType(int nSubRecType);
AFX_EXT_API string	RecGetRecSaveFolder(int nSubRecType);
AFX_EXT_API string	RecGetRecDevType(int nNVSType);
AFX_EXT_API string	RecGetManufacturer(int nNVSType);
//}}

//是否采集Mic的声音
AFX_EXT_API bool	RecIsCaptureAudioUseMic(const string&eid, const ULONG cid);

//下载录像信息
AFX_EXT_API bool	RecGetRecordFileInfoList(RECFILE_INFO&rinfo, RECFILE_INFO&rinfo_last, RECFILE_INFO_LIST&rinfolst, int page = 0, int pagesize = -1);
AFX_EXT_API void	RecRemoveRecordFileInfo(const char*before);
AFX_EXT_API void	RecRemoveEncoderData(const char*eid);

// lzy [7/31/2008 ]
AFX_EXT_API int 	GetCurrentPageTotal();

//上传录像
AFX_EXT_API bool	RecGetUploadInfo(UPLOAD_INFO&uinfo);
//////////////////////////////////////////////////////////////////////////

#endif//__HPDVR_RECORD_H__
