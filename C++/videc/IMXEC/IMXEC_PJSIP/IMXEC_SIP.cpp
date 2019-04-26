#include "stdafx.h"

#include "../DefineHeader/SIPDefineHeader.h"

#if !defined(HAS_SIP_AVCON_MP) || HAS_SIP_AVCON_MP == 0

#include <IMXEC/IMXEC_SIP.h>

#elif defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>

#endif

#include "CallMgr.h"

#ifdef HAS_SIP
#include "SIPCallMgr.h"
#endif

IMXEC_SIP::IMXEC_SIP(void)
{

}

IMXEC_SIP::~IMXEC_SIP(void)
{

}

/******************************************************************************
* AddVirtualLine
*���������������·
*���룺	cszUserID		-�û��ʺ�
		cszUserName		-�û�����
		cszPassword		-�û�����
		cszHost			-SIP���������
*�������
*����ֵ���ɹ�����>=0[line id]��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::AddVirtualLine(const char*cszUserID,const char*cszUserName,const char*cszPassword,const char*cszSIPProxy,const char*cszAuthUserID,const char*cszDomain)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().AddVirtualLine(cszUserID,cszUserName,cszPassword,cszSIPProxy,cszAuthUserID,cszDomain);
#else
	return -1;
#endif
}

/******************************************************************************
* RemoveVirtualLine
*������ɾ��������·
*���룺	nLineID			-��·��ʶ
*�������
*����ֵ����
*****************************************************************************/
void IMXEC_SIP::RemoveVirtualLine(int nLineID)
{
#ifdef HAS_SIP

	SIPCallMgr::Instance().HungupAll(nLineID);
	SIPCallMgr::Instance().RemoveVirtualLine(nLineID);

#else
	return ;
#endif
}

int IMXEC_SIP::SetCurrentVirtualLine(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetCurrentVirtualLine(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::GetCurrentVirtualLine(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetCurrentVirtualLine();
#else
	return -1;
#endif
}

int IMXEC_SIP::IsVirtualLineRegistrationSuccess(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsVirtualLineRegistrationSuccess(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::IsCurrentVirtualLineRegistrationSuccess(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsCurrentVirtualLineRegistrationSuccess();
#else
	return -1;
#endif
}

int IMXEC_SIP::IsVirtualLineRegistrationError(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsVirtualLineRegistrationError(nLineID);
#else
	return -1;
#endif
}

int IMXEC_SIP::IsCurrentVirtualLineRegistrationError(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().IsCurrentVirtualLineRegistrationError();
#else
	return -1;
#endif
}

const char*IMXEC_SIP::GetVirtualLineRegistrationErrorCode(int nLineID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetVirtualLineRegistrationErrorCode(nLineID);
#else
	return "";
#endif
}

const char*IMXEC_SIP::GetCurrentVirtualLineRegistrationErrorCode(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().GetCurrentVirtualLineRegistrationErrorCode();
#else
	return "";
#endif
}

/******************************************************************************
* SetSIPListenPort
*���������������˿�
*���룺	nSIPListenPort			-�����˿�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetSIPListenPort(unsigned short nSIPListenPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPListenPort(nSIPListenPort);
#else
	return -1;
#endif
}

/******************************************************************************
* SetLocalIP
*����������ע���ʺ�
*���룺	cszSIPID		-ע���ʺ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetLocalIP(const char*cszLocalIP)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetLocalIP(cszLocalIP);
#else
	return -1;
#endif
}

/******************************************************************************
* SetRTPBasePort
*���������������˿�
*���룺	nRTPBasePort			-RTP��ʼ�˿�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetRTPBasePort(unsigned short nRTPBasePort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetRTPBasePort(nRTPBasePort);
#else
	return -1;
#endif
}


/******************************************************************************
* SetAudioDevParam
*����������������Ƶ�豸����
*���룺 nAudioCodecID		-��Ƶ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetAudioDevParam(nAudioCodecID);
#else
	return -1;
#endif
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
int IMXEC_SIP::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#else
	return -1;
#endif
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
int IMXEC_SIP::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
#else
	return -1;
#endif
}

/******************************************************************************
* SetSupportH239
*�����������Ƿ�֧��H239
*���룺nSupportH239		-�Ƿ�֧��H239
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetSupportH239(int nSupportH239)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSupportH239(nSupportH239);
#else
	return -1;
#endif
}



/******************************************************************************
* Start
*����������SIP����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::Start(void)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().Start();
#else
	return -1;
#endif
}

/******************************************************************************
* Stop
*������ֹͣSIP����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::Stop(void)
{
#ifdef HAS_SIP
	SIPCallMgr::Instance().Stop();

	return 0;
#else
	return -1;
#endif
}

/******************************************************************************
* SetSIPTransportType
*����������SIP���������
*���룺type			-����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetSIPTransportType(IMXEC_SIP::TRANS_TYPE type)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPTransportType(type);
#else
	return -1;
#endif
}


/******************************************************************************
* SetICETransportCount
*���������ô�����ÿ��ý����ICEͨ����
*���룺	nCount		-ÿ��ý����ICEͨ����(����ֻ��RTP����Ϊ1���绹��Ҫ��RTCP����Ϊ2)
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetICETransportCount(int nCount)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetICETransportCount(nCount);
#else
	return -1;
#endif
}
/******************************************************************************
* SetICEEnable
*����������ICEӦ��״̬
*���룺	bEnable		-ICEӦ��״̬ trueʹ�ã�false��ʹ�ã�Ĭ��false
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetICEEnable(bool bEnable)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetICEEnable(bEnable);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTURNLoginAuth
*���������õ�¼TURN�������˻�
*���룺	cszTurnUser		-TURN�������˻���
cszTurnPWD		-TURN�������˻�����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetTURNLoginAuth(const char*cszTurnUser,const char*cszTurnPWD)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTURNLoginAuth(cszTurnUser,cszTurnPWD);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTURNServerIPPort
*����������TURN��������ַ,Ҳ����Ϊ������ַ
*���룺	cszTurnIPPort		-TURN��������ַ ����IP:Port
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetTURNServerIPPort(const char*cszTurnIPPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTURNServerIPPort(cszTurnIPPort);
#else
	return -1;
#endif
}
/******************************************************************************
* SetSTUNServerIPPort
*����������STUN��������ַ,Ҳ����Ϊ������ַ
*���룺	cszStunIPPort		-STUN��������ַ ����IP:Port
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetSTUNServerIPPort(const char*cszStunIPPort)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSTUNServerIPPort(cszStunIPPort);
#else
	return -1;
#endif
}
/******************************************************************************
* SetTurnTransportType
*����������TURN����ͨ������
*���룺	nType		-TURN����ͨ������ 17 udp, 6 tcp, 255 tls, ����ɲο�pj_turn_tp_type
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetTurnTransportType(int nType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetTurnTransportType(nType);
#else
	return -1;
#endif
}
/******************************************************************************
* SetDNSServerIP
*����������DNS��������ַ
*���룺	cszDNSIP		-����DNS��������ַ ����IP
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetDNSServerIP(const char*cszDNSIP)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDNSServerIP(cszDNSIP);
#else
	return -1;
#endif
}
/******************************************************************************
* SetDNSServerIP2
*����������DNS��������ַ2
*���룺	cszDNSIP2		-����DNS��������ַ ����IP
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetDNSServerIP2(const char*cszDNSIP2)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDNSServerIP2(cszDNSIP2);
#else
	return -1;
#endif
}
