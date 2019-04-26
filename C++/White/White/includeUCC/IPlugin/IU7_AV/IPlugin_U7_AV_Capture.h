// UCC7-音视频采集功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	typedef enum _CAP_TYPE
	{
		CAP_TYPE_AV		= 0,
		CAP_TYPE_SCREEN,
		CAP_TYPE_MEDIA,
		CAP_TYPE_COMPOUND,
		CAP_TYPE_IPCAM,
	}CAP_TYPE;

	typedef enum _VIDEC_CODEC
	{
		CODEC_H263P		= 0,
		CODEC_H264,
		CODEC_H264_SVC,
	}VIDEC_CODEC;

	class IPlugin_U7_AV_Capture_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_Capture_Observer(){};

	public:
		//音频数据回调函数
		virtual void OnCapChanAudioData(unsigned char*pData,int nLen,bool&bLoop){};
		virtual void OnCapChanAudioSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp){};
		//视频数据回调函数
		//nKeyFrame,0:非关键帧 1:关键帧
		virtual void OnCapChanVideoData(unsigned char*pData,int nLen,int nKeyFrame){};

		virtual void OnCapChanFilePositionChanged(unsigned long ulSeconds){};
		virtual void OnCapChanFilePause(void){};
		virtual void OnCapChanFilePlay(void){};
		virtual void OnCapChanFileEnd(void){};
		virtual void OnCapChanRecvdCtrlData(void*pData,int nLen){};

		virtual void OnCapChanAudioIDChanged(unsigned long nAudioID){};
		virtual void OnCapChanVideoDevOpen(void){};

		virtual void OnCapChanCallbackFilePositionChanged(unsigned long ulSeconds){};
		virtual void OnCapChanCallbackFilePause(void){};
		virtual void OnCapChanCallbackFilePlay(void){};
		virtual void OnCapChanCallbackFileEnd(void){};
		virtual void OnCapChanCallbackRecvdCtrlData(void*pData,int nLen){};
	};

	class IPlugin_U7_AV_Capture
	{
	public:
		virtual ~IPlugin_U7_AV_Capture(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_Capture_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_Capture_Observer* pObserver) = 0;

		virtual HWND GetHWnd(void) = 0;
		virtual int GetCardIndex(void) = 0;

		// 打开视频设备
		virtual int OpenVideo(int nDevID, int nSizeIndex, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub,bool bOpenOSD = true) = 0;
		//virtual int OpenVideo(int x, int y, int cx, int cy, int nBitCount, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		virtual int OpenVideo(const char*cszMediaFile, int nLoop, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		//virtual int OpenVideo(int nScreenIndex, int nScreenType, int nWidth, int nHeight, int nVideoFrame, int nBitrate, int nBitrateCtrlType, VIDEC_CODEC CodecType, int nSub, int nQSub) = 0;
		//virtual int OpenVideo(int nSizeIndex,int nVideoFrame,int nBitrate,int nBitrateCtrlType,int nSub,int nQSub) = 0;
		// 关闭视频
		virtual void CloseVideo(void) = 0;

		// 打开音频
		// nDevID:音频采集设备号，-2从采集卡中采集音频，>=-1从声卡中采集音频
		virtual int OpenAudio(int nDevID) = 0;
		// 关闭音频
		virtual void CloseAudio(void) = 0;

		virtual bool IsVideoOpen(void) = 0;
		virtual bool IsAudioOpen(void) = 0;

		// 强制启用编码
		virtual void SetForceEncode(bool bForce) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;

		// 设置视频属性
		// 亮度值nBrightness[0-255 128] 对比度nContrast[0-127 64] 饱和度nSaturation[0-127 64] 色调nHue[0-255 0] 伽马nGamma[1-400 100] 锐化nSharpen[0-127 0]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,int nGammaR,int nGammaG,int nGammaB)=0;

		virtual unsigned long GetAudioID(void) = 0;
		virtual unsigned long GetVideoID(void) = 0;

		// 获取视频分辨率
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		// 获取视频帧率
		virtual int GetVideoFrame(void) = 0;
		/* 获取音视频的包的统计数据
		ulTotalPackets		-总共发送了多少个包
		ulLostPackets		-总共丢弃了多少个包
		AvgPacketLossRate	-平均丢包率
		CurPacketLossRate	-当前丢包率
		nBitrate			-码流【bps】*/
		virtual int GetAudioPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;
		virtual int GetVideoPacketStatistics(unsigned long&ulTotalPackets,unsigned long&ulLostPackets,double&AvgPacketLossRate,double&CurPacketLossRate,int&nBitrate)=0;

		//设置音量
		virtual void SetVolume(int nVolume) = 0;
		//取当前音感值
		virtual int GetLevel(void) = 0;

		//保存当前图象到BMP文件
		virtual bool CaptureToBMPFile(const char* cszFile) = 0;

		// 媒体播放相关接口
		virtual unsigned long GetDuration(void) = 0;
		virtual unsigned long GetPosition(void) = 0;
		virtual void Seek(unsigned long ulSeconds) = 0;
		virtual void Pause(void) = 0;
		virtual void Play(void) = 0;
		virtual void SetMute(bool bMute = true) = 0;

		//画面颠倒
		virtual void SetInvert(int nInvert) = 0;
		virtual int GetInvert(void) = 0;

		virtual void SetChanName(const char*cszChanName) = 0;
		virtual void SetMemberName(const char*cszMemberName) = 0;
		virtual void ApplyOSD() = 0;
	};

	class IPlugin_U7_AV_CaptureMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_CaptureMgr(){};

	public:
		virtual IPlugin_U7_AV_Capture* CreateU7AVCapture(CAP_TYPE nType, int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(int nCardIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(HWND hWnd) = 0;
		virtual void DestroyU7AVCapture(int nCardIndex) = 0;
		virtual void DestroyU7AVCapture(void) = 0;

		//取设备个数
		virtual int GetDevCount(void) = 0;
		//取设备名称
		virtual int GetDevName(int nDevID,char*szName,int nMaxCount) = 0;

		//取设备类型
		virtual int GetDevType(int nDevID,int&nDevType) = 0;

		//列出格式的个数
		virtual int GetFormatCount(int nDevType) = 0;
		//取格式名称
		virtual int GetFormatName(int nDevType,int nIndex,char*szName,int nMaxCount) = 0;
		//取格式大小
		virtual int GetFormatSize(int nDevType,int nIndex,int&nWidth,int&nHeight) = 0;

		//取音频设备个数
		virtual int GetMicDevCount(void) = 0;
		//取音频设备名称
		virtual int GetMicDevName(int nDev,char desc[]) = 0;

		virtual int GetSpeakerNumDevs(void) = 0;

		//0:ok,<0:error
		virtual int GetSpeakerDevDesc(int dev,char desc[]) = 0;

		// 取音频设置
		virtual const AUDIOSET_INFO& GetAudioSet(int nCardIndex) = 0;
		// 设置音频
		virtual void SetAudioSet(int nCardIndex, const AUDIOSET_INFO& audioset) = 0;

		// 取视频设置
		virtual const VIDEOSET_INFO& GetVideoSet(int nCardIndex) = 0;
		// 设置视频
		virtual void SetVideoSet(int nCardIndex, const VIDEOSET_INFO& videoset) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
	};
}
