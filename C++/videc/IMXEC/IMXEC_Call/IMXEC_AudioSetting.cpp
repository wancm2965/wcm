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
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
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

