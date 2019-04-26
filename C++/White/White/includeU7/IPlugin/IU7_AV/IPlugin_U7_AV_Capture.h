// UCC7-音视频采集功能实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"
#include "U7_AVDefine.h"

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

		virtual void OnCapChanRecordStart(void){};

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

		// 视频设置
		virtual void SetVideoSet(const VIDEOSET_INFO& videosetInfo) = 0;
		virtual void GetVideoSet(VIDEOSET_INFO& videosetInfo) = 0;

		// 打开视频设备
		// 仅应用视频参数，不保存
		virtual int OpenVideo(int nVideoSizeIndex,int nWidth,int nHeight,int nFrameRate,int nBitrate,int nBitrateControlType,VIDEC_CODEC videoCodec,int nSub,int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		// 从配置中读取视频参数来打开
		virtual int OpenVideo(int nSub, int nQSub) = 0;
		virtual int OpenVideo(int x, int y, int cx, int cy, int nSub, int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0, HDC hDC = NULL) = 0;
		virtual int OpenVideo(const char* cszMediaFile, int nLoop, int nSub, int nQSub, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		virtual int OpenVideo(const char* cszIP, int nPort, const char* cszPassword, int nSubWidth = 0, int nSubHeight = 0, int nSubBitrate = 0, int nQSubWidth = 0, int nQSubHeight = 0, int nQSubBitrate = 0) = 0;
		// 关闭视频
		virtual void CloseVideo(void) = 0;

		// 音频设置
		virtual void SetAudioSet(int nDevID, int nAudioCodec) = 0;
		virtual void GetAudioSet(int& nDevID, int& nAudioCodec) = 0;

		// 打开音频
		virtual int OpenAudio(void) = 0;
		// 关闭音频
		virtual void CloseAudio(void) = 0;

		virtual bool IsVideoOpen(void) = 0;
		virtual bool IsAudioOpen(void) = 0;

		// 强制启用编码
		virtual void SetForceEncode(bool bForce) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// 获取视频显示模式 0：填充 1：全景
		virtual int GetVideoShowMode(void) = 0;

		// 设置视频属性
		// 亮度值nBrightness[0-255 128] 对比度nContrast[0-127 64] 饱和度nSaturation[0-127 64] 色调nHue[0-255 0] 伽马nGamma[1-400 100] 锐化nSharpen[0-127 0]
		// 锐化仅应用到本地bSharpenLocalOnly[默认值为true]
		// nGammaR[1-400 100] nGammaG[1-400 100] nGammaB[1-400 100]
		// 按幅度调整bAjustAmp[默认值为false] 为true时会在原有值的基础上加上传入的参数值
		virtual void SetVideoParam(int nBrightness, int nContrast, int nSaturation, int nHue,int nGamma,int nSharpen,bool bSharpenLocalOnly,
			int nGammaR,int nGammaG,int nGammaB,bool bAjustAmp) = 0;
		virtual void GetVideoParam(int& nBrightness, int& nContrast, int& nSaturation, int& nHue, int& nGamma,int& nSharpen, bool& bSharpenLocalOnly,
			int& nGammaR, int& nGammaG, int& nGammaB) = 0;

		virtual bool HasVideoAdjustWnd(void)=0;
		virtual void ShowAdjustVideoWnd(void)=0;

		virtual unsigned long GetAudioID(void) = 0;
		virtual unsigned long GetVideoID(void) = 0;
		virtual CAP_TYPE GetCapType(void) = 0;

		// 获取视频分辨率
		virtual bool GetVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;

		virtual bool GetAutoVideoSize(int& nVideoWidth, int& nVideoHeight) = 0;
		// 获取视频帧率
		virtual int GetVideoFrame(void) = 0;
		//获取视频码流值
		virtual int GetVideoBitrate(void) = 0;

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

		// 请求关键帧
		virtual void RequestKeyFrame() = 0;

		// 支持格式：avms、avi、mkv、mp4
		virtual void StartRecord(const char* cszPathName) = 0;
		virtual void StopRecord(void) = 0;

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

		// 网络摄像机云台控制
		virtual int DVCSendData(const char*pData,int nLen) = 0;

		// 设置是否显示OSD
		virtual void EnableOSD(bool bEnable) = 0;
		// 获取是否显示OSD
		virtual bool IsOSDEnable(void) = 0;

		// 设置OSD的文本
		// Align==0 靠左上角 1:靠左下角,2:靠上中间,3:靠下中间,4:靠右上角,5:靠右下角
		virtual void SetOSDText(const char*cszText,int nAlign = 0) = 0;
		// 获取OSD的文本
		virtual void GetOSDText(std::string& strText, int& nAlign) = 0;

		// 设置OSD的字体
		virtual void SetOSDFont(const LOGFONTA& logfont,COLORREF crText=RGB(0,0,255),COLORREF crBk=RGB(255,255,255)) = 0;
		// 获取OSD的字体
		virtual void GetOSDFont(LOGFONTA& logfont,COLORREF& crText,COLORREF& crBk) = 0;
		virtual int OpenCompoundVideo(int nScreenIndex, int nScreenType, int nWidth, int nHeight, int nFrameRate, int nBitrate, int nBitrateControlType, VIDEC_CODEC_TYPE CodecType, int nSub, int nQSub) = 0;
		
		virtual void SetScreenWnd(int nScreenIndex,int nWndIndex,int nFullScreen) = 0;
		//设置是否允许打开音视频
		virtual void SetAVOpenbAllow(bool bAllowOpenAV) = 0;

		//锁定分辨率及码流
		//参数设置为0表示不进行更改
		//nBitreate可设置为256、512等
		virtual void Lock(int nWidth, int nHeight, int nBitreate) = 0;
		virtual void UnLock(bool bResolution, bool bBitreate) = 0;
	};

	class IPlugin_U7_AV_CaptureMgr : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_CaptureMgr(){};

	public:
		virtual IPlugin_U7_AV_Capture* CreateU7AVCapture(CAP_TYPE nType, int nDevID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(int nDevID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCaptureByChnID(int nChannelID) = 0;
		virtual IPlugin_U7_AV_Capture* GetU7AVCapture(HWND hWnd) = 0;
		virtual void DestroyU7AVCapture(int nDevID) = 0;
		virtual void DestroyU7AVCapture(void) = 0;

		// 创建某通道的拷贝，耗CPU，慎用
		virtual IPlugin_U7_AV_Capture* CreateDevCopy(CAP_TYPE nType, int nDevID, unsigned long ulCopyIndex) = 0;
		virtual IPlugin_U7_AV_Capture* GetDevCopy(unsigned long ulCopyIndex) = 0;
		virtual void DestroyCardCopy(unsigned long ulCopyIndex) = 0;

		// 启用发送视频
		virtual void EnableVideoSend(bool bEnable) = 0;
		virtual bool IsEnableVideoSend(void) = 0;

		// 获取视频通道个数
		virtual int GetChannelCount(void) = 0;
		// 获取视频通道对应的设备
		virtual int GetChannelDev(int nChannelID) = 0;

		// 取设备个数
		virtual int GetDevCount(void) = 0;
		// 取设备名称
		virtual int GetDevName(int nDevID,char*szName,int nMaxCount) = 0;

		// 取设备类型
		virtual int GetDevType(int nDevID,int&nDevType) = 0;

		// 列出格式的个数
		virtual int GetFormatCount(int nDevType) = 0;
		// 取格式名称
		virtual int GetFormatName(int nDevType,int nIndex,char*szName,int nMaxCount) = 0;
		// 取格式大小
		virtual int GetFormatSize(int nDevType,int nIndex,int&nWidth,int&nHeight) = 0;

		// 取音频设备个数
		virtual int GetMicDevCount(void) = 0;
		// 取音频设备名称
		virtual int GetMicDevName(int nDev,char desc[]) = 0;

		virtual int GetSpeakerNumDevs(void) = 0;

		// 0:ok,<0:error
		virtual int GetSpeakerDevDesc(int dev,char desc[]) = 0;

		// 取音频设置
		virtual AUDIOSET_INFO GetAudioSet(int nCardIndex) = 0;
		// 设置音频
		virtual void SetAudioSet(int nCardIndex, const AUDIOSET_INFO& audioset) = 0;

		// 取视频设置
		virtual VIDEOSET_INFO GetVideoSet(int nCardIndex) = 0;
		// 设置视频
		virtual void SetVideoSet(int nCardIndex, const VIDEOSET_INFO& videoset) = 0;

		// 编码视频设置字符串
		virtual void EncodeVideoSet(const VIDEOSET_INFO& videoset, std::string& strCodec, std::string& strSize
			, std::string& strFrameRate, std::string& strBitrate, std::string& strNetType) = 0;
		// 解码应用视频设置
		// nSize为分辨率索引，nSzWidth、nSzHeight分别为真实分辨率宽、高
		// nStream为码流索引，nBitrate为视频真实码流值。
		virtual void DecodeSetVideoSet(int nDevID, int nCodec, int nSize, int nSzWidth, int nSzHeight
			, int nFrameRate, int nStream, int nBitrate, int nNetType, bool bDenoise) = 0;

		// 设置视频显示模式 0：填充 1：全景
		virtual void SetVideoShowMode(int nVideoShowMode) = 0;
		// 获取视频显示模式 0：填充 1：全景
		virtual int GetVideoShowMode(void) = 0;

		// 获取当前会议上传总带宽
		virtual unsigned long GetUploadTotalBitrate(void) = 0;

		virtual int SetScreenPos(int nScreenIndex,int x,int y,int cx,int cy) = 0;

		virtual int SetScreenWndCount(int nScreenIndex,int nWndCount) = 0;

		virtual int SetScreenWndPos(int nScreenIndex,int nWndIndex,int x,int y,int cx,int cy) = 0;
	};
}
