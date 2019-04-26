//NETEC_Server.h
#ifndef __NETEC_SERVER_H__
#define __NETEC_SERVER_H__

#include "NETEC_Session.h"

//����Server
class NETEC_ServerCallback
{
public:
	NETEC_ServerCallback(void){};
	virtual~NETEC_ServerCallback(void){};
public:
	virtual void OnNETEC_ServerClientConnected(uint32_t ulClientID,const char*cszClientRemoteIP)=0;
	virtual void OnNETEC_ServerClientDisconnected(uint32_t ulClientID)=0;
	virtual void OnNETEC_ServerReceivedFromClient(uint32_t ulClientID,const char*pData,int32_t nLen)=0;

};

class NETEC_API NETEC_Server
{
public:
	NETEC_Server(void){};
	virtual ~NETEC_Server(void){};
public:
	//�������ط���
	virtual int32_t Start(void)=0;
	
	//ֹͣ���ط���
	virtual void Stop(void)=0;

	//��ÿͻ�SESSION��Ŀ
	virtual uint32_t GetClientCount(void)=0;

	//�������ݸ�ָ��������ͻ���
	virtual int32_t SendDataToClient(uint32_t nClientID,const char*pData,int32_t nLen)=0;

	//ǿ�ƶϿ��ͻ���
	virtual void DisconnectClient(uint32_t nClientID)=0;

	static NETEC_Server*Create(NETEC_ServerCallback&rCallback,unsigned short usServerType);
};

#endif