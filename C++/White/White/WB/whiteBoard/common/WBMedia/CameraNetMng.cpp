
#include "stdafx.h"
#include <NETEC/NETEC_Core.h>
#include <NETEC/NETEC_Setting.h>
#include "CameraNetMng.h"
#include "CameraData.h"


BOOL CWBCameraNetMng::m_bNetConnected = FALSE;
unsigned short CWBCameraNetMng::m_nCounter = 0;

CWBCameraNetMng::CWBCameraNetMng()
{

}

CWBCameraNetMng::~CWBCameraNetMng()
{

}

void CWBCameraNetMng::OnNETEC_NodeConnectStatusChanged( NETEC_Session::CONNECT_STATUS cs )
{
	if (cs==NETEC_Session::CS_CONNECTED ||
		cs==NETEC_Session::CS_RESTARTED ||
		cs==NETEC_Session::CS_RECONNECTED)
	{
		m_bNetConnected = TRUE;
	}
	else
	{
		//连接断开
		m_bNetConnected = FALSE;
	}
	OnConnectStatusChanged(m_bNetConnected);
}

BOOL CWBCameraNetMng::GetNetState()
{
	return m_bNetConnected;
}

BOOL CWBCameraNetMng::Connect()
{
	BOOL bSuccess = TRUE;
	if (!GetNetState())
	{
		SCameraServerData ServerData;

		int nReturn = NETEC_Core::Start(0);
		if(nReturn)
		{
			OutputDebugString("\nconnect error\n");
		}
		bSuccess = (0 == nReturn);

		NETEC_Node::SetServerIP(ServerData.sServerAddress.c_str());//南昌这边的地址
		NETEC_Node::SetServerPort(ServerData.nServerPort);

		nReturn = NETEC_Node::Start();
		bSuccess = (0 == nReturn);
		if(nReturn)
		{
			OutputDebugString("\nconnect error\n");
		}

		NETEC_Setting::SetAudioProtocolType(NETEC_Setting::PT_UDP);
		NETEC_Setting::SetVideoProtocolType(NETEC_Setting::PT_RTP);


		NETEC_Setting::SetMCUOnly(1);
	}
	m_nCounter++;
	return bSuccess;
}

void CWBCameraNetMng::DisConnect()
{
	m_nCounter--;
	if (0 == m_nCounter)
	{
		NETEC_Node::Stop();
		NETEC_Core::Stop();
	}
}
