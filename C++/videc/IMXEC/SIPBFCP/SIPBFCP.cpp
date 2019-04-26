
#include "SIPBFCP.h"
#include "BFCPUdp.h"
#include "BFCPTcpClient.h"
#include "BFCPTcpServer.h"
#include "BFCPPJSIPUdp.h"

#include "BFCPSockUDP.h"
#include "BFCPSockTCPServer.h"
#include "BFCPSockTCPClient.h"
//#include "IPCSockTCPServer.h"

#include <NETEC/NETEC_Core.h>
#include <re/re.h>
#include <re/bfcp.h>


void SIP_BFCP::SIP_BFCPStart()
{
	//NETEC_Core::Start(0);
	//baresip bfcp
	rand_init();
	mod_init();
}

void SIP_BFCP::SIP_BFCPStop()
{
	//NETEC_Core::Stop();
	//baresip bfcp
	mod_close();
}

SIP_BFCP* SIP_BFCP::CreateBFCPUDP(SIP_BFCPCallback& rCallback)
{
	SIP_BFCP * pBFCPUDP=NULL;
	//pBFCPUDP=new BFCP_UDP(rCallback);
	pBFCPUDP=new BFCPSockUDP(rCallback);
	return pBFCPUDP;
}

SIP_BFCP* SIP_BFCP::CreateBFCPTCPClient(SIP_BFCPCallback& rCallback)
{
	SIP_BFCP * pBFCPTCPClient=NULL;
	//pBFCPTCPClient=new BFCP_TCPClient(rCallback);
	pBFCPTCPClient=new BFCPSockTCPClient(rCallback);
	return pBFCPTCPClient;

}

SIP_BFCP* SIP_BFCP::CreateBFCPTCPServer(SIP_BFCPCallback& rCallback)
{
	SIP_BFCP * pBFCPTCPServer=NULL;
	//pBFCPTCPServer=new BFCP_TCPServer(rCallback);
	pBFCPTCPServer=new BFCPSockTCPServer(rCallback);
	//pBFCPTCPServer=new IPCSockTCPServer(rCallback);

	return pBFCPTCPServer;

}

SIP_BFCP* SIP_BFCP::CreateBFCPPJSIPUDP(SIP_BFCPCallback& rCallback)
{
	//notice use need change code , pjsip 2.0 version not use this
	SIP_BFCP * pBFCPPJSIPUDP=NULL;
	//pBFCPPJSIPUDP=new BFCP_PJSIP_UDP(rCallback);
	return pBFCPPJSIPUDP;

}