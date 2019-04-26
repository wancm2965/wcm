//CallMgr.h
#ifndef __CALLMGR_H__
#define __CALLMGR_H__

#include <string>
#include <map>
#include <vector>
#include <IMXEC/IMXEC_Call.h>
#include <NETEC/XAutoLock.h>
#include <IMXEC/IMXEC_VideoSetting.h>
#include <AUDEC/AUDEC_Conference.h>
#include <IMXEC/AdapterHeaderDefine.h>
#include "ScreenCapChanRTP.h"
#include "XAutoLock.h"
#include "IPProcess.h"

//media release rtp port save array 20140718 add
#define MAX_PORTRLEASE_NUM  128*6
struct PortReleaseItem
{
	unsigned short m_nRtpPort;
};

struct MediaPortRelease
{
	int                m_nCount;    //有效数据个数
	PortReleaseItem    m_Array[MAX_PORTRLEASE_NUM];
	MediaPortRelease()
	{
		m_nCount = 0;
	}
};

class CallMgr
{
public:
	CallMgr(void);
	virtual ~CallMgr(void);
public:
	static CallMgr&Instance(void);
	int    m_nUseNETECSend;         //使用NETEC私有协议传输媒体流设置为1 否则设置为0 默认0 
	int    m_nUseCheckMedia;        //使用媒体流定时检测设置为1 否则设置为0 默认0
	int    m_nUseAjustMediaRate;    //使用媒体流码率适应调整设置为1 否则设置为0 默认0
	XCritSec m_csMapCall;		    //界面操作表加锁
	int    m_nIPCPort;
	int    m_nResStand;             //0 标清 1 高清 2 全高清 默认99全能力
	IPItemSave    m_AllLocalIPSave; //本地IP地址列表
	RouteItemSave m_AddrRoute;      //多网卡呼叫路由表
	std::vector<std::string> m_vecBlackIP; //ip黑名单列表
	bool  m_bUseBlackIPList;        //是否使用ip黑名单列表
	bool IsBlackListIP(const char* strIP);           //是否是黑名单ip

public:
	void AddIMXEC_Call(IMXEC_Call*pIMXEC_Call);
	void RemoveIMXEC_Call(IMXEC_Call*pIMXEC_Call);

	/******************************************************************************
	* MakeCall
	*描述：发起呼叫
	*输入：	cszUserID		-对方的用户ID或IP
			cszProtocol		-呼叫协议，"H323","SIP","IMXEC"
	*输出：无
	*返回值：呼叫标识
	*****************************************************************************/
	const char*MakeCall(const char*cszUserID,const char*cszProtocol);

	/******************************************************************************
	* AcceptCall
	*描述：接受呼叫请求
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AcceptCall(const char*cszCallID);

	/******************************************************************************
	* HangupCall
	*描述：挂断呼叫
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*描述：发送PTZ【发送云台控制命令】
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*描述：开始辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*描述：停止辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StopSecondVideo (const char*cszCallID);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* SetLocalMainVideoWnd
	*描述：设置本地主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-本地主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*描述：设置本地主流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*描述：设置本地主流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*描述：设置本地主流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*描述：设置本地主流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*描述：设置本地主流视频OSD字体,字的颜色和背景色
	*输入：	cszCallID		-呼叫标识
			logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));


	/******************************************************************************
	* SetRemoteMainVideoWnd
	*描述：设置远程主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*描述：设置远程主流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*描述：设置远程主流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*描述：获取远端主视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*描述：设置辅流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程辅流视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*描述：设置辅流视频显示的窗口位置
	*输入：	cszCallID		-呼叫标识
			x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*描述：设置辅流视频参数
	*输入：	cszCallID		-呼叫标识
			nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*描述：设置辅流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*描述：设置辅流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*描述：设置辅流视频OSD字体,字的颜色和背景色
	*输入：	cszCallID		-呼叫标识
			logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

#endif

	/******************************************************************************
	* GetLocalMainVideoInfo
	*描述：获取本地主流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			nFrame          -图像帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*描述：获取本地主视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*描述：获取远程主流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			nFrame          -图像帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetSecondVideoInfo
	*描述：获取辅流视频信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			nFrame          -图像帧率
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*描述：获取辅流视频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*描述：获取本地音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*描述：获取本地音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*描述：获取本地音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*描述：获取本地音频编码信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*描述：获取本机音频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*描述：获取远程音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*描述：获取远程音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*描述：获取远程音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*描述：获取远端音频编码
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*描述：获取远端音频的包的统计数据
	*输入：	cszCallID			-呼叫标识
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*描述：获取呼叫是否支持辅流【H239】
	*输入：	cszCallID		-呼叫标识
	*输出：	无
	*返回值：支持返回非0，不支持返回0
	*****************************************************************************/
	int GetCallSupportSecondVideo(const char*cszCallID);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*描述：开始本地主流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*描述：停止本地主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*描述：开始远端主流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*描述：停止远端主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*描述：开始辅流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*描述：停止辅流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void StopRecordSecondToAVI(const char*cszCallID);

#endif

public:
	void OnCallRingIn(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,const std::string&strProto);
	void OnCallRingOut(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName);
	void OnCallEstablished(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName);
	void OnCallDestroyed(const std::string&strCallID,const std::string&strUserID,const std::string&strUserName,IMXEC_CALL_END_REASON nReason ,int callNum=0);

