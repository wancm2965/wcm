#include "stdafx.h"

#if defined(HAS_H323_AVCON_MP) && (HAS_H323_AVCON_MP == 1)

#include <AVCONMP/IMXEC_H323.h>
#else
#include <IMXEC/IMXEC_DAPH323.h>
#endif

#include "H323CallMgr.h"
#include "CallMgr.h"

IMXEC_H323::IMXEC_H323(void)
{

}

IMXEC_H323::~IMXEC_H323(void)
{

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
	return H323CallMgr::Instance().DAPStart();
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
	return H323CallMgr::Instance().DAPStop();
}

/******************************************************************************
* SetTypeRegisterToGK
*�������趨ע�ᵽGK����
*���룺nType	-ע������ �Զ�����GK��������Ϊ0,ע�ᵽָ����GK��������Ϊ��0ֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
//int IMXEC_H323::SetTypeRegisterToGK(int nType)
//{
//	return H323CallMgr::Instance().SetTypeDAPRegisterToGK(nType);
//}

/******************************************************************************
* SetTypeDAPAutoManualRegGK
*�����������Զ�ע�ỹ���ֶ�ע�� 
*���룺nType	-ע������ 0�Զ�ע��,1�ֶ�ע��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
//int IMXEC_H323::SetTypeDAPAutoManualRegGK(int nType)
//{
//	return H323CallMgr::Instance().SetTypeDAPAutoManualRegGK(nType);
//}

/******************************************************************************
* RegisterToGK
*������ע�ᵽGK ע���ʺź͵绰������������һ�� �趨�Զ�����GKʱcszHost����Ϊ""��NULL
*���룺	cszH323ID			-ע���ʺ�
cszTelNumber		-�绰����
cszHost				-ע���GK��������ַ 
cszRegPassword		-ע���˺�����
*�������
*����ֵ���ɹ�����>=0[account id]��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::RegisterToGK(const char*cszH323ID,const char*cszTelNumber,const char*cszHost,char*cszRegPassword)
{
	return H323CallMgr::Instance().DAPRegisterToGK(cszH323ID,cszTelNumber,cszHost,cszRegPassword);
}

/******************************************************************************
* UnRegisterToGK
*��������GK��ע���˺�
*���룺	nAccountID			-ע���ʺ�ID
*����ֵ�����óɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::UnRegisterToGK(int nAccountID)
{
	return H323CallMgr::Instance().UnDAPRegisterToGK(nAccountID);
}

/******************************************************************************
* SetUserName
*�����������û���ʾ����
*���룺	cszUserName		-�û���ʾ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetUserName(const char*cszUserName)
{
	return H323CallMgr::Instance().SetUserName(cszUserName);
}

/******************************************************************************
* SetUserID
*�����������û���
*���룺	cszUserID		-�û���
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetUserID(const char*cszUserID)
{
	return H323CallMgr::Instance().SetH323ID(cszUserID);
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
*���������ñ���IP
*���룺	cszLocalIP		-����IP
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalIP(const char*cszLocalIP)
{
	return H323CallMgr::Instance().SetLocalIP(cszLocalIP);
}

/******************************************************************************
* SetExternalIP
*���������ñ������ڵ�����ӳ��IP
*���룺	cszExternalIP		-��������ӳ��IP
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetExternalIP(const char*cszExternalIP)
{
	//return H323CallMgr::Instance().SetExternalIP(cszExternalIP);
	return 0;
}

/******************************************************************************
* SetH245RangePort
*����������H245���Ӷ˿ڷ�Χ
*���룺	nFromPort			-H245������ʼ�˿�
nToPort				-H245�������ն˿�
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetH245RangePort(unsigned short nFromPort,unsigned short nToPort)
{
	return H323CallMgr::Instance().SetDAPH245RangePort(nFromPort,nToPort);
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
	return H323CallMgr::Instance().SetRTPBasePort(nRTPBasePort);
}


/******************************************************************************
* SetProductID
*���������ò�ƷID
*���룺	cszProductID		-��ƷID
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetProductID(const char*cszProductID)
{
	return H323CallMgr::Instance().SetDAPProductID(cszProductID);
}


/******************************************************************************
* SetProductVersionID
*���������ò�Ʒ�汾��
*���룺	cszVersionID		-�汾��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetProductVersionID(const char*cszVersionID)
{
	return H323CallMgr::Instance().SetDAPProductVersionID(cszVersionID);
}


/******************************************************************************
* SetT35CountryCode
*���������ò�ƷT35CountryCode �����������������
*���룺	nT35Code		-T35CountryCode
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetT35CountryCode(int nT35Code)
{
	return H323CallMgr::Instance().SetDAPT35CountryCode(nT35Code);
}


/******************************************************************************
* SetT35Extension
*���������ò�ƷT35Extension �������������������չ
*���룺	nT35Extension		-T35Extension
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetT35Extension(int nT35Extension)
{
	return H323CallMgr::Instance().SetDAPT35Extension(nT35Extension);
}

/******************************************************************************
* SetManufacturerCode
*���������ò�ƷManufacturerCode �����̴�����Ϣ
*���룺	nManufacturerCode		-ManufacturerCode
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetManufacturerCode(int nManufacturerCode)
{
	return H323CallMgr::Instance().SetDAPManufacturerCode(nManufacturerCode);
}

/******************************************************************************
* SetH323EndPointType
*�������趨�ն�����ֵ
*���룺	nType		-��ͨH323�ն���Ϊ0,H323MCU����Ϊ1
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetH323EndPointType(int nType)
{
	return H323CallMgr::Instance().SetDAPH323EndPointType(nType);
}

/******************************************************************************
* SetCapPrintEnable
*���������ô�ӡ���˼�Զ����������ͨ��������������Ϣ����,Ĭ�ϲ���ӡ
*���룺	nPrint		-����ӡ��Ϊ0,��ӡ����Ϊ1
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetCapPrintEnable(int nPrint)
{
	return H323CallMgr::Instance().SetDAPCapPrintEnable(nPrint);
}

/******************************************************************************
* SetMasterSlaveValue
*�������趨����������ȷ��ֵ һ��MCU����Ϊ240��190 ,��ͨ�ն�����Ϊ50
*���룺	nValue		-����������ȷ��ֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetMasterSlaveValue(int nValue)
{
	return H323CallMgr::Instance().SetDAPMasterSlaveValue(nValue);
}

/******************************************************************************
* SetLocalMainVideoParam
*����������������Ƶ֡�����ʲ���
*���룺 cszCallID			-���б�ʶ
nFrameRate			-���֡��
nBitrate			-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
	return H323CallMgr::Instance().SetDAPLocalMainVideoParam(cszCallID,nFrameRate,nBitrate);
}

/******************************************************************************
* SetLocalSecondVideoParam
*���������ø�����Ƶ֡�����ʲ���
*���룺 cszCallID			-���б�ʶ
nFrameRate			-���֡��
nBitrate			-�������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
	return H323CallMgr::Instance().SetDAPLocalSecondVideoParam(cszCallID,nFrameRate,nBitrate);
}

/******************************************************************************
* SetSupportH239
*�����������Ƿ�֧��H239
*���룺cszCallID -���б�ʶ nSupportH239		-�Ƿ�֧��H239
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetSupportH239(const char*cszCallID,int nSupportH239)
{
	return H323CallMgr::Instance().SetH323CallSupportH239(cszCallID,nSupportH239);
}

/******************************************************************************
* SetLocalMainVideoH264Level
*���������ñ���������ƵH264����ʹ�õ����Levelֵ ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-��������ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalMainVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalSecondVideoH264Level
*���������ñ��ظ�����ƵH264����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-���ظ�����ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalSecondVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalMainVideoH264HPLevel
*���������ñ���������ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-��������ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalMainVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalSecondVideoH264HPLevel
*���������ñ��ظ�����ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-���ظ�����ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return H323CallMgr::Instance().SetLocalSecondVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetCallRTPMediaMulticast 
*����������RTP ý�����鲥 ���������鲥IP��������鲥
*���룺	cszCallID		-���б�ʶ
cszMulticastIP	-�鲥IP
ttl				-�鲥���ݰ���TTL(����ʱ��)
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return H323CallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

/******************************************************************************
* SendCallNonstandardData
*��������Զ˷��ͷǱ���Ϣ
*���룺	cszCallID		-���б�ʶ
cszPacket		-�Ǳ�׼��Ϣ����
nPacketLen		-�Ǳ�׼��Ϣ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SendCallNonstandardData(const char*cszCallID,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().DAPSendCallH245NonstandardData(cszCallID,cszPacket);
}

/******************************************************************************
* OpenDualVideoOutChannel
*�������򿪸�������ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::OpenDualVideoOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().OpenDualVideoOutChannel(cszCallID);
}


/******************************************************************************
* OpenFECCOutChannel
*��������FECC���Ʒ���ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ����
*****************************************************************************/
int IMXEC_H323::OpenFECCOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().OpenFECCOutChannel(cszCallID);
}

