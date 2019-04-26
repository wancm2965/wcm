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
*描述：设置主流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
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
*描述：设置辅流视频设备参数
*输入： nDevID				-设备标识
		nFrameRate			-帧率
		nBitrate			-码流
		nVideoCodecType		-视频编码
		nVideoFormatType	-视频格式
*输出：无
*返回值：成功返回0，失败返回-1
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
