//NETEC_Server.h
#ifndef __NETEC_SERVER_H__
#define __NETEC_SERVER_H__

#include "NETEC_Session.h"

//网络Server
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
	//启动本地服务
	virtual int32_t Start(void)=0;
	
	//停止本地服务
	virtual void Stop(void)=0;

	//获得客户SESSION数目
	virtual uint32_t GetClientCount(void)=0;

	//发送数据给指定的网络客户端
	virtual int32_t SendDataToClient(uint32_t nClientID,const char*pData,int32_t nLen)=0;

	//强制断开客户端
	virtual void DisconnectClient(uint32_t nClientID)=0;

	static NETEC_Server*Create(NETEC_ServerCallback&rCallback,unsigned short usServerType);
};

#endif