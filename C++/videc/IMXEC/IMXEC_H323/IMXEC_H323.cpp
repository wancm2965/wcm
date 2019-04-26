#include "stdafx.h"
#include "../DefineHeader/H323DefineHeader.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#else
#include <IMXEC/IMXEC_H323.h>
#endif

#include "H323CallMgr.h"
#ifdef HAS_SIP
#include "../IMXEC_PJSIP/SIPCallMgr.h"
#endif

IMXEC_H323::IMXEC_H323(void)
{

}

IMXEC_H323::~IMXEC_H323(void)
{

}

/******************************************************************************
* SetH323ID
*����������ע���ʺ�
*���룺	cszH323ID		-ע���ʺ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetH323ID(const char*cszH323ID)
{
	return H323CallMgr::Instance().SetH323ID(cszH323ID);
}

/******************************************************************************
* SetTelNumber
*���������õ绰����
*���룺	cszTelNumber		-�绰����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetTelNumber(const char*cszTelNumber)
{
	return H323CallMgr::Instance().SetTelNumber(cszTelNumber);
}

/******************************************************************************
* SetRegPassword
*����������ע������
*���룺	cszRegPassword	-ע���ʺ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetRegPassword(const char*cszRegPassword)
{
	return H323CallMgr::Instance().SetRegPassword(cszRegPassword);
}

/******************************************************************************
* SetUserName
*�����������û�����
*���룺	cszUserName		-�û�����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetUserName(const char*cszUserName)
{
	return H323CallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetRegHost
*�����������û�����
*���룺	cszHost			-ע�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetRegHost(const char*cszHost)
{
	return H323CallMgr::Instance().SetRegHost(cszHost);
}

/******************************************************************************
* Register
*������ע��
*���룺nType			-ע�����͡�0:�Զ�����GK����������0:ע�ᵽָ����GK��������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::Register(int nType)
{
	return H323CallMgr::Instance().Register(nType);
}

/******************************************************************************
* UnRegister
*������ע��
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::UnRegister(void)
{
	return H323CallMgr::Instance().UnRegister();
}

/******************************************************************************
* GetRegState
*��������ȡע��״̬
*���룺��
*�����	nRegState			-ע��״̬
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::GetRegState(REG_STATE&nRegState)
{
	return H323CallMgr::Instance().GetRegState(nRegState);
}

/******************************************************************************
* SetH323ListenPort
*���������������˿�
*���룺	nH323ListenPort			-�����˿�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetH323ListenPort(unsigned short nH323ListenPort)
{
	return H323CallMgr::Instance().SetH323ListenPort(nH323ListenPort);
}

/******************************************************************************
* SetLocalIP
*����������ע���ʺ�
*���룺	cszH323ID		-ע���ʺ�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalIP(const char*cszLocalIP)
{
	return H323CallMgr::Instance().SetLocalIP(cszLocalIP);
}

/******************************************************************************
* SetRTPBasePort
*���������������˿�
*���룺	nRTPBasePort			-RTP��ʼ�˿�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetRTPBasePort(unsigned short nRTPBasePort)
{
	//H323CallMgr::Instance().SetMediaPortArrayEmpty();
	//SIPCallMgr::Instance().SetMediaPortArrayEmpty();
	//CallMgr::Instance().SetRTPBasePort(nRTPBasePort); //ע�������ͬʱ��˿ڻ��ճ�
	return H323CallMgr::Instance().SetRTPBasePort(nRTPBasePort);
}


/******************************************************************************
* SetAudioDevParam
*����������������Ƶ�豸����
*���룺 nAudioCodecID		-��Ƶ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetAudioDevParam(X_AUDIO_CODEC_ID nAudioCodecID)
{
	return H323CallMgr::Instance().SetAudioDevParam(nAudioCodecID);
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
int IMXEC_H323::SetMainVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	return H323CallMgr::Instance().SetMainVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
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
int IMXEC_H323::SetSecondVideoDevParam(int nDevID,int nFrameRate,int nBitrate,VIDEC_CODEC_TYPE nVideoCodecType,VIDEO_FORMAT_TYPE nVideoFormatType,int nNetworkType)
{
	return H323CallMgr::Instance().SetSecondVideoDevParam(nDevID,nFrameRate,nBitrate,nVideoCodecType,nVideoFormatType,nNetworkType);
}


/******************************************************************************
* Start
*����������H323����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::Start(void)
{
	return H323CallMgr::Instance().Start();
}

/******************************************************************************
* Stop
*������ֹͣH323����
*���룺��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::Stop(void)
{
	H323CallMgr::Instance().Stop();

	return 0;
}

/******************************************************************************
* SetSupportH239
*�����������Ƿ�֧��H239
*���룺nSupportH239		-�Ƿ�֧��H239
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetSupportH239(int nSupportH239)
{
	return H323CallMgr::Instance().SetSupportH239(nSupportH239);
}




//use H235 Media

/******************************************************************************
* H235SecuritySettingInitialize
*������	H235��ȫѡ���ʼ��
*���룺	
*****************************************************************************/
void IMXEC_H323::H235SecuritySettingInitialize()
{
	H323CallMgr::Instance().H235SecuritySettingInitialize();
}
/******************************************************************************
* SetH235SecurityAuthOption
*������	����ʹ��H235��֤ѡ��
*���룺	option 0Ϊ��ʹ�ã�1Ϊʹ��
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAuthOption(int option)
{
	H323CallMgr::Instance().SetH235SecurityAuthOption(option);
}

/******************************************************************************
* SetH235MediaSecurityOption
*������	����ʹ��ý��������ѡ��
*���룺	option 0Ϊ��ʹ�ã�1Ϊ��ѡʹ�ã�2Ϊǿ��ʹ��H235ý��������
*****************************************************************************/
void IMXEC_H323::SetH235MediaSecurityOption(int option)
{
	H323CallMgr::Instance().SetH235MediaSecurityOption(option);
}


