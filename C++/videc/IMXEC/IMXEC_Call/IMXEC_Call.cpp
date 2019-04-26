#include <IMXEC/IMXEC_Call.h>
#include "CallMgr.h"

IMXEC_Call::IMXEC_Call(void)
:m_nExclusive(0)
{

}

IMXEC_Call::~IMXEC_Call(void)
{
}

int IMXEC_Call::GetExclusive(void)
{
	return m_nExclusive;
}

/******************************************************************************
* Start
*描述：启动呼叫
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::Start(int nExclusive)
{
	OutputDebugString("IMXEC_Call::Start ---------------------------------------------------\n");
	m_nExclusive=nExclusive;
	CallMgr::Instance().AddIMXEC_Call(this);
	return 0;
}

/******************************************************************************
* Stop
*描述：停止呼叫
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
void IMXEC_Call::Stop(void)
{
	CallMgr::Instance().RemoveIMXEC_Call(this);
	OutputDebugString("IMXEC_Call::Stop ---------------------------------------------------\n");

}


/******************************************************************************
* MakeCall
*描述：发起呼叫
*输入：	cszUserID		-对方的用户ID或IP
		cszProtocol		-呼叫协议，"H323","SIP","IMXEC"
*输出：无
*返回值：呼叫标识
*****************************************************************************/
const char*IMXEC_Call::MakeCall(const char*cszUserID,const char*cszProtocol)
{
	return CallMgr::Instance().MakeCall(cszUserID,cszProtocol);
}


