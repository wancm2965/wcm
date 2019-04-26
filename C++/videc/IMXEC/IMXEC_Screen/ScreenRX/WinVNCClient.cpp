#include "stdafx.h"
#include "WinVNCClient.h"
#include <NETEC/NETEC_Node.h>

static WinVNCClient*g_pWinVNCClient=NULL;

IMXEC_ScreenRX*IMXEC_ScreenRX::Create(IMXEC_ScreenRXCallback&rCallback)
{
	if (g_pWinVNCClient!=NULL)
	{
		return NULL;
	}
	return new WinVNCClient(rCallback);
}

WinVNCClient::WinVNCClient(IMXEC_ScreenRXCallback&rCallback)
:m_rCallback(rCallback)
,m_usListeningPort(0)
,m_pClientConnection(NULL)
,m_pVNCClientWnd(NULL)
,m_pPin(NULL)
,m_pStream(NULL)

,m_nPinID(0)
,m_strPeerNodeID("")

,m_strPeerMCUID("")
,m_strPeerMCUIP("")
,m_strPeerMCUType("")
,m_nPeerMCUPort(0)

,m_bWantToStop(false)

,m_strUserName("")
{
	g_pWinVNCClient=this;
}

WinVNCClient::~WinVNCClient(void)
{
	g_pWinVNCClient=NULL;
}

int WinVNCClient::Start(const char*cszPeerUserName,const char*cszPeerNodeID,unsigned long nScreenID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,const char*cszPeerMCUType)
{
	try
	{
		if (cszPeerNodeID==NULL || strlen(cszPeerNodeID)==0 || nScreenID==0)
		{
			return -1;
		}

		if (cszPeerUserName!=NULL)
		{
			m_strUserName=cszPeerUserName;
		}

		m_nPinID=nScreenID;
		m_strPeerNodeID=cszPeerNodeID;

		if (cszPeerMCUID==NULL)
		{
			m_strPeerMCUID="";
		}
		else
		{
			m_strPeerMCUID=cszPeerMCUID;
		}
		if (cszPeerMCUIP==NULL)
		{
			m_strPeerMCUIP="";
		}
		else
		{
			m_strPeerMCUIP=cszPeerMCUIP;
		}
		if (cszPeerMCUType==NULL)
		{
			m_strPeerMCUType="";
		}
		else
		{
			m_strPeerMCUType=cszPeerMCUType;
		}
		m_nPeerMCUPort=nPeerMCUPort;

		m_pVNCClientWnd=new VNCClientWnd(*this,m_usListeningPort);
		if (m_pVNCClientWnd==NULL)
		{
			return -1;
		}

		m_pPin=NETEC_Pin::Create(*this);
		if (m_pPin==NULL)
		{
			return -1;
		}
		if (0!=m_pPin->Open(m_strPeerNodeID.c_str(),m_nPinID,m_strPeerMCUID.c_str(),m_strPeerMCUIP.c_str(),m_nPeerMCUPort,m_strPeerMCUType.c_str()))
		{
			m_pPin->Close();
			delete m_pPin;
			m_pPin=NULL;
			return -1;
		}

		//char szDebug[1024]="";
		//sprintf(szDebug,"LocalNodeID=%s PeerNodeID=%s PinID=%u PeerMCUID=%s PeerMCUIP=%s PeerMCUPort=%u PeerMCUType=%s\n",NETEC_Node::GetNodeID(),m_strPeerNodeID.c_str(),m_nPinID,m_strPeerMCUID.c_str(),m_strPeerMCUIP.c_str(),m_nPeerMCUPort,m_strPeerMCUType.c_str());
		//OutputDebugString(szDebug);
	}
	catch(...)
	{
		return -1;
	}
	return 0;
}

void WinVNCClient::Stop(void)
{
	//OutputDebugString("WinVNCClient::Stop\n");

	m_bWantToStop=true;

	if (m_pClientConnection!=NULL)
	{
		m_pClientConnection->CloseWindow();
		while (m_pClientConnection!=NULL)
		{
			MSG msg;
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				Sleep(1);
			}
		}
	}

	if (m_pVNCClientWnd!=NULL)
	{
		delete m_pVNCClientWnd;
		m_pVNCClientWnd=NULL;
	}

	NETEC_Stream*pStream=NULL;
	{
		XAutoLock l(m_csStream);
		pStream=m_pStream;
		m_pStream=NULL;
	}
	if (pStream!=NULL)
	{
		pStream->Close();
		delete pStream;
		pStream=NULL;
	}

	if (m_pPin!=NULL)
	{
		m_pPin->Close();
		delete m_pPin;
		m_pPin=NULL;
	}

}

