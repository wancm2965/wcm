#ifndef __HPDVR_ENCODER_H__
#define __HPDVR_ENCODER_H__

// nvs������Ϣ by lgx [5/6/2008]
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
	int type;/*0��ƽ��1������2�󻪣�3����*/
}HIK_HDL;
typedef std::vector<HIK_HDL> VEC_HIK_HDL;
AFX_EXT_API bool GetHikHandle(const char*eid, const long cid, VEC_HIK_HDL&data);
//  [5/6/2008]

// ������������֪ͨ�ӿ�
class IHPDVREncoderMgrNotify
{
protected:
	IHPDVREncoderMgrNotify(){};
	virtual ~IHPDVREncoderMgrNotify(){};

public: // �ص�
public: // �¼�
	virtual void OnNotifyDevStateChanged(const string&eid, const ULONG cid, int nvstype, int state) = 0;
	virtual void OnNotifyInputAlarmStarted(const string&eid, const ULONG cid, int nvstype, int type, int subtype) = 0;
	virtual void OnNotifyInputAlarmStopped(const string&eid, const ULONG cid, int nvstype, int type, int subtype) = 0;
	virtual void OnNotifyAudioPacketData(const string&eid, const ULONG cid, char*pData, int nLen, bool bKeyFrame) = 0;
	virtual void OnNotifyVideoPacketData(const string&eid, const ULONG cid, char*pData, int nLen, bool bKeyFrame) = 0;
	virtual void OnNotifyComStateChanged(int comport, int state) = 0;
	virtual void OnNotifyInterphoneChanged(string& eid, string& fromid, int state) = 0;
	virtual void OnNotifyAVSenderConnected(string codid, int chnid, unsigned long ulIP,int nAudType,int nVidType) = 0;
	virtual void OnNotifyAVSenderRefreshMCU(void) = 0;// +MCUƯ�� for PICC[SW-AV6-EV-09020901] [2/9/2009 lgx]
};

// �������ӿ�
class IHPDVREncoderMgr
{
public:
	IHPDVREncoderMgr(){};
	virtual ~IHPDVREncoderMgr(){};

public: 
	virtual bool Connect() = 0;
	virtual bool IsConnected() = 0;
	virtual void ReleaseConnections() = 0;

public: // ������
	virtual int GetEncoderTotal() = 0;
	virtual bool ConnectEncoder(ENCODER_INFO&einfo) = 0;
	virtual void DisconnectEncoder(const string&eid) = 0;
	virtual bool IsReconnectEncoder(const string&eid) = 0;
	virtual bool RebootEncoder(const string&eid) = 0;
	virtual bool ClickSoftKey(string&eid, int key, int input) = 0; // + ����� for ������ʯ�ͼ��ϵͳ�������ؿ�����������¼����� by lgx [4/21/2008]

public:	// ͨ��
	// ��ͨ��
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

	// ʵʱ��Ԥ��
	virtual bool RealPlay(string&eid, unsigned long cid) = 0;
	virtual void StopPlay(string&eid, unsigned long cid) = 0;

	// �Խ�
	virtual HWND OpenInterphone(HWND hWnd, const AUDIO_INFO&ainfo, const VIDEO_INFO&vinfo) = 0;
	virtual void CloseInterphone(const string&eid) = 0;
	virtual bool IsInterphoneBusy(string&localid, string&remoteid) = 0;
	virtual bool StartReceiveInterphone(HWND hWnd, const MCU_INFO&minfo, const string&fromid) = 0;
	virtual void StopReceiveInterphone(const string&eid, const string&fromid) = 0;

	virtual void DataRecoderStatusChanged(string&codid, int chnid, int state) = 0;

	// ��̨����
	virtual bool ControlCamera(const string&eid, unsigned long cid, DWORD dwPTZCommand) = 0;
	virtual bool SetControlCamera(const PTZ_INFO&pinfo) = 0;
	virtual bool GetControlCamera(PTZ_INFO&pinfo) = 0;

	// �ƶ�֡��
	virtual void InitMotionDetect(const string&eid, unsigned long cid, DWORD dwGrade/*�����ȵȼ�0-6*/, RECT*pRectList/*��������б�*/, int nRectCount/*���������*/, bool bStarted)=0;
	virtual bool StartMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual void StopMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual int IsMotionDetect(const string&eid, unsigned long cid) = 0;
	virtual bool SetMotionDetect(const string&eid, unsigned long cid, DWORD dwGrade/*�����ȵȼ�0-6*/, RECT*pRectList/*��������б�*/, int nRectCount/*���������*/) = 0;
	virtual bool GetMotionDetect(const string&eid, unsigned long cid, DWORD&dwGrade/*�����ȵȼ�0-6*/, RECT*pRectList/*��������б�*/, int&nRectCount/*���������*/) = 0;

	// ��Ƶ�ڵ�
	virtual bool StartVideoMask(const string&eid, unsigned long cid) = 0;
	virtual void StopVideoMask(const string&eid, unsigned long cid) = 0;
	virtual int IsVideoMask(const string&eid, unsigned long cid) = 0;
	virtual bool SetVideoMask(const string&eid, unsigned long cid, RECT*pRectList/*�ڵ������б�*/, int nRectCount/*�ڵ�������*/) = 0;
	virtual bool GetVideoMask(const string&eid, unsigned long cid, RECT*pRectList/*�ڵ������б�*/, int&nRectCount/*�ڵ�������*/) = 0;

	// ��Ƶ��ʧ
	virtual bool StartDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual void StopDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual int IsDetectVideoLost(const string&eid, unsigned long cid) = 0;
	virtual bool SetDetectVideoLost(const string&eid, unsigned long cid, bool check) = 0;
	virtual bool GetDetectVideoLost(const string&eid, unsigned long cid, bool&check) = 0;

	// ֹͣ����
	virtual bool ManualStopAlarm(const string&eid, unsigned long cid) = 0;
	// �ؼ�֡
	virtual bool NeedVideoKeyFrame(const string&eid, unsigned long cid) = 0;
	// ץ��
	virtual bool Capture2JPG(const string&eid, unsigned long cid, const char*lpszJPGFile) = 0;
	// ��Ƶ����
	virtual bool SetVideoInput(const string&eid, const ULONG cid) = 0;
	// ��Ƶ����[9/4/2008 lzy]
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

	// �л�����ģʽ
	virtual bool SwitchStreamMode(const string&eid, unsigned long cid, int mode) = 0;
};

// ������ʵ��
AFX_EXT_API IHPDVREncoderMgr* CreateHPDVREncoderMgr(IHPDVREncoderMgrNotify&rNotify);
#endif//__HPDVR_ENCODER_H__
