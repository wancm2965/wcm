#ifndef __H323CALLMGR_H__
#define __H323CALLMGR_H__

#include <string>
#include <map>
#include "H323Call.h"
#include "H323APIType.h"
#include "../DefineHeader/H323DefineHeader.h"
#include "CallMgr.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include "XThreadBase.h"

#if defined(HAS_A7MCU_INTERFACE) && (HAS_A7MCU_INTERFACE == 1)
#include "H323A7DAPCall.h"
#else 
#include "H323DAPCall.h"
#endif

#else
#include "H323MediaCall.h"
#endif

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#include <AVCONMP/IMXEC_Call.h>
#include <AVCONMP/AdapterHeaderDefine.h>

#else

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPH323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#include <IMXEC/IMXEC_Call.h>
#include <IMXEC/AdapterHeaderDefine.h>

#endif


#include "XAutoLock.h"
#include "CallCapability.h"
#include "XBufferPool.h"
#include "StrPacket.h"
#include "H235Mgr.h"

#include "H323RegUser.h"



/////////////////////////////////////////////////////////////////////////

//呼叫解析出的DNS IP对
#define MAX_DNSIP_NUM   10

struct DNSIPItem
{
char strDNS[128];
char strIP[16];
};

struct DNSIP
{
	unsigned int	m_nNum;	//有效数据个数
	DNSIPItem		m_Array[MAX_DNSIP_NUM];

	DNSIP()
	{
		m_nNum = 0;
	}
};

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

class H323CallMgr 
	: public XBufferPool
	, public XThreadBase

#else

class H323CallMgr 
	: public XBufferPool

#endif

{
public:
	H323CallMgr();
	~H323CallMgr();
public:
	static H323CallMgr&Instance(void);
public:
	/******************************************************************************
	* SetH323ID
	*描述：设置注册帐号
	*输入：	cszH323ID		-注册帐号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetH323ID(const char*cszH323ID);

	/******************************************************************************
	* SetTelNumber
	*描述：设置电话号码
	*输入：	cszTelNumber		-电话号码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetTelNumber(const char*cszTelNumber);

	/******************************************************************************
	* SetRegPassword
	*描述：设置注册密码
	*输入：	cszRegPassword	-注册帐号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRegPassword(const char*cszRegPassword);

	/******************************************************************************
	* SetUserName
	*描述：设置用户名称
	*输入：	cszUserName		-用户名称
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetUserName(const char*cszUserName);

	/******************************************************************************
	* SetRegHost
	*描述：设置用户名称
	*输入：	cszHost			-注册服务器
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRegHost(const char*cszHost);

	/******************************************************************************
	* Register
	*描述：注册
	*输入：nType			-注册类型【0:自动查找GK服务器，非0:注册到指定的GK服务器】
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int Register(int nType);

	/******************************************************************************
	* UnRegister
	*描述：注销
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int UnRegister(void);

	/******************************************************************************
	* GetRegState
	*描述：获取注册状态
	*输入：无
	*输出：	nRegState			-注册状态
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetRegState(IMXEC_H323::REG_STATE&nRegState);

	/******************************************************************************
	* SetH323ListenPort
	*描述：设置侦听端口
	*输入：	nH323ListenPort			-侦听端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetH323ListenPort(unsigned short nH323ListenPort);

	/******************************************************************************
	* SetLocalIP
	*描述：设置注册帐号
	*输入：	cszH323ID		-注册帐号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalIP(const char*cszLocalIP);

	/******************************************************************************
	* SetRTPBasePort
	*描述：设置侦听端口
	*输入：	nRTPBasePort			-RTP开始端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRTPBasePort(unsigned short nRTPBasePort);

	/******************************************************************************
	* SetSupportH239
	*描述：设置是否支持H239
	*输入：nSupportH239		-是否支持H239
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSupportH239(int nSupportH239);
	int GetSupportH239(void){return m_nSupportH239;};

	int GetMainVideoDevID(void){return m_nMainVideoDevID;}; 
	int GetMainVideoDevFrameRate(void){return m_nMainVideoDevFrameRate;};
	VIDEC_CODEC_TYPE GetMainVideoCodecType(void){return m_nMainVideoCodecType;};
	VIDEO_FORMAT_TYPE GetMainVideoFormatType(void){return m_nMainVideoFormatType;};
	int GetSecondVideoDevID(void){return m_nSecondVideoDevID;}; 
	int GetSecondVideoDevFrameRate(void){return m_nSecondVideoDevFrameRate;};
	VIDEC_CODEC_TYPE GetSecondVideoCodecType(void){return m_nSecondVideoCodecType;};
	VIDEO_FORMAT_TYPE GetSecondVideoFormatType(void){return m_nSecondVideoFormatType;};
    int GetMainVideoNetworkType(void){return m_nMainVideoNetworkType;};
    int GetSecondVideoNetworkType(void){return m_nSecondVideoNetworkType;};

	/******************************************************************************
	* SetAudioDevParam
	*描述：设置主流视频设备参数
	*输入： nAudioCodecID		-音频编码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);
	X_AUDIO_CODEC_ID GetAudioCodecID(void){return m_nAudioCodecID;};

	/******************************************************************************
	* SetMainVideoDevParam
	*描述：设置主流视频设备参数
	*输入： nDevID				-设备标识
			nFrameRate			-帧率
			nBitrate			-码流
			nVideoCodecType		-视频编码
			nVideoFormatType	-视频格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);

	/******************************************************************************
	* SetSecondVideoDevParam
	*描述：设置辅流视频设备参数
	*输入： nDevID				-设备标识
			nFrameRate			-帧率
			nBitrate			-码流
			nVideoCodecType		-视频编码
			nVideoFormatType	-视频格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);

	const std::string GetUserName(void){return m_strUserName;};
	unsigned short GetRTPBasePort(void);
	HAPP GetHApp(void){return m_hApp;};
	unsigned long GetLocalIPAddress(const std::string&strCallLocalIP);

	/******************************************************************************
	* MakeCall
	*描述：发起呼叫
	*输入：	cszUserID		-对方的用户ID或IP
	*输出：无
	*返回值：呼叫标识
	*****************************************************************************/
	const char*MakeCall(const char*cszUserID);

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


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

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

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


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
	virtual int GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else


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


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



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
	virtual int GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else


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

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


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
	virtual int GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd);

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

	virtual int GetAudioMaxBitrate(void);
	virtual int GetMainVideoMaxBitrate(void);
	virtual int GetSecondVideoMaxBitrate(void);

	virtual int Start(void);//启动
	virtual void Stop(void);			//停止


	//============ 回调函数 ================
	//===注册状态事件
	virtual void OnRegistrationState(bool bState);

	//===呼叫状态变化事件
	virtual void OnCallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);
	virtual void OnCallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	virtual void OnCallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);
	virtual void OnCallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	virtual void OnCallSecondVideoStart(const std::string&strCallID);
	virtual void OnCallSecondVideoStop(const std::string&strCallID);
	virtual void OnCallLocalMainVideoStart(const std::string&strCallID);
	virtual void OnCallLocalMainVideoStop(const std::string&strCallID);

