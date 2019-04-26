//NETEC_App.h
#ifndef __NETEC_APP_H__
#define __NETEC_APP_H__


#include <NETEC/NETEC_Export.h>

#include <NETEC/NETEC_TCPIO.h>
#include <NETEC/NETEC_UDPIO.h>

class NETEC_API NETEC_App
{
public:
	NETEC_App(uint32_t ulAppType);
	virtual~NETEC_App(void);
public:
	virtual int32_t Open(void);
	virtual void Close(void);

	virtual uint32_t GetAppType(void);

	virtual int32_t OnNETEC_TCPIONotifyConnected(uint32_t ulType,const void*pData,int32_t nLen,SOCKET hSocket,const char*cszLocalIP,const char*cszPeerIP);
	virtual int32_t OnNETEC_UDPIONotifyRecvdPacket(XPacketUDP*pXPacketUDP);
protected:
	uint32_t	m_ulAppType;
};


#endif