int WinVNCClient::SetFullScreen(int nFullScreen)
{
	try
	{
		if (m_pClientConnection!=NULL)
		{
			return m_pClientConnection->SetFullScreen(nFullScreen);
		}
	}
	catch(...)
	{
	}
	return -1;
}

int WinVNCClient::SendCtrlEsc(void)
{
	try
	{
		if (m_pClientConnection!=NULL)
		{
			return m_pClientConnection->SendCtrlEsc();
		}
	}
	catch(...)
	{
	}
	return -1;
}

int WinVNCClient::SendCtrlAltDel(void)
{
	try
	{
		if (m_pClientConnection!=NULL)
		{
			return m_pClientConnection->SendCtrlAltDel();
		}
	}
	catch(...)
	{
	}
	return -1;
}

int WinVNCClient::SetUse8Bit(int nUse8Bit)
{
	try
	{
		if (m_pClientConnection!=NULL)
		{
			return m_pClientConnection->SetUse8Bit(nUse8Bit);
		}
	}
	catch(...)
	{
	}
	return -1;

}


IMXEC_API int TranslateScreenRXMsg(MSG *pMsg)
{
	if (g_pWinVNCClient==NULL)
		return 0;

	return g_HotKeys.TranslateAccel(pMsg);
}


void WinVNCClient::OnSocketAccepted(SOCKET hSocket)
{
	if (m_pClientConnection!=NULL)
	{
		closesocket(hSocket);
		hSocket = 0;
	}
	else
	{
		m_pClientConnection=new ClientConnection(*this,hSocket);

		try
		{
			m_pClientConnection->Run();
		}
		catch (...)
		{
			m_pClientConnection->KillThread();
			m_pClientConnection->DestroyWindow();
			m_pClientConnection=NULL;
		}
	}
}


void WinVNCClient::OnClientConnectionReleased(void)
{
	m_pClientConnection=NULL;
	if (!m_bWantToStop)
	{
		m_rCallback.OnIMXEC_ScreenRXCallbackDisconnected();
	}
}

const std::string&WinVNCClient::GetPeerUserName(void)
{
	return m_strUserName;
}

void WinVNCClient::OnNETEC_PinCallbackRecvdData(const char*pData,int nLen)
{
	//OutputDebugString("WinVNCClient::OnNETEC_PinCallbackRecvdData nLen=%d\n",nLen);
	XAutoLock l(m_csStream);
	if (m_pStream!=NULL)
	{
		m_pStream->SendData(pData,nLen);
	}
}

void WinVNCClient::OnNETEC_PinCallbackConnected(void)
{
	//OutputDebugString("WinVNCClient::OnNETEC_PinCallbackConnected\n");
	//m_bConnected=true;
	//m_bDisconnected=false;
	NETEC_Stream*pStream=NULL;
	{
		XAutoLock l(m_csStream);
		pStream=m_pStream;
		m_pStream=NULL;
	}
	if (pStream!=NULL)
	{
		pStream->Close();
		delete pStream;
		pStream=NULL;
	}

	pStream=NETEC_Stream::Create(*this);
	if (pStream!=NULL)
	{
		if (0!=pStream->Open("127.0.0.1",m_usListeningPort))
		{
			pStream->Close();
			delete pStream;
			pStream=NULL;
		}
	}

	{
		XAutoLock l(m_csStream);
		m_pStream=pStream;
	}

	m_rCallback.OnIMXEC_ScreenRXCallbackConnected();
}

void WinVNCClient::OnNETEC_PinCallbackDisconnected(void)
{
	//OutputDebugString("WinVNCClient::OnNETEC_PinCallbackDisconnected\n");
	//m_bConnected=false;
	//m_bDisconnected=true;
	NETEC_Stream*pStream=NULL;
	{
		XAutoLock l(m_csStream);
		pStream=m_pStream;
		m_pStream=NULL;
	}
	if (pStream!=NULL)
	{
		pStream->Close();
		delete pStream;
		pStream=NULL;
	}
}

void WinVNCClient::OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen)
{
	//OutputDebugString("WinVNCClient::OnNETEC_StreamCallbackRecvdData nLen=%d\n",nLen);
	if (m_pPin!=NULL)
	{
		m_pPin->SendData(pData,nLen);
	}
}

void WinVNCClient::OnNETEC_StreamCallbackConnected(void)
{
	//OutputDebugString("WinVNCClient::OnNETEC_StreamCallbackConnected\n");
}

void WinVNCClient::OnNETEC_StreamCallbackDisconnected(void)
{
	//OutputDebugString("WinVNCClient::OnNETEC_StreamCallbackDisconnected\n");
}