#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2))
//#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

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
	virtual int StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*描述：停止本地主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*描述：开始远端主流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*描述：停止远端主流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(const char*cszCallID);

	/******************************************************************************
	* StartRecordSecondToAVI
	*描述：开始辅流录像
	*输入：	cszCallID			-呼叫标识
			cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*描述：停止辅流录像
	*输入：cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(const char*cszCallID);



#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	/******************************************************************************
	* GetMainAudioID
	*描述：获取主流音频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainAudioID	-主流音频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*描述：获取主流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nMainVideoID	-主流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*描述：获取辅流视频标识
	*输入： cszCallID		-呼叫标识
	*输出： nSecondVideoID	-辅流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*描述：设置主流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*描述：设置辅流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex);


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
	virtual void SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

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
	virtual void SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen);

	//added by jiangdingfeng 2013-09-25 增加I帧请求
	/******************************************************************************
	* RequestMainVideoKeyFrame
	*描述：请求主流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void RequestMainVideoKeyFrame(const char*cszCallID);
	/******************************************************************************
	* RequestSecondVideoKeyFrame
	*描述：请求辅流I帧
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void RequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SetLocalMainVideoSource
	*描述：设置本地主视频源
	*输入：cszCallID		  -呼叫标识
	       nMainVideoDevID    -主流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID);
	/******************************************************************************
	* SetLocalSecondVideoSource
	*描述：设置本地辅视频源
	*输入：cszCallID		  -呼叫标识
		   nSecondVideoDevID  -辅流设备ID
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID);


#if defined(HAS_H323_AVCON_MP) && ( (HAS_H323_AVCON_MP == 1) || (HAS_H323_AVCON_MP == 2) )

#else



	/******************************************************************************
	* SetLocalRecordCallback
	*描述：设置本地录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*描述：设置远程录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*描述：设置辅流录像回调
	*输入：	cszCallID		-呼叫标识
			pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback);

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


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)



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
	* SendDTMF
	*描述：发送DTMF
	*输入：	cszCallID		-呼叫标识
			nDTMF			-DTMF
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendDTMF(const char*cszCallID,const char nDTMF);

	/******************************************************************************
	* GetSecondVideoDirection
	*描述：获取辅流的方向
	*输入：	cszCallID		-呼叫标识
	*输出： nDir			-辅流的方向
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);

	void DoCheckPTZAction(void);

public:
	bool					m_bRegisteredToGK;
	int						m_nUseGK;
	HAPP					m_hApp;
	CallCapability			m_CallCapability;

public:
	bool HasCall();
	void AddCall(H323MediaCall*pH323MediaCall);
	void RemoveCall(H323MediaCall*pH323MediaCall);
	virtual void OnTimer(UINT32 nIDEvent);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	virtual void OnTimerDtmf(UINT32 nIDEvent);

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

public:
	typedef std::map<std::string,H323MediaCall*>	MapH323MediaCall;

	XCritSec				m_csMapH323MediaCall;					//界面操作表加锁
	MapH323MediaCall		m_MapH323MediaCall;						//界面来的操作表

private:

	XCritSec				m_csMapTempH323MediaCall;				//线程操作表加锁
	MapH323MediaCall		m_MapTempH323MediaCall;					//线程来的操作表
	int						m_nStarted;
	std::string				m_strEmpty;
//test
public:
	std::string				m_strH323ID;							//H323ID;
	std::string				m_strTelNumber;							//E164;
	std::string				m_strUserName;
	std::string				m_strRegHost;
	std::string				m_strRegPassword;
	unsigned short			m_nH323ListenPort;
	unsigned short			m_nRTPBasePort;
	std::string				m_strLocalIP;

	//MC地址与会议号
	std::string				m_strPeerMCUAddr;
	std::string				m_strConfID;

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	UINT32					m_nIDDtmfTimer;

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


	UINT32					m_nIDEventTimer;

	int						m_nAudioMaxBitrate;
	X_AUDIO_CODEC_ID		m_nAudioCodecID;
	int						m_nMainVideoDevID;
	int						m_nMainVideoDevFrameRate;
	int						m_nMainVideoMaxBitrate;
	VIDEC_CODEC_TYPE		m_nMainVideoCodecType;
	VIDEO_FORMAT_TYPE		m_nMainVideoFormatType;
    int                     m_nMainVideoNetworkType;


	int						m_nSupportH239;
	int						m_nSecondVideoDevID;
	int						m_nSecondVideoDevFrameRate;
	int						m_nSecondVideoMaxBitrate;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;
    int                     m_nSecondVideoNetworkType;

			
	int						m_nH245PortFrom;
	int						m_nH245PortTo;


private:
	void RegisterGK();

public:

	EncryptionCapability        m_EncryptionCapability;					//加解密能力集
	int							m_nH235MediaUse;						//0 不使用 1可选使用 2强制使用H235媒体流加密
	int							m_nH235AuthUse;							//0 不使用 1使用
	int							m_nUseAlogrithmId;						//首选加密算法
	H235Mgr						m_h235Mgr;

	bool						m_bCheckIncoming;						//是否对接收到的呼叫消息进行检查验证
	char						m_strPassword[64];						//验证密码
	char						m_strSenderId[256];						//发送标识即本端标识
	char						m_strGeneralId[256];					//接收标识即对端标识
	bool						m_bH235AnnexI;                          //annex D I 消息完整性及密码认证
	bool						m_bH235AnnexIA;                         //annex D IA 仅密码认证
	bool						m_bH235AnnexII;							//annex E II 采用公钥/私钥对的数字签名
	bool						m_bH235AnnexIII;						//annex E III 端到端认证
	bool						m_bH235AnnexIV;							//annex F 规程IV 逐段转接的安全性 结合了规程I和规程II
	bool						m_bUserPrimes;							//媒体流加密 使用预定义素数参与生成共享密钥
	bool						m_bH235MediaDH512;						//媒体流加密 使用DH512生成7字节共享密钥 若都选择则按最长DH生成共享密钥
	bool						m_bH235MediaDH1024;						//媒体流加密 使用DH1024生成21字节共享密钥
	bool						m_bH235MediaDH1536;						//媒体流加密 使用DH1536生成21字节共享密钥

	int							m_nMediaEncrypSpecified;				//0 为不使用指定密码(自动协商) 1 为使用指定密码
	char						m_strMediaEncrypPWD[64];				//使用指定密码时设置的加密密码

public:

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
	* SetH235MediaSecurityEncryptionPWD
	*描述：	设置媒体流加密密码
	*输入：	type 0 为不使用指定密码(自动协商) 1 为使用指定密码
			encrypPwd 使用指定密码时设置的加密密码 长度小于64字节
	*****************************************************************************/
	void SetH235MediaSecurityEncryptionPWD(int type=0,char * encrypPwd=NULL);

	//H235SecuritySettingInitialize
	void H235SecuritySettingInitialize();

	//取加解密算法类型 基于ISO OID 
	//return >0 - successful, =0 - failure.
	int GetEncDecAlgorithmTypeFromOID(EncDecAlgorithmID &id,unsigned char * pData,int dataLen);
	//return >0 - successful, =0 - failure.
	int GetEncDecAlgorithmOIDFromType(EncDecAlgorithmID algId,unsigned char * pData,int &dataLen);

	//H235 call 初始化
	void H235SecurityCallInit(H323Call *pH323MediaCall);
	void H235SecurityCallEnd(H323Call *pH323MediaCall);

	//设置H235相关 start前调用
	void SetH235AuthData();


	//设置H235选项 0 不使用 1使用
	void SetH235SecurityAuthOption(int option);

	//设置媒体流加密选项 0 不使用 1可选使用 2强制使用H235媒体流加密
	void SetH235MediaSecurityOption(int option);

	//设置媒体流优先加密算法
	void SetH235MediaEncryptionAlogrithmFirst(int alogrimId);


	//设置H235验证信息 senderId 发送标识即本端标识 generalId接收标识即对端标识 password 验证密码
	//本端用senderId来检查对方发来的generalID 本端用generalID来检查对方发来的senderId
	void SetH235SecurityAuth(char * senderId,char * generalId,char * password);

	//设置可用状态 预定义素数是否用于媒体加密共享密钥协商
	void SetH235SecurityUserPrimeEnable(bool enable);
	//设置对接收呼叫等消息进行验证检查状态
	void SetH235SecurityCheckIncomingMsgEnable(bool enable);

	//设置安全验证模式可用状态 可设置为所有可用
	//annex D I 消息完整性及密码认证 设置可用状态
	void SetH235SecurityAnnexIEnable(bool enable);

	//annex D IA 仅密码认证 设置可用状态
	void SetH235SecurityAnnexIAEnable(bool enable);

	//annex E II 采用公钥/私钥对的数字签名 设置可用状态
	void SetH235SecurityAnnexIIEnable(bool enable);

	//annex E III 端到端认证 设置可用状态
	void SetH235SecurityAnnexIIIEnable(bool enable);

	//annex F 规程IV 设置逐段转接的安全性 结合了规程I和规程II 可用状态
	void SetH235SecurityAnnexIVEnable(bool enable);

	//媒体流加密 使用长度为512 DH生成7字节共享密钥 若都选择则按最长DH生成共享密钥
	//仅用于DES加密等使用7字节以内的密码加密的算法
	void SetH235SecurityMediaDH512Enable(bool enable);

	//媒体流加密 使用长度为1024 DH生成21字节共享密钥 用于DES 3DES加密等
	void SetH235SecurityMediaDH1024Enable(bool enable);

	//媒体流加密 使用长度为1536 DH生成21字节共享密钥 用于DES 3DES加密等
	void SetH235SecurityMediaDH1536Enable(bool enable);


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

	//H239 辅流令牌控制及辅流收发20121212部分

