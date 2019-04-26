//ScreenTX.h
#ifndef __SCREENTX_H__
#define __SCREENTX_H__

#include <string>
#include <IMXEC/IMXEC_Screen.h>
#include <NETEC/NETEC_Pin.h>
#include <NETEC/NETEC_Stream.h>

#include "vncServer.h"

class ScreenTX
	: public vncServer
	, public IMXEC_ScreenTX
	, public NETEC_PinCallback
	, public NETEC_StreamCallback
{
public:
	ScreenTX(IMXEC_ScreenTXCallback&rCallback);
	virtual~ScreenTX(void);
public:
	virtual int Start(const char*cszPeerNodeID);
	virtual void Stop(void);
	virtual unsigned long GetScreenID(void);
	virtual void SetRemoteViewOnly(int nViewOnly);
	virtual int GetRemoteViewOnly(void);
protected:
	virtual void OnNETEC_PinCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_PinCallbackConnected(void);
	virtual void OnNETEC_PinCallbackDisconnected(void);

	virtual void OnNETEC_StreamCallbackRecvdData(const char*pData,int nLen);
	virtual void OnNETEC_StreamCallbackConnected(void);
	virtual void OnNETEC_StreamCallbackDisconnected(void);
protected:
	IMXEC_ScreenTXCallback&	m_rCallback;
	bool						m_bInitHooksOK;
	unsigned short				m_usListeningPort;
	int							m_nRemoteViewOnly;

	unsigned long				m_nPinID;
	std::string					m_strPeerNodeID;

	bool						m_bConnected;
	bool						m_bDisconnected;
	NETEC_Pin*					m_pPin;
	NETEC_Stream*				m_pStream;
};

#endif
