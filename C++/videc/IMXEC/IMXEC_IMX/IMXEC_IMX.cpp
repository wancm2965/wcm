#include "stdafx.h"
#include <IMXEC/IMXEC_IMX.h>
#include "IMXCallMgr.h"

IMXEC_IMX::IMXEC_IMX(void)
{

}

IMXEC_IMX::~IMXEC_IMX(void)
{

}

/******************************************************************************
* SetUserID
*描述：设置用户标识
*输入： cszUserID		-用户标识
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_IMX::SetUserID(const char*cszUserID)
{
	return IMXCallMgr::Instance().SetUserID(cszUserID);
}

/******************************************************************************
* SetUserName
*描述：设置用户名称
*输入： cszUserName		-用户名称
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_IMX::SetUserName(const char*cszUserName)
{
	return IMXCallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetAudioDevParam
*描述：设置主流视频设备参数
*输入： nAudioCodecID		-音频编码
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_IMX::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	return IMXCallMgr::Instance().SetAudioDevParam(nAudioCodecID);
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
int IMXEC_IMX::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType)
{
	return IMXCallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType);
}

/******************************************************************************
* Start
*描述：启动SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_IMX::Start(void)
{
	return IMXCallMgr::Instance().Start();
}

/******************************************************************************
* Stop
*描述：停止SIP服务
*输入：无
*输出：无
*返回值：成功返回0，失败返回-1
*****************************************************************************/
int IMXEC_IMX::Stop(void)
{
	IMXCallMgr::Instance().Stop();

	return 0;
}
