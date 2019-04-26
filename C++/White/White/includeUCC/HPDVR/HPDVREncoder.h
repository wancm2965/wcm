#ifndef __HPDVR_ENCODER_H__
#define __HPDVR_ENCODER_H__

// nvs链接信息 by lgx [5/6/2008]
#include <vector>
typedef struct _HIK_HDL
{
	string eid;
	long cid;
	long handle;
	long channel;
	string ip;
	long port;
	string user;
	string pwd;
	int type;/*0华平，1海康，2大华，3南自*/
}HIK_HDL;
typedef std::vector<HIK_HDL> VEC_HIK_HDL;
AFX_EXT_API bool GetHikHandle(const char*eid, const long cid, VEC_HIK_HDL&data);
//  [5/6/2008]

// 编码器管理器通知接口
class IHPDVREncoderMgrNotify
{
protected:
	IHPDVREncoderMgrNotify(){};
	virtual ~IHPDVREncoderMgrNotify(){};

public: // 回调
public: // 事件
	virtual void OnNotifyDevStateChanged(const string&eid, const ULONG cid, int nvstype, int state) = 0;
	virtual void OnNotifyInputAlarmStarted(const string&eid, const ULONG cid, int nvstype, int type, int subtype) = 0;
	virtual void OnNotifyInputAlarmStopped(const string&eid, const ULONG cid, int nvstype, int type, int subtype) = 0;
	virtual void OnNotifyAudioPacketData(const string&eid, const ULONG cid, char*pData, int nLen, bool bKeyFrame) = 0;
	virtual void OnNotifyVideoPacketData(const string&eid, const ULONG cid, char*pData, int nLen, bool bKeyFrame) = 0;
	virtual void OnNotifyComStateChanged(int comport, int state) = 0;
	virtual void OnNotifyInterphoneChanged(string& eid, string& fromid, int state) = 0;
	virtual void OnNotifyAVSenderConnected(string codid, int chnid, unsigned long ulIP,int nAudType,int nVidType) = 0;
	virtual void OnNotifyAVSenderRefreshMCU(void) = 0;// +MCU漂移 for PICC[SW-AV6-EV-09020901] [2/9/2009 lgx]
};

// 编码器接口
class IHPDVREncoderMgr
{
public:
	IHPDVREncoderMgr(){};
	virtual ~IHPDVREncoderMgr(){};

public: 
	virtual bool Connect() = 0;
	virtual bool IsConnected() = 0;
	virtual void ReleaseConnections() = 0;

public: // 编码器
	virtual int GetEncoderTotal() = 0;
	virtual bool ConnectEncoder(ENCODER_INFO&einfo) = 0;
	virtual void DisconnectEncoder(const string&eid) = 0;
	virtual bool IsReconnectEncoder(const string&eid) = 0;
	virtual bool RebootEncoder(const string&eid) = 0;
	virtual bool ClickSoftKey(string&eid, int key, int input) = 0; // + 软键盘 for 华南中石油监控系统整合网关开发软件交办事件处理单 by lgx [4/21/2008]

public:	// 通道
	// 打开通道
	virtual HWND OpenChannel(HWND hWnd, VIDEO_INFO&vinfo, AUDIO_INFO&ainfo) = 0;
	virtual void CloseChannel(const string&eid, unsigned long cid) = 0;
	virtual int GetChannelTotal(const string&eid) = 0;
	virtual bool SetChannelInfo(VIDEO_INFO&vinfo, AUDIO_INFO&ainfo) = 0;
	virtual bool GetChannelInfo(VIDEO_INFO&vinfo, AUDIO_INFO&ainfo) = 0;
	virtual bool SetVideoParam(VIDEO_PARAM&vparam) = 0;
	virtual bool GetVideoParam(VIDEO_PARAM&vparam)=0;
	virtual bool GetAudioSenderInfo(string& codid, int chnid, unsigned long&id, int&type, string&ip) = 0;
	virtual bool GetVideoSenderInfo(string& codid, int chnid, unsigned long&id, int&type, string&ip) = 0;
	virtual bool GetSubVideoSenderInfo(string& codid, int chnid, unsigned long&id, int&type, string&ip) = 0;

	// 实时流预览
	virtual bool RealPlay(string&eid, unsigned long cid) = 0;
	virtual void StopPlay(string&eid, unsigned long cid) = 0;