/******************************************************************************
* AcceptCall
*描述：接受呼叫请求
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::AcceptCall(const char*cszCallID)
{
	return CallMgr::Instance().AcceptCall(cszCallID);
}

/******************************************************************************
* HangupCall
*描述：挂断呼叫
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::HangupCall(const char*cszCallID)
{
	return CallMgr::Instance().HangupCall(cszCallID);
}


/******************************************************************************
* SendPTZ
*描述：发送PTZ【发送云台控制命令】
*输入：	cszCallID		-呼叫标识
		nPTZAction		-云台动作
		nSpeed			-动作的速度
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SendPTZ (const char*cszCallID,PTZ_ACTION nPTZAction,int nSpeed)
{
	return CallMgr::Instance().SendPTZ(cszCallID,nPTZAction,nSpeed);
}


/******************************************************************************
* StartSecondVideo
*描述：开始辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StartSecondVideo (const char*cszCallID)
{
	return CallMgr::Instance().StartSecondVideo(cszCallID);
}

/******************************************************************************
* StopSecondVideo
*描述：停止辅流
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StopSecondVideo (const char*cszCallID)
{
	return CallMgr::Instance().StopSecondVideo(cszCallID);
}


/******************************************************************************
* SetLocalMainVideoWnd
*描述：设置本地主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-本地主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetLocalMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetLocalMainVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetLocalMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetLocalMainVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetLocalMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	CallMgr::Instance().SetLocalMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
}

/******************************************************************************
* SetLocalMainVideoEnableOSD
*描述：设置本地主流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoEnableOSD(const char*cszCallID,int nEnable)
{
	CallMgr::Instance().SetLocalMainVideoEnableOSD(cszCallID,nEnable);
}

/******************************************************************************
* SetLocalMainVideoOSDText
*描述：设置本地主流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	CallMgr::Instance().SetLocalMainVideoOSDText(cszCallID,cszText,nAlign);
}

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
void IMXEC_Call::SetLocalMainVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	CallMgr::Instance().SetLocalMainVideoOSDFont(cszCallID,logfont,crText,crBk);
}

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
int IMXEC_Call::GetLocalMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetLocalMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetLocalMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetLocalMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetRemoteMainVideoWnd
*描述：设置远程主流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程主视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetRemoteMainVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetRemoteMainVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetRemoteMainVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetRemoteMainVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetRemoteMainVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen)
{
	CallMgr::Instance().SetRemoteMainVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen);
}

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
int IMXEC_Call::GetRemoteMainVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetRemoteMainVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetRemoteMainVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetRemoteMainVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetSecondVideoWnd
*描述：设置辅流视频显示的窗口
*输入：	cszCallID		-呼叫标识
		hWnd			-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetSecondVideoWnd(const char*cszCallID,HWND hWnd)
{
	return CallMgr::Instance().SetSecondVideoWnd(cszCallID,hWnd);
}

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
int IMXEC_Call::SetSecondVideoWndPos(const char*cszCallID,int x,int y,int cx,int cy)
{
	return CallMgr::Instance().SetSecondVideoWndPos(cszCallID,x,y,cx,cy);
}

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
void IMXEC_Call::SetSecondVideoParam(const char*cszCallID,int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly)
{
	CallMgr::Instance().SetSecondVideoParam(cszCallID,nBrightness,nContrast,nSaturation,nHue,nGamma,nSharpen,bSharpenLocalOnly);
}

/******************************************************************************
* SetSecondVideoEnableOSD
*描述：设置辅流视频是否启用OSD
*输入：	cszCallID		-呼叫标识
		nEnable			-是否启用OSD
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetSecondVideoEnableOSD(const char*cszCallID,int nEnable)
{
	CallMgr::Instance().SetSecondVideoEnableOSD(cszCallID,nEnable);
}

/******************************************************************************
* SetSecondVideoOSDText
*描述：设置辅流视频OSD文字
*输入：	cszCallID		-呼叫标识
		cszText			-OSD文字
		nAlign			-对齐方式
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetSecondVideoOSDText(const char*cszCallID,const char*cszText,int nAlign)
{
	CallMgr::Instance().SetSecondVideoOSDText(cszCallID,cszText,nAlign);
}

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
void IMXEC_Call::SetSecondVideoOSDFont(const char*cszCallID,LOGFONTA logfont,COLORREF crText,COLORREF crBk)
{
	CallMgr::Instance().SetSecondVideoOSDFont(cszCallID,logfont,crText,crBk);
}

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
int IMXEC_Call::GetSecondVideoInfo(const char*cszCallID, int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,CallChannelInfo & chInfo,HWND&hVideoWnd)
{
	return CallMgr::Instance().GetSecondVideoInfo(cszCallID,nWidth,nHeight,nVideoCodecType,chInfo,hVideoWnd);
}

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
int IMXEC_Call::GetSecondVideoPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetSecondVideoPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* GetLocalAudioLevel
*描述：获取本地音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetLocalAudioLevel(const char*cszCallID,int&nLevel)
{
	return CallMgr::Instance().GetLocalAudioLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalAudioVolume
*描述：获取本地音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalAudioVolume(const char*cszCallID,int nVolume)
{
	CallMgr::Instance().SetLocalAudioVolume(cszCallID,nVolume);
}

/******************************************************************************
* SetLocalAudioMute
*描述：获取本地音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalAudioMute(const char*cszCallID,int nMute)
{
	CallMgr::Instance().SetLocalAudioMute(cszCallID,nMute);
}

/******************************************************************************
* GetLocalAudioCodecID
*描述：获取本地音频编码信息
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetLocalAudioCodecID(const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return CallMgr::Instance().GetLocalAudioCodecID(cszCallID,nAudioCodecID);
}

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
int IMXEC_Call::GetLocalAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetLocalAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* GetRemoteAudioLevel
*描述：获取远程音频Level【实际声音大小】
*输入：	cszCallID		-呼叫标识
*输出：	nLevel			-实际声音大小【0-10】
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetRemoteAudioLevel(const char*cszCallID,int&nLevel)
{
	return CallMgr::Instance().GetRemoteAudioLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetRemoteAudioVolume
*描述：获取远程音频音量
*输入：	cszCallID		-呼叫标识
		nVolume			-音量
*输出：	无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetRemoteAudioVolume(const char*cszCallID,int nVolume)
{
	CallMgr::Instance().SetRemoteAudioVolume(cszCallID,nVolume);
}

/******************************************************************************
* SetRemoteAudioMute
*描述：获取远程音频是否静音
*输入：	cszCallID		-呼叫标识
		nMute			-是否静音
*输出：	无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetRemoteAudioMute(const char*cszCallID,int nMute)
{
	CallMgr::Instance().SetRemoteAudioMute(cszCallID,nMute);
}

/******************************************************************************
* GetRemoteAudioCodecID
*描述：获取远端音频编码
*输入：	cszCallID		-呼叫标识
*输出：	nAudioCodecID	-编码ID
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetRemoteAudioCodecID (const char*cszCallID, X_AUDIO_CODEC_ID &nAudioCodecID)
{
	return CallMgr::Instance().GetRemoteAudioCodecID(cszCallID,nAudioCodecID);
}

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
int IMXEC_Call::GetRemoteAudioPacketStatistics(const char*cszCallID,unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)
{
	return CallMgr::Instance().GetRemoteAudioPacketStatistics(cszCallID,ulTotalPackets,ulLostPackets,AvgPacketLossRate,CurPacketLossRate,nBitrate);
}

/******************************************************************************
* SetCallType
*描述：设置呼叫类型
*输入：	nCallType		-呼叫类型
*输出：	无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetCallType(IMXEC_CALL_TYPE nCallType)
{
	return CallMgr::Instance().SetCallType(nCallType);
}

/******************************************************************************
* GetCallType
*描述：获取呼叫类型
*输入：	无
*输出：	nCallType		-呼叫类型
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetCallType(IMXEC_CALL_TYPE&nCallType)
{
	return CallMgr::Instance().GetCallType(nCallType);
}

/******************************************************************************
* GetCallSupportSecondVideo
*描述：获取呼叫是否支持辅流【H239】
*输入：	cszCallID		-呼叫标识
*输出：	无
*返回值：支持返回非0，不支持返回0
*****************************************************************************/
int IMXEC_Call::GetCallSupportSecondVideo(const char*cszCallID)
{
	return CallMgr::Instance().GetCallSupportSecondVideo(cszCallID);
}



