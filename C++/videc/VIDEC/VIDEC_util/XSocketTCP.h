// XSocketTCP.h: interface for the XSocketTCP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XSOCKETTCP_H__
#define __XSOCKETTCP_H__

#include "XSocket.h"

class XSocketTCP 
	: public XSocket  
{
public:
	XSocketTCP(void);
	virtual ~XSocketTCP(void);
public:
	bool Bind(unsigned long ulIP,unsigned short usPort);
	bool Connect(unsigned long ulIP,unsigned short usPort,unsigned long ulInterface,bool bNoneBlocking=false);
	int Receive(char*pBuffer,int nLen);
	int Send(const char*pData,int nLen);
	SOCKET Accept(void);
	unsigned long GetPeerIP(void);
};

#endif 