	/******************************************************************************
	* OnCallRecvdPTZ
	*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnCallRecvdPTZ(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	//add by Jan for DTMF [20140730]
	/******************************************************************************
	* OnCallRecvdDTMF
	*描述：接收到DTMF事件，当对方发送DTMF命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			szDtmf			-DTMF信令
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnCallRecvdDTMF(const char*cszCallID,char dtmf);
	//end [20140730]

	/******************************************************************************
	* OnCallSecondVideoStart
	*描述：辅流开始事件，当辅流开始时，系统会调用此事件
	*输入：	strCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnCallSecondVideoStart(const std::string&strCallID);

	/******************************************************************************
	* OnCallSecondVideoStop
	*描述：辅流停止事件，当辅流停止时，系统会调用此事件
	*输入：	strCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnCallSecondVideoStop(const std::string&strCallID);

	/******************************************************************************
	* OnSIPVirtualLineRegistrationSuccess
	*描述：SIP注册成功事件
	*输入：	nLineID		-线路标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnSIPVirtualLineRegistrationSuccess(int nLineID);

	/******************************************************************************
	* OnSIPVirtualLineRegistrationError
	*描述：SIP注册失败事件
	*输入：	nLineID			-线路标识
			cszErrorCode	-错误码
	*输出：无
	*返回值：无
	*****************************************************************************/
	 void OnSIPVirtualLineRegistrationError(int nLineID,const char*cszErrorCode);

