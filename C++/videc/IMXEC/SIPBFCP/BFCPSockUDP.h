
/************************************************************************/
//BFCPUdp.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP UDP Net Interface
//Date:2014-08-15
/************************************************************************/

#ifndef __SIP_BFCP_UDPSOCK_H__
#define __SIP_BFCP_UDPSOCK_H__

#include "BFCPProcess.h"
#include "SocketHeader.h"

/************************************************************************/
//BFCPSockUDP
//Description:SIP BFCP UDP Net Interface
/************************************************************************/
class BFCPSockUDP
	: public BFCP_Process
	, public CSocketControlCallback
{
public:
	BFCPSockUDP(SIP_BFCPCallback&rCallback);
	virtual~BFCPSockUDP(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

protected:
	char m_szRemoteIP[64];
	unsigned short m_nRemotePort;
	char m_szLocalIP[64];
	unsigned short m_nLocalPort;

	CSocketControl * m_pUDPServer;

protected:
	virtual void OnDataCallback(CSocketControl* pSession,const char* pData,int nDataLen);


};


#endif