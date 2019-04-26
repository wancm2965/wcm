#ifndef __SIPCALLMGR_H__
#define __SIPCALLMGR_H__

#include "../DefineHeader/SIPDefineHeader.h"
#include "SIPUser.h"
#include "XThreadBase.h"
#include "pjsip.h" 
#include "SIPCallCapability.h"
#include "XBufferPool.h"
#include "StrPacket.h"
#include "CallMgr.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

#include "SIPDAPCall.h"
#else
#include "SIPMediaCall.h"
#endif

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>
#include <AVCONMP/IMXEC_Call.h>

#else

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)
#include <IMXEC/IMXEC_DAPSIP.h>
#else
#include <IMXEC/IMXEC_SIP.h>
#endif

#include <IMXEC/IMXEC_Call.h>
#endif


#include <vector>
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>





#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

class SIPCallMgr 
	: public XBufferPool
	, public XThreadBase

#else
class SIPCallMgr 
	: public XBufferPool

#endif

{
public:
	SIPCallMgr(void);
	virtual ~SIPCallMgr(void);
public:
	static SIPCallMgr&Instance(void);

public:
	int m_nLogModuleEnabled ;

public:
	//输出SIP文本信息
	void StackDisplayMsg(char * msg,int type=0);//type=1 system msg
	SipConfigStruct					m_sipConfig;
	SIPCallCapability				m_sdpCap;
	XCritSec						m_csSdpCap;

	SIPMediaCall*  GetSIPMediaCallFromCallID(const char*cszCallID);
	std::string GetHostAddress(void);
	//use to ip call, strHost 仅用于未注册情况 IP呼叫时
	SIPMediaCall* CreateSIPMediaCall(SIPUser * pSIPUser,std::string callID,std::string strHost) ;

	void OnUserIdRegistration(int nAccountId,int status,const std::string&strErrorCode);
	SIPUser * GetSIPUserFromAccountID(int accountID) ;

public:
	/******************************************************************************
	* AddVirtualLine
	*描述：添加虚拟线路
	*输入：	cszUserID		-用户帐号
			cszUserName		-用户名称
			cszPassword		-用户密码
			cszHost			-SIP代理服务器
	*输出：无
	*返回值：成功返回>=0[line id]，失败返回-1
	*****************************************************************************/
	virtual int AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID,const char*cszDomain);

	/******************************************************************************
	* RemoveVirtualLine
	*描述：删除虚拟线路
	*输入：	nLineID			-线路标识
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void RemoveVirtualLine(int nLineID);

	virtual int SetCurrentVirtualLine(int nLineID);
	virtual int GetCurrentVirtualLine(void);
	virtual bool HasVirtualLineRegistrationSuccess(void);
	virtual int IsVirtualLineRegistrationSuccess(int nLineID);
	virtual int IsCurrentVirtualLineRegistrationSuccess(void);
	virtual int IsVirtualLineRegistrationError(int nLineID);
	virtual int IsCurrentVirtualLineRegistrationError(void);
	virtual const char*GetVirtualLineRegistrationErrorCode(int nLineID);
	virtual const char*GetCurrentVirtualLineRegistrationErrorCode(void);

	/******************************************************************************
	* SetSIPListenPort
	*描述：设置侦听端口
	*输入：	nSIPListenPort			-侦听端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSIPListenPort(unsigned short nSIPListenPort);
	virtual unsigned short GetSIPListenPort(void);

	/******************************************************************************
	* SetLocalIP
	*描述：设置注册帐号
	*输入：	cszSIPID		-注册帐号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetLocalIP(const char*cszLocalIP);
	std::string m_strLocalIP;
	std::string GetLocalIP();
	/******************************************************************************
	* SetRTPBasePort
	*描述：设置侦听端口
	*输入：	nRTPBasePort			-RTP开始端口
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetRTPBasePort(unsigned short nRTPBasePort);
	virtual unsigned short GetRTPBasePort(void);

	/******************************************************************************
	* SetAudioDevParam
	*描述：设置主流视频设备参数
	*输入： nAudioCodecID		-音频编码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID);
	virtual X_AUDIO_CODEC_ID GetAudioCodecID(void){return m_nAudioCodecID;};

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
	virtual int SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual int GetMainVideoDevID(void){return m_nMainVideoDevID;};
	virtual int GetMainVideoFrameRate(void){return m_nMainVideoFrameRate;};
	virtual int GetMainVideoBitrate(void){return m_nMainVideoBitrate;};
	virtual VIDEC_CODEC_TYPE GetMainVideoCodecType(void){return m_nMainVideoCodecType;};
	virtual VIDEO_FORMAT_TYPE GetMainVideoFormatType(void){return m_nMainVideoFormatType;};
    virtual int GetMainVideoNetworkType(void){return m_nMainVideoNetworkType;};


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
	virtual int SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual int GetSecondVideoDevID(void){return m_nSecondVideoDevID;}; 
	virtual int GetSecondVideoDevFrameRate(void){return m_nSecondVideoDevFrameRate;};
	virtual int GetSecondVideoMaxBitrate(void){return m_nSecondVideoMaxBitrate;};
	virtual VIDEC_CODEC_TYPE GetSecondVideoCodecType(void){return m_nSecondVideoCodecType;};
	virtual VIDEO_FORMAT_TYPE GetSecondVideoFormatType(void){return m_nSecondVideoFormatType;};
    virtual int GetSecondVideoNetworkType(void){return m_nSecondVideoNetworkType;};

	/******************************************************************************
	* SetSupportH239
	*描述：设置是否支持H239
	*输入：nSupportH239		-是否支持H239
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSupportH239(int nSupportH239);
	int GetSupportH239(void){return m_nSupportH239;};
	int						m_nSupportH239;

	/******************************************************************************
	* GetSecondVideoDirection
	*描述：获取辅流的方向
	*输入：	cszCallID		-呼叫标识
	*输出： nDir			-辅流的方向
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir);
	
	/******************************************************************************
	* Start
	*描述：启动SIP服务
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int Start(void);

	/******************************************************************************
	* Stop
	*描述：停止SIP服务
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int Stop(void);


	virtual bool GetCallSDP(const std::string&strCallID,std::string&strLocalIP,unsigned short&nLocalAudioPort,unsigned short&nLocalVideoPort);
	virtual void OnMediaUpdate(const std::string&strCallID,const std::string&strRemoteIP,unsigned short nRemoteAudioPort,int nAudioPayloadType,const std::string&strAudioPayloadName,int nAudioSampleRate,X_AUDIO_CODEC_ID AudioCodecID,unsigned short nRemoteVideoPort,int nVideoPayloadType,const std::string&strVideoPayloadName,int nVideoBitrate,int nVideoWidth,int nVideoHeight);


	/******************************************************************************
	* MakeCall
	*描述：发起呼叫
	*输入：	cszUserID		-对方的用户ID或IP
	*输出：无
	*返回值：呼叫标识
	*****************************************************************************/
	virtual const char*MakeCall(const char*cszUserID,ConfInfo *pConfInfo=NULL);

	void AddCall(SIPMediaCall*pSIPMediaCall);

	/******************************************************************************
	* AcceptCall
	*描述：接受呼叫请求
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int AcceptCall(const char*cszCallID,ConfInfo *pConfInfo=NULL);

	/******************************************************************************
	* HangupCall
	*描述：挂断呼叫
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int HangupCall(const char*cszCallID);

	/******************************************************************************
	* SendPTZ
	*描述：发送PTZ【发送云台控制命令】
	*输入：	cszCallID		-呼叫标识
			nPTZAction		-云台动作
			nSpeed			-动作的速度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed);

	/******************************************************************************
	* StartSecondVideo
	*描述：开始辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartSecondVideo (const char*cszCallID);

	/******************************************************************************
	* StopSecondVideo
	*描述：停止辅流
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StopSecondVideo (const char*cszCallID);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SetLocalMainVideoWnd
	*描述：设置本地主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-本地主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*描述：设置本地主流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*描述：设置本地主流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

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
	virtual void SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);


#if defined(HAS_SIP_AVCON_MP) && ( (HAS_SIP_AVCON_MP == 1) || (HAS_SIP_AVCON_MP == 2) )

#else

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*描述：设置远程主流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SetSecondVideoWnd
	*描述：设置辅流视频显示的窗口
	*输入：	cszCallID		-呼叫标识
			hWnd			-远程辅流视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(const char*cszCallID,HWND hWnd);

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
	virtual int SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy);

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
	virtual void SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*描述：设置辅流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(const char*cszCallID,int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*描述：设置辅流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

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
	virtual void SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));


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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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
	virtual int GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetLocalAudioLevel
	*描述：获取本地音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*描述：获取本地音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*描述：获取本地音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*描述：获取本地音频编码信息
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

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
	virtual int GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetRemoteAudioLevel
	*描述：获取远程音频Level【实际声音大小】
	*输入：	cszCallID		-呼叫标识
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(const char*cszCallID,int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*描述：获取远程音频音量
	*输入：	cszCallID		-呼叫标识
			nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(const char*cszCallID,int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*描述：获取远程音频是否静音
	*输入：	cszCallID		-呼叫标识
			nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteAudioMute(const char*cszCallID,int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*描述：获取远端音频编码
	*输入：	cszCallID		-呼叫标识
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID);

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
	virtual int GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* GetCallSupportSecondVideo
	*描述：获取呼叫是否支持辅流【H239】
	*输入：	cszCallID		-呼叫标识
	*输出：	无
	*返回值：支持返回非0，不支持返回0
	*****************************************************************************/
	virtual int GetCallSupportSecondVideo(const char*cszCallID);

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

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

