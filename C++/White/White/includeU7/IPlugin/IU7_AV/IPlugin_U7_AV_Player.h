// UCC7-音视频播放功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "AVCONSys/AUDEC/AUDEC_CodecID.h"

namespace AVCONPlugin
{
	class IPlugin_U7_AV_Player_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Player_Observer(){};

	public:
		//音频数据回调函数
		virtual void OnPlayChanAudioData(unsigned char*pData,int nLen,bool&bLoop){};
		virtual void OnPlayChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
		//视频数据回调函数
		//nKeyFrame,0:非关键帧 1:关键帧
		virtual void OnPlayChanVideoData(unsigned char*pData,int nLen,int nKeyFrame){};

		virtual void OnPlayChanRecordStart(){};
	};

	class IPlugin_U7_AV_Player
	{
	public:
		virtual ~IPlugin_U7_AV_Player(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_Player_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_Player_Observer* pObserver) = 0;

		virtual HWND GetHWnd(void) = 0;
		virtual const char* GetMemberID(void) = 0;
		virtual int GetCardIndex(void) = 0;

		virtual int Open(bool bLocalPlay) = 0;
		virtual int Open(unsigned long nAudChanID, unsigned long nVidChanID, const char*cszPeerNodeID, 
			const char*cszPeerNATIP, unsigned short nPeerNATPort, const char*cszPeerLocalIP, unsigned short nPeerLocalPort, 
			const char*cszPeerMCUID, const char*cszPeerMCUIP, unsigned short nPeerMCUPort, int nForceUseMCU = 0) = 0;
		virtual void Close(void) = 0;

		virtual void RecvAudio(bool bRecv) = 0;
		virtual void RecvVideo(bool bRecv) = 0;

		virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen)=0;

		virtual int SetVolume(int nVolume) = 0;
		virtual int GetAudioLevel(void) = 0;

		// 帧率控制 0：全帧率 1：半帧率 2：自动帧率
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// 获取当前帧率
		virtual int GetFrameRateControlMode() = 0;

		// 码流控制 0：主流 1：子流 2：辅流
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// 获取当前码流
		virtual int GetFrameTypeControlMode() = 0;

		// 设置视频属性
		// 亮度值nBrightness[0-255 128] 对比度nContrast[0-127 64] 饱和度nSaturation[0-127 64] 色调nHue[0-255 0] 伽马nGamma[1-400 100] 锐化nSharpen[0-127 0]
		// 锐化仅应用到本地bSharpenLocalOnly[默认值为true]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// 按幅度调整bAjustAmp[默认值为false] 为true时会在原有值的基础上加上传入的参数值
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;
		virtual void GetVideoParam(int& nBrightness, int& nContrast, int& nSaturation, int& nHue, int& nGamma,int& nSharpen, bool& bSharpenLocalOnly,
			int& nGammaR, int& nGammaG, int& nGammaB) = 0;

		virtual void PlayAudioData(const void* pData, int nLen) = 0;
		virtual void PlayVideoData(const void* pData, int nLen) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// 获取视频显示模式 0：填充 1：全景
		virtual int GetVideoShowMode(void) = 0;

		//保存当前图象到BMP文件
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// 请求关键帧
		virtual void RequestKeyFrame() = 0;

		// 录像支持格式：avms、avi、mkv、mp4
		virtual bool StartRecord(const char* cszPathName) = 0;
		virtual void StopRecord(void) = 0;

		// 获取视频分辨率
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;

		/* 获取音视频的包的统计数据
		ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate) = 0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate) = 0;

		//码流锁定切换
		virtual void SetSreamLocked(bool bStreamLocked) = 0;
		virtual bool GetStreamLocked() =0;

		// 获取其他信息
		virtual bool GetMemberCardInfo(unsigned long &audiocid, unsigned long &videocid, std::string &nodeid, std::string &nataddr, std::string &localaddr, unsigned short &localport, std::string &mcuid, std::string &mcuaddr, unsigned short &mcuport) = 0;
		virtual bool GetMonChannelInfo(unsigned long &audiocid, unsigned long &videocid, std::string &nodeid, std::string &nataddr, std::string &localaddr, unsigned short &localport, std::string &mcuid, std::string &mcuaddr, unsigned short &mcuport) = 0;

		virtual int StartMixingAudio(X_AUDIO_CODEC_ID nCodecID)=0;
		virtual void StopMixingAudio(void)=0;
		virtual int GetMixingAudioID(unsigned long&nAudioID)=0;
		//获得音频通道ID
		virtual unsigned long GetAudioID(void) = 0;
		//发送控制数据
		virtual void SendCtrlData(void* pData,int nLen) = 0;
		//是否允许打开接收音视频
		virtual void SetAllowRecvAV(bool bAllow) = 0;
	};

	class IPlugin_U7_AV_PlayerMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_PlayerMgr(){};

	public:
		virtual IPlugin_U7_AV_Player* CreateU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Player* GetU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Player* GetU7AVPlayer(HWND hWnd) = 0;
		virtual void DestroyU7AVPlayer(const char* strMemberID, int nCardIndex) = 0;
		virtual void DestroyU7AVPlayer(void) = 0;

		// 帧率控制 0：全帧率 1：半帧率 2：自动帧率
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// 码流控制 0：主流 1：子流 2：辅流
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// 获取视频显示模式 0：填充 1：全景
		virtual int GetVideoShowMode(void) = 0;

		// 启动\停止 会议录像时调用
		virtual void SetConferenceRecording(int nFlagConferenceRecording) = 0;

		// 设置所有Player视频属性
		// 亮度值nBrightness[0-255 128] 对比度nContrast[0-127 64] 饱和度nSaturation[0-127 64] 色调nHue[0-255 0] 伽马nGamma[1-400 100] 锐化nSharpen[0-127 0]
		// 锐化仅应用到本地bSharpenLocalOnly[默认值为true]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// 按幅度调整bAjustAmp[默认值为false] 为true时会在原有值的基础上加上传入的参数值
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;

		// 获取当前会议下载总带宽
		virtual unsigned long GetDownloadTotalBitrate(void) = 0;

		// 强制接收视频
		virtual void SetForcibleReceiving(bool bForcible) = 0;

		virtual int SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy) = 0;

		virtual void ChangeOwner(HWND hWnd, const char* strMemberID, int nCardIndex) = 0;

		//功能：设置轮循参数
		virtual void SetRRParamter(const char* cmd, const char* strParam) = 0;
	};
}
