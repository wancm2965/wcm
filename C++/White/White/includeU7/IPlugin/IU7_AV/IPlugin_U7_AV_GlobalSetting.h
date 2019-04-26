// UCC7-音视频全局设置实现插件
// 
#pragma once
#include "../../ISystem/IPlugin.h"

namespace AVCONPlugin
{
	typedef enum _TMIC_INPUT{
		INPUT_MIC = 0,
		INPUT_LINE_IN,
		INPUT_WAVE_OUT_MIXER,
		INPUT_UNKOWN
	}TMIC_INPUT;

	class IPlugin_U7_AV_GlobalSetting_Observer
	{
	public:
		virtual ~IPlugin_U7_AV_GlobalSetting_Observer(){};

	public:
		virtual void OnSpeakerMuteChanged(int nMute) = 0;		// nMute: 0|1
		virtual void OnSpeakerVolumeChanged(int nVolume) = 0;	// nVolume:0-100

		virtual void OnMicBoostChanged(int nBoost) = 0;		//nBoost: 0|1
		virtual void OnMicInputChanged(TMIC_INPUT nType) = 0;
		virtual void OnMicVolumeChanged(int nVolume) = 0;	//nVolume:0-100
		virtual void OnMicMuteChanged(int nMute) = 0;	//nMute: 0|1
		virtual void OnPreAGCChanged(int bAuto) = 0;
	};

	// UCC7-音视频全局设置实现插件
	class IPlugin_U7_AV_GlobalSetting : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_GlobalSetting(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;

		virtual int GetSupportHardwareDecoder(void) = 0;			// 是否支持硬件解码
		virtual int GetHardwareDecoderMaxCount(void) = 0;			// 获取硬件解码路数
		virtual void SetHardwareDecoderMaxCount(int nCount) = 0;		// 设置硬件解码路数，不启用设为0

		virtual int GetSupportHardwareEncoder(void) = 0;		// 是否支持硬件编码
		virtual int GetHardwareEncoderMaxCount(void) = 0;		// 获取硬件编码路数
		virtual void SetHardwareEncoderMaxCount(int nCount) = 0;		// 设置硬件编码路数，不启用设为0

		virtual void EnableVideoDisplaySmoothly(int nEnable) = 0;	// 启用视频平滑显示
		virtual int IsEnableVideoDisplaySmoothly(void) = 0;				// 是否启用视频平滑显示

		virtual void EnableImageColorEnhance(int nEnable) = 0;		// 启用图像色彩增强
		virtual int IsEnableImageColorEnhance(void) = 0;					// 是否启用图像色彩增强

		virtual bool MicTest(bool bStart, int nAudioDev = -5, int nAudioCodec = 42) = 0;	// 麦克风测试
		virtual int GetMicTestLevel(void) = 0;	 // 获取麦克风测试的音感
		virtual bool SpeakerTest(bool bStart, const char* cszWavPath, int nSpeakerDev = -5) = 0;	// 扬声器测试
		virtual int GetSpeakerTestLevel(void) = 0;	 // 获取扬声器测试的音感

		virtual float GetMicphoneDB(void) = 0;	// 获取麦克风的分贝
		virtual float GetSpeakerDB(void) = 0;		// 获取扬声器的分贝

		virtual int SetSpeakerDev(int nDev) = 0;	// 设置音频播放设备
		virtual int GetSpeakerDev(void) = 0;			// 获取音频播放设备
		virtual int SetMicDev(int nDev) = 0;			// 设置音频采集设备
		virtual int GetMicDev(void) = 0;				// 获取音频采集设备

		virtual int SetFEC(int nFEC) = 0;			// 前向纠错
		virtual int GetFEC(void) = 0;

		// 自动调整麦克风大小（前置自动增益）
		// 0：关闭自动调整音量功能(包括麦克风和扬声器)
		// 1：打开自动麦克风音量功能
		// 2：打开自动扬声器音量功能
		virtual int SetPreAGC(int nAGC) = 0;
		virtual int GetPreAGC(void) = 0;

		virtual int SetVAD(int nVAD) = 0;			// 静音检测
		virtual int GetVAD(void) = 0;

		virtual int SetAEC(int nAEC) = 0;			// 回音消除
		virtual int GetAEC(void) = 0;

		virtual int SetSynFlag(int nFlag) = 0;		// 唇音同步
		virtual int GetSynFlag(void) = 0;

		virtual int SetPostAGC(int nAGC) = 0;		// 后置自动增益
		virtual int GetPostAGC(void) = 0;

		virtual int SetPreNS(int nNS) = 0;			// 噪音抑制
		virtual int GetPreNS(void) = 0;

		virtual int GetSpeakerLevel(void) = 0;		// 扬声器音感
		virtual int GetMicLevel(void) = 0;			// 麦克风音感

		virtual int SetSpeakerMute(int nMute) = 0;	// 扬声器静音
		virtual int GetSpeakerMute(void) = 0;

		virtual int SetSpeakerVolume(int nVolume) = 0;		// 扬声器音量
		virtual int GetSpeakerVolume(void) = 0;

		virtual int SetMicBoost(int nBoost) = 0;			// 麦克风增强
		virtual int GetMicBoost(void) = 0;

		virtual int SetMicInputType(TMIC_INPUT nType) = 0;	// 麦克风输入
		virtual TMIC_INPUT GetMicInputType(void) = 0;

		virtual int SetMicVolume(int nVolume) = 0;			// 麦克风音量
		virtual int GetMicVolume(void) = 0;

		//<0:error else mute[0:disable 1:enable]
		virtual int SetMicphoneMute(int nMute) = 0;			// 麦克风静音
		virtual int GetMicphoneMute(void) = 0;

		// 录制所有声音
		virtual int SetRecordAllAudio(int nFlagRecordAll) = 0;
	};
}