/******************************************************************************
* StartRecordLocalMainToAVI
*描述：开始本地主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StartRecordLocalMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordLocalMainToAVI(cszCallID,cszPathName,nStandardFormat);
}

/******************************************************************************
* StopRecordLocalMainToAVI
*描述：停止本地主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::StopRecordLocalMainToAVI(const char*cszCallID)
{
	CallMgr::Instance().StopRecordLocalMainToAVI(cszCallID);
}

/******************************************************************************
* StartRecordRemoteMainToAVI
*描述：开始远端主流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StartRecordRemoteMainToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordRemoteMainToAVI(cszCallID,cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordRemoteMainToAVI
*描述：停止远端主流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::StopRecordRemoteMainToAVI(const char*cszCallID)
{
	CallMgr::Instance().StopRecordRemoteMainToAVI(cszCallID);
}

/******************************************************************************
* StartRecordSecondToAVI
*描述：开始辅流录像
*输入：	cszCallID			-呼叫标识
		cszPathName			-录像文件名称
		nStandardFormat		-是否录制成标准文件格式
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StartRecordSecondToAVI(const char*cszCallID,const char*cszPathName,int nStandardFormat)
{
	return CallMgr::Instance().StartRecordSecondToAVI(cszCallID,cszPathName,nStandardFormat);
}


/******************************************************************************
* StopRecordSecondToAVI
*描述：停止辅流录像
*输入：cszCallID			-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::StopRecordSecondToAVI(const char*cszCallID)
{
	return CallMgr::Instance().StopRecordSecondToAVI(cszCallID);
}

/******************************************************************************
* GetMainAudioID
*描述：获取主流音频标识
*输入： cszCallID		-呼叫标识
*输出： nMainAudioID	-主流音频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetMainAudioID(const char*cszCallID,unsigned long&nMainAudioID)
{
	return CallMgr::Instance().GetMainAudioID(cszCallID,nMainAudioID);
}

/******************************************************************************
* GetMainVideoID
*描述：获取主流视频标识
*输入： cszCallID		-呼叫标识
*输出： nMainVideoID	-主流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetMainVideoID(const char*cszCallID,unsigned long&nMainVideoID)
{
	return CallMgr::Instance().GetMainVideoID(cszCallID,nMainVideoID);
}

/******************************************************************************
* GetSecondVideoID
*描述：获取辅流视频标识
*输入： cszCallID		-呼叫标识
*输出： nSecondVideoID	-辅流视频标识
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetSecondVideoID(const char*cszCallID,unsigned long&nSecondVideoID)
{
	return CallMgr::Instance().GetSecondVideoID(cszCallID,nSecondVideoID);
}

/******************************************************************************
* SetMainVideoScreenIndex
*描述：设置主流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetMainVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	return CallMgr::Instance().SetMainVideoScreenIndex(cszCallID,nScreenIndex);
}

/******************************************************************************
* SetSecondVideoScreenIndex
*描述：设置辅流视频图像桌面序号
*输入： cszCallID		-呼叫标识
		nScreenIndex	-桌面序号
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetSecondVideoScreenIndex(const char*cszCallID,int nScreenIndex)
{
	return CallMgr::Instance().SetSecondVideoScreenIndex(cszCallID,nScreenIndex);
}

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
void IMXEC_Call::SetSecondVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	CallMgr::Instance().SetSecondVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
}

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
void IMXEC_Call::SetRemoteVideoScreenWnd(const char*cszCallID,int nScreenIndex,int nWndIndex,int nFullScreen)
{
	CallMgr::Instance().SetRemoteVideoScreenWnd(cszCallID,nScreenIndex,nWndIndex,nFullScreen);
}

/******************************************************************************
* SetLocalRecordCallback
*描述：设置本地录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetLocalRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SetRemoteRecordCallback
*描述：设置远程录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetRemoteRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetRemoteRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SetSecondRecordCallback
*描述：设置辅流录像回调
*输入：	cszCallID		-呼叫标识
		pRecordCallback	-录像回调
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetSecondRecordCallback(const char*cszCallID,IMXEC_RecordCallback*pRecordCallback)
{
	CallMgr::Instance().SetSecondRecordCallback(cszCallID,pRecordCallback);
}

/******************************************************************************
* SaveLocalMainVideoToBMPFile
*描述：保存本地主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：无
*****************************************************************************/
int IMXEC_Call::SaveLocalMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveLocalMainVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SaveRemoteMainVideoToBMPFile
*描述：保存远程主流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：无
*****************************************************************************/
int IMXEC_Call::SaveRemoteMainVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveRemoteMainVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SaveSecondVideoToBMPFile
*描述：保存辅流视频到BMP图片文件
*输入：	cszCallID		-呼叫标识
		cszPathName		-图片文件全路径
