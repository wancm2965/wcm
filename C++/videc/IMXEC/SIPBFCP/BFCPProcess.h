
/************************************************************************/
//BFCPProcess.h
//Copyright(c):AVCON
//Author:Gao
//Notice:SIP BFCP Process
//Date:2012-03-05
/************************************************************************/

#ifndef __SIP_BFCP_PROCESS_H__
#define __SIP_BFCP_PROCESS_H__

#include "SIPBFCP.h"
#include <winsock2.h>

#define MaxDataBufLen 1600

/************************************************************************/
//BFCP_Process
//Description:BFCP消息解析及生成处理
/************************************************************************/
class BFCP_Process
	: public SIP_BFCP
{
public:
	BFCP_Process(SIP_BFCPCallback& rCallback);
	virtual~BFCP_Process(void){};
public:
	virtual bool Open(const char*cszLocalIP,unsigned short nLocalPort,const char*cszRemoteIP,unsigned short nRemotePort){return false;};
	virtual void Close(void){};
	virtual int SendBFCPFloorRequest(void);
	virtual int SendBFCPFloorRequestStatus(unsigned short transactionId,unsigned short userId,unsigned short floorId,unsigned short status);
	virtual int SendBFCPFloorRelease(void);
	virtual void OnBFCPTCPConnected(EBFCPTCPCONType nType);
	virtual void OnRcvdData(const char*pData,int nLen);
	virtual int SendData(const char*pData,int nLen)=0;

	virtual int SendBFCPHelloACK();						//hello ack
	virtual int SendBFCPHello();						//hello

protected:
	char m_strReqReason[256];
	unsigned int m_nRequestConfId;						//local bfcp info
	unsigned short m_nRequestUserId;					//local bfcp info
	unsigned short m_nRequestFloorId;					//local bfcp info
	unsigned short m_nRequestPriority;					//local bfcp info
	EBFCPTransportType m_nRequestTransportType;			//local bfcp info
	EBFCPFloorControlType m_nRequestFloorControlType;	//local bfcp info
	EBFCPSetupType m_nRequestSetupType;					//local bfcp info
	unsigned short m_nBfcpRequestId;					//request floor id recv remote floor request status response 
	unsigned short m_nBfcpLocalRequestId;				//local request floor id send floor request status response
	unsigned short int m_nBfcpTransactionID;
	char m_szSurplusBuf[MaxDataBufLen];
	int m_nSurplusBufLen;
protected:
	void ParseBFCPMsg(struct mbuf *mb);
	SIP_BFCPCallback& m_rBFCPCallback;

public:	
	//set bfcp local info
	void SetBFCPUserId(int userId);
	void SetBFCPConfId(int confId);
	void SetBFCPFloorId(int floorId);
	void SetBFCPRequestPriority(unsigned short priority);
	void SetBFCPSetupType(EBFCPSetupType type);
	void SetBFCPFloorControlType(EBFCPFloorControlType type);
	void SetBFCPTransportType(EBFCPTransportType type);


};


#endif