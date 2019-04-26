
/************************************************************************/
//BFCPTcpServer.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP TCP Net Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_TCP_SERVER_H__
#define __SIP_BFCP_TCP_SERVER_H__

#include "BFCPProcess.h"
#include <NETEC/NETEC_TCPServer.h>


/************************************************************************/
//BFCP_TCPServer
//Description:SIP BFCP TCP Net Interface
/************************************************************************/

class BFCP_TCPServer
	: public BFCP_Process
	, public NETEC_TCPServerNotify
	, public NETEC_TCPServerStreamCallback
{
public:
	BFCP_TCPServer(SIP_BFCPCallback&rCallback);
	virtual~BFCP_TCPServer(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	virtual bool OnNETEC_TCPServerNotifyConnected(SOCKET hSocket,const char*cszLocalIP,const char*cszPeerIP);
	virtual void OnNETEC_TCPServerStreamCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_TCPServerStreamCallbackDisconnected(void);

	NETEC_TCPServer * m_pTCPServer;
	NETEC_TCPServerStream * m_pTCPServerStream;

};

#endif