//IMXCall.h
#ifndef __IMXCALL_H__
#define __IMXCALL_H__

#include <string>
#include "../IMXEC_Call/Callbase.h"
#include <IMXEC/IMXEC_MediaChannel.h>

class IMXCall
	: public CallBase
	, public IMXEC_PlayChanCallback
{
public:
	IMXCall(const std::string&strUserID,const std::string&strUserName,const std::string&strCallID);
	virtual~IMXCall(void);
public:
	typedef enum
	{
		CALL_DIR_IN=0,
		CALL_DIR_OUT,
		CALL_DIR_COUNT,
	}CALL_DIRECTION;

	typedef enum
	{
		CALL_STATUS_IDLE=0,
		CALL_STATUS_RING,
		CALL_STATUS_ESTABLISHED,
	}CALL_STATUS;
public:
	virtual void Close(void);

	virtual int AcceptCall(void);
	virtual int HangupCall(void);

	virtual int OpenPlay(unsigned long nAudioID,unsigned long nVideoID,const char*cszPeerNodeID,const char*cszPeerNATIP,unsigned short nPeerNATPort,const char*cszPeerLocalIP,unsigned short nPeerLocalPort,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,int nForceUseMCU);

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
	virtual int GetLocalMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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
	virtual int GetRemoteMainVideoInfo(int&nWidth, int&nHeight,VIDEC_CODEC_TYPE&nVideoCodecType,HWND&hVideoWnd);

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

	/******************************************************************************
	* GetMainVideoID
	*描述：获取主流视频标识
	*输入： 无
	*输出： nMainVideoID	-主流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetMainVideoID(unsigned long&nMainVideoID);

	/******************************************************************************
	* GetSecondVideoID
	*描述：获取辅流视频标识
	*输入： 无
	*输出： nSecondVideoID	-辅流视频标识
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	virtual int GetSecondVideoID(unsigned long&nSecondVideoID);

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

#ifdef _TIANZHU_CACHE_
	/******************************************************************************
	* SetRemoteVideoCache
	*描述：设置远端视播放的缓冲帧数
	*输入：nCacheFrameCount	-缓冲的帧数（0 - 60）
	*输出：无
	*返回值：成功返回0，失败返回-1
	*****************************************************************************/
	int SetRemoteVideoCache(int nCacheFrameCount);
#endif

protected:
	//音频数据回调函数
	virtual void OnIMXEC_PlayChanCallbackAudChanData(unsigned char*pData,int nLen,bool&bLoop){};
	virtual void OnIMXEC_PlayChanCallbackAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
	//视频数据回调函数
	//nKeyFrame,0:非关键帧 1:关键帧
	virtual void OnIMXEC_PlayChanCallbackVidChanData(unsigned char*pData,int nLen,int nKeyFrame,int& nFlagDontDecode){};
protected:
	std::string					m_strUserID;
	std::string					m_strUserName;

	CALL_DIRECTION				m_CallDirection;
	CALL_STATUS					m_CallStatus;
	std::string					m_strCallID;

	IMXEC_PlayChan*				m_pRemoteMainPlayChan;
	HWND						m_hVideoWndRemoteMain;
};

#endif
