//========================================================================
//类名：本地音视频采集类
//功能：采集本地音视频
//作者：戴磊
//时间：2008-2-26
//说明：
//--------------------------------------------------------------------------
#pragma once

#include "../../HPLoadData/HPLoadiniData.h"
/*
typedef struct
{
	bool				HPImageNoise;
	int 				HPVideoSize;
	int 				nFrameRate;
	int 				nQuality;
	VIDEC_CODEC_TYPE	codecType;
	int 				nBitrateControlType;
	bool				bEnableSub;
	int 				nSubScaleType;
	int 				nSubQuality;
}HPAV_VIDEO_CAPTURE_TYPE;
*/

class HPAV_API HPAV_ChannelLocalNotify
{
public:
	HPAV_ChannelLocalNotify(){};
	~HPAV_ChannelLocalNotify(){};
public:
	/*	函数功能：关闭当前窗口
		参    数：	pWindow：		当前窗口的指针
					ulSessionID：	标记
		返 回 值：*/
	//virtual void HPAV_PlayerNotifyClose(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;

	/*	函数功能：关闭所有窗口
		参    数：	pWindow：		当前窗口的指针
					ulSessionID：	标记
		返 回 值：*/
	//virtual void HPAV_PlayerNotifyCloseAll(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;
};

class HPAV_API HPAV_ChannelLocal
{
public:
	HPAV_ChannelLocal(void){};
	virtual ~ HPAV_ChannelLocal(void){};

public:
	virtual void OpenAudioCapture(int nDevID, X_AUDIO_CODEC_ID CodecType) = 0;
	virtual void CloseAudioCapture() = 0;
	virtual void OpenVideoCapture(int nDevID, int nSizeIndex, int nFrameRate, int nBitrate, int nBitrateControlType, VIDEC_CODEC_TYPE CodecType, int nSub, int nQSub) = 0;
	virtual void CloseVideoCapture() = 0;

	//获取音量感值（建议200毫秒取一次）
	virtual int  GetAudioLevel(void) = 0;
	//获取占用带宽值
	virtual unsigned long GetBandwidth(void) = 0;
	//获取音视频码流值
	virtual void GetAVBitrate(unsigned long& ulAudioBitrate, unsigned long& ulVideoBitrate) = 0;
	//获取视频大小
	virtual void GetAVSize(unsigned short& uWidth, unsigned short& uHeight) = 0;
	//获取本地音视频通道ID
	virtual void GetChannelLocalID(unsigned long& ulAudioChannel, unsigned long& ulVideoChannel) = 0;

	/* 设置录像保存路径 */
	virtual void SetRecordPath(std::string strRecordPath, std::string strLocalMemberID) = 0;
	/* 开始录像 */
	virtual bool StartRecord() = 0;
	/* 停止录像 */
	virtual void StopRecord() = 0;
	/* 是否正在录像 */
	virtual bool IsRecording() = 0;

	/* 应用视频字幕 */
	virtual void ApplyOSD(BOOL bUseOSD, int iVODLocation, std::string strText) = 0;
	virtual void SetVODFormat(CHARFORMAT cfVOD) = 0;
	virtual CHARFORMAT GetVODFormat(void) = 0;
	virtual void SetVODBackColor(COLORREF clrBack) = 0;
	virtual COLORREF GetVODBackColor(void) = 0;

	virtual HWND GetSafeHwnd() const = 0;
	virtual BOOL ShowWindow(int nCmdShow) = 0;
	virtual BOOL SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags) = 0;
	virtual void GetWindowRect(LPRECT lpRect) const = 0;
	virtual bool IsWindowVisible(void) = 0;
};


class HPAV_API HPAV_ChannelLocalMgrNotify
{
public:
	HPAV_ChannelLocalMgrNotify(){};
	~HPAV_ChannelLocalMgrNotify(){};
public:
	/*	函数功能：关闭当前窗口
		参    数：	pWindow：		当前窗口的指针
					ulSessionID：	标记
		返 回 值：*/
	//virtual void HPAV_PlayerNotifyClose(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;

	/*	函数功能：关闭所有窗口
		参    数：	pWindow：		当前窗口的指针
					ulSessionID：	标记
		返 回 值：*/
	//virtual void HPAV_PlayerNotifyCloseAll(HPAV_Player* pWindow, unsigned long ulSessionID) = 0;
};

class HPAV_API HPAV_ChannelLocalMgr
{
public:
	HPAV_ChannelLocalMgr(void){};
	virtual~ HPAV_ChannelLocalMgr(void){};

public:
	/*	函数功能：销毁CHPAVChannelLocalMgr对象
		参    数：
		返 回 值：*/
	virtual void Destory(void) = 0;

	/*	函数功能：设置MCU
		参    数：
		返 回 值：*/
	virtual void SetMCU(std::string mcuid, std::string mcuaddr, unsigned short mcuport, std::string mcutype) = 0;

	/*	函数功能：创建本地通道
		参    数：	ulChannelIndex：	通道序号
					ulAudioChannel：	音频通道
					ulVideoChannel：	视频通道
					ulVideoChannelSub：	视频子流通道
		返 回 值：*/
	virtual HPAV_ChannelLocal* CreateHPAV_ChannelLocal(unsigned long ulChannelIndex,CWnd* pParentWnd) = 0;

	/*	函数功能：创建监控对讲通道
		参    数：
		返 回 值：*/
	virtual HPAV_ChannelLocal* CreateMonitorAudioChannel(int nDevID, X_AUDIO_CODEC_ID CodecType) = 0;

	/*	函数功能：获取本地音视频通道
		参    数：	ulChannelIndex：	通道序号
		返 回 值：*/
	virtual HPAV_ChannelLocal* GetHPAV_ChannelLocal(unsigned long ulChannelIndex) = 0;

	/*	函数功能：获取本地音视频通道
		参    数：
		返 回 值：*/
	virtual HPAV_ChannelLocal* GetHPAV_MonitorAudioChannel() = 0;

	/*	函数功能：获取本地音视频占用的总带宽
		参    数：
		返 回 值：*/
	virtual unsigned long GetChannelLocal_Bandwidth() = 0;
};

HPAV_API HPAV_ChannelLocalMgr* CreateHPAV_ChannelLocalMgr(HPAV_ChannelLocalMgrNotify& rNotify, unsigned long ulSessionID = 0);
HPAV_API void DestoryHPAV_ChannelLocalMgr(void);
HPAV_API HPAV_ChannelLocalMgr* GetHPAV_ChannelLocalMgr(void);
