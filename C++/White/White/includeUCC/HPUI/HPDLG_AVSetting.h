//========================================================================
//类名：音视频设置向导
//功能：设置音视频参数
//作者：戴磊
//时间：2009-9-15
//说明：
//--------------------------------------------------------------------------
#include "../HPLoadData/HPLoadiniData.h"

#pragma once


typedef enum{
	AV_SET_LOCAL = 0,	//本地
	AV_SET_REMOTE,		//远程
	AV_SET_DESKTOP,		//桌面共享
	AV_SET_DEVICE,		//设备
	AV_SET_TALK,		//对讲通道
	AV_SET_MIXVIDEO,	//混合屏幕
	AV_SET_IPCAM,		//IP Camer
}HPUI_AV_SET_TYPE;

typedef enum{
	SHOW_AVSETTING = 0,		//标准版本音视频设置
	SHOW_AUDIOSETTING,		//只有音频设置
	SHOW_VIDEOSETTING,		//只有视频设置
}DLG_AVSETING_SHOWTYPE;

//音频设备列表
typedef struct tagHPUI_AUDIO_DEVICE
{
	std::string	strDeviceName;
	int			iDeviceNo;
}HPUI_AUDIO_DEVICE, *PHPUI_AUDIO_DEVICE;
typedef std::vector<HPUI_AUDIO_DEVICE>			HPUI_AUDIO_DEVICE_LIST;
typedef HPUI_AUDIO_DEVICE_LIST::iterator	HPUI_AUDIO_DEVICE_LIST_IT;

//视频设备列表
typedef struct tagHPUI_VIDEO_DEVICE
{
	std::string	strDeviceName;		//设备名称
	int			iDeviceNo;			//设备编号
	bool		bVideo_HDDevice;	//是否为高清卡
	bool		bHaveSoundCard;
}HPUI_VIDEO_DEVICE, *PHPUI_VIDEO_DEVICE;
typedef std::vector<HPUI_VIDEO_DEVICE>			HPUI_VIDEO_DEVICE_LIST;
typedef HPUI_VIDEO_DEVICE_LIST::iterator	HPUI_VIDEO_DEVICE_LIST_IT;
/*
typedef struct tagHPUI_AVSET
{
	int	nVideoDev;				//视频设备
	int	nVideoCodec;			//视频编码器
	int	nResolution;			//分辨率
	int	nFrameRate;				//图像帧率
	int	nAverageStream;			//平均码流
	int	nBitrateControlType;	//码流控制方式

	int	nAudioDev;				//音频设备
	int	nAudioCodec;			//音频编码器
	int	nAudioInType;			//音频输入类型
}HPUI_AVSET, *PHPUI_AVSET;
*/
class HPDLG_API HPDLG_AVSetNotify
{
public:
	HPDLG_AVSetNotify(){};
	virtual~ HPDLG_AVSetNotify(){};

	virtual void OnApplyAVSet(HPUI_AVSET videoset, unsigned long ulSessionID) = 0;
	virtual void OnAbandonAVSet(unsigned long ulSessionID) = 0;
	virtual void SetRemoteAudioAdvanceNotify(string strAudioSet) = 0;
	virtual void SetVideoAdvanceSetNotify(string strVideoSet, HPUI_AV_SET_TYPE type) = 0;
	virtual void OnAVSetHide() = 0;
};

class AFX_EXT_CLASS HPDLG_AVSetting
{
public:
	HPDLG_AVSetting(void){};
	virtual~ HPDLG_AVSetting(void){};

	// 显示/隐藏对话框
	virtual void ShowWindow(int nCmdShow) = 0;

	//当前框口是否已打开
	virtual bool IsWindowShow() = 0;

	// 设置音视频初始化值
	virtual void SetIniOption(HPUI_AUDIO_DEVICE_LIST listMic, HPUI_VIDEO_DEVICE_LIST listCamer, HPUI_AVSET initValue, HPUI_AV_SET_TYPE SetType = AV_SET_LOCAL, bool bWindows7 = false, std::string strSupport = "") = 0;

	//修改父窗口
	virtual CWnd* SetAVSetDlgParent(CWnd* pParent) = 0;

	//根据类型改变显示
	virtual void UpdateDlgShowType() = 0;

	//设置类型
	virtual void SetShowType(DLG_AVSETING_SHOWTYPE type) = 0;

	//设置类型
	virtual void SetSessionID(unsigned long ulSessionID) = 0;

	//设置远程音频的参数
	virtual void SetRemoteAdvanceSetString(string strAudioSet, string strVideoSet) = 0;

	//设置接收回调消息的窗口指针
	virtual void SetMessageWnd(CWnd* pWnd) = 0;
};
// 创建对话框
HPDLG_API HPDLG_AVSetting* CreateAVSettingDlg(HPDLG_AVSetNotify& rNotify, CWnd* pParent = NULL, DLG_AVSETING_SHOWTYPE type = SHOW_AVSETTING, unsigned long ulSessionID = 0, CWnd* pMessageWnd = NULL);
// 销毁对话框
HPDLG_API void DestroyAVSettingDlg();
// 获得指针
HPDLG_API HPDLG_AVSetting* GetHPDLG_AVSetting(void);

