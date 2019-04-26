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
	};

	// UCC7-音视频全局设置实现插件
	class IPlugin_U7_AV_GlobalSetting : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_GlobalSetting(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;

		virtual void MicTest(bool bStart) = 0;			// 麦克风测试
		virtual void SpeakerTest(bool bStart, const char* cszWavPath) = 0;	// 扬声器测试

		virtual int SetSpeakerDev(int nDev) = 0;	// 设置音频播放设备
		virtual int SetMicDev(int nDev) = 0;		// 设置音频采集设备

		virtual int SetFEC(int nFEC) = 0;			// 前向纠错
		virtual int GetFEC(void) = 0;

		virtual int SetPreAGC(int nAGC) = 0;		// 自动调整麦克风大小（前置自动增益）
		virtual int GetPreAGC(void) = 0;

		virtual int SetVAD(int nVAD) = 0;			// 静音检测
		virtual int GetVAD(void) = 0;

		virtual int SetAEC(int nAEC) = 0;			// 回音消除
		virtual int GetAEC(void) = 0;

		virtual int SetSynFlag(int nFlag) = 0;		// 唇音同步

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
	};
}
