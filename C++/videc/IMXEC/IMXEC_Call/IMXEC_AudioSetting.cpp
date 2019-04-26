#include <IMXEC/IMXEC_AudioSetting.h>

#include <IMXEC/IMXEC_IMX.h>

#ifdef HAS_H323
#include <IMXEC/IMXEC_H323.h>
#endif

#ifdef HAS_SIP
#include <IMXEC/IMXEC_SIP.h>
#endif

IMXEC_AudioSetting::IMXEC_AudioSetting(void)
{

}

IMXEC_AudioSetting::~IMXEC_AudioSetting(void)
{

}

/******************************************************************************
* SetAudioDevParam
*����������������Ƶ�豸����
*���룺 nAudioCodecID		-��Ƶ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_AudioSetting::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
#ifdef HAS_H323
	IMXEC_H323::SetAudioDevParam(nAudioCodecID);
#endif

#ifdef HAS_SIP
	IMXEC_SIP::SetAudioDevParam(nAudioCodecID);
#endif

	IMXEC_IMX::SetAudioDevParam(nAudioCodecID);

	return 0;
}

