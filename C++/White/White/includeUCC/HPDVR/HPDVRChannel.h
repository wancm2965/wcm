#pragma once

#include <map>
#include <string>
using namespace std;


class IHPDVRChannelCallBack
{
public:
	IHPDVRChannelCallBack(){};
	virtual ~IHPDVRChannelCallBack(){};
public:
	virtual void OnSerialChanged(int comid, int state) = 0;
	virtual void OnChannelChanged(string& codid, int chnid, int state) = 0;
	virtual void OnAlarmDelivered(string& codid, int chnid, int type, int state, int detail) = 0;
	virtual void OnAudioDelivered(string& codid, int chnid, char*data, int len, bool key) = 0;
	virtual void OnVideoDelivered(string& codid, int chnid, char*data, int len, bool key) = 0;
	virtual void OnVideoDeliveredSub(string& codid, int chnid, char*data, int len, bool key) = 0;
	virtual void OnInterphoneChanged(string& codid, string& fromid, int state) = 0;
};

class IHPDVRChannel
{ 
public:
	IHPDVRChannel(){};
	virtual ~IHPDVRChannel(){};
public:
	virtual bool Connect() = 0;
	virtual void Release() = 0;
	virtual bool IsConnected() = 0;

	virtual string& GetCoderId() = 0;
	virtual int GetChannelId() = 0;
	virtual int GetRealChnId() = 0;
	virtual long GetHwndId() = 0;

public:
	// 通道
	virtual long OpenChannel(HD_CHN_INFO& param) = 0;
	virtual void CloseChannel() = 0;
	virtual bool RealPlay() = 0;
	virtual void StopPlay() = 0;
	virtual void InterphoneDataRecvStarted(string&fromid) =0;
	virtual void InterphoneDataRecvStopped(string&fromid) =0;
	virtual bool InterphoneDataDelivered(char*data, int len) = 0;
	virtual void DataSenderStatusChanged(int state) = 0; 
	virtual void DataRecoderStatusChanged(int state) = 0;

	// 通道参数
	virtual bool SetChannelParam(HD_CHN_AVINFO& param) = 0;
	virtual bool GetChannelParam(HD_CHN_AVINFO& param) = 0;
	
	//  [9/4/2008 lzy]
	virtual void SetVideoAdjust(HD_CHN_VIDEO& param)=0;
	virtual void ShowVideoInput(void) = 0;// fixed for 中科院 [3/26/2009 lgx]

	// 视频帧测
	virtual void InitVideoMotion(bool bStarted, HD_VIDEO_MOTION& param)=0;
	virtual bool StartVideoMotion() = 0;
	virtual void StopVideoMotion() = 0;
	virtual long IsVideoMotion() = 0; 
	virtual bool SetVideoMotion(HD_VIDEO_MOTION& param) = 0;
	virtual bool GetVideoMotion(HD_VIDEO_MOTION& param) = 0;

	// 视频遮掩
	virtual bool StartVideoCover() = 0;
	virtual void StopVideoCover() = 0;
	virtual long IsVideoCover() = 0;
	virtual bool SetVideoCover(HD_VIDEO_COVER& param) = 0;
	virtual bool GetVideoCover(HD_VIDEO_COVER& param) = 0;

	// 视频遮挡
	virtual bool StartVideoBlock() = 0;
	virtual void StopVideoBlock() = 0;
	virtual long IsVideoBlock() = 0;
	virtual bool SetVideoBlock(HD_VIDEO_BLOCK& param) = 0;
	virtual bool GetVideoBlock(HD_VIDEO_BLOCK& param) = 0;

	// 视频丢失
	virtual bool StartVideoLost() = 0;
	virtual void StopVideoLost() = 0;
	virtual long IsVideoLost() = 0;
	virtual bool SetVideoLost(int defend) = 0;
	virtual bool GetVideoLost(int& defend) = 0;

	// 云台控制
	virtual bool PTZControl(int ptzcmd) = 0;
	virtual bool SetPTZParam(HD_CHN_PTZ& param) = 0;
	virtual bool GetPTZParam(HD_CHN_PTZ& param) = 0;

	// 其他
	virtual bool CaptureKeyFrame(bool bSub) = 0;
	virtual bool CapturePictures(const char*file, int step, int num) = 0;
	virtual bool SwitchStream(int index) = 0;
};

typedef map<int, IHPDVRChannel*> MAP_CHANNEL;
typedef map<int, IHPDVRChannel*>::iterator MAP_CHANNEL_IT;
typedef map <string, MAP_CHANNEL> MAP_COD_CHN;
typedef map <string, MAP_CHANNEL>::iterator	MAP_COD_CHN_IT;