*输出：无
*返回值：无
*****************************************************************************/
int IMXEC_Call::SaveSecondVideoToBMPFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().SaveSecondVideoToBMPFile(cszCallID,cszPathName);
}

/******************************************************************************
* SendDTMF
*描述：发送DTMF
*输入：	cszCallID		-呼叫标识
		nDTMF			-DTMF
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SendDTMF(const char*cszCallID,const char nDTMF)
{
	return CallMgr::Instance().SendDTMF(cszCallID,nDTMF);
}

/******************************************************************************
* StartMixingWithWaveFile
*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
*输入：	cszCallID		-呼叫标识
		cszPathName		-WAVE文件全路径
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StartMixingWithWaveFile(const char*cszCallID,const char*cszPathName)
{
	return CallMgr::Instance().StartMixingWithWaveFile(cszCallID,cszPathName);
}

/******************************************************************************
* StopMixingWithWaveFile
*描述：停止对呼叫进行WAVE文件混音
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::StopMixingWithWaveFile(const char*cszCallID)
{
	return CallMgr::Instance().StopMixingWithWaveFile(cszCallID);
}

/******************************************************************************
* SetAudioOnly
*描述：设置是否只支持音频
*输入：	nAudioOnly		-是否只支持音频【0：支持视频音频，非0：只支持音频】
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::SetAudioOnly(int nAudioOnly)
{
	return CallMgr::Instance().SetAudioOnly(nAudioOnly);
}

/******************************************************************************
* GetCallProtocol
*描述：获取呼叫协议
*输入：	cszCallID		-呼叫标识
*输出：无
*返回值：成功返回呼叫协议，失败返回空串""
*****************************************************************************/
const char*IMXEC_Call::GetCallProtocol(const char*cszCallID)
{
	return CallMgr::Instance().GetCallProtocol(cszCallID);
}

