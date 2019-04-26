#include "stdafx.h"
#include "ScreenTX.h"
#include "vncService.h"
#include <NETEC/XUtil.h>
#include <NETEC/NETEC_Node.h>

extern BOOL HooksInitInstance();
extern BOOL HooksExitInstance();

static ScreenTX*g_pScreenTX=NULL;

IMXEC_ScreenTX*IMXEC_ScreenTX::Create(IMXEC_ScreenTXCallback&rCallback)
{
	if (g_pScreenTX!=NULL)
	{
		return NULL;
	}

	return new ScreenTX(rCallback);
}

ScreenTX::ScreenTX(IMXEC_ScreenTXCallback&rCallback)
:m_rCallback(rCallback)
,m_usListeningPort(0)
,m_nPinID(XGetTimestamp()+(unsigned long)this)
,m_strPeerNodeID("")

,m_pPin(NULL)
,m_pStream(NULL)

,m_nRemoteViewOnly(0)
{
	m_bConnected=false;
	m_bDisconnected=false;

	g_pScreenTX=this;

	m_bInitHooksOK=HooksInitInstance();
}

ScreenTX::~ScreenTX(void)
{
	g_pScreenTX=NULL;

	HooksExitInstance();
}

int ScreenTX::Start(const char*cszPeerNodeID)
{
	vncService::SimulateCtrlAltDel();

	if (!m_bInitHooksOK)
		return -1;

	if (cszPeerNodeID==NULL || strlen(cszPeerNodeID)==0)
	{
		return -1;
	}

	m_strPeerNodeID=cszPeerNodeID;

	UINT nPort=0;
	if (!StartListen(nPort))
	{
		return -1;
	}

	m_usListeningPort=nPort;

	m_pPin=NETEC_Pin::Create(*this);
	if (m_pPin==NULL)
	{
		return -1;
	}
	if (0!=m_pPin->Open(m_strPeerNodeID.c_str(),m_nPinID))
	{
		m_pPin->Close();
		delete m_pPin;
		m_pPin=NULL;
		return -1;
	}

	//char szDebug[1024]="";
	//sprintf(szDebug,"LocalNodeID=%s PeerNodeID=%s PinID=%u\n",NETEC_Node::GetNodeID(),m_strPeerNodeID.c_str(),m_nPinID);
	//OutputDebugString(szDebug);

	return 0;
}

void ScreenTX::Stop(void)
{
	//OutputDebugString("ScreenTX::Stop\n");
	// Remove any active clients!
	KillAuthClients();
	KillUnauthClients();

	// Wait for all the clients to die
	WaitUntilAuthEmpty();
	WaitUntilUnauthEmpty();

	if (m_pStream!=NULL)
	{
		m_pStream->Close();
		delete m_pStream;
		m_pStream=NULL;
	}

	if (m_pPin!=NULL)
	{
		m_pPin->Close();
		delete m_pPin;
		m_pPin=NULL;
	}

	StopListen();
}

unsigned long ScreenTX::GetScreenID(void)
{
	return m_nPinID;
}

void ScreenTX::SetRemoteViewOnly(int nViewOnly)
{
	m_nRemoteViewOnly=nViewOnly;
	if (m_nRemoteViewOnly)
	{
		m_enable_remote_inputs=FALSE;
		vncServer::SetRemoteViewOnly(TRUE);
	}
	else
	{
		m_enable_remote_inputs=TRUE;
		vncServer::SetRemoteViewOnly(FALSE);
	}
}

int ScreenTX::GetRemoteViewOnly(void)
{
	return m_nRemoteViewOnly;
}


void ScreenTX::OnNETEC_PinCallbackRecvdData(const char*pData,int nLen)
{
	//OutputDebugString("ScreenTX::OnNETEC_PinCallbackRecvdData nLen=%d\n",nLen);
	if (m_pStream!=NULL)
	{
		m_pStream->SendData(pData,nLen);
	}
}

void ScreenTX::OnNETEC_PinCallbackConnected(void)
{
	//OutputDebugString("ScreenTX::OnNETEC_PinCallbackConnected\n");
	m_bConnected=true;
	m_bDisconnected=false;
	if (m_pStream!=NULL)
	{
		m_pStream->Close();
		delete m_pStream;
		m_pStream=NULL;
	}

	m_pStream=NETEC_Stream::Create(*this);
	if (m_pStream!=NULL)
	{
		if (0!=m_pStream->Open("127.0.0.1",m_usListeningPort))
		{
			m_pStream->Close();
			delete m_pStream;
			m_pStream=NULL;
		}
	}

	m_rCallback.OnIMXEC_ScreenTXCallbackConnected();
}

void ScreenTX::OnNETEC_PinCallbackDisconnected(void)
{
	//OutputDebugString("ScreenTX::OnNETEC_PinCallbackDisconnected\n");
	m_bConnected=false;
	m_bDisconnected=true;
	if (m_pStream!=NULL)
	{
		m_pStream->Close();
		delete m_pStream;
		m_pStream=NULL;
	}

	m_rCallback.OnIMXEC_ScreenTXCallbackDisconnected();
}

void ScreenTX::OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen)
{
	//OutputDebugString("ScreenTX::OnNETEC_StreamCallbackRecvdData nLen=%d\n",nLen);

	if (m_pPin!=NULL)
	{
		m_pPin->SendData(pData,nLen);
	}
}

void ScreenTX::OnNETEC_StreamCallbackConnected(void)
{
	//OutputDebugString("ScreenTX::OnNETEC_StreamCallbackConnected\n");
}

void ScreenTX::OnNETEC_StreamCallbackDisconnected(void)
{
	//OutputDebugString("ScreenTX::OnNETEC_StreamCallbackConnected\n");
}

