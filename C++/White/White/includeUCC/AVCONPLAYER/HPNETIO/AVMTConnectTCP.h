// AVMTConnectTCP.h: interface for the AVMTConnectTCP class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __AVMTCONNECTTCP_H__
#define __AVMTCONNECTTCP_H__

#include "AVMTSocketTCP.h"
#include "AVThread.h"
#include <string>


#define XHTTP_TUNNEL_REQUEST	("CONNECT 127.0.0.1:10544 HTTP/1.1\r\n\r\n")
#define XHTTP_TUNNEL_RESPONCE	("HTTP/1.0 200 connection established\r\n\r\n")

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectTCP;

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectTCPNotify
{
public:
	AVMTConnectTCPNotify(void){};
	virtual~AVMTConnectTCPNotify(void){};

	virtual void OnAVMTConnectTCPNotifyConnected(AVMTConnectTCP* pAVMTConnectTCP,SOCKET hSocket)=0;
	virtual void OnAVMTConnectTCPNotifyFailed(AVMTConnectTCP* pAVMTConnectTCP)=0;
};

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectTCP : public AVThread
{
public:
	AVMTConnectTCP(AVMTConnectTCPNotify& rNotify);
	virtual~AVMTConnectTCP(void);

public:
	virtual bool Connect(const std::string& serveraddr,unsigned short serverport,const std::string& interfaceaddr);
	virtual void ReleaseConnections(void);
	virtual unsigned long GetIP(void);
	virtual unsigned short GetPort(void);

protected:
	AVMTConnectTCPNotify&	m_rNotify;
	AVMTSocketTCP			m_AVMTSocketTCP;
	std::string				m_serveraddr;
	unsigned short			m_serverport;
	std::string				m_interfaceaddr;

	bool					m_bWantToStop;

	virtual void ThreadProcMain(void);
};

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectHTTPProxy : public AVMTConnectTCP
{
public:
	AVMTConnectHTTPProxy(AVMTConnectTCPNotify& rNotify,const std::string& proxyhost,unsigned short proxyport,const std::string& proxyuser,const std::string& proxypassword);
	virtual~AVMTConnectHTTPProxy(void);
	virtual bool Connect(const std::string& serveraddr,unsigned short serverport,const std::string& interfaceaddr);

protected:
	std::string			m_proxyhost;
	unsigned short		m_proxyport;
	std::string			m_proxyuser;
	std::string			m_proxypassword;

	virtual void ThreadProcMain(void);
};

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectSOCKS5 : public AVMTConnectTCP
{
public:
	AVMTConnectSOCKS5(AVMTConnectTCPNotify& rNotify,const std::string& proxyhost,unsigned short proxyport,const std::string& proxyuser,const std::string& proxypassword);
	virtual~AVMTConnectSOCKS5(void);

protected:
	std::string			m_proxyhost;
	unsigned short		m_proxyport;
	std::string			m_proxyuser;
	std::string			m_proxypassword;

	virtual void ThreadProcMain(void);
};

//---------------------------------------------------------------------------------------
class AFX_EXT_CLASS AVMTConnectHTTPTunnel: public AVMTConnectTCP
{
public:
	AVMTConnectHTTPTunnel(AVMTConnectTCPNotify& rNotify);
	virtual~AVMTConnectHTTPTunnel(void);

protected:
	virtual void ThreadProcMain(void);
};
#endif 