/******************************************************************************
* GetHasVideo
*描述：获取是否只有视频
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：如果有视频，那么返回非0，否则返回0
*****************************************************************************/
int IMXEC_Call::GetHasVideo(const char*cszCallID)
{
	return CallMgr::Instance().GetHasVideo(cszCallID);
}

/******************************************************************************
* SetEnableVideoTranscoding
*描述：设置是否启用视频转码
*输入：nEnable		-是否启用视频转码【0：不启用视频转码，非0：启用视频转码】
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetEnableVideoTranscoding(int nEnable)
{
	CallMgr::Instance().SetEnableVideoTranscoding(nEnable);
}

/******************************************************************************
* GetEnableVideoTranscoding
*描述：获取是否启用视频转码
*输入：无
*输出：无
*返回值：如果启用视频转码，那么返回非0，否则返回0
*****************************************************************************/
int IMXEC_Call::GetEnableVideoTranscoding(void)
{
	return CallMgr::Instance().GetEnableVideoTranscoding();
}

/******************************************************************************
* GetSecondVideoDirection
*描述：获取辅流的方向
*输入：	cszCallID		-呼叫标识
*输出： nDir			-辅流的方向
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::GetSecondVideoDirection(const char*cszCallID,IMXEC_CALL_DIRECTION&nDir)
{
	return CallMgr::Instance().GetSecondVideoDirection(cszCallID,nDir);
}




//H235 Media use

/******************************************************************************
* GetH235AuthCheckState
*描述：获取H235验证检查状态 用于取呼叫验证及为网守时作验证检查
*输入：	cszCallID		  -呼叫标识
*输出： nState -验证检查状态
nLength -返回验证失败原因字符串长度
*返回值：成功返回验证失败原因，失败返回空
*****************************************************************************/

const char* IMXEC_Call::GetH235AuthCheckState(const char*cszCallID,IMXEC_CALL_H235AUTH_STATE &nState,int &nLength)
{
	return CallMgr::Instance().GetH235AuthCheckState(cszCallID,nState,nLength);
}


