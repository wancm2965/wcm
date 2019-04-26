
/************************************************************************/
//SIPBFCP.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP Interface
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_H__
#define __SIP_BFCP_H__

// 1 s-only floor control server
// 2 c-only floor control client
// 3 c-s    both floor control client and server
enum EBFCPFloorControlType{
	e_Floor_Control_None=0,
	e_Floor_Control_Server,
	e_Floor_Control_Client,
	e_Floor_Control_All,
};

//1 setup:passive
//2 setup:actpass
//3 setup:active
enum EBFCPSetupType{
	e_BFCP_Setup_None=0,
	e_BFCP_Setup_Passive,
	e_BFCP_Setup_Actpass,
	e_BFCP_Setup_Active,
};

//1 udp 
//2 tcp
//3 udp and tcp
enum EBFCPTransportType{
	e_BFCP_Transport_None=0,
	e_BFCP_Transport_UDP,
	e_BFCP_Transport_TCP,
	e_BFCP_Transport_UDP_TCP,
};


enum EBFCPTCPCONType{
	e_BFCP_TCP_None=0,
	e_BFCP_TCP_Client,
	e_BFCP_TCP_Server,
};


/************************************************************************/
//SIP_BFCPCallback
//Description:接收BFCP消息解析后回调接口
/************************************************************************/
class SIP_BFCPCallback
{
public:
	SIP_BFCPCallback(void){};
	virtual~SIP_BFCPCallback(void){};
public:
	virtual void OnSIPBFCPTCPConnected(EBFCPTCPCONType nType)=0;
	virtual void OnSIPBFCPCallbackFloorRequest(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorId)=0;
	virtual void OnSIPBFCPCallbackFloorRequestStatus(unsigned short nFloorReqId, unsigned short nReqStatus)=0;
	virtual void OnSIPBFCPCallbackFloorRelease(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorReqId)=0;
	virtual void OnSIPBFCPCallbackHello()=0;

};

/************************************************************************/
//SIP_BFCP
//Description:BFCP消息发送调用接口
/************************************************************************/

class SIP_BFCP
{
public:
	SIP_BFCP(void){};
	virtual~SIP_BFCP(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort)=0;
	virtual void Close(void)=0;
	virtual int SendBFCPFloorRequest(void)=0;
	virtual int SendBFCPFloorRequestStatus(unsigned short nTransactionId,unsigned short nUserId,unsigned short nFloorId,unsigned short nStatus)=0;
	virtual int SendBFCPFloorRelease(void)=0;
	virtual int SendBFCPHelloACK()=0; //hello ack
	virtual int SendBFCPHello()=0; //hello

	static SIP_BFCP* CreateBFCPUDP(SIP_BFCPCallback& rCallback);
	static SIP_BFCP* CreateBFCPTCPClient(SIP_BFCPCallback& rCallback);
	static SIP_BFCP* CreateBFCPTCPServer(SIP_BFCPCallback& rCallback);
	static SIP_BFCP* CreateBFCPPJSIPUDP(SIP_BFCPCallback& rCallback);

	static void SIP_BFCPStart();
	static void SIP_BFCPStop();

public:	
	//set bfcp local info
	virtual void SetBFCPUserId(int userId)=0;
	virtual void SetBFCPConfId(int confId)=0;
	virtual void SetBFCPFloorId(int floorId)=0;
	virtual void SetBFCPRequestPriority(unsigned short priority)=0;
	virtual void SetBFCPSetupType(EBFCPSetupType type)=0;
	virtual void SetBFCPFloorControlType(EBFCPFloorControlType type)=0;
	virtual void SetBFCPTransportType(EBFCPTransportType type)=0;

};


#endif