	/******************************************************************************
	* OnH323RegisterOK
	*描述：H323注册成功事件
	*输入：cszGKHost 注册到的GK地址 port 注册到的GK端口
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnH323RegisterOK(const char*cszGKHost,int port);

	/******************************************************************************
	* OnH323RegisterFailed
	*描述：H323注册失败事件
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnH323RegisterFailed(void);

	/******************************************************************************
	* SetCallType
	*描述：设置呼叫类型
	*输入：	nCallType		-呼叫类型
	*输出：	无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetCallType(IMXEC_CALL_TYPE nCallType);

	/******************************************************************************
	* GetCallType
	*描述：获取呼叫类型
	*输入：	无
	*输出：	nCallType		-呼叫类型
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetCallType(IMXEC_CALL_TYPE&nCallType);

	/******************************************************************************
	* GetMainAudioID
	*描述：获取主流音频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainAudioID	-主流音频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*描述：获取主流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainVideoID	-主流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*描述：获取辅流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nSecondVideoID	-辅流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*描述：设置主流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*描述：设置辅流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenWnd
	*描述：设置辅流视频所在屏幕窗口位置
	*输入：	cszCallID		-呼叫标识
			nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*描述：设置远端视频所在屏幕窗口位置
	*输入：	cszCallID		-呼叫标识
			nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#else

	/******************************************************************************
	* SetLocalRecordCallback
	*描述：设置本地录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*描述：设置远程录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*描述：设置辅流录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*描述：保存本地主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*描述：保存远程主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*描述：保存辅流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* HotDisplayVideo
	*描述：视频显示支持显示器热插拔接口
	*输入：	cszCallID		    -呼叫标识
			hWndLocal			-本地主视频显示的窗口句柄 如没有请设置为NULL
			hWndRemote			-远程主视频显示的窗口句柄 如没有请设置为NULL
			hWndSecond         	-远程辅流视频显示的窗口句柄 如没有请设置为NULL
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond);

#endif

	/******************************************************************************
	* SendDTMF
	*描述：发送DTMF
	*输入：	cszCallID		-呼叫标识
			nDTMF			-DTMF
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* StartMixingWithWaveFile
	*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
	*输入：	cszCallID		-呼叫标识
			cszPathName		-WAVE文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*描述：停止对呼叫进行WAVE文件混音
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StopMixingWithWaveFile(const char*cszCallID);

	/******************************************************************************
	* SetAudioOnly
	*描述：设置是否只支持音频
	*输入：	nAudioOnly		-是否只支持音频【0：支持视频音频，非0：只支持音频】
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetAudioOnly(int nAudioOnly);
	int GetAudioOnly(void);

	/******************************************************************************
	* GetCallProtocol
	*描述：获取呼叫协议
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回呼叫协议，失败返回空串""
	*****************************************************************************/
	const char*GetCallProtocol(const char*cszCallID);

	/******************************************************************************
	* GetHasVideo
	*描述：获取是否只有视频
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：如果有视频，那么返回非0，否则返回0
	*****************************************************************************/
	int GetHasVideo(const char*cszCallID);

	/******************************************************************************
	* SetEnableVideoTranscoding
	*描述：设置是否启用视频转码
	*输入：nEnable		-是否启用视频转码【0：不启用视频转码，非0：启用视频转码】
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetEnableVideoTranscoding(int nEnable);

	/******************************************************************************
	* GetEnableVideoTranscoding
	*描述：获取是否启用视频转码
	*输入：无
	*输出：无
	*返回值：如果启用视频转码，那么返回非0，否则返回0
	*****************************************************************************/
	int GetEnableVideoTranscoding(void);

