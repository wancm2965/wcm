
#include "BFCPTcpClient.h"

BFCP_TCPClient::BFCP_TCPClient(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{
	m_pTCPClient=NULL;
}

void BFCP_TCPClient::OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen)
{
	OnRcvdData(pData,nLen);
}

void BFCP_TCPClient::OnNETEC_StreamCallbackConnected(void)
{
	OnBFCPTCPConnected(e_BFCP_TCP_Client);
}

void BFCP_TCPClient::OnNETEC_StreamCallbackDisconnected(void)
{

}


bool BFCP_TCPClient::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	bool bResult=false;
	
	if(m_pTCPClient==NULL)
	{
		m_pTCPClient=NETEC_Stream::Create(*this);
		if(m_pTCPClient) 
		{
			if(m_pTCPClient->Open(cszRemoteIP,nRemotePort)==0)
			{
				bResult=true;
			}
			else
			{
				m_pTCPClient->Close();
				delete m_pTCPClient;
				m_pTCPClient=NULL;
			}
		}
	}
	return bResult;
}

void BFCP_TCPClient::Close(void)
{
	NETEC_Stream * pTCPClient=m_pTCPClient;
	m_pTCPClient=NULL;

	if(pTCPClient)
	{
		pTCPClient->Close();
		delete pTCPClient;
		pTCPClient=NULL;
	}

}

int BFCP_TCPClient::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	if(m_pTCPClient) 
	{
		nSendlen=m_pTCPClient->SendData(pData,nLen);
	}
	return nSendlen;
}
