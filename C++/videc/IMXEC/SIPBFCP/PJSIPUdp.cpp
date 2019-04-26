
#include "PJSIPUdp.h"


PJSIP_UDPServer*PJSIP_UDPServerCreate(PJSIP_UDPCallback& rPJSIP_UDPCallback)
{
	PJSIP_UDPServer *  pPJSIP_UDPServer=NULL;
	pPJSIP_UDPServer=new PJSIP_UDPServer(rPJSIP_UDPCallback);
	return pPJSIP_UDPServer;
}

static void on_rx_bfcp( void *data,void *pkt,pj_ssize_t bytes_read)
{
	PJSIP_UDPServer * pPJSIP_UDPServer=(PJSIP_UDPServer *)data;
	if(pPJSIP_UDPServer) pPJSIP_UDPServer->OnPJSIPRecvData((const char *)pkt,bytes_read);
}

static void on_rx_bfcp2( void *data,void *pkt,pj_ssize_t bytes_read)
{

}


PJSIP_UDPServer::PJSIP_UDPServer(PJSIP_UDPCallback& rPJSIP_UDPCallback)
:m_rPJSIP_UDPCallback(rPJSIP_UDPCallback)
{
	m_pRtpSession=&m_session;
	m_pRtpRecvSession=&m_recvsession;
	m_pRtcpSession=&m_rtcpsession;
	m_bThreadRegistered=false;
	m_pMediaTransport=NULL;
}


void PJSIP_UDPServer::OnPJSIPRecvData(const char*pData,int nLen)
{
	m_rPJSIP_UDPCallback.OnPJSIP_UDPNotifyRecvdPacket(pData,nLen);
}

void PJSIP_UDPServer::PJSIPCreateBFCPTransports(const char*cszLocalIP,unsigned short nLocalPort,
												const char*cszRemoteIP,unsigned short nRemotePort)
{
	PJSIPCloseBFCPTransports();

	unsigned int port=0;char ip[16]={0};
	int pt=34;	pj_status_t status=-1;
	pj_uint32_t sender_ssrc=1;
	pjsua_transport_config  rtp_cfg;
	char *name="VideoRTCP";
	unsigned clock_rate=1000;
	unsigned samples_per_frame=30;
	pjmedia_sock_info  skinfo;

	port=nRemotePort;
	strcpy(ip,cszRemoteIP);

	int af = PJ_AF_INET;
	pj_str_t  rem_addr=pj_str(ip);
	int rem_port=port;
	pj_sockaddr_in addr;
	int addrlen;
	addrlen = sizeof(addr);

	pj_sockaddr		rem_rtp_addr;
	pj_sockaddr		rem_rtcp_addr;
	status = pj_sockaddr_init(af, &rem_rtp_addr, &rem_addr,port);
	status = pj_sockaddr_init(af, &rem_rtcp_addr, &rem_addr,port+1);

	pjsua_transport_config_default(&rtp_cfg);
	rtp_cfg.port = nLocalPort;
	pjmedia_rtp_session_init(m_pRtpSession,pt,sender_ssrc);
	pjmedia_rtp_session_init(m_pRtpRecvSession,pt,sender_ssrc);
	pjmedia_rtcp_init(m_pRtcpSession,name,clock_rate,samples_per_frame,sender_ssrc);

	status=create_udp_media_transports_bfcp(&rtp_cfg,&skinfo,&m_pMediaTransport);
	if (status != PJ_SUCCESS) 
		m_pMediaTransport=NULL;
	if(m_pMediaTransport)	
		transport_attach_bfcp(m_pMediaTransport,this,&rem_rtp_addr,&rem_rtcp_addr,addrlen,on_rx_bfcp,on_rx_bfcp2);

}

void PJSIP_UDPServer::PJSIPCloseBFCPTransports()
{
	if(m_pMediaTransport) 
	{
		transport_detach_bfcp(m_pMediaTransport,this);
		transport_destroy_bfcp(m_pMediaTransport);
		m_pMediaTransport=NULL;
	}
}

int PJSIP_UDPServer::PJSIPSendData(const char*pData,int nLen)
{
	pj_status_t status=-1;
	if(m_pMediaTransport) 
		status=transport_send_bfcp(m_pMediaTransport,pData,nLen);
	if (status != PJ_SUCCESS) 
		nLen=0;
	return nLen;

}