	/******************************************************************************
	* GetSecondVideoDirection
	*描述：获取辅流的方向
	*输入：	cszCallID		-呼叫标识
	*输出： nDir			-辅流的方向
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);


	unsigned short GetRTPBasePort(void);
	void SetRTPBasePort(unsigned short nRTPBasePort);

	int OpenLocalMainVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	void OnRequestLocalMainVideoKeyFrame(MediaCall*pMediaCall);
	void CloseLocalMainVideoCapDev(MediaCall*pMediaCall);
	
	int OpenLocalSecondVideoCapDev(MediaCall*pMediaCall,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate);
	void OnRequestSecondVideoKeyFrame(MediaCall*pMediaCall);
	void CloseLocalSecondVideoCapDev(MediaCall*pMediaCall);


	//H235 Media use

	/******************************************************************************
	* GetH235AuthCheckState
	*描述：获取H235验证检查状态 用于取呼叫验证及为网守时作验证检查
	*输入：	cszCallID		  -呼叫标识
	*输出： nState -验证检查状态
			nLength -返回验证失败原因字符串长度
	*返回值：成功返回验证失败原因，失败返回空
	*****************************************************************************/
	const char* GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength);


	/******************************************************************************
	* OnChannelRecvMediaEncryption
	*描述：接收媒体通道已加密状态通知，当打开加密的接收媒体通道时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	nMediaType		-媒体类型
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnChannelRecvMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType);

	/******************************************************************************
	* OnChannelSendMediaEncryption
	*描述：发送媒体通道已加密状态通知，当打开加密的发送媒体通道时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
	nMediaType		-媒体类型
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnChannelSendMediaEncryption(const char*cszCallID,IMXEC_CALL_MEDIA_TYPE nMediaType);



	/******************************************************************************
	* SetExternalIP
	*描述：设置外网映射IP
	*输入：cszExternalIP		-外网映射IP
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetExternalIP(const char*cszExternalIP);
	const char*GetExternalIP(void);
	unsigned long GetExternalIPAddress(void);


	/******************************************************************************
	* OnDAP_RecvCallH245ControlMessage
	*描述：接收到H245控制消息回调 包含flowControlIndication H239令牌相关 会控消息
	*输入：	cszCallID			-呼叫标识
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	//added by jiangdingfeng 2013-09-25 增加I帧请求
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*描述：请求主流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*描述：请求辅流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void RequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* HasActiveCall
	*描述：是否有呼叫
	*输入：无
	*输出：无
	*返回值：BOOL
	*****************************************************************************/
	bool HasActiveCall();

	/******************************************************************************
	* SetLocalMainVideoSource
	*描述：设置本地主视频源
	*输入：cszCallID		  -呼叫标识
	       nMainVideoDevID    -主流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*描述：设置本地辅视频源
	*输入：cszCallID		  -呼叫标识
		   nSecondVideoDevID  -辅流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);

	//add use to send private msg
	/******************************************************************************
	*描述:	向对端发送自定义非标消息
	*输入:	cszCallID		-呼叫标识
			cszContent		-自定义非标准信息内容
			nContentLen		-自定义非标准信息长度
			cszMimeType		-自定义非标信息内容类型 如为XML 值设为application/xml,如为一般字符串则设为NULL或""
	*****************************************************************************/
	int SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType);

	/******************************************************************************
	* OnDAP_CallRecvNonstandardData
	*描述：接收到有终端发来H245非标消息时系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			cszPacket		-非标准信息负载
			nPacketLen		-非标准信息长度
			cszMimeType		-目前仅SIP使用 信息内容类型 如application/xml
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_RecvNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen,const char * cszMimeType);

	//限制同时只能有一个call在呼叫
	bool IsCanCall();
	void SetCallNumZero();
	void SetCanCallNot();

	/******************************************************************************
	* GetCallH323ID
	*描述：获取H323ID
	*输入：	cszCallID		-呼叫标识
            szH323ID        -H323ID Buf
			nMaxLen         -Buf 最大长度
	*输出：	无
	*返回值：获取到返回H323ID长度，否则返回NULL
	*****************************************************************************/
	int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);
	/******************************************************************************
	* GetCallE164ID
	*描述：获取E164ID
	*输入：	cszCallID		-呼叫标识
            szE164ID        -E164ID Buf
			nMaxLen         -Buf 最大长度
	*输出：	无
	*返回值：获取到返回E164ID长度，否则返回NULL
	*****************************************************************************/
	int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);

	//Add U6 U7 use
	/******************************************************************************
	* OnDAP_RecvRemoteAudioRTPPacket
	*描述：接收到音频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecID             -音频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	* OnDAP_RecvRemoteMainVideoRTPPacket
	*描述：接收到主流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecType           -视频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
	/******************************************************************************
	* OnDAP_RecvRemoteSecondVideoRTPPacket
	*描述：接收到辅流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
	        codecType           -视频编码类型
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);



	/******************************************************************************
	* OnPeerEntityInfo
	*描述：返回对端实体信息
	*输入：	pCallParam			-CallParamSet 信息体
	*输出：无
	*返回值：无
	*****************************************************************************/
    void OnPeerEntityInfo(const CallParamSet * pCallParam);


    /******************************************************************************
    * SetNetecTransEnable
    *描述：是否使用NETEC私有传流
    *输入：	cszCallID			-呼叫标识
            nEnable             -是否使用,1为使用,0为不使用
    *输出：无
    *返回值：无
    *****************************************************************************/
    void SetNetecTransEnable(const char*cszCallID,int nEnable);

protected:
	typedef std::map<IMXEC_Call*,IMXEC_Call*>		MapIMXEC_Call;
	typedef std::map<std::string,std::string>		MapCallID;

	typedef std::map<std::string,ScreenCapChanRTP*>	MapScreenCapChanRTP;

