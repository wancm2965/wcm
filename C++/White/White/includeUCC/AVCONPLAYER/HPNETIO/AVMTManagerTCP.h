// AVMTManagerTCP.h: interface for the AVMTManagerTCP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTMANAGERTCP_H__
#define __AVMTMANAGERTCP_H__

#include "libHPNETIO.h"
#include "AVMTSocketServer.h"
#include "AVMTTCPListener.h"
#include "AVAutoLock.h"

#include <map>

typedef std::map<void*,AVMTSocketServer*>		MapAVMTSocketServer;


class AVMTManagerTCP  
	: public AVMTTCPListenerNotify
	, public AVMTSocketServerNotify
	, public IHPTCPIO
{
public:
	AVMTManagerTCP(IHPTCPIONotify&rIHPTCPIONotify);
	virtual ~AVMTManagerTCP(void);
public:
	bool Connect(unsigned short usPort);
	void ReleaseConnections(void);
	unsigned short GetTCPPort(void);
	void DoCheck(void);
	void ClearAVMTSocketServerDone(void);
protected:
	void OnAVMTTCPListenerNotifyAccpted(SOCKET hSocket);
	bool OnAVMTSocketServerNotifyConnected(AVMTSocketServer*pAVMTSocketServer,SOCKET hSocket,unsigned long ulType,const void*pData,int nLen);
	void OnAVMTSocketServerNotifyFailed(AVMTSocketServer*pAVMTSocketServer);
	void ClearAVMTSocketServerConnnecting(void);
protected:
	IHPTCPIONotify&			m_rIHPTCPIONotify;
	AVMTTCPListener*		m_pAVMTTCPListener;
	AVCritSec				m_AVCritSecMapAVMTSocketServer;
	MapAVMTSocketServer		m_MapAVMTSocketServerConnnecting;
	MapAVMTSocketServer		m_MapAVMTSocketServerDone;
};

#endif 