public:
	XCritSec				m_csMapH239MediaCall;						//令牌响应MAP控制锁
	MapH323MediaCall		m_MapH239MediaCall;							//令牌响应MAP
	bool					m_bTokenUseState;							//令牌在用状态 true为在使用，false为不在使用
	H323MediaCall*			m_pKeepH323MediaCall;						//记录令牌拥有者
	H323MediaCall*			m_pRequestH323MediaCall;					//记录令牌请求者
	//20130322
	H323MediaCall*			m_pVitualH323MediaCall;						//虚拟终端 用于MP自身发辅流

public:
	//检查令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
	bool CheckH239TokenState(H323MediaCall*pH323MediaCall);

	//置令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
	void SetH239TokenState(H323MediaCall*pH323MediaCall,bool bState);

	//置所有的会议室令牌使用状态，bState true为在使用，false为不在使用
	void SetAllH239TokenState(bool bState);

	//添加令牌响应MAP表项，传入CALL指针参数
	void AddH239MediaCallItem(H323MediaCall*pH323MediaCall);

	//删除令牌响应MAP表项，传入CALL指针参数
	void DelH239MediaCallItem(H323MediaCall*pH323MediaCall);

	//清空令牌响应MAP表，传入CALL指针参数
	void RemoveH239MediaCallAll(H323MediaCall*pH323MediaCall);

	//广播令牌请求至所有非令牌拥有者，传入CALL指针参数
	void BroadGenericRequest(H323MediaCall*pH323MediaCall);
	//广播令牌请求至所有非令牌拥有者，传入CALL指针参数
	void OnBroadGenericRequest(H323MediaCall*pH323MediaCall);

	void SendGenericResponse(H323MediaCall*pH323MediaCall,bool bAccept);

	//广播令牌取消至所有非令牌拥有者，传入CALL指针参数
	void BroadGenericCommand(H323MediaCall*pH323MediaCall);

	void BroadGenericIndication(H323MediaCall*pH323MediaCall);

	//广播辅流至所有非令牌拥有者，传入CALL指针参数
	void BroadSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen);

	//收到令牌请求，传入CALL指针参数 type为1 时发响应,默认发,  0时不发(如收到指示后就不需发响应)
	void OnRecvGenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking,int type=1);

	//收到令牌取消，传入CALL指针参数
	void OnRecvGenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	//收到令牌响应，传入CALL指针参数
	void OnRecvGenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID);

	void OnRecvGenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	//收到辅流，传入CALL指针参数
	void OnRecvdRemoteSecondVideoRTPPacket(H323MediaCall*pH323MediaCall,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	//清空删除令牌拥有者相关
	void DelH239TokenKeeper(H323MediaCall*pH323MediaCall);

	//挂断后检查令牌在用状态删除令牌响应MAP表项
	void CheckDelH239MediaCallItem(H323MediaCall*pH323MediaCall);
	//呼通后检查令牌在用状态发送令牌请求
	void CheckSendGenericRequest(H323MediaCall*pH323MediaCall);

	//检查令牌在用状态，传入ConferenceID参数 返回true为在使用，false为不在使用
	bool CheckH239TokenState(std::string & strConfID);

	//置令牌使用状态，传入ConferenceID参数，bState true为在使用，false为不在使用
	void SetH239TokenState(std::string & strConfID,bool bState);

	//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
	void BroadGenericCommand(std::string & strConfID);

	//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
	void BroadGenericRequest(std::string & strConfID);
	//广播令牌请求至所有非令牌拥有者，传入ConferenceID参数
	void OnBroadGenericRequest(std::string & strConfID);

	//清空令牌响应MAP表，传入ConferenceID参数
	void RemoveH239MediaCallAll(std::string & strConfID);

	//收到其它MP发来的令牌使用消息，传入ConferenceID参数,MPID参数
	void OnRecvMPGenericRequest(std::string strConfID,std::string strMPID);

	//收到其它MP发来的令牌不再使用消息，传入ConferenceID参数,MPID参数
	void OnRecvMPGenericCommand(std::string strConfID,std::string strMPID);

	//收到其它MP发来的再广播至所有非令牌拥有者，传入ConferenceID参数
	void OnRecvMPSecondVideoRTPPacket(std::string strConfID,unsigned char*pRTPPacketData,int nRTPPacketLen);

	//挂断后检查呼叫表是否为空 为空则置令牌不在用状态，传入CALL指针参数
	void CheckSetH239TokenState(H323MediaCall*pH323MediaCall);


public:
	bool m_bH239ForceGetToken;
	bool m_bH239ForceGetResp;
	int  m_nH239ControlAgent;
	int  m_nH239MsgType;
	int  m_nH239MediaSend;
	bool m_bH239HasToken;
	bool m_bH239UseToken;
	bool m_bResponseAccept;
	bool m_bMPUseToken;							//MP在用令牌
	bool m_bTermianlUseToken;					//普通终端在用令牌

public:
	void InitH239VirtualCallState(void);
	void GetH239TokenForce(void);
	int  RecvExternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen);
	int  SendInternalH239Media(H323MediaCall*pH323MediaCall,char*pRTPPacketData,int nRTPPacketLen);
	int  OnH239TokenAssign(int nDir);
	int  OnH239TokenRelease(int nDir);
	void RecordH239MsgType(int nType){m_nH239MsgType=nType;};
	void SetH239MediaStatus(int nStatus){m_nH239MediaSend=nStatus;};
	void SetH239TokenUseState(bool bState){m_bH239UseToken=bState;};
	void SetH239TokenOwnState(bool bState){m_bH239HasToken=bState;};
	void SetH239ControlAgent(int nType){m_nH239ControlAgent=nType;};
	int  GetRecordH239MsgType(void){return m_nH239MsgType;};
	int  GetH239MediaStatus(void){return m_nH239MediaSend;};
	bool GetH239TokenUseState(void){return m_bH239UseToken;};
	bool GetH239TokenOwnState(void){return m_bH239HasToken;};
	int  GetH239ControlAgent(void){return m_nH239ControlAgent;};