#endif //#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*描述：保存本地主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*描述：保存远程主流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*描述：保存辅流视频到BMP图片文件
	*输入：	cszCallID		-呼叫标识
			cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName);

	/******************************************************************************
	* SendDTMF
	*描述：发送DTMF
	*输入：	cszCallID		-呼叫标识
			nDTMF			-DTMF
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SendDTMF(const char*cszCallID,const char nDTMF);

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
	* GetHasVideo
	*描述：获取是否只有视频
	*输入：cszCallID		-呼叫标识
	*输出：无
	*返回值：如果有视频，那么返回非0，否则返回0
	*****************************************************************************/
	int GetHasVideo(const char*cszCallID);
	
	bool ExcludePlayChan(unsigned long nAudioID,AUDEC_PlayChan*pPlayChan);
public:
	virtual void OnRegistrationSuccess(int nLineID,std::string strUserID);
	virtual void OnRegistrationError(int nLineID,const std::string&strErrorCode,std::string strUserID);
	virtual void OnSIPMediaCallCreated (SIPMediaCall*pSIPMediaCall);
	virtual void OnCallRingIn (const std::string& strCallID);
	virtual void OnCallRingOut (const std::string& strCallID);
	virtual void OnCallEstablished (const std::string& strCallID);
	virtual void OnCallDestroyed (const std::string& strCallID,IMXEC_CALL_END_REASON discReason);
	virtual void OnTimer(UINT32 nIDEvent);

	virtual void OnCallSecondVideoStart(const std::string&strCallID,int type=0);
	virtual void OnCallSecondVideoStop(const std::string&strCallID,int type=0);
	
	void OnCallBFCPTransportChange(SIPCall*pSIPMediaCall);

	//在收到SDP协商后重新打开codec
	void OnCallSDPNegotiate(SIPCall*pSIPMediaCall);

	//当接收到的对方发来的媒体数据与本端打开的解码CODEC不同时，以PT匹配重新打开解码CODEC
	//对端发来的媒体数据是以本端PT进行RTP封包，参数pt为接收的媒体数据的PalyloadType
	bool GetAudioLocalCodecTypeFromPalyloadType(X_AUDIO_CODEC_ID &type,int pt);
	bool GetVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt);

	bool GetDualVideoLocalCodecTypeFromPalyloadType(VIDEC_CODEC_TYPE &type,int pt);
	bool GetDataLocalCodecTypeFromPalyloadType(DATA_CODEC_ID &type,int pt);

	/******************************************************************************
	* OnRecvHangupCall
	*描述：接收到对端挂断呼叫
	*输入：	cszCallID		-呼叫标识
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int OnRecvHangupCall(const char*cszCallID);

	//added by jiangdingfeng 2013-11-18 判断是否有呼叫
	bool HasCall();

protected:
	typedef std::map<int,SIPUser*>	MapSIPUser;

	XCritSec			m_csMapSIPUser;
	MapSIPUser			m_MapSIPUser;
	XCritSec			m_csMapThrSIPUser;
	MapSIPUser			m_MapThrSIPUser;
	int					m_H264PT;
	int					m_H263PPT;
	int					m_H263PPPT;
	int					m_H224PT;

	int					m_H264SVCPT;
	int					m_DualH264PT;
	int					m_DualH264SVCPT;
	int					m_DualH263PPT;
	int					m_DualH263PPPT;

	int					m_G7221_24k_PT;
	int					m_G7221_32k_PT;
	int					m_G7221C_24k_PT;
	int					m_G7221C_32k_PT;
	int					m_G7221C_48k_PT;
	bool				m_bRunning;
	friend class SIPUser;
	typedef std::map<std::string,SIPMediaCall*>	MapSIPMediaCall;

	XCritSec			m_csMapSIPMediaCall;					//界面操作表加锁
	MapSIPMediaCall		m_MapSIPMediaCall;						//界面来的操作表

	XCritSec			m_csMapThrSIPMediaCall;					//线程操作表加锁
	MapSIPMediaCall		m_MapThrSIPMediaCall;					//线程来的操作表

	std::string			m_strEmpty;
	int					m_nCurrentLineID;
	unsigned short		m_nRTPBasePort;
	unsigned short		m_nSIPListenPort;
	unsigned int		m_nTimerID;

	X_AUDIO_CODEC_ID	m_nAudioCodecID;
	int					m_nMainVideoDevID;
	int					m_nMainVideoFrameRate;
	VIDEC_CODEC_TYPE	m_nMainVideoCodecType;
	//VIDEO_FORMAT_TYPE	m_nMainVideoFormatType;
    int                 m_nMainVideoNetworkType;

	int						m_nSecondVideoDevID;
	int						m_nSecondVideoDevFrameRate;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	//VIDEO_FORMAT_TYPE		m_nSecondVideoFormatType;
    int                     m_nSecondVideoNetworkType;

public:
	unsigned long			m_nActualSIPListenPort;
	int					    m_nMainVideoBitrate;
	int						m_nSecondVideoMaxBitrate;

public:
	unsigned int			m_nRequestConfId;					//local bfcp info
	unsigned short			m_nRequestUserId;					//local bfcp info
	unsigned short			m_nRequestFloorId;					//local bfcp info
	unsigned short			m_nRequestPriority;					//local bfcp info
	EBFCPTransportType		m_nRequestTransportType;			//local bfcp info
	EBFCPFloorControlType	m_nRequestFloorControlType;			//local bfcp info
	EBFCPSetupType			m_nRequestSetupType;	

	unsigned int			m_nRequestConfIdCalled;					//Called local bfcp info
	unsigned short			m_nRequestUserIdCalled;					//Called local bfcp info
	unsigned short			m_nRequestFloorIdCalled;			    //Called local bfcp info
	EBFCPFloorControlType	m_nRequestFloorControlTypeCalled;		//Called local bfcp info
	EBFCPSetupType			m_nRequestSetupTypeCalled;	

	int						m_nStarted;

	//set bfcp local info
	void SetBFCPUserId(int userId);
	void SetBFCPConfId(int confId);
	void SetBFCPFloorId(int floorId);
	void SetBFCPRequestPriority(unsigned short priority);
	void SetBFCPSetupType(EBFCPSetupType type);
	void SetBFCPFloorControlType(EBFCPFloorControlType type);
	void SetBFCPTransportType(EBFCPTransportType type);
	//keep connect
	void KeepConnectedSendBfcpInfo();
	//set sip bfcp sdp local info
	void SetSDPBFCPLocalInfo(SIPCallCapability & sdpCap,int nDir=IMXEC_CALL_DIR_OUT);
	//set sip bfcp call local info
	void SetSIPCallBFCPLocalInfo(SIPMediaCall* pSIPMediaCall,int nDir=IMXEC_CALL_DIR_OUT);
	//set sip bfcp net local info
	void SetSIPBFCPLocalInfo(SIP_BFCP * pSipBfcp,int nDir=IMXEC_CALL_DIR_OUT);
	void SIPCallMgr::SipSetting();

	/******************************************************************************
	* SetSIPTransportType
	*描述：设置SIP信令传输类型
	*输入：type			-类型
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type);

	//0 udp 1 tcp 2 tls 
	int GetSIPTransportType();

	//通过uri查找到SIPUser
	SIPUser * GetSIPUserFromSipUri(pjsip_uri *uri,pjsip_transport * transport);
	//等待SIP账号注销完成
	void WaitSIPUserUnRegComplete();
	void WaitSIPUserDelete(SIPUser *pSIPUser);

	/******************************************************************************
	* SetICETransportCount
	*描述：设置创建的每种媒体流ICE通道数
	*输入：	nCount		-每种媒体流ICE通道数(即如只有RTP则设为1，如还需要有RTCP则设为2)
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetICETransportCount(int nCount);
	/******************************************************************************
	* SetICEEnable
	*描述：设置ICE应用状态
	*输入：	bEnable		-ICE应用状态 true使用，false不使用，默认false
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetICEEnable(bool bEnable);
	/******************************************************************************
	* SetTURNLoginAuth
	*描述：设置登录TURN服务器账户
	*输入：	cszTurnUser		-TURN服务器账户名
	cszTurnPWD		-TURN服务器账户密码
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD);
	/******************************************************************************
	* SetTURNServerIPPort
	*描述：设置TURN服务器地址,也可设为域名地址
	*输入：	cszTurnIPPort		-TURN服务器地址 形如IP:Port
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetTURNServerIPPort(const char*cszTurnIPPort);
	/******************************************************************************
	* SetSTUNServerIPPort
	*描述：设置STUN服务器地址,也可设为域名地址
	*输入：	cszStunIPPort		-STUN服务器地址 形如IP:Port
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSTUNServerIPPort(const char*cszStunIPPort);
	/******************************************************************************
	* SetTurnTransportType
	*描述：设置TURN传输通道类型
	*输入：	nType		-TURN传输通道类型 17 udp, 6 tcp, 255 tls, 具体可参看pj_turn_tp_type
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetTurnTransportType(int nType);
	/******************************************************************************
	* SetDNSServerIP
	*描述：设置DNS服务器地址 如果不是有效DNS就不要设置
	*输入：	cszDNSIP		-设置DNS服务器地址 形如IP
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetDNSServerIP(const char*cszDNSIP);
	/******************************************************************************
	* SetDNSServerIP2
	*描述：设置DNS服务器地址2  如果不是有效DNS就不要设置
	*输入：	cszDNSIP2		-设置DNS服务器地址 形如IP
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetDNSServerIP2(const char*cszDNSIP2);

	/******************************************************************************
	* GetICEInstanceConfig
	*描述：获取ICE CONFIG
	*输入：无
	*输出：无
	*返回值：返回ICE CONFIG *
	*****************************************************************************/
	pj_ice_strans_cfg * GetICEInstanceConfig();

	/******************************************************************************
	* GetICEMediaTrans
	*描述：取ICE媒体通道,如果小于等于1则创建3*m_nICETransportCount个 
	       对于取到的使用结束后会自动关闭,无需再在外面关闭
	*输入：无
	*输出：无
	*返回值：成功返回ICE通道指针，失败返回NULL
	*****************************************************************************/
	pjmedia_transport* GetICEMediaTrans();

	/******************************************************************************
	* GetMediaEndpoint
	*描述：取MediaEndpoint
	*输入：无
	*输出：无
	*返回值：成功返回MediaEndpoint指针，失败返回NULL
	*****************************************************************************/
	pjmedia_endpt * GetMediaEndpoint();

	//ICE协商结果回调
	void OnICENegotiationComplete(pjmedia_transport *tp,pj_ice_strans_op op,pj_status_t status);

