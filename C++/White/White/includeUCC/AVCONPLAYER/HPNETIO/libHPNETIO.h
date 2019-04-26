//libHPNETIO.h
#ifndef __libHPNETIO_H__
#define __libHPNETIO_H__

#include "AVMTSocket.h"
#include "IHPNetIOQueue.h"

class IHPTCPIONotify
{
public:
	IHPTCPIONotify(void){};
	virtual~IHPTCPIONotify(void){};
public:
	virtual bool OnIHPTCPIONotifyConnected(SOCKET hSocket,unsigned long ulType,const void*pData,int nLen)=0;
};

class IHPTCPIO
{
public:
	IHPTCPIO(void){};
	virtual~IHPTCPIO(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual unsigned short GetTCPPort(void)=0;
	virtual void DoCheck(void)=0;
};

AFX_EXT_CLASS IHPTCPIO*CreateIHPTCPIO(IHPTCPIONotify&rIHPTCPIONotify,unsigned short usTCPPort);

class IHPUDPIONotify
{
public:
	IHPUDPIONotify(void){};
	virtual~IHPUDPIONotify(void){};
public:
	virtual void OnIHPUDPIONotifyReceivedData(const char*pData,int nLen,SOCKET hSocket,unsigned long ulIP,unsigned short usPort)=0;
};

class IHPUDPIO
{
public:
	IHPUDPIO(void){};
	virtual~IHPUDPIO(void){};
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool IsLocalIP(unsigned long ulIP)=0;
	virtual SOCKET GetSocketHandle(unsigned long ulIP)=0;
	virtual unsigned short GetUDPPort(void)=0;
};

AFX_EXT_CLASS IHPUDPIO*CreateIHPUDPIO(IHPUDPIONotify&rIHPUDPIONotify,unsigned short usUDPPort,unsigned char nTos=160);

AFX_EXT_CLASS void XNETIOInit(void);
AFX_EXT_CLASS void XNETIOUninit(void);


#endif