//MediaCall.h
#ifndef __MEDIACALL_H__
#define __MEDIACALL_H__

#include <string>
#include <VIDEC/VIDEC.h>
#include <VIDEC/VIDEC_Header.h>
#include <VIDEC/VIDEC_CameraDev.h>
#include <VIDEC/VIDEC_AH2KDev.h>
#include <VIDEC/VIDEC_AH400Dev.h>
#include <VIDEC/VIDEC_AVIWriter.h>
#include <AUDEC/AUDEC_Engine.h>
#include <AUDEC/AUDEC_CodecID.h>
#include <AUDEC/AUDEC_Header.h>
#include <AUDEC/AUDEC_Conference.h>
#include <IMXEC/IMXEC_Call.h>

#include <NETEC/NETEC_MediaSender.h>
#include <NETEC/NETEC_Node.h>
#include <NETEC/NETEC_RTP2Frame.h>
#include <NETEC/NETEC_RTPJBuf.h>
#include <IMXEC/IMXEC_MediaChannel.h>

//Add WebRTC by zys 20160802
#include <WEBRTC/X86/my_voice_engine_interface.h>
using namespace voice_engine;

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

#include <MPVideo/MPVideo.h>
#include <MPVideo/MPVideo_Transcode.h>

#endif


#include "CapChanRTP.h"
#include "PlayChanRTP.h"
#include "XBitrate.h"
#include "XAutoLock.h"

