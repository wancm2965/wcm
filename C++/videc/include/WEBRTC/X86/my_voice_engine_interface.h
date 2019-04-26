#ifndef MY_VOICE_ENGINE_INTERFACE_H
#define MY_VOICE_ENGINE_INTERFACE_H

#ifdef _WIN32
#ifdef VOICE_ENGINE_EXPORT
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif // VOICE_ENGINE_EXPORT
#else //_WIN32
#define API __attribute__ ((visibility("default")))
#endif

#include "my_voice_engine_types.h"

namespace voice_engine {

class API VoiceEngineTrace {

public:
	//设置Log回调
	static int SetTraceCallback (TraceCallback* cb);
	//设置Log过滤
	static void SetTraceFilter (int filter);
	//设置Log文件
	static int SetTraceFile (const char* filename);

};
class API MyVoiceEngineInterface {

public:

	//设置javaVM对象和java上下文
	static void SetAndroidObjects (void* javaVM, void* context);

	//获取VoiceEngine实例 第一次调用时会创建VoiceEngine对象，之后调用会返回该对象
	//注意：不要手动 delete VoiceEngine对象 请调用DestroyVoiceEngine销毁VoiceEngine
	static MyVoiceEngineInterface* GetVoiceEngine ();
	//销毁VoiceEngine
	static void DestroyVoiceEngine (MyVoiceEngineInterface*);

	//获取版本信息
	static const char* Version ();

	//初始化VoiceEngine
	virtual int Init () = 0;

	//设置音频编码格式
	virtual int SetSendCodec (VOICE_ENGINE_CODEC_ID codec_id) = 0;
	//获取编码格式的详细信息
	virtual int GetCodecInfo (VOICE_ENGINE_CODEC_ID codec_id, CodecInst& inst) = 0;

	//注册音频编码数据回调函数
	virtual int RegisterAudioCallback (Transport* cb) = 0;
	//取消注册音频编码数据回调
	virtual int DeRegisterAudioCallback () = 0;
	//开始录制/编码
	virtual int StartRecording () = 0;
	//停止录制/编码
	virtual int StopRecording () = 0;

	//创建一路音频解码通道
	virtual int CreateDecodeChannel () = 0;
	//销毁一路音频解码通道
	virtual int DestroyDecodeChannel (int decodeChannelID) = 0;
	//向解码通道写入音频数据
	virtual int Decode (int channelID, const void* data, int nlen) = 0;
	//开始播放/解码
	virtual int StartPlayout (int channelID) = 0;
	//停止播放/解码
	virtual int StopPlayout (int channelID) = 0;
	//播放WAV文件
	virtual int StartPlayLocalWAVFile (const char* filename, bool loop) = 0;
	virtual int StopPlayLocalWAVFile () = 0;

	virtual int GetNumOfMicDevices () = 0;
	virtual int GetMicDevices (int index, RecordDevice& dev) = 0;
	virtual int SetMicDevice (int index) = 0;

	virtual int GetNumOfPlayDevices () = 0;
	virtual int GetPlayDevices (int index, PlayDevice& dev) = 0;
	virtual int SetPlayDevice (int index) = 0;

	//设置麦克风音量 0~255  安卓平台暂不支持
	virtual int SetMicVolume (int level) = 0;
	//设置扬声器音量 0~255
	virtual int SetSpeakerVolume (int level) = 0;
	//获取麦克风音量 0~255  安卓平台暂不支持
	virtual int GetMicVolume () = 0;
	//获取扬声器音量 0~255
	virtual int GetSpeakerVolume () = 0;
	//设置麦克风静音   //安卓平台暂不支持
	virtual int SetMicMute (bool mute) = 0;
	//设置扬声器静音
	virtual int SetSpeakerMute (bool mute) = 0;
	//获取麦克风输入DB -96db ~ 0db
	virtual float GetMicSpeechDBValue () = 0;
	//获取扬声器输出DB -96db ~ 0db
	virtual float GetSpeakerSpeechDBValue () = 0;

	//设置回音消除模式
	virtual int SetEcStatus (bool enable, EcModes ecMode) = 0;
	//设置AECM模式 仅在回音消除模式设置为AECM时有效
	virtual int SetAecmMode (AecmModes aecmMode, bool cng) = 0;
	//设置AGC模式
	virtual int SetAgcStatus (bool enable, AgcModes mode) = 0;
	//设置AGC参数
	virtual int SetAgcConfig (AgcConfig config) = 0;
	//设置噪音抑制模式
	virtual int SetNsStatus (bool enable, NsModes mode) = 0;
	//设置噪音抑制参数，暂时无用，不要调用此接口
	virtual int SetNsConfig (float overdive, float denoiseband, int gainmap) = 0;
	//设置静音检测是否开启
	virtual int SetVAD (bool enable) = 0;

	virtual int GetEcStatus (bool& enable, EcModes& ecMode) = 0;
	virtual int GetAgcStatus (bool& enable, AgcModes& mode) = 0;
	virtual int GetNsStatus (bool& enable, NsModes& mode) = 0;
	virtual int GetVAD (bool& enable) = 0;

	virtual int EnableStatistics (bool enable) = 0;
	virtual int GetStatistics (int channel) = 0;

protected:
	virtual ~MyVoiceEngineInterface () {}

};

}

#endif // MY_VOICE_ENGINE_INTERFACE_H
