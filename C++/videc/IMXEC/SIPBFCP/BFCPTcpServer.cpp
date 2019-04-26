
#include "BFCPTcpServer.h"

BFCP_TCPServer::BFCP_TCPServer(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{
	m_pTCPServer=NULL;
	m_pTCPServerStream=NULL;
}

void BFCP_TCPServer::OnNETEC_TCPServerStreamCallbackRecvdData(const char*pData,int nLen)
{
	OnRcvdData(pData,nLen);
}

bool BFCP_TCPServer::OnNETEC_TCPServerNotifyConnected(SOCKET hSocket,const char*cszLocalIP,const char*cszPeerIP)
{
	if (m_pTCPServerStream == NULL)
	{
		m_pTCPServerStream=NETEC_TCPServerStream::Create(*this,hSocket,cszLocalIP,cszPeerIP);
		if(m_pTCPServerStream) 
		{
			OnBFCPTCPConnected(e_BFCP_TCP_Server);
		}
	}

	return (m_pTCPServerStream!=NULL);

}

void BFCP_TCPServer::OnNETEC_TCPServerStreamCallbackDisconnected(void)
{
	NETEC_TCPServerStream*pTCPServerStream=m_pTCPServerStream;
	m_pTCPServerStream=NULL;

	if(pTCPServerStream)
	{
		pTCPServerStream->Close();
		delete pTCPServerStream;
		pTCPServerStream=NULL;
	}

	if(m_pTCPServer)
	{
		m_pTCPServer->ReleaseConnections();
		delete m_pTCPServer;
		m_pTCPServer=NULL;
	}
}

bool BFCP_TCPServer::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	if(m_pTCPServer==NULL)
	{
		m_pTCPServer=NETEC_TCPServerCreate(*this,nLocalPort);
	}
	return (m_pTCPServer!=NULL);
}

void BFCP_TCPServer::Close(void)
{
	NETEC_TCPServerStream*pTCPServerStream=m_pTCPServerStream;
	m_pTCPServerStream=NULL;

	if(pTCPServerStream)
	{
		pTCPServerStream->Close();
		delete pTCPServerStream;
		pTCPServerStream=NULL;
	}

	if(m_pTCPServer)
	{
		m_pTCPServer->ReleaseConnections();
		delete m_pTCPServer;
		m_pTCPServer=NULL;
	}
}

int BFCP_TCPServer::SendData(const char*pData,int nLen)
{
	int nRet=0;
	if(m_pTCPServerStream)
	{
		nRet=m_pTCPServerStream->SendData(pData,nLen);
	}
	return nRet;
}
