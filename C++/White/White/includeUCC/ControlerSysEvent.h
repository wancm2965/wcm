#pragma once
#include <string>
using namespace std;
class IControlerSYS
{
public:
	IControlerSYS() {};
	virtual ~IControlerSYS() {};
	virtual void Release() = 0;
	virtual void EnableKeyBorad(BOOL bEnable) = 0;
};

class IControlerSYSEvent
{
public:
	IControlerSYSEvent(){};
	virtual~IControlerSYSEvent(){};
	//帮助
	//virtual void OnControlHelp() = 0;
	//菜单
	virtual void OnControlMenu() = 0;
	//好友
	virtual void OnControlFriend() = 0;
	//进入会议
	virtual void OnControlConference() = 0;
	//视频调节
	virtual void OnControlVideoAdjust() = 0;
	//音频调节
	virtual void OnControlAudioAdjust() = 0;
	//选项
	virtual void OnControlOptions() = 0;
	//拉远
	//virtual void OnControlZoomOut() = 0;
	//拉进
	//virtual void OnControlZoomIn() = 0;
	//网络设置
	virtual void OnControlNetworkSetting() = 0;
    //退出系统
	virtual void OnControlQuitSystem() = 0;
	//呼叫
	virtual void OnCallPhone() = 0;
	//挂断
	virtual void OnShutPhone() = 0;
	//MIC UP
	virtual void OnMicVolumeUp() = 0;
	//MIC DOWN
	virtual void OnMicVolumeDown() = 0;
	//AVCON隐藏/显示
	virtual void OnAvconHideShow() = 0;
	//提示正在PTZ中
	virtual void OnPTZing() = 0;
	//VOL+
	virtual void OnUpVolumeSpeaker() = 0;
	//VOL-
	virtual void OnDownVolumeSpeaker() = 0;
	//静音
	virtual void OnMuteSpeaker() = 0;
};
AFX_EXT_CLASS IControlerSYS* CreateControlSYS(IControlerSYSEvent *pControlerSYSEvent);
