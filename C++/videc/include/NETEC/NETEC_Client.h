//NETEC_Client.h
#ifndef __NETEC_CLIENT_H__
#define __NETEC_CLIENT_H__

#include "NETEC_Session.h"

//����Ӧ�÷���
class NETEC_API NETEC_ClientCallback
{
public:
	NETEC_ClientCallback(void){};
	virtual~NETEC_ClientCallback(void){};
public:
	//�յ�����״̬
	virtual void OnNETEC_SessionConnectStatus(NETEC_Session::CONNECT_STATUS cs,int32_t nErrorCode=0){};

	//����������Ӧ���յ������ݰ��ص�
	virtual void OnNETEC_ClientReceivedFromServer(const char* pData,int32_t nLen){};
};

class NETEC_API NETEC_Client
{
public:
	NETEC_Client(void){};
	virtual~NETEC_Client(void){};
public:
	//���ӷ�����
	virtual int32_t Connect(unsigned short nServerType,const char*cszIP,unsigned short usPort,const char*cszInterface)=0;
	//�Ͽ�����
	virtual void Disconnect(void)=0;

	//�������ݸ�ָ��������Server
	virtual int32_t SendDataToServer(const char*pData,int32_t nLen)=0;

	virtual uint32_t GetClientID(void)=0;

	//ȡ���ص�ַ
	virtual const char*GetLocalIP(void)=0;

	//ȡ������ַ
	virtual const char*GetNATIP(void)=0;

	static NETEC_Client*Create(NETEC_ClientCallback&rCallback);
};

#endif