
/************************************************************************/
//BFCPTcpServer.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP TCP Net Interface
//Date:2014-08-15
/************************************************************************/

#ifndef __SIP_BFCP_TCPSOCK_SERVER_H__
#define __SIP_BFCP_TCPSOCK_SERVER_H__

#include "BFCPProcess.h"
#include "SocketHeader.h"


/************************************************************************/
//BFCPSockTCPServer
//Description:SIP BFCP TCP Net Interface
/************************************************************************/

class BFCPSockTCPServer
	: public BFCP_Process
	, public CSocketControlCallback
{
public:
	BFCPSockTCPServer(SIP_BFCPCallback&rCallback);
	virtual~BFCPSockTCPServer(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

	CSocketControl * m_pTCPServer;
	CSocketControl * m_pTCPServerStream;

protected:
	virtual void OnDataCallback(CSocketControl* pSession,const char* pData,int nDataLen);
	virtual void OnConnectCallback(CSocketControl* pSession);


};

#endif