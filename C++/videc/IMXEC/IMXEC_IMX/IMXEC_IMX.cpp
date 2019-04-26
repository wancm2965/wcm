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
*�����������û���ʶ
*���룺 cszUserID		-�û���ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_IMX::SetUserID(const char*cszUserID)
{
	return IMXCallMgr::Instance().SetUserID(cszUserID);
}

/******************************************************************************
* SetUserName
*�����������û�����
*���룺 cszUserName		-�û�����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_IMX::SetUserName(const char*cszUserName)
{
	return IMXCallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetAudioDevParam
*����������������Ƶ�豸����
*���룺 nAudioCodecID		-��Ƶ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_IMX::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	return IMXCallMgr::Instance().SetAudioDevParam(nAudioCodecID);
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
int IMXEC_IMX::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType)
{
	return IMXCallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType);
}

/******************************************************************************
* Start
*����������SIP����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_IMX::Start(void)
{
	return IMXCallMgr::Instance().Start();
}

/******************************************************************************
* Stop
*������ֹͣSIP����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_IMX::Stop(void)
{
	IMXCallMgr::Instance().Stop();

	return 0;
}
