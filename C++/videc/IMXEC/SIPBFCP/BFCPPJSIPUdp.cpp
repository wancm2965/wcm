
#include "BFCPPJSIPUdp.h"

BFCP_PJSIP_UDP::BFCP_PJSIP_UDP(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{	
	m_pUDPServer=NULL;
}

bool BFCP_PJSIP_UDP::OnPJSIP_UDPNotifyRecvdPacket(const char*pData,int nLen)
{
	OnRcvdData(pData,nLen);
	return true;
}

bool BFCP_PJSIP_UDP::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	if(m_pUDPServer == NULL)
	{
		m_pUDPServer=PJSIP_UDPServerCreate(*this);
		if(m_pUDPServer) 
		{
			m_pUDPServer->PJSIPCreateBFCPTransports(cszLocalIP,nLocalPort,cszRemoteIP,nRemotePort);

		}
	}
	return (m_pUDPServer != NULL);
}

void BFCP_PJSIP_UDP::Close(void)
{
	PJSIP_UDPServer * pUDPServer=m_pUDPServer;
	m_pUDPServer=NULL;
	
	if(pUDPServer)
	{
		pUDPServer->PJSIPCloseBFCPTransports();
		delete pUDPServer;
		pUDPServer=NULL;
	}
}

int BFCP_PJSIP_UDP::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	
	if(m_pUDPServer) 
	{
		//notice test
		if(nLen>12)
			nSendlen=m_pUDPServer->PJSIPSendData(pData,nLen);
	}

	return nSendlen;
}