/******************************************************************************
* SetH235SecurityAuth
*������	����H235��֤��Ϣ ������senderId�����Է�������generalID ������generalID�����Է�������senderId
*���룺	senderId ���ͱ�ʶ�����˱�ʶ 
generalId���ձ�ʶ���Զ˱�ʶ 
password ��֤����
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAuth(char * senderId,char * generalId,char * password)
{
	H323CallMgr::Instance().SetH235SecurityAuth(senderId,generalId,password);
}


/******************************************************************************
* SetH235SecurityAnnexIEnable
*������	���ð�ȫ��֤ģʽ����״̬��H235 AnnexD I�����Ϣ�����Լ�������֤
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIAEnable
*������	���ð�ȫ��֤ģʽ����״̬��H235 AnnexD IA ��̽�������֤
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIAEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIAEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIIEnable
*������	���ð�ȫ��֤ģʽ����״̬��H235 AnnexE II ��̲��ù�Կ/˽Կ�Ե�����ǩ��
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIIEnable(enable);
}

/******************************************************************************
* SetH235SecurityAnnexIIIEnable
*������	���ð�ȫ��֤ģʽ����״̬��H235 AnnexE III ��̶˵�����֤
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityAnnexIIIEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityAnnexIIIEnable(enable);
}



/******************************************************************************
* SetH235SecurityAnnexIVEnable
*������	���ð�ȫ��֤ģʽ����״̬��H235 AnnexF IV������ת�ӵİ�ȫ�� ����˹��I�͹��II
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/

//void IMXEC_H323::SetH235SecurityAnnexIVEnable(bool enable)
//{
//	H323CallMgr::Instance().SetH235SecurityAnnexIVEnable(enable);
//}


/******************************************************************************
* SetH235SecurityMediaDH512Enable
*������	���ð�ȫ��֤ģʽ����״̬������ý�������ܣ�ʹ��512-bit Diffie-Hellman group����7�ֽڹ�����Կ
������DES���ܵ�ʹ��7�ֽ����ڵ�������ܵ��㷨
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH512Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH512Enable(enable);
}

/******************************************************************************
* SetH235SecurityMediaDH1024Enable
*������	���ð�ȫ��֤ģʽ����״̬������ý�������ܣ�ʹ��1024-bit Diffie-Hellman group����21�ֽڹ�����Կ
����DES 3DES���ܵ�
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH1024Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH1024Enable(enable);
}


/******************************************************************************
* SetH235SecurityMediaDH1536Enable
*������	���ð�ȫ��֤ģʽ����״̬������ý�������ܣ�ʹ��1536-bit Diffie-Hellman group����21�ֽڹ�����Կ
����DES 3DES���ܵ�
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityMediaDH1536Enable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityMediaDH1536Enable(enable);
}


/******************************************************************************
* SetH235SecurityUserPrimeEnable
*������	���ÿ���״̬��Ԥ���������Ƿ�����ý����ܹ�����ԿЭ��
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityUserPrimeEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityUserPrimeEnable(enable);
}


/******************************************************************************
* SetH235SecurityCheckIncomingMsgEnable
*������	���öԽ��պ�����Ϣ������֤���״̬
*���룺	enable ����״̬ true ��Ϊ���� false������
*****************************************************************************/
void IMXEC_H323::SetH235SecurityCheckIncomingMsgEnable(bool enable)
{
	H323CallMgr::Instance().SetH235SecurityCheckIncomingMsgEnable(enable);
}


/******************************************************************************
* SetH235MediaEncryptionAlogrithmFirst
*������	����ý�������ȼ����㷨
*���룺	alogrimId �㷨ID,alogrimId �㷨ID,des is set 2, aes-128 is set 7 ,aes-256 is set 9 ,3DES is set 5
*****************************************************************************/
void IMXEC_H323::SetH235MediaEncryptionAlogrithmFirst(int alogrimId)
{
	H323CallMgr::Instance().SetH235MediaEncryptionAlogrithmFirst(alogrimId);
}


/******************************************************************************
* SetH235MediaSecurityEncryptionPWD
*������	����ý������������
*���룺	type 0 Ϊ��ʹ��ָ������(�Զ�Э��) 1 Ϊʹ��ָ������
		encrypPwd ʹ��ָ������ʱ���õļ�������
*****************************************************************************/
void IMXEC_H323::SetH235MediaSecurityEncryptionPWD(int type,char * encrypPwd)
{
	H323CallMgr::Instance().SetH235MediaSecurityEncryptionPWD(type,encrypPwd);
}

/******************************************************************************
* SendCallH245ControlMessage
*����������H245������Ϣ ����flowControlIndication H239������� �����Ϣ
*���룺	cszCallID			-���б�ʶ
nMsgType			-������Ϣ����
cszPacket			-��Ϣ��
nPacketLen			-��Ϣ������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SendCallH245ControlMessage(const char*cszCallID,H245ControlMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().DAPSendCallH245ControlMessage(cszCallID,nMsgType, cszPacket,nPacketLen);
}

int IMXEC_H323::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return H323CallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

void IMXEC_H323::SetH460NATFwOption(int option)
{
	H323CallMgr::Instance().SetH460NATFwOption(option);
}

//void SetNETECSendEnable(const char*cszH323ID,int nOptions)

