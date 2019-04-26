
/************************************************************************/
//BFCPPJSIPUdp.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP UDP Net Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_PJSIP_UDP_H__
#define __SIP_BFCP_PJSIP_UDP_H__

#include "BFCPProcess.h"
#include "PJSIPUdp.h"

/************************************************************************/
//BFCP_PJSIP_UDP
//Description:SIP BFCP PJSIP UDP Net Interface
/************************************************************************/
class BFCP_PJSIP_UDP
	: public BFCP_Process
	, public PJSIP_UDPCallback
{
public:
	BFCP_PJSIP_UDP(SIP_BFCPCallback&rCallback);
	virtual~BFCP_PJSIP_UDP(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void Close(void);
	virtual int SendData(const char*pData,int nLen);

public:
	virtual bool OnPJSIP_UDPNotifyRecvdPacket(const char*pData,int nLen);
	PJSIP_UDPServer * m_pUDPServer;

};


#endif