public:
	//收到令牌请求，传入CALL指针参数
	void OnRecvH239GenericRequest(H323MediaCall*pH323MediaCall, int terLabel, int chID, int symBreaking);
	//收到令牌取消，传入CALL指针参数
	void OnRecvH239GenericCommand(H323MediaCall*pH323MediaCall, int terLabel, int chID);
	//收到令牌响应，传入CALL指针参数
	void OnRecvH239GenericResponse(H323MediaCall*pH323MediaCall, int response, int terLabel, int chID);
	void OnRecvH239GenericIndication(H323MediaCall*pH323MediaCall, int terLabel, int chID);

	void SendH239GenericRequest(H323MediaCall*pH323MediaCall);
	void SendH239GenericResponse(H323MediaCall*pH323MediaCall, bool bAccept);
	void SendH239GenericCommand(H323MediaCall*pH323MediaCall);
	void SendH239GenericIndication(H323MediaCall*pH323MediaCall);

	//检查MP角色令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
	bool CheckMPAgentTokenState(H323MediaCall*pH323MediaCall);
	//检查普通终端角色令牌在用状态，传入CALL指针参数 返回true为在使用，false为不在使用
	bool CheckTerminalAgentTokenState(H323MediaCall*pH323MediaCall);
	//置MP角色令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
	void SetMPAgentTokenState(H323MediaCall*pH323MediaCall,bool bState);
	//置普通角色令牌使用状态，传入CALL指针参数，bState true为在使用，false为不在使用
	void SetTerminalAgentTokenState(H323MediaCall*pH323MediaCall,bool bState);


#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

public:
	typedef std::map<int,H323RegUser*>	MapH323RegUser;
	unsigned long			m_nLastTimestampThread;								//线程中计时
	unsigned long			m_nLastTimestampTimer;								//OnTimer中计时
	unsigned long			m_nLastTimestampCKACT;								//定时间隔3秒,调DoCheckPTZAction
	int						m_nRegGKTimestamp;									//账号注册GK定时扫描间隔时差 s 默认2s

	MapH323RegUser			m_MapH323RegUser;									//注册账号表
	XCritSec				m_csH323RegUser;									//注册账号表加锁

	unsigned short			GetH245BasePort(void);
	int						GetMasterSlaveValue(void);
	void					OutPutStackLog(int nLogType,const char * cszLog,char * szCallID=NULL);
	void					OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);
	int						GetDAPCapPrintEnable(void);
	int						InsideRegisterToGK(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword);
	void					TimerRegisterToGK(const char*cszHost=NULL,int type=0);	//定时注册所有账号 type is 0 default,type is 1 restart

//add adapter
public:
	std::string								m_strProductID;
	std::string								m_strProductVersionID;
	int										m_nT35Code;
	int										m_nT35Extension;
	int										m_nManufacturerCode;
	int										m_nEndpontType;						//普通H323终端设为0,H323MCU设置为1
	int										m_nCapPrint;						//输出能力集信息内容 0不打印 1打印
	int										m_nMasterSlaveValue;				//MCU设置为240或190 ,普通终端设置为50
	int										m_nGateKeeperPort;
	int										m_nAutoManualReg;					//0自动注册,1手动注册
	cmTransportAddress						m_TransportAddr;
	bool									m_nFindGKAddr;
	bool									m_bRunning;
	std::string								m_strMCUID;

protected:
	virtual void							ThreadProcMain(void);

