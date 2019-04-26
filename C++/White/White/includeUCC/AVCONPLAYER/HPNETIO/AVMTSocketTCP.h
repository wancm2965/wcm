// AVMTSocketTCP.h: interface for the AVMTSocketTCP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTSOCKETTCP_H__
#define __AVMTSOCKETTCP_H__

#include "AVMTSocket.h"

class AFX_EXT_CLASS AVMTSocketTCP 
	: public AVMTSocket  
{
public:
	AVMTSocketTCP(void);
	virtual ~AVMTSocketTCP(void);
public:
	bool Bind(unsigned long ulIP,unsigned short usPort);
	bool Connect(unsigned long ulIP,unsigned short usPort,unsigned long ulInterface);
	int Receive(char*pBuffer,int nLen);
	int Send(const char*pData,int nLen);
	SOCKET Accept(void);
};

#endif 
