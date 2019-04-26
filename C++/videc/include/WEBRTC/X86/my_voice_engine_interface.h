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
	//����Log�ص�
	static int SetTraceCallback (TraceCallback* cb);
	//����Log����
	static void SetTraceFilter (int filter);
	//����Log�ļ�
	static int SetTraceFile (const char* filename);

};
class API MyVoiceEngineInterface {

public:

	//����javaVM�����java������
	static void SetAndroidObjects (void* javaVM, void* context);

	//��ȡVoiceEngineʵ�� ��һ�ε���ʱ�ᴴ��VoiceEngine����֮����û᷵�ظö���
	//ע�⣺��Ҫ�ֶ� delete VoiceEngine���� �����DestroyVoiceEngine����VoiceEngine
	static MyVoiceEngineInterface* GetVoiceEngine ();
	//����VoiceEngine
	static void DestroyVoiceEngine (MyVoiceEngineInterface*);

	//��ȡ�汾��Ϣ
	static const char* Version ();

	//��ʼ��VoiceEngine
	virtual int Init () = 0;

	//������Ƶ�����ʽ
	virtual int SetSendCodec (VOICE_ENGINE_CODEC_ID codec_id) = 0;
	//��ȡ�����ʽ����ϸ��Ϣ
	virtual int GetCodecInfo (VOICE_ENGINE_CODEC_ID codec_id, CodecInst& inst) = 0;

	//ע����Ƶ�������ݻص�����
	virtual int RegisterAudioCallback (Transport* cb) = 0;
	//ȡ��ע����Ƶ�������ݻص�
	virtual int DeRegisterAudioCallback () = 0;
	//��ʼ¼��/����
	virtual int StartRecording () = 0;
	//ֹͣ¼��/����
	virtual int StopRecording () = 0;

	//����һ·��Ƶ����ͨ��
	virtual int CreateDecodeChannel () = 0;
	//����һ·��Ƶ����ͨ��
	virtual int DestroyDecodeChannel (int decodeChannelID) = 0;
	//�����ͨ��д����Ƶ����
	virtual int Decode (int channelID, const void* data, int nlen) = 0;
	//��ʼ����/����
	virtual int StartPlayout (int channelID) = 0;
	//ֹͣ����/����
	virtual int StopPlayout (int channelID) = 0;
	//����WAV�ļ�
	virtual int StartPlayLocalWAVFile (const char* filename, bool loop) = 0;
	virtual int StopPlayLocalWAVFile () = 0;

	virtual int GetNumOfMicDevices () = 0;
	virtual int GetMicDevices (int index, RecordDevice& dev) = 0;
	virtual int SetMicDevice (int index) = 0;

	virtual int GetNumOfPlayDevices () = 0;
	virtual int GetPlayDevices (int index, PlayDevice& dev) = 0;
	virtual int SetPlayDevice (int index) = 0;

	//������˷����� 0~255  ��׿ƽ̨�ݲ�֧��
	virtual int SetMicVolume (int level) = 0;
	//�������������� 0~255
	virtual int SetSpeakerVolume (int level) = 0;
	//��ȡ��˷����� 0~255  ��׿ƽ̨�ݲ�֧��
	virtual int GetMicVolume () = 0;
	//��ȡ���������� 0~255
	virtual int GetSpeakerVolume () = 0;
	//������˷羲��   //��׿ƽ̨�ݲ�֧��
	virtual int SetMicMute (bool mute) = 0;
	//��������������
	virtual int SetSpeakerMute (bool mute) = 0;
	//��ȡ��˷�����DB -96db ~ 0db
	virtual float GetMicSpeechDBValue () = 0;
	//��ȡ���������DB -96db ~ 0db
	virtual float GetSpeakerSpeechDBValue () = 0;

	//���û�������ģʽ
	virtual int SetEcStatus (bool enable, EcModes ecMode) = 0;
	//����AECMģʽ ���ڻ�������ģʽ����ΪAECMʱ��Ч
	virtual int SetAecmMode (AecmModes aecmMode, bool cng) = 0;
	//����AGCģʽ
	virtual int SetAgcStatus (bool enable, AgcModes mode) = 0;
	//����AGC����
	virtual int SetAgcConfig (AgcConfig config) = 0;
	//������������ģʽ
	virtual int SetNsStatus (bool enable, NsModes mode) = 0;
	//�����������Ʋ�������ʱ���ã���Ҫ���ô˽ӿ�
	virtual int SetNsConfig (float overdive, float denoiseband, int gainmap) = 0;
	//���þ�������Ƿ���
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
