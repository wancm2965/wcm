
/************************************************************************/
//PJSIPUdp.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP UDP Net Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_PJSIP_UDP_H__
#define __SIP_PJSIP_UDP_H__

#include "pjsip.h" 
#include <pjsua-lib/pjsua.h>
#include <pjsua-lib/pjsua_internal.h>
/************************************************************************/
//PJSIP_UDP
//Description:PJSIP UDP Net Interface
/************************************************************************/

class PJSIP_UDPCallback
{
public:
	PJSIP_UDPCallback(void){};
	virtual~PJSIP_UDPCallback(void){};
public:
	virtual bool OnPJSIP_UDPNotifyRecvdPacket(const char*pData,int nLen)=0;
};


class PJSIP_UDPServer
{
public:
	PJSIP_UDPServer(PJSIP_UDPCallback& rPJSIP_UDPCallback);
	virtual~PJSIP_UDPServer(){};

public:
	virtual void OnPJSIPRecvData(const char*pData,int nLen);
	virtual void PJSIPCreateBFCPTransports(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort);
	virtual void PJSIPCloseBFCPTransports();
	virtual int PJSIPSendData(const char*pData,int nLen);

protected:
	bool m_bThreadRegistered;
	pjmedia_rtp_session m_session;
	pjmedia_rtp_session *m_pRtpSession;
	pjmedia_rtcp_session m_rtcpsession;
	pjmedia_rtcp_session *m_pRtcpSession;
	pjmedia_transport * m_pMediaTransport;
	pjmedia_rtp_session m_recvsession;
	pjmedia_rtp_session *m_pRtpRecvSession;
	PJSIP_UDPCallback& m_rPJSIP_UDPCallback;
};

PJSIP_UDPServer*PJSIP_UDPServerCreate(PJSIP_UDPCallback& rPJSIP_UDPCallback);


#endif