	// 对讲
	virtual HWND OpenInterphone(HWND hWnd, const AUDIO_INFO&ainfo, const VIDEO_INFO&vinfo) = 0;
	virtual void CloseInterphone(const string&eid) = 0;
	virtual bool IsInterphoneBusy(string&localid, string&remoteid) = 0;
	virtual bool StartReceiveInterphone(HWND hWnd, const MCU_INFO&minfo, const string&fromid) = 0;
	virtual void StopReceiveInterphone(const string&eid, const string&fromid) = 0;

	virtual void DataRecoderStatusChanged(string&codid, int chnid, int state) = 0;

	// 云台控制
	virtual bool ControlCamera(const string&eid, unsigned long cid, DWORD dwPTZCommand) = 0;
	virtual bool SetControlCamera(const PTZ_INFO&pinfo) = 0;
	virtual bool GetControlCamera(PTZ_INFO&pinfo) = 0;

	// 移动帧测
	virtual void InitMotionDetect(const string&eid, unsigned long cid, DWORD dwGrade/*灵敏度等级0-6*/, RECT*pRectList/*侦测区域列表*/, int nRectCount/*侦测区域数*/, bool bStarted)=0;
	virtual bool StartMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual void StopMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual int IsMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual bool SetMotionDetect(const string&eid, unsigned long cid, DWORD dwGrade/*灵敏度等级0-6*/, RECT*pRectList/*侦测区域列表*/, int nRectCount/*侦测区域数*/) = 0;
	virtual bool GetMotionDetect(const string&eid, unsigned long cid, DWORD&dwGrade/*灵敏度等级0-6*/, RECT*pRectList/*侦测区域列表*/, int&nRectCount/*侦测区域数*/) = 0;

	// 视频遮挡
	virtual bool StartVideoMask(const string&eid, unsigned long cid) = 0;
	virtual void StopVideoMask(const string&eid, unsigned long cid) = 0;
	virtual int IsVideoMask(const string&eid, unsigned long cid) = 0;
	virtual bool SetVideoMask(const string&eid, unsigned long cid, RECT*pRectList/*遮挡区域列表*/, int nRectCount/*遮挡区域数*/) = 0;
	virtual bool GetVideoMask(const string&eid, unsigned long cid, RECT*pRectList/*遮挡区域列表*/, int&nRectCount/*遮挡区域数*/) = 0;

	// 视频丢失
	virtual bool StartDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual void StopDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual int IsDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual bool SetDetectVideoLost(const string&eid, unsigned long cid, bool check) = 0;
	virtual bool GetDetectVideoLost(const string&eid, unsigned long cid, bool&check) = 0;

	// 停止报警
	virtual bool ManualStopAlarm(const string&eid, unsigned long cid) = 0;
	// 关键帧
	virtual bool NeedVideoKeyFrame(const string&eid, unsigned long cid) = 0;
	// 抓拍
	virtual bool Capture2JPG(const string&eid, unsigned long cid, const char*lpszJPGFile) = 0;
	// 视频输入
	virtual bool SetVideoInput(const string&eid, const ULONG cid) = 0;
	// 视频调节[9/4/2008 lzy]
	virtual bool SetVideoAdjust(const string&eid, const ULONG cid,VIDEO_PARAM& video_param) = 0;
	

	// MCU
	virtual bool GetChannelMCU(MCU_INFO&minfo) = 0;
	virtual bool OnAVCON_Dispatch_MCU(string&eid, string&mcuid, string&mcuaddr, unsigned short mcuport, string&mcutype) = 0;
	virtual bool OnAVCON_Dispatch_CHN_Count(string&eid, int issue_num) = 0;

	// TCP UDP
	virtual int GetAudioTransType(string&codid, int chnid) = 0;
	virtual void SetAudioTransType(string&codid, int chnid, int type) = 0;

	virtual int GetVideoTransType(string&codid, int chnid) = 0;

	virtual void SetVideoTransType(string&codid, int chnid, int type) = 0;

	// 切换接流模式
	virtual bool SwitchStreamMode(const string&eid, unsigned long cid, int mode) = 0;
};

// 编码器实例
AFX_EXT_API IHPDVREncoderMgr* CreateHPDVREncoderMgr(IHPDVREncoderMgrNotify&rNotify);
#endif//__HPDVR_ENCODER_H__
