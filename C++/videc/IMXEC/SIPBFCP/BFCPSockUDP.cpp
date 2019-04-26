
#include "BFCPSockUDP.h"
#include "SocketControl.h"

BFCPSockUDP::BFCPSockUDP(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{	
	m_pUDPServer=NULL;
	memset(m_szRemoteIP,0,64);
	m_nRemotePort=0;
	memset(m_szLocalIP,0,64);
	m_nLocalPort=0;
}

void BFCPSockUDP::OnDataCallback(CSocketControl* pSession,const char* pData,int nDataLen)
{
	OnRcvdData((const char *)pData,nDataLen);
}

bool BFCPSockUDP::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	strcpy(m_szRemoteIP,cszRemoteIP);
	m_nRemotePort=nRemotePort;
	strcpy(m_szLocalIP,cszLocalIP);
	m_nLocalPort=nLocalPort;

	if(m_pUDPServer == NULL)
	{
		m_pUDPServer=new CSocketControl(*this);
		if(m_pUDPServer) 
		{
			m_pUDPServer->Open(nLocalPort,(char *)cszLocalIP,3,nRemotePort,(char *)cszRemoteIP);
		}
	}
	return (m_pUDPServer != NULL);
}

void BFCPSockUDP::Close(void)
{
	CSocketControl * pUDPServer=m_pUDPServer;
	m_pUDPServer=NULL;
	if(pUDPServer)
	{
		pUDPServer->Close();
		delete pUDPServer;
		pUDPServer=NULL;
	}
}

int BFCPSockUDP::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	if(m_pUDPServer) 
	{
		//m_pUDPServer->SendToData((unsigned char *)pData,nLen,m_nRemotePort,m_szRemoteIP);
		m_pUDPServer->SendData((unsigned char *)pData,nLen);
	}
	return nSendlen;
}

