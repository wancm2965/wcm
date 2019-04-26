#include <IMXEC/IMXEC_VideoSetting.h>

#include <IMXEC/IMXEC_IMX.h>

#ifdef HAS_H323
#include <IMXEC/IMXEC_H323.h>
#endif

#ifdef HAS_SIP
#include <IMXEC/IMXEC_SIP.h>
#endif

IMXEC_VideoSetting::IMXEC_VideoSetting(void)
{
}

IMXEC_VideoSetting::~IMXEC_VideoSetting(void)
{
}

/******************************************************************************
* SetMainVideoDevParam
*����������������Ƶ�豸����
*���룺 nDevID				-�豸��ʶ
		nFrameRate			-֡��
		nBitrate			-����
		nVideoCodecType		-��Ƶ����
		nVideoFormatType	-��Ƶ��ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_VideoSetting::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_H323
	IMXEC_H323::SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#endif

#ifdef HAS_SIP
	IMXEC_SIP::SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#endif

	IMXEC_IMX::SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType);
	return 0;
}

/******************************************************************************
* SetSecondVideoDevParam
*���������ø�����Ƶ�豸����
*���룺 nDevID				-�豸��ʶ
		nFrameRate			-֡��
		nBitrate			-����
		nVideoCodecType		-��Ƶ����
		nVideoFormatType	-��Ƶ��ʽ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_VideoSetting::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_H323
	IMXEC_H323::SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#endif

//20120605
#ifdef HAS_SIP
	IMXEC_SIP::SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#endif

	return 0;
}
