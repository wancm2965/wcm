#include "stdafx.h"

#include "../DefineHeader/SIPDefineHeader.h"

#if defined(HAS_SIP_AVCON_MP) && (HAS_SIP_AVCON_MP == 1)

#include <AVCONMP/IMXEC_SIP.h>
#else
#include <IMXEC/IMXEC_DAPSIP.h>
#endif


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
* SetAgentName
*����������Ӧ��ʵ���� userAgent
*���룺	cszAgent		-Ӧ��ʵ����
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetAgentName(const char*cszAgent)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetAgentName(cszAgent);
#else
	return -1;
#endif
}

/******************************************************************************
* SetCapPrintEnable
*���������ô�ӡ���˼�Զ����������ͨ��������������Ϣ����,Ĭ�ϲ���ӡ
*���룺	nPrint		-����ӡ��Ϊ0,��ӡ����Ϊ1
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetCapPrintEnable(int nPrint)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPCapPrintEnable(nPrint);
#else
	return -1;
#endif
}

int IMXEC_SIP::SetRegisterTimer(int regTimer,int regFailTimer)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetRegisterTimer(regTimer,regFailTimer);
#else
	return -1;
#endif
}

int IMXEC_SIP::SetSIPLogFile(int logLevel,const char*cszLogFileName)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPLogFile(logLevel,cszLogFileName);
#else
	return -1;
#endif
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
int IMXEC_SIP::SetLocalMainVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPLocalMainVideoParam(cszCallID,nFrameRate,nBitrate);
#else
	return -1;
#endif
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
int IMXEC_SIP::SetLocalSecondVideoParam(const char*cszCallID,int nFrameRate,int nBitrate)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetDAPLocalSecondVideoParam(cszCallID,nFrameRate,nBitrate);
#else
	return -1;
#endif
}

/******************************************************************************
* SetSupportH239
*�����������Ƿ�֧��H239
*���룺cszCallID -���б�ʶ nSupportH239		-�Ƿ�֧��H239
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetSupportH239(const char*cszCallID,int nSupportH239)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().SetSIPCallSupportH239(cszCallID,nSupportH239);
#else
	return -1;
#endif
}

/******************************************************************************
* SetLocalMainVideoH264Level
*���������ñ���������ƵH264����ʹ�õ����Levelֵ ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-��������ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetLocalMainVideoH264Level(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalMainVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalSecondVideoH264Level
*���������ñ��ظ�����ƵH264����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-���ظ�����ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetLocalSecondVideoH264Level(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalSecondVideoH264Level(cszCallID,nLevel);
}


/******************************************************************************
* SetLocalMainVideoH264HPLevel
*���������ñ���������ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-��������ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetLocalMainVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalMainVideoH264HPLevel(cszCallID,nLevel);
}

/******************************************************************************
* SetLocalSecondVideoH264HPLevel
*���������ñ��ظ�����ƵH264HighProfile����ʹ�õ����Levelֵ  ���������ں��м���������ʱ������ʹ�ô�Levelֵ
*���룺	cszCallID		-���б�ʶ
nLevel			-���ظ�����ƵLevelֵ
*�������
*����ֵ���ɹ�����0��ʧ�ܷ���-1
*****************************************************************************/
int IMXEC_SIP::SetLocalSecondVideoH264HPLevel(const char*cszCallID,int nLevel)
{
	return SIPCallMgr::Instance().SetLocalSecondVideoH264HPLevel(cszCallID,nLevel);
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
int IMXEC_SIP::SetCallRTPMediaMulticast(const char*cszCallID,const char*cszMulticastIP,int ttl)
{
	return SIPCallMgr::Instance().SetCallRTPMediaMulticast(cszCallID,cszMulticastIP,ttl);
}

/******************************************************************************
*���������͸�������������Ϣ
*���룺	cszCallID		-���б�ʶ
nType			-��������������Ϣ���� 1 ���� 2 ��Ӧ 3 ָʾ 4 ȡ��
transactionId	-����ID  
userId			-�û�ID  
floorId			-����������ID 
status			-״̬ �緢����Ӧʱ 0��ʾ�ܾ� 1��ʾ����
*****************************************************************************/
int IMXEC_SIP::SendCallDualVideoControl(const char*cszCallID,int nType,int transactionId,int userId,int floorId,int status)
{
	return SIPCallMgr::Instance().SendCallDualVideoControl(cszCallID,nType,transactionId,userId,floorId,status);
}

/******************************************************************************
*��������Զ˷����Զ���Ǳ���Ϣ
*���룺	cszCallID		-���б�ʶ
		cszContent		-�Զ���Ǳ�׼��Ϣ����
		cszMimeType		-�Զ���Ǳ���Ϣ�������� ��application/xml
*****************************************************************************/

int IMXEC_SIP::SendCallCustomMsg(const char*cszCallID,const char * cszContent,const char * cszMimeType)
{
#ifdef HAS_SIP
	return SIPCallMgr::Instance().DAPSendCallCustomMsg(cszCallID,cszContent,cszMimeType);
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
