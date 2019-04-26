// AVMTConnectTCPGroup.h: interface for the AVMTConnectTCPGroup class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTCONNECTTCPGROUP_H__
#define __AVMTCONNECTTCPGROUP_H__

#include "HPBC.h"
#include "AVMTConnectTCP.h"
//#include "AVUtil.h"
//#include "AVAutoLock.h"
//#include "ListPtr.h"


//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectTCPGroupNotify
{
public:
	AVMTConnectTCPGroupNotify(void){};
	virtual~AVMTConnectTCPGroupNotify(void){};
public:
	virtual void OnAVMTConnectTCPGroupNotifyConnected(SOCKET hSocket,unsigned long ulIP,unsigned short usPort)=0;
	virtual void OnAVMTConnectTCPGroupNotifyFailed(void)=0;
};

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectTCPGroup : public AVMTConnectTCPNotify
                                        , public AVThread
{
public:
	AVMTConnectTCPGroup(AVMTConnectTCPGroupNotify& rNotify);
	virtual ~AVMTConnectTCPGroup(void);

public:
	bool Connect(void);
	void ReleasConnections(void);
	void AddConnect(const std::string& serveraddr,unsigned short serverport,const std::string& interfaceaddr="");
	void AddConnectHTTPTunnel(const std::string& serveraddr,unsigned short serverport,const std::string& interfaceaddr="");
	void AddConnectHTTPProxy(const std::string& serveraddr,unsigned short serverport,const std::string& proxyhost,unsigned short proxyport=80,const std::string& proxyuser="",const std::string& proxypassword="");
	void AddConnectSOCKS5(const std::string& serveraddr,unsigned short serverport,const std::string& proxyhost,unsigned short proxyport,const std::string& proxyuser="",const std::string& proxypassword="");

	void AddConnect(unsigned long serverip,unsigned short serverport,unsigned long interfaceaddr=0);
	void AddConnectHTTPTunnel(unsigned long serverip,unsigned short serverport,unsigned long interfaceaddr=0);
	void AddConnectHTTPProxy(unsigned long serverip,unsigned short serverport,const std::string& proxyhost,unsigned short proxyport=80,const std::string& proxyuser="",const std::string& proxypassword="");
	void AddConnectSOCKS5(unsigned long serverip,unsigned short serverport,const std::string& proxyhost,unsigned short proxyport,const std::string& proxyuser="",const std::string& proxypassword="");

	void DoCheck(void);
protected:
	void ThreadProcMain(void);
	void OnAVMTConnectTCPNotifyConnected(AVMTConnectTCP* pAVMTConnectTCP,SOCKET hSocket);
	void OnAVMTConnectTCPNotifyFailed(AVMTConnectTCP* pAVMTConnectTCP);
	void ClearConnecting(void);
	void ClearDone(void);

protected:
	AVMTConnectTCPGroupNotify&	m_rNotify;
	bool						m_bWantToStop;
	bool						m_bInitialized;
	AVCritSec					m_AVCritSecListAVMTConnectTCP;
	ListPtr						m_ListAVMTConnectTCPConnecting;
	ListPtr						m_ListAVMTConnectTCPDone;
	bool						m_bConnected;
};

#endif 
