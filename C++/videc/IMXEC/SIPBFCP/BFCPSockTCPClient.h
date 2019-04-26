
/************************************************************************/
//BFCPTcpClient.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP TCP Net Interface
//Date:2014-08-15
/************************************************************************/

#ifndef __SIP_BFCP_TCPSOCK_CLIENT_H__
#define __SIP_BFCP_TCPSOCK_CLIENT_H__

#include "BFCPProcess.h"
#include "SocketHeader.h"

/************************************************************************/
//BFCPSockTCPClient
//Description:SIP BFCP TCP Net Interface
/************************************************************************/

class BFCPSockTCPClient
	: public BFCP_Process
	, public CSocketControlCallback
{
public:
	BFCPSockTCPClient(SIP_BFCPCallback&rCallback);
	virtual~BFCPSockTCPClient(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	CSocketControl * m_pTCPClient;

protected:
	virtual void OnDataCallback(CSocketControl* pSession,const char* pData,int nDataLen);
	virtual void OnConnectCallback(CSocketControl* pSession);

};

#endif