private:
	/******************************************************************************
	* ICEInstanceInit
	*描述：初始化ICE应用，注意在SipInit后调用或放于SipInit中最后调用
	*输入：	无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int ICEInstanceInit();

	/******************************************************************************
	* ICEInstanceExit
	*描述：ICE应用退出
	*输入：	无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int ICEInstanceExit();

	/******************************************************************************
	* CreateMediaEndpoint
	*描述：创建MediaEndpoint
	*输入：	无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int CreateMediaEndpoint();

	/******************************************************************************
	* CloseMediaEndpoint
	*描述：释放MediaEndpoint
	*输入：	无
	*输出：无
	*返回值：无
	*****************************************************************************/
	void CloseMediaEndpoint();

	/******************************************************************************
	* CreateICEMediaTrans
	*描述：创建ICE通道
	*输入： nCount初始创建最多ICE通道数,默认3*m_nICETransportCount
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int CreateICEMediaTrans(int nCount);

	/******************************************************************************
	* GetICEMediaTrans
	*描述：取ICE媒体通道,如果小于等于1则创建3*m_nICETransportCount个 
	       对于取到的使用结束后会自动关闭,无需再在外面关闭
	*输入： pIceTrans取到的ICE通道指针
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int GetICEMediaTrans(pjmedia_transport** pIceTrans);

	//删除指定的pjmedia_transport
	int DestroyICEMediaTrans(pjmedia_transport* pMediaTransport);
	//删除所有
	int DestroyAllICEMediaTrans();
	//ICE线程
	static int ICEWorkerThread(void *unused);
	//ICE线程函数执行体
	int ICEHandleEvents(unsigned max_msec, unsigned *p_count);
	//设置NAT检测结果回调函数
	void SetNatDetectCallback();

	void DoCheckMediaStop(void);

	//SIP注册失败后定时注册 使用m_nSIPRegFailTimestamp间隔时间定时注册
	void TimerRegAccountCheck(unsigned long nTimestamp);

public:
	//用于SIP在未注册情况下IP呼叫
	SIPUser * m_pIPCallSIPUser;
	IMXEC_SIP::TRANS_TYPE	m_nSIPTransType;				//SIP信令传输类型
	char					m_strUserAgentName[256];
	int						m_nICETransportCount;			//每种媒体流ICE通道数
	std::string				m_strStunSrv;					//STUN服务器地址 形如IP:Port
	std::string				m_strTurnSrv;					//TURN服务器地址 形如IP:Port
	std::string				m_strTurnUser;					//TURN服务器账户名
	std::string				m_strTurnPWD;					//TURN服务器账户密码
	std::string				m_strDNSSrv;
	std::string				m_strDNSSrv2;
	bool					m_bICEEnable;					//ICE应用状态 true使用，false不使用，默认false
	pj_thread_t *			m_pICEThread;					//ICE线程
	bool					m_bICEThreadQuitFlag;			//ICE线程退出标志

private:
	pj_ice_strans_cfg		m_ICEConfig;					//ICE CONFIG
	pj_caching_pool			m_ICECachingPool;
	int						m_nTurnTPType;					//TURN传输通道类型 参看pj_turn_tp_type
	int						m_nICEInit;						//default 0
	pj_pool_t *				m_pICEPool;
	pjmedia_endpt *			m_pMediaEndpoint;					
	pj_dns_resolver	*		m_pDNSResolver;					//DNS resolver
	typedef std::vector<pjmedia_transport*>	MapSIPICE;
	XCritSec				m_csMapSIPMediaICETrans;
	MapSIPICE				m_MapSIPMediaICETrans;			//ice pjmedia_transport
	int						m_nMaxICESocketCount;			//准备创建最多ICE SOCKET数,注 默认6*10路

public:
	unsigned						m_nOutboundProxyCount;						//Number of outbound proxies in the  m_OutboundProxyArray array.
	char							m_strOutboundProxy[8][256];					//outbound proxies
	pjsip_route_hdr					m_OutboundProxyRoute;						//List of outbound proxies
	pjsip_route_hdr					m_RouteHdr;									//Complete route set inc. outbnd.
	int								m_nForcelrParam;							//此控制request line不再使用outbound uri
	int								m_nUseRegReqLine;							//reg request line use new addr
	int								m_nUseInvReqLine;							//call request line use new addr
	std::string						m_strRegReqLineAddr;						//request line new addr
	std::string						m_strInvReqLineAddr;						//request line new addr
	std::string						m_strDateTime;
	int								m_nUseDualVideoToken;						//use token interactive is 1,else is 0
	int								m_nSDPOrignIPConverse;						//SDP orign ip is converse ,not converse is 0 ,converse is 1
	int								m_nSendSubscribre;							//call sip mcu Send Subscribre is 1,else is 0
	int								m_nSendPicFastUpdate;						// Send Picture Fast Update is 1,not Send is 0
	int								m_nSendMultipartSDP;						//Send Multipart SDP set 1 ,not set 0
	//unsigned long					m_nSIPRegFailLastTimestamp;
	unsigned long					m_nSIPRegFailTimerTimestamp;				//注册失败后再次注册间隔时间 ms 默认2*60*1000
	int								m_nSIPREGFail;								//注册失败状态 1为失败状态 2成功状态
	int								m_nUseDualVideoTokenType;					//0 not use , 1 use to zhongxing ,
	char							m_strConfCentralNum[256];					//conference central number

public:
	void SetCapabilityParam(SIPCall * pSIPCall,SIPCallCapability & sdpCap,int nSupportDual,std::string strLocalIP,int nLocalAudioPort,int nLocalVideoPort,int nLocalDualVideoPort,int nLocalBFCPTransPort,int nDir=IMXEC_CALL_DIR_OUT);

protected:
	virtual void							ThreadProcMain(void);
	int										m_nCapPrint;						//输出能力集信息内容 0不打印 1打印
	int										m_nRegExpires;
	int										m_nSIPLogLevel;
	char									m_strSIPLogFile[256];

private:
	void SetDataBufferChannelDestroyed(const std::string& strCallID,std::string strCMD,std::string strMedia);
	void SetDataBufferChannelConnectedVD(const std::string& strCallID ,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth,int nHeight,int nLevel,std::string strCMD,std::string strMedia,HDAPAPPCALL hdapCall=NULL);


public:

	/******************************************************************************
	* OutPutStackLog
	*描述：协议栈日志输出
	*输入：	nLogType			-日志类型  0 正常日志 1 错误日志 
			cszLog				-日志内容
			nLen				-日志内容长度
	*输出： 无
	*返回值：无
	*****************************************************************************/
	void					OutPutStackLog(int nLogType,const char * cszLog,char * szCallID=NULL);

	/******************************************************************************
	* OutPutCapContent
	*描述：能力集信令内容输出 输出需设置开关
	*输入：	cszCallID			-呼叫标识
			nCapType			-能力信令类型 0 远端能力集内容 1 本端能力集内容 2 远端通道通力集内容 3 本端通道能力集内容
			cszContent			-信令内容
			nLen				-信令内容长度
	*输出： 无
	*返回值：无
	*****************************************************************************/
	void					OutPutCapContent(const char*cszCallID,int nCapType,const char * cszContent);

	int						GetDAPCapPrintEnable(void);

	/******************************************************************************
	*描述：发送DTMF二次拨号按键值 注此也可在外面实现{ AUDEC_GetEngine().AUDEC_PlayDTMF(dtmf);} 内部实现不再调AUDEC_GetEngine
	*输入：	cszCallID			-呼叫标识
			dtmf				-按键值
	*****************************************************************************/
	int DAPSendCallDTMF(const char*cszCallID,const char dtmf);

	/******************************************************************************
	*描述：向对端发送自定义非标消息
	*输入：	cszCallID		-呼叫标识
			cszContent		-自定义非标准信息内容
			cszMimeType		-自定义非标信息内容类型 如application/xml
	*****************************************************************************/
	int DAPSendCallCustomMsg(const char*cszCallID,const char * cszContent,const char * cszMimeType);

	/******************************************************************************
	*描述：发送辅流控制令牌消息
	*输入：	cszCallID		-呼叫标识
			nType			-辅流控制令牌消息类型 1 请求 2 响应 3 指示 4 取消
			transactionId	-事务ID  
			userId			-用户ID  
			floorId			-事务子类型ID 
			status			-状态 如发送响应时 0表示拒绝 1表示允许
	*****************************************************************************/
	int SendCallDualVideoControl(const char*cszCallID,int nType,int transactionId=0,int userId=0,int floorId=0,int status=0);

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
	* SetAgentName
	*描述：设置应用实体名 userAgent
	*输入：	cszAgent		-应用实体名
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetAgentName(const char*cszAgent);

	/******************************************************************************
	* SetCapPrintEnable
	*描述：设置打印本端及远端能力集信令消息内容,默认不打印
	*输入：	nPrint		-不打印设为0,打印设置为1
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetDAPCapPrintEnable(int nPrint=0);

	/******************************************************************************
	* SetRegisterTimer
	*描述：设置SIP注册间隔时间 
	*输入：	regTimer		-注册成功后再次注册刷新时间 单位,秒
			regFailTimer	-注册失败后再次注册间隔时间 单位,秒
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRegisterTimer(int regTimer,int regFailTimer);

	/******************************************************************************
	* SetSIPLogFile
	*描述：设置SIP协议栈记录日志级别及日志文件名
	*输入：	logLevel		-日志级别 0,fatal error 1,error 2,warning 3,info 4,debug 5,trace 6,more detailed trace
			cszLogFileName	-日志文件名
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetSIPLogFile(int logLevel,const char*cszLogFileName);

	int SetDAPLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetDAPLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate);
	int SetSIPCallSupportH239(const char*cszCallID,int nSupportH239);

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
	*描述：接收到音频媒体流回调
	*输入：	cszCallID			-呼叫标识
			pRTPPacketData		-RTP包
			nRTPPacketLen		-RTP包长度
			nPayloadType		-PayloadType
			nRTPHeaderLen		-RTP包头长度
			usSequence			-RTP包次序
			ulTimestamp			-RTP包时间戳
	*****************************************************************************/
	virtual void OnDAP_RecvRemoteAudioRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

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
	virtual void OnDAP_RecvRemoteMainVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	
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
	virtual void OnDAP_RecvRemoteSecondVideoRTPPacket(const char*cszCallID,unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);

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
	*描述：媒体通道连接成功事件
	*输入：	type				-1 媒体类型 1 audio 2 main video 3 second video 4 fecc data
			sendRecv			-1 send 2 recv
			cszCallID			-呼叫标识
			codecType			-视频CodecID
			nBitrate			-码率
			nPayloadType		-PayloadType
			width				-分辨率宽度像素数 可选
			nHeight				-分辨率高度像素数 可选
			ip					-本端接收rtp ip
			port				-本端接收rtp port
			nLevel				-分辨率实际H264level值 可选
	*****************************************************************************/
	virtual void OnDAP_CallMediaChannelConnected(int type,int sendRecv,HDAPAPPCALL hdapCall,const std::string& strCallID ,VIDEC_CODEC_TYPE codecType
		,int nBitrate,int nPayloadType,unsigned long ip, int port,int nWidth=0,int nHeight=0,int nLevel=0);

	/******************************************************************************
	*描述：媒体通道关闭事件
	*输入：	type				-1 媒体类型 1 audio 2 main video 3 second video 4 fecc data
			sendRecv			-1 send 2 recv
			cszCallID			-呼叫标识
	*****************************************************************************/
	virtual void OnDAP_CallMediaChannelDestroyed(int type,int sendRecv,const std::string& strCallID);


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
	*描述：收到对端发送的SIP INFO负载的消息
	*输入：	strCallID		-呼叫标识
			cszContent		-自定义信息内容
			cszMimeType		-自定义内容类型 如application/xml
	*****************************************************************************/
	virtual void OnDAP_RecvCallCustomMsg(const std::string& strCallID,const char * cszContent,const char * cszMimeType);

	/******************************************************************************
	*描述：收到对端发送的辅流控制令牌消息
	*输入：	strCallID		-呼叫标识
			nType			-辅流控制令牌消息类型 1 请求 2 响应 3 指示 4 取消
			transactionId	-事务ID  
			userId			-用户ID  
			floorId			-事务子类型ID 
			status			-状态 如发送响应时 0表示拒绝 1表示允许
	*****************************************************************************/
	virtual void OnDAP_RecvCallDualVideoControl(const std::string& strCallID,int nType,int transactionId=0,int userId=0,int floorId=0,int status=0);


