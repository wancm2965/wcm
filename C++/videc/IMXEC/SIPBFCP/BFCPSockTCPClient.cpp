
#include "BFCPSockTCPClient.h"
#include "SocketControl.h"

BFCPSockTCPClient::BFCPSockTCPClient(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{
	m_pTCPClient=NULL;
}

void BFCPSockTCPClient::OnDataCallback(CSocketControl* pSession,const char*pData,int nLen)
{
	OnRcvdData(pData,nLen);
}

void BFCPSockTCPClient::OnConnectCallback(CSocketControl* pSession)
{
	OnBFCPTCPConnected(e_BFCP_TCP_Client);
}

bool BFCPSockTCPClient::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	bool bResult=false;
	if(m_pTCPClient==NULL)
	{
		m_pTCPClient=new CSocketControl(*this);
		if(m_pTCPClient) 
		{
			//m_pTCPClient->SetTCPManualAddrEnable(true);
			m_pTCPClient->SetTCPKeepAliveEnable(true,2000,1000);
			if(m_pTCPClient->Open(nLocalPort,(char *)cszLocalIP, 2,nRemotePort,(char *)cszRemoteIP))
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

void BFCPSockTCPClient::Close(void)
{
	CSocketControl * pTCPClient=m_pTCPClient;
	m_pTCPClient=NULL;
	if(pTCPClient)
	{
		pTCPClient->Close();
		delete pTCPClient;
		pTCPClient=NULL;
	}
}

int BFCPSockTCPClient::SendData(const char*pData,int nLen)
{
	int nSendlen=0;
	if(m_pTCPClient) 
	{
		nSendlen=m_pTCPClient->SendData((unsigned char *)pData,nLen);
	}
	return nSendlen;
}
