//WinVNCClient.h
#ifndef __WINVNCCLIENT_H__
#define __WINVNCCLIENT_H__

#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Pin.h>
#include <NETEC/NETEC_Stream.h>
#include <NETEC/XAutoLock.h>
#include "ClientConnection.h"
#include "VNCClientWnd.h"

class WinVNCClient
	: public IMXEC_ScreenRX
	, public NETEC_PinCallback
	, public NETEC_StreamCallback
{
public:
	WinVNCClient(IMXEC_ScreenRXCallback&rCallback);
	virtual~WinVNCClient(void);
public:
	virtual int Start(const char*cszPeerUserName,const char*cszPeerNodeID,unsigned long nScreenID,const char*cszPeerMCUID,const char*cszPeerMCUIP,unsigned short nPeerMCUPort,const char*cszPeerMCUType);
	virtual void Stop(void);
	virtual int SetFullScreen(int nFullScreen);
	virtual int SendCtrlEsc(void);
	virtual int SendCtrlAltDel(void);
	virtual int SetUse8Bit(int nUse8Bit);

	virtual void OnSocketAccepted(SOCKET hSocket);
	virtual void OnClientConnectionReleased(void);
	virtual const std::string&GetPeerUserName(void);
protected:
	virtual void OnNETEC_PinCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_PinCallbackConnected(void);
	virtual void OnNETEC_PinCallbackDisconnected(void);

	virtual void OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_StreamCallbackConnected(void);
	virtual void OnNETEC_StreamCallbackDisconnected(void);
protected:
	IMXEC_ScreenRXCallback&	m_rCallback;
	unsigned short				m_usListeningPort;
	ClientConnection*			m_pClientConnection;
	VNCClientWnd*				m_pVNCClientWnd;
	bool						m_bWantToStop;

	NETEC_Pin*					m_pPin;
	XCritSec					m_csStream;
	NETEC_Stream*				m_pStream;

	unsigned long				m_nPinID;
	std::string					m_strPeerNodeID;

	std::string					m_strPeerMCUID;
	std::string					m_strPeerMCUIP;
	std::string					m_strPeerMCUType;
	unsigned short				m_nPeerMCUPort;

	std::string					m_strUserName;
};

#endif
