//XSocketUDP

#ifndef __XSOCKETUDP_H__
#define __XSOCKETUDP_H__

#include "XSocket.h"

class XSocketUDP
	: public XSocket  
{
public:
	XSocketUDP(void);
	virtual~XSocketUDP();
public:
	virtual int Open(const char*cszLocalIP,unsigned short nLocalPort,bool bReuseAddr);
	virtual int Connect(const char*cszHost,unsigned short usPort);
	virtual void Close(void);
private:
	void SetOpt();
};

#endif
