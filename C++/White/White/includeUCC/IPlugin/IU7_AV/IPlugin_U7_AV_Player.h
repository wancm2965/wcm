// UCC7-音视频播放功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	class IPlugin_U7_AV_Player_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Player_Observer(){};

	public:
		//音频数据回调函数
		virtual void OnPlayChanAudioData(unsigned char*pData,int nLen,bool&bLoop) = 0;
		virtual void OnPlayChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp) = 0;
		//视频数据回调函数
		//nKeyFrame,0:非关键帧 1:关键帧
		virtual void OnPlayChanVideoData(unsigned char*pData,int nLen,int nKeyFrame) = 0;
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
		virtual void Close(void) = 0;

		virtual void RecvAudio(bool bRecv) = 0;
		virtual void RecvVideo(bool bRecv) = 0;

		virtual int GetAudioLevel(void) = 0;

		// 帧率控制 0：全帧率 1：半帧率 2：自动帧率
		virtual void SetFrameRateControlMode(int nMode) = 0;
		// 码流控制 0：主流 1：子流 2：辅流
		virtual void SetFrameTypeControlMode(int nMode) = 0;

		// 设置视频属性
		// 亮度值nBrightness[0-255 128] 对比度nContrast[0-127 64] 饱和度nSaturation[0-127 64] 色调nHue[0-255 0] 伽马nGamma[1-400 100] 锐化nSharpen[0-127 0]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,int nGammaR,int nGammaG,int nGammaB)=0;

		virtual void PlayAudioData(const void* pData, int nLen) = 0;
		virtual void PlayVideoData(const void* pData, int nLen) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;

		//保存当前图象到BMP文件
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// 获取视频分辨率
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		/* 获取音视频的包的统计数据
		ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
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
	};
}
