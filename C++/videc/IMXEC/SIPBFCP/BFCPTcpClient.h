
/************************************************************************/
//BFCPTcpClient.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP TCP Net Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_TCP_CLIENT_H__
#define __SIP_BFCP_TCP_CLIENT_H__

#include "BFCPProcess.h"
#include <NETEC/NETEC_Stream.h>

/************************************************************************/
//BFCP_TCPClient
//Description:SIP BFCP TCP Net Interface
/************************************************************************/

class BFCP_TCPClient
	: public BFCP_Process
	, public NETEC_StreamCallback
{
public:
	BFCP_TCPClient(SIP_BFCPCallback&rCallback);
	virtual~BFCP_TCPClient(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	virtual void OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_StreamCallbackConnected(void);
	virtual void OnNETEC_StreamCallbackDisconnected(void);
	NETEC_Stream * m_pTCPClient;

};

#endif