#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 2)

public:
	void StartVideoToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopVideoToCall(const char*cszCallID);
	void StartH239ToCall(const char*cszCallID,unsigned long nVideoID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopH239ToCall(const char*cszCallID);
	void StartAudioToCall(const char*cszCallID,unsigned long nAudioID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort);
	void StopAudioToCall(const char*cszCallID);

#endif

	// 允许对端发送双流
	void AllowToSendSecondVideo(const char*cszCallID, bool bAllow);
	// 发流控命令
	void SendFlowControlCommand(const char*cszCallID, IMXEC_CALL_MEDIA_TYPE emMediaType, int nMaximumBitRate);
	//用于呼通后再打开通道
	void OpenSIPMediaChannel(SIPMediaCall* pSIPMediaCall);
	void HungupAll(int nLineID =-1);

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

	unsigned short GetCallRTPPort(void);

public:
	unsigned long	m_nLastPacketCheckTimestamp;
	unsigned long	m_nLastBFCPCheckTimestamp;
	unsigned long	m_nSIPCheckLastTimestamp;
	int			    m_nProxySendRtp;						// use nat check server proxy send rtp is 1,not Send is 0
	int			    m_nProxyUseFEC;						    // use nat check server proxy send rtp is 1,not Send is 0
	int			    m_nRTPUseType;                          // use medianet rtp is 1,use pjsip rtp is 2 ,dufault is 1
	VIDEO_FORMAT_TYPE       m_nMainVideoFormatType;
	VIDEO_FORMAT_TYPE       m_nSecondVideoFormatType;
	int             m_nSIPServerType;                       //1 is kamailio,2 is sgb sip server,default 0
	int             m_nDualVideoSDPMode;                    //1 is Send first invite Add DualVideo SDP,2 is send second invite DualVideo SDP,default 0
	unsigned long	m_nLastCheckCallNumTS;

	MediaPortRelease m_MediaPortRelease;
	void SaveMediaReleasePort(unsigned short rtpPort);
	void GetTerminalInfo(SIPMediaCall* pSIPMediaCall,pjsip_rx_data *rdata);
	void GetServerInfo(pjsip_rx_data *rdata);
	void SetMediaPortArrayEmpty();
	//回调挂断状态时再检测下是不是因为网络收不到媒体流原因挂断的
	bool DisconnectCheckRoundTripDalay(SIPMediaCall * pSIPMediaCall);

	bool CheckMapCallEmpty(void);

};

#endif
