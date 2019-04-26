// AVMTSocketUDP.h: interface for the AVMTSocketUDP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTSOCKETUDP_H__
#define __AVMTSOCKETUDP_H__

#include "AVMTSocket.h"

class AFX_EXT_CLASS AVMTSocketUDP 
	: public AVMTSocket  
{
public:
	AVMTSocketUDP(void);
	virtual ~AVMTSocketUDP(void);
public:
	virtual bool Bind(unsigned long ulIP,unsigned short usPort,bool bReuseAddr=true,bool bNB=true);
	virtual bool SendTo(const char*pData,int nLen,unsigned long ulIP,unsigned short usPort);
	virtual bool SendTo(const char*pData,int nLen,struct sockaddr_in sa);
	virtual int ReceivedFrom(char*pBuffer,int nLen,unsigned long&ulIP,unsigned short&usPort);
	virtual bool PollData(unsigned long ulMS);
	virtual bool ConnectTo(unsigned long ulIP,unsigned short usPort);
	virtual unsigned long GetLocalAddr(void);
};

#endif
