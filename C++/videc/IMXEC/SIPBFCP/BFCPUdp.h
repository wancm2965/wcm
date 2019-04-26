
/************************************************************************/
//BFCPUdp.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP UDP Net Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_UDP_H__
#define __SIP_BFCP_UDP_H__

#include "BFCPProcess.h"
#include <NETEC/NETEC_UDPServer.h>

/************************************************************************/
//BFCP_UDP
//Description:SIP BFCP UDP Net Interface
/************************************************************************/
class BFCP_UDP
	: public BFCP_Process
	, public NETEC_UDPServerNotify
{
public:
	BFCP_UDP(SIP_BFCPCallback&rCallback);
	virtual~BFCP_UDP(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	char m_szRemoteIP[64];
	unsigned short m_nRemotePort;
	char m_szLocalIP[64];
	unsigned short m_nLocalPort;

protected:
	virtual bool OnNETEC_UDPServerNotifyRecvdPacket(UDPServerPacket*pUDPServerPacket);
	NETEC_UDPServer * m_pUDPServer;
	SOCKET m_nSocket;

};


#endif