
#include "BFCPUdp.h"

BFCP_UDP::BFCP_UDP(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{	
	m_pUDPServer=NULL;
	m_nSocket=0;

	memset(m_szRemoteIP,0,64);
	m_nRemotePort=0;

	memset(m_szLocalIP,0,64);
	m_nLocalPort=0;
}

bool BFCP_UDP::OnNETEC_UDPServerNotifyRecvdPacket(UDPServerPacket*pUDPServerPacket)
{
	OnRcvdData(pUDPServerPacket->GetData(),pUDPServerPacket->GetDataLen() );
	return true;
}

bool BFCP_UDP::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	strcpy(m_szRemoteIP,cszRemoteIP);
	m_nRemotePort=nRemotePort;
	strcpy(m_szLocalIP,cszLocalIP);
	m_nLocalPort=nLocalPort;

	if(m_pUDPServer == NULL)
	{
		m_pUDPServer=NETEC_UDPServerCreate(*this,nLocalPort);
		if(m_pUDPServer) 
		{
			m_nSocket=m_pUDPServer->GetSocketHandle(cszLocalIP);

			//SendBFCPHello();
		}
	}
	return (m_pUDPServer != NULL);
}

void BFCP_UDP::Close(void)
{
	NETEC_UDPServer * pUDPServer=m_pUDPServer;
	m_pUDPServer=NULL;
	
	//OutputDebugString("BFCP_UDP::Close\n");

	if(pUDPServer)
	{
		pUDPServer->ReleaseConnections();
		delete pUDPServer;
		pUDPServer=NULL;
	}
}

int BFCP_UDP::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	
	//OutputDebugString("BFCP_UDP SendData\n");

	if(m_pUDPServer) 
	{
		if(m_nSocket<=0) 
		{
			m_nSocket=m_pUDPServer->GetSocketHandle(m_szLocalIP);
		}

		sockaddr_in struSockAddr; //socket address
		struSockAddr.sin_family=AF_INET;
		struSockAddr.sin_port=htons(m_nRemotePort);  
		struSockAddr.sin_addr.s_addr=inet_addr((char * )(m_szRemoteIP));
		int addrLen = sizeof(sockaddr_in);
		UDPServerPacket tPacket(m_nSocket,(sockaddr*)&struSockAddr,addrLen,pData,nLen);
		nSendlen=tPacket.Send();
	}

	return nSendlen;
}

