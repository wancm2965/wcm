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
	};

	// UCC7-����Ƶȫ������ʵ�ֲ��
	class IPlugin_U7_AV_GlobalSetting : public IPlugin
	{
	public:
		virtual ~IPlugin_U7_AV_GlobalSetting(){};

	public:
		virtual void AddObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;
		virtual void DelObserver(IPlugin_U7_AV_GlobalSetting_Observer* pObserver) = 0;

		virtual void MicTest(bool bStart) = 0;			// ��˷����
		virtual void SpeakerTest(bool bStart, const char* cszWavPath) = 0;	// ����������

		virtual int SetSpeakerDev(int nDev) = 0;	// ������Ƶ�����豸
		virtual int SetMicDev(int nDev) = 0;		// ������Ƶ�ɼ��豸

		virtual int SetFEC(int nFEC) = 0;			// ǰ�����
		virtual int GetFEC(void) = 0;

		virtual int SetPreAGC(int nAGC) = 0;		// �Զ�������˷��С��ǰ���Զ����棩
		virtual int GetPreAGC(void) = 0;

		virtual int SetVAD(int nVAD) = 0;			// �������
		virtual int GetVAD(void) = 0;

		virtual int SetAEC(int nAEC) = 0;			// ��������
		virtual int GetAEC(void) = 0;

		virtual int SetSynFlag(int nFlag) = 0;		// ����ͬ��

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
	};
}