class MediaCall
	: public CapChanRTPCallback
	, public PlayChanRTPCallback
	, public VIDEC_CapChanCallback
	, public NETEC_MediaSenderCallback
	, public NETEC_RTP2FrameCallback
	, public NETEC_RTPJBufCallback
	, public IMXEC_PlayChanCallback
	, public AUDEC_DTMFCallback		//add by Jan for DTMF [20140730]
	, public VIDEC_WndCallback
	, public Transport              //Add WebRTC by zys 20160802
{
public:
	MediaCall(void);
	virtual ~MediaCall(void);
public:
	virtual void Close(void);
	/******************************************************************************
	* SetLocalMainVideoWnd
	*描述：设置本地主流视频显示的窗口
	*输入：	hWnd			-本地主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetLocalMainVideoWndPos
	*描述：设置本地主流视频显示的窗口位置
	*输入：	x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetLocalMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetLocalMainVideoParam
	*描述：设置本地主流视频参数
	*输入：	nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetLocalMainVideoEnableOSD
	*描述：设置本地主流视频是否启用OSD
	*输入：	cszCallID		-呼叫标识
			nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetLocalMainVideoOSDText
	*描述：设置本地主流视频OSD文字
	*输入：	cszCallID		-呼叫标识
			cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetLocalMainVideoOSDFont
	*描述：设置本地主流视频OSD字体,字的颜色和背景色
	*输入：	logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalMainVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetLocalMainVideoInfo
	*描述：获取本地主流视频信息
	*输入：	无
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate);

	/******************************************************************************
	* GetLocalMainVideoPacketStatistics
	*描述：获取本地主视频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetRemoteMainVideoWnd
	*描述：设置远程主流视频显示的窗口
	*输入：	无
			hWnd			-远程主视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetRemoteMainVideoWndPos
	*描述：设置远程主流视频显示的窗口位置
	*输入：	x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetRemoteMainVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetRemoteMainVideoParam
	*描述：设置远程主流视频参数
	*输入：	nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteMainVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen);

	/******************************************************************************
	* GetRemoteMainVideoInfo
	*描述：获取远程主流视频信息
	*输入：	无
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd,int &nFrameRate);

	/******************************************************************************
	* GetRemoteMainVideoPacketStatistics
	*描述：获取远端主视频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteMainVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	/******************************************************************************
	* SetSecondVideoWnd
	*描述：设置辅流视频显示的窗口
	*输入：无
			hWnd			-远程辅流视频显示的窗口句柄
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSecondVideoWnd(HWND hWnd);

	/******************************************************************************
	* SetSecondVideoWndPos
	*描述：设置辅流视频显示的窗口位置
	*输入：	x				-窗口的左边位置
			y				-窗口的上边位置
			cx				-窗口的宽度
			cy				-窗口的高度
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSecondVideoWndPos(int x,int y,int cx,int cy);

	/******************************************************************************
	* SetSecondVideoParam
	*描述：设置辅流视频参数
	*输入：	nBrightness		-亮度值0-255
			nContrast		-对比度0-127
			nSaturation		-饱和度0-127
			nHue			-色调0-255
			nGamma			-伽马1-400
			nSharpen		-锐化0-127
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly);

	/******************************************************************************
	* SetSecondVideoEnableOSD
	*描述：设置辅流视频是否启用OSD
	*输入：	nEnable			-是否启用OSD
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoEnableOSD(int nEnable);

	/******************************************************************************
	* SetSecondVideoOSDText
	*描述：设置辅流视频OSD文字
	*输入：	cszText			-OSD文字
			nAlign			-对齐方式
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoOSDText(const char*cszText,int nAlign=VIDEC_OSD_ALIGN_TOP_LEFT);//Align==0 TOP_LEFT 1:BOTOM_LEFT,2:TOP_CENTER,3:BOTTOM_CENTER,4:TOP_RIGHT,5:BOTTOM_RIGHT

	/******************************************************************************
	* SetSecondVideoOSDFont
	*描述：设置辅流视频OSD字体,字的颜色和背景色
	*输入：	logfont			-OSD字体
			crText			-字的颜色
			crBk			-背景色
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoOSDFont(LOGFONTA logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255));

	/******************************************************************************
	* GetSecondVideoInfo
	*描述：获取辅流视频信息
	*输入：	无
	*输出：	nWidth			-图像宽度
			nHeight			-图像高度
			nVideoCodecType	-编码类型
			hVideoWnd		-窗口句柄
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSecondVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);
	
	/******************************************************************************
	* GetSecondVideoPacketStatistics
	*描述：获取辅流视频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSecondVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenLocalMainVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType=0,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual void OnRequestLocalMainVideoKeyFrame(void);
	virtual void CloseLocalMainVideoCapDev(void);

	/******************************************************************************
	* GetLocalAudioLevel
	*描述：获取本地音频Level【实际声音大小】
	*输入：	无
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalAudioLevel(int&nLevel);

	/******************************************************************************
	* SetLocalAudioVolume
	*描述：获取本地音频音量
	*输入：	nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalAudioVolume(int nVolume);

	/******************************************************************************
	* SetLocalAudioMute
	*描述：获取本地音频是否静音
	*输入：	nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalAudioMute(int nMute);

	/******************************************************************************
	* GetLocalAudioCodecID
	*描述：获取本地音频编码信息
	*输入：	无
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalAudioCodecID(X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetLocalAudioPacketStatistics
	*描述：获取本机音频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetLocalAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenLocalAudioCapDev(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);
	virtual void CloseLocalAudioCapDev(void);

	//virtual int OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort);
	virtual int OpenRemoteMainPlayChan(unsigned long nAudioID,unsigned long nVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID,unsigned long nEnableMulticast);
	virtual int OpenRemoteSecondPlayChan(unsigned long nSecondVideoID,const std::string&strPeerLocalIP,unsigned short nPeerLocalPort,const std::string&strPeerMCUID,unsigned long nEnableMulticast);
	virtual void CloseRemoteSecondPlayChanMedia();

	virtual int OpenRemoteMainVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteMainVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);
	virtual void CloseRemoteMainVideoPlayChan();

	/******************************************************************************
	* GetRemoteAudioLevel
	*描述：获取远程音频Level【实际声音大小】
	*输入：	无
	*输出：	nLevel			-实际声音大小【0-10】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteAudioLevel(int&nLevel);

	/******************************************************************************
	* SetRemoteAudioVolume
	*描述：获取远程音频音量
	*输入：	nVolume			-音量
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteAudioVolume(int nVolume);

	/******************************************************************************
	* SetRemoteAudioMute
	*描述：获取远程音频是否静音
	*输入：	nMute			-是否静音
	*输出：	无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteAudioMute(int nMute);

	/******************************************************************************
	* GetRemoteAudioCodecID
	*描述：获取远端音频编码
	*输入：	无
	*输出：	nAudioCodecID	-编码ID
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteAudioCodecID (X_AUDIO_CODEC_ID &nAudioCodecID);

	/******************************************************************************
	* GetRemoteAudioPacketStatistics
	*描述：获取远端音频的包的统计数据
	*输入：	无
	*输出： ulTotalPackets		-总共发送了多少个包
			ulLostPackets		-总共丢弃了多少个包
			AvgPacketLossRate	-平均丢包率
			CurPacketLossRate	-当前丢包率
			nBitrate			-码流【bps】
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetRemoteAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate);

	virtual int OpenRemoteAudioPlayChan(X_AUDIO_CODEC_ID AudioCodecID,int nPayloadType);
	virtual void OnRecvdRemoteAudioRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteAudioRTPPacketJBuf(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void CloseRemoteAudioPlayChan(void);

	virtual int OpenLocalSecondVideoCapDev(int nDevID,int nWidth,int nHeight,int nFrameRate,VIDEC_CODEC_TYPE codecType,int nBitrate,int nNetworkType=VIDEC_NETWORK_PRIVATE);
	virtual void OnRequestSecondVideoKeyFrame(void);
	virtual void CloseLocalSecondVideoCapDev(void);

	virtual int OpenRemoteSecondVideoPlayChan(VIDEC_CODEC_TYPE codecType,int nBitrate,int nPayloadType);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen,int nPayloadType,int nRTPHeaderLen,unsigned short usSequence,unsigned long ulTimestamp);
	virtual void OnRecvdRemoteSecondVideoRTPPacket(unsigned char*pRTPPacketData,int nRTPPacketLen);
	virtual void CloseRemoteSecondVideoPlayChan(void);

	/******************************************************************************
	* StartRecordLocalMainToAVI
	*描述：开始本地主流录像
	*输入：	cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordLocalMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordLocalMainToAVI
	*描述：停止本地主流录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordLocalMainToAVI(void);

	/******************************************************************************
	* StartRecordRemoteMainToAVI
	*描述：开始远端主流录像
	*输入：	cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordRemoteMainToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordRemoteMainToAVI
	*描述：停止远端主流录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordRemoteMainToAVI(void);

	/******************************************************************************
	* StartRecordSecondToAVI
	*描述：开始辅流录像
	*输入：	cszPathName			-录像文件名称
			nStandardFormat		-是否录制成标准文件格式
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int StartRecordSecondToAVI(const char*cszPathName,int nStandardFormat=1);
	
	/******************************************************************************
	* StopRecordSecondToAVI
	*描述：停止辅流录像
	*输入：无
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void StopRecordSecondToAVI(void);

	/******************************************************************************
	* GetMainAudioID
	*描述：获取主流音频标识
	*输入： 无
	*输出： nMainAudioID	-主流音频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetMainAudioID(unsigned long&nMainAudioID);
	virtual int GetLocalMainAudioID(unsigned long&nMainAudioID);

	/******************************************************************************
	* GetMainVideoID
	*描述：获取主流视频标识
	*输入： 无
	*输出： nMainVideoID	-主流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetMainVideoID(unsigned long&nMainVideoID);
	virtual int GetLocalMainVideoID(unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*描述：获取辅流视频标识
	*输入： 无
	*输出： nSecondVideoID	-辅流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSecondVideoID(unsigned long&nSecondVideoID);
	virtual int GetLocalSecondVideoID(unsigned long&nSecondVideoID);

	/******************************************************************************
	* SetMainVideoScreenIndex
	*描述：设置主流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetMainVideoScreenIndex(int nScreenIndex);

	/******************************************************************************
	* SetSecondVideoScreenIndex
	*描述：设置辅流视频图像桌面序号
	*输入： cszCallID		-呼叫标识
			nScreenIndex	-桌面序号
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SetSecondVideoScreenIndex(int nScreenIndex);


	/******************************************************************************
	* SetSecondVideoScreenWnd
	*描述：设置辅流视频所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetRemoteVideoScreenWnd
	*描述：设置远端视频所在屏幕窗口位置
	*输入：	nScreenIndex	-屏幕号
			nWndIndex		-窗口号
			nFullScreen		-是否全屏显示【0：普通显示，非0：全屏显示】
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteVideoScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen);

	/******************************************************************************
	* SetLocalRecordCallback
	*描述：设置本地录像回调
	*输入：	pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetLocalRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetRemoteRecordCallback
	*描述：设置远程录像回调
	*输入：pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetRemoteRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SetSecondRecordCallback
	*描述：设置辅流录像回调
	*输入：	pRecordCallback	-录像回调
	*输出：无
	*返回值：无
	*****************************************************************************/
	virtual void SetSecondRecordCallback(IMXEC_RecordCallback*pRecordCallback);

	/******************************************************************************
	* SaveLocalMainVideoToBMPFile
	*描述：保存本地主流视频到BMP图片文件
	*输入：	cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveLocalMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveRemoteMainVideoToBMPFile
	*描述：保存远程主流视频到BMP图片文件
	*输入：	cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveRemoteMainVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* SaveSecondVideoToBMPFile
	*描述：保存辅流视频到BMP图片文件
	*输入：	cszPathName		-图片文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int SaveSecondVideoToBMPFile(const char*cszPathName);

	/******************************************************************************
	* StartMixingWithWaveFile
	*描述：开始对呼叫进行WAVE文件混音，对方会听到WAVE文件播放的声音
	*输入：	cszPathName		-WAVE文件全路径
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StartMixingWithWaveFile(const char*cszPathName);

	/******************************************************************************
	* StopMixingWithWaveFile
	*描述：停止对呼叫进行WAVE文件混音
	*输入：无
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int StopMixingWithWaveFile(void);

	virtual int GetSecondVideoDirection(IMXEC_CALL_DIRECTION&nDir){nDir=IMXEC_CALL_DIR_UNKOWN;return -1;}

	virtual void OnNETEC_MediaSenderCallbackRecvdPTZ(void*pData,int nLen);
	virtual void OnNETEC_MediaSenderCallbackRecvdCtrl(void*pData,int nLen);
	//nSkipFlag,1:跳过这帧不压缩，0:压缩当前帧
	virtual void OnVIDEC_CapChanCallbackPreEncode(VIDEC_CapChan*pCapChan,int&nSkipFlag);

	virtual int RequestMainVideoKeyFrame(void)=0;
	virtual int RequestSecondVideoKeyFrame(void)=0;

	virtual void ExcludePlayChan(AUDEC_PlayChan*pPlayChan);

	virtual void SendAudio(char*pRTPPacketData,int nRTPPacketLen)=0;
	virtual void SendMainVideo(char*pRTPPacketData,int nRTPPacketLen)=0;
	virtual void SendSecondVideo(char*pRTPPacketData,int nRTPPacketLen)=0;

	virtual void OnRecvDTMF(char dtmf);	//add by Jan for DTMF [20140730]

protected:
	virtual void OnCapChanRTPCallbackSendVideo(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnCapChanRTPCallbackSendAudio(CapChanRTP*pCapChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnPlayChanRTPCallbackVideoPacket(PlayChanRTP*pPlayChanRTP,void*pPacketData,int nPacketLen);
	virtual void OnNETEC_RTP2FrameCallbackFramePacket(NETEC_RTP2Frame*pRTP2Frame,unsigned char*pPacketData,int nPacketLen,int nKeyFrame);
	virtual void OnNETEC_RTPJBufCallbackRTPPacket(NETEC_RTPJBuf*pRTPJBuf,unsigned char*pRTPPacketData,int32_t nRTPPacketLen,int32_t nPayloadType,int32_t nRTPHeaderLen,unsigned short nSeqNo,uint32_t nTimestamp);

	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop);
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode);

	virtual bool HasAVCONTransport(void){return false;};

	//Add WebRTC by zys 20160802
	virtual bool SendRtp(const uint8_t* packet, size_t length, const PacketOptions& options);
	virtual bool SendRtcp(const uint8_t* packet, size_t length);

	//添加AUDEC头
	virtual int SetAudecHeader(unsigned char* pData,void const* pSrcData,size_t nlen);

	//AUDEC头转RTP头
	virtual int AudecHeaderToRTPHeader(unsigned char* pRTPData,void const* pAudecData,size_t nAudecLen);

protected:

	HWND					m_hLocalMainVideoWnd;
	VIDEC_Wnd*				m_pLocalMainVideoWnd;
	VIDEC_CapDev*			m_pLocalMainVideoCapDev;
	VIDEC_CapChanRTP*		m_pLocalMainVideoCapChan;
	VIDEC_CODEC_TYPE		m_nLocalMainVideoCodecType;
	CapChanRTP				m_LocalMainCapChanRTP;
	int						m_nLocalMainVideoBrightness;
	int						m_nLocalMainVideoContrast;
	int						m_nLocalMainVideoSaturation;
	int						m_nLocalMainVideoHue;
	int						m_nLocalMainVideoGamma;
	int						m_nLocalMainVideoSharpen;
	bool					m_bLocalMainVideoSharpenLocalOnly;

	int						m_nLocalMainVideoEnableOSD;
	std::string				m_strLocalMainVideoOSD;
	int						m_nLocalMainVideoAlign;
	COLORREF				m_crLocalMainVideoText;
	COLORREF				m_crLocalMainVideoBk;
	LOGFONTA				m_fontLocalMainVideoOSD;
	XBitrate				m_XBitrateLocalMainVideo;

	HWND					m_hRemoteMainVideoWnd;
	VIDEC_Wnd*				m_pRemoteMainVideoWnd;
	VIDEC_PlayChanRTP*		m_pRemoteMainVideoPlayChan;
	VIDEC_CODEC_TYPE		m_nRemoteMainVideoCodecType;
	int						m_nRemoteMainVideoBrightness;
	int						m_nRemoteMainVideoContrast;
	int						m_nRemoteMainVideoSaturation;
	int						m_nRemoteMainVideoHue;
	int						m_nRemoteMainVideoGamma;
	int						m_nRemoteMainVideoSharpen;
	XBitrate				m_XBitrateRemoteMainVideo;

	HWND					m_hSecondVideoWnd;
	VIDEC_CapDev*			m_pSecondVideoCapDev;
	VIDEC_CapChanRTP*		m_pSecondVideoCapChan;
	VIDEC_Wnd*				m_pSecondVideoWnd;
	VIDEC_PlayChanRTP*		m_pSecondVideoPlayChan;
	VIDEC_CapChan*			m_pSecondVideoTranscoder;
	VIDEC_CapChan*			m_pMainVideoTranscoder;
	VIDEC_CODEC_TYPE		m_nSecondVideoCodecType;
	CapChanRTP				m_LocalSecondCapChanRTP;
	int						m_nSecondVideoBrightness;
	int						m_nSecondVideoContrast;
	int						m_nSecondVideoSaturation;
	int						m_nSecondVideoHue;
	int						m_nSecondVideoGamma;
	int						m_nSecondVideoSharpen;
	bool					m_bSecondVideoSharpenLocalOnly;
	int						m_nSecondVideoEnableOSD;
	std::string				m_strSecondVideoOSD;
	int						m_nSecondVideoAlign;
	COLORREF				m_crSecondVideoText;
	COLORREF				m_crSecondVideoBk;
	LOGFONTA				m_fontSecondVideoOSD;
	XBitrate				m_XBitrateSecondVideo;

	AUDEC_CapChan*			m_pLocalAudioCapChan;
	unsigned char*			m_pLocalAudioCapChanBuf;
	int						m_nLocalAudioCapChanBufLen;
	unsigned long			m_ulLocalAudioCapChanRTPTimestamp;
	int						m_nLocalAudioCapChanRTPPayloadType;
	X_AUDIO_CODEC_ID		m_nLocalAudioCodecID;
	int						m_nLocalAudioMute;
	int						m_nLocalAudioVolume;

	AUDEC_PlayChan*			m_pRemoteAudioPlayChan;
	X_AUDIO_CODEC_ID		m_nRemoteAudioPlayChanCodecID;
	unsigned char*			m_pRemoteAudioPlayChanBuf;
	int						m_nRemoteAudioPlayChanBufLen;
	int						m_nRemoteAudioMute;
	int						m_nRemoteAudioVolume;
	int						m_nRemoteAudioPayloadType;

	int						m_nMainVideoScreenIndex;
	int						m_nSecondVideoScreenIndex;
	VIDEC_MPScreen*			m_pMainVideoMPScreen;
	VIDEC_MPScreen*			m_pSecondVideoMPScreen;

	AUDEC_Mixer*			m_pAudioMixer;
	AUDEC_PlayChan*			m_pExcludePlayChan;

	NETEC_MediaSender*		m_pMediaSenderRemoteMain;
	NETEC_MediaSender*		m_pMediaSenderRemoteSecond;
	NETEC_RTP2Frame*		m_pRTP2FrameRemoteMain;
	NETEC_RTP2Frame*		m_pRTP2FrameRemoteSecond;
	unsigned short			m_nSeqNoRemoteMain;
	unsigned short			m_nSeqNoRemoteSecond;

	NETEC_MediaSender*		m_pMediaSenderLocalMain;
	NETEC_RTP2Frame*		m_pRTP2FrameLocalMain;
	unsigned short			m_nSeqNoLocalMain;

	NETEC_MediaSender*		m_pMediaSenderLocalSecond;
	NETEC_RTP2Frame*		m_pRTP2FrameLocalSecond;
	unsigned short			m_nSeqNoLocalSecond;

	PlayChanRTP				m_RemoteMainPlayChanRTP;
	PlayChanRTP				m_RemoteSecondPlayChanRTP;

	std::string				m_strSecondRecordPathName;
	int						m_nSecondRecordIndex;

	XCritSec				m_csLocalRecordCallback;
	IMXEC_RecordCallback*	m_pLocalRecordCallback;
	XCritSec				m_csRemoteRecordCallback;
	IMXEC_RecordCallback*	m_pRemoteRecordCallback;
	XCritSec				m_csSecondRecordCallback;
	IMXEC_RecordCallback*	m_pSecondRecordCallback;


	NETEC_RTPJBuf*			m_pRTPJBufMainVideo;
	NETEC_RTPJBuf*			m_pRTPJBufSecondVideo;
	NETEC_RTPJBuf*			m_pRTPJBufAudio;

	IMXEC_PlayChan*			m_pRemoteMainPlayChan;
	IMXEC_PlayChan*			m_pRemoteSecondPlayChan;
	bool					m_bRecording;

#if ( (defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1) ) || ( defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1) ) )

	MPVideo_Transcode*		m_pTranscodeRemoteMain;
	MPVideo_Transcode*		m_pTranscodeRemoteSecond;
#endif

	//Add WebRTC by zys 20160929
	MyVoiceEngineInterface* m_pWebRTCAudio;
	int m_nWebRTCPlayChan;

	int m_nRTPTimestamp;
	int m_nRTPSeqNo;
	int m_nAudioSeqNo;

public:
	//notice netec send ptz 
	int MediaSendCtrl(void*pData,int nLen);
	int MediaSendPTZ(void*pData,int nLen);
	virtual void OnNETEC_RecvdPTZ(void*pData,int nLen) {};
	virtual void OnNETEC_RecvdCtrl(void*pData,int nLen) {};
	virtual void OnAudioRecvDTMF(char dtmf) {};//add by Jan for DTMF [20140730]
	int ReSetMainAudioCapBitrate(int nBitrate);
	int ReSetMainVideoCapBitrate(int nBitrate);
	int ReSetSecondVideoCapBitrate(int nBitrate);

	virtual int OnNETECRequestMainVideoKeyFrame(void)=0;
	virtual int OnNETECRequestSecondVideoKeyFrame(void)=0;

	int m_nMainVideoFlowCCBitRate;          //主流收到的流控码率 单位100 bit/s 默认为1
	int m_nDualVideoFlowCCBitRate;          //辅流收到的流控码率 单位100 bit/s 默认为1
	int m_nAudioFlowCCBitRate;              //辅流收到的流控码率 单位100 bit/s 默认为1
	//XCritSec		m_csVideoSender;        //锁控

};


#endif