public:

	/******************************************************************************
	*描述：设定注册到GK类型
	*输入：nType	-注册类型 自动查找GK服务器设为0,注册到指定的GK服务器设为非0值
	*****************************************************************************/
	int SetTypeDAPRegisterToGK(int nType=1);

	////设置自动注册还是手动注册 0自动注册,1手动注册
	//int SetTypeDAPAutoManualRegGK(int nType=0);

	/******************************************************************************
	*描述：注册到GK 注册帐号和电话号码至少设置一个 设定自动查找GK时cszHost可设为""或NULL
	*输入：	cszH323ID			-注册帐号
	        cszTelNumber		-电话号码
			cszHost				-注册的GK服务器地址
			cszRegPassword		-注册账号密码
	*返回值：成功返回>=0[account id]，失败返回-1
	*****************************************************************************/
	int DAPRegisterToGK(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword=NULL,int natFwOption=0);

	/******************************************************************************
	* UnRegisterToGK
	*描述：从GK上注销账号
	*输入：	nAccountID			-注册帐号ID
	*返回值：调用成功返回0，失败返回-1
	*****************************************************************************/
	int UnDAPRegisterToGK(int nAccountID);

	int SetDAPH245RangePort(unsigned short nFromPort,unsigned short nToPort);

	int SetDAPProductID(const char*cszProductID);

	int SetDAPProductVersionID(const char*cszVersionID);

	int SetDAPT35CountryCode(int nT35Code=88);

	int SetDAPT35Extension(int nT35Extension=0);

	int SetDAPManufacturerCode(int nManufacturerCode=1);

	//普通H323终端设为0,H323MCU设置为1
	int SetDAPH323EndPointType(int nType=0);

	int SendDAPPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	int SetDAPCapPrintEnable(int nPrint=0);

	//MCU设置为240或190 ,普通终端设置为50
	int SetDAPMasterSlaveValue(int nValue=50);

	int SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);

	int SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetH323CallSupportH239(const char*cszCallID,int nSupportH239);

	void SetUserNonstandardInfo(H323MediaCall *pH323MediaCall,cmCallState_e state,ConfInfo *pConfInfo=NULL,char * cszPacket=NULL);

	void GetCallUserNonstandardInfo(H323MediaCall *pH323MediaCall,ConfInfo *pConfInfo,char * cszPacket);

	int DAPStart(void);
	int DAPStop(void);

	void RegGKSetAutoManual(int type=0);					//0自动注册,1手动注册

	int InsideStart(int fromPort,int toPort);

	const char* InsideMakeCall(const char*cszUserID,int nBandWidth=0,char * cszPacket=NULL,ConfInfo *pConfInfo=NULL);
	int InsideAcceptCall(const char*cszCallID,char * cszPacket=NULL,ConfInfo *pConfInfo=NULL);

	int DAPHangupCall(const char*cszCallID);

	int DAPGatekeeperRequest();

	/******************************************************************************
	*描述：发起呼叫
	*输入：	cszUserID		-对方的用户ID或IP
			nBandWidth		-总带宽值 kbps
			cszPacket		-非标准信息负载
	*****************************************************************************/
	const char* DAPMakeCall(const char*cszUserID,int nBandWidth,char * cszPacket,ConfInfo *pConfInfo);

	/******************************************************************************
	* AcceptCall
	*描述：接受呼叫请求
	*输入：	cszCallID		-呼叫标识
			cszPacket		-非标准信息负载
	*****************************************************************************/
	int DAPAcceptCall(const char*cszCallID,char * cszPacket,ConfInfo *pConfInfo);


	/******************************************************************************
	* SetLocalMainVideoH264Level
	*描述：设置本地主流视频H264能力使用的最大Level值 如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	cszCallID		-呼叫标识
			nLevel			-本地主视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoH264Level(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264Level
	*描述：设置本地辅流视频H264能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	cszCallID		-呼叫标识
			nLevel			-本地辅流视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalMainVideoH264HPLevel
	*描述：设置本地主流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	cszCallID		-呼叫标识
			nLevel			-本地主视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetLocalSecondVideoH264HPLevel
	*描述：设置本地辅流视频H264HighProfile能力使用的最大Level值  如有设置在呼叫加入能力集时会优先使用此Level值
	*输入：	cszCallID		-呼叫标识
			nLevel			-本地辅流视频Level值
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* SetCallRTPMediaMulticast 
	*描述：设置RTP 媒体流组播 如有设置组播IP则会启用组播
	*输入：	cszCallID		-呼叫标识
			cszMulticastIP	-组播IP
			ttl				-组播数据包的TTL(生存时间)
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl=1);


	/******************************************************************************
	*描述：发送音频媒体流
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			RTPPacketData		-RTP包
			PacketDataLen		-RTP包长度
	*****************************************************************************/
	int DAPSendCallAudio(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*描述：发送主流视频媒体流
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			RTPPacketData		-RTP包
			PacketDataLen		-RTP包长度
	*****************************************************************************/
	int DAPSendCallMainVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*描述：发送辅流视频媒体流
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			RTPPacketData		-RTP包
			PacketDataLen		-RTP包长度
	*****************************************************************************/
	int DAPSendCallDualVideo(HDAPAPPCALL hdapCall,const char*cszCallID,char* RTPPacketData, int PacketDataLen);

	/******************************************************************************
	*描述：发送PTZ【发送云台控制命令】
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*****************************************************************************/
	int DAPSendPTZControl (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	*描述：发送DTMF二次拨号按键值 注此也可在外面实现{ AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);} 内部实现不再调AUDEC_GetEngine
	*输入：	cszCallID			-呼叫标识
			dtmf				-按键值
	*****************************************************************************/
	int DAPSendCallDTMF(const char*cszCallID,const char dtmf);

	/******************************************************************************
	*描述：向对端发送H245非标消息
	*输入：	cszCallID		-呼叫标识
			strPacket		-非标准信息负载
	*****************************************************************************/
	int DAPSendCallH245NonstandardData(const char*cszCallID,std::string strPacket);

	/******************************************************************************
	* SendCallH245ControlMessage
	*描述：发送H245控制消息 包含flowControlIndication H239令牌相关 会控消息
	*输入：	
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int DAPSendCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);


	/******************************************************************************
	* SendRequestMainVideoKeyFrame
	*描述：发送主流视频关键帧请求消息
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendRequestMainVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SendRequestSecondVideoKeyFrame
	*描述：发送辅流视频关键帧请求消息
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendRequestSecondVideoKeyFrame(const char*cszCallID);

	/******************************************************************************
	* SendCallChannelMessage
	*描述：发送逻辑通道消息 包含FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
	*输入：	cszCallID			-呼叫标识
			nMsgType			-控制消息类型
			cszPacket			-消息包 含有媒体类型字段 收发方向字段
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* SendRASMessage
	*描述：发送RAS消息 nMsgType 重定义
	*输入：	nMsgType			-消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SendRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	* GetCallQ931TransportAddr
	*描述：获取呼叫Q931通信IP地址信息
	*输入：	cszCallID			-呼叫标识
			szAddr				-IP地址字符串Buffer
			nPort				-通信端口
			nLen				-IP地址字符串长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetCallQ931TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen);

	/******************************************************************************
	* GetCallH245TransportAddr
	*描述：获取呼叫H245通信IP地址信息
	*输入：	cszCallID			-呼叫标识
			szAddr				-IP地址字符串Buffer
			nPort				-通信端口
			nLen				-IP地址字符串长度
	*输出： 无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetCallH245TransportAddr(const char*cszCallID,char * szAddr,int &nPort,int &nLen);

	void SetMCUID(const char*cszMCUID);

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

	int OpenDualVideoOutChannel(const char*cszCallID);

	int OpenFECCOutChannel(const char*cszCallID);

	int CloseDualVideoOutChannel(const char*cszCallID);

	int CloseFECCOutChannel(const char*cszCallID);

	int AudioRecvChannelAnswer(const char*cszCallID);

	int MainVideoRecvChannelAnswer(const char*cszCallID);

	int DualVideoRecvChannelAnswer(const char*cszCallID);

	int FECCRecvChannelAnswer(const char*cszCallID);

	int AudioRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int MainVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int DualVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

	int FECCRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason);

#endif

	int DAPGetCallSupportSecondVideo(const char*cszCallID);

public:

	/******************************************************************************
	//*描述：收到对端音频发送通道打开事件
	//*输入：	strCallID			-呼叫标识
	//		AudioCodecID		-音频CodecID
	//		nPayloadType		-PayloadType
	//*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelOpen(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);

	///******************************************************************************
	//*描述：收到对端主流视频发送通道打开事件
	//*输入：	strCallID			-呼叫标识
	//		codecType			-视频CodecID
	//		nBitrate			-码率
	//		nPayloadType		-PayloadType
	//		width				-分辨率宽度像素数 可选
	//		nHeight				-分辨率高度像素数 可选
	//		nLevel				-分辨率实际H264level值 可选
	//*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth=0,int nHeight=0,int nLevel=0);

	///******************************************************************************
	//*描述：收到对端辅流视频发送通道打开事件
	//*输入：	strCallID			-呼叫标识
	//		codecType			-视频CodecID
	//		nBitrate			-码率
	//		nPayloadType		-PayloadType
	//		width				-分辨率宽度像素数 可选
	//		nHeight				-分辨率高度像素数 可选
	//		nLevel				-分辨率实际H264level值 可选
	//*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelOpen(const std::string& strCallID,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,int nWidth=0,int nHeight=0,int nLevel=0);

	///******************************************************************************
	//* OnDAP_CallFECCRecvChannelOpen
	//*描述：收到对端FECC控制发送通道打开事件
	//*输入：	strCallID			-呼叫标识
	//			nPayloadType		-PayloadType
	//*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelOpen(const std::string& strCallID,int nPayloadType);

	/******************************************************************************
	*描述：音频接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelConnected(const std::string& strCallID,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	*描述：音频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallAudioRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelConnected(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*描述：主流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelConnected(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*描述：辅流视频接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*描述：FECC控制接收通道连接成功事件
	*输入：	cszCallID			-呼叫标识
			nPayloadType		-PayloadType
			ip					-本端接收rtp ip
			port				-本端接收rtp port
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelConnected(const std::string& strCallID ,int nPayloadType,unsigned long ip
		, int port);

	/******************************************************************************
	*描述：FECC控制接收通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallFECCRecvChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*描述：音频发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			AudioCodecID		-音频CodecID
			nPayloadType		-PayloadType
			ip					-对端rtp ip
			port				-对端rtp port
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,X_AUDIO_CODEC_ID AudioCodecID
		,int nPayloadType,unsigned long ip, int port);

	/******************************************************************************
	*描述：音频发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallAudioSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip,int port,int nWidth,int nHeight,int nLevel = 0);

	/******************************************************************************
	*描述：主流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallMainVideoSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
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
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel = 0);
	
	/******************************************************************************
	*描述：辅流视频发送通道连接成功事件
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallSecondVideoSendChannelDestroyed(const std::string& strCallID);

	/******************************************************************************
	*描述：FECC控制发送通道连接成功事件
	*输入：	HDAPAPPCALL hdapCall 实体call句柄
			cszCallID			-呼叫标识
			ip					-对端rtp ip
			port				-对端rtp port
			nPayloadType		-PayloadType
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelConnected(HDAPAPPCALL hdapCall,const std::string& strCallID,int nPayloadType
		,unsigned long ip, int port);

	/******************************************************************************
	* OnDAP_CallFECCSendChannelDestroyed
	*描述：FECC控制发送通道关闭事件
	*输入：	cszCallID			-呼叫标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallFECCSendChannelDestroyed(const std::string& strCallID);



	/******************************************************************************
	*描述：接收到音频媒体流回调
	*输入：	cszCallID			-呼叫标识
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,X_AUDIO_CODEC_ID codecID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	*描述：接收到主流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
	/******************************************************************************
	*描述：接收到辅流视频媒体流回调
	*输入：	cszCallID			-呼叫标识
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,VIDEC_CODEC_TYPE codecType,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

	/******************************************************************************
	*描述：接收到主流视频关键帧请求消息
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_RecvMainVideoRequestKeyFrame(const char*cszCallID);

	/******************************************************************************
	*描述：接收到辅流视频关键帧请求消息
	*输入：	cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_RecvSecondVideoRequestKeyFrame(const char*cszCallID);

	/******************************************************************************
	*描述：接收到H245控制消息回调 包含flowControlIndication H239令牌相关 会控消息
	*输入：	cszCallID			-呼叫标识
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*****************************************************************************/
	virtual void OnDAP_RecvCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*描述：接收到接收逻辑通道消息回调 包含FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
	*输入：	cszCallID			-呼叫标识
			nMsgType			-控制消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*****************************************************************************/
	virtual void OnDAP_RecvCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*描述：接收到RAS消息回调 挂接RAS消息
	*输入：	nMsgType			-消息类型
			cszPacket			-消息包
			nPacketLen			-消息包长度
	*****************************************************************************/
	virtual void OnDAP_RecvRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen);

	/******************************************************************************
	*描述：协议栈日志输出回调
	*输入：	nLogType			-日志类型 0 正常日志 1 错误日志 
			cszLog				-日志内容
			nLen				-日志内容长度
	*****************************************************************************/
	virtual void OnDAP_OutPutStackLog(int nLogType,const char * cszLog,int nLen);

	/******************************************************************************
	*描述：能力集及通道能力集信令内容输出回调 输出需设置开关
	*输入：	cszCallID			-呼叫标识
			nCapType			-能力信令类型 0 远端能力集内容 1 本端能力集内容 2 远端通道通力集内容 3 本端通道能力集内容
			cszContent			-信令内容
			nLen				-信令内容长度
	*****************************************************************************/
	virtual void OnDAP_OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);

	/******************************************************************************
	*描述：接收到PTZ事件，当对方发送云台控制命令时，系统会调用此事件
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*****************************************************************************/
	virtual void OnDAP_CallRecvdPTZControl(const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);


	/******************************************************************************
	*描述：呼入事件，有终端呼叫本机的时候系统会调用此事件
	*输入：	strCallID		-呼叫标识
			strUserID		-用户标识或用户IP
			strUserName		-用户名称
			strPacket		-非标准信息负载
			strTelNum		-注册网守号码 可用于指示会议号 未注册网守时为空串
	*****************************************************************************/
	virtual void OnDAP_CallRingIn(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strE164ID,const std::string& strPacket,std::string & strTelNum);

	/******************************************************************************
	*描述：呼出事件，本机发起呼叫成功后系统会调用此事件
	*输入：	strCallID		-呼叫标识
			strUserID		-用户标识或用户IP
			strUserName		-用户名称
	*****************************************************************************/
	virtual void OnDAP_CallRingOut(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName);

	/******************************************************************************
	*描述：呼叫建立事件，当双方建立通话后【本机接受了远端的呼叫请求或远端接受了本机的呼叫请求】系统会调用此事件
	*输入：	strCallID		-呼叫标识
			strUserID		-用户标识或用户IP
			strUserName		-用户名称
			strPacket		-对端非标准信息负载
	*****************************************************************************/
	virtual void OnDAP_CallEstablished(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,const std::string& strPacket);


	/******************************************************************************
	*描述：呼叫销毁事件，当通话结束后【本机挂断或远端挂断】系统会调用此事件
	*输入：	strCallID		-呼叫标识
			strUserID		-用户标识或用户IP
			strUserName		-用户名称
			strReason		-结束原因
	*****************************************************************************/
	virtual void OnDAP_CallDestroyed(const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,IMXEC_CALL_END_REASON nReason,const std::string& strReason );

	/******************************************************************************
	* OnDAP_CallH235CapSet
	*描述：添加能力集，当有呼叫进来时中系统会先于OnCallRingIn调用此事件, 可在函数中设置是否启用媒体流加密
	*输入：	cszCallID		-呼叫标识
			cszE164			-呼叫的E164号 没有值时为空串
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void OnDAP_CallH235CapSet(H323MediaCall*pH323MediaCall,const char * cszE164,ConfMediaSet & h235Set);


	/******************************************************************************
	*描述：接收到有终端发来H245非标消息时系统会调用此事件
	*输入：	strCallID		-呼叫标识
			strPacket		-非标准信息负载
	*****************************************************************************/
	virtual void OnDAP_RecvNonstandardData(const std::string& strCallID,const std::string& strPacket);


	/******************************************************************************
	*描述：H323注册成功事件
	*输入：	cszH323ID			-注册帐号
			cszTelNumber		-电话号码
	*****************************************************************************/
	virtual void OnDAP_H323RegistrationSuccess(const char*cszH323ID,const char*cszTelNumber);

	/******************************************************************************
	*描述：H323注册失败事件
	*输入：	cszH323ID			-注册帐号
			cszTelNumber		-电话号码
			cszReason			-失败原因
	*****************************************************************************/
	virtual void OnDAP_H323RegistrationError(const char*cszH323ID,const char*cszTelNumber,char* cszReason=NULL);


#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)


	/******************************************************************************
	* AddCapAudioAll
	*描述：添加音频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapAudioAll
	*描述：移除音频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapAudioAll(const char*cszCallID);

	/******************************************************************************
	* AddCapG711A_64K 注,各媒体类型能力集项先添加者优先
	*描述：添加音频G711A能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG711A_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapG711U_64K
	*描述：添加音频G711U能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG711U_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG722_64K
	*描述：添加音频G722-64K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG722_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221_24K
	*描述：添加音频G7221-24K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221_24K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221_32K
	*描述：添加音频G7221-32K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_24K
	*描述：添加音频G7221C-24K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_24K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_32K
	*描述：添加音频G7221C-32K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG7221C_48K
	*描述：添加音频G7221C-48K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG7221C_48K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG729A
	*描述：添加音频G729A能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG729A(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_32K
	*描述：添加音频G719-32K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_32K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_48K
	*描述：添加音频G719-48K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_48K(const char*cszCallID);

	/******************************************************************************
	* AddCapAudioG719_64K
	*描述：添加音频G719_64K能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapAudioG719_64K(const char*cszCallID);

	/******************************************************************************
	* AddCapVideoAll
	*描述：添加主流视频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapVideoAll
	*描述：移除主流视频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapVideoAll(const char*cszCallID);

	/******************************************************************************
	* AddCapH263
	*描述：添加主流视频H263能力集
	*输入：	cszCallID		-呼叫标识
			nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
			nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
			nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263(const char*cszCallID,int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapVideoH263Plus
	*描述：添加主流视频H263+能力集
	*输入：	cszCallID		-呼叫标识
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263Plus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH263PlusPlus
	*描述：添加主流视频H263++能力集
	*输入：	cszCallID		-呼叫标识
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH263PlusPlus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapVideoH264QCIF
	*描述：添加主流视频的H264QCIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264QCIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264CIF
	*描述：添加主流视频的H264CIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2644CIF
	*描述：添加主流视频的H264 4CIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH2644CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264720P
	*描述：添加主流视频的H264720P能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH2641080P
	*描述：添加主流视频的H2641080P能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH2641080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP720P
	*描述：添加主流视频的H264HighProfile能力集,默认最高720P
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264HP720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapVideoH264HP1080P
	*描述：添加主流视频的H264HighProfile能力集,默认最高1080P
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapVideoH264HP1080P(const char*cszCallID,int nLevel=0);


	/******************************************************************************
	* AddCapDualVideoAll
	*描述：添加辅流视频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapDualVideoAll
	*描述：移除辅流视频的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapDualVideoAll(const char*cszCallID);

	/******************************************************************************
	* AddCapDualVideoH263
	*描述：添加辅流视频H263能力集
	*输入：	cszCallID		-呼叫标识
			nHasQCIF		-使用QCIF分辨率 不使用设置为0，使用设置为1
			nHasCIF			-使用CIF分辨率 不使用设置为0，使用设置为1
			nHas4CIF		-使用4CIF分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263(const char*cszCallID,int nHasQCIF=0,int nHasCIF=0,int nHas4CIF=0);

	/******************************************************************************
	* AddCapDualVideoH263Plus
	*描述：添加辅流视频H263+能力集
	*输入：	cszCallID		-呼叫标识
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263Plus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH263PlusPlus
	*描述：添加辅流视频H263++能力集
	*输入：	cszCallID		-呼叫标识
			nHasVGA			-使用VGA分辨率 不使用设置为0，使用设置为1
			nHasSVGA		-使用SVGA分辨率 不使用设置为0，使用设置为1
			nHasXGA			-使用XGA分辨率 不使用设置为0，使用设置为1
			nHasSXGA		-使用SXGA分辨率 不使用设置为0，使用设置为1
			nHas720P		-使用720P分辨率 不使用设置为0，使用设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH263PlusPlus(const char*cszCallID,int nHasVGA=0,int nHasSVGA=0,int nHasXGA=0,int nHasSXGA=0,int nHas720P=0);

	/******************************************************************************
	* AddCapDualVideoH264QCIF
	*描述：添加辅流视频的H264QCIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264QCIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264CIF
	*描述：添加辅流视频的H264CIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2644CIF
	*描述：添加辅流视频的H264 4CIF能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH2644CIF(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264720P
	*描述：添加辅流视频的H264720P能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH2641080P
	*描述：添加辅流视频的H2641080P能力集
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此分辨率对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH2641080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP720P
	*描述：添加辅流视频的H264HighProfile能力集,默认最高720P
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264HP720P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* AddCapDualVideoH264HP1080P
	*描述：添加辅流视频的H264HighProfile能力集,默认最高1080P
	*输入：	cszCallID	-呼叫标识
			nLevel		-设定此能力对应的H264level值,当nLevel大于0时使用此值,可选
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapDualVideoH264HP1080P(const char*cszCallID,int nLevel=0);

	/******************************************************************************
	* RemoveCapAll
	*描述：移除所有媒体类型的所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapAll(const char*cszCallID);

	/******************************************************************************
	* RemoveCapFECCDataAll
	*描述：移除FECC控制所有能力集
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int RemoveCapFECCDataAll(const char*cszCallID);

	/******************************************************************************
	* AddCapFECCData
	*描述：添加FECC控制能力集
	*输入：	cszCallID	-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int AddCapFECCData(const char*cszCallID);
#endif

private:
	void SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia);
	void SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall=NULL);



#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2)

public:
	void StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopVideoToCall(const char*cszCallID);
	void StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopH239ToCall(const char*cszCallID);
	void StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopAudioToCall(const char*cszCallID);

#endif

public:
	unsigned long	m_nLastSendTripTimestamp;
	std::string		m_strProName;
	int				m_nRegAccountNum;
private:
	void DoCheckRoundTrip(void);
	void TimerSendRoundTripDalay(H323MediaCall * pH323MediaCall,int type=1);
	bool TimerCheckRoundTripDalay(H323MediaCall* pH323MediaCall,int type=1);  ////0 terminal use, 1 mp use or mcu use

public:
	int HangupCallPointer(H323MediaCall*pH323MediaCall,int type =1); //type 1 is check media hungup , 0 is terminal ras URQ hungup ,2 拔网线再插上挂断
	H323RegUser* FindH323RegAccount(std::string strID);
	int AddH323RegAccount(cmRASTransaction rasTransact,const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword,int natFwOption);
	void RemoveH323RegAccount(int nLineID);

	//A7 add
public:
	virtual void OnMainVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate);
	virtual void OnSecondVideoFlowControlCommand(const std::string&strCallID, unsigned int nBitRate);
	// 允许对端发送双流
	void AllowToSendSecondVideo(const char*cszCallID, bool bAllow);
	// 发流控命令
	void SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate);
	//终端用,是否启用H460 NAT穿透, 0 not use ,1 use,default 0
	void SetH460NATFwOption(int option);
	void SendChannelKeepAlive(void);
	void HungupAll(void);
	void OnDAP_CallDestroyed(H323MediaCall*pH323MediaCall);

public:

	HH323FWNATMGR		m_hFwNat;					//add for 460client
	int					m_nUseTerminalFWNAT;		//终端用,是否启用H460 NAT穿透, 0 not use ,1 use,default 0
	int					m_nUseMCUFWNAT;				//MCU用,是否默认发送打洞包,  0 not use ,1 use,default 1
	int					m_nSendKeepliveSeconds;		//NAT通道定时保持时间 默认10秒
	unsigned long		m_nSendKeepliveTimestamp;
	int					m_nRegStatusTimestamp;		//终端用 账号注册结果时间计时
	int					m_nRegGKPort;		        //终端用
	int					m_nTimeToLive;		        //终端用 s
	bool				m_bRegToGKStateOld;         //终端用
	int                 m_nRASCastPort;
	int                 m_nRASLocalPort;
	std::string         m_strLastGKHost;            //MCU用 保存上次设置的GK地址
	int 				m_nSendURQ;                 //终端用 重启协议栈后控制不允许发URQ的次数
	int 				m_nMaxCallNum;              //MCU用  最大呼叫在线数
	//int 				m_nIPCPort;
	int                 m_nDoCheckPTZCount;
	int                 m_nCheckNetCardDisable;     //检测网卡掉线后的次数
	HWND                m_hTestWnd;                 //use test
	std::string         m_strH225LogicalKA;         //H460 H225通道保持
	int                 m_bRoundTripDiscMaxCount;   //挂断后检测没收到回环信令消息的最大次数

public:

	MediaPortRelease    m_MediaPortRelease;
	int                 m_nREGRejectReason;         //终端用 保存注册失败原因码

	void SaveMediaReleasePort(unsigned short rtpPort);
	void OnCallH245CapChanged (const std::string& strCallID,const std::string& strUserID,const std::string& strUserName,unsigned int nMediaType);

	//获取H323ID 终端用 获取到返回H323ID长度，否则返回NULL
	int GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen);

	//获取E164ID 终端用 获取到返回E164ID长度，否则返回NULL
	int GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen);

	//设置H323注册相关端口
	int SetH323RASPort(unsigned short nLocalPort,unsigned short nRemotePort,unsigned short nCastPort);

	void H323SettingFromINI();
	void DAPCheckRasModule(const char*cszHost);
	//终端用 STOP后检测控制发送URQ仅一次
	RvBool CheckRASURQSend();
	void SetMediaPortArrayEmpty();
	bool H323CallMgr::TimerCheckNetCardDisable(H323MediaCall * pH323MediaCall,int type=1);

	//1016 add
	DNSIP m_DNSIP;
	bool IsDNS(const char * strDNS);
	bool GetDNSIPPart1(const char * strDNS,char * DNSIP,int & nLen);
	bool GetDNSIP(const char * strDNS,char * DNSIP,int & nLen);
	void SaveToDNSIP(char * strDNS,char * strIP);
	bool FindDNSIP(char * strDNS,char * strIP);
	//回调挂断状态时再检测下是不是因为网络收不到媒体流原因挂断的
	bool DisconnectCheckRoundTripDalay(H323MediaCall * pH323MediaCall);

	void  OnCallRTPChanged(const std::string& strCallID,H323Channel * pChannel,unsigned int nType);
	//以下几个函数处理H323函数在其它线程中调用会有内存泄漏情况
#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 2) 
#else 
	DWORD m_dwThreadID;
#endif

	bool  m_bThreadH323Attach;
	int   m_nCallerRate;            //主叫呼叫码率 用于MCU级联特殊情况测试
	int   m_nTerminalCapsType;      //1 use terminal caps ,2 use mcu confcaps
	int   m_nUseMediaNetRTP;        //1 use medianet rtp  ,0 not use 
	int   m_nSendFlowCC;            //1 send flowControlCommand  ,0 not send 
	bool  m_bNETECNodeToGK;         //NETEC node connect to gk NETEC MCU
	int   m_nNETECNodeCount;        //NETECNode Create Count

	void  OnCallKeyFrameRequest(const std::string& strCallID,unsigned nType);  //nType 1 main 2 dualvideo

public:
	void  AddQ931CallPartyNumber(HCALL hsCall,const char * cszUserID,cmCallParam callParam,char * cszPartyNumber);
	int   GetH323IDAlias(char * szH323ID,cmAlias & termAlias,char * bmpStr);
	int   GetE164IDAlias(char * szTelNumber,cmAlias & termAlias,char * bmpStr);

	bool  CheckMapCallEmpty(void);
	//mcu use ,support dtmf ,传入真正的会议能力
	int   DapChanegCallCaps(const char*cszCallID,ConfInfo *pConfInfo);
	void  SetMCUInfo();
	//gk mcu use 20150409 
	void  NETECStopToGK();
	void  NETECStartToGK(void);
	int   SetBandWidth1(ConfInfo * pConInfo);
	int   SetBandWidth2(ConfInfo * pConInfo);

};




#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


#if (_MSC_VER && _MSC_VER<=1200) || (__BORLANDC__) 
#define __FUNCTION__ ""
#endif

//sprintf(strDebugMsg,"File %s,Func %s,Line %d .\n",__FILE__,__FUNCTION__,__LINE__); \
//sprintf(strDebugMsg,"File %s,Func %s,Line %d,Debug %s .\n",__FILE__,__FUNCTION__,__LINE__,(char *)strDebugINFO); \

#if 1
#define PrintDebugString() \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d .\n",__FUNCTION__,__LINE__); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo(strDebugINFO) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo2(strDebugINFO1,strDebugINFO2) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s,%s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO1,(char *)strDebugINFO2); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#define PrintDebugStringInfo3(strDebugINFO1,strDebugINFO2,strDebugINFO3) \
{ \
	char strDebugMsg[1024]; \
	sprintf(strDebugMsg,"Func %s,Line %d,Debug %s,%s,%s .\n",__FUNCTION__,__LINE__,(char *)strDebugINFO1,(char *)strDebugINFO2,(char *)strDebugINFO3); \
	OutputDebugString(strDebugMsg); \
	LogFile(NULL,NULL,"","",strDebugMsg); \
}

#else

#define PrintDebugString()

#define PrintDebugStringInfo(strDebugINFO) \
{ \
}

#define PrintDebugStringInfo2(strDebugINFO1,strDebugINFO2) \
{ \
}

#define PrintDebugStringInfo3(strDebugINFO1,strDebugINFO2,strDebugINFO3) \
{ \
}

#endif	//#if 1

#endif //#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)


#endif	//#ifndef __H323CallMgr_H__