protected:
	std::string				m_strEmpty;
	XCritSec				m_csMapIMXEC_Call;
	MapIMXEC_Call			m_MapIMXEC_Call;

	IMXEC_Call*				m_pIMXEC_CallExclusive;

	MapCallID				m_MapCallID;
	IMXEC_CALL_TYPE			m_nCallType;


	int						m_nAudioOnly;

	unsigned short			m_nRTPBasePort;



	int						m_nEnableVideoTranscoding;


	int						m_nMainVideoScreenIndex;
	int						m_nSecondVideoScreenIndex;

	XCritSec				m_csMapScreenCapChanRTPMain;
	MapScreenCapChanRTP		m_MapScreenCapChanRTPMain;

	XCritSec				m_csMapScreenCapChanRTPSecond;
	MapScreenCapChanRTP		m_MapScreenCapChanRTPSecond;

	std::string				m_strExternalIP;
	unsigned long			m_nExternalIPAddress;
	int						m_nCallNum;				//当前呼入的call数

public:
	void SaveMediaReleasePort(MediaPortRelease & MediaPortRelease,unsigned short rtpPort);
	unsigned short GetMediaReleasePort(MediaPortRelease & MediaPortRelease);




	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*描述：音频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallAudioRecvChannelConnected(const char*cszCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallAudioRecvChannelConnected
	*描述：音频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallAudioRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelConnected
	*描述：主流视频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			ip					-本端接收rtp ip
			port				-本端接收rtp port
			nLevel				-分辨率实际H264level值 可选
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallMainVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0);

	/******************************************************************************
	* OnDAP_CallMainVideoRecvChannelDestroyed
	*描述：主流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallMainVideoRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelConnected
	*描述：辅流视频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			ip					-本端接收rtp ip
			port				-本端接收rtp port
			nLevel				-分辨率实际H264level值 可选
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallSecondVideoRecvChannelConnected(const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth=0,int nHeight=0,*/int nLevel=0);

	/******************************************************************************
	* OnDAP_CallSecondVideoRecvChannelDestroyed
	*描述：辅流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallSecondVideoRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelConnected
	*描述：FECC控制接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallFECCRecvChannelConnected(const char*cszCallID ,int nPayloadType,unsigned long ip
		, int port);

	/******************************************************************************
	* OnDAP_CallFECCRecvChannelDestroyed
	*描述：FECC控制接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallFECCRecvChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallAudioSendChannelConnected
	*描述：音频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallAudioSendChannelDestroyed
	*描述：音频发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallAudioSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelConnected
	*描述：主流视频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			nLevel				-分辨率实际H264level值 可选
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0);

	/******************************************************************************
	* OnDAP_CallMainVideoSendChannelDestroyed
	*描述：主流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallMainVideoSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelConnected
	*描述：辅流视频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			nLevel				-分辨率实际H264level值 可选
			ip					-对端rtp ip
			port				-对端rtp port
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,VIDEO_FORMAT_TYPE VideoFormatType,/*int nWidth,int nHeight,*/int nLevel = 0);
	
	/******************************************************************************
	* OnDAP_CallSecondVideoSendChannelDestroyed
	*描述：辅流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallSecondVideoSendChannelDestroyed(const char*cszCallID);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelConnected
	*描述：FECC控制发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			ip					-对端rtp ip
			port				-对端rtp port
			nPayloadType		-PayloadType
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const char*cszCallID,int nPayloadType
		,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*描述：FECC控制发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	void OnDAP_CallFECCSendChannelDestroyed(const char*cszCallID);

	void CheckMapCallEmpty(void);

	bool GetOneSaveLocalIP(std::string & strLocalIP);
	bool GetSameToDestIP(std::string & strLocalIP,std::string & strDestIP);

#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*描述：设置远端视播放的缓冲帧数
	*输入：	cszCallID			-呼叫标识
	        nCacheFrameCount	-缓冲的帧数（0 - 60）
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount);
#endif

};

#endif
