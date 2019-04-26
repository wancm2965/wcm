



#ifndef _CONTROLERCONFERENCEEVENT_H 
#define _CONTROLERCONFERENCEEVENT_H 

#include <string>
using namespace std;
//#include "../CONTROLERHOOK/ontrolerConferenceEventChild.h"

class IControlerConference
{
public:
	IControlerConference() {}
	virtual ~IControlerConference(void) {};
	virtual void Release() = 0;
	virtual void EnableKeyBorad(BOOL bEnable) = 0;
};

class IControlerConferenceEvent
{
public:
	IControlerConferenceEvent(){}; 
	virtual ~IControlerConferenceEvent(void){};
	//推出会议
	virtual void OnControlQuitConference() = 0;
	//模板调节
	virtual void OnControlTemplateAdjust() = 0;
	//视频调节
	virtual void OnControlVideoPreset() = 0;
	//广播视频
	virtual void OnControlVideoBroadcast() = 0;
	//会议白板
	virtual void OnControlWhiteBroad() = 0;
	//视频共享
	virtual void OnControlVideoShare() = 0;
	//会议屏幕
	virtual void OnControlScreen() = 0;
	//会议讨论
	virtual void OnControlVideoDiscussion() = 0;
	//录制所有
	virtual void OnControlRecordAll() = 0;
	//录制所有/停止
	virtual void OnControlRecordAllOrStop() = 0;
	//选择主屏
	virtual void OnControlSelectMainFrame(int iWin) = 0;
	//在云台中
	virtual void OnPTZing() = 0;
	//选项设置
	virtual void OnControlOptionSet() = 0;
	//按F1切换本地视频通道
	virtual void OnControlChangeLocalChannel() = 0;
	//按F2切换本地视频源
	virtual void OnControlChangeLocalResource() = 0;
	//按ESC隐藏本地视频
	virtual void OnControlHideLocalAV() = 0;
	//显示模板控制器
	virtual void OnShowTemplateControl() = 0;

	//AVCON隐藏/显示
	virtual void OnAvconHideShow() = 0;

	//显示云台控制
	virtual void OnPTZControl() = 0;
	//显示浮动窗口
	virtual void OnShowFloatWindow() = 0;
	//本地视频窗口
	virtual void OnLocalVideoWindow() = 0;
	//麦克风静音
	virtual void OnMuteMicrophone() = 0;
	//静音
	virtual void OnMuteSpeaker() = 0;
	//方向键
	virtual void OnControlDirect(DWORD dDirect) =0;
};

//创建实例
AFX_EXT_CLASS IControlerConference* CreateControlConference(IControlerConferenceEvent *pControlerConferenceEvent);


#endif