/******************************************************************************
* SetExternalIP
*描述：设置外网映射IP
*输入：cszExternalIP		-外网映射IP
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetExternalIP(const char*cszExternalIP)
{
	CallMgr::Instance().SetExternalIP(cszExternalIP);
}


/******************************************************************************
* RequestMainVideoKeyFrame
*描述：请求主流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::RequestMainVideoKeyFrame(const char*cszCallID)
{
	CallMgr::Instance().RequestMainVideoKeyFrame(cszCallID);
}

/******************************************************************************
* RequestSecondVideoKeyFrame
*描述：请求辅流I帧
*输入：cszCallID		-呼叫标识
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::RequestSecondVideoKeyFrame(const char*cszCallID)
{
	CallMgr::Instance().RequestSecondVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SetLocalMainVideoSource
*描述：设置本地主视频源
*输入：cszCallID		  -呼叫标识
nMainVideoDevID    -主流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalMainVideoSource(const char*cszCallID, int nMainVideoDevID)
{
	CallMgr::Instance().SetLocalMainVideoSource(cszCallID, nMainVideoDevID);
}
/******************************************************************************
* SetLocalSecondVideoSource
*描述：设置本地辅视频源
*输入：cszCallID		  -呼叫标识
nSecondVideoDevID  -辅流设备ID
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetLocalSecondVideoSource(const char*cszCallID, int nSecondVideoDevID)
{
	CallMgr::Instance().SetLocalSecondVideoSource(cszCallID, nSecondVideoDevID);
}

/******************************************************************************
*描述:	向对端发送自定义非标消息
*输入:	cszCallID		-呼叫标识
cszContent		-自定义非标准信息内容
nContentLen		-自定义非标准信息长度
cszMimeType		-自定义非标信息内容类型 如为XML 值设为application/xml,如为一般字符串则设为NULL
*****************************************************************************/
int IMXEC_Call::SendCallCustomMsg(const char*cszCallID,const char * cszContent,int nContentLen,const char * cszMimeType)
{
	return CallMgr::Instance().SendCallCustomMsg(cszCallID,cszContent,nContentLen,cszMimeType);
}

/******************************************************************************
* HotDisplayVideo
*描述：视频显示支持显示器热插拔接口
*输入：	cszCallID		    -呼叫标识
hWndLocal			-本地主视频显示的窗口句柄
hWndRemote			-远程主视频显示的窗口句柄
hWndSecond         	-远程辅流视频显示的窗口句柄
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_Call::HotDisplayVideo(const char*cszCallID,HWND hWndLocal,HWND hWndRemote,HWND hWndSecond)
{
	return CallMgr::Instance().HotDisplayVideo(cszCallID,hWndLocal,hWndRemote,hWndSecond);
}

/******************************************************************************
* GetCallH323ID
*描述：获取H323ID
*输入：	cszCallID		-呼叫标识
szH323ID        -H323ID Buf
nMaxLen         -Buf 最大长度
*输出：	无
*返回值：获取到返回H323ID长度，否则返回NULL
*****************************************************************************/
int IMXEC_Call::GetCallH323ID(const char*cszCallID ,char * szH323ID,int nMaxLen)
{
	return CallMgr::Instance().GetCallH323ID(cszCallID ,szH323ID,nMaxLen);
}

/******************************************************************************
* GetCallE164ID
*描述：获取E164ID
*输入：	cszCallID		-呼叫标识
szE164ID        -E164ID Buf
nMaxLen         -Buf 最大长度
*输出：	无
*返回值：获取到返回E164ID长度，否则返回NULL
*****************************************************************************/

int IMXEC_Call::GetCallE164ID(const char*cszCallID ,char * szE164ID,int nMaxLen)
{
	return CallMgr::Instance().GetCallE164ID(cszCallID ,szE164ID,nMaxLen);
}

/******************************************************************************
* SetNetecTransEnable
*描述：是否使用NETEC私有传流
*输入：	cszCallID			-呼叫标识
        nEnable             -是否使用,1为使用,0为不使用
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetNetecTransEnable(const char*cszCallID,int nEnable)
{
    CallMgr::Instance().SetNetecTransEnable(cszCallID,nEnable);
}

/******************************************************************************
* SetResolutionStand
*描述：设置终端支持的最大分辨率能力
*输入：	nResStand			-最大分辨率能力
*输出：无
*返回值：无
*****************************************************************************/
void IMXEC_Call::SetResolutionStand(ResStandard nResStand)
{
    CallMgr::Instance().m_nResStand =nResStand;
}


#ifdef _TIANZHU_CACHE_
int IMXEC_Call::SetRemoteVideoCache(const char*cszCallID, int nCacheFrameCount)
{
	return CallMgr::Instance().SetRemoteVideoCache(cszCallID,nCacheFrameCount);
}
#endif