/******************************************************************************
* CloseDualVideoOutChannel
*�������رո�������ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::CloseDualVideoOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().CloseDualVideoOutChannel(cszCallID);
}

/******************************************************************************
* CloseFECCOutChannel
*�������ر�FECC���Ʒ���ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::CloseFECCOutChannel(const char*cszCallID)
{
	return H323CallMgr::Instance().CloseFECCOutChannel(cszCallID);
}


/******************************************************************************
* AudioRecvChannelAnswer
*���������ܶԶ˴򿪵���Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::AudioRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().AudioRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* MainVideoRecvChannelAnswer
*���������ܶԶ˴򿪵�������Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::MainVideoRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().MainVideoRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* DualVideoRecvChannelAnswer
*���������ܶԶ˴򿪵ĸ�����Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::DualVideoRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().DualVideoRecvChannelAnswer(cszCallID);
}


/******************************************************************************
* FECCRecvChannelAnswer
*���������ܶԶ˴򿪵�FECC���Ʒ���ͨ��
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::FECCRecvChannelAnswer(const char*cszCallID)
{
	return H323CallMgr::Instance().FECCRecvChannelAnswer(cszCallID);
}



/******************************************************************************
* AudioRecvChannelReject
*�������ܾ����ܶԶ˴򿪵���Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
nReason				-�ܾ�ԭ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::AudioRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().AudioRecvChannelReject(cszCallID,nReason);
}

/******************************************************************************
* MainVideoRecvChannelReject
*�������ܾ����ܶԶ˴򿪵�������Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
nReason				-�ܾ�ԭ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::MainVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().MainVideoRecvChannelReject(cszCallID,nReason);
}


/******************************************************************************
* DualVideoRecvChannelReject
*�������ܾ����ܶԶ˴򿪵ĸ�����Ƶ����ͨ��
*���룺	cszCallID			-���б�ʶ
nReason				-�ܾ�ԭ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::DualVideoRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().DualVideoRecvChannelReject(cszCallID,nReason);
}


/******************************************************************************
* FECCRecvChannelReject
*�������ܾ����ܶԶ˴򿪵�FECC���Ʒ���ͨ��
*���룺	cszCallID			-���б�ʶ
nReason				-�ܾ�ԭ��
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::FECCRecvChannelReject(const char*cszCallID,H245ChanRejectReason nReason)
{
	return H323CallMgr::Instance().FECCRecvChannelReject(cszCallID,nReason);
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

/******************************************************************************
* SendRequestMainVideoKeyFrame
*����������������Ƶ�ؼ�֡������Ϣ
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SendRequestMainVideoKeyFrame(const char*cszCallID)
{
	return H323CallMgr::Instance().SendRequestMainVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SendRequestSecondVideoKeyFrame
*���������͸�����Ƶ�ؼ�֡������Ϣ
*���룺	cszCallID			-���б�ʶ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SendRequestSecondVideoKeyFrame(const char*cszCallID)
{
	return H323CallMgr::Instance().SendRequestSecondVideoKeyFrame(cszCallID);
}

/******************************************************************************
* SendCallChannelMessage
*�����������߼�ͨ����Ϣ ����FlowControlCommand videoFastUpdateGOB videoFastUpdateMB
*���룺	cszCallID			-���б�ʶ
nMsgType			-������Ϣ����
cszPacket			-��Ϣ�� ����ý�������ֶ� �շ������ֶ�
nPacketLen			-��Ϣ������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
//int IMXEC_H323::SendCallChannelMessage(const char*cszCallID,H245ChanMessageType nMsgType,const char * cszPacket,int nPacketLen)
//{
//	return H323CallMgr::Instance().SendCallChannelMessage(cszCallID,nMsgType, cszPacket,nPacketLen);
//}

/******************************************************************************
* SendRASMessage
*����������RAS��Ϣ nMsgType �ض���
*���룺	nMsgType			-��Ϣ����
cszPacket			-��Ϣ��
nPacketLen			-��Ϣ������
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_H323::SendRASMessage(H225RASMessageType nMsgType,const char * cszPacket,int nPacketLen)
{
	return H323CallMgr::Instance().SendRASMessage(nMsgType, cszPacket,nPacketLen);
}


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
*���룺	alogrimId �㷨ID 2ΪDES CBCģʽ��5Ϊ3DES CBCģʽ
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
