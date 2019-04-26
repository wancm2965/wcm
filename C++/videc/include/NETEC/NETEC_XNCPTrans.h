//NETEC_XNCPTrans.h
#ifndef __NETEC_XNCPTRANS_H__
#define __NETEC_XNCPTRANS_H__

#include <NETEC/NETEC_Export.h>
#include <NETEC/XSocket.h>
#include <NETEC/NETEC_App.h>

class XNCPTransNotify
{
public:
	XNCPTransNotify(void)
	{
	}
	virtual~XNCPTransNotify(void)
	{
	}
public:
	virtual void OnXNCPTransNotifyReceivedFrame(void*pFrameData,int32_t nFrameLen)=0;
	virtual bool OnXNCPTransNotifyHasPendingData(void)=0;
	virtual void OnXNCPTransNotifyGetPendingData(void)=0;
	virtual bool OnXNCPTransNotifyPreConnected(uint32_t ulType,void*pData,int32_t nLen){return false;};
	virtual void OnXNCPTransNotifyConnected(void){};
};

class NETEC_API XNCPTrans
{
public:
	XNCPTrans(void){}
	virtual~XNCPTrans(void){}
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool IsDisconnected(void)=0;
	virtual bool SendFrame(void*pFrameData,int32_t nFrameLen)=0;
	virtual bool SendPendingData(void*pData,int32_t nLen)=0;
	virtual void ActivateSending(void)=0;
	virtual const char*GetLocalIP(void)=0;
	virtual const char*GetPeerIP(void)=0;
	virtual unsigned short GetLocalPort(void)=0;
	virtual bool IsClient(void)=0;

	static XNCPTrans*Create(XNCPTransNotify&rNotify,SOCKET hSocket,const char*cszLocalIP,const char*cszPeerIP,void*pHeaderData,int32_t nHeaderLen);
	static XNCPTrans*Create(XNCPTransNotify&rNotify,const char*cszIP,unsigned short usPort,const char*cszInterface,uint32_t ulType,void*pData,int32_t nLen,bool bDontUseProxy=false);
};

// 

#endif