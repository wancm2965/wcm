
#include "BFCPSockTCPServer.h"
#include "SocketControl.h"

BFCPSockTCPServer::BFCPSockTCPServer(SIP_BFCPCallback& rCallback)
:BFCP_Process(rCallback)
{
	m_pTCPServer=NULL;
	m_pTCPServerStream=NULL;
}

void BFCPSockTCPServer::OnDataCallback(CSocketControl* pSession,const char*pData,int nLen)
{
	OnRcvdData(pData,nLen);
}

void BFCPSockTCPServer::OnConnectCallback(CSocketControl* pSession)
{
	if (m_pTCPServerStream == NULL)
	{
		if(pSession) m_pTCPServerStream=pSession;
		if(m_pTCPServerStream) 
		{
			OnBFCPTCPConnected(e_BFCP_TCP_Server);
		}
	}
}

//void BFCPSockTCPServer::OnNETEC_TCPServerStreamCallbackDisconnected(void)
//{
//}

bool BFCPSockTCPServer::Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)
{
	bool bResult=false;
	if(m_pTCPServer==NULL)
	{
		m_pTCPServer=new CSocketControl(*this);
		if(m_pTCPServer) 
		{
			m_pTCPServer->SetTCPKeepAliveEnable(true,2000,1000);
			if(m_pTCPServer->Open(nLocalPort,(char *)cszLocalIP, 1,nRemotePort,(char *)cszRemoteIP))
			{
				bResult=true;
			}
			else
			{
				m_pTCPServer->Close();
				delete m_pTCPServer;
				m_pTCPServer=NULL;
			}
		}
	}
	return bResult;
}

void BFCPSockTCPServer::Close(void)
{
	CSocketControl * pTCPServer=m_pTCPServer;
	m_pTCPServer=NULL;
	if(pTCPServer)
	{
		pTCPServer->Close();
		delete pTCPServer;
		pTCPServer=NULL;
	}
}

int BFCPSockTCPServer::SendData(const char*pData,int nLen)
{
	int nRet=0;
	if(m_pTCPServerStream)
	{
		nRet=m_pTCPServerStream->SendData((unsigned char * )pData,nLen);
	}
	return nRet;
}
