// UCC7-����Ƶȫ������ʵ�ֲ��
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

	// UCC7-����Ƶȫ������ʵ�ֲ��
	class IPlugin_U7_AV_GlobalSetting : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_GlobalSetting(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;

		virtual int GetSupportHardwareDecoder(void) = 0;			// �Ƿ�֧��Ӳ������
		virtual int GetHardwareDecoderMaxCount(void) = 0;			// ��ȡӲ������·��
		virtual void SetHardwareDecoderMaxCount(int nCount) = 0;		// ����Ӳ������·������������Ϊ0

		virtual int GetSupportHardwareEncoder(void) = 0;		// �Ƿ�֧��Ӳ������
		virtual int GetHardwareEncoderMaxCount(void) = 0;		// ��ȡӲ������·��
		virtual void SetHardwareEncoderMaxCount(int nCount) = 0;		// ����Ӳ������·������������Ϊ0

		virtual void EnableVideoDisplaySmoothly(int nEnable) = 0;	// ������Ƶƽ����ʾ
		virtual int IsEnableVideoDisplaySmoothly(void) = 0;				// �Ƿ�������Ƶƽ����ʾ

		virtual void EnableImageColorEnhance(int nEnable) = 0;		// ����ͼ��ɫ����ǿ
		virtual int IsEnableImageColorEnhance(void) = 0;					// �Ƿ�����ͼ��ɫ����ǿ

		virtual bool MicTest(bool bStart, int nAudioDev = -5, int nAudioCodec = 42) = 0;	// ��˷����
		virtual int GetMicTestLevel(void) = 0;	 // ��ȡ��˷���Ե�����
		virtual bool SpeakerTest(bool bStart, const char* cszWavPath, int nSpeakerDev = -5) = 0;	// ����������
		virtual int GetSpeakerTestLevel(void) = 0;	 // ��ȡ���������Ե�����

		virtual float GetMicphoneDB(void) = 0;	// ��ȡ��˷�ķֱ�
		virtual float GetSpeakerDB(void) = 0;		// ��ȡ�������ķֱ�

		virtual int SetSpeakerDev(int nDev) = 0;	// ������Ƶ�����豸
		virtual int GetSpeakerDev(void) = 0;			// ��ȡ��Ƶ�����豸
		virtual int SetMicDev(int nDev) = 0;			// ������Ƶ�ɼ��豸
		virtual int GetMicDev(void) = 0;				// ��ȡ��Ƶ�ɼ��豸

		virtual int SetFEC(int nFEC) = 0;			// ǰ�����
		virtual int GetFEC(void) = 0;

		// �Զ�������˷��С��ǰ���Զ����棩
		// 0���ر��Զ�������������(������˷��������)
		// 1�����Զ���˷���������
		// 2�����Զ���������������
		virtual int SetPreAGC(int nAGC) = 0;
		virtual int GetPreAGC(void) = 0;

		virtual int SetVAD(int nVAD) = 0;			// �������
		virtual int GetVAD(void) = 0;

		virtual int SetAEC(int nAEC) = 0;			// ��������
		virtual int GetAEC(void) = 0;

		virtual int SetSynFlag(int nFlag) = 0;		// ����ͬ��
		virtual int GetSynFlag(void) = 0;

		virtual int SetPostAGC(int nAGC) = 0;		// �����Զ�����
		virtual int GetPostAGC(void) = 0;

		virtual int SetPreNS(int nNS) = 0;			// ��������
		virtual int GetPreNS(void) = 0;

		virtual int GetSpeakerLevel(void) = 0;		// ����������
		virtual int GetMicLevel(void) = 0;			// ��˷�����

		virtual int SetSpeakerMute(int nMute) = 0;	// ����������
		virtual int GetSpeakerMute(void) = 0;

		virtual int SetSpeakerVolume(int nVolume) = 0;		// ����������
		virtual int GetSpeakerVolume(void) = 0;

		virtual int SetMicBoost(int nBoost) = 0;			// ��˷���ǿ
		virtual int GetMicBoost(void) = 0;

		virtual int SetMicInputType(TMIC_INPUT nType) = 0;	// ��˷�����
		virtual TMIC_INPUT GetMicInputType(void) = 0;

		virtual int SetMicVolume(int nVolume) = 0;			// ��˷�����
		virtual int GetMicVolume(void) = 0;

		//<0:error else mute[0:disable 1:enable]
		virtual int SetMicphoneMute(int nMute) = 0;			// ��˷羲��
		virtual int GetMicphoneMute(void) = 0;

		// ¼����������
		virtual int SetRecordAllAudio(int nFlagRecordAll) = 